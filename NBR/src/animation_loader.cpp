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
  nikola::String name;
  nikola::String parent_name = "INVALID";
  nikola::i32 index          = 0;

  aiMatrix4x4 inverse_bind_pose;
  aiMatrix4x4 transform;
};
/// NodeData
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// PoseData
struct PoseData {
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
  
  nikola::HashMap<nikola::String, NodeData> node_map;
  nikola::HashMap<nikola::String, PoseData> poses; 
 
  nikola::DynamicArray<nikola::String> node_indices;

  nikola::f32 duration, frame_rate;
  nikola::DynamicArray<nikola::NBRJoint> joints;
};
// AnimData
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static bool is_valid_extension(const nikola::FilePath& ext) {
  return ext == ".gltf" || 
         ext == ".glb"  || 
         ext == ".dae";
}

static void decompose_ai_matrix(nikola::NBRJoint* joint, const aiMatrix4x4& mat) {
  // 1st row

  joint->inverse_bind_pose[0] = mat[0][0];
  joint->inverse_bind_pose[1] = mat[0][1];
  joint->inverse_bind_pose[2] = mat[0][2];
  joint->inverse_bind_pose[3] = mat[0][3];

  // 2nd row

  joint->inverse_bind_pose[4] = mat[1][0];
  joint->inverse_bind_pose[5] = mat[1][1];
  joint->inverse_bind_pose[6] = mat[1][2];
  joint->inverse_bind_pose[7] = mat[1][3];

  // 3rd row

  joint->inverse_bind_pose[8]  = mat[2][0];
  joint->inverse_bind_pose[9]  = mat[2][1];
  joint->inverse_bind_pose[10] = mat[2][2];
  joint->inverse_bind_pose[11] = mat[2][3];
}

static void strip_junk_from_name(nikola::String* name) {
  // @NOTE: We need this function because Assimp adds some junk to 
  // the name for FBX files in order to structure animations correctly (I think?)
  // Since we rely heavily on the names for getting the indices, we _have_ to get 
  // the "raw" name of the bone. Otherwise, the `load_bone_map` function won't find 
  // the correct nodes.
  //
  // By the way, for reference, the name can look something like this "mixamorig:RightHand_$AssimpFbx$_Rotation".

  nikola::sizei unique_sign = name->find_first_of('$');
  *name                     = name->substr(0, unique_sign - 1);
}

static void push_node_from_bone(AnimData* data, aiBone* bone) {
  NodeData node_data{};

  node_data.name        = bone->mNode->mName.C_Str();
  node_data.parent_name = bone->mNode->mParent->mName.C_Str();
  strip_junk_from_name(&node_data.name);
  strip_junk_from_name(&node_data.parent_name);

  if(data->node_map.find(node_data.name) != data->node_map.end()) { // Node is already in the map
    return;
  } 

  node_data.index             = (nikola::i32)data->node_map.size();
  node_data.inverse_bind_pose = bone->mOffsetMatrix;
  node_data.transform         = bone->mNode->mTransformation; 

  data->node_indices.push_back(node_data.name);
  data->node_map[node_data.name] = node_data;
}

static void push_parent_node(AnimData* data, aiBone* bone) {
  // Recursively go through all the parents until you hit a 
  // parent joint that is already in the map. This way, 
  // we can make sure that the sequential order is kept 
  // for the runtime. Parents _MUST_ come before children 
  // in the array. 

  aiBone* parent_bone = nullptr;
  if(data->node_map.find(bone->mNode->mParent->mName.C_Str()) == data->node_map.end()) {
    parent_bone = data->ai_scene->findBone(bone->mNode->mParent->mName);
  }

  if(parent_bone) {
    push_parent_node(data, parent_bone);
  }

  push_node_from_bone(data, bone);
}

static void load_bone_map(AnimData* data) {
  for(nikola::u32 i = 0; i < data->ai_scene->mNumMeshes; i++) {
    aiMesh* mesh = data->ai_scene->mMeshes[i];
    
    for(nikola::u32 j = 0; j < mesh->mNumBones; j++) {
      aiBone* bone = mesh->mBones[j];

      nikola::String node_name   = bone->mNode->mName.C_Str();
      nikola::String parent_name = bone->mNode->mParent->mName.C_Str();
    
      push_parent_node(data, bone);
      push_node_from_bone(data, bone);
    }
  }
}

