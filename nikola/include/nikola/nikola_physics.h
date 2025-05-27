#pragma once

#include "nikola_base.h"
#include "nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Physics ***

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
/// SphereCollider
struct SphereCollider {
  f32 radius = 0.0f; 

  /// Set the given `radius` as the radius 
  /// of the sphere collider.
  SphereCollider(const f32 radius) 
    :radius(radius)
    {}
};
/// SphereCollider
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// CollisionPoint
struct CollisionPoint {
  /// The position of the first collided point.
  Vec3 point_a; 
  
  /// The position of the second collided point.
  Vec3 point_b;

  /// The normal direction of the collision.
  Vec3 normal; 
  
  /// The depth value of the collision.
  f32 depth; 

  /// A flag indicating if there was a collision or not.
  bool has_collided = false;
};
/// CollisionPoint
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RayIntersection
struct RayIntersection {
  /// The point of intersection of the ray. 
  Vec3 point;
 
  /// The distance travelled from the Ray's position till the `point`.
  f32 distance; 

  /// A flag indicating if there was an intersection or not.
  bool has_intersected;
};
/// RayIntersection
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Ray
struct Ray {
  /// The starting position of the ray.
  Vec3 position; 

  /// The direction of the ray.
  Vec3 direction;
};
/// Ray
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Collider functions

/// Check and return the collision information between box collider `coll_a` located at `pos_a` and box collider 
/// `coll_b` located at `pos_b`.
NIKOLA_API CollisionPoint collider_check_collision(const BoxCollider& coll_a, const Vec3& pos_a, const BoxCollider& coll_b, const Vec3& pos_b);

/// Check and return the collision information between sphere collider `coll_a` located at `pos_a` and sphere collider 
/// `coll_b` located at `pos_b`.
NIKOLA_API CollisionPoint collider_check_collision(const SphereCollider& coll_a, const Vec3& pos_a, const SphereCollider& coll_b, const Vec3& pos_b);

/// Check and return the collision information between a box collider `box` located at `box_pos` and a sphere collider 
/// `sphere` located at `sphere_pos`.
NIKOLA_API CollisionPoint collider_check_collision(const BoxCollider& box, const Vec3& box_pos, const SphereCollider& sphere, const Vec3& sphere_pos);

/// Return `true` if an AABB at `pos_a` with a `size_a` collided with an AABB at `pos_b` with a `size_b`.
NIKOLA_API bool collider_check_collision(const Vec3& pos_a, const Vec3& size_a, const Vec3& pos_b, const Vec3& size_b);

/// Collider functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Ray functions

/// Check and return the ray intersection information between the given `ray` with a
/// box collider `collider` located at `position`.
NIKOLA_API RayIntersection ray_check_intersection(const Ray& ray, const Vec3& position, BoxCollider& collider);

/// Check and return the ray intersection information between the given `ray` with a
/// sphere collider `collider` located at `position`.
NIKOLA_API RayIntersection ray_check_intersection(const Ray& ray, const Vec3& position, SphereCollider& collider);

/// Ray functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Physics ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
