#include "nbr.h"

#include <nikola/nikola.h>
#include <cgltf/cgltf.h>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nbr

/// ----------------------------------------------------------------------
/// Private functions

static bool is_valid_extension(const nikola::FilePath& ext) {
  return ext == ".gltf" || 
         ext == ".glb";
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Skeleton loader functions

bool skeleton_loader_load(nikola::NBRSkeleton* skele, const nikola::FilePath& path) {
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

  // Allocate the joints array

  cgltf_skin* skin = &gltf->skins[0];

  skele->joints_count = (nikola::u16)skin->joints_count;
  skele->joints       = (nikola::NBRSkeleton::NBRJoint*)nikola::memory_allocate(sizeof(nikola::NBRSkeleton::NBRJoint) * skele->joints_count);
  skele->root_index   = 0; 

  // Load the joint hierarchy

  for(nikola::sizei i = 0; i < skin->joints_count; i++) {
    cgltf_node* joint                        = skin->joints[i];
    nikola::NBRSkeleton::NBRJoint* nbr_joint = &skele->joints[i];
   
    // Allocate children

    nbr_joint->children_count = (nikola::u16)joint->children_count;
    nbr_joint->children       = (nikola::u16*)nikola::memory_allocate(sizeof(nikola::u16) * nbr_joint->children_count);

    // Set the children indices
    
    for(nikola::sizei j = 0; j < joint->children_count; j++) {
      nbr_joint->children[j] = (nikola::u16)cgltf_node_index(gltf, joint->children[j]);
    }

    // Set the transform
    
    nikola::memory_copy(nbr_joint->position, joint->translation, sizeof(joint->translation));
    nikola::memory_copy(nbr_joint->rotation, joint->rotation, sizeof(joint->rotation));
    nikola::memory_copy(nbr_joint->scale, joint->scale, sizeof(joint->scale));
  }

  // Done!

  cgltf_free(gltf);
  return true;
}

void skeleton_loader_unload(nikola::NBRSkeleton& skele) {
  for(nikola::u16 i = 0; i < skele.joints_count; i++) {
    nikola::memory_free(skele.joints[i].children);
  }

  nikola::memory_free(skele.joints);
}

/// Skeleton loader functions
/// ----------------------------------------------------------------------

} // End of nbr

//////////////////////////////////////////////////////////////////////////
