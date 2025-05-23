#include "nikola/nikola_physics.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_math.h"
#include "nikola/nikola_containers.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Physics ***

///---------------------------------------------------------------------------------------------------------------------
/// CollisionData
struct CollisionData {
  PhysicsBody* body_a;
  PhysicsBody* body_b;

  CollisionPoint point;
};
/// CollisionData
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsWorld
struct PhysicsWorld {
  Vec3 gravity; 

  DynamicArray<PhysicsBody*> bodies;
  DynamicArray<CollisionData> collisions;

  OnCollisionFunc collision_callback = nullptr;
};

static PhysicsWorld s_world;
/// PhysicsWorld
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void apply_forces(const f32 timestep) {
  /*
   *
   * @NOTE (23/5/2025, Mohamed):
   *
   * This physics system uses the Semi-Implicit Euler integration system. 
   * It is perhaps not the best/accurate integration out there. However, 
   * it does satisfy the needs of a real-time game physics simulation. 
   * It's fast, easy to use, and accurate enough for game simulations. 
   *
   * Here's a link for more information: 
   * https://en.wikipedia.org/wiki/Semi-implicit_Euler_method
   *
   */

  f32 damp_factor = 1.0f - 0.95f;
  f32 frame_damp  = nikola::pow(damp_factor, timestep);

  for(auto& body : s_world.bodies) {
    // Inactive and static bodies do not need to be updated 
    if(!body->is_awake || !body->is_dynamic) {
      continue;
    }

    // Apply the acceleration
    Vec3 acceleration = body->force * body->inverse_mass;

    // Don't apply gravity to infinitely heavy bodies
    if(body->inverse_mass < 0) {  
      acceleration += s_world.gravity; 
    }

    // Semi-Implicit Euler in effect
    body->linear_velocity    += acceleration * timestep;
    body->linear_velocity    *= frame_damp;
    body->transform.position += body->linear_velocity * timestep;
    
    // Adding angular velocity 
    Vec3 angular_accel      = body->torque * body->inertia_tensor;
    body->angular_velocity += angular_accel * timestep;
    body->angular_velocity *= frame_damp; // Apply some damping to the angular velocity as well 

    // Adding the rotation to the body 
    Quat orientation = body->transform.rotation;
    orientation     += (Quat(0.0f, body->angular_velocity * timestep * 0.5f) * orientation);
    orientation      = quat_normalize(orientation);

    // Moving the body by the new position/displacment and rotating it as weel
    transform_translate(body->transform, body->transform.position); 
    transform_rotate(body->transform, orientation);

    // Clear all forces accumulated this frame
    body->force = Vec3(0.0f); 
  } 
}

static void check_collisions() {
  // @TODO (Physics): There is no need to say that this is awful. 
  // Going through each body in the world and checking if it collides 
  // with _every other_ body is insane to say the least. 
  // There should be an "island" system in place as to only check collisions
  // against the bodies that are close together. 

  for(sizei i = 0; i < s_world.bodies.size(); i++) {
    PhysicsBody* body_a = s_world.bodies[i];
    if(!body_a->is_awake) {
      continue;
    }

    for(sizei j = 0; j < s_world.bodies.size(); j++) {
      PhysicsBody* body_b = s_world.bodies[i];
      if(!body_b->is_awake || i == j) {
        continue;
      }


      CollisionPoint point = collider_check_collision(body_a->collider, body_a->transform, body_b->collider, body_b->transform);
      if(!point.has_collided) {
        continue;
      }

      // Invoke the collision callback if one is available
      if(s_world.collision_callback) {
        s_world.collision_callback(body_a, body_b);
      } 

      // If both colliders are not triggers, we can add 
      // the collision data to be resolved later.
      if(!body_a->collider.is_trigger && !body_b->collider.is_trigger) {
        CollisionData data = {
          .body_a = body_a, 
          .body_b = body_b, 
          .point  = point,
        };
        s_world.collisions.push_back(data);
      } 
    }
  }
}

