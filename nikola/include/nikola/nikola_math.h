#pragma once

#include "nikola_base.h"
#include "nikola_containers.h"

#include "nikola_pch.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Math ***

///---------------------------------------------------------------------------------------------------------------------
/// Math consts 

/// The value of pi
const f64 PI        = 3.14159265359;

/// A multiplier to convert a value from radians to degrees
const f32 RAD2DEG   = (180.0f / PI);

/// A multiplier to convert a value from degrees to radians
const f32 DEG2RAD   = (PI / 180.0f);

/// The value of epsilon
const f64 EPSILON   = 1.192092896e-07f;

/// The minimum possible float value
const f64 FLOAT_MIN = -3.40282e+38F;

/// The maximum possible float value
const f64 FLOAT_MAX = 3.40282e+38F;

/// Math consts 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Macros

/// Convert the given `x` to a value representing a Kibibyte.
#define KiB(x)        (x * 1024)

/// Convert the given `x` to a value representing a Mebibyte.
#define MiB(x)        ((KiB(x)) * 1024)

/// Convert the given `x` to a value representing a Gibibyte.
#define GiB(x)        ((MiB(x)) * 1024)

/// Convert the given `x` to radians
#define TO_RADIANS(x) (x * nikola::DEG2RAD)

/// Convert the given `x` to degrees
#define TO_DEGREES(x) (x * nikola::RAD2DEG)

/// Macros
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// VertexComponentType 
enum VertexComponentType {
  /// A bit for the position component of a vertex.
  VERTEX_COMPONENT_POSITION       = 1 << 0,
  
  /// A bit for the normal component of a vertex.
  VERTEX_COMPONENT_NORMAL         = 1 << 1,
  
  /// A bit for the tangent component of a vertex.
  VERTEX_COMPONENT_TANGENT        = 1 << 2,
  
  /// A bit for the texture coordinate component of a vertex.
  VERTEX_COMPONENT_TEXTURE_COORDS = 1 << 3,
  
  /// A bit for the joint index component of a vertex.
  VERTEX_COMPONENT_JOINT_ID       = 1 << 4,
  
  /// A bit for the joint weight component of a vertex.
  VERTEX_COMPONENT_JOINT_WEIGHT   = 1 << 5,
};
/// VertexComponentType 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Vec2
typedef glm::vec2 Vec2;
/// Vec2
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// IVec2
typedef glm::ivec2 IVec2;
/// IVec2
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// BVec2
typedef glm::bvec2 BVec2;
/// BVec2
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Vec3
typedef glm::vec3 Vec3;
/// Vec3
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// IVec3
typedef glm::ivec3 IVec3;
/// IVec3
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// BVec3
typedef glm::bvec3 BVec3;
/// BVec3
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Vec4
typedef glm::vec4 Vec4;
/// Vec4
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// IVec4
typedef glm::ivec4 IVec4;
/// IVec4
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// BVec4
typedef glm::bvec4 BVec4;
/// BVec4
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Mat3
typedef glm::mat3 Mat3;
/// Mat3
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Mat4
typedef glm::mat4 Mat4;
/// Mat4
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Mat4x3
typedef glm::mat4x3 Mat4x3;
/// Mat4x3
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Quat
typedef glm::quat Quat;
/// Quat
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Transform
struct Transform {
  Vec3 position = Vec3(0.0f); 
  Vec3 scale    = Vec3(1.0f);
  Quat rotation = Quat(1.0f, 0.0f, 0.0f, 0.0f);
  
  Mat4 transform = Mat4(1.0f);
};
/// Transform
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Vertex3D
struct Vertex3D {
  Vec3 position; 
  Vec3 normal;
  Vec3 tangent;

  Vec4 joint_ids     = Vec4(-2.0f); 
  Vec4 joint_weights = Vec4(0.0f); 
  
  Vec2 texture_coords;
};
/// Vertex3D
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ScalarAnimSample
struct ScalarAnimSample {
  f32 value; 
  f32 time;
};
/// ScalarAnimSample
///---------------------------------------------------------------------------------------------------------------------
  
