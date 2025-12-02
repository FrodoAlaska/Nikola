#include "nbr.h"

#include <nikola/nikola.h>
#include <cgltf/cgltf.h>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nbr

/// ----------------------------------------------------------------------
/// NodeAnim

struct NodeAnim {
  nikola::DynamicArray<nikola::VectorAnimSample> positions;
  nikola::DynamicArray<nikola::QuatAnimSample> rotations;
  nikola::DynamicArray<nikola::VectorAnimSample> scales;

  nikola::sizei samples_max = 0;
  nikola::f32 duration      = 0.0f;
};

/// NodeAnim
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// AnimData

struct AnimData {
  nikola::HashMap<nikola::String, NodeAnim> tracks;
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

static void read_positions(NodeAnim* track, cgltf_animation_sampler* sampler) {
  /// @NOTE: 
  ///
  /// I realize that the actions below probably can be pulled out and 
  /// placed into a function so that they are a bit less error-prone, which 
  /// will also make me more "professional". But, listen, I'm tired, okay? 
  ///
  /// Leave me alone... 
  ///

  // Read the input (the time scale)

  nikola::sizei floats_read = cgltf_accessor_unpack_floats(sampler->input, nullptr, sampler->input->count);

  nikola::f32* input_buffer = (nikola::f32*)nikola::memory_allocate(sizeof(nikola::f32) * floats_read);
  cgltf_accessor_unpack_floats(sampler->input, input_buffer, floats_read);

  // Read the position values
  
  floats_read = cgltf_accessor_unpack_floats(sampler->output, nullptr, sampler->output->count);

  nikola::f32* output_buffer = (nikola::f32*)nikola::memory_allocate(sizeof(nikola::f32) * floats_read);
  cgltf_accessor_unpack_floats(sampler->output, output_buffer, floats_read);

  // Add the values into our internal array

  for(nikola::sizei i = 0, j = 0; i < floats_read; i += 3, j++) { // 3 = number of components in a Vec3
    nikola::f32 time = input_buffer[j];
    track->duration  = nikola::max_float(track->duration, sampler->input->max[0]); 

    nikola::f32 x_pos = output_buffer[i + 0];
    nikola::f32 y_pos = output_buffer[i + 1];
    nikola::f32 z_pos = output_buffer[i + 2];

    track->positions.emplace_back(nikola::Vec3(x_pos, y_pos, z_pos), time);
  }
 
  // Freeing up some memory 
  
  nikola::memory_free(input_buffer);
  nikola::memory_free(output_buffer);

  // Update the samples count for later
  track->samples_max = track->positions.size();
}

static void read_rotations(NodeAnim* track, cgltf_animation_sampler* sampler) {
  // Read the input (the time scale)

  nikola::sizei floats_read = cgltf_accessor_unpack_floats(sampler->input, nullptr, sampler->input->count);

  nikola::f32* input_buffer = (nikola::f32*)nikola::memory_allocate(sizeof(nikola::f32) * floats_read);
  cgltf_accessor_unpack_floats(sampler->input, input_buffer, floats_read);

  // Read the rotation values
  
  floats_read = cgltf_accessor_unpack_floats(sampler->output, nullptr, sampler->output->count);

  nikola::f32* output_buffer = (nikola::f32*)nikola::memory_allocate(sizeof(nikola::f32) * floats_read);
  cgltf_accessor_unpack_floats(sampler->output, output_buffer, floats_read);

  // Add the values into our internal array

  for(nikola::sizei i = 0, j = 0; i < floats_read; i += 4, j++) { // 4 = number of components in a Quat
    nikola::f32 time = input_buffer[j];
    track->duration  = nikola::max_float(track->duration, sampler->input->max[0]); 

    nikola::f32 x_pos = output_buffer[i + 0];
    nikola::f32 y_pos = output_buffer[i + 1];
    nikola::f32 z_pos = output_buffer[i + 2];
    nikola::f32 w_pos = output_buffer[i + 3];

    track->rotations.emplace_back(nikola::Quat(w_pos, x_pos, y_pos, z_pos), time);
  }
 
  // Freeing up some memory 
  
  nikola::memory_free(input_buffer);
  nikola::memory_free(output_buffer);

  // Update the samples count for later
 
  if(track->samples_max < track->rotations.size()) {
    track->samples_max = track->rotations.size();
  }
}

static void read_scales(NodeAnim* track, cgltf_animation_sampler* sampler) {
  // Read the input (the time scale)

  nikola::sizei floats_read = cgltf_accessor_unpack_floats(sampler->input, nullptr, sampler->input->count);

  nikola::f32* input_buffer = (nikola::f32*)nikola::memory_allocate(sizeof(nikola::f32) * floats_read);
  cgltf_accessor_unpack_floats(sampler->input, input_buffer, floats_read);

  // Read the scale values
  
  floats_read = cgltf_accessor_unpack_floats(sampler->output, nullptr, sampler->output->count);

  nikola::f32* output_buffer = (nikola::f32*)nikola::memory_allocate(sizeof(nikola::f32) * floats_read);
  cgltf_accessor_unpack_floats(sampler->output, output_buffer, floats_read);

  // Add the values into our internal array

  for(nikola::sizei i = 0, j = 0; i < floats_read; i += 3, j++) { // 4 = number of components in a Vec3
    nikola::f32 time = input_buffer[j];
    track->duration  = nikola::max_float(track->duration, sampler->input->max[0]); 

    nikola::f32 x_pos = output_buffer[i + 0];
    nikola::f32 y_pos = output_buffer[i + 1];
    nikola::f32 z_pos = output_buffer[i + 2];

    track->scales.emplace_back(nikola::Vec3(x_pos, y_pos, z_pos), time);
  }
 
  // Freeing up some memory 
  
  nikola::memory_free(input_buffer);
  nikola::memory_free(output_buffer);
  
  // Update the samples count for later
 
  if(track->samples_max < track->scales.size()) {
    track->samples_max = track->scales.size();
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

    data.tracks[name] = NodeAnim{};
  }

  // Load the animation data

  cgltf_animation* gltf_anim = &gltf->animations[0];
  for(nikola::sizei i = 0; i < gltf_anim->channels_count; i++) {
    cgltf_animation_channel* channel = &gltf_anim->channels[i];
    cgltf_animation_sampler* sampler = &gltf_anim->samplers[cgltf_animation_sampler_index(gltf_anim, channel->sampler)];

    nikola::String node_name = channel->target_node->name;
    if(data.tracks.find(node_name) == data.tracks.end()) { // Can't find the node in the lookup table... probably a dull
      continue;
    }

    switch(channel->target_path) {
      case cgltf_animation_path_type_translation:
        read_positions(&data.tracks[nikola::String(channel->target_node->name)], sampler);
        break;
      case cgltf_animation_path_type_rotation:
        read_rotations(&data.tracks[nikola::String(channel->target_node->name)], sampler);
        break;
      case cgltf_animation_path_type_scale:
        read_scales(&data.tracks[nikola::String(channel->target_node->name)], sampler);
        break;
      default: 
        break;
    }
  }

  // Converting the data accumlated into our stupid NBR format 
  // @NOTE: Yes, I'm traversing a hash map. Get off my back, dude. 

  anim->tracks_count = (nikola::u16)data.tracks.size();
  anim->tracks       = (nikola::NBRAnimation::NBRJointTrack*)nikola::memory_allocate(sizeof(nikola::NBRAnimation::NBRJointTrack) * anim->tracks_count);

  nikola::sizei index = 0;
  for(auto& [name, track] : data.tracks) {
    nikola::NBRAnimation::NBRJointTrack* joint = &anim->tracks[index];

    // Make sure that there is at least _one_ value in each array 
    // for easier navigation.
    
    if(track.positions.empty()) {
      track.positions.emplace_back(nikola::Vec3(0.0f), 0.0f);
    }
    
    if(track.rotations.empty()) {
      track.rotations.emplace_back(nikola::Quat(1.0f, 0.0f, 0.0f, 0.0f), 0.0f);
    }
    
    if(track.scales.empty()) {
      track.scales.emplace_back(nikola::Vec3(1.0f), 0.0f);
    }

    // Convert positions
    
    joint->positions_count  = (nikola::u16)track.positions.size();
    joint->position_samples = (nikola::f32*)nikola::memory_allocate(sizeof(nikola::VectorAnimSample) * joint->positions_count);
    
    for(nikola::sizei i = 0, j = 0; i < track.positions.size(); i++, j += 4) {
      joint->position_samples[j + 0] = track.positions[i].value.x; 
      joint->position_samples[j + 1] = track.positions[i].value.y; 
      joint->position_samples[j + 2] = track.positions[i].value.z; 
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
    joint->scale_samples = (nikola::f32*)nikola::memory_allocate(sizeof(nikola::QuatAnimSample) * joint->scales_count);
    
    for(nikola::sizei i = 0, j = 0; i < track.scales.size(); i++, j += 4) {
      joint->scale_samples[j + 0] = track.scales[i].value.x; 
      joint->scale_samples[j + 1] = track.scales[i].value.y; 
      joint->scale_samples[j + 2] = track.scales[i].value.z; 
      joint->scale_samples[j + 3] = track.scales[i].time;
    }

    index++;
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
