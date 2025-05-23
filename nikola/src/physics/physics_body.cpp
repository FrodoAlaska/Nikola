#include "nikola/nikola_physics.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Physics ***

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void build_cube_tensor(PhysicsBody* body, const Vec3& scale) {
  f32 x = (1.0f / 12.0f) * body->mass * ((scale.z * scale.z) + (scale.y * scale.y));
  f32 y = (1.0f / 12.0f) * body->mass * ((scale.x * scale.x) + (scale.z * scale.z));
  f32 z = (1.0f / 12.0f) * body->mass * ((scale.x * scale.x) + (scale.y * scale.y));

  body->inertia_tensor         = Mat3(x,    0.0f, 0.0f, 
                                      0.0f, y,    0.0f, 
                                      0.0f, 0.0f, z);
  body->inverse_inertia_tensor = body->inertia_tensor * -1.0f;
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBody functions

PhysicsBody* physics_body_create(const PhysicsBodyDesc& desc) {
  PhysicsBody* body = (PhysicsBody*)memory_allocate(sizeof(PhysicsBody));
  memory_zero(body, sizeof(PhysicsBody));

  transform_translate(body->transform, desc.position);

  body->force  = Vec3(0.0f);
  body->torque = Vec3(0.0f);

  body->linear_velocity  = Vec3(0.0f);
  body->angular_velocity = Vec3(0.0f);

  body->inertia_tensor         = Mat3(1.0f);
  body->inverse_inertia_tensor = Mat3(1.0f);

  body->mass         = desc.mass; 
  body->inverse_mass = body->mass * -1.0f;
  body->restitution  = desc.restitution;

  body->is_awake   = true;
  body->is_dynamic = desc.is_dynamic;

  body->user_data = desc.user_data;

  return body;
}

void physics_body_destroy(PhysicsBody* body) {
  if(!body) {
    return;
  }

  memory_free(body);
}

void physics_body_add_collider(PhysicsBody* body, const Vec3& collider_size, const bool trigger) {
  body->collider.half_size  = collider_size / 2.0f;
  body->collider.is_trigger = trigger;
  body->collider.body       = body;

  build_cube_tensor(body, body->collider.half_size);
}

void physics_body_apply_force_at(PhysicsBody* body, const Vec3& force, const Vec3& pos) {
  if(!body->is_dynamic) {
    return;
  }

  Vec3 local_pos = pos - body->transform.position;

  body->force  += force; 
  body->torque += vec3_cross(local_pos, -force); 
}

void physics_body_apply_linear_force(PhysicsBody* body, const Vec3& force) {
  if(!body->is_dynamic) {
    return;
  }
  
  body->force += force;
}

void physics_body_apply_angular_force(PhysicsBody* body, const Vec3& force) {
  if(!body->is_dynamic) {
    return;
  }
  
  body->torque += force;
}

void physics_body_apply_linear_impulse(PhysicsBody* body, const Vec3& force) {
  if(!body->is_dynamic) {
    return;
  }

  body->linear_velocity += force * body->inverse_mass;
}

void physics_body_apply_angular_impulse(PhysicsBody* body, const Vec3& force) {
  if(!body->is_dynamic) {
    return;
  }

  body->angular_velocity += body->inverse_inertia_tensor * force;
}

/// PhysicsBody functions
///---------------------------------------------------------------------------------------------------------------------


/// *** Physics ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
