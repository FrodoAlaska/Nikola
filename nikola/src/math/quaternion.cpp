#include "nikola/nikola_base.h"
#include "nikola/nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Math quaternion ***

/// ----------------------------------------------------------------------
/// Quat functions

const f32 quat_dot(const Quat& q1, const Quat& q2) {
  return glm::dot(q1, q2);
}

const Quat quat_normalize(const Quat& q) {
  return glm::normalize(q);
}

const Quat quat_lerp(const Quat& start, const Quat& end, const f32 amount) {
  return glm::lerp(start, end, amount);
}

const Quat quat_set_mat3(const Mat3& mat) {
  return glm::quat_cast(mat);
}

const Quat quat_set_mat4(const Mat4& mat) {
  return glm::quat_cast(mat);
}

const Quat quat_angle_axis(const Vec3& axis, const f32 angle) {
  return glm::angleAxis(angle, axis);
}

const Mat3 quat_to_mat3(const Quat& q) {
  return glm::mat3_cast(q);
}

const Mat4 quat_to_mat4(const Quat& q) {
  return glm::mat4_cast(q);
}

const Vec3 quat_to_euler(const Quat& q) {
  return glm::eulerAngles(q);
}

/// Quat functions
/// ----------------------------------------------------------------------

/// *** Math quaternion ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
