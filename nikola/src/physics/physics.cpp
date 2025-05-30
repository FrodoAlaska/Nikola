#include "nikola/nikola_physics.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_math.h"
#include "nikola/nikola_containers.h"

#include <q3.h>
#include <dynamics/q3Contact.h>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Physics ***

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBody 
struct PhysicsBody {
  PhysicsBodyID id;
  q3Body* body; 

  Transform transform;
  Vec3 linear_velocity; 
  Vec3 angular_velocity;

  void* user_data;
};
/// PhysicsBody 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Collider
struct Collider {
  ColliderID id;
  q3Box* box; 

  Transform local; 
  Vec3 extents; 

  void* user_data;
};
/// Collider
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsWorld
struct PhysicsWorld {
  q3Scene* scene;

  OnCollisionFunc begin_func;
  OnCollisionFunc end_func; 

  HashMap<PhysicsBodyID, PhysicsBody> bodies; 
  HashMap<ColliderID, Collider> colliders; 
};

static PhysicsWorld s_world;
/// PhysicsWorld
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ContanctListener
///
/// @NOTE (30/5/2025, Mohamed): SADLY, qu3e requires sub-classing this class for any information 
/// on contacts between bodies and/or colliders. It is VERY out of place, but 
/// what can you do?
///
class ContanctListener : public q3ContactListener
{
	void BeginContact(const q3ContactConstraint *contact) {
    CollisionPoint point = {
      .body_a = ((PhysicsBody*)contact->bodyA->GetUserData())->id,
      .body_b = ((PhysicsBody*)contact->bodyB->GetUserData())->id,
    
      .coll_a = ((Collider*)contact->A->GetUserdata())->id,
      .coll_b = ((Collider*)contact->B->GetUserdata())->id,
    };

    s_world.begin_func(point);
  }

	void EndContact(const q3ContactConstraint* contact) {
    CollisionPoint point = {
      .body_a = ((PhysicsBody*)contact->bodyA->GetUserData())->id,
      .body_b = ((PhysicsBody*)contact->bodyB->GetUserData())->id,
    
      .coll_a = ((Collider*)contact->A->GetUserdata())->id,
      .coll_b = ((Collider*)contact->B->GetUserdata())->id,
    };

    s_world.end_func(point);
  }
};
/// ContanctListener
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Callbacks 

static void on_collision_begin(const CollisionPoint& point) {
  // @NOTE: Placeholder collision callback
}

static void on_collision_end(const CollisionPoint& point) {
  // @NOTE: Placeholder collision callback
}

/// Callbacks 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static Vec3 q3vec_to_vec(const q3Vec3& vec) {
  return Vec3(vec.x, vec.y, vec.z);
}

static Quat q3quaternion_to_quat(const q3Quaternion& quat) {
  return Quat(quat.w, quat.x, quat.y, quat.z);
}

static Mat3 q3mat_to_mat(const q3Mat3& mat) {
  Vec3 x = q3vec_to_vec(mat.ex);
  Vec3 y = q3vec_to_vec(mat.ey);
  Vec3 z = q3vec_to_vec(mat.ez);

  return Mat3(x, y, z);
}

static Transform q3transform_to_transform(const q3Transform& trans) {
  Vec3 pos = q3vec_to_vec(trans.position);
  Mat3 rot = q3mat_to_mat(trans.rotation);

  Transform transform; 
  transform_translate(transform, pos);
  transform_rotate(transform, quat_set_mat3(rot));

  return transform;
}

static q3Vec3 vec_to_q3vec(const Vec3& vec) {
  return q3Vec3(vec.x, vec.y, vec.z);
}

static q3Quaternion quat_to_q3quaternion(const Quat& quat) {
  return q3Quaternion(quat.x, quat.y, quat.z, quat.w);
}

static q3Mat3 mat_to_q3mat(const Mat3& mat) {
  q3Vec3 ex = vec_to_q3vec(mat[0]);
  q3Vec3 ey = vec_to_q3vec(mat[1]);
  q3Vec3 ez = vec_to_q3vec(mat[2]);

  return q3Mat3(ex, ey, ez);
}

