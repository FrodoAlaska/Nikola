#pragma once

#include "nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Physics ***

///---------------------------------------------------------------------------------------------------------------------
/// Consts 

const u32 PHYSICS_DEFAULT_MAX_BODIES              = 24000;
const u32 PHYSICS_DEFAULT_MAX_CONTACT_CONSTRAINTS = 10240;

const u32 PHYSICS_ID_INVALID = ((u32)-1);

/// Consts 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBroadPhaseLayer
enum PhysicsBroadPhaseLayer {
  PHYSICS_BROAD_PHASE_LAYER_STATIC  = 0,
  PHYSICS_BROAD_PHASE_LAYER_DYNAMIC,

  PHYSICS_BROAD_PHASE_LAYERS_MAX,
};
/// PhysicsBroadPhaseLayer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsObjectLayer
enum PhysicsObjectLayer {
  // Collides with both the STATIC and DYNAMIC broad phase layers.
  PHYSICS_OBJECT_LAYER_0 = 0,
  PHYSICS_OBJECT_LAYER_1,
  PHYSICS_OBJECT_LAYER_2,
  PHYSICS_OBJECT_LAYER_3,
  PHYSICS_OBJECT_LAYER_4,
  PHYSICS_OBJECT_LAYER_5,

  // Collides _only_ with the STATIC broad phase layer.
  PHYSICS_OBJECT_LAYER_6,
  PHYSICS_OBJECT_LAYER_7,
  
  // Collides _only_ with the DYNAMIC broad phase layer.
  PHYSICS_OBJECT_LAYER_8,
  PHYSICS_OBJECT_LAYER_9,

  PHYSICS_OBJECT_LAYERS_MAX,
};
/// PhysicsObjectLayer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBodyType
enum PhysicsBodyType {
  /// Indicates to a physics body to be static, 
  /// completely unmoving, even by external forces.
  PHYSICS_BODY_STATIC, 

  /// Indicates to a physics body to be dynmaic. 
  /// Affected by all forces. 
  PHYSICS_BODY_DYNAMIC, 

  /// Indicates to a physics body to be kinematic. 
  /// Not affected by external forces, but movable.
  PHYSICS_BODY_KINEMATIC, 
};
/// PhysicsBodyType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ColliderType
enum ColliderType {
  COLLIDER_BOX = 0, 
  COLLIDER_SPHERE, 
  COLLIDER_CAPSULE,
};
/// ColliderType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GroundState
enum GroundState {
  GROUND_STATE_ON_GROUND = 0, 
  GROUND_STATE_ON_STEEP_GROUND, 
  GROUND_STATE_NOT_SUPPORTED, 
  GROUND_STATE_IN_AIR,
};
/// GroundState
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBodyID
struct PhysicsBodyID {
  u32 _id = PHYSICS_ID_INVALID;
};
/// PhysicsBodyID
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ColliderID
struct ColliderID {
  ColliderType _type;
  u32 _id = PHYSICS_ID_INVALID;
};
/// ColliderID
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// CharacterID
struct CharacterID {
  u32 _id = PHYSICS_ID_INVALID;
};
/// CharacterID
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// CollisionData
struct CollisionData {
  PhysicsBodyID body1_id;
  PhysicsBodyID body2_id; 

  Vec3 base_offset      = Vec3(0.0f);
  Vec3 normal           = Vec3(0.0f);
  f32 penetration_depth = 0.0f;
};
/// CollisionData
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RayCastDesc
struct RayCastDesc {
  Vec3 origin    = Vec3(0.0f); 
  Vec3 direction = Vec3(0.0f);
  f32 distance   = 1.0f;

  PhysicsBroadPhaseLayer broad_phase_layer = PHYSICS_BROAD_PHASE_LAYER_DYNAMIC;
  PhysicsObjectLayer object_layer          = PHYSICS_OBJECT_LAYER_0;
};
/// RayCastDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RayCastResult
struct RayCastResult {
  PhysicsBodyID body_id = {};
  Vec3 point            = Vec3(0.0f);

  bool has_hit = false;
};
/// RayCastResult
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsWorldDesc
struct PhysicsWorldDesc {
  sizei allocater_size = MiB(10);

  u32 max_jobs     = 256; 
  u32 max_barriers = 16;
  i32 max_threads  = -1;
  u32 max_mutexes  = 0;

  u32 max_bodies              = PHYSICS_DEFAULT_MAX_BODIES;
  u32 max_body_pairs          = PHYSICS_DEFAULT_MAX_BODIES;
  u32 max_contact_constraints = PHYSICS_DEFAULT_MAX_CONTACT_CONSTRAINTS;

  Vec3 gravity = Vec3(0.0f, -9.81f, 0.0f);
};
/// PhysicsWorldDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBodyDesc
struct PhysicsBodyDesc {
  Vec3 position;
  Quat rotation;

  PhysicsBodyType type;
  PhysicsObjectLayer layers;

