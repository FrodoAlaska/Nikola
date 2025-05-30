#pragma once

#include "nikola_base.h"
#include "nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Physics ***

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBodyID
typedef u64 PhysicsBodyID;
/// PhysicsBodyID
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ColliderID
typedef u64 ColliderID;
/// ColliderID
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBodyType
enum PhysicsBodyType {
  PHYSICS_BODY_STATIC    = 20 << 0, 
  PHYSICS_BODY_DYNAMIC   = 20 << 1, 
  PHYSICS_BODY_KINEMATIC = 20 << 2, 
};
/// PhysicsBodyType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBodyDesc
struct PhysicsBodyDesc {
  Vec3 position; 
  PhysicsBodyType type;

  Vec3 rotation_axis = Vec3(0.0f);
  f32 rotation_angle = 0.0f;

  bool is_awake      = true; 
  void* user_data    = nullptr;
};
/// PhysicsBodyDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ColliderDesc
struct ColliderDesc {
  Vec3 position; 
  Vec3 extents; 

  f32 friction    = 0.4f; 
  f32 restitution = 0.2f;
  f32 density     = 1.0f;

  bool is_sensor  = false;
  void* user_data = nullptr;
};
/// ColliderDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// CollisionPoint
struct CollisionPoint {
  PhysicsBodyID body_a, body_b; 
  ColliderID coll_a, coll_b;
};
/// CollisionPoint
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// OnCollisionFunc
using OnCollisionFunc = void(*)(const CollisionPoint& collision);
/// OnCollisionFunc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Physics world functions

NIKOLA_API void physics_world_init(const Vec3& gravity, const f32 timestep);

NIKOLA_API void physics_world_shutdown();

NIKOLA_API void physics_world_step();

NIKOLA_API void physics_world_set_gravity(const Vec3& gravity);

NIKOLA_API void physics_world_set_iterations_count(const i32 iterations);

NIKOLA_API void physics_world_set_collision_callback(const OnCollisionFunc& begin_func, const OnCollisionFunc& end_func);

NIKOLA_API Vec3 physics_world_get_gravity();

/// Physics world functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBody functions

NIKOLA_API PhysicsBodyID physics_body_create(const PhysicsBodyDesc& desc);

NIKOLA_API void physics_body_destroy(PhysicsBodyID& id);

NIKOLA_API ColliderID physics_body_add_collider(PhysicsBodyID& id, const ColliderDesc& desc);

NIKOLA_API void physics_body_remove_collider(PhysicsBodyID& id, const ColliderID& coll_id);

NIKOLA_API void physics_body_apply_force(PhysicsBodyID& id, const Vec3& force);

NIKOLA_API void physics_body_apply_force_at(PhysicsBodyID& id, const Vec3& force, const Vec3& point);

NIKOLA_API void physics_body_apply_impulse(PhysicsBodyID& id, const Vec3& impulse);

NIKOLA_API void physics_body_apply_impulse_at(PhysicsBodyID& id, const Vec3& impulse, const Vec3& point);

NIKOLA_API void physics_body_apply_torque(PhysicsBodyID& id, const Vec3& torque);

NIKOLA_API void physics_body_set_position(PhysicsBodyID& id, const Vec3& pos);

NIKOLA_API void physics_body_set_rotation(PhysicsBodyID& id, const Vec3& axis, const f32 angle);

NIKOLA_API void physics_body_set_linear_velocity(PhysicsBodyID& id, const Vec3& vel);

NIKOLA_API void physics_body_set_angular_velocity(PhysicsBodyID& id, const Vec3& vel);

NIKOLA_API void physics_body_set_awake(PhysicsBodyID& id, const bool awake);

NIKOLA_API Vec3 physics_body_get_position(PhysicsBodyID& id);

NIKOLA_API Quat physics_body_get_quaternion(PhysicsBodyID& id);

NIKOLA_API Transform physics_body_get_transform(PhysicsBodyID& id);

NIKOLA_API Vec3 physics_body_get_linear_velocity(PhysicsBodyID& id);

NIKOLA_API Vec3 physics_body_get_angular_velocity(PhysicsBodyID& id);

NIKOLA_API bool physics_body_is_awake(PhysicsBodyID& id);

NIKOLA_API void* physics_body_get_user_data(PhysicsBodyID& id);

/// PhysicsBody functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Collider functions

NIKOLA_API void collider_set_friction(ColliderID& id, const f32 friction);

NIKOLA_API void collider_set_restitution(ColliderID& id, const f32 restitution);

NIKOLA_API void collider_set_density(ColliderID& id, const f32 density);

NIKOLA_API void collider_set_sensor(ColliderID& id, const bool sensor);

NIKOLA_API void collider_set_user_data(ColliderID& id, const void* user_data);

NIKOLA_API f32 collider_get_friction(ColliderID& id);

NIKOLA_API f32 collider_get_restitution(ColliderID& id);

NIKOLA_API f32 collider_get_density(ColliderID& id);

NIKOLA_API bool collider_get_sensor(ColliderID& id);

NIKOLA_API void* collider_get_user_data(ColliderID& id);

/// Collider functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Physics ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
