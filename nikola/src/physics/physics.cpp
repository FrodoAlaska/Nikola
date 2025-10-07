#include "nikola/nikola_physics.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_math.h"
#include "nikola/nikola_containers.h"
#include "nikola/nikola_event.h"
#include "nikola/nikola_render.h"

#include <Jolt/Jolt.h>

#include <Jolt/RegisterTypes.h>

#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>

#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>

#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceTable.h>
#include <Jolt/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterTable.h>
#include <Jolt/Physics/Collision/ObjectLayerPairFilterTable.h>

#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>

#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Character/CharacterBase.h>

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include <Jolt/Geometry/AABox.h>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Physics ***

///---------------------------------------------------------------------------------------------------------------------
/// Defines

#define BODY_CHECK(body)     NIKOLA_ASSERT((body && body->handle), "Trying to access a physics body with an invalid ID")
#define COLLIDER_CHECK(coll) NIKOLA_ASSERT((coll && coll->handle), "Trying to access a collider with an invalid ID")
#define CHARACTER_CHECK(ch)  NIKOLA_ASSERT((ch && ch->handle), "Trying to access a character with an invalid ID")

/// Defines
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions declarations

static inline Vec3 jph_vec3_to_vec3(const JPH::Vec3& vec);
static inline Vec4 jph_vec4_to_vec4(const JPH::Vec4& vec);
static inline Quat jph_quat_to_quat(const JPH::Quat& quat);
static inline PhysicsBodyType jph_body_type_to_body_type(const JPH::EMotionType type);
static inline GroundState jph_ground_state_top_ground_state(const JPH::Character::EGroundState state);
static inline JPH::Vec3 vec3_to_jph_vec3(const Vec3& vec);
static inline JPH::Vec4 vec4_to_jph_vec4(const Vec4& vec);
static inline JPH::Quat quat_to_jph_quat(const Quat& quat);
static inline JPH::EMotionType body_type_to_jph_body_type(const PhysicsBodyType type);
static bool assert_impl(const char* expr, const char* msg, const char* file, JPH::uint line);

/// Private functions declarations
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBody
struct PhysicsBody {
  JPH::Body* handle  = nullptr;
  void* user_data    = nullptr;
  Collider* collider = nullptr;
};
/// PhysicsBody
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Character
struct Character {
  JPH::Ref<JPH::Character> handle = nullptr;
  void* user_data                 = nullptr;
  Collider* collider              = nullptr;
};
/// Character
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Collider
struct Collider {
  JPH::Ref<JPH::Shape> handle = nullptr;
  ColliderType type;

  Vec3 extents = Vec3(0.0f);
};
/// Collider
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NKBodyActivationListener  
class NKBodyActivationListener : public JPH::BodyActivationListener
{
public:
	void OnBodyActivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override {
    Event event = {
      .type = EVENT_PHYSICS_BODY_ACTIVATED,
      .body = reinterpret_cast<PhysicsBody*>(inBodyUserData), 
    };
    event_dispatch(event);
	}

	void OnBodyDeactivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override {
    Event event = {
      .type = EVENT_PHYSICS_BODY_DEACTIVATED,
      .body = reinterpret_cast<PhysicsBody*>(inBodyUserData), 
    };
    event_dispatch(event);
	}
};
/// NKBodyActivationListener  
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NKContactListener  
class NKContactListener : public JPH::ContactListener
{
public:
	JPH::ValidateResult OnContactValidate(const JPH::Body& inBody1, 
                                        const JPH::Body& inBody2, 
                                        JPH::RVec3Arg inBaseOffset, 
                                        const JPH::CollideShapeResult& inCollisionResult) override {
		return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	void OnContactAdded(const JPH::Body& inBody1, 
                      const JPH::Body& inBody2, 
                      const JPH::ContactManifold& inManifold, 
                      JPH::ContactSettings& ioSettings) override {
    CollisionData data = {
      .body1 = reinterpret_cast<PhysicsBody*>(inBody1.GetUserData()), 
      .body2 = reinterpret_cast<PhysicsBody*>(inBody2.GetUserData()), 

      .base_offset       = jph_vec3_to_vec3(inManifold.mBaseOffset), 
      .normal            = jph_vec3_to_vec3(inManifold.mWorldSpaceNormal),
      .penetration_depth = inManifold.mPenetrationDepth,
    };

    Event event = {
      .type           = EVENT_PHYSICS_CONTACT_ADDED,
      .collision_data = data,
    };
    event_dispatch(event);
	}

	void OnContactPersisted(const JPH::Body& inBody1, 
                          const JPH::Body& inBody2, 
                          const JPH::ContactManifold& inManifold, 
                          JPH::ContactSettings& ioSettings) override {
    CollisionData data = {
      .body1 = reinterpret_cast<PhysicsBody*>(inBody1.GetUserData()), 
      .body2 = reinterpret_cast<PhysicsBody*>(inBody2.GetUserData()), 

      .base_offset       = jph_vec3_to_vec3(inManifold.mBaseOffset), 
      .normal            = jph_vec3_to_vec3(inManifold.mWorldSpaceNormal),
      .penetration_depth = inManifold.mPenetrationDepth,
    };

    Event event = {
      .type           = EVENT_PHYSICS_CONTACT_PERSISTED,
      .collision_data = data,
    };
    event_dispatch(event);
	}

	void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override {
    // @TODO (Physics)
    // CollisionData data = {
    // };
    //
    // Event event = {
    //   .type           = EVENT_PHYSICS_CONTACT_REMOVED,
    //   .collision_data = data,
    // };
    // event_dispatch(event);
	}
};
/// NKContactListener  
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsWorld
struct PhysicsWorld {
  JPH::TempAllocatorImpl* temp_allocater;
  JPH::JobSystemThreadPool* job_system;

