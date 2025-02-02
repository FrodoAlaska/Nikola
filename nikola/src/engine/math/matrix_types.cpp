#include "nikola/nikola_engine.hpp"
#include "nikola/nikola_core.hpp"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Math matrix types ***

/// ----------------------------------------------------------------------
/// Mat3 functions

const f32 mat3_det(const Mat3& mat) {
  return glm::determinant(mat);
}

const Mat3 mat3_transpose(const Mat3& mat) {
  return glm::transpose(mat);
}

const Mat3 mat3_inverse(const Mat3& mat) {
  return glm::inverse(mat);
}

const f32* mat3_raw_data(const Mat3& mat) {
  return glm::value_ptr(mat);
}

/// Mat3 functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Mat4 functions

const f32 mat4_det(const Mat4& mat) {
  return glm::determinant(mat);
}

const Mat4 mat4_transpose(const Mat4& mat) {
  return glm::transpose(mat);
}

const Mat4 mat4_inverse(const Mat4& mat) {
  return glm::inverse(mat);
}

const Mat4 mat4_translate(const Vec3& position) {
  return glm::translate(Mat4(1.0f), position);
}

const Mat4 mat4_rotate(const Vec3& axis, const f32 angle) {
  return glm::rotate(Mat4(1.0f), angle, axis);
}

const Mat4 mat4_scale(const Vec3& scale) {
  return glm::scale(Mat4(1.0f), scale);
}

const Mat4 mat4_perspective(const f32 fov, const f32 aspect_ratio, const f32 near, const f32 far) {
  return glm::perspective(fov, aspect_ratio, near, far);
}

const Mat4 mat4_ortho(const f32 left, const f32 right, const f32 bottom, const f32 top) {
  return glm::ortho(left, right, bottom, left);
}

const Mat4 mat4_look_at(const Vec3& eye, const Vec3& center, const Vec3& up) {
  return glm::lookAt(eye, center, up);
}

const f32* mat4_raw_data(const Mat4& mat) {
  return glm::value_ptr(mat);
}

/// Mat4 functions
/// ----------------------------------------------------------------------

/// *** Math matrix types ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
