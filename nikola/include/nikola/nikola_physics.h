#pragma once

#include "nikola_base.h"
#include "nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Physics ***

// A forward declaration needed here
struct PhysicsBody;

///---------------------------------------------------------------------------------------------------------------------
/// Collider
struct Collider {
  Vec3 half_size;
  Vec3 min, max; 
  
  bool is_trigger;

  PhysicsBody* body = nullptr;
};
/// Collider
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// CollisionPoint
struct CollisionPoint {
  Vec3 point_a; 
  Vec3 point_b;

  Vec3 normal; 
  f32 depth; 

  bool has_collided = false;
};
/// CollisionPoint
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RayIntersection
struct RayIntersection {
  Vec3 point;
  
  f32 distance; 
  bool has_intersected;
};
/// RayIntersection
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Ray
struct Ray {
  Vec3 position; 
  Vec3 direction;
};
/// Ray
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBodyDesc
struct PhysicsBodyDesc {
  Vec3 position   = Vec3(0.0f); 
  
  f32 mass        = 1.0f;
  f32 restitution = 0.5f;
  bool is_dynamic = true;
  
  void* user_data = nullptr;
};
/// PhysicsBodyDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBody
struct PhysicsBody {
  Transform transform = {};
  Collider collider   = {};

  Vec3 force, torque;
  Vec3 linear_velocity, angular_velocity;

  Mat3 inertia_tensor, inverse_inertia_tensor;

  f32 mass, inverse_mass, restitution;
  bool is_awake, is_dynamic;

  void* user_data;
};
/// PhysicsBody
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// OnCollisionFunc

using OnCollisionFunc = void(*)(PhysicsBody* body_a, PhysicsBody* body_b);

/// OnCollisionFunc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Collider functions

NIKOLA_API CollisionPoint collider_check_collision(const Collider& coll_a, const Transform& trans_a, const Collider& coll_b, const Transform& trans_b);

NIKOLA_API bool collider_check_collision(const Vec3& pos_a, const Vec3& size_a, const Vec3& pos_b, const Vec3& size_b);

/// Collider functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Ray functions

NIKOLA_API RayIntersection ray_check_intersection(const Ray& ray, const Transform& trans, Collider& collider);

/// Ray functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBody functions

NIKOLA_API PhysicsBody* physics_body_create(const PhysicsBodyDesc& desc);

NIKOLA_API void physics_body_destroy(PhysicsBody* body);

NIKOLA_API void physics_body_add_collider(PhysicsBody* body, const Vec3& collider_size, const bool trigger);

NIKOLA_API void physics_body_apply_force_at(PhysicsBody* body, const Vec3& force, const Vec3& pos);

NIKOLA_API void physics_body_apply_linear_force(PhysicsBody* body, const Vec3& force);

NIKOLA_API void physics_body_apply_angular_force(PhysicsBody* body, const Vec3& force);

NIKOLA_API void physics_body_apply_linear_impulse(PhysicsBody* body, const Vec3& force);

NIKOLA_API void physics_body_apply_angular_impulse(PhysicsBody* body, const Vec3& force);

/// PhysicsBody functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Physics world functions

NIKOLA_API void physics_world_init(const Vec3& gravity);

NIKOLA_API void physics_world_shutdown();

NIKOLA_API void physics_world_set_gravity(const Vec3& gravity);

NIKOLA_API Vec3& physics_world_get_gravity();

NIKOLA_API void physics_world_set_callback(const OnCollisionFunc& callback);

NIKOLA_API void physics_world_step(const f64 timestep);

NIKOLA_API void physics_world_add_body(PhysicsBody* body);

/// Physics world functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Physics ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
