#include "nikola/nikola_base.h"
#include "nikola/nikola_math.h"

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

const String mat3_to_string(const Mat3& mat) {
  return 
    "{" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[0][1]) + ", " + std::to_string(mat[0][1]) + "}\n"
    "{" + std::to_string(mat[1][0]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[1][1]) + "}\n"
    "{" + std::to_string(mat[2][0]) + ", " + std::to_string(mat[2][1]) + ", " + std::to_string(mat[2][1]) + "}";
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

const String mat4_to_string(const Mat4& mat) {
  return 
    "{" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[0][1]) + ", " + std::to_string(mat[0][1]) + ", " + std::to_string(mat[0][2]) + "}\n"
    "{" + std::to_string(mat[1][0]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[1][2]) + "}\n"
    "{" + std::to_string(mat[2][0]) + ", " + std::to_string(mat[2][1]) + ", " + std::to_string(mat[2][1]) + ", " + std::to_string(mat[2][2]) + "}\n"
    "{" + std::to_string(mat[3][0]) + ", " + std::to_string(mat[3][1]) + ", " + std::to_string(mat[3][1]) + ", " + std::to_string(mat[3][2]) + "}";
}

/// Mat4 functions
/// ----------------------------------------------------------------------

/// *** Math matrix types ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
