#include "nbr.h"

#include <nikola/nikola.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nbr

/// ----------------------------------------------------------------------
/// NodeData
struct NodeData {
  NodeData* parent = nullptr;

  nikola::i32 index = 0;

  aiMatrix4x4 transform;
  aiMatrix4x4 inverse_bind_pose;
};
/// NodeData
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// PoseData
struct PoseData {
  nikola::String name;

  nikola::DynamicArray<aiVectorKey> positions;
  nikola::DynamicArray<aiQuatKey> rotations;
  nikola::DynamicArray<aiVectorKey> scalings;
};
/// PoseData
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// AnimData

struct AnimData {
  const aiScene* ai_scene;
  
  NodeData* current_node = nullptr;
  nikola::HashMap<nikola::String, NodeData> node_map;
  nikola::HashMap<nikola::String, aiBone*> bone_map;
  nikola::DynamicArray<PoseData> poses; 
  
  nikola::String anim_name; 
  nikola::f32 duration, frame_rate;
  nikola::DynamicArray<nikola::NBRJoint> joints;
  
  nikola::FilePath parent_dir;
  nikola::sizei nodes_count = 0;
};
// AnimData
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static void print_matrix(const char* name, const aiMatrix4x4& ai_mat) {
  nikola::Mat4 nk_mat; 

  nk_mat[0][0] = ai_mat[0][0];
  nk_mat[0][1] = ai_mat[0][1];
  nk_mat[0][2] = ai_mat[0][2];
  nk_mat[0][3] = ai_mat[0][3];

  nk_mat[1][0] = ai_mat[1][0];
  nk_mat[1][1] = ai_mat[1][1];
  nk_mat[1][2] = ai_mat[1][2];
  nk_mat[1][3] = ai_mat[1][3];

  nk_mat[2][0] = ai_mat[2][0];
  nk_mat[2][1] = ai_mat[2][1];
  nk_mat[2][2] = ai_mat[2][2];
  nk_mat[2][3] = ai_mat[2][3];

  nk_mat[3][0] = ai_mat[3][0];
  nk_mat[3][1] = ai_mat[3][1];
  nk_mat[3][2] = ai_mat[3][2];
  nk_mat[3][3] = ai_mat[3][3];

  NIKOLA_LOG_TRACE("%s =\n%s", name, nikola::mat4_to_string(nk_mat).c_str());
}

static bool is_valid_extension(const nikola::FilePath& ext) {
  return ext == ".fbx"  || 
         ext == ".gltf" || 
         ext == ".glb";
}

static void load_node_data(AnimData* data, const char* name) {
  aiNode* node = data->ai_scene->mRootNode->FindNode(name);
  
  aiMatrix4x4 transform = node->mTransformation; 
  if(data->current_node) {
    transform = (data->current_node->transform * node->mTransformation);
  }

  NodeData node_data = {
    .parent            = data->current_node,
    .index             = (nikola::i32)data->nodes_count++,
    .transform         = node->mTransformation,
    .inverse_bind_pose = transform.Inverse(), 
  };
  NIKOLA_LOG_TRACE("%s", node->mName.C_Str());
  print_matrix("CALC", node_data.inverse_bind_pose);

  data->node_map[name] = node_data;
  data->current_node   = &data->node_map[name];
}

static void load_bone_map(AnimData* data) {
  for(nikola::u32 i = 0; i < data->ai_scene->mNumMeshes; i++) {
    aiMesh* mesh = data->ai_scene->mMeshes[i];

    for(nikola::u32 j = 0; j < mesh->mNumBones; j++) {
      NIKOLA_LOG_TRACE("%s", mesh->mBones[i]->mName.C_Str());
      print_matrix("OFFSET", mesh->mBones[i]->mOffsetMatrix);
    }
  }
}

