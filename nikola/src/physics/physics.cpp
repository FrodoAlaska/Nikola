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
/// Private functions declarations

static Vec3 q3vec_to_vec(const q3Vec3& vec);
static Quat q3quaternion_to_quat(const q3Quaternion& quat);
static Mat3 q3mat_to_mat(const q3Mat3& mat);
static Transform q3transform_to_transform(const q3Transform& trans);
static q3Vec3 vec_to_q3vec(const Vec3& vec);
static q3Quaternion quat_to_q3quaternion(const Quat& quat);
static q3Mat3 mat_to_q3mat(const Mat3& mat);
static q3BodyType body_type_to_q3body_type(const PhysicsBodyType type);

/// Private functions declarations
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBody 
struct PhysicsBody {
  u64 id;
  q3Body* body; 

  PhysicsBodyType type;

  void* user_data;
};
/// PhysicsBody 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Collider
struct Collider {
  sizei world_index;
  
  q3Box* box; 
  PhysicsBody* body;

  Transform local; 
  Vec3 extents; 

  void* user_data;
};
/// Collider
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsWorld
struct PhysicsWorld {
  q3Scene* scene = nullptr;
  bool is_paused = false;

  OnCollisionFunc begin_func;
  OnCollisionFunc end_func; 
  OnRayIntersectionFunc ray_func;

  HashMap<u64, PhysicsBody*> bodies; 
  DynamicArray<Collider*> colliders;
};

static PhysicsWorld s_world{};
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
      .body_a = ((PhysicsBody*)contact->bodyA->GetUserData()),
      .body_b = ((PhysicsBody*)contact->bodyB->GetUserData()),
    
      .coll_a = ((Collider*)contact->A->GetUserdata()),
      .coll_b = ((Collider*)contact->B->GetUserdata()),
    };

    s_world.begin_func(point);
  }

	void EndContact(const q3ContactConstraint* contact) {
    CollisionPoint point = {
      .body_a = ((PhysicsBody*)contact->bodyA->GetUserData()),
      .body_b = ((PhysicsBody*)contact->bodyB->GetUserData()),
    
      .coll_a = ((Collider*)contact->A->GetUserdata()),
      .coll_b = ((Collider*)contact->B->GetUserdata()),
    };

    s_world.end_func(point);
  }
};
/// ContanctListener
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// QueryCallback 
class QueryCallback : public q3QueryCallback
{
  public:
    q3RaycastData data;

  public:
    void Init(const q3RaycastData& raycast) {
      data = raycast;
    }

    bool ReportShape( q3Box *box ) {
      if(!box->Raycast(box->body->GetTransform(), &data)) {
        return false;
      }
      

      Ray ray = {
        .position  = q3vec_to_vec(data.start),
        .direction = q3vec_to_vec(data.dir),
      };

      RayIntersection intersect = {
        .point           = q3vec_to_vec(data.GetImpactPoint()),
        .normal          = q3vec_to_vec(data.normal),
        .time_of_impact  = data.toi,
        .has_intersected = true,
      };
      
      if(s_world.ray_func) {
        Collider* coll = ((Collider*)box->GetUserdata());
        s_world.ray_func(ray, intersect, coll);
      }

      return true;
    }
};
/// QueryCallback 
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
 
  s_world.bodies.reserve(16);
  s_world.colliders.reserve(16);

  NIKOLA_LOG_INFO("The physics world was successfully initialized");
}

void physics_world_shutdown() {
  if(!s_world.scene) {
    return;
  };

  // Shuting down the scene 
  s_world.scene->Shutdown();
 
  // Clearing all bodies
  for(auto& [id, body] : s_world.bodies) {
    delete body; 
  }
  s_world.bodies.clear();
  
  // Clearing all colliders
  for(auto& collider : s_world.colliders) {
    delete collider; 
  }
  s_world.colliders.clear();

  // Bye bye, world
  delete s_world.scene;
  
  NIKOLA_LOG_INFO("The physics world was successfully shutdown"); 
}

void physics_world_step() {
  if(s_world.is_paused) {
    return;
  }

  s_world.scene->Step();
}

