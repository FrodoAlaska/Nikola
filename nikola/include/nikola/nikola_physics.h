#pragma once

#include "nikola_base.h"
#include "nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Physics ***

// A forward declaration needed here
struct PhysicsBody;

///---------------------------------------------------------------------------------------------------------------------
/// BoxCollider
struct BoxCollider {
  Vec3 half_size;
  Vec3 min, max; 

  /// Given the `size`, the collider will 
  /// half it for future calculations.
  BoxCollider(const Vec3& size) {
    half_size = size / 2.0f; 

    min = Vec3(0.0f);
    max = Vec3(0.0f);
  }
};
/// BoxCollider
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// CollisionPoint
struct CollisionPoint {
  Vec3 point_a; 
  Vec3 point_b;

  Vec3 normal; 
  f32 depth; 

  bool has_collided = false;
};
/// CollisionPoint
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RayIntersection
struct RayIntersection {
  Vec3 point;
  
  f32 distance; 
  bool has_intersected;
};
/// RayIntersection
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Ray
struct Ray {
  Vec3 position; 
  Vec3 direction;
};
/// Ray
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Collider functions

NIKOLA_API CollisionPoint collider_check_collision(const BoxCollider& coll_a, const Vec3& pos_a, const BoxCollider& coll_b, const Vec3& pos_b);

NIKOLA_API bool collider_check_collision(const Vec3& pos_a, const Vec3& size_a, const Vec3& pos_b, const Vec3& size_b);

/// Collider functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Ray functions

NIKOLA_API RayIntersection ray_check_intersection(const Ray& ray, const Vec3& position, BoxCollider& collider);

/// Ray functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Physics ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