  JPH::BroadPhaseLayerInterfaceTable* bp_layer_table;
  JPH::ObjectLayerPairFilterTable* obj_vs_obj_layer_table;
  JPH::ObjectVsBroadPhaseLayerFilterTable* obj_vs_bp_layer_table;
  JPH::PhysicsSystem physics_system;

  NKBodyActivationListener activation_listener;
  NKContactListener contact_listener;

  JPH::BodyInterface* body_interface;

  f32 collision_tolerance = 0.05f;
  bool is_paused          = false;
};

static PhysicsWorld* s_world;
/// PhysicsWorld
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static inline Vec3 jph_vec3_to_vec3(const JPH::Vec3& vec) {
  return Vec3(vec[0], vec[1], vec[2]);
}

static inline Vec4 jph_vec4_to_vec4(const JPH::Vec4& vec) {
  return Vec4(vec[0], vec[1], vec[2], vec[3]);
}

static inline Quat jph_quat_to_quat(const JPH::Quat& quat) {
  return Quat(quat.GetW(), quat.GetX(), quat.GetY(), quat.GetZ());
}

static inline PhysicsBodyType jph_body_type_to_body_type(const JPH::EMotionType type) {
  switch(type) {
    case JPH::EMotionType::Static:
      return PHYSICS_BODY_STATIC;
    case JPH::EMotionType::Dynamic:
      return PHYSICS_BODY_DYNAMIC;
    case JPH::EMotionType::Kinematic:
      return PHYSICS_BODY_KINEMATIC;
  }
}

static inline GroundState jph_ground_state_top_ground_state(const JPH::Character::EGroundState state) {
  switch(state) {
    case JPH::Character::EGroundState::OnGround:
      return GROUND_STATE_ON_GROUND;
    case JPH::Character::EGroundState::OnSteepGround:
      return GROUND_STATE_ON_STEEP_GROUND;
    case JPH::Character::EGroundState::NotSupported:
      return GROUND_STATE_NOT_SUPPORTED;
    case JPH::Character::EGroundState::InAir:
      return GROUND_STATE_IN_AIR;
  }
}

static inline JPH::Vec3 vec3_to_jph_vec3(const Vec3& vec) {
  return JPH::Vec3(vec.x, vec.y, vec.z);
}

static inline JPH::Vec4 vec4_to_jph_vec4(const Vec4& vec) {
  return JPH::Vec4(vec.x, vec.y, vec.z, vec.w);
}

static inline JPH::Quat quat_to_jph_quat(const Quat& quat) {
  return JPH::Quat(quat.x, quat.y, quat.z, quat.w);
}

static inline JPH::EMotionType body_type_to_jph_body_type(const PhysicsBodyType type) {
  switch(type) {
    case PHYSICS_BODY_STATIC:
      return JPH::EMotionType::Static;
    case PHYSICS_BODY_DYNAMIC:
      return JPH::EMotionType::Dynamic;
    case PHYSICS_BODY_KINEMATIC:
      return JPH::EMotionType::Kinematic;
  }
}

static bool assert_impl(const char* expr, const char* msg, const char* file, JPH::uint line) {
  logger_log_assert(expr, msg, file, line);
  return true;
};

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Physics world functions

void physics_world_init(const PhysicsWorldDesc& desc) {
  // World init
  s_world = new PhysicsWorld{};

  // Jolt init
  
  JPH::RegisterDefaultAllocator();
  JPH::Factory::sInstance = new JPH::Factory();
  JPH::RegisterTypes();

  // Assert and trace implementations 
  JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = assert_impl;)

