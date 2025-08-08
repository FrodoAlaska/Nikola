#include "nbr.h"

#include <nikola/nikola.h>

#include <cgltf/cgltf.h>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nbr

/// ----------------------------------------------------------------------
/// JointData
struct JointData {
  JointData* parent = nullptr;

  nikola::sizei index = 0;
  nikola::Mat4 inverse_bind_pose;
};
/// JointData
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// AnimData
struct AnimData {
  cgltf_data* gltf;
  nikola::FilePath parent_dir;

  nikola::HashMap<nikola::String, JointData> joints;
};
/// AnimData
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Animation loader functions

bool gltf_animation_loader_load(nikola::NBRAnimation* anim, const nikola::FilePath& path) {
  AnimData data;
  data.parent_dir = nikola::filepath_parent_path(path);

  // Load the GLTF file

  cgltf_options options;
  nikola::memory_zero(&options, sizeof(cgltf_options)); 

  data.gltf        = (cgltf_data*)nikola::memory_allocate(sizeof(cgltf_data));
  cgltf_result res = cgltf_parse_file(&options, path.c_str(), &data.gltf); 

  if(res != cgltf_result_success) {
    NIKOLA_LOG_ERROR("Could not load GLTF file at \'%s\'", path.c_str());
    return false;
  }

  // Load the GLTF bin file (where all the actual data lives)

  nikola::FilePath bin_path = nikola::filepath_append(data.parent_dir, data.gltf->buffers[0].uri);
  res = cgltf_load_buffers(&options, data.gltf, bin_path.c_str());
  
  if(res != cgltf_result_success) {
    NIKOLA_LOG_ERROR("Failed to load GLTF bin at \'%s\'", path.c_str());
    return false;
  }

  // Check if the GLTF file is valid

  res = cgltf_validate(data.gltf);
  if(res != cgltf_result_success) {
    NIKOLA_LOG_ERROR("Could not validate GLTF file at \'%s\'", path.c_str());
    return false;
  }

  NIKOLA_LOG_TRACE("Animations count = %zu", data.gltf->animations_count);
  cgltf_animation* animation = &data.gltf->animations[0];
  NIKOLA_LOG_TRACE("Animation name = %s", animation->name);
  NIKOLA_LOG_TRACE("  Channels = %zu", animation->channels_count);
  NIKOLA_LOG_TRACE("  Samplers = %zu", animation->samplers_count);

  // Loading all the joints

  cgltf_skin* skin = &data.gltf->skins[0];
  for(nikola::sizei i = 0; i < skin->joints_count; i++) {
    cgltf_node* joint    = skin->joints[i];
    JointData joint_data = {};

    // Loading in the inverse bind pose at the correct joint index
    cgltf_accessor_read_float(skin->inverse_bind_matrices, 
                              i,
                              (nikola::f32*)nikola::mat4_raw_data(joint_data.inverse_bind_pose), 
                              16);

    // Filling in some important information for the next joints
   
    joint_data.parent = nullptr;
    joint_data.index  = i;
    
    // Trying to find the correct parent in the map
    if(data.joints.find(joint->parent->name) != data.joints.end()) {
      joint_data.parent = &data.joints[joint->parent->name];
    }

    // Welcome to the family, Mr. Joint!
    data.joints[joint->name] = joint_data;
  }

  for(nikola::sizei i = 0; i < animation->channels_count; i++) {
    cgltf_animation_channel* channel = &animation->channels[i];
    cgltf_node* target_node          = channel->target_node;

    cgltf_animation_sampler* sampler = channel->sampler;

    NIKOLA_LOG_TRACE("Target name = %s", target_node->name);
    NIKOLA_LOG_TRACE("  Parent = %zu", cgltf_node_index(data.gltf, target_node->parent));
    NIKOLA_LOG_TRACE("  Index  = %zu", i);

    nikola::Vec4 timestep(0.0f);
    cgltf_accessor_read_float(sampler->input, i, &timestep[0], 16);

    NIKOLA_LOG_TRACE("IN  = %s", nikola::vec4_to_string(timestep).c_str());
    NIKOLA_LOG_TRACE("OUT = %zu", sampler->output->stride);
  }

  // cgltf_free(gltf_data);
  return true;
}

/// Animation loader functions
/// ----------------------------------------------------------------------

} // End of nbr

//////////////////////////////////////////////////////////////////////////
