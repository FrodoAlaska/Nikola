#include "nikola/nikola_physics.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Physics ***

///---------------------------------------------------------------------------------------------------------------------
/// Ray functions

RayIntersection ray_check_intersection(const Ray& ray, const Vec3& position, BoxCollider& collider) {
  collider.min = position - collider.half_size; 
  collider.max = position + collider.half_size; 
  
  Vec3 tvals(-1.0f);
   
  // Checking which sides of the box to check against
  for(u32 i = 0; i < 3; i++) {
    // Get the negative sides of the cube if the direction of the ray 
    // is positive
    if(ray.direction[i] > 0) {
      tvals[i] = (collider.min[i] - ray.position[i]) / ray.direction[i]; 
    }
    // The positive sides of the cube 
    else if(ray.direction[i] < 0) {
      tvals[i] = (collider.max[i] - ray.position[i]) / ray.direction[i];
    }
  }
    
  f32 best_t = 0.0f;

  // Try to get the max value
  if(tvals.x > tvals.y && tvals.x > tvals.z) {
    best_t = tvals.x;
  } 
  else if(tvals.y > tvals.x && tvals.y > tvals.z) {
    best_t = tvals.y;
  }
  else if(tvals.z > tvals.x && tvals.z > tvals.y) {
    best_t = tvals.z;
  }
    
  // The value is behind the ray
  if(best_t < 0.0f) {
    return RayIntersection{.has_intersected = false};
  }
    
  float epsilon          = 0.0001f; // Just to get some floating point precision
  Vec3 intersection = ray.position + (ray.direction * best_t); // Getting the intersection point
  
  for(u32 i = 0; i < 3; i++) {
    // The best intersection that was found doesn't even touch the box!!!
    // It's outside of the bounds of the box
    if((intersection[i] + epsilon) < collider.min[i] || intersection[i] > collider.max[i]) {
      return RayIntersection{.has_intersected = false};
    }
  }

  // Yep, the ray has collided
  return RayIntersection{
    .point           = intersection, 
    .distance        = best_t, 
    .has_intersected = true
  };
}

/// Ray functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Physics ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