static void load_joint_map(AnimData* data) {
  // @TEMP (Animation loader): The loader currently only support one animation 
  // per file, which is annoying overall. Perhaps we can do something about this 
  // by creating multiple animation files or keeping it all in one `NBRAnimation` struct.
  // Perhaps by using armature data from Assimp... 
  
  // Load some animation data

  aiAnimation* anim = data->ai_scene->mAnimations[0];
  
  data->anim_name  = anim->mName.C_Str();
  data->duration   = anim->mDuration; 
  data->frame_rate = anim->mTicksPerSecond;

  // Some performance boosts?

  data->poses.reserve(anim->mNumChannels);
  data->node_map.reserve(anim->mNumChannels);

  // Running through all of the affected nodes of the animation

  for(nikola::sizei ia = 0; ia < anim->mNumChannels; ia++) {
    aiNodeAnim* anim_node    = anim->mChannels[ia];
    nikola::String node_name = anim_node->mNodeName.C_Str();

    // Load the specific node that this animation channel refer to
  
    data->poses.push_back(PoseData{});
    
    PoseData* pose_data = &data->poses[data->poses.size() - 1];
    pose_data->name     = node_name;

    load_node_data(data, node_name.c_str());

    // Load positions
    
    for(nikola::u32 i = 0; i < anim_node->mNumPositionKeys; i++) {
      pose_data->positions.push_back(anim_node->mPositionKeys[i]);
    }
    
    // Load positions
    
    for(nikola::u32 i = 0; i < anim_node->mNumRotationKeys; i++) {
      pose_data->rotations.push_back(anim_node->mRotationKeys[i]);
    }

    // Load scales
    
    for(nikola::u32 i = 0; i < anim_node->mNumScalingKeys; i++) {
      pose_data->scalings.push_back(anim_node->mScalingKeys[i]);
    }
  }
}

static void load_joint_data(nikola::NBRJoint* joint, const NodeData& node) {
  // Load the inverse bind matrix
  
  // 1st row

  joint->inverse_bind_pose[0] = node.inverse_bind_pose[0][0];
  joint->inverse_bind_pose[1] = node.inverse_bind_pose[0][1];
  joint->inverse_bind_pose[2] = node.inverse_bind_pose[0][2];
  joint->inverse_bind_pose[3] = node.inverse_bind_pose[0][3];

  // 2nd row

  joint->inverse_bind_pose[4] = node.inverse_bind_pose[1][0];
  joint->inverse_bind_pose[5] = node.inverse_bind_pose[1][1];
  joint->inverse_bind_pose[6] = node.inverse_bind_pose[1][2];
  joint->inverse_bind_pose[7] = node.inverse_bind_pose[1][3];

  // 3rd row

  joint->inverse_bind_pose[8]  = node.inverse_bind_pose[2][0];
  joint->inverse_bind_pose[9]  = node.inverse_bind_pose[2][1];
  joint->inverse_bind_pose[10] = node.inverse_bind_pose[2][2];
  joint->inverse_bind_pose[11] = node.inverse_bind_pose[2][3];

  // @TODO (Model loader): Also, as a performance boost, we may not need the last row.  
  // Since it is only needed for perspective projection.
  // 4th row

  joint->inverse_bind_pose[12] = node.inverse_bind_pose[3][0];
  joint->inverse_bind_pose[13] = node.inverse_bind_pose[3][1];
  joint->inverse_bind_pose[14] = node.inverse_bind_pose[3][2];
  joint->inverse_bind_pose[15] = node.inverse_bind_pose[3][3];

  // Parent index
  
  joint->parent_index = -1;
  if(node.parent) {
    joint->parent_index = (nikola::i16)node.parent->index;
  }
}