  // Allocaters and job systems init

  s_world->temp_allocater = new JPH::TempAllocatorImpl(desc.allocater_size);
  s_world->job_system     = new JPH::JobSystemThreadPool(desc.max_jobs, desc.max_barriers, desc.max_threads);

  // Broad phase layers init

  s_world->bp_layer_table = new JPH::BroadPhaseLayerInterfaceTable(PHYSICS_OBJECT_LAYERS_MAX, 
                                                                   PHYSICS_BROAD_PHASE_LAYERS_MAX); 

  // Setting fiters that are available for both broad phase layers
  
  for(u32 i = PHYSICS_OBJECT_LAYER_0; i <= PHYSICS_OBJECT_LAYER_5; i++) {
    s_world->bp_layer_table->MapObjectToBroadPhaseLayer((JPH::ObjectLayer)i, 
                                                        (JPH::BroadPhaseLayer)PHYSICS_BROAD_PHASE_LAYER_STATIC);
    s_world->bp_layer_table->MapObjectToBroadPhaseLayer((JPH::ObjectLayer)i, 
                                                        (JPH::BroadPhaseLayer)PHYSICS_BROAD_PHASE_LAYER_DYNAMIC);
  }
  
  // Setting fiters that are available for only the STATIC broad phase layer
 
  for(u32 i = PHYSICS_OBJECT_LAYER_6; i <= PHYSICS_OBJECT_LAYER_7; i++) {
    s_world->bp_layer_table->MapObjectToBroadPhaseLayer((JPH::ObjectLayer)i, 
                                                        (JPH::BroadPhaseLayer)PHYSICS_BROAD_PHASE_LAYER_STATIC);
  }

  // Setting fiters that are available for only the DYNAMIC broad phase layer
 
  for(u32 i = PHYSICS_OBJECT_LAYER_8; i <= PHYSICS_OBJECT_LAYER_9; i++) {
    s_world->bp_layer_table->MapObjectToBroadPhaseLayer((JPH::ObjectLayer)i, 
                                                        (JPH::BroadPhaseLayer)PHYSICS_BROAD_PHASE_LAYER_DYNAMIC);
  }

  // Object layers init
  
  s_world->obj_vs_obj_layer_table = new JPH::ObjectLayerPairFilterTable(PHYSICS_OBJECT_LAYERS_MAX);
  
  for(u32 i = 1; i < PHYSICS_OBJECT_LAYERS_MAX; i++) {
    for(u32 j = 0; j < PHYSICS_OBJECT_LAYERS_MAX; j++) {
      PhysicsObjectLayer inner_layer = desc.layers_matrix[i][j];
      if(inner_layer == PHYSICS_OBJECT_LAYER_NONE) {
        continue;
      }

      s_world->obj_vs_obj_layer_table->EnableCollision((JPH::ObjectLayer)i, 
                                                       (JPH::ObjectLayer)inner_layer);
    }
  }

  // Object/Broad phase layers init
  s_world->obj_vs_bp_layer_table = new JPH::ObjectVsBroadPhaseLayerFilterTable(*s_world->bp_layer_table, 
                                                                               PHYSICS_BROAD_PHASE_LAYERS_MAX,   
                                                                               *s_world->obj_vs_obj_layer_table, 
                                                                               PHYSICS_OBJECT_LAYERS_MAX);

  // Physics system init
  
  s_world->physics_system.Init(desc.max_bodies, 
                              desc.max_mutexes,
                              desc.max_body_pairs, 
                              desc.max_contact_constraints, 
                              *s_world->bp_layer_table, 
                              *s_world->obj_vs_bp_layer_table, 
                              *s_world->obj_vs_obj_layer_table);  
  s_world->physics_system.SetGravity(vec3_to_jph_vec3(desc.gravity));

  // Body interface init
  s_world->body_interface = &s_world->physics_system.GetBodyInterface();

  // Listeners init

  s_world->physics_system.SetBodyActivationListener(&s_world->activation_listener);
  s_world->physics_system.SetContactListener(&s_world->contact_listener);
}

void physics_world_shutdown() {
  // De-initialize Jolt

  JPH::UnregisterTypes();

  delete s_world->bp_layer_table;
  delete s_world->obj_vs_obj_layer_table;
  delete s_world->obj_vs_bp_layer_table;

  delete JPH::Factory::sInstance;
  delete s_world->job_system;
  delete s_world->temp_allocater;
  delete s_world;
}
 
