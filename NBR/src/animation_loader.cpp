#include "nbr.h"

#include <nikola/nikola.h>
#include <cgltf/cgltf.h>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nbr

/// ----------------------------------------------------------------------
/// Consts

const nikola::f32 ANIMATION_TIME_SCALE = 100.0f;

/// Consts
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// NodeAnim

struct NodeAnim {
  nikola::DynamicArray<nikola::VectorAnimSample> positions;
  nikola::DynamicArray<nikola::QuatAnimSample> rotations;
  nikola::DynamicArray<nikola::VectorAnimSample> scales;

  nikola::f32 duration = 0.0f;

  nikola::Vec3 local_position;
  nikola::Quat local_rotation;
  nikola::Vec3 local_scale;
};

/// NodeAnim
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// AnimData

struct AnimData {
  nikola::HashMap<nikola::String, nikola::sizei> tracks_table;
  nikola::DynamicArray<NodeAnim> tracks;

  nikola::f32 duration = 0.0f;
};

/// AnimData
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static bool is_valid_extension(const nikola::FilePath& ext) {
  return ext == ".gltf" || 
         ext == ".glb";
}

static void read_sampler(cgltf_animation_sampler* sampler, 
                         nikola::DynamicArray<nikola::f32>& time_buffer, 
                         nikola::DynamicArray<nikola::f32>& values_buffer) {
  // Read the time buffer
  
  nikola::sizei time_floats_read = cgltf_accessor_unpack_floats(sampler->input, nullptr, sampler->input->count);

  time_buffer.resize(time_floats_read);
  cgltf_accessor_unpack_floats(sampler->input, time_buffer.data(), time_floats_read);

  // Read the values buffer
  
  nikola::sizei values_floats_read = cgltf_accessor_unpack_floats(sampler->output, nullptr, sampler->output->count);

  values_buffer.resize(values_floats_read);
  cgltf_accessor_unpack_floats(sampler->output, values_buffer.data(), values_floats_read);
}

static void read_positions(NodeAnim* track, cgltf_animation_sampler* sampler) {
  // Read the input and output accessors and get the corresponding buffers back...
  // It's smart, I know. I'm vewy pwoud of mysewf.
  
  nikola::DynamicArray<nikola::f32> time_buffer;
  nikola::DynamicArray<nikola::f32> values_buffer;

  read_sampler(sampler, time_buffer, values_buffer);

  // Add the values into our internal array

  track->positions.reserve(values_buffer.size());

  for(nikola::sizei i = 0, j = 0; i < values_buffer.size(); i += 3, j++) { // 3 = number of components in a Vec3
    nikola::f32 time = time_buffer[j];
    track->duration  = nikola::max_float(track->duration, sampler->input->max[0]); 

    nikola::Vec3 pos;
    pos.x = values_buffer[i + 0];
    pos.y = values_buffer[i + 1];
    pos.z = values_buffer[i + 2];

    track->positions.emplace_back(pos, time);
  }
}

static void read_rotations(NodeAnim* track, cgltf_animation_sampler* sampler) {
  // Read the input and output accessors and get the corresponding buffers back...
  
  nikola::DynamicArray<nikola::f32> time_buffer;
  nikola::DynamicArray<nikola::f32> values_buffer;

  read_sampler(sampler, time_buffer, values_buffer);

  // Add the values into our internal array
  
  track->rotations.reserve(values_buffer.size());
  
  for(nikola::sizei i = 0, j = 0; i < values_buffer.size(); i += 4, j++) { // 4 = number of components in a Quat
    nikola::f32 time = time_buffer[j];
    track->duration  = nikola::max_float(track->duration, sampler->input->max[0]); 

    nikola::Quat rot;
    rot.x = values_buffer[i + 0];
    rot.y = values_buffer[i + 1];
    rot.z = values_buffer[i + 2];
    rot.w = values_buffer[i + 3];

    track->rotations.emplace_back(rot, time);
  }
}

