#include "nikola/nikola_physics.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Physics ***

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void check_collisions(const Collider& coll_a, const Vec3& pos_a, const Collider& coll_b, const Vec3& pos_b, CollisionPoint* out_point) {
  Vec3 min_a = pos_a - coll_a.half_size;
  Vec3 max_a = pos_a + coll_a.half_size;
  
  Vec3 min_b = pos_b - coll_b.half_size;
  Vec3 max_b = pos_b + coll_b.half_size;
 
  bool colliding = collider_check_collision(pos_a, coll_a.half_size, pos_b, coll_b.half_size);
  if(!colliding) {
    out_point->has_collided = false;
    return;
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

  // @TODO (Physics): Get the exact collision point of the aabb collisions
  out_point->point_a = Vec3(0.0f);
  out_point->point_b = Vec3(0.0f); 

  out_point->normal = normal; 
  out_point->depth  = depth;

  out_point->has_collided = true;
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Collider functions

CollisionPoint collider_check_collision(const Collider& coll_a, const Transform& trans_a, const Collider& coll_b, const Transform& trans_b) {
  CollisionPoint point = {};
  check_collisions(coll_a, trans_a.position, coll_b, trans_b.position, &point);

  return point;
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