void physics_world_step(const f32 delta_time, const i32 collision_steps) {
  if(s_world->is_paused) {
    return;
  }

  // Update the world
  s_world->physics_system.Update(delta_time, collision_steps, s_world->temp_allocater, s_world->job_system);
}

PhysicsBody* physics_world_create_body(const PhysicsBodyDesc& desc) {
  COLLIDER_CHECK(desc.collider);
  
  PhysicsBody* nk_body = new PhysicsBody{};

  // Create the Jolt body

  JPH::BodyCreationSettings body_settings(desc.collider->handle, 
                                          vec3_to_jph_vec3(desc.position), 
                                          quat_to_jph_quat(desc.rotation), 
                                          body_type_to_jph_body_type(desc.type), 
                                          (JPH::ObjectLayer)desc.layers);

  body_settings.mRestitution   = desc.restitution;
  body_settings.mFriction      = desc.friction;
  body_settings.mGravityFactor = desc.gravity_factor;
  body_settings.mIsSensor      = desc.is_sensor;  
  body_settings.mUserData      = (JPH::uint64)nk_body;

  // Create our body
 
  nk_body->handle      = s_world->body_interface->CreateBody(body_settings); 
  nk_body->collider    = (Collider*)desc.collider; 
  nk_body->user_data   = (void*)desc.user_data;

  return nk_body;
}

void physics_world_add_body(const PhysicsBody* body, const bool is_active) {
  BODY_CHECK(body);

  JPH::EActivation active = is_active ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
  s_world->body_interface->AddBody(body->handle->GetID(), active);
}

void physics_world_add_character(const Character* character, const bool is_active) {
  CHARACTER_CHECK(character);

  JPH::EActivation activate = is_active ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
  character->handle->AddToPhysicsSystem(activate);
}

PhysicsBody* physics_world_create_and_add_body(const PhysicsBodyDesc& desc, const bool is_active) {
  PhysicsBody* body = physics_world_create_body(desc);
  physics_world_add_body(body, is_active);

  return body;
}

void physics_world_remove_body(PhysicsBody* body) {
  BODY_CHECK(body);
  
  s_world->body_interface->RemoveBody(body->handle->GetID());
}

void physics_world_destroy_body(PhysicsBody* body) {
  BODY_CHECK(body);
  
  s_world->body_interface->DestroyBody(body->handle->GetID());

  delete body->collider;
  delete body;
}

void physics_world_destroy_and_remove(PhysicsBody* body) {
  physics_world_remove_body(body);
  physics_world_destroy_body(body);
}

void physics_world_remove_character(Character* character) {
  CHARACTER_CHECK(character);
  character->handle->RemoveFromPhysicsSystem();
}

const bool physics_world_cast_ray(const RayCastDesc& cast_desc) {
  const JPH::BroadPhaseQuery& broad_phase = s_world->physics_system.GetBroadPhaseQuery();
  
  // Cast the ray into the Jolt world

  JPH::AllHitCollisionCollector<JPH::RayCastBodyCollector> collector; 
  JPH::RayCast ray(vec3_to_jph_vec3(cast_desc.origin), vec3_to_jph_vec3(cast_desc.direction * cast_desc.distance));

  broad_phase.CastRay(ray, 
                      collector, 
                      JPH::SpecifiedBroadPhaseLayerFilter((JPH::BroadPhaseLayer)cast_desc.broad_phase_layer), 
                      JPH::SpecifiedObjectLayerFilter((JPH::ObjectLayer)cast_desc.object_layer));
  
  // Complete our ray cast and send out events for all the successful hits

  for(sizei i = 0; i < collector.mHits.size(); i++) {
    // Make sense of the ray cast data
    
    JPH::BroadPhaseCastResult* result = &collector.mHits[i];
    
    JPH::Vec3 hit_point = ray.mOrigin + result->mFraction * 
                          (s_world->body_interface->GetCenterOfMassPosition(result->mBodyID) - ray.mOrigin);

    RayCastResult ray_result = {
      .body          = reinterpret_cast<PhysicsBody*>(s_world->body_interface->GetUserData(result->mBodyID)), 
      .point         = jph_vec3_to_vec3(hit_point),
      .ray_direction = cast_desc.direction, 
    };

    // Send out an event
   
    Event event {
      .type        = EVENT_PHYSICS_RAYCAST_HIT,
      .cast_result = ray_result,
    };
    event_dispatch(event);
  }

  return (collector.mHits.size() > 0);
}

