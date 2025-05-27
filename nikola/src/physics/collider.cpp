#include "nikola/nikola_physics.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Physics ***

///---------------------------------------------------------------------------------------------------------------------
/// Collider functions

CollisionPoint collider_check_collision(const BoxCollider& coll_a, const Vec3& pos_a, const BoxCollider& coll_b, const Vec3& pos_b) {
  Vec3 min_a = pos_a - coll_a.half_size;
  Vec3 max_a = pos_a + coll_a.half_size;
  
  Vec3 min_b = pos_b - coll_b.half_size;
  Vec3 max_b = pos_b + coll_b.half_size;

  // A possible early out if there are no collisions
  bool colliding = collider_check_collision(pos_a, coll_a.half_size, pos_b, coll_b.half_size);
  if(!colliding) {
    return CollisionPoint{.has_collided = false};
  }

  // All possible axises
  Vec3 axises[6] = {
    Vec3(-1.0f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f),
    Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f),
    Vec3(0.0f, 0.0f, -1.0f), Vec3(0.0f, 0.0f, 1.0f),
  };

  // Compiling the distances between the minimums and maximums of both boxes
  f32 dists[6] = {
    (max_b.x - min_a.x), // Distance between A's left and B's right
    (max_a.x - min_b.x), // Distance between A's right and  B's left
    (max_b.y - min_a.y), // Distance between A's top and B's bottom
    
    (max_a.y - min_b.y), // Distance between A's bottom and B's top
    (max_b.z - min_a.z), // Distance between A's far and B's near
    (max_a.z - min_b.z), // Distance between A's near and B's far
  };

  f32 depth   = FLOAT_MAX;
  Vec3 normal = Vec3(0.0f);

  // Trying to determine which has the least depth out of the distances.
  // The least wins. 
  for(u32 i = 0; i < 6; i++) {
    if(dists[i] < depth) {
      depth = dists[i];
      normal = axises[i];
    }
  } 

  return CollisionPoint {
    // @TODO (Physics): Get the exact collision point of the aabb collisions
    .point_a = Vec3(0.0f),
    .point_b = Vec3(0.0f),

    .normal = normal, 
    .depth  = depth, 

    .has_collided = true,
  };
}

CollisionPoint collider_check_collision(const SphereCollider& coll_a, const Vec3& pos_a, const SphereCollider& coll_b, const Vec3& pos_b) {
  f32 radii = coll_a.radius + coll_b.radius;
  f32 dist  = vec3_distance(pos_a, pos_b);

  // Not colliding!
  if(dist > radii) {
    return CollisionPoint{.has_collided = false};
  }

  Vec3 normal = vec3_normalize(pos_b - pos_a);

  return CollisionPoint {
    .point_a = normal * coll_a.radius, 
    .point_b = -normal * coll_b.radius, 

    .normal = normal,
    .depth  = radii - dist, 

    .has_collided = true,
  };
}

CollisionPoint collider_check_collision(const BoxCollider& box, const Vec3& box_pos, const SphereCollider& sphere, const Vec3& sphere_pos) {
  Vec3 diff          = box_pos - sphere_pos;
  Vec3 closest_point = vec3_clamp(diff, -box.half_size, box.half_size);
  
  Vec3 point     = diff - closest_point; 
  f32 point_dist = vec3_distance(closest_point, diff);

  // Sphere and box are not intersecting
  if(point_dist > sphere.radius) {
    return CollisionPoint{.has_collided = false};
  }

  Vec3 normal = glm::normalize(point);

  return CollisionPoint {
    // Sphere
    .point_a = -normal * sphere.radius, 
   
    // Box
    .point_b = Vec3(0.0f), 

    .normal = normal,
    .depth  = sphere.radius - point_dist, 

    .has_collided = true,
  };
}

bool collider_check_collision(const Vec3& pos_a, const Vec3& size_a, const Vec3& pos_b, const Vec3& size_b) { 
  Vec3 sum_size = size_a + size_b;
  Vec3 diff     = pos_b - pos_a; 

  // The sum of the sizes is greater than the difference between
  // the two boxes on ALL axises
  if(nikola::abs(diff.x) < sum_size.x && nikola::abs(diff.y) < sum_size.y && nikola::abs(diff.z) < sum_size.z) {
    return true;
  }

  return true;
}

/// Collider functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Physics ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