///---------------------------------------------------------------------------------------------------------------------
/// VectorAnimSample
struct VectorAnimSample {
    Vec3 value; 
    f32 time;
};
/// VectorAnimSample
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// QuatAnimSample
struct QuatAnimSample {
  Quat value; 
  f32 time;
};
/// QuatAnimSample
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Rect2D
struct Rect2D {
  Vec2 size; 
  Vec2 position;
};
/// Rect2D
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Math common functions

/// Clamp the float `value` between `min` and `max`
NIKOLA_API const f32 clamp_float(const f32 value, const f32 min, const f32 max);

/// Clamp the int `value` between `min` and `max`
NIKOLA_API const i32 clamp_int(const i32 value, const i32 min, const i32 max);

/// Returns the cosine of `x`
NIKOLA_API const f64 cos(const f64 x);

/// Returns the sine of `x`
NIKOLA_API const f64 sin(const f64 x);

/// Returns the tangent of `x`
NIKOLA_API const f64 tan(const f64 x);

/// Returns the tangent of `x`
NIKOLA_API const f64 atan(const f64 x);

/// Returns the tangent of `y` and `x`
NIKOLA_API const f64 atan(const f64 y, const f64 x);

/// Returns the floored value of `x`
NIKOLA_API const f64 floor(const f64 x);

/// Returns the square root of `x`
NIKOLA_API const f64 sqrt(const f64 x);

/// Returns the absolute value `x`
NIKOLA_API const f32 abs(const f32 x);

/// Returns `x` raised to the power of y
NIKOLA_API const f32 pow(const f32 x, const f32 y);

/// Returns `x - y * floor(x / y)`.
NIKOLA_API const f32 fmod(const f32 x, const f32 y);

/// Returns the lesser value between `x` and `y`
NIKOLA_API const f32 min_float(const f32 x, const f32 y);

/// Returns the greater value between `x` and `y`
NIKOLA_API const f32 max_float(const f32 x, const f32 y);

/// Returns the lesser value between `x` and `y`
NIKOLA_API const i32 min_int(const i32 x, const i32 y);

/// Returns the greater value between `x` and `y`
NIKOLA_API const i32 max_int(const i32 x, const i32 y);

/// Returns the linear interpolation from `start` to `end` by `amount`
NIKOLA_API const f32 lerp(const f32 start, const f32 end, const f32 amount);

/// Perform a hermite interpolation between `edge0` and `edge1` by `x`.
NIKOLA_API const f32 smoothstep(const f32 edge0, const f32 edge1, const f32 x);

/// Returns the re mapped `value` from `old_min` - `old_max` to `new_min` to `new_max`
NIKOLA_API const f32 remap(const f32 value, const f32 old_min, const f32 old_max, const f32 new_min, const f32 new_max);

/// Math common functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Math random functions

/// Returns a random 32-bit float value
NIKOLA_API const f32 random_f32();

/// Returns a random 32-bit float value between `min` and `max`
NIKOLA_API const f32 random_f32(const f32 min, const f32 max);

/// Returns a random 64-bit float value
NIKOLA_API const f64 random_f64();

/// Returns a random 64-bit float value between `min` and `max`
NIKOLA_API const f64 random_f64(const f64 min, const f64 max);

/// Returns a random 32-bit signed int value
NIKOLA_API const i32 random_i32();

/// Returns a random 32-bit signed int value between `min` and `max`
NIKOLA_API const i32 random_i32(const i32 min, const i32 max);

/// Returns a random 64-bit signed int value
NIKOLA_API const i64 random_i64();

/// Returns a random 64-bit signed int value between `min` and `max`
NIKOLA_API const i64 random_i64(const i64 min, const i64 max);

/// Returns a random 32-bit unsigned int value
NIKOLA_API const u32 random_u32();

/// Returns a random 32-bit unsigned int value between `min` and `max`
NIKOLA_API const u32 random_u32(const u32 min, const u32 max);

/// Returns a random 64-bit unsigned int value
NIKOLA_API const u64 random_u64();

/// Returns a random 64-bit unsigned int value between `min` and `max`
NIKOLA_API const u64 random_u64(const u64 min, const u64 max);

/// Math random functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Vec2 functions

/// Returns the normalized vector of `v`
NIKOLA_API const Vec2 vec2_normalize(const Vec2& v);