static void load_pose_map(AnimData* data) {
  // @TEMP (Animation loader): The loader currently only support one animation 
  // per file, which is annoying overall. Perhaps we can do something about this 
  // by creating multiple animation files or keeping it all in one `NBRAnimation` struct.
  // Perhaps by using armature data from Assimp... 
  
  // Load some animation data

  aiAnimation* anim = data->ai_scene->mAnimations[0];
  
  data->duration   = anim->mDuration; 
  data->frame_rate = anim->mTicksPerSecond;

  // Running through all of the affected nodes of the animation

  for(nikola::sizei ia = 0; ia < anim->mNumChannels; ia++) {
    aiNodeAnim* anim_node = anim->mChannels[ia];
    PoseData pose_data    = {};

    // Load positions
    
    for(nikola::u32 i = 0; i < anim_node->mNumPositionKeys; i++) {
      pose_data.positions.push_back(anim_node->mPositionKeys[i]);
    }
    
    // Load positions
    
    for(nikola::u32 i = 0; i < anim_node->mNumRotationKeys; i++) {
      pose_data.rotations.push_back(anim_node->mRotationKeys[i]);
    }

    // Load scales
    
    for(nikola::u32 i = 0; i < anim_node->mNumScalingKeys; i++) {
      pose_data.scalings.push_back(anim_node->mScalingKeys[i]);
    }

    data->poses[anim_node->mNodeName.C_Str()] = pose_data;
  }
}

static void load_position_channels(nikola::NBRJoint* joint, aiVectorKey* keys, const nikola::sizei count) {
  joint->positions_count  = (nikola::u16)(4 * count); 
  joint->position_samples = (nikola::f32*)nikola::memory_allocate(sizeof(float) * joint->positions_count);

  for(nikola::u32 i = 0, j = 0; i < count; i++, j += 4) {
    joint->position_samples[j + 0] = keys[i].mValue.x;
    joint->position_samples[j + 1] = keys[i].mValue.y;
    joint->position_samples[j + 2] = keys[i].mValue.z;
    joint->position_samples[j + 3] = (nikola::f32)keys[i].mTime;
  }
}

static void load_rotation_channels(nikola::NBRJoint* joint, aiQuatKey* keys, const nikola::sizei count) {
  joint->rotations_count  = (nikola::u16)(5 * count); 
  joint->rotation_samples = (nikola::f32*)nikola::memory_allocate(sizeof(float) * joint->rotations_count);

  for(nikola::u32 i = 0, j = 0; i < count; i++, j += 5) {
    joint->rotation_samples[j + 0] = keys[i].mValue.x;
    joint->rotation_samples[j + 1] = keys[i].mValue.y;
    joint->rotation_samples[j + 2] = keys[i].mValue.z;
    joint->rotation_samples[j + 3] = keys[i].mValue.w;
    joint->rotation_samples[j + 4] = (nikola::f32)keys[i].mTime;
  }
}

static void load_scale_channels(nikola::NBRJoint* joint, aiVectorKey* keys, const nikola::sizei count) {
  joint->scales_count  = (nikola::u16)(4 * count);
  joint->scale_samples = (nikola::f32*)nikola::memory_allocate(sizeof(float) * joint->scales_count);

  for(nikola::u32 i = 0, j = 0; i < count; i++, j += 4) {
    joint->scale_samples[j + 0] = keys[i].mValue.x;
    joint->scale_samples[j + 1] = keys[i].mValue.y;
    joint->scale_samples[j + 2] = keys[i].mValue.z;
    joint->scale_samples[j + 3] = (nikola::f32)keys[i].mTime;
  }
}

