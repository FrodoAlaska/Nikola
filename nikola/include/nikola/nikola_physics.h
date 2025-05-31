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
typedef u32 ColliderID;
/// ColliderID
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBodyType
enum PhysicsBodyType {
  /// Indicates to a physics body to be static, 
  /// completely unmoving, even by external forces.
  PHYSICS_BODY_STATIC    = 20 << 0, 

  /// Indicates to a physics body to be dynmaic. 
  /// Affected by all forces. 
  PHYSICS_BODY_DYNAMIC   = 20 << 1, 

  /// Indicates to a physics body to be kinematic. 
  /// Not affected by external forces, but movable.
  PHYSICS_BODY_KINEMATIC = 20 << 2, 
};
/// PhysicsBodyType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBodyDesc
struct PhysicsBodyDesc {
  /// The initial position of the physics body.
  Vec3 position; 

  /// The type of the physics body.
  PhysicsBodyType type;

  /// The initial rotation axis of the physics body. 
  ///
  /// @NOTE: This is set to `Vec3(0.0f)` by default.
  Vec3 rotation_axis = Vec3(0.0f);

  /// The initial rotation angle (in radians) of the physics body. 
  ///
  /// @NOTE: This is set to `0.0f` by default.
  f32 rotation_angle = 0.0f;

  /// The initial wake state of the body. 
  ///
  /// @NOTE: This is set to `true` by default.
  bool is_awake      = true; 
  
  /// The user data the physics body will cary for later use. 
  ///
  /// @NOTE: The value this pointer will point to MUST 
  /// have a long lifetime. It should not just live on the stack 
  /// in a limited scope.
  void* user_data    = nullptr;
};
/// PhysicsBodyDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ColliderDesc
struct ColliderDesc {
  /// The initial _offset_ of the collider from 
  /// the attached body's position. This can 
  /// be left as `Vec3(0.0f)` if you do not 
  /// wish to offset the collider.
  Vec3 position; 

  /// The initial extents (or size) of the collider.
  Vec3 extents; 

  /// The friction that will be applied to 
  /// two colliding bodies. 
  ///
  /// @NOTE: This is set to `0.4f` by default.
  f32 friction    = 0.4f; 
  
  /// The initial restitution or "bounciness" of the collider. 
  ///
  /// @NOTE: This is set to `0.2f` by default.
  f32 restitution = 0.2f;
  
  /// The initial density of the collider. 
  ///
  /// @NOTE: This is set to `1.0f` by default.
  f32 density     = 1.0f;

  /// Indicates that the collider should be a sensor or not. 
  /// When a collider is set as a "sensor", it will only 
  /// detect collisions with other colliders but not _resolve_ them. 
  ///
  /// @NOTE: This is set to `false` by default.
  bool is_sensor  = false;
  
  /// The user data the collider will cary for later use. 
  ///
  /// @NOTE: The value this pointer will point to MUST 
  /// have a long lifetime. It should not just live on the stack 
  /// in a limited scope.
  void* user_data = nullptr;
};
/// ColliderDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// CollisionPoint
struct CollisionPoint {
  /// Both of the bodies invloved in the collision.
  PhysicsBodyID body_a, body_b; 

  /// The exact colliders invloved in the collision.
  ColliderID coll_a, coll_b;
};
/// CollisionPoint
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Ray
struct Ray {
  /// The position or start of the ray.
  Vec3 position; 

  /// The direction the ray is pointing towards.
  ///
  /// @NOTE: The distance is assumed to be infinite 
  Vec3 direction;
};
/// Ray
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RayIntersection
struct RayIntersection {
  /// The point at which the ray intersected at.
  ///
  /// @NOTE: This is set in world space.
  Vec3 point; 

  /// The surface normal of the intersection point.
  Vec3 normal; 

  /// The exact time of impact of the intersection.
  f32 time_of_impact;

  /// Indicates whether the ray has intersected with a collider or not.
  bool has_intersected;
};
/// RayIntersection
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// OnCollisionFunc

/// A callback to be invoked everytime a collision occurs, passing in the 
/// collision information in `collision`.
using OnCollisionFunc = void(*)(const CollisionPoint& collision);

