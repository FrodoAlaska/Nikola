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

#define BODY_ID_CHECK(id)      NIKOLA_ASSERT((id._id != PHYSICS_ID_INVALID), "Trying to access a physics body with an invalid ID")
#define COLLIDER_ID_CHECK(id)  NIKOLA_ASSERT((id._id != PHYSICS_ID_INVALID), "Trying to access a collider with an invalid ID")
#define CHARACTER_ID_CHECK(id) NIKOLA_ASSERT((id._id != PHYSICS_ID_INVALID), "Trying to access a character with an invalid ID")

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
/// NKBodyActivationListener  
class NKBodyActivationListener : public JPH::BodyActivationListener
{
public:
	void OnBodyActivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override {
    PhysicsBodyID body_id = {
      ._id = inBodyID.GetIndex(),
    };
    
    Event event = {
      .type    = EVENT_PHYSICS_BODY_ACTIVATED,
      .body_id = body_id,
    };
    event_dispatch(event);
	}

	void OnBodyDeactivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override {
    PhysicsBodyID body_id = {
      ._id = inBodyID.GetIndex(),
    };
    
    Event event = {
      .type    = EVENT_PHYSICS_BODY_DEACTIVATED,
      .body_id = body_id,
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
      .body1_id = inBody1.GetID().GetIndex(), 
      .body2_id = inBody2.GetID().GetIndex(), 

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
      .body1_id = inBody1.GetID().GetIndex(), 
      .body2_id = inBody2.GetID().GetIndex(), 

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
    CollisionData data = {
      .body1_id = inSubShapePair.GetBody1ID().GetIndex(), 
      .body2_id = inSubShapePair.GetBody2ID().GetIndex(), 
    };

    Event event = {
      .type           = EVENT_PHYSICS_CONTACT_REMOVED,
      .collision_data = data,
    };
    event_dispatch(event);
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

  DynamicArray<JPH::Ref<JPH::Shape>> shapes; 
  DynamicArray<JPH::BodyID> bodies;
  DynamicArray<JPH::Ref<JPH::Character>> characters;

  Queue<JPH::BodyInterface::AddState> batches_queue;

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

  // Listeners init

  s_world->physics_system.SetBodyActivationListener(&s_world->activation_listener);
  s_world->physics_system.SetContactListener(&s_world->contact_listener);

  // Body interface init
  s_world->body_interface = &s_world->physics_system.GetBodyInterface();
}

void physics_world_shutdown() {
  // Delete all characters from the world
 
  for(sizei i = 0; i < s_world->characters.size(); i++) {
    JPH::Ref<JPH::Character> character = s_world->characters[i];

    character->RemoveFromPhysicsSystem();

    sizei body_index = character->GetBodyID().GetIndex();
    s_world->bodies[body_index] = JPH::BodyID();
  }
  s_world->characters.clear();

  // Delete all the bodies in the world
  
  for(sizei i = 0; i < s_world->bodies.size(); i++) {
    JPH::BodyID body = s_world->bodies[i];
    if(body.IsInvalid()) {
      continue;
    }

    s_world->body_interface->RemoveBody(body);
    s_world->body_interface->DestroyBody(body);
  }

  s_world->bodies.clear();
  s_world->shapes.clear();

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

  // Post update for all the characters 
  for(auto& character : s_world->characters) {
    character->PostSimulation(s_world->collision_tolerance);
  }
}

PhysicsBodyID physics_world_create_body(const PhysicsBodyDesc& desc) {
  COLLIDER_ID_CHECK(desc.collider_id);

  // Create the body
  
  JPH::BodyCreationSettings body_settings(s_world->shapes[desc.collider_id._id], 
                                          vec3_to_jph_vec3(desc.position), 
                                          quat_to_jph_quat(desc.rotation), 
                                          body_type_to_jph_body_type(desc.type), 
                                          (JPH::ObjectLayer)desc.layers);

  body_settings.mRestitution   = desc.restitution;
  body_settings.mFriction      = desc.friction;
  body_settings.mGravityFactor = desc.gravity_factor;
  body_settings.mIsSensor      = desc.is_sensor;  
  body_settings.mUserData      = desc.user_data;

  JPH::Body* body = s_world->body_interface->CreateBody(body_settings); 

  // Add the body to our internal buffer
  
  s_world->bodies.push_back(body->GetID());
  return PhysicsBodyID {
    ._id = (body->GetID().GetIndex()),
  };
}

void physics_world_add_body(const PhysicsBodyID& body_id, const bool is_active) {
  BODY_ID_CHECK(body_id);

  JPH::EActivation active = is_active ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
  JPH::BodyID body        = s_world->bodies[body_id._id];
  s_world->body_interface->AddBody(body, active);
}

void physics_world_add_character(const CharacterID& char_id, const bool is_active) {
  CHARACTER_ID_CHECK(char_id);

  JPH::Character* character = s_world->characters[char_id._id];
  JPH::EActivation activate = is_active ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;

  character->AddToPhysicsSystem(activate);
  s_world->bodies.push_back(character->GetBodyID());
}

PhysicsBodyID physics_world_create_and_add_body(const PhysicsBodyDesc& desc, const bool is_active) {
  PhysicsBodyID id = physics_world_create_body(desc);
  physics_world_add_body(id, is_active);

  return id;
}

void physics_world_prepare_bodies(PhysicsBodyID* bodies, const sizei bodies_count) {
  JPH::BodyInterface::AddState batch = s_world->body_interface->AddBodiesPrepare(&s_world->bodies[bodies[0]._id], bodies_count);
  s_world->batches_queue.push(batch);
}

const bool physics_world_finalize_bodies(PhysicsBodyID* bodies, const sizei bodies_count, const bool is_active) {
  if(s_world->batches_queue.empty()) {
    return false;
  }

  JPH::BodyInterface::AddState batch = s_world->batches_queue.front();
  JPH::EActivation active            = is_active ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
  s_world->bodies.resize(s_world->bodies.size() + bodies_count);

  s_world->body_interface->AddBodiesFinalize(&s_world->bodies[bodies[0]._id], bodies_count, batch, active);
  s_world->batches_queue.pop();

  s_world->physics_system.OptimizeBroadPhase();
  return true;
}

const bool physics_world_abort_bodies(PhysicsBodyID* bodies, const sizei bodies_count) {
  if(s_world->batches_queue.empty()) {
    return false;
  }

  JPH::BodyInterface::AddState batch = s_world->batches_queue.front();

  s_world->body_interface->AddBodiesAbort(&s_world->bodies[bodies[0]._id], bodies_count, batch);
  s_world->batches_queue.pop();

  return true;
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
    
    PhysicsBodyID body_id = {
      ._id = result->mBodyID.GetIndex(),
    };

    JPH::Vec3 hit_point = ray.mOrigin + result->mFraction * 
                          (s_world->body_interface->GetCenterOfMassPosition(result->mBodyID) - ray.mOrigin);

    RayCastResult ray_result = {
      .body_id       = body_id,
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

void physics_world_draw() {
  DynamicArray<Transform> cubes;
  cubes.reserve(s_world->bodies.size());
  
  DynamicArray<Transform> spheres;
  spheres.reserve(s_world->bodies.size());
 
  // Go through all of the bodies and render their shapes

  for(auto& body : s_world->bodies) {
    JPH::RefConst<JPH::Shape> shape = s_world->body_interface->GetShape(body);
    JPH::AABox shape_aabb           = shape->GetLocalBounds();
   
    Transform transform;
    transform.position = jph_vec3_to_vec3(s_world->body_interface->GetCenterOfMassPosition(body));
    transform.scale    = jph_vec3_to_vec3(shape_aabb.GetExtent());

    transform_apply(transform);

    // Push the shape to the appropriate array

    ColliderType collider_type = (ColliderType)shape->GetUserData();
    switch(collider_type) {
      case COLLIDER_BOX:
        cubes.push_back(transform);
        break;
      case COLLIDER_SPHERE:
        spheres.push_back(transform);
        break;
      case COLLIDER_CAPSULE:
        // @TODO (Physics/Debug): Capsules...
        break;
    }
  }
  
  // Render cubes
 
  if(!cubes.empty()) {
    renderer_queue_debug_cube_instanced(cubes.data(), cubes.size());
  }
  
  // Render spheres
 
  if(!spheres.empty()) {
    renderer_queue_debug_sphere_instanced(spheres.data(), spheres.size());
  }
}

/// Physics world functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Physics body functions

void physics_body_set_position(const PhysicsBodyID& body_id, const Vec3 position, const bool activate) {
  BODY_ID_CHECK(body_id);
  JPH::BodyID body = s_world->bodies[body_id._id];

  JPH::EActivation active = activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
  s_world->body_interface->SetPosition(body, vec3_to_jph_vec3(position), active);
}

void physics_body_set_rotation(const PhysicsBodyID& body_id, const Quat rotation, const bool activate) {
  BODY_ID_CHECK(body_id);
  JPH::BodyID body = s_world->bodies[body_id._id];

  JPH::EActivation active = activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
  s_world->body_interface->SetRotation(body, quat_to_jph_quat(rotation), active);
}

void physics_body_set_rotation(const PhysicsBodyID& body_id, const Vec3 axis, const f32 angle, const bool activate) {
  BODY_ID_CHECK(body_id);
  JPH::BodyID body = s_world->bodies[body_id._id];
 
  JPH::EActivation active = activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
  JPH::Quat rotation      = quat_to_jph_quat(quat_angle_axis(axis, angle));
  
  s_world->body_interface->SetRotation(body, rotation, active);
}

void physics_body_set_transform(const PhysicsBodyID& body_id, const Transform& transform, const bool activate) {
  BODY_ID_CHECK(body_id);
  JPH::BodyID body = s_world->bodies[body_id._id];
 
  JPH::EActivation active = activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
  JPH::Vec3 position      = vec3_to_jph_vec3(transform.position);
  JPH::Quat rotation      = quat_to_jph_quat(transform.rotation);

  s_world->body_interface->SetPositionAndRotation(body, position, rotation, active);
}

void physics_body_set_linear_velocity(const PhysicsBodyID& body_id, const Vec3 velocity) {
  BODY_ID_CHECK(body_id);
  JPH::BodyID body = s_world->bodies[body_id._id];
  
  s_world->body_interface->SetLinearVelocity(body, vec3_to_jph_vec3(velocity));
}

void physics_body_set_angular_velocity(const PhysicsBodyID& body_id, const Vec3 velocity) {
  BODY_ID_CHECK(body_id);
  JPH::BodyID body = s_world->bodies[body_id._id];
  
  s_world->body_interface->SetAngularVelocity(body, vec3_to_jph_vec3(velocity));
}

void physics_body_set_active(const PhysicsBodyID& body_id, const bool active) {
  BODY_ID_CHECK(body_id);
  JPH::BodyID body = s_world->bodies[body_id._id];
 
  if(active) {
    s_world->body_interface->ActivateBody(body);
  }
  else {
    s_world->body_interface->DeactivateBody(body);
  }
}

void physics_body_set_user_data(const PhysicsBodyID& body_id, const u64 user_data) {
  BODY_ID_CHECK(body_id);
  NIKOLA_ASSERT(user_data, "Passing invalid user data to body"); 

  JPH::BodyID body = s_world->bodies[body_id._id];
  s_world->body_interface->SetUserData(body, (JPH::uint64)user_data);
}

void physics_body_set_layer(const PhysicsBodyID& body_id, const PhysicsObjectLayer layer) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  s_world->body_interface->SetObjectLayer(body, (JPH::ObjectLayer)layer);
}

void physics_body_set_restitution(const PhysicsBodyID& body_id, const f32 restitution) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  s_world->body_interface->SetRestitution(body, restitution);
}

void physics_body_set_friction(const PhysicsBodyID& body_id, const f32 friction) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  s_world->body_interface->SetFriction(body, friction);
}

void physics_body_set_gravity_factor(const PhysicsBodyID& body_id, const f32 factor) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  s_world->body_interface->SetGravityFactor(body, factor);
}

