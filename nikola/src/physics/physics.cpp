#include "nikola/nikola_physics.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_math.h"
#include "nikola/nikola_containers.h"

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
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceTable.h>
#include <Jolt/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterTable.h>
#include <Jolt/Physics/Collision/ObjectLayerPairFilterTable.h>

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyInterface.h>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Physics ***

///---------------------------------------------------------------------------------------------------------------------
/// NKBodyActivationListener  
class NKBodyActivationListener : public JPH::BodyActivationListener
{
public:
	void OnBodyActivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override {
	}

	void OnBodyDeactivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override {
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
	}

	void OnContactPersisted(const JPH::Body& inBody1, 
                          const JPH::Body& inBody2, 
                          const JPH::ContactManifold& inManifold, 
                          JPH::ContactSettings& ioSettings) override {
	}

	void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override {
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
  Queue<JPH::BodyInterface::AddState> batches_queue;

  bool is_paused = false;
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
  return Quat(quat.GetX(), quat.GetY(), quat.GetZ(), quat.GetW());
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
  // @TEMP (Physics)
  
  s_world->obj_vs_obj_layer_table = new JPH::ObjectLayerPairFilterTable(PHYSICS_OBJECT_LAYERS_MAX);
  s_world->obj_vs_obj_layer_table->EnableCollision((JPH::ObjectLayer)PHYSICS_OBJECT_LAYER_0, 
                                                   (JPH::ObjectLayer)PHYSICS_OBJECT_LAYER_1);

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
  // Delete all the bodies in the world
  
  for(sizei i = 0; i < s_world->bodies.size(); i++) {
    JPH::BodyID body = s_world->bodies[i];

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

  s_world->physics_system.Update(delta_time, collision_steps, s_world->temp_allocater, s_world->job_system);
}

PhysicsBodyID physics_world_create_body(const PhysicsBodyDesc& desc) {
  NIKOLA_ASSERT((desc.collider_id._id != COLLIDER_ID_INVALID), "Invalid collider ID given to body creation function");

  // Create the body
  
  JPH::BodyCreationSettings body_settings(s_world->shapes[desc.collider_id._id].GetPtr(), 
                                          vec3_to_jph_vec3(desc.position), 
                                          quat_to_jph_quat(desc.rotation), 
                                          body_type_to_jph_body_type(desc.type), 
                                          (JPH::ObjectLayer)desc.layers);

  body_settings.mRestitution   = desc.restitution;
  body_settings.mFriction      = desc.friction;
  body_settings.mGravityFactor = desc.gravity_factor;
  body_settings.mIsSensor      = desc.is_sensor;  
  body_settings.mUserData      = desc.user_data ? (*(JPH::uint64*)desc.user_data) : 0;

  JPH::Body* body = s_world->body_interface->CreateBody(body_settings); 

  // Add the body to our internal buffer
  
  s_world->bodies.push_back(body->GetID());
  PhysicsBodyID body_id {
    ._id = (u32)(s_world->bodies.size() - 1),
  };

  return body_id;
}

void physics_world_add_body(const PhysicsBodyID& body_id, const bool is_active) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
 
  JPH::EActivation active = is_active ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
  JPH::BodyID body        = s_world->bodies[body_id._id];
  s_world->body_interface->AddBody(body, active);
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

  s_world->body_interface->AddBodiesFinalize(&s_world->bodies[bodies[0]._id], bodies_count, batch, active);
  s_world->batches_queue.pop();

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

void physics_body_set_position(PhysicsBodyID& body_id, const Vec3 position, const bool activate) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];

  JPH::EActivation active = activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
  s_world->body_interface->SetPosition(body, vec3_to_jph_vec3(position), active);
}

void physics_body_set_rotation(PhysicsBodyID& body_id, const Quat rotation, const bool activate) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];

  JPH::EActivation active = activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
  s_world->body_interface->SetRotation(body, quat_to_jph_quat(rotation), active);
}

void physics_body_set_rotation(PhysicsBodyID& body_id, const Vec3 axis, const f32 angle, const bool activate) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];
 
  JPH::EActivation active = activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
  JPH::Quat rotation      = quat_to_jph_quat(quat_angle_axis(axis, angle));
  
  s_world->body_interface->SetRotation(body, rotation, active);
}

void physics_body_set_transform(PhysicsBodyID& body_id, const Transform& transform, const bool activate) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];
 
  JPH::EActivation active = activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
  JPH::Vec3 position      = vec3_to_jph_vec3(transform.position);
  JPH::Quat rotation      = quat_to_jph_quat(transform.rotation);

  s_world->body_interface->SetPositionAndRotation(body, position, rotation, active);
}

void physics_body_set_linear_velocity(PhysicsBodyID& body_id, const Vec3 velocity) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];
  
  s_world->body_interface->SetLinearVelocity(body, vec3_to_jph_vec3(velocity));
}

void physics_body_set_angular_velocity(PhysicsBodyID& body_id, const Vec3 velocity) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];
  
  s_world->body_interface->SetAngularVelocity(body, vec3_to_jph_vec3(velocity));
}

void physics_body_set_active(PhysicsBodyID& body_id, const bool active) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];
 
  if(active) {
    s_world->body_interface->ActivateBody(body);
  }
  else {
    s_world->body_interface->DeactivateBody(body);
  }
}

void physics_body_set_user_data(PhysicsBodyID& body_id, const void* user_data) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  NIKOLA_ASSERT(user_data, "Passing invalid user data to body"); 

  JPH::BodyID body = s_world->bodies[body_id._id];
  s_world->body_interface->SetUserData(body, *((JPH::uint64*)user_data));
}

