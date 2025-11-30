#include "nbr.h"

#include <nikola/nikola.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nbr

/// ----------------------------------------------------------------------
/// AnimData

struct AnimData {
  const aiScene* ai_scene;
};
// AnimData
/// ----------------------------------------------------------------------

// /// ----------------------------------------------------------------------
// /// Private functions

static bool is_valid_extension(const nikola::FilePath& ext) {
  return ext == ".gltf" || 
         ext == ".glb"  || 
         ext == ".dae";
}

static void load_position_channels(nikola::NBRAnimation::NBRJointTrack* track, aiVectorKey* keys, const nikola::sizei count) {
  track->positions_count  = (nikola::u16)(4 * count); 
  track->position_samples = (nikola::f32*)nikola::memory_allocate(sizeof(float) * track->positions_count);

  for(nikola::u32 i = 0, j = 0; i < count; i++, j += 4) {
    track->position_samples[j + 0] = keys[i].mValue.x;
    track->position_samples[j + 1] = keys[i].mValue.y;
    track->position_samples[j + 2] = keys[i].mValue.z;
    track->position_samples[j + 3] = (nikola::f32)keys[i].mTime;
  }
}

static void load_rotation_channels(nikola::NBRAnimation::NBRJointTrack* track, aiQuatKey* keys, const nikola::sizei count) {
  track->rotations_count  = (nikola::u16)(5 * count); 
  track->rotation_samples = (nikola::f32*)nikola::memory_allocate(sizeof(float) * track->rotations_count);

  for(nikola::u32 i = 0, j = 0; i < count; i++, j += 5) {
    track->rotation_samples[j + 0] = keys[i].mValue.x;
    track->rotation_samples[j + 1] = keys[i].mValue.y;
    track->rotation_samples[j + 2] = keys[i].mValue.z;
    track->rotation_samples[j + 3] = keys[i].mValue.w;
    track->rotation_samples[j + 4] = (nikola::f32)keys[i].mTime;
  }
}

static void load_scale_channels(nikola::NBRAnimation::NBRJointTrack* track, aiVectorKey* keys, const nikola::sizei count) {
  track->scales_count  = (nikola::u16)(4 * count);
  track->scale_samples = (nikola::f32*)nikola::memory_allocate(sizeof(float) * track->scales_count);

  for(nikola::u32 i = 0, j = 0; i < count; i++, j += 4) {
    track->scale_samples[j + 0] = keys[i].mValue.x;
    track->scale_samples[j + 1] = keys[i].mValue.y;
    track->scale_samples[j + 2] = keys[i].mValue.z;
    track->scale_samples[j + 3] = (nikola::f32)keys[i].mTime;
  }
}

static void load_animation_data(const aiScene* scene, nikola::NBRAnimation* nbr_anim) {
  // @TEMP (Animation loader): The loader currently supports only one animation 
  // per file, which is annoying overall. Perhaps we can do something about this 
  // by creating multiple animation files or keeping it all in one `NBRAnimation` struct.
  // Perhaps by using armature data from Assimp... 
  
  // Load some animation data

  aiAnimation* anim = scene->mAnimations[0];

  nikola::sizei tracks_size = sizeof(nikola::NBRAnimation::NBRJointTrack) * anim->mNumChannels;

  nbr_anim->duration      = anim->mDuration; 
  nbr_anim->tracks_count = (nikola::u16)anim->mNumChannels; 
  nbr_anim->tracks       = (nikola::NBRAnimation::NBRJointTrack*)nikola::memory_allocate(tracks_size);
  
  // Running through all of the affected nodes of the animation

  for(nikola::sizei i = 0; i < anim->mNumChannels; i++) {
    aiNodeAnim* anim_node = anim->mChannels[i];

    // Load positions
    load_position_channels(&nbr_anim->tracks[i], anim_node->mPositionKeys, anim_node->mNumPositionKeys); 
    
    // Load rotations
    load_rotation_channels(&nbr_anim->tracks[i], anim_node->mRotationKeys, anim_node->mNumRotationKeys); 

    // Load scales
    load_scale_channels(&nbr_anim->tracks[i], anim_node->mScalingKeys, anim_node->mNumScalingKeys); 
  }

  NIKOLA_LOG_TRACE("%s - %f, %i", anim->mName.C_Str(), anim->mDuration, anim->mNumChannels);
}

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

  // Load everything into the given animation
  load_animation_data(scene, anim);

  // Done!
  return true;
}

void animation_loader_unload(nikola::NBRAnimation& anim) {
  for(nikola::sizei i = 0; i < anim.tracks_count; i++) {
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