void physics_body_set_type(const PhysicsBodyID& body_id, const PhysicsBodyType type) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  s_world->body_interface->SetMotionType(body, body_type_to_jph_body_type(type), JPH::EActivation::Activate);
}

void physics_body_set_collider(const PhysicsBodyID& body_id, const ColliderID& collider_id, const bool activate) {
  BODY_ID_CHECK(body_id);
  COLLIDER_ID_CHECK(collider_id);
  
  JPH::BodyID body        = s_world->bodies[body_id._id];
  JPH::EActivation active = activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;

  s_world->body_interface->SetShape(body, s_world->shapes[collider_id._id], true, active);
}

void physics_body_apply_linear_velocity(const PhysicsBodyID& body_id, const Vec3 velocity) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  s_world->body_interface->AddLinearVelocity(body, vec3_to_jph_vec3(velocity));
}

void physics_body_apply_force(const PhysicsBodyID& body_id, const Vec3 force) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  s_world->body_interface->AddForce(body, vec3_to_jph_vec3(force));
}

void physics_body_apply_force_at(const PhysicsBodyID& body_id, const Vec3 force, const Vec3 point) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  s_world->body_interface->AddForce(body, vec3_to_jph_vec3(force), vec3_to_jph_vec3(point));
}

void physics_body_apply_torque(const PhysicsBodyID& body_id, const Vec3 torque) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  s_world->body_interface->AddTorque(body, vec3_to_jph_vec3(torque));
}