static void resolve_collisions() {
  for(auto& collision : s_world.collisions) {
    PhysicsBody* body_a = collision.body_a;
    PhysicsBody* body_b = collision.body_b;
 
    f32 sum_mass = body_a->inverse_mass + body_b->inverse_mass;

    // Move the bodies away from each other.
    // We're basically pushing the two bodies away from each other taking into account the normal, depth, and masses.
    // Bodies with more mass will be pushed away less than lighter bodies. 
    if(body_a->is_dynamic) {
      Vec3 new_pos_a = body_a->transform.position - ((collision.point.normal * collision.point.depth) * (body_a->inverse_mass / sum_mass));
      transform_translate(body_a->transform, new_pos_a); 
    }

    if(body_b->is_dynamic) {
      Vec3 new_pos_b = body_b->transform.position + ((collision.point.normal * collision.point.depth) * (body_b->inverse_mass / sum_mass));
      transform_translate(body_b->transform, new_pos_b); 
    }

    // Integrate the Impulse Method for collision response
    Vec3 rel_pos_a = collision.point.point_a - body_a->transform.position; 
    Vec3 rel_pos_b = collision.point.point_b - body_b->transform.position; 

    Vec3 ang_vel_a = vec3_cross(body_a->angular_velocity, rel_pos_a);
    Vec3 ang_vel_b = vec3_cross(body_b->angular_velocity, rel_pos_b);

    Vec3 full_vel_a = body_a->linear_velocity + ang_vel_a;
    Vec3 full_vel_b = body_b->linear_velocity + ang_vel_b;

    Vec3 contact_vel = full_vel_b - full_vel_a;

    Vec3 inertia_a = vec3_cross(body_a->inertia_tensor * vec3_cross(rel_pos_a, collision.point.normal), rel_pos_a);
    Vec3 inertia_b = vec3_cross(body_b->inertia_tensor * vec3_cross(rel_pos_b, collision.point.normal), rel_pos_b);
    f32 angular_effect = vec3_dot(inertia_a + inertia_b, collision.point.normal); 

    f32 restitution = body_a->restitution * body_b->restitution;

    f32 impulse_force = vec3_dot(contact_vel, collision.point.normal);
    f32 impulse = (-(1.0f + restitution) * impulse_force) / sum_mass; // @TODO (Physics): + angular_effect);

    // The resulting impulse
    Vec3 full_impulse = impulse * collision.point.normal;

    // Giving impulse to the two bodies based on the mass
    physics_body_apply_linear_impulse(body_a, -full_impulse); 
    physics_body_apply_linear_impulse(body_b, full_impulse); 

    // @TODO (Physics): Have to get the exact collision point in order for this to work
    // physics_body_apply_angular_impulse(body_a, vec3_cross(rel_pos_a, -full_impulse));
    // physics_body_apply_angular_impulse(body_b, vec3_cross(rel_pos_b, full_impulse));
  }

  // Empty out the collisions after resolving all of them
  s_world.collisions.clear();
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Physics world functions

void physics_world_init(const Vec3& gravity) {
  s_world.gravity = gravity;
  s_world.bodies.reserve(32);
}

void physics_world_shutdown() {
  for(auto& body : s_world.bodies) {
    physics_body_destroy(body);
  }

  s_world.bodies.clear();
}

void physics_world_set_gravity(const Vec3& gravity) {
  s_world.gravity = gravity;
}

Vec3& physics_world_get_gravity() {
  return s_world.gravity;
}

void physics_world_set_callback(const OnCollisionFunc& callback) {
  s_world.collision_callback = callback; 
}

void physics_world_step(const f64 timestep) {
  apply_forces((f32)timestep);
  check_collisions();
  resolve_collisions();
}

void physics_world_add_body(PhysicsBody* body) {
  NIKOLA_ASSERT(body, "Attempting to add an invalid PhysicsBody to the world");

  s_world.bodies.push_back(body);
}

/// Physics world functions
///---------------------------------------------------------------------------------------------------------------------


/// *** Physics ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
