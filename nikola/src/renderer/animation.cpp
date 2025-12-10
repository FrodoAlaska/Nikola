#include "nikola/nikola_render.h"
#include "nikola/nikola_input.h"

#include <ozz/animation/offline/raw_skeleton.h>
#include <ozz/animation/offline/raw_animation.h>
#include <ozz/animation/offline/skeleton_builder.h>
#include <ozz/animation/offline/animation_builder.h>

#include <ozz/animation/runtime/animation.h>
#include <ozz/animation/runtime/skeleton.h>
#include <ozz/animation/runtime/local_to_model_job.h>
#include <ozz/animation/runtime/sampling_job.h>

#include <ozz/base/maths/transform.h>
#include <ozz/base/maths/simd_math.h>
#include <ozz/base/maths/soa_transform.h>
#include <ozz/base/maths/vec_float.h>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Skeleton
struct Skeleton {
  ozz::unique_ptr<ozz::animation::Skeleton> handle;
  DynamicArray<Mat4> inverse_bind_matrices;
};
/// Skeleton
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Animation
struct Animation {
  ozz::unique_ptr<ozz::animation::Animation> handle;
};
/// Animation
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AnimationSampler
struct AnimationSampler {
  DynamicArray<Animation*> animations; 
  Skeleton* skeleton = nullptr;

  ozz::animation::SamplingJob::Context context;
  ozz::vector<ozz::math::SoaTransform> locals;

  ozz::vector<ozz::math::Float4x4> models;
  Array<Mat4, JOINTS_MAX> skinning_palette;