void physics_body_apply_impulse(const PhysicsBodyID& body_id, const Vec3 impulse) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  s_world->body_interface->AddImpulse(body, vec3_to_jph_vec3(impulse));
}

void physics_body_apply_impulse_at(const PhysicsBodyID& body_id, const Vec3 impulse, const Vec3 point) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  s_world->body_interface->AddImpulse(body, vec3_to_jph_vec3(impulse), vec3_to_jph_vec3(point));
}

void physics_body_apply_angular_impulse(const PhysicsBodyID& body_id, const Vec3 impulse) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  s_world->body_interface->AddAngularImpulse(body, vec3_to_jph_vec3(impulse));
}

const Vec3 physics_body_get_position(const PhysicsBodyID& body_id) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  return jph_vec3_to_vec3(s_world->body_interface->GetPosition(body));
}

const Vec3 physics_body_get_com_position(const PhysicsBodyID& body_id) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  return jph_vec3_to_vec3(s_world->body_interface->GetCenterOfMassPosition(body));
}

const Quat physics_body_get_rotation(const PhysicsBodyID& body_id) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  return jph_quat_to_quat(s_world->body_interface->GetRotation(body));
}

const Vec3 physics_body_get_linear_velocity(const PhysicsBodyID& body_id) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  return jph_vec3_to_vec3(s_world->body_interface->GetLinearVelocity(body));
}