static q3BodyType body_type_to_q3body_type(const PhysicsBodyType type) {
  switch(type) {
    case PHYSICS_BODY_STATIC:
      return eStaticBody;
    case PHYSICS_BODY_DYNAMIC:
      return eDynamicBody;
    case PHYSICS_BODY_KINEMATIC:
      return eKinematicBody;
  }
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Physics world functions

void physics_world_init(const Vec3& gravity, const f32 timestep) {
  s_world.scene = new q3Scene(timestep, vec_to_q3vec(gravity));

  s_world.begin_func = on_collision_begin;
  s_world.end_func   = on_collision_end;
  s_world.scene->SetContactListener(new ContanctListener);
}

void physics_world_shutdown() {
  if(!s_world.scene) {
    return;
  };

  s_world.scene->Shutdown();
  s_world.bodies.clear();
  s_world.colliders.clear();
 
  delete s_world.scene;
}

void physics_world_step() {
  s_world.scene->Step();
}

void physics_world_set_gravity(const Vec3& gravity) {
  s_world.scene->SetGravity(vec_to_q3vec(gravity));
}

void physics_world_set_iterations_count(const i32 iterations) {
  s_world.scene->SetIterations(iterations); 
}

void physics_world_set_collision_callback(const OnCollisionFunc& begin_func, const OnCollisionFunc& end_func) {
  s_world.begin_func = !begin_func ? on_collision_begin : begin_func;
  s_world.end_func   = !end_func ? on_collision_end : end_func;
}

Vec3 physics_world_get_gravity() {
  return q3vec_to_vec(s_world.scene->GetGravity());
}

/// Physics world functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBody functions

PhysicsBodyID physics_body_create(const PhysicsBodyDesc& desc) {
  // Add the body to the world
  PhysicsBodyID id   = random_u64();
  s_world.bodies[id] = PhysicsBody{.id = id}; 

  // q3Body init
  q3BodyDef def; 
  def.position = vec_to_q3vec(desc.position);
  def.bodyType = body_type_to_q3body_type(desc.type);
  def.axis     = vec_to_q3vec(desc.rotation_axis);
  def.angle    = desc.rotation_angle;
  def.awake    = desc.is_awake;
  def.userData = &s_world.bodies[id];

  // Physics body init
  s_world.bodies[id].body             = s_world.scene->CreateBody(def);
  s_world.bodies[id].transform        = q3transform_to_transform(s_world.bodies[id].body->GetTransform());
  s_world.bodies[id].linear_velocity  = q3vec_to_vec(def.linearVelocity); 
  s_world.bodies[id].angular_velocity = q3vec_to_vec(def.angularVelocity); 
  s_world.bodies[id].user_data        = desc.user_data;

  return id;
}

void physics_body_destroy(PhysicsBodyID& id) {
  s_world.scene->RemoveBody(s_world.bodies[id].body);
  s_world.bodies.erase(id);
}

ColliderID physics_body_add_collider(PhysicsBodyID& id, const ColliderDesc& desc) {
  // Add the collider to the body
  ColliderID coll_id         = random_u64();
  s_world.colliders[coll_id] = Collider{.id = coll_id};

  // Transform init
  q3Transform trans; 
  q3Identity(trans);
  trans.position = vec_to_q3vec(desc.position);

  // Box definition init
  q3BoxDef def;
  def.Set(trans, vec_to_q3vec(desc.extents));
  def.SetFriction(desc.friction);
  def.SetRestitution(desc.restitution);
  def.SetDensity(desc.density);
  def.SetSensor(desc.is_sensor);

  // Box init
  const q3Box* box = s_world.bodies[id].body->AddBox(def);
  box->SetUserdata(&s_world.colliders[coll_id]);

  // Add the box
  s_world.colliders[coll_id].box       = (q3Box*)box;
  s_world.colliders[coll_id].local     = q3transform_to_transform(box->local);
  s_world.colliders[coll_id].extents   = desc.extents;
  s_world.colliders[coll_id].user_data = desc.user_data;

  return coll_id;
}

void physics_body_remove_collider(PhysicsBodyID& id, const ColliderID& coll_id) {
  s_world.bodies[id].body->RemoveBox(s_world.colliders[coll_id].box);
  s_world.colliders.erase(coll_id);
}

void physics_body_apply_force(PhysicsBodyID& id, const Vec3& force) {
  s_world.bodies[id].body->ApplyLinearForce(vec_to_q3vec(force));
}

void physics_body_apply_force_at(PhysicsBodyID& id, const Vec3& force, const Vec3& point) {
  s_world.bodies[id].body->ApplyForceAtWorldPoint(vec_to_q3vec(force), vec_to_q3vec(point));
}

void physics_body_apply_impulse(PhysicsBodyID& id, const Vec3& impulse) {
  s_world.bodies[id].body->ApplyLinearImpulse(vec_to_q3vec(impulse));
}

void physics_body_apply_impulse_at(PhysicsBodyID& id, const Vec3& impulse, const Vec3& point) {
  s_world.bodies[id].body->ApplyLinearImpulseAtWorldPoint(vec_to_q3vec(impulse), vec_to_q3vec(point));
}

void physics_body_apply_torque(PhysicsBodyID& id, const Vec3& torque) {
  s_world.bodies[id].body->ApplyTorque(vec_to_q3vec(torque));
}

void physics_body_set_position(PhysicsBodyID& id, const Vec3& pos) {
  s_world.bodies[id].body->SetTransform(vec_to_q3vec(pos));
  transform_translate(s_world.bodies[id].transform, pos);
}

void physics_body_set_rotation(PhysicsBodyID& id, const Vec3& axis, const f32 angle) {
  q3Vec3 pos = vec_to_q3vec(physics_body_get_transform(id).position);
  s_world.bodies[id].body->SetTransform(pos, vec_to_q3vec(axis), angle);
  
  transform_rotate(s_world.bodies[id].transform, axis, angle);
}

void physics_body_set_linear_velocity(PhysicsBodyID& id, const Vec3& vel) {
  s_world.bodies[id].body->SetLinearVelocity(vec_to_q3vec(vel));
  s_world.bodies[id].linear_velocity = vel;
}

void physics_body_set_angular_velocity(PhysicsBodyID& id, const Vec3& vel) {
  s_world.bodies[id].body->SetAngularVelocity(vec_to_q3vec(vel));
  s_world.bodies[id].angular_velocity = vel;
}

void physics_body_set_awake(PhysicsBodyID& id, const bool awake) {
  if(awake) {
    s_world.bodies[id].body->SetToAwake();
  }
  else {
    s_world.bodies[id].body->SetToSleep();
  }
}

Vec3& physics_body_get_position(const PhysicsBodyID& id) {
  return s_world.bodies[id].transform.position;
}

Quat& physics_body_get_quaternion(const PhysicsBodyID& id) {
  return s_world.bodies[id].transform.rotation;
}

Transform& physics_body_get_transform(const PhysicsBodyID& id) {
  s_world.bodies[id].transform = q3transform_to_transform(s_world.bodies[id].body->GetTransform());
  return s_world.bodies[id].transform;
}

Vec3& physics_body_get_linear_velocity(const PhysicsBodyID& id) {
  return s_world.bodies[id].linear_velocity;
}

Vec3& physics_body_get_angular_velocity(const PhysicsBodyID& id) {
  return s_world.bodies[id].angular_velocity;
}

bool physics_body_is_awake(const PhysicsBodyID& id) {
  return s_world.bodies[id].body->IsAwake();
}

void* physics_body_get_user_data(const PhysicsBodyID& id) {
  return s_world.bodies[id].user_data;
}

/// PhysicsBody functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Collider functions

void collider_set_extents(ColliderID& id, const Vec3& extents) {
  s_world.colliders[id].extents = extents;
  s_world.colliders[id].box->e  = vec_to_q3vec(extents);
}

void collider_set_friction(ColliderID& id, const f32 friction) {
  s_world.colliders[id].box->friction = friction;
}

void collider_set_restitution(ColliderID& id, const f32 restitution) {
  s_world.colliders[id].box->restitution = restitution;
}

void collider_set_density(ColliderID& id, const f32 density) {
  s_world.colliders[id].box->density = density;
}

void collider_set_user_data(ColliderID& id, const void* user_data) {
  s_world.colliders[id].user_data = (void*)user_data;
}

Vec3& collider_get_extents(const ColliderID& id) {
  return s_world.colliders[id].extents;
}

f32 collider_get_friction(const ColliderID& id) {
  return s_world.colliders[id].box->friction;
}

f32 collider_get_restitution(const ColliderID& id) {
  return s_world.colliders[id].box->restitution;
}

f32 collider_get_density(const ColliderID& id) {
  return s_world.colliders[id].box->density;
}

bool collider_get_sensor(const ColliderID& id) {
  return s_world.colliders[id].box->sensor;
}

void* collider_get_user_data(const ColliderID& id) {
  return s_world.colliders[id].user_data;
}

Transform& collider_get_local_transform(const ColliderID& id) {
  return s_world.colliders[id].local;
}

Transform collider_get_world_transform(const ColliderID& id) {
  return q3transform_to_transform(q3Mul(s_world.colliders[id].box->body->GetTransform(), s_world.colliders[id].box->local));
}

/// Collider functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Physics ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
