#include "nikol/nikol_engine.hpp"
#include "nikol/nikol_core.hpp"

#include <glm/glm.hpp>

//////////////////////////////////////////////////////////////////////////

namespace nikol { // Start of nikol

/// ----------------------------------------------------------------------
/// *** Math common ***

/// ----------------------------------------------------------------------
/// Math common functions

const f32 clamp_float(const f32 value, const f32 min, const f32 max) {
  return glm::clamp(value, min, max);
}

const i32 clamp_int(const i32 value, const i32 min, const i32 max) {
  return glm::clamp(value, min, max);
}

const f64 cos(const f64 x) {
  return glm::cos(x);
}

const f64 sin(const f64 x) {
  return glm::sin(x);
}

const f64 tan(const f64 x) {
  return glm::tan(x);
}

const f64 atan(const f64 x) {
  return glm::atan(x);
}

const f64 atan(const f64 y, const f64 x) {
  return glm::atan(y, x);
}

const f64 floor(const f64 x) {
  return glm::floor(x);
}

const f64 sqrt(const f64 x) {
  return glm::sqrt(x);
}

const f32 min_float(const f32 x, const f32 y) {
  return glm::min(x, y);
}

const f32 max_float(const f32 x, const f32 y) {
  return glm::max(x, y);
}

const i32 min_int(const i32 x, const i32 y) {
  return glm::min(x, y);
}

const i32 max_int(const i32 x, const i32 y) {
  return glm::max(x, y);
}

const f32 lerp(const f32 start, const f32 end, const f32 amount) {
  return start + amount * (end - start);
}

const f32 remap(const f32 value, const f32 old_min, const f32 old_max, const f32 new_min, const f32 new_max) {
  return (value - old_min) / (old_max - old_min) * (new_max - new_min) + new_max;
}

/// Math common functions
/// ----------------------------------------------------------------------

/// *** Math common ***
/// ----------------------------------------------------------------------

} // End of nikol

//////////////////////////////////////////////////////////////////////////
