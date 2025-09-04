#pragma once

#include "nikola_base.h"
#include "nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Physics ***

///---------------------------------------------------------------------------------------------------------------------
/// Consts 

const u32 PHYSICS_DEFAULT_MAX_BODIES              = 24000;
const u32 PHYSICS_DEFAULT_MAX_CONTACT_CONSTRAINTS = 10240;

const u32 COLLIDER_ID_INVALID = 42560;

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
/// PhysicsBodyID
struct PhysicsBodyID {
  u32 _id;
};
/// PhysicsBodyID
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ColliderID
struct ColliderID {
  u32 _id;
};
/// ColliderID
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

  void* user_data = nullptr;
  bool is_sensor  = false;    
};
/// PhysicsBodyDesc
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

NIKOLA_API void physics_world_add_body(const PhysicsBodyID& body_id, const bool is_active);

NIKOLA_API PhysicsBodyID physics_world_create_and_add_body(const PhysicsBodyDesc& desc, const bool is_active);

NIKOLA_API void physics_world_prepare_bodies(PhysicsBodyID* bodies, const sizei bodies_count);

NIKOLA_API void physics_world_finalize_bodies(PhysicsBodyID* bodies, const sizei bodies_count, const bool is_active);

NIKOLA_API void physics_world_abort_bodies(PhysicsBodyID* bodies, const sizei bodies_count);

NIKOLA_API void physics_world_set_gravity(const Vec3& gravity);

NIKOLA_API void physics_world_toggle_paused();

/// Physics world functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Physics body functions

NIKOLA_API Transform physics_body_get_transform(const PhysicsBodyID& body_id);

/// Physics body functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Collider functions

NIKOLA_API ColliderID collider_create(const BoxColliderDesc& desc);

NIKOLA_API ColliderID collider_create(const SphereColliderDesc& desc);

NIKOLA_API ColliderID collider_create(const CapsuleColliderDesc& desc);

/// Collider functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Physics ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