static void load_joints(AnimData* data) {
  data->joints.reserve(data->node_map.size());

  for(nikola::sizei i = 0; i < data->node_indices.size(); i++) {
    NodeData* node_data    = &data->node_map[data->node_indices[i]];
    nikola::NBRJoint joint = {};

    // Load the channel keys if this joint gets animated

    if(data->poses.find(node_data->name) != data->poses.end()) { // The node is affected by the animation
      PoseData* pose = &data->poses[node_data->name];

      load_position_channels(&joint, pose->positions.data(), pose->positions.size());
      load_rotation_channels(&joint, pose->rotations.data(), pose->rotations.size());
      load_scale_channels(&joint, pose->scalings.data(), pose->scalings.size());
    }
    else { // Just use the node's position, rotation, and scale since this joint isn't affected by the animation
      aiVectorKey position, scale; 
      aiQuatKey rotation;
      node_data->transform.Decompose(position.mValue, rotation.mValue, scale.mValue);

      position.mTime = 0.0f;
      rotation.mTime = 0.0f;
      scale.mTime    = 0.0f;
      scale.mValue   = aiVector3D(1.0f, 1.0f, 1.0f);

      load_position_channels(&joint, &position, 1);
      load_rotation_channels(&joint, &rotation, 1);
      load_scale_channels(&joint, &scale, 1);
    }
 
    // Load other relevant information

    joint.parent_index = -1;
    if(data->node_map.find(node_data->parent_name) != data->node_map.end()) {
      joint.parent_index = (nikola::i16)data->node_map[node_data->parent_name].index;
    }

    // NIKOLA_LOG_TRACE("%s (%i) - %s (%i)", node_data->name.c_str(), node_data->index, node_data->parent_name.c_str(), joint.parent_index);
    decompose_ai_matrix(&joint, node_data->inverse_bind_pose);

    // Welcome, Mr. Joint!
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
    NIKOLA_LOG_ERROR("Unsupported animation format found at \'%s\'. The supported formats are: GLTF, GLB", path.c_str());
    return false;
  } 

  // Load Assimp file
  
  int flags = (aiProcess_Triangulate           | 
               aiProcess_SplitLargeMeshes      |
               aiProcess_ImproveCacheLocality  | 
               aiProcess_JoinIdenticalVertices |
               aiProcess_PopulateArmatureData  |
               aiProcess_LimitBoneWeights      | 
               aiProcess_GlobalScale           | 
               aiProcess_OptimizeMeshes); 

  Assimp::Importer imp; 
  imp.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, nikola::NBR_MODEL_IMPORT_SCALE);
  imp.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, nikola::NBR_JOINT_WEIGHTS_MAX);

  const aiScene* scene = imp.ReadFile(path, flags);
  if(!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
    NIKOLA_LOG_ERROR("Could not load animation at \'%s\' - %s", path.c_str(), imp.GetErrorString());
    return false;
  }

  // Load everything into the `AnimData` struct 

  AnimData* data  = new AnimData{};
  data->ai_scene  = scene;
 
  load_bone_map(data);
  load_pose_map(data);

  // Load the joints 

  load_joints(data);

  anim->joints_count = (nikola::u16)data->joints.size(); 
  anim->joints       = (nikola::NBRJoint*)nikola::memory_allocate(sizeof(nikola::NBRJoint) * anim->joints_count);
  nikola::memory_copy(anim->joints, data->joints.data(), sizeof(nikola::NBRJoint) * anim->joints_count);

  // Load the animation's time info

  anim->duration   = data->duration; 
  anim->frame_rate = data->frame_rate;
 
  data->node_map.clear();
  // delete data; @TEMP: Causes a segfault. Fuck it. Let it leak...
  
  return true;
}

void animation_loader_unload(nikola::NBRAnimation& anim) {
  for(nikola::sizei i = 0; i < anim.joints_count; i++) {
    if(anim.joints[i].position_samples) {
      nikola::memory_free(anim.joints[i].position_samples);
    }

    if(anim.joints[i].rotation_samples) {
      nikola::memory_free(anim.joints[i].rotation_samples);
    }

    if(anim.joints[i].scale_samples) {
      nikola::memory_free(anim.joints[i].scale_samples);
    }
  } 

  nikola::memory_free(anim.joints);
}

/// Animation loader functions
/// ----------------------------------------------------------------------

} // End of nbr

//////////////////////////////////////////////////////////////////////////