  AnimationSamplerInfo info;
};
/// AnimationSampler
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void traverse_joints(Skeleton* skeleton,
                            ozz::animation::offline::RawSkeleton::Joint* joint, 
                            const NBRSkeleton& nbr_skele, 
                            const NBRSkeleton::NBRJoint& nbr_joint) {
  // Convert the name  
  joint->name = nbr_joint.name;

  // Convert the inverse bind matrices  

  const f32* mat = &nbr_joint.inverse_bind_matrix[0];
  skeleton->inverse_bind_matrices.emplace_back(mat[0], mat[1],  mat[2],  0.0f, 
                                               mat[3], mat[4],  mat[5],  0.0f, 
                                               mat[6], mat[7],  mat[8],  0.0f, 
                                               mat[9], mat[10], mat[11], 1.0f);

  // Converting the transform

  joint->transform.translation = ozz::math::Float3(nbr_joint.position[0], 
                                                   nbr_joint.position[1], 
                                                   nbr_joint.position[2]);
  joint->transform.rotation    = ozz::math::Quaternion(nbr_joint.rotation[0], 
                                                       nbr_joint.rotation[1], 
                                                       nbr_joint.rotation[2],
                                                       nbr_joint.rotation[3]);
  joint->transform.scale       = ozz::math::Float3(nbr_joint.scale[0], 
                                                   nbr_joint.scale[1], 
                                                   nbr_joint.scale[2]);

  // Going over the children

  if(nbr_joint.children_count == 0) {
    return;
  }

  joint->children.resize(nbr_joint.children_count);
  for(sizei i = 0; i < joint->children.size(); i++) {
    traverse_joints(skeleton, &joint->children[i], nbr_skele, nbr_skele.joints[nbr_joint.children[i]]);
  }
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Skeleton functions

Skeleton* skeleton_create(const NBRSkeleton& nbr_skele) {
  Skeleton* skele = new Skeleton{};

  // Converting our NBR format to the OZZ runtime format

  ozz::animation::offline::RawSkeleton raw_skele;
  raw_skele.roots.resize(1);

  // Root joint init
  traverse_joints(skele, &raw_skele.roots[0], nbr_skele, nbr_skele.joints[nbr_skele.root_index]);

  // Validate the skeleton first before existing. 
  // This is crucial, as the skeleton will not be built 
  // if this validation fails.

  if(!raw_skele.Validate()) {
    delete skele;

    NIKOLA_LOG_ERROR("Failed to validate skeleton!");
    return nullptr;
  }

  // Build the skeleton

  ozz::animation::offline::SkeletonBuilder builder;
  skele->handle = builder(raw_skele);

  // Done!
  return skele;
}

void skeleton_destroy(Skeleton* skele) {
  if(!skele) {
    return;
  }

  skele->handle.reset();
  skele->inverse_bind_matrices.clear();

  delete skele;
}

/// Skeleton functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Animation functions

Animation* animation_create(const NBRAnimation& nbr_anim) {
  Animation* anim = new Animation{};

  // Converting our NBR format to the OZZ runtime format

  ozz::animation::offline::RawAnimation raw_anim;

  raw_anim.tracks.resize(nbr_anim.tracks_count);
  raw_anim.duration = nbr_anim.duration;
  raw_anim.name     = nbr_anim.name;

  // Converting the tracks

  for(sizei i = 0; i < raw_anim.tracks.size(); i++) {
    ozz::animation::offline::RawAnimation::JointTrack* track = &raw_anim.tracks[i];
    NBRAnimation::NBRJointTrack* nbr_track                   = &nbr_anim.tracks[i];

    // Converting the positions
   
    track->translations.resize(nbr_track->positions_count);
    for(sizei ip = 0, j = 0; ip < track->translations.size(); ip++, j += 4) {
      track->translations[ip].value.x = nbr_track->position_samples[j + 0];
      track->translations[ip].value.y = nbr_track->position_samples[j + 1];
      track->translations[ip].value.z = nbr_track->position_samples[j + 2];
      track->translations[ip].time    = nbr_track->position_samples[j + 3];
    }

    // Converting the rotations
    
    track->rotations.resize(nbr_track->rotations_count);
    for(sizei ip = 0, j = 0; ip < track->rotations.size(); ip++, j += 5) {
      track->rotations[ip].value.x = nbr_track->rotation_samples[j + 0];
      track->rotations[ip].value.y = nbr_track->rotation_samples[j + 1];
      track->rotations[ip].value.z = nbr_track->rotation_samples[j + 2];
      track->rotations[ip].value.w = nbr_track->rotation_samples[j + 3];
      track->rotations[ip].time    = nbr_track->rotation_samples[j + 4];
    }

    // Converting the scales
    
    track->scales.resize(nbr_track->scales_count);
    for(sizei ip = 0, j = 0; ip < track->scales.size(); ip++, j += 4) {
      track->scales[ip].value.x = nbr_track->scale_samples[j + 0];
      track->scales[ip].value.y = nbr_track->scale_samples[j + 1];
      track->scales[ip].value.z = nbr_track->scale_samples[j + 2];
      track->scales[ip].time    = nbr_track->scale_samples[j + 3];
    }

    if(!track->Validate(raw_anim.duration)) {
      NIKOLA_LOG_ERROR("Failed to validate track at index \'%zu\'", i);
    }
  }
  
  // Validate the animation first...

  if(!raw_anim.Validate()) {
    delete anim;

    NIKOLA_LOG_ERROR("Failed to validate animation!");
    return nullptr;
  }

  // Build the animation

  ozz::animation::offline::AnimationBuilder builder;
  anim->handle = builder(raw_anim);

  // Done!
  return anim;
}

void animation_destroy(Animation* anim) {
  if(!anim) {
    return;
  }

  anim->handle.reset();
  delete anim;
}

/// Animation functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// AnimatorSampler functions

AnimationSampler* animation_sampler_create(const ResourceID& skeleton_id, const ResourceID* animations, const sizei animations_count) {
  AnimationSampler* sampler = new AnimationSampler{};

  // Retrieving the resources

  sampler->animations.reserve(animations_count);
  for(sizei i = 0; i < animations_count; i++) {
    sampler->animations.push_back(resources_get_animation(animations[i]));
  } 

  sampler->skeleton = resources_get_skeleton(skeleton_id);

  // Resizing arrays for performance reasons

  sampler->locals.resize(sampler->skeleton->handle->num_soa_joints());
  sampler->models.resize(sampler->skeleton->handle->num_joints());
  sampler->context.Resize(sampler->skeleton->handle->num_joints());

  // Done!
  return sampler;
}

AnimationSampler* animation_sampler_create(const ResourceID& skeleton_id, const ResourceID& animation_id) {
  return animation_sampler_create(skeleton_id, &animation_id, 1);
}

void animation_sampler_destroy(AnimationSampler* sampler) {
  if(!sampler) {
    return;
  }

  sampler->animations.clear();
  sampler->skeleton = nullptr;

  sampler->locals.clear();
  sampler->models.clear();
  sampler->context.Invalidate();

  delete sampler;
}

AnimationSamplerInfo& animation_sampler_get_info(AnimationSampler* sampler) {
  NIKOLA_ASSERT(sampler, "Invalid AnimationSampler given to animation_sampler_get_info");
  return sampler->info;
}

const Array<Mat4, JOINTS_MAX>& animation_sampler_get_skinning_palette(const AnimationSampler* sampler) {
  NIKOLA_ASSERT(sampler, "Invalid AnimationSampler given to animation_sampler_get_skinning_palette");
  return sampler->skinning_palette;
}

void animation_sampler_update(AnimationSampler* sampler, const f32 dt) {
  NIKOLA_ASSERT(sampler, "Invalid AnimationSampler given to animation_sampler_update");

  // Sorry. You're not animating, dude

  if(!sampler->info.is_animating) { 
    return;
  }

  // Get the current animation
  
  Animation* animation = sampler->animations[sampler->info.current_animation];
  f32 duration         = animation->handle->duration();  

  // Looping is turned off and we're past the end so return...
  // Otherwise, we can start the animation again. 

  if(!sampler->info.is_looping && sampler->info.current_time > duration) {
    return;
  }
  else if(sampler->info.current_time > duration) { 
    sampler->info.current_time = 0.0f;
  }

  // Update the time 
  sampler->info.current_time += (dt * sampler->info.play_speed) / duration;

  // Sampling job

  ozz::animation::SamplingJob sample_job;
  sample_job.animation = animation->handle.get();
  sample_job.context   = &sampler->context;
  sample_job.ratio     = sampler->info.current_time;
  sample_job.output    = make_span(sampler->locals);

  if(!sample_job.Run()) {
    NIKOLA_LOG_DEBUG("Failed to run the sampling job for a sampler");
    return;
  }

  // Local to model job

  ozz::animation::LocalToModelJob local_to_model_job;
  local_to_model_job.skeleton = sampler->skeleton->handle.get();
  local_to_model_job.input    = make_span(sampler->locals);
  local_to_model_job.output   = make_span(sampler->models);

  if(!local_to_model_job.Run()) {
    NIKOLA_LOG_DEBUG("Failed to run the local to model job for a sampler");
    return;
  }

  // Convert the newly calculated models into our engine format

  for(sizei i = 0; i < sampler->models.size(); i++) {
    const ozz::math::Float4x4& ozz_mat = sampler->models[i];

    // Loading from SIMD registers into an array of floats

    f32 raw_mat[16];
    ozz::math::StorePtr(ozz_mat.cols[0], &raw_mat[0]);
    ozz::math::StorePtr(ozz_mat.cols[1], &raw_mat[4]);
    ozz::math::StorePtr(ozz_mat.cols[2], &raw_mat[8]);
    ozz::math::StorePtr(ozz_mat.cols[3], &raw_mat[12]);
    
    // Set the skinning matrix
    sampler->skinning_palette[i] = mat4_make(raw_mat) * sampler->skeleton->inverse_bind_matrices[i];
  }
}

/// AnimatorSampler functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