const Vec3 physics_body_get_angular_velocity(const PhysicsBodyID& body_id) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  return jph_vec3_to_vec3(s_world->body_interface->GetAngularVelocity(body));
}

const bool physics_body_is_active(const PhysicsBodyID& body_id) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  return s_world->body_interface->IsActive(body);
}

const u64 physics_body_get_user_data(const PhysicsBodyID& body_id) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  return (u64)s_world->body_interface->GetUserData(body);
}

const PhysicsObjectLayer physics_body_get_layer(const PhysicsBodyID& body_id) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  return (PhysicsObjectLayer)s_world->body_interface->GetObjectLayer(body);
}

const f32 physics_body_get_restitution(const PhysicsBodyID& body_id) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  return s_world->body_interface->GetRestitution(body);
}

const f32 physics_body_get_friction(const PhysicsBodyID& body_id) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  return s_world->body_interface->GetFriction(body);
}

const f32 physics_body_get_gravity_factor(const PhysicsBodyID& body_id) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  return s_world->body_interface->GetGravityFactor(body);
}

const PhysicsBodyType physics_body_get_type(const PhysicsBodyID& body_id) {
  BODY_ID_CHECK(body_id);
  
  JPH::BodyID body = s_world->bodies[body_id._id];
  return jph_body_type_to_body_type(s_world->body_interface->GetMotionType(body));
}