/// Returns the clamped `value` between `min` and `max`
NIKOLA_API const Vec2 vec2_clamp(const Vec2& value, const Vec2& min, const Vec2& max);

/// Returns the linearly interpolated vector from `start` to `end` by `amount`.
NIKOLA_API const Vec2 vec2_lerp(const Vec2& start, const Vec2& end, const f32 amount);

/// Perform a hermite interpolation between `edge0` and `edge1` by `x`.
NIKOLA_API const Vec2 vec2_smoothstep(const Vec2& edge0, const Vec2& edge1, const Vec2& x);

/// Returns the lesser vector between `v1` and `v2`
NIKOLA_API const Vec2 vec2_min(const Vec2& v1, const Vec2& v2);

/// Returns the greater vector between `v1` and `v2`
NIKOLA_API const Vec2 vec2_max(const Vec2& v1, const Vec2& v2);

/// Returns the dot product between `v1` and `v2`
NIKOLA_API const f32 vec2_dot(const Vec2& v1, const Vec2& v2);

/// Returns the distance between `v1` and `v2`
NIKOLA_API const Vec2 vec2_distance(const Vec2& vec, const Vec2& target, const f32 );

/// Returns the angle in radians between `point1` and `point2`
NIKOLA_API const f32 vec2_angle(const Vec2& point1, const Vec2& point2);

/// Returns the string representation of `vec`
NIKOLA_API const String vec2_to_string(const Vec2& vec);

/// Vec2 functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Vec3 functions

/// Returns the normalized vector of `v`
NIKOLA_API const Vec3 vec3_normalize(const Vec3& v);

/// Returns the clamped `value` between `min` and `max`
NIKOLA_API const Vec3 vec3_clamp(const Vec3& value, const Vec3& min, const Vec3& max);

/// Returns the linearly interpolated vector from `start` to `end` by `amount`.
NIKOLA_API const Vec3 vec3_lerp(const Vec3& start, const Vec3& end, const f32 amount);

/// Perform a hermite interpolation between `edge0` and `edge1` by `x`.
NIKOLA_API const Vec3 vec3_smoothstep(const Vec3& edge0, const Vec3& edge1, const Vec3& x);

/// Returns the lesser vector between `v1` and `v2`
NIKOLA_API const Vec3 vec3_min(const Vec3& v1, const Vec3& v2);

/// Returns the greater vector between `v1` and `v2`
NIKOLA_API const Vec3 vec3_max(const Vec3& v1, const Vec3& v2);

/// Returns the dot product between `v1` and `v2`
NIKOLA_API const f32 vec3_dot(const Vec3& v1, const Vec3& v2);

/// Returns the cross product between `v1` and `v2`
NIKOLA_API const Vec3 vec3_cross(const Vec3& v1, const Vec3& v2);

/// Returns the distance between `point1` and `point2`
NIKOLA_API const f32 vec3_distance(const Vec3& v1, const Vec3& v2);

/// Returns the string representation of `vec`
NIKOLA_API const String vec3_to_string(const Vec3& vec);

/// Vec3 functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Vec4 functions

/// Returns the normalized vector of `v`
NIKOLA_API const Vec4 vec4_normalize(const Vec4& v);

/// Returns the clamped `value` between `min` and `max`
NIKOLA_API const Vec4 vec4_clamp(const Vec4& value, const Vec4& min, const Vec4& max);

/// Returns the linearly interpolated vector from `start` to `end` by `amount`.
NIKOLA_API const Vec4 vec4_lerp(const Vec4& start, const Vec4& end, const f32 amount);

/// Perform a hermite interpolation between `edge0` and `edge1` by `x`.
NIKOLA_API const Vec4 vec4_smoothstep(const Vec4& edge0, const Vec4& edge1, const Vec4& x);

/// Returns the lesser vector between `v1` and `v2`
NIKOLA_API const Vec4 vec4_min(const Vec4& v1, const Vec4& v2);

/// Returns the greater vector between `v1` and `v2`
NIKOLA_API const Vec4 vec4_max(const Vec4& v1, const Vec4& v2);

/// Returns the dot product between `v1` and `v2`
NIKOLA_API const f32 vec4_dot(const Vec4& v1, const Vec4& v2);

