#include "nikola/nikola_base.h"
#include "nikola/nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Math ***

///---------------------------------------------------------------------------------------------------------------------
/// Point functions

bool point_in_rect(const Vec2& point, const Vec2& position, const Vec2& size) {
  Vec2 min = position; 
  Vec2 max = position + size; 

  return (point.x > min.x && point.x < max.x) && 
         (point.y > min.y && point.y < max.y);
}

bool point_in_rect(const Vec2& point, const Rect2D& rect) {
  return point_in_rect(point, rect.position, rect.size);
}

bool point_in_circle(const Vec2& point, const Vec2& position, const f32 radius) {
  Vec2 diff = position - point;
  f32 len   = glm::length(diff); 

  return len < radius;
}

/// Point functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Math ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
