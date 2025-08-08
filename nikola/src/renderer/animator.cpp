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

  f32 scale_factor = mid_way_len / frame_diff;

  return clamp_float(scale_factor, 0.0f, 1.0f);
}

static void animate_joint(const f32& current_time, Joint* joint) {
  // Find the next position

  sizei next_position_sample = joint->current_position_sample;

  for(sizei i = 0; i < joint->position_samples.size(); i++) {
    if(joint->position_samples[i].time > current_time) {
      next_position_sample = i; 
      break;
    }
  }

  // Find the next rotation

  sizei next_rotation_sample = joint->current_rotation_sample;

  for(sizei i = 0; i < joint->rotation_samples.size(); i++) {
    if(joint->rotation_samples[i].time > current_time) {
      next_rotation_sample = i; 
      break;
    }
  }

  // Find the next scale

  sizei next_scale_sample = joint->current_scale_sample;

  for(sizei i = 0; i < joint->scale_samples.size(); i++) {
    if(joint->scale_samples[i].time > current_time) {
      next_scale_sample = i; 
      break;
    }
  }

  // Interpolate between two position samples (if there is a change)

  Vec3 next_position = joint->position_samples[next_position_sample].value;

  if(next_position_sample != joint->current_position_sample) {
    VectorAnimSample previous_sample = joint->position_samples[joint->current_position_sample]; 
    VectorAnimSample next_sample     = joint->position_samples[next_position_sample]; 

    f32 delta     = get_scale_factor(previous_sample.time, next_sample.time, current_time);
    next_position = vec3_lerp(previous_sample.value, next_sample.value, delta);

    joint->current_position_sample = next_position_sample;
  }

  transform_translate(joint->current_transform, next_position);
  
  // Interpolate between two rotation samples (if there is a change)

  Quat next_rotation = joint->rotation_samples[next_rotation_sample].value;

  if(next_rotation_sample != joint->current_rotation_sample) {
    QuatAnimSample previous_sample = joint->rotation_samples[joint->current_rotation_sample]; 
    QuatAnimSample next_sample     = joint->rotation_samples[next_rotation_sample]; 

    f32 delta     = get_scale_factor(previous_sample.time, next_sample.time, current_time);
    next_rotation = quat_normalize(quat_slerp(previous_sample.value, next_sample.value, delta));

    joint->current_rotation_sample = next_rotation_sample;
  }
    
  transform_rotate(joint->current_transform, next_rotation);
  
  // Interpolate between two scale samples (if there is a change)

  Vec3 next_scale = joint->scale_samples[next_scale_sample].value;

  if(next_scale_sample != joint->current_scale_sample) {
    VectorAnimSample previous_sample = joint->scale_samples[joint->current_scale_sample]; 
    VectorAnimSample next_sample     = joint->scale_samples[next_scale_sample]; 

    f32 delta  = get_scale_factor(previous_sample.time, next_sample.time, current_time);
    next_scale = vec3_lerp(previous_sample.value, next_sample.value, delta);

    joint->current_scale_sample = next_scale_sample;
  }
    
  transform_scale(joint->current_transform, next_scale);
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Animator functions

void animator_create(Animator* animator, const ResourceID& animation) {
  animator->animation_id = animation;

  animator->current_time = 0.0f;
  animator->is_looping   = true;
  animator->is_animating = true;
}

void animator_animate(Animator& animator, const f32& dt) {
  if(!animator.is_animating) {
    return;
  }

  Animation* animation = resources_get_animation(animator.animation_id); // For easier visualization

  // Timing calculations. Making sure it resets when it needs to, and etc...
   
  animator.current_time += (animation->frame_rate * dt);
  if((animator.current_time >= animation->duration) && !animator.is_looping) {
    animator.is_animating = false; 
    animator.current_time = 0.0f;

    return;
  }
  animator.current_time = fmod(animator.current_time, animation->duration);
 
  // Animating each joint, building a parent-child array of transforms

  for(sizei i = 0; i < animation->joints.size(); i++) {
    Joint* joint = animation->joints[i];
    animate_joint(animator.current_time, joint); 

    Mat4 parent_transform = Mat4(1.0f);
    if(joint->parent_index != -1) {
      parent_transform = animation->skinning_palette[joint->parent_index];
    }

    animation->skinning_palette[i] = (parent_transform * joint->current_transform.transform);
  }
}

void animator_set_animation(Animator& animator, const ResourceID& animation) {
  animator.animation_id = animation;
  animator.current_time = 0.0f; 
}

/// Animator functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