void physics_world_set_safe_mode(const bool safe) {
  if(safe) {
    s_world->body_interface = &s_world->physics_system.GetBodyInterface();
  }
  else {
    s_world->body_interface = &s_world->physics_system.GetBodyInterfaceNoLock();
  }
}

void physics_world_set_layers_matrix(PhysicsLayersMatrix matrix) {
  for(u32 i = 1; i < PHYSICS_OBJECT_LAYERS_MAX; i++) {
    for(u32 j = 0; j < PHYSICS_OBJECT_LAYERS_MAX; j++) {
      PhysicsObjectLayer inner_layer = matrix[i][j];
      if(inner_layer == PHYSICS_OBJECT_LAYER_NONE) {
        continue;
      }

      s_world->obj_vs_obj_layer_table->EnableCollision((JPH::ObjectLayer)i, 
                                                       (JPH::ObjectLayer)inner_layer);
    }
  }
}

void physics_world_set_gravity(const Vec3& gravity) {
  s_world->physics_system.SetGravity(vec3_to_jph_vec3(gravity));
}

Vec3 physics_world_get_gravity() {
  return jph_vec3_to_vec3(s_world->physics_system.GetGravity());
}

void physics_world_toggle_paused() {
  s_world->is_paused = !s_world->is_paused;
}

const bool physics_world_is_paused() {
  return s_world->is_paused;
}

/// Physics world functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Physics body functions

void physics_body_set_position(PhysicsBody* body, const Vec3 position, const bool activate) {
  BODY_CHECK(body);

  JPH::EActivation active = activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
  s_world->body_interface->SetPosition(body->handle->GetID(), vec3_to_jph_vec3(position), active);
}

void physics_body_set_rotation(PhysicsBody* body, const Quat rotation, const bool activate) {
  BODY_CHECK(body);

  JPH::EActivation active = activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
  s_world->body_interface->SetRotation(body->handle->GetID(), quat_to_jph_quat(rotation), active);
}

void physics_body_set_rotation(PhysicsBody* body, const Vec3 axis, const f32 angle, const bool activate) {
  BODY_CHECK(body);
 
  JPH::EActivation active = activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
  JPH::Quat rotation      = quat_to_jph_quat(quat_angle_axis(axis, angle));
  
  s_world->body_interface->SetRotation(body->handle->GetID(), rotation, active);
}

void physics_body_set_transform(PhysicsBody* body, const Transform& transform, const bool activate) {
  BODY_CHECK(body);
 
  JPH::EActivation active = activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
  JPH::Vec3 position      = vec3_to_jph_vec3(transform.position);
  JPH::Quat rotation      = quat_to_jph_quat(transform.rotation);

  s_world->body_interface->SetPositionAndRotation(body->handle->GetID(), position, rotation, active);
}

void physics_body_set_linear_velocity(PhysicsBody* body, const Vec3 velocity) {
  BODY_CHECK(body);
  body->handle->SetLinearVelocity(vec3_to_jph_vec3(velocity));
}

void physics_body_set_angular_velocity(PhysicsBody* body, const Vec3 velocity) {
  BODY_CHECK(body);
  body->handle->SetAngularVelocity(vec3_to_jph_vec3(velocity));
}

void physics_body_set_active(PhysicsBody* body, const bool active) {
  BODY_CHECK(body);
 
  if(active) {
    s_world->body_interface->ActivateBody(body->handle->GetID());
  }
  else {
    s_world->body_interface->DeactivateBody(body->handle->GetID());
  }
}

void physics_body_set_user_data(PhysicsBody* body, const void* user_data) {
  BODY_CHECK(body);
  NIKOLA_ASSERT(user_data, "Passing invalid user data to body"); 

  body->user_data = (void*)user_data;
}

void physics_body_set_layer(PhysicsBody* body, const PhysicsObjectLayer layer) {
  BODY_CHECK(body);
  s_world->body_interface->SetObjectLayer(body->handle->GetID(), (JPH::ObjectLayer)layer);
}

void physics_body_set_restitution(PhysicsBody* body, const f32 restitution) {
  BODY_CHECK(body);
  body->handle->SetRestitution(restitution);
}

void physics_body_set_friction(PhysicsBody* body, const f32 friction) {
  BODY_CHECK(body);
  body->handle->SetFriction(friction);
}

void physics_body_set_gravity_factor(PhysicsBody* body, const f32 factor) {
  BODY_CHECK(body);
  body->handle->GetMotionProperties()->SetGravityFactor(factor);
}

void physics_body_set_type(PhysicsBody* body, const PhysicsBodyType type) {
  BODY_CHECK(body);
  
  body->handle->SetMotionType(body_type_to_jph_body_type(type));
}