Transform physics_body_get_transform(const PhysicsBodyID& body_id) {
  BODY_ID_CHECK(body_id);
  JPH::BodyID body = s_world->bodies[body_id._id];

  Transform transform;
  transform.position = jph_vec3_to_vec3(s_world->body_interface->GetPosition(body));
  transform.rotation = jph_quat_to_quat(s_world->body_interface->GetRotation(body));

  transform_apply(transform);
  return transform;
}

/// Physics body functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Collider functions

ColliderID collider_create(const BoxColliderDesc& desc) {
  JPH::BoxShapeSettings shape_settings(vec3_to_jph_vec3(desc.half_size));
  JPH::Shape::ShapeResult result = shape_settings.Create();

  if(!result.IsValid()) {
    NIKOLA_LOG_ERROR("Failed to create a box collider - %s", result.GetError().c_str());
    return ColliderID{};
  }

  s_world->shapes.push_back(result.Get());
  result.Get()->SetUserData((JPH::uint64)COLLIDER_BOX);

  ColliderID coll_id = {
    ._type = COLLIDER_BOX,
    ._id   = (u32)(s_world->shapes.size() - 1),
  };

  return coll_id;
}

ColliderID collider_create(const SphereColliderDesc& desc) {
  JPH::SphereShapeSettings shape_settings(desc.radius);
  JPH::Shape::ShapeResult result = shape_settings.Create();

  if(!result.IsValid()) {
    NIKOLA_LOG_ERROR("Failed to create a sphere collider - %s", result.GetError().c_str());
    return ColliderID{};
  }

  s_world->shapes.push_back(result.Get());
  result.Get()->SetUserData((JPH::uint64)COLLIDER_SPHERE);

  ColliderID coll_id = {
    ._type = COLLIDER_SPHERE,
    ._id   = (u32)(s_world->shapes.size() - 1),
  };

  return coll_id;
}

ColliderID collider_create(const CapsuleColliderDesc& desc) {
  JPH::CapsuleShapeSettings shape_settings(desc.half_height, desc.radius);
  JPH::Shape::ShapeResult result = shape_settings.Create();

  if(!result.IsValid()) {
    NIKOLA_LOG_ERROR("Failed to create a capsule collider - %s", result.GetError().c_str());
    return ColliderID{};
  }

  s_world->shapes.push_back(result.Get());
  result.Get()->SetUserData((JPH::uint64)COLLIDER_CAPSULE);

  ColliderID coll_id = {
    ._type = COLLIDER_CAPSULE,
    ._id   = (u32)(s_world->shapes.size() - 1),
  };

  return coll_id;
}

/// Collider functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Character body functions

CharacterID character_body_create(const CharacterBodyDesc& desc) {
  COLLIDER_ID_CHECK(desc.collider_id);

  JPH::Ref<JPH::CharacterSettings> settings = new JPH::CharacterSettings();

  settings->mLayer         = (JPH::ObjectLayer)desc.layer; 
  settings->mMaxSlopeAngle = desc.max_slope_angle; 
  settings->mMass          = desc.mass; 
  settings->mFriction      = desc.friction; 
  settings->mGravityFactor = desc.gravity_factor;

  settings->mUp    = vec3_to_jph_vec3(desc.up_axis);
  settings->mShape = s_world->shapes[desc.collider_id._id];

  JPH::Ref<JPH::Character> character = new JPH::Character(settings, 
                                                          vec3_to_jph_vec3(desc.position), 
                                                          quat_to_jph_quat(desc.rotation), 
                                                          desc.user_data, 
                                                          &s_world->physics_system);
  s_world->characters.push_back(character);

  return CharacterID {
    ._id = (u32)(s_world->characters.size() - 1),
  };
}

void character_body_set_position(const CharacterID& char_id, const Vec3 position) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  character->SetPosition(vec3_to_jph_vec3(position));
}

void character_body_set_rotation(const CharacterID& char_id, const Quat rotation) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  character->SetRotation(quat_to_jph_quat(rotation));
}

