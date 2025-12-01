#include "nikola/nikola_render.h"
#include "nikola/nikola_input.h"

#include <ozz/animation/offline/raw_skeleton.h>
#include <ozz/animation/offline/raw_animation.h>
#include <ozz/animation/offline/skeleton_builder.h>
#include <ozz/animation/offline/animation_builder.h>

#include <ozz/animation/runtime/animation.h>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Skeleton
struct Skeleton {

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
/// Animator
struct Animator {
  AnimatorDesc desc;
};
/// Animator
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Skeleton functions

Skeleton* skeleton_create(const NBRSkeleton& nbr_skele) {
  Skeleton* skele = new Skeleton{};

  return skele;
}

void skeleton_destroy(Skeleton* skele) {
  if(!skele) {
    return;
  }

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
  }

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
/// Animator functions

Animator* animator_create() {
  Animator* anim = new Animator{};

  return anim;
}

void animator_animate(Animator* animator, const ResourceID& skeleton_id, const ResourceID& animation_id, const f32 dt) {
  NIKOLA_ASSERT(animator, "Invalid Animator given to animator_animate");
}

AnimatorDesc& animator_get_desc(Animator* animator) {
  NIKOLA_ASSERT(animator, "Invalid Animator given to animator_get_desc");
  return animator->desc;
}

/// Animator functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