void physics_body_set_collider(PhysicsBody* body, const Collider* collider, const bool activate) {
  BODY_CHECK(body);
  COLLIDER_CHECK(collider);
  
  JPH::EActivation active = activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
  s_world->body_interface->SetShape(body->handle->GetID(), collider->handle, true, active);

  body->collider = (Collider*)collider;
}

void physics_body_apply_linear_velocity(PhysicsBody* body, const Vec3 velocity) {
  BODY_CHECK(body);
  s_world->body_interface->AddLinearVelocity(body->handle->GetID(), vec3_to_jph_vec3(velocity));
}

void physics_body_apply_force(PhysicsBody* body, const Vec3 force) {
  BODY_CHECK(body);
  body->handle->AddForce(vec3_to_jph_vec3(force));
}

void physics_body_apply_force_at(PhysicsBody* body, const Vec3 force, const Vec3 point) {
  BODY_CHECK(body);
  body->handle->AddForce(vec3_to_jph_vec3(force), vec3_to_jph_vec3(point));
}

void physics_body_apply_torque(PhysicsBody* body, const Vec3 torque) {
  BODY_CHECK(body);
  body->handle->AddTorque(vec3_to_jph_vec3(torque));
}

void physics_body_apply_impulse(PhysicsBody* body, const Vec3 impulse) {
  BODY_CHECK(body);
  body->handle->AddImpulse(vec3_to_jph_vec3(impulse));
}

void physics_body_apply_impulse_at(PhysicsBody* body, const Vec3 impulse, const Vec3 point) {
  BODY_CHECK(body);
  body->handle->AddImpulse(vec3_to_jph_vec3(impulse), vec3_to_jph_vec3(point));
}

void physics_body_apply_angular_impulse(PhysicsBody* body, const Vec3 impulse) {
  BODY_CHECK(body);
  s_world->body_interface->AddAngularImpulse(body->handle->GetID(), vec3_to_jph_vec3(impulse));
}

const Vec3 physics_body_get_position(const PhysicsBody* body) {
  BODY_CHECK(body);
  return jph_vec3_to_vec3(body->handle->GetPosition());
}

const Vec3 physics_body_get_com_position(const PhysicsBody* body) {
  BODY_CHECK(body);
  return jph_vec3_to_vec3(s_world->body_interface->GetCenterOfMassPosition(body->handle->GetID()));
}

const Quat physics_body_get_rotation(const PhysicsBody* body) {
  BODY_CHECK(body);
  return jph_quat_to_quat(body->handle->GetRotation());
}

const Vec3 physics_body_get_linear_velocity(const PhysicsBody* body) {
  BODY_CHECK(body);
  return jph_vec3_to_vec3(body->handle->GetLinearVelocity());
}

const Vec3 physics_body_get_angular_velocity(const PhysicsBody* body) {
  BODY_CHECK(body);
  return jph_vec3_to_vec3(body->handle->GetAngularVelocity());
}

const bool physics_body_is_active(const PhysicsBody* body) {
  BODY_CHECK(body);
  return body->handle->IsActive();
}

const bool physics_body_is_sensor(const PhysicsBody* body) {
  BODY_CHECK(body);
  return body->handle->IsSensor();
}

void* physics_body_get_user_data(const PhysicsBody* body) {
  BODY_CHECK(body);
  return body->user_data;
}

const PhysicsObjectLayer physics_body_get_layer(const PhysicsBody* body) {
  BODY_CHECK(body);
  return (PhysicsObjectLayer)body->handle->GetObjectLayer();
}

const f32 physics_body_get_restitution(const PhysicsBody* body) {
  BODY_CHECK(body);
  return body->handle->GetRestitution();
}

const f32 physics_body_get_friction(const PhysicsBody* body) {
  BODY_CHECK(body);
  return body->handle->GetFriction();
}

const f32 physics_body_get_gravity_factor(const PhysicsBody* body) {
  BODY_CHECK(body);
  return body->handle->GetMotionProperties()->GetGravityFactor();
}

const PhysicsBodyType physics_body_get_type(const PhysicsBody* body) {
  BODY_CHECK(body);
  return jph_body_type_to_body_type(body->handle->GetMotionType());
}

Transform physics_body_get_transform(const PhysicsBody* body) {
  BODY_CHECK(body);

  Transform transform;
  transform.position = jph_vec3_to_vec3(body->handle->GetPosition());
  transform.rotation = jph_quat_to_quat(body->handle->GetRotation());

  transform_apply(transform);
  return transform;
}