  ColliderID collider_id;

  f32 restitution    = 0.1f;
  f32 friction       = 0.0f;
  f32 gravity_factor = 1.0f;

  u64 user_data  = 0;
  bool is_sensor = false;    
};
/// PhysicsBodyDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// CharacterBodyDesc
struct CharacterBodyDesc {
  Vec3 position;
  Quat rotation;

  PhysicsObjectLayer layer; 

  f32 max_slope_angle = (50.0f * DEG2RAD);
  f32 mass            = 80.0f;
  f32 friction        = 0.5f;
  f32 gravity_factor  = 1.0f;

  Vec3 up_axis           = Vec3(0.0f, 1.0f, 0.0f);
  ColliderID collider_id = {};
  u64 user_data          = 0;
};
/// CharacterBodyDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// BoxColliderDesc
struct BoxColliderDesc {
  Vec3 half_size; 
};
/// BoxColliderDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// SphereColliderDesc
struct SphereColliderDesc {
  f32 radius; 
};
/// SphereColliderDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// CapsuleColliderDesc
struct CapsuleColliderDesc {
  f32 half_height;
  f32 radius; 
};
/// CapsuleColliderDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Physics world functions

NIKOLA_API void physics_world_init(const PhysicsWorldDesc& desc);

NIKOLA_API void physics_world_shutdown();

NIKOLA_API void physics_world_step(const f32 delta_time, const i32 collision_steps = 1);

NIKOLA_API PhysicsBodyID physics_world_create_body(const PhysicsBodyDesc& desc);

NIKOLA_API void physics_world_add_body(const PhysicsBodyID& body_id, const bool is_active = true);

NIKOLA_API void physics_world_add_character(const CharacterID& char_id, const bool is_active = true);

NIKOLA_API PhysicsBodyID physics_world_create_and_add_body(const PhysicsBodyDesc& desc, const bool is_active = true);

NIKOLA_API void physics_world_prepare_bodies(PhysicsBodyID* bodies, const sizei bodies_count);

NIKOLA_API const bool physics_world_finalize_bodies(PhysicsBodyID* bodies, const sizei bodies_count, const bool is_active = true);

NIKOLA_API const bool physics_world_abort_bodies(PhysicsBodyID* bodies, const sizei bodies_count);

NIKOLA_API const bool physics_world_cast_ray(const RayCastDesc& cast_desc);

NIKOLA_API void physics_world_set_safe_mode(const bool safe);

NIKOLA_API void physics_world_set_gravity(const Vec3& gravity);

NIKOLA_API Vec3 physics_world_get_gravity();

NIKOLA_API void physics_world_toggle_paused();

NIKOLA_API const bool physics_world_is_paused();

NIKOLA_API void physics_world_draw();

/// Physics world functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Physics body functions

NIKOLA_API void physics_body_set_position(PhysicsBodyID& body_id, const Vec3 position, const bool activate = true);

NIKOLA_API void physics_body_set_rotation(PhysicsBodyID& body_id, const Quat rotation, const bool activate = true);

NIKOLA_API void physics_body_set_rotation(PhysicsBodyID& body_id, const Vec3 axis, const f32 angle, const bool activate = true);

NIKOLA_API void physics_body_set_transform(PhysicsBodyID& body_id, const Transform& transform, const bool activate = true);

NIKOLA_API void physics_body_set_linear_velocity(PhysicsBodyID& body_id, const Vec3 velocity);

NIKOLA_API void physics_body_set_angular_velocity(PhysicsBodyID& body_id, const Vec3 velocity);

NIKOLA_API void physics_body_set_active(PhysicsBodyID& body_id, const bool active);

NIKOLA_API void physics_body_set_user_data(PhysicsBodyID& body_id, const u64 user_data);

NIKOLA_API void physics_body_set_layer(PhysicsBodyID& body_id, const PhysicsObjectLayer layer);

NIKOLA_API void physics_body_set_restitution(PhysicsBodyID& body_id, const f32 restitution);

NIKOLA_API void physics_body_set_friction(PhysicsBodyID& body_id, const f32 friction);

NIKOLA_API void physics_body_set_gravity_factor(PhysicsBodyID& body_id, const f32 factor);

NIKOLA_API void physics_body_set_type(PhysicsBodyID& body_id, const PhysicsBodyType type);

NIKOLA_API void physics_body_set_collider(PhysicsBodyID& body_id, const ColliderID& collider_id, const bool activate = true);

NIKOLA_API void physics_body_apply_linear_velocity(PhysicsBodyID& body_id, const Vec3 velocity);

NIKOLA_API void physics_body_apply_force(PhysicsBodyID& body_id, const Vec3 force);

NIKOLA_API void physics_body_apply_force_at(PhysicsBodyID& body_id, const Vec3 force, const Vec3 point);

NIKOLA_API void physics_body_apply_torque(PhysicsBodyID& body_id, const Vec3 torque);