void physics_world_set_paused(const bool paused) {
  s_world.is_paused = paused;
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

void physics_world_check_raycast(const Ray& ray, const OnRayIntersectionFunc& ray_func) {
  s_world.ray_func = ray_func;

  q3RaycastData data;
  data.Set(vec_to_q3vec(ray.position), vec_to_q3vec(ray.direction), 10000.0f);

  QueryCallback callback; 
  callback.Init(data);

  s_world.scene->RayCast(&callback, data);
}

Vec3 physics_world_get_gravity() {
  return q3vec_to_vec(s_world.scene->GetGravity());
}

/// Physics world functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBody functions

PhysicsBody* physics_body_create(const PhysicsBodyDesc& desc) {
  // Allocate a new body
  PhysicsBody* body = new PhysicsBody{};

  // q3Body init
  q3BodyDef def; 
  def.position  = vec_to_q3vec(desc.position);
  def.bodyType  = body_type_to_q3body_type(desc.type);
  def.axis      = vec_to_q3vec(desc.rotation_axis);
  def.angle     = desc.rotation_angle;
  def.awake     = desc.is_awake;
  def.userData  = body;
  def.lockAxisX = desc.locked_axises.x;
	def.lockAxisY = desc.locked_axises.y;
	def.lockAxisZ = desc.locked_axises.z;

  // Physics body init
  body->body      = s_world.scene->CreateBody(def);
  body->user_data = desc.user_data;
  body->type      = desc.type;
  body->id        = random_u64();

  // Add the body to the world
  s_world.bodies[body->id] = body;

  return body;
}

void physics_body_destroy(PhysicsBody* body) {
  if(!body) {
    return;
  }

  s_world.scene->RemoveBody(body->body);
  s_world.bodies.erase(body->id);

  delete body;
}

Collider* physics_body_add_collider(PhysicsBody* body, const ColliderDesc& desc) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_add_collider");

  // Allocate a new collider
  Collider* coll = new Collider{};

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
  const q3Box* box = body->body->AddBox(def);
  box->SetUserdata(coll);

  // Add the box
  coll->box       = (q3Box*)box;
  coll->body      = body; 
  coll->local     = q3transform_to_transform(box->local);
  coll->extents   = desc.extents;
  coll->user_data = desc.user_data;
  
  // Add the collider to the world
  coll->world_index = s_world.colliders.size();
  s_world.colliders.push_back(coll);

  return coll;
}

void physics_body_remove_collider(PhysicsBody* body, Collider* coll) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_remove_collider");
  NIKOLA_ASSERT(coll, "Invalid collider given to physics_body_remove_collider");

  body->body->RemoveBox(coll->box);
  s_world.colliders.erase(s_world.colliders.begin() + coll->world_index);

  delete coll;
}

void physics_body_apply_force(PhysicsBody* body, const Vec3& force) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_apply_force");
  
  body->body->ApplyLinearForce(vec_to_q3vec(force));
}

void physics_body_apply_force_at(PhysicsBody* body, const Vec3& force, const Vec3& point) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_apply_force_at");
  
  body->body->ApplyForceAtWorldPoint(vec_to_q3vec(force), vec_to_q3vec(point));
}

void physics_body_apply_impulse(PhysicsBody* body, const Vec3& impulse) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_apply_impulse");
  
  body->body->ApplyLinearImpulse(vec_to_q3vec(impulse));
}

void physics_body_apply_impulse_at(PhysicsBody* body, const Vec3& impulse, const Vec3& point) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_apply_impulse_at");
  
  body->body->ApplyLinearImpulseAtWorldPoint(vec_to_q3vec(impulse), vec_to_q3vec(point));
}

void physics_body_apply_torque(PhysicsBody* body, const Vec3& torque) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_apply_torque");
  
  body->body->ApplyTorque(vec_to_q3vec(torque));
}

void physics_body_set_position(PhysicsBody* body, const Vec3& pos) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_set_position");
  
  body->body->SetTransform(vec_to_q3vec(pos));
}

void physics_body_set_rotation(PhysicsBody* body, const Vec3& axis, const f32 angle) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_set_rotation");
  
  q3Vec3 pos = vec_to_q3vec(physics_body_get_transform(body).position);
  body->body->SetTransform(pos, vec_to_q3vec(axis), angle);
}

void physics_body_set_linear_velocity(PhysicsBody* body, const Vec3& vel) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_set_linear_velocity");
  
  body->body->SetLinearVelocity(vec_to_q3vec(vel));
}

void physics_body_set_angular_velocity(PhysicsBody* body, const Vec3& vel) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_set_angular_velocity");
  
  body->body->SetAngularVelocity(vec_to_q3vec(vel));
}

void physics_body_set_awake(PhysicsBody* body, const bool awake) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_set_awake");
  
  if(awake) {
    body->body->SetToAwake();
  }
  else {
    body->body->SetToSleep();
  }
}

PhysicsBodyType physics_body_get_type(const PhysicsBody* body) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_get_type");
  
  return body->type;
}

Vec3 physics_body_get_position(const PhysicsBody* body) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_get_position");
  
  return q3vec_to_vec(body->body->GetTransform().position);
}

Vec4 physics_body_get_rotation(const PhysicsBody* body) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_get_rotation");
  
  q3Quaternion quat_rot = body->body->GetQuaternion();

  q3Vec3 axis; 
  f32 angle; 
  quat_rot.ToAxisAngle(&axis, &angle); 

  return Vec4(q3vec_to_vec(axis), angle);
}

Quat physics_body_get_quaternion(const PhysicsBody* body) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_get_quaternion");
  
  return q3quaternion_to_quat(body->body->GetQuaternion());
}