/// Returns the distance between `point1` and `point2`
NIKOLA_API const f32 vec4_distance(const Vec4& v1, const Vec4& v2);

/// Returns the string representation of `vec`
NIKOLA_API const String vec4_to_string(const Vec4& vec);

/// Vec4 functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Mat3 functions

/// Returns the determinant value of `mat` 
NIKOLA_API const f32 mat3_det(const Mat3& mat);

/// Returns the transposed matrix of `mat` 
NIKOLA_API const Mat3 mat3_transpose(const Mat3& mat);

/// Returns the inverse matrix of `mat` 
NIKOLA_API const Mat3 mat3_inverse(const Mat3& mat);

/// Returns the raw array of floats of `mat` 
NIKOLA_API const f32* mat3_raw_data(const Mat3& mat);

/// Returns the string representation of `mat3`
NIKOLA_API const String mat3_to_string(const Mat3& mat);

/// Mat3 functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Mat4 functions

/// Creates and returns a `Mat4` from the given array 
/// of `values`. 
///
/// @NOTE: The `values` array MUST have 16 elements.
NIKOLA_API Mat4 mat4_make(const f32* values);

/// Returns the determinant value of `mat` 
NIKOLA_API const f32 mat4_det(const Mat4& mat);

/// Returns the transposed matrix of `mat` 
NIKOLA_API const Mat4 mat4_transpose(const Mat4& mat);

/// Returns the inverse matrix of `mat` 
NIKOLA_API const Mat4 mat4_inverse(const Mat4& mat);

/// Returns the translation matrix affected by `position`
NIKOLA_API const Mat4 mat4_translate(const Vec3& position);

/// Returns the roation matrix affected by `axis` and `angle`
NIKOLA_API const Mat4 mat4_rotate(const Vec3& axis, const f32 angle);

/// Returns the sclae matrix affected by `scale`
NIKOLA_API const Mat4 mat4_scale(const Vec3& scale);

/// Generates a 4x4 matrix for a symmetric perspective view frustrum, using the left hand coordinate system.
///
/// @NOTE: The fov is in radians.
NIKOLA_API const Mat4 mat4_perspective(const f32 fov, const f32 aspect_ratio, const f32 near, const f32 far);

/// Generates a 4x4 matrix for a orthographic view frustrum for a 2D space, 
/// using the left hand coordinate system
NIKOLA_API const Mat4 mat4_ortho(const f32 left, const f32 right, 
                                 const f32 bottom, const f32 top);

/// Generates a 4x4 matrix for a orthographic view frustrum for a 3D space, 
/// using the left hand coordinate system
NIKOLA_API const Mat4 mat4_ortho(const f32 left, const f32 right, 
                                 const f32 bottom, const f32 top, 
                                 const f32 near, const f32 far);

/// Generates a 4x4 look at view matrix, using ther left handed coordinate system 
NIKOLA_API const Mat4 mat4_look_at(const Vec3& eye, const Vec3& center, const Vec3& up);

/// Returns the raw array of floats of `mat` 
NIKOLA_API const f32* mat4_raw_data(const Mat4& mat);

/// Returns the string representation of `mat3`
NIKOLA_API const String mat4_to_string(const Mat4& mat);

/// Mat4 functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Math Quat functions

/// Create and return an identity quaternion.
NIKOLA_API Quat quat_identity();

/// Returns the dot product between `q1` and `q2`.
NIKOLA_API const f32 quat_dot(const Quat& q1, const Quat& q2);

/// Returns the normalized quaternion of `q`.
NIKOLA_API const Quat quat_normalize(const Quat& q);

/// Returns the linearly interpolated quaternion from `start` to `end` by `amount`.
NIKOLA_API const Quat quat_lerp(const Quat& start, const Quat& end, const f32 amount);

/// Returns the smooth linearly interpolated quaternion from `start` to `end` by `amount`.
NIKOLA_API const Quat quat_slerp(const Quat& start, const Quat& end, const f32 amount);

/// Sets and returns the rotation of a quaternion using the given 3x3 `mat`.
NIKOLA_API const Quat quat_set_mat3(const Mat3& mat);