Collider* physics_body_get_collider(const PhysicsBody* body) {
  BODY_CHECK(body);
  return body->collider;
}

/// Physics body functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Collider functions

Collider* collider_create(const BoxColliderDesc& desc) {
  // Create the Jolt shape

  JPH::BoxShapeSettings shape_settings(vec3_to_jph_vec3(desc.half_size));
  JPH::Shape::ShapeResult result = shape_settings.Create();

  if(!result.IsValid()) {
    NIKOLA_LOG_ERROR("Failed to create a box collider - %s", result.GetError().c_str());
    return nullptr;
  }

  // Create the collider from the shape

  Collider* collider = new Collider{};
  collider->handle   = result.Get();
  collider->type     = COLLIDER_BOX;
  collider->extents  = desc.half_size;

  return collider;
}

Collider* collider_create(const SphereColliderDesc& desc) {
  // Create the Jolt shape

  JPH::SphereShapeSettings shape_settings(desc.radius);
  JPH::Shape::ShapeResult result = shape_settings.Create();

  if(!result.IsValid()) {
    NIKOLA_LOG_ERROR("Failed to create a sphere collider - %s", result.GetError().c_str());
    return nullptr;
  }

  // Create the collider from the shape

  Collider* collider = new Collider{};
  collider->handle   = result.Get();
  collider->type     = COLLIDER_SPHERE;
  collider->extents  = Vec3(desc.radius);

  return collider;
}

Collider* collider_create(const CapsuleColliderDesc& desc) {
  // Create the Jolt shape

  JPH::CapsuleShapeSettings shape_settings(desc.half_height, desc.radius);
  JPH::Shape::ShapeResult result = shape_settings.Create();

  if(!result.IsValid()) {
    NIKOLA_LOG_ERROR("Failed to create a capsule collider - %s", result.GetError().c_str());
    return nullptr;
  }

  // Create the collider from the shape

  Collider* collider = new Collider{};
  collider->handle   = result.Get();
  collider->type     = COLLIDER_CAPSULE;
  collider->extents  = Vec3(desc.radius, desc.half_height, 0.0f);

  return collider;
}

ColliderType collider_get_type(const Collider* collider) {
  COLLIDER_CHECK(collider);
  return collider->type;
}

Vec3 collider_get_extents(const Collider* collider) {
  COLLIDER_CHECK(collider);
  return collider->extents;
}

/// Collider functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Character body functions

Character* character_body_create(const CharacterBodyDesc& desc) {
  COLLIDER_CHECK(desc.collider);
  
  Character* nk_char = new Character{}; 

  // Create the Jolt character

  JPH::Ref<JPH::CharacterSettings> settings = new JPH::CharacterSettings();

  settings->mLayer         = (JPH::ObjectLayer)desc.layer; 
  settings->mMaxSlopeAngle = desc.max_slope_angle; 
  settings->mMass          = desc.mass; 
  settings->mFriction      = desc.friction; 
  settings->mGravityFactor = desc.gravity_factor;

  settings->mUp    = vec3_to_jph_vec3(desc.up_axis);
  settings->mShape = desc.collider->handle;

  JPH::Ref<JPH::Character> character = new JPH::Character(settings, 
                                                          vec3_to_jph_vec3(desc.position), 
                                                          quat_to_jph_quat(desc.rotation), 
                                                          (JPH::uint64)nk_char,
                                                          &s_world->physics_system);

  // Create our character

  nk_char->handle    = character;
  nk_char->user_data = (void*)desc.user_data;
  nk_char->collider  = (Collider*)desc.collider;

  return nk_char;
}

void character_body_destroy(Character* character) {
  CHARACTER_CHECK(character);

  delete character->collider;
  delete character;
}

void character_body_update(Character* character) {
  CHARACTER_CHECK(character);
  character->handle->PostSimulation(s_world->collision_tolerance);
}

void character_body_set_position(Character* character, const Vec3 position) {
  CHARACTER_CHECK(character);
  character->handle->SetPosition(vec3_to_jph_vec3(position));
}

void character_body_set_rotation(Character* character, const Quat rotation) {
  CHARACTER_CHECK(character);
  character->handle->SetRotation(quat_to_jph_quat(rotation));
}

void character_body_set_rotation(Character* character, const Vec3 axis, const f32 angle) {
  CHARACTER_CHECK(character);
  character->handle->SetRotation(quat_to_jph_quat(quat_angle_axis(axis, angle)));
}

void character_body_set_linear_velocity(Character* character, const Vec3 velocity) {
  CHARACTER_CHECK(character);
  character->handle->SetLinearVelocity(vec3_to_jph_vec3(velocity));
}