static void read_scales(NodeAnim* track, cgltf_animation_sampler* sampler) {
  // Read the input and output accessors and get the corresponding buffers back...
  
  nikola::DynamicArray<nikola::f32> time_buffer;
  nikola::DynamicArray<nikola::f32> values_buffer;

  read_sampler(sampler, time_buffer, values_buffer);

  // Add the values into our internal array
  
  track->scales.reserve(values_buffer.size());

  for(nikola::sizei i = 0, j = 0; i < values_buffer.size(); i += 3, j++) { // 3 = number of components in a Vec3
    nikola::f32 time = time_buffer[j];
    track->duration  = nikola::max_float(track->duration, sampler->input->max[0]); 

    nikola::Vec3 scale;
    scale.x = values_buffer[i + 0];
    scale.y = values_buffer[i + 1];
    scale.z = values_buffer[i + 2];

    track->scales.emplace_back(scale, time);
  }
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Animation loader functions

bool animation_loader_load(nikola::NBRAnimation* anim, const nikola::FilePath& path) {
  // Some sanity checks

  nikola::FilePath ext = nikola::filepath_extension(path);

  if(!is_valid_extension(ext)) {
    NIKOLA_LOG_ERROR("Unsupported animation format found at \'%s\'. The supported formats are: GLTF, GLB", path.c_str());
    return false;
  } 

  // Load the GLTF file

  cgltf_data* gltf      = new cgltf_data{};
  cgltf_options options = {}; 

  cgltf_result res = cgltf_parse_file(&options, path.c_str(), &gltf);
  if(res != cgltf_result_success) {
    cgltf_free(gltf);

    NIKOLA_LOG_ERROR("Failed to open GLTF file at \'%s\'", path.c_str());
    return false;
  }

  // Load the buffers

  res = cgltf_load_buffers(&options, gltf, path.c_str());
  if(res != cgltf_result_success) {
    cgltf_free(gltf);

    NIKOLA_LOG_ERROR("Failed to load GLTF buffers at \'%s\'", path.c_str());
    return false;
  }
  
  AnimData data;

  // Build a joint hierarchy first to make sure everything is in order

  cgltf_skin* skin = &gltf->skins[0];
  for(nikola::sizei i = 0; i < skin->joints_count; i++) {
    cgltf_node* joint   = skin->joints[i];
    nikola::String name = joint->name;

    data.tracks_table[name] = i;

    data.tracks.push_back(NodeAnim{
      .local_position = nikola::Vec3(joint->translation[0], joint->translation[1], joint->translation[2]),
      .local_rotation = nikola::Quat(joint->rotation[3], joint->rotation[0], joint->rotation[1], joint->rotation[2]), // w, x, y, z
      .local_scale    = nikola::Vec3(joint->scale[0], joint->scale[1], joint->scale[2]),
    });
  }

  // Load the animation data

  cgltf_animation* gltf_anim = &gltf->animations[0];
  for(nikola::sizei i = 0; i < gltf_anim->channels_count; i++) {
    cgltf_animation_channel* channel = &gltf_anim->channels[i];
    cgltf_animation_sampler* sampler = &gltf_anim->samplers[cgltf_animation_sampler_index(gltf_anim, channel->sampler)];

    nikola::String node_name = channel->target_node->name;
    NodeAnim* node           = &data.tracks[data.tracks_table[node_name]];

    switch(channel->target_path) {
      case cgltf_animation_path_type_translation:
        read_positions(node, sampler);
        break;
      case cgltf_animation_path_type_rotation:
        read_rotations(node, sampler);
        break;
      case cgltf_animation_path_type_scale:
        read_scales(node, sampler);
        break;
      default: 
        break;
    }
  }

  // Convert the name 

  nikola::String anim_name = gltf_anim->name; 

  anim->name_length = (nikola::u8)anim_name.size(); 
  nikola::memory_copy(anim->name, anim_name.c_str(), anim->name_length);

  // Converting the data accumlated into our stupid NBR format 

  anim->tracks_count = (nikola::u16)data.tracks.size();
  anim->tracks       = (nikola::NBRAnimation::NBRJointTrack*)nikola::memory_allocate(sizeof(nikola::NBRAnimation::NBRJointTrack) * anim->tracks_count);

  for(nikola::sizei i = 0; i < data.tracks.size(); i++) {
    NodeAnim& track                            = data.tracks[i];
    nikola::NBRAnimation::NBRJointTrack* joint = &anim->tracks[i];

    // Make sure that there is at least _one_ value in each array 
    // for easier navigation. Usually, it's going to be 
    // the local position, rotation, or scale of the joint.
    
    if(track.positions.empty()) {
      track.positions.emplace_back(track.local_position, 0.0f);
    }

    if(track.rotations.empty()) {
      track.rotations.emplace_back(track.local_rotation, 0.0f);
    }

    if(track.scales.empty()) {
      track.scales.emplace_back(track.local_scale, 0.0f);
    }

    // Convert positions
    
    joint->positions_count  = (nikola::u16)track.positions.size();
    joint->position_samples = (nikola::f32*)nikola::memory_allocate(sizeof(nikola::VectorAnimSample) * joint->positions_count);
    
    for(nikola::sizei i = 0, j = 0; i < track.positions.size(); i++, j += 4) {
      joint->position_samples[j + 0] = track.positions[i].value.x * nikola::NBR_MODEL_IMPORT_SCALE; 
      joint->position_samples[j + 1] = track.positions[i].value.y * nikola::NBR_MODEL_IMPORT_SCALE; 
      joint->position_samples[j + 2] = track.positions[i].value.z * nikola::NBR_MODEL_IMPORT_SCALE; 
      joint->position_samples[j + 3] = track.positions[i].time;
    }

    // Convert rotations
    
    joint->rotations_count  = (nikola::u16)track.rotations.size();
    joint->rotation_samples = (nikola::f32*)nikola::memory_allocate(sizeof(nikola::QuatAnimSample) * joint->rotations_count);
    
    for(nikola::sizei i = 0, j = 0; i < track.rotations.size(); i++, j += 5) {
      joint->rotation_samples[j + 0] = track.rotations[i].value.x; 
      joint->rotation_samples[j + 1] = track.rotations[i].value.y; 
      joint->rotation_samples[j + 2] = track.rotations[i].value.z; 
      joint->rotation_samples[j + 3] = track.rotations[i].value.w; 
      joint->rotation_samples[j + 4] = track.rotations[i].time;
    }
    
    // Convert scales
    
    joint->scales_count  = (nikola::u16)track.scales.size();
    joint->scale_samples = (nikola::f32*)nikola::memory_allocate(sizeof(nikola::VectorAnimSample) * joint->scales_count);
    
    for(nikola::sizei i = 0, j = 0; i < track.scales.size(); i++, j += 4) {
      joint->scale_samples[j + 0] = track.scales[i].value.x; 
      joint->scale_samples[j + 1] = track.scales[i].value.y; 
      joint->scale_samples[j + 2] = track.scales[i].value.z; 
      joint->scale_samples[j + 3] = track.scales[i].time;
    }

    data.duration = nikola::max_float(data.duration, track.duration);
  }

  // Getting the final duration of the animation
  anim->duration = data.duration;

  // Done!

  cgltf_free(gltf);
  return true;
}

void animation_loader_unload(nikola::NBRAnimation& anim) {
  for(nikola::u16 i = 0; i < anim.tracks_count; i++) {
    if(anim.tracks[i].position_samples) {
      nikola::memory_free(anim.tracks[i].position_samples);
    }

    if(anim.tracks[i].rotation_samples) {
      nikola::memory_free(anim.tracks[i].rotation_samples);
    }

    if(anim.tracks[i].scale_samples) {
      nikola::memory_free(anim.tracks[i].scale_samples);
    }
  } 

  nikola::memory_free(anim.tracks);
}

/// Animation loader functions
/// ----------------------------------------------------------------------

} // End of nbr

//////////////////////////////////////////////////////////////////////////