/// OnCollisionFunc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// OnRayIntersectionFunc

/// A callback to be invoked everytime a ray intersection occurs with a collider, passing in the 
/// ray that started the intersection, intersection information, and the `collider`.
using OnRayIntersectionFunc = void(*)(const Ray& ray, const RayIntersection& info, const ColliderID& collider);

/// OnRayIntersectionFunc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Physics world functions

/// Initialize the physics world with the initial `gravity` and the `timestep` that 
/// will be used when updating the physics world. 
///
/// @NOTE: Varying timesteps is not supported.
NIKOLA_API void physics_world_init(const Vec3& gravity, const f32 timestep);

/// Shutdown/de-initialize the physics world, reclaiming any memory 
/// allocated in the process. 
///
/// @NOTE: This function will also remove any and all bodies and colliders 
/// currently in the world.
NIKOLA_API void physics_world_shutdown();

/// Step through the simulation with the initial timestep given at initialization.
NIKOLA_API void physics_world_step();

/// Set the gravity of the physics world to the given `gravity`.
NIKOLA_API void physics_world_set_gravity(const Vec3& gravity);

/// Set the iterations count of the physics world to the given `iterations`.
///
/// @NOTE: Iteration counts between 5 to 20 are the MOST ideal.
NIKOLA_API void physics_world_set_iterations_count(const i32 iterations);

/// Set collision callbacks both at the beginning of collision (`begin_func`) and at the end of collision (`end_func`).
NIKOLA_API void physics_world_set_collision_callback(const OnCollisionFunc& begin_func, const OnCollisionFunc& end_func);

/// Shoot a raycast supplied by `ray` through the objects in the world and call `ray_func` 
/// if any successful intersections occur.
NIKOLA_API void physics_world_check_raycast(const Ray& ray, const OnRayIntersectionFunc& ray_func);

/// Retrieve the current gravity of the physics world.
NIKOLA_API Vec3 physics_world_get_gravity();

/// Physics world functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBody functions

/// Create a new physics body in the world given the information in `desc` 
/// and return a `PhysicsBodyID` identifier.
NIKOLA_API PhysicsBodyID physics_body_create(const PhysicsBodyDesc& desc);

/// Destroy and remove the physics body `id`, reclaiming any allocated memory. 
///
/// @NOTE: Unless explicitly necessary, this is not required to be called since 
/// the physics world will remove all the bodies on de-initialization.
NIKOLA_API void physics_body_destroy(PhysicsBodyID& id);

/// Add a new collider to the given `id` body with the information supplied at 
/// `desc`, returning back a `ColliderID` identifier.
NIKOLA_API ColliderID physics_body_add_collider(PhysicsBodyID& id, const ColliderDesc& desc);

/// Remove the given `coll_id` from the body `id`.
///
/// @NOTE: Unless explicitly necessary, this is not required to be called since 
/// the physics world will remove all colliders on de-initialization.
NIKOLA_API void physics_body_remove_collider(PhysicsBodyID& id, const ColliderID& coll_id);

/// Apply a linear force `force` to the body `id`.
NIKOLA_API void physics_body_apply_force(PhysicsBodyID& id, const Vec3& force);

/// Apply a linear force `force` at the world position `point` to the body `id`.
NIKOLA_API void physics_body_apply_force_at(PhysicsBodyID& id, const Vec3& force, const Vec3& point);

/// Apply an impulse `impulse` to the body `id`.
NIKOLA_API void physics_body_apply_impulse(PhysicsBodyID& id, const Vec3& impulse);

/// Apply an impulse `impulse` at the world position `point` to the body `id`.
NIKOLA_API void physics_body_apply_impulse_at(PhysicsBodyID& id, const Vec3& impulse, const Vec3& point);

/// Apply a rotational torque force `torque`.
NIKOLA_API void physics_body_apply_torque(PhysicsBodyID& id, const Vec3& torque);

/// Set the current position of body `id` to `pos`. 
///
/// @NOTE: This is not recommended to be used unless necessary. 
/// The simulation will alter the position in the next step.
NIKOLA_API void physics_body_set_position(PhysicsBodyID& id, const Vec3& pos);

