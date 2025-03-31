#include "nikola/nikola_base.h"
#include "nikola/nikola_math.h"

#include <glm/glm.hpp>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Math vector types ***

/// ----------------------------------------------------------------------
/// Vec2 functions

const Vec2 vec2_normalize(const Vec2& v) {
  return glm::normalize(v);
}

const Vec2 vec2_clamp(const Vec2& value, const Vec2& min, const Vec2& max) {
  return glm::clamp(value, min, max);
}

const Vec2 vec2_min(const Vec2& v1, const Vec2& v2) {
  return glm::min(v1, v2);
}

const Vec2 vec2_max(const Vec2& v1, const Vec2& v2) {
  return glm::max(v1, v2);
}

const f32 vec2_dot(const Vec2& v1, const Vec2& v2) {
  return glm::dot(v1, v2);
}

const f32 vec2_distance(const Vec2& v1, const Vec2& v2) {
  return (v2 - v1).length();
}

const f32 vec2_angle(const Vec2& point1, const Vec2& point2) {
  Vec2 diff = point2 - point1;
  return nikola::atan(diff.y, diff.x);
}

const String vec2_to_string(const Vec2& vec) {
  return "X = " + std::to_string(vec.x) + ", Y = " + std::to_string(vec.y);
}

/// Vec2 functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Vec3 functions

const Vec3 vec3_normalize(const Vec3& v) {
  return glm::normalize(v);
}

const Vec3 vec3_clamp(const Vec3& value, const Vec3& min, const Vec3& max) {
  return glm::clamp(value, min, max);
}

const Vec3 vec3_min(const Vec3& v1, const Vec3& v2) {
  return glm::min(v1, v2);
}

const Vec3 vec3_max(const Vec3& v1, const Vec3& v2) {
  return glm::max(v1, v2);
}

const Vec3 vec3_cross(const Vec3& v1, const Vec3& v2) {
  return glm::cross(v1, v2);
}

const f32 vec3_dot(const Vec3& v1, const Vec3& v2) {
  return glm::dot(v1, v2);
}

const f32 vec3_distance(const Vec3& v1, const Vec3& v2) {
  return (v2 - v1).length();
}

const String vec3_to_string(const Vec3& vec) {
  return "X = " + std::to_string(vec.x) + ", Y = " + std::to_string(vec.y) + ", Z = " + std::to_string(vec.z);
}

/// Vec3 functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Vec4 functions

const Vec4 vec4_normalize(const Vec4& v) {
  return glm::normalize(v);
}

const Vec4 vec4_clamp(const Vec4& value, const Vec4& min, const Vec4& max) {
  return glm::clamp(value, min, max);
}

const Vec4 vec4_min(const Vec4& v1, const Vec4& v2) {
  return glm::min(v1, v2);
}

const Vec4 vec4_max(const Vec4& v1, const Vec4& v2) {
  return glm::max(v1, v2);
}

const f32 vec4_dot(const Vec4& v1, const Vec4& v2) {
  return glm::dot(v1, v2);
}

const f32 vec4_distance(const Vec4& v1, const Vec4& v2) {
  return (v2 - v1).length();
}

const String vec4_to_string(const Vec4& vec) {
  return "X = " + std::to_string(vec.x)   + 
         ", Y = " + std::to_string(vec.y) + 
         ", Z = " + std::to_string(vec.z) + 
         ", W = " + std::to_string(vec.w);
}

/// Vec4 functions
/// ----------------------------------------------------------------------

/// *** Math vector types ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
