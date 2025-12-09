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

  // Build a joint hierarchy first to make sure everything is in order

  nikola::HashMap<nikola::String, nikola::u16> joints_lookup_table;
  for(nikola::sizei i = 0; i < skin->joints_count; i++) {
    cgltf_node* joint   = skin->joints[i];
    nikola::String name = joint->name;

    joints_lookup_table[name] = (nikola::u16)i;
  }

  // Load the inverse bind matrices  
  
  nikola::f32 floats_read = cgltf_accessor_unpack_floats(skin->inverse_bind_matrices, nullptr, skin->inverse_bind_matrices->count);

  nikola::DynamicArray<nikola::f32> inverse_matrices;
  inverse_matrices.resize(floats_read);
  
  cgltf_accessor_unpack_floats(skin->inverse_bind_matrices, inverse_matrices.data(), floats_read);

  // Load the joint hierarchy

  for(nikola::sizei i = 0, j = 0; i < skin->joints_count; i++, j += 16) {
    cgltf_node* joint                        = skin->joints[i];
    nikola::NBRSkeleton::NBRJoint* nbr_joint = &skele->joints[i];

    /// @NOTE (9/12/2025, Mohamed):
    ///
    /// Every position value has to be multiplied with the default 
    /// import scale in order to match the models. That only 
    /// applies to positions, however, and nothing else.  
    ///

    //
    // Set the transform
    //

    nbr_joint->position[0] = joint->translation[0] * nikola::NBR_MODEL_IMPORT_SCALE;
    nbr_joint->position[1] = joint->translation[1] * nikola::NBR_MODEL_IMPORT_SCALE;
    nbr_joint->position[2] = joint->translation[2] * nikola::NBR_MODEL_IMPORT_SCALE;

    nikola::memory_copy(nbr_joint->rotation, joint->rotation, sizeof(joint->rotation));
    nikola::memory_copy(nbr_joint->scale, joint->scale, sizeof(joint->scale));

    //
    // Set the inverse bind matrix
    // 

    // 1st column

    nbr_joint->inverse_bind_matrix[0] = inverse_matrices[j];
    nbr_joint->inverse_bind_matrix[1] = inverse_matrices[j + 1];
    nbr_joint->inverse_bind_matrix[2] = inverse_matrices[j + 2];

    // 2nd column

    nbr_joint->inverse_bind_matrix[3] = inverse_matrices[j + 4];
    nbr_joint->inverse_bind_matrix[4] = inverse_matrices[j + 5];
    nbr_joint->inverse_bind_matrix[5] = inverse_matrices[j + 6];

    // 3rd column

    nbr_joint->inverse_bind_matrix[6] = inverse_matrices[j + 8];
    nbr_joint->inverse_bind_matrix[7] = inverse_matrices[j + 9];
    nbr_joint->inverse_bind_matrix[8] = inverse_matrices[j + 10];

    // 4th column

    nbr_joint->inverse_bind_matrix[9]  = inverse_matrices[j + 12] * nikola::NBR_MODEL_IMPORT_SCALE; 
    nbr_joint->inverse_bind_matrix[10] = inverse_matrices[j + 13] * nikola::NBR_MODEL_IMPORT_SCALE;
    nbr_joint->inverse_bind_matrix[11] = inverse_matrices[j + 14] * nikola::NBR_MODEL_IMPORT_SCALE;

    nikola::f32* mat = &nbr_joint->inverse_bind_matrix[0];

    //
    // Set the name 
    //

    nikola::String joint_name = joint->name; 
    nbr_joint->name_length    = (nikola::u8)joint_name.size();

    nikola::memory_copy(nbr_joint->name, joint_name.c_str(), nbr_joint->name_length);

    //
    // Set the children
    //

    // No children, no processing. 
    // Sorry, dude. Go get laid or something... 

    if(joint->children_count <= 0) {
      nbr_joint->children_count = 0;
      nbr_joint->children       = nullptr;

      continue;
    }

    // Allocate the children's indices

    nbr_joint->children_count = (nikola::u16)joint->children_count;
    nbr_joint->children       = (nikola::u16*)nikola::memory_allocate(sizeof(nikola::u16) * nbr_joint->children_count);

    // Set the children's indices
    
    for(nikola::sizei j = 0; j < joint->children_count; j++) {
      nbr_joint->children[j] = joints_lookup_table[nikola::String(joint->children[j]->name)];
    }
  }

  // Done!

  cgltf_free(gltf);
  return true;
}

void skeleton_loader_unload(nikola::NBRSkeleton& skele) {
  for(nikola::u16 i = 0; i < skele.joints_count; i++) {
    if(skele.joints[i].children_count > 0) {
      nikola::memory_free(skele.joints[i].children);
    }
  }

  nikola::memory_free(skele.joints);
}

/// Skeleton loader functions
/// ----------------------------------------------------------------------

} // End of nbr

//////////////////////////////////////////////////////////////////////////