/// Set the current rotation of body `id` around `axis` by `angle` in radians. 
///
/// @NOTE: This is not recommended to be used unless necessary. 
/// The simulation will alter the rotation in the next step.
NIKOLA_API void physics_body_set_rotation(PhysicsBodyID& id, const Vec3& axis, const f32 angle);

/// Set the linear velocity of body `id` to `vel`.
NIKOLA_API void physics_body_set_linear_velocity(PhysicsBodyID& id, const Vec3& vel);

/// Set the angular (rotational) velocity of body `id` to `vel`.
NIKOLA_API void physics_body_set_angular_velocity(PhysicsBodyID& id, const Vec3& vel);

/// Set the awake state of body `id` to `awake`.
NIKOLA_API void physics_body_set_awake(PhysicsBodyID& id, const bool awake);

/// Retrieve the current position of body `id`.
NIKOLA_API Vec3& physics_body_get_position(const PhysicsBodyID& id);

/// Retrieve the current rotation of body `id` represented as a `Quat`.
NIKOLA_API Quat& physics_body_get_quaternion(const PhysicsBodyID& id);

/// Retrieve the current transform of body `id`.
NIKOLA_API Transform& physics_body_get_transform(const PhysicsBodyID& id);

/// Retrieve the current linear velocity of body `id`.
NIKOLA_API Vec3& physics_body_get_linear_velocity(const PhysicsBodyID& id);

/// Retrieve the current angular (rotational) velocity of body `id`.
NIKOLA_API Vec3& physics_body_get_angular_velocity(const PhysicsBodyID& id);

/// Retrieve the current awake state of body `id`.
NIKOLA_API bool physics_body_is_awake(const PhysicsBodyID& id);

/// Retrieve the user data of body `id`.
NIKOLA_API void* physics_body_get_user_data(const PhysicsBodyID& id);

/// PhysicsBody functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Collider functions

/// Shoot a raycast supplied by `ray` at the collider `id` and 
/// return the intersection information in `RayIntersection`.
NIKOLA_API RayIntersection collider_check_raycast(const ColliderID& id, const Ray& ray);

/// Set extents of collider `id` to `extents`.
NIKOLA_API void collider_set_extents(ColliderID& id, const Vec3& extents);

/// Set friction of collider `id` to `friction`.
NIKOLA_API void collider_set_friction(ColliderID& id, const f32 friction);

/// Set restitution of collider `id` to `restitution`.
NIKOLA_API void collider_set_restitution(ColliderID& id, const f32 restitution);

/// Set density of collider `id` to `density`.
NIKOLA_API void collider_set_density(ColliderID& id, const f32 density);

/// Set a user-defined `user_data` of collider `id`.
NIKOLA_API void collider_set_user_data(ColliderID& id, const void* user_data);

/// Retrieve the current attached body of collider `id`.
NIKOLA_API PhysicsBodyID& collider_get_attached_body(const ColliderID& id);

/// Retrieve the current extents of collider `id`.
NIKOLA_API Vec3& collider_get_extents(const ColliderID& id);

/// Retrieve the current friction of collider `id`.
NIKOLA_API f32 collider_get_friction(const ColliderID& id);

/// Retrieve the current restitution of collider `id`.
NIKOLA_API f32 collider_get_restitution(const ColliderID& id);

/// Retrieve the current density of collider `id`.
NIKOLA_API f32 collider_get_density(const ColliderID& id);

/// Retrieve the current sensor state of collider `id`.
NIKOLA_API bool collider_get_sensor(const ColliderID& id);

/// Retrieve the user data of collider `id`.
NIKOLA_API void* collider_get_user_data(const ColliderID& id);

/// Retrieve the local transform of collider `id`.
NIKOLA_API Transform& collider_get_local_transform(const ColliderID& id);

/// Retrieve the world transform of collider `id`.
///
/// @NOTE: This is usually the multiplied transform of the collider's 
/// attached body and its local transform.
NIKOLA_API Transform collider_get_world_transform(const ColliderID& id);

/// Collider functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Physics ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
