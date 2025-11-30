#include "nikola/nikola_render.h"
#include "nikola/nikola_input.h"

#include <ozz/animation/offline/raw_skeleton.h>
#include <ozz/animation/offline/raw_animation.h>
#include <ozz/animation/offline/skeleton_builder.h>
#include <ozz/animation/offline/animation_builder.h>

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

  return anim;
}

void animation_destroy(Animation* anim) {
  if(!anim) {
    return;
  }

  delete anim;
}

/// Animation functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Animator functions

Animator* animator_create(const AnimatorDesc& desc) {
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