static void load_joints(AnimData* data) {
  for(nikola::sizei i = 0; i < data->poses.size(); i++) {
    PoseData* pose_data    = &data->poses[i];
    nikola::NBRJoint joint = {};
 
    // Load joint data (inverse bind pose and joint's parent index)
    load_joint_data(&joint, data->node_map[pose_data->name]);

    // Load positions

    joint.positions_count  = (nikola::u16)(4 * pose_data->positions.size()); 
    joint.position_samples = (nikola::f32*)nikola::memory_allocate(sizeof(float) * joint.positions_count);

    for(nikola::u32 i = 0, j = 0; i < pose_data->positions.size(); i++, j += 4) {
      joint.position_samples[j + 0] = pose_data->positions[i].mValue.x;
      joint.position_samples[j + 1] = pose_data->positions[i].mValue.y;
      joint.position_samples[j + 2] = pose_data->positions[i].mValue.z;
      joint.position_samples[j + 3] = (nikola::f32)pose_data->positions[i].mTime;
    }

    // Load rotations

    joint.rotations_count  = (nikola::u16)(5 * pose_data->rotations.size()); 
    joint.rotation_samples = (nikola::f32*)nikola::memory_allocate(sizeof(float) * joint.rotations_count);

    for(nikola::u32 i = 0, j = 0; i < pose_data->rotations.size(); i++, j += 5) {
      joint.rotation_samples[j + 0] = pose_data->rotations[i].mValue.x;
      joint.rotation_samples[j + 1] = pose_data->rotations[i].mValue.y;
      joint.rotation_samples[j + 2] = pose_data->rotations[i].mValue.z;
      joint.rotation_samples[j + 3] = pose_data->rotations[i].mValue.w;
      joint.rotation_samples[j + 4] = (nikola::f32)pose_data->rotations[i].mTime;
    }

    // Load scales

    joint.scales_count  = (nikola::u16)(4 * pose_data->scalings.size()); 
    joint.scale_samples = (nikola::f32*)nikola::memory_allocate(sizeof(float) * joint.scales_count);

    for(nikola::u32 i = 0, j = 0; i < pose_data->scalings.size(); i++, j += 4) {
      joint.scale_samples[j + 0] = pose_data->scalings[i].mValue.x;
      joint.scale_samples[j + 1] = pose_data->scalings[i].mValue.y;
      joint.scale_samples[j + 2] = pose_data->scalings[i].mValue.z;
      joint.scale_samples[j + 3] = (nikola::f32)pose_data->scalings[i].mTime;
    }

    data->joints.push_back(joint);
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
    NIKOLA_LOG_ERROR("No valid model loader for \'%s\'", ext.c_str());
    return false;
  } 

  // Load Assimp file
  
  int flags = (aiProcess_Triangulate           | 
               aiProcess_SplitLargeMeshes      |
               aiProcess_ImproveCacheLocality  | 
               aiProcess_JoinIdenticalVertices |
               aiProcess_GlobalScale           | 
               aiProcess_OptimizeMeshes); 

  Assimp::Importer imp; 
  imp.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 1.0f);

  const aiScene* scene = imp.ReadFile(path, flags);
  if(!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
    NIKOLA_LOG_ERROR("Could not load Model at \'%s\' - %s", path.c_str(), imp.GetErrorString());
    return false;
  }

  // Load everything into the `AnimData` struct 

  AnimData data   = {};
  data.parent_dir = nikola::filepath_parent_path(path); 
  data.ai_scene   = scene;
 
  load_bone_map(&data);
  load_joint_map(&data);

  // Load the joints 

  load_joints(&data);

  anim->joints_count = (nikola::u16)data.joints.size(); 
  anim->joints       = (nikola::NBRJoint*)nikola::memory_allocate(sizeof(nikola::NBRJoint) * anim->joints_count);
  nikola::memory_copy(anim->joints, data.joints.data(), sizeof(nikola::NBRJoint) * anim->joints_count);

  // Load the animation's time info

  anim->duration   = data.duration; 
  anim->frame_rate = data.frame_rate;

  NIKOLA_LOG_TRACE("Animation loaded:");
  NIKOLA_LOG_TRACE("  Name         = %s", data.anim_name.c_str());
  NIKOLA_LOG_TRACE("  Joints count = %zu", data.joints.size());
  NIKOLA_LOG_TRACE("  Duration     = %f", data.duration);
  NIKOLA_LOG_TRACE("  Frame rate   = %f", data.frame_rate);
  
  return true;
}

void animation_loader_unload(nikola::NBRAnimation& anim) {
  for(nikola::sizei i = 0; i < anim.joints_count; i++) {
    nikola::memory_free(anim.joints[i].position_samples);
    nikola::memory_free(anim.joints[i].rotation_samples);
    nikola::memory_free(anim.joints[i].scale_samples);
  } 

  nikola::memory_free(anim.joints);
}

/// Animation loader functions
/// ----------------------------------------------------------------------

} // End of nbr

//////////////////////////////////////////////////////////////////////////