void character_body_set_layer(Character* character, const PhysicsObjectLayer layer) {
  CHARACTER_CHECK(character);
  character->handle->SetLayer((JPH::ObjectLayer)layer);
}

void character_body_set_slope_angle(Character* character, const f32 max_slope_angle) {
  CHARACTER_CHECK(character);
  character->handle->SetMaxSlopeAngle(max_slope_angle);
}

void character_body_set_collider(Character* character, const Collider* collider, const f32 max_penetration_depth) {
  CHARACTER_CHECK(character);
  COLLIDER_CHECK(collider); 

  character->handle->SetShape(collider->handle, max_penetration_depth);
  character->collider = (Collider*)collider;
}

void character_body_activate(Character* character) {
  CHARACTER_CHECK(character);
  character->handle->Activate();
}

void character_body_apply_linear_velocity(Character* character, const Vec3 velocity) {
  CHARACTER_CHECK(character);
  character->handle->AddLinearVelocity(vec3_to_jph_vec3(velocity));
}

void character_body_apply_impulse(Character* character, const Vec3 impulse) {
  CHARACTER_CHECK(character);
  character->handle->AddImpulse(vec3_to_jph_vec3(impulse));
}

const Vec3 character_body_get_position(const Character* character) {
  CHARACTER_CHECK(character);
  return jph_vec3_to_vec3(character->handle->GetPosition());
}

const Vec3 character_body_get_com_position(const Character* character) {
  CHARACTER_CHECK(character);
  return jph_vec3_to_vec3(character->handle->GetCenterOfMassPosition());
}

const Quat character_body_get_rotation(const Character* character) {
  CHARACTER_CHECK(character);
  return jph_quat_to_quat(character->handle->GetRotation());
}

const Vec3 character_body_get_linear_velocity(const Character* character) {
  CHARACTER_CHECK(character);
  return jph_vec3_to_vec3(character->handle->GetLinearVelocity());
}

const PhysicsObjectLayer character_body_get_layer(const Character* character) {
  CHARACTER_CHECK(character);
  return (PhysicsObjectLayer)character->handle->GetLayer();
}

const f32 character_body_get_slope_angle(const Character* character) {
  CHARACTER_CHECK(character);
  return character->handle->GetCosMaxSlopeAngle();
}

Transform character_body_get_transform(const Character* character) {
  CHARACTER_CHECK(character);

  Transform transform;
  transform.position = jph_vec3_to_vec3(character->handle->GetCenterOfMassPosition());
  transform.rotation = jph_quat_to_quat(character->handle->GetRotation());

  transform_apply(transform);
  return transform;
}

GroundState character_body_query_ground_state(const Character* character) {
  CHARACTER_CHECK(character);
  return jph_ground_state_top_ground_state(character->handle->GetGroundState());
}

const bool character_body_is_supported(const Character* character) {
  CHARACTER_CHECK(character);
  return character->handle->IsSupported();
}

const bool character_body_is_slope_too_steep(const Character* character, const Vec3 surface_normal) {
  CHARACTER_CHECK(character);
  return character->handle->IsSlopeTooSteep(vec3_to_jph_vec3(surface_normal));
}

const Vec3 character_body_get_ground_position(const Character* character) {
  CHARACTER_CHECK(character);
  return jph_vec3_to_vec3(character->handle->GetGroundPosition());
}

const Vec3 character_body_get_ground_normal(const Character* character) {
  CHARACTER_CHECK(character);
  return jph_vec3_to_vec3(character->handle->GetGroundNormal());
}

const Vec3 character_body_get_ground_velocity(const Character* character) {
  CHARACTER_CHECK(character);
  return jph_vec3_to_vec3(character->handle->GetGroundVelocity());
}

const PhysicsBody* character_body_get_ground_body(const Character* character) {
  CHARACTER_CHECK(character);

  JPH::uint64 user_data = s_world->body_interface->GetUserData(character->handle->GetGroundBodyID());
  return reinterpret_cast<const PhysicsBody*>(user_data);
}

const bool character_body_cast_ray(const Character* character, const RayCastDesc& cast_desc) {
  CHARACTER_CHECK(character);
  
  JPH::TransformedShape trans_shape = character->handle->GetTransformedShape();
 
  JPH::RayCast ray(vec3_to_jph_vec3(cast_desc.origin), vec3_to_jph_vec3(cast_desc.direction * cast_desc.distance));
  JPH::RayCastResult ray_result; 

  return trans_shape.CastRay(JPH::RRayCast(ray), ray_result);
}

/// Character body functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Physics ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