void physics_body_set_layer(PhysicsBodyID& body_id, const PhysicsObjectLayer layer) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];
  
  s_world->body_interface->SetObjectLayer(body, (JPH::ObjectLayer)layer);
}

void physics_body_set_restitution(PhysicsBodyID& body_id, const f32 restitution) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];
  
  s_world->body_interface->SetRestitution(body, restitution);
}

void physics_body_set_friction(PhysicsBodyID& body_id, const f32 friction) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];
  
  s_world->body_interface->SetFriction(body, friction);
}

void physics_body_set_gravity_factor(PhysicsBodyID& body_id, const f32 factor) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];
  
  s_world->body_interface->SetGravityFactor(body, factor);
}

void physics_body_set_type(PhysicsBodyID& body_id, const PhysicsBodyType type) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];
  
  s_world->body_interface->SetMotionType(body, body_type_to_jph_body_type(type), JPH::EActivation::Activate);
}

void physics_body_apply_linear_velocity(PhysicsBodyID& body_id, const Vec3 velocity) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];
  
  s_world->body_interface->AddLinearVelocity(body, vec3_to_jph_vec3(velocity));
}

void physics_body_apply_force(PhysicsBodyID& body_id, const Vec3 force) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];
  
  s_world->body_interface->AddForce(body, vec3_to_jph_vec3(force), JPH::EActivation::DontActivate);
}

void physics_body_apply_force_at(PhysicsBodyID& body_id, const Vec3 force, const Vec3 point) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];
  
  s_world->body_interface->AddForce(body, vec3_to_jph_vec3(force), vec3_to_jph_vec3(point));
}

void physics_body_apply_torque(PhysicsBodyID& body_id, const Vec3 torque) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];
  
  s_world->body_interface->AddTorque(body, vec3_to_jph_vec3(torque));
}

void physics_body_apply_impulse(PhysicsBodyID& body_id, const Vec3 impulse) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];
  
  s_world->body_interface->AddImpulse(body, vec3_to_jph_vec3(impulse));
}

void physics_body_apply_impulse_at(PhysicsBodyID& body_id, const Vec3 impulse, const Vec3 point) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];
  
  s_world->body_interface->AddImpulse(body, vec3_to_jph_vec3(impulse), vec3_to_jph_vec3(point));
}

void physics_body_apply_angular_impulse(PhysicsBodyID& body_id, const Vec3 impulse) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];
  
  s_world->body_interface->AddAngularImpulse(body, vec3_to_jph_vec3(impulse));
}

const Vec3 physics_body_get_position(const PhysicsBodyID& body_id) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];

  return jph_vec3_to_vec3(s_world->body_interface->GetPosition(body));
}

const Vec3 physics_body_get_com_position(const PhysicsBodyID& body_id) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];

  return jph_vec3_to_vec3(s_world->body_interface->GetCenterOfMassPosition(body));
}

const Quat physics_body_get_rotation(const PhysicsBodyID& body_id) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];

  return jph_quat_to_quat(s_world->body_interface->GetRotation(body));
}

const Vec3 physics_body_get_linear_velocity(const PhysicsBodyID& body_id) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];

  return jph_vec3_to_vec3(s_world->body_interface->GetLinearVelocity(body));
}

const Vec3 physics_body_get_angular_velocity(const PhysicsBodyID& body_id) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];

  return jph_vec3_to_vec3(s_world->body_interface->GetAngularVelocity(body));
}

const bool physics_body_is_active(const PhysicsBodyID& body_id) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];

  return s_world->body_interface->IsActive(body);
}

const void* physics_body_get_user_data(const PhysicsBodyID& body_id) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];

  return (const void*)s_world->body_interface->GetUserData(body);
}

const PhysicsObjectLayer physics_body_get_layer(const PhysicsBodyID& body_id) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];

  return (PhysicsObjectLayer)s_world->body_interface->GetObjectLayer(body);
}

const f32 physics_body_get_restitution(const PhysicsBodyID& body_id) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];

  return s_world->body_interface->GetRestitution(body);
}

const f32 physics_body_get_friction(const PhysicsBodyID& body_id) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];

  return s_world->body_interface->GetFriction(body);
}

const f32 physics_body_get_gravity_factor(const PhysicsBodyID& body_id) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];

  return s_world->body_interface->GetGravityFactor(body);
}

const PhysicsBodyType physics_body_get_type(const PhysicsBodyID& body_id) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
  JPH::BodyID body = s_world->bodies[body_id._id];

  return jph_body_type_to_body_type(s_world->body_interface->GetMotionType(body));
}

Transform physics_body_get_transform(const PhysicsBodyID& body_id) {
  NIKOLA_ASSERT((body_id._id < s_world->bodies.size()), "Trying to access a physics body that is non-existent in the world");
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
    return ColliderID{._id = COLLIDER_ID_INVALID};
  }

  s_world->shapes.push_back(result.Get());
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
    return ColliderID{._id = COLLIDER_ID_INVALID};
  }

  s_world->shapes.push_back(result.Get());
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
    return ColliderID{._id = COLLIDER_ID_INVALID};
  }

  s_world->shapes.push_back(result.Get());
  ColliderID coll_id = {
    ._type = COLLIDER_CAPSULE,
    ._id   = (u32)(s_world->shapes.size() - 1),
  };

  return coll_id;
}

/// Collider functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Physics ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