/// Sets and returns the rotation of a quaternion using the given 4x4 `mat`.
NIKOLA_API const Quat quat_set_mat4(const Mat4& mat);

/// Rotates and returns a quaternion using the given `axis` by `angle` in radians.
NIKOLA_API const Quat quat_angle_axis(const Vec3& axis, const f32 angle);

/// Convert the given `q` to a 3x3 rotation matrix
NIKOLA_API const Mat3 quat_to_mat3(const Quat& q);

/// Convert the given `q` to a 4x4 rotation matrix
NIKOLA_API const Mat4 quat_to_mat4(const Quat& q);

/// Convert the given `q` to a `Vec3` representing an euler angle. 
/// The resulting angles will be expressed in radians.
NIKOLA_API const Vec3 quat_to_euler(const Quat& q);

/// Math Quat functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Transform functions

/// Apply the transformation given the position, rotation, and scale memebers of `trans`.
/// 
/// @NOTE: This is more performant method to edit transforms, sicne it will only update 
/// the internal matrix once instead of three times.
NIKOLA_API void transform_apply(Transform& trans);

/// Translate the given `trans` by `pos`
NIKOLA_API void transform_translate(Transform& trans, const Vec3& pos);

/// Rotate the given `trans` by `rot`
///
/// @NOTE: Internally, the given `rot` quaternion is normalized for better precision.
NIKOLA_API void transform_rotate(Transform& trans, const Quat& rot);

/// Rotate the given `trans` by `axis_angle`, using `x, y, z` as the axis and 
/// `z` as the angle in radians.
NIKOLA_API void transform_rotate(Transform& trans, const Vec4& axis_angle);

/// Rotate the given `trans` by `angle` in radians around `axis` 
NIKOLA_API void transform_rotate(Transform& trans, const Vec3& axis, const f32 angle);

/// Scale the given `trans` by `scale`
NIKOLA_API void transform_scale(Transform& trans, const Vec3& scale);

/// Lerp between the two given transforms `trans_a` and `trans_b` by the given `delta` value.
NIKOLA_API void transform_lerp(Transform& trans_a, const Transform& trans_b, const f32 delta);

/// Lerp the components of the given transform `trans` to `position`, `rotation`, 
/// and `scale` by the given `delta` value.
NIKOLA_API void transform_lerp(Transform& trans, 
                               const Vec3& position, 
                               const Quat& rotation, 
                               const Vec3& scale,
                               const f32 delta);

/// Lerp the position component of the given transform `trans` to `position` by the given `delta` value.
NIKOLA_API void transform_lerp_position(Transform& trans, const Vec3& position, const f32 delta);

/// Use the spherical linear interpolation method to interpolate the roation component 
/// of `trans` to `rotation` by `delta`.
NIKOLA_API void transform_slerp_rotation(Transform& trans, const Quat& rotation, const f32 delta);

/// Lerp the rotation component of the given transform `trans` to `axis` and `angle` by the given `delta` value.
NIKOLA_API void transform_lerp_rotation(Transform& trans, const Vec3& axis, const f32 angle, const f32 delta);

/// Lerp the scale component of the given transform `trans` to `scale` by the given `delta` value.
NIKOLA_API void transform_lerp_scale(Transform& trans, const Vec3& scale, const f32 delta);

/// Transform functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Vertex functions

/// Returns the number of components per vertex type in the given `vertex_flags`.
NIKOLA_API const sizei vertex_get_components_count(const i32 vertex_flags);

/// Returns the stride in bytes in the given `vertex_flags`.
NIKOLA_API const sizei vertex_get_stride(const i32 vertex_flags);

/// Vertex functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Point functions

/// Check if the given `point` is inside the bounds of `position` and `position + size`.
NIKOLA_API bool point_in_rect(const Vec2& point, const Vec2& position, const Vec2& size);

/// Check if the given `point` is inside the bounds `rect`.
NIKOLA_API bool point_in_rect(const Vec2& point, const Rect2D& rect);

/// Check if the given `point` is inside the circle centered at `position` with `radius` radius.
NIKOLA_API bool point_in_circle(const Vec2& point, const Vec2& position, const f32 radius);

/// Point functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Math ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