Transform physics_body_get_transform(const PhysicsBody* body) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_get_transform");
  
  return q3transform_to_transform(body->body->GetTransform());
}

Vec3 physics_body_get_linear_velocity(const PhysicsBody* body) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_get_linear_velocity");
  
  return q3vec_to_vec(body->body->GetLinearVelocity());
}

Vec3 physics_body_get_angular_velocity(const PhysicsBody* body) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_get_angular_velocity");
  
  return q3vec_to_vec(body->body->GetAngularVelocity());
}

bool physics_body_is_awake(const PhysicsBody* body) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_is_awake");
  
  return body->body->IsAwake();
}

void* physics_body_get_user_data(const PhysicsBody* body) {
  NIKOLA_ASSERT(body, "Invalid body given to physics_body_get_user_data");
  
  return body->user_data;
}

/// PhysicsBody functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Collider functions

RayIntersection collider_check_raycast(const Collider* coll, const Ray& ray) {
  NIKOLA_ASSERT(coll, "Invalid collider given to collider_check_raycast");
  
  RayIntersection intersect = {};
  q3Box* collider           = coll->box;
  
  // Set the Qu3e raycast
  q3RaycastData raycast;
  raycast.Set(vec_to_q3vec(ray.position), vec_to_q3vec(ray.direction), 10000.0f);

  // Check the intersection
	intersect.has_intersected = collider->Raycast(collider->body->GetTransform(), &raycast);
  
  // No point to keep going if there is not intersection
  if(!intersect.has_intersected) {
    return intersect;
  }

  // Fill in some useful information
  intersect.point          = q3vec_to_vec(raycast.GetImpactPoint());    
  intersect.normal         = q3vec_to_vec(raycast.normal);
  intersect.time_of_impact = raycast.toi;   

  return intersect;
}

void collider_set_extents(Collider* coll, const Vec3& extents) {
  NIKOLA_ASSERT(coll, "Invalid collider given to collider_set_extents");
  
  coll->extents = extents;
  coll->box->e  = vec_to_q3vec(extents);
}

void collider_set_local_position(Collider* coll, const Vec3& local_pos) {
  NIKOLA_ASSERT(coll, "Invalid collider given to collider_set_local_position");
  
  coll->box->local.position = vec_to_q3vec(local_pos);
}

void collider_set_friction(Collider* coll, const f32 friction) {
  NIKOLA_ASSERT(coll, "Invalid collider given to collider_set_friction");
  
  coll->box->friction = friction;
}

void collider_set_restitution(Collider* coll, const f32 restitution) {
  NIKOLA_ASSERT(coll, "Invalid collider given to collider_set_restitution");
  
  coll->box->restitution = restitution;
}

void collider_set_density(Collider* coll, const f32 density) {
  NIKOLA_ASSERT(coll, "Invalid collider given to collider_set_density");
  
  coll->box->density = density;
}

void collider_set_user_data(Collider* coll, const void* user_data) {
  NIKOLA_ASSERT(coll, "Invalid collider given to collider_set_user_data");
  
  coll->user_data = (void*)user_data;
}

PhysicsBody* collider_get_attached_body(const Collider* coll) {
  NIKOLA_ASSERT(coll, "Invalid collider given to collider_get_attached_body");
  
  return coll->body;
}

Vec3 collider_get_extents(const Collider* coll) {
  NIKOLA_ASSERT(coll, "Invalid collider given to collider_get_extents");
  
  return coll->extents;
}

f32 collider_get_friction(const Collider* coll) {
  NIKOLA_ASSERT(coll, "Invalid collider given to collider_get_friction");
  
  return coll->box->friction;
}

f32 collider_get_restitution(const Collider* coll) {
  NIKOLA_ASSERT(coll, "Invalid collider given to collider_get_restitution");
  
  return coll->box->restitution;
}

f32 collider_get_density(const Collider* coll) {
  NIKOLA_ASSERT(coll, "Invalid collider given to collider_get_density");
  
  return coll->box->density;
}

bool collider_get_sensor(const Collider* coll) {
  NIKOLA_ASSERT(coll, "Invalid collider given to collider_get_sensor");
  
  return coll->box->sensor;
}

void* collider_get_user_data(const Collider* coll) {
  NIKOLA_ASSERT(coll, "Invalid collider given to collider_get_user_data");
  
  return coll->user_data;
}

Transform collider_get_local_transform(const Collider* coll) {
  NIKOLA_ASSERT(coll, "Invalid collider given to collider_get_local_transform");
  
  return q3transform_to_transform(coll->box->local);
}

Transform collider_get_world_transform(const Collider* coll) {
  NIKOLA_ASSERT(coll, "Invalid collider given to collider_get_world_transform");
  
  return q3transform_to_transform(q3Mul(coll->box->body->GetTransform(), coll->box->local));
}

/// Collider functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Physics ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