void character_body_set_rotation(const CharacterID& char_id, const Vec3 axis, const f32 angle) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  character->SetRotation(quat_to_jph_quat(quat_angle_axis(axis, angle)));
}

void character_body_set_linear_velocity(const CharacterID& char_id, const Vec3 velocity) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  character->SetLinearVelocity(vec3_to_jph_vec3(velocity));
}

void character_body_set_layer(const CharacterID& char_id, const PhysicsObjectLayer layer) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  character->SetLayer((JPH::ObjectLayer)layer);
}

void character_body_set_slope_angle(const CharacterID& char_id, const f32 max_slope_angle) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  character->SetMaxSlopeAngle(max_slope_angle);
}

void character_body_set_collider(const CharacterID& char_id, const ColliderID& collider_id, const f32 max_penetration_depth) {
  CHARACTER_ID_CHECK(char_id);
  COLLIDER_ID_CHECK(collider_id); 

  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  character->SetShape(s_world->shapes[collider_id._id], max_penetration_depth);
}

void character_body_activate(const CharacterID& char_id) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  character->Activate();
}

const Vec3 character_body_get_position(const CharacterID& char_id) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  return jph_vec3_to_vec3(character->GetPosition());
}

const Vec3 character_body_get_com_position(const CharacterID& char_id) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  return jph_vec3_to_vec3(character->GetCenterOfMassPosition());
}

const Quat character_body_get_rotation(const CharacterID& char_id) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  return jph_quat_to_quat(character->GetRotation());
}

const Vec3 character_body_get_linear_velocity(const CharacterID& char_id) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  return jph_vec3_to_vec3(character->GetLinearVelocity());
}

const PhysicsObjectLayer character_body_get_layer(const CharacterID& char_id) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  return (PhysicsObjectLayer)character->GetLayer();
}

const f32 character_body_get_slope_angle(const CharacterID& char_id) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  return character->GetCosMaxSlopeAngle();
}

Transform character_body_get_transform(const CharacterID& char_id) {
  CHARACTER_ID_CHECK(char_id);
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];

  Transform transform;
  transform.position = jph_vec3_to_vec3(character->GetCenterOfMassPosition());
  transform.rotation = jph_quat_to_quat(character->GetRotation());

  transform_apply(transform);
  return transform;
}

const PhysicsBodyID character_body_get_body(const CharacterID& char_id) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  return PhysicsBodyID {
    ._id = character->GetBodyID().GetIndex(),
  };
}

GroundState character_body_query_ground_state(const CharacterID& char_id) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  return jph_ground_state_top_ground_state(character->GetGroundState());
}

void character_body_apply_linear_velocity(const CharacterID& char_id, const Vec3 velocity) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  character->AddLinearVelocity(vec3_to_jph_vec3(velocity));
}

void character_body_apply_impulse(const CharacterID& char_id, const Vec3 impulse) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  character->AddImpulse(vec3_to_jph_vec3(impulse));
}

const bool character_body_is_supported(const CharacterID& char_id) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  return character->IsSupported();
}

const bool character_body_is_slope_too_steep(const CharacterID& char_id, const Vec3 surface_normal) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  return character->IsSlopeTooSteep(vec3_to_jph_vec3(surface_normal));
}

const Vec3 character_body_get_ground_position(const CharacterID& char_id) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  return jph_vec3_to_vec3(character->GetGroundPosition());
}

const Vec3 character_body_get_ground_normal(const CharacterID& char_id) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  return jph_vec3_to_vec3(character->GetGroundNormal());
}

const Vec3 character_body_get_ground_velocity(const CharacterID& char_id) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  return jph_vec3_to_vec3(character->GetGroundVelocity());
}

const PhysicsBodyID character_body_get_ground_body(const CharacterID& char_id) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  return PhysicsBodyID {
    ._id = character->GetGroundBodyID().GetIndex(),
  };
}

const bool character_body_cast_ray(const CharacterID& char_id, const RayCastDesc& cast_desc) {
  CHARACTER_ID_CHECK(char_id);
  
  JPH::Ref<JPH::Character> character = s_world->characters[char_id._id];
  JPH::TransformedShape trans_shape  = character->GetTransformedShape();
 
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
