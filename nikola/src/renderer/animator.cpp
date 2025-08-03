#include "nikola/nikola_render.h"
#include "nikola/nikola_input.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

// @TEMP
static const f32 get_scale_factor(const f32 last_time, const f32 next_time, const f32 anim_time) {
  f32 mid_way_len = anim_time - last_time; 
  f32 frame_diff  = next_time - last_time;

  return mid_way_len / frame_diff;
}

static void animate_joint(const f32& current_time, Joint* joint) {
  // Find the next position

  VectorAnimSample next_position = joint->position_samples[joint->current_position_sample];
  sizei next_position_sample     = joint->current_position_sample;

  for(sizei i = joint->current_position_sample; i < joint->position_samples.size(); i++) {
    if(joint->position_samples[i].time > current_time) {
      next_position         = joint->position_samples[i];
      next_position_sample = i; 

      break;
    }
  }

  // Find the next rotation

  QuatAnimSample next_rotation = joint->rotation_samples[joint->current_rotation_sample];
  sizei next_rotation_sample   = joint->current_rotation_sample;

  for(sizei i = joint->current_rotation_sample; i < joint->rotation_samples.size(); i++) {
    if(joint->rotation_samples[i].time > current_time) {
      next_rotation        = joint->rotation_samples[i];
      next_rotation_sample = i; 

      break;
    }
  }

  // Find the next scale

  VectorAnimSample next_scale = joint->scale_samples[joint->current_scale_sample];
  sizei next_scale_sample     = joint->current_scale_sample;

  for(sizei i = joint->current_scale_sample; i < joint->scale_samples.size(); i++) {
    if(joint->scale_samples[i].time > current_time) {
      next_scale        = joint->scale_samples[i];
      next_scale_sample = i; 

      break;
    }
  }

  // Interpolate between the two samples

  if(joint->position_samples.size() > 1) {
    f32 pos_delta = get_scale_factor(joint->position_samples[joint->current_position_sample].time, next_position.time, current_time);
    Vec3 lerp_pos = vec3_lerp(joint->position_samples[joint->current_position_sample].value, next_position.value, pos_delta);
    
    transform_translate(joint->current_transform, lerp_pos);
  }

  if(joint->rotation_samples.size() > 1) {
    f32 rot_delta = get_scale_factor(joint->rotation_samples[joint->current_rotation_sample].time, next_rotation.time, current_time);
    Quat lerp_rot = quat_slerp(joint->rotation_samples[joint->current_rotation_sample].value, next_rotation.value, rot_delta);
    
    transform_rotate(joint->current_transform, lerp_rot);
  }
  
  if(joint->scale_samples.size() > 1) {
    f32 scale_delta = get_scale_factor(joint->scale_samples[joint->current_scale_sample].time, next_scale.time, current_time);
    Vec3 lerp_scale = vec3_lerp(joint->scale_samples[joint->current_scale_sample].value, next_scale.value, scale_delta);
    
    transform_scale(joint->current_transform, lerp_scale);
  }

  // Set the indices for next frame 

  joint->current_position_sample = next_position_sample;
  joint->current_rotation_sample = next_rotation_sample;
  joint->current_scale_sample    = next_scale_sample;
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Animator functions

void animator_create(Animator* animator, const ResourceID& animation) {
  animator->animation = resources_get_animation(animation);

  animator->current_time = 0.0f;
  animator->is_looping   = false;
  animator->is_animating = true;
}

void animator_animate(Animator& animator, const f32& dt) {
  if(!animator.is_animating) {
    return;
  }
   
  animator.current_time += (animator.animation->frame_rate * dt);
  if((animator.current_time >= animator.animation->duration) && !animator.is_looping) {
    animator.is_animating = false; 
    animator.current_time = 0.0f;

    return;
  }
  animator.current_time = fmod(animator.current_time, animator.animation->duration);

  for(sizei i = 0; i < animator.animation->joints.size(); i++) {
    Joint* joint = animator.animation->joints[i];
    animate_joint(animator.current_time, joint); 
  }
}

/// Animator functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