NIKOLA_API void physics_body_apply_impulse(PhysicsBodyID& body_id, const Vec3 impulse);

NIKOLA_API void physics_body_apply_impulse_at(PhysicsBodyID& body_id, const Vec3 impulse, const Vec3 point);

NIKOLA_API void physics_body_apply_angular_impulse(PhysicsBodyID& body_id, const Vec3 impulse);

NIKOLA_API const Vec3 physics_body_get_position(const PhysicsBodyID& body_id);

NIKOLA_API const Vec3 physics_body_get_com_position(const PhysicsBodyID& body_id);

NIKOLA_API const Quat physics_body_get_rotation(const PhysicsBodyID& body_id);

NIKOLA_API const Vec3 physics_body_get_linear_velocity(const PhysicsBodyID& body_id);

NIKOLA_API const Vec3 physics_body_get_angular_velocity(const PhysicsBodyID& body_id);

NIKOLA_API const bool physics_body_is_active(const PhysicsBodyID& body_id);

NIKOLA_API const u64 physics_body_get_user_data(const PhysicsBodyID& body_id);

NIKOLA_API const PhysicsObjectLayer physics_body_get_layer(const PhysicsBodyID& body_id);

NIKOLA_API const f32 physics_body_get_restitution(const PhysicsBodyID& body_id);

NIKOLA_API const f32 physics_body_get_friction(const PhysicsBodyID& body_id);

NIKOLA_API const f32 physics_body_get_gravity_factor(const PhysicsBodyID& body_id);

NIKOLA_API const PhysicsBodyType physics_body_get_type(const PhysicsBodyID& body_id);

NIKOLA_API Transform physics_body_get_transform(const PhysicsBodyID& body_id);

/// Physics body functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Collider functions

NIKOLA_API ColliderID collider_create(const BoxColliderDesc& desc);

NIKOLA_API ColliderID collider_create(const SphereColliderDesc& desc);

NIKOLA_API ColliderID collider_create(const CapsuleColliderDesc& desc);

NIKOLA_API const bool collider_set_scale(ColliderID& collider_id, const Vec3 scale);

NIKOLA_API const bool collider_set_offset(ColliderID& collider_id, const Vec3 offset);

/// Collider functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Character body functions

NIKOLA_API CharacterID character_body_create(const CharacterBodyDesc& desc);

NIKOLA_API void character_body_set_position(CharacterID& char_id, const Vec3 position);

NIKOLA_API void character_body_set_rotation(CharacterID& char_id, const Quat rotation);

NIKOLA_API void character_body_set_rotation(CharacterID& char_id, const Vec3 axis, const f32 angle);

NIKOLA_API void character_body_set_linear_velocity(CharacterID& char_id, const Vec3 velocity);

NIKOLA_API void character_body_set_layer(CharacterID& char_id, const PhysicsObjectLayer layer);

NIKOLA_API void character_body_set_slope_angle(CharacterID& char_id, const f32 max_slope_angle);

NIKOLA_API void character_body_set_collider(CharacterID& char_id, const ColliderID& collider_id, const f32 max_penetration_depth);

NIKOLA_API void character_body_activate(CharacterID& char_id);

NIKOLA_API const Vec3 character_body_get_position(const CharacterID& char_id);

NIKOLA_API const Vec3 character_body_get_com_position(const CharacterID& char_id);

NIKOLA_API const Quat character_body_get_rotation(const CharacterID& char_id);

NIKOLA_API const Vec3 character_body_get_linear_velocity(const CharacterID& char_id);

NIKOLA_API const PhysicsObjectLayer character_body_get_layer(const CharacterID& char_id);

NIKOLA_API const f32 character_body_get_slope_angle(const CharacterID& char_id);

NIKOLA_API Transform character_body_get_transform(const CharacterID& char_id);

NIKOLA_API const PhysicsBodyID character_body_get_body(const CharacterID& char_id);

NIKOLA_API GroundState character_body_query_ground_state(const CharacterID& char_id);

NIKOLA_API void character_body_apply_linear_velocity(CharacterID& char_id, const Vec3 velocity);

NIKOLA_API void character_body_apply_impulse(CharacterID& char_id, const Vec3 impulse);

NIKOLA_API const bool character_body_is_supported(const CharacterID& char_id);

NIKOLA_API const bool character_body_is_slope_too_steep(const CharacterID& char_id, const Vec3 surface_normal);

NIKOLA_API const Vec3 character_body_get_ground_position(const CharacterID& char_id);

NIKOLA_API const Vec3 character_body_get_ground_normal(const CharacterID& char_id);

NIKOLA_API const Vec3 character_body_get_ground_velocity(const CharacterID& char_id);

NIKOLA_API const PhysicsBodyID character_body_get_ground_body(const CharacterID& char_id);

NIKOLA_API const bool character_body_cast_ray(const CharacterID& char_id, const RayCastDesc& cast_desc);

/// Character body functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Physics ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
