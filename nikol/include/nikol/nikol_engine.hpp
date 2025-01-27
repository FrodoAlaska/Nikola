#pragma once

#include "nikol_core.hpp"

#include <glm/glm.hpp>
#include <glm/ext/quaternion_float.hpp>

//////////////////////////////////////////////////////////////////////////

namespace nikol { // Start of nikol

/// ----------------------------------------------------------------------
/// *** Consts ***
/// ----------------------------------------------------------------------

/// Math consts 

const f64 NIKOL_PI        = 3.14159265359;

const f32 NIKOL_RAD2DEG   = (180.0f / NIKOL_PI);

const f32 NIKOL_DEG2RAD   = (NIKOL_PI / 180.0f);

const f64 NIKOL_EPSILON   = 1.192092896e-07f;

const f64 NIKOL_FLOAT_MIN = -3.40282e+38F;

const f64 NIKOL_FLOAT_MAX = 3.40282e+38F;

/// Math consts 
/// ----------------------------------------------------------------------

/// *** Consts ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** String ***

/// ----------------------------------------------------------------------
/// String
struct String {
  sizei length   = 0; 
  sizei capacity = 5;
  char* data     = nullptr;
  
  /// ----------------------------------------------------------------------
  /// String operator overloads
  
  char& operator[](const sizei index);
  
  const char& operator[](const sizei index) const;
  
  String& operator=(const String& str);
  
  String& operator=(const char* str);
  
  /// String operator overloads
  /// ----------------------------------------------------------------------
};
/// String
/// ----------------------------------------------------------------------
  
/// ----------------------------------------------------------------------
/// String operator overloads

const bool operator==(const String& str1, const String& str2);
const bool operator!=(const String& str1, const String& str2);

void operator+(String& str, const String& other);
void operator+(String& str, const char* other);
void operator+=(String& str, const String& other);
void operator+=(String& str, const char* other);

void operator-(String& str, const String& other);
void operator-(String& str, const char* other);
void operator-=(String& str, const String& other);
void operator-=(String& str, const char* other);

/// String operator overloads
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// String functions

void string_destroy(String& str);

const bool string_is_empty(String& str);

/// String functions
/// ----------------------------------------------------------------------

/// *** String ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Math common ***

/// Math common functions

const f32 clamp_float(const f32 value, const f32 min, const f32 max);

const i32 clamp_int(const i32 value, const i32 min, const i32 max);

const f64 cos(const f64 x);

const f64 sin(const f64 x);

const f64 tan(const f64 x);

const f64 atan(const f64 x);

const f64 atan(const f64 y, const f64 x);

const f64 floor(const f64 x);

const f64 sqrt(const f64 x);

const f32 min_float(const f32 x, const f32 y);

const f32 max_float(const f32 x, const f32 y);

const i32 min_int(const i32 x, const i32 y);

const i32 max_int(const i32 x, const i32 y);

const f32 lerp(const f32 start, const f32 end, const f32 amount);

const f32 remap(const f32 value, const f32 old_min, const f32 old_max, const f32 new_min, const f32 new_max);

/// Math common functions
/// ----------------------------------------------------------------------

/// *** Math common ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Math random ***

/// ----------------------------------------------------------------------
/// Math random functions

const f32 random_f32();

const f32 random_f32(const f32 min, const f32 max);

const f64 random_f64();

const f64 random_f64(const f64 min, const f64 max);

const i32 random_i32();

const i32 random_i32(const i32 min, const i32 max);

const i64 random_i64();

const i64 random_i64(const i64 min, const i64 max);

const u32 random_u32();

const u32 random_u32(const u32 min, const u32 max);

const u64 random_u64();

const u64 random_u64(const u64 min, const u64 max);

/// Math random functions
/// ----------------------------------------------------------------------

/// *** Math random ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Math vector types ***

/// ----------------------------------------------------------------------
/// Vec2
typedef glm::vec2 Vec2;
/// Vec2
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// IVec2
typedef glm::ivec2 IVec2;
/// IVec2
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Vec3
typedef glm::vec3 Vec3;
/// Vec3
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// IVec3
typedef glm::ivec3 IVec3;
/// IVec3
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Vec4
typedef glm::vec4 Vec4;
/// Vec4
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// IVec4
typedef glm::ivec4 IVec4;
/// IVec4
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Vec2 functions

const Vec2 vec2_normalize(const Vec2& v);

const Vec2 vec2_clamp(const Vec2& value, const Vec2& min, const Vec2& max);

const Vec2 vec2_min(const Vec2& v1, const Vec2& v2);

const Vec2 vec2_max(const Vec2& v1, const Vec2& v2);

const f32 vec2_dot(const Vec2& v1, const Vec2& v2);

const f32 vec2_distance(const Vec2& v1, const Vec2& v2);

const f32 vec2_angle(const Vec2& point1, const Vec2& point2);

/// Vec2 functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Vec3 functions

const Vec3 vec3_normalize(const Vec3& v);

const Vec3 vec3_clamp(const Vec3& value, const Vec3& min, const Vec3& max);

const Vec3 vec3_min(const Vec3& v1, const Vec3& v2);

const Vec3 vec3_max(const Vec3& v1, const Vec3& v2);

const Vec3 vec3_cross(const Vec3& v1, const Vec3& v2);

const f32 vec3_dot(const Vec3& v1, const Vec3& v2);

const f32 vec3_distance(const Vec3& v1, const Vec3& v2);

/// Vec3 functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Vec4 functions

const Vec4 vec4_normalize(const Vec4& v);

const Vec4 vec4_clamp(const Vec4& value, const Vec4& min, const Vec4& max);

const Vec4 vec4_min(const Vec4& v1, const Vec4& v2);

const Vec4 vec4_max(const Vec4& v1, const Vec4& v2);

const f32 vec4_dot(const Vec4& v1, const Vec4& v2);

const f32 vec4_distance(const Vec4& v1, const Vec4& v2);

/// Vec4 functions
/// ----------------------------------------------------------------------

/// *** Math vector types ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Math matrix types ***

/// ----------------------------------------------------------------------
/// Mat3
typedef glm::mat3 Mat3;
/// Mat3
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Mat4
typedef glm::mat4 Mat4;
/// Mat4
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Mat4x3
typedef glm::mat4x3 Mat4x3;
/// Mat4x3
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Mat3 functions

const f32 mat3_det(const Mat3& mat);

const Mat3 mat3_transpose(const Mat3& mat);

const Mat3 mat3_inverse(const Mat3& mat);

const f32* mat3_raw_data(const Mat3& mat);

/// Mat3 functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Mat4 functions

const f32 mat4_det(const Mat4& mat);

const Mat4 mat4_transpose(const Mat4& mat);

const Mat4 mat4_inverse(const Mat4& mat);

const Mat4 mat4_translate(const Vec3& position);

const Mat4 mat4_rotate(const Vec3& axis, const f32 angle);

const Mat4 mat4_scale(const Vec3& scale);

const Mat4 mat4_perspective(const f32 fov, const f32 aspect_ratio, const f32 near, const f32 far);

const Mat4 mat4_ortho(const f32 left, const f32 right, const f32 bottom, const f32 top);

const Mat4 mat4_look_at(const Vec3& eye, const Vec3& center, const Vec3& up);

const f32* mat4_raw_data(const Mat4& mat);

/// Mat4 functions
/// ----------------------------------------------------------------------

/// *** Math matrix types ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Math quaternion ***

/// ----------------------------------------------------------------------
/// Quat
typedef glm::quat Quat;
/// Quat
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Math Quat functions

const f32 quat_dot(const Quat& q1, const Quat& q2);

const Quat quat_normalize(const Quat& q);

const Quat quat_lerp(const Quat& start, const Quat& end, const f32 amount);

const Quat quat_set_mat3(const Mat3& mat);

const Quat quat_set_mat4(const Mat4& mat);

const Quat quat_angle_axis(const Vec3& axis, const f32 angle);

const Mat3 quat_to_mat3(const Quat& q);

const Mat4 quat_to_mat4(const Quat& q);

/// Math Quat functions
/// ----------------------------------------------------------------------

/// *** Math quaternion ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Math transform ***

/// ----------------------------------------------------------------------
/// Transform
struct Transform {
  Vec3 position  = Vec3(0.0f); 
  Vec3 scale     = Vec3(1.0f);
  Quat rotation  = Quat(0.0f, 0.0f, 0.0f, 0.0f);
  Mat4 transform = Mat4(1.0f);
};
/// Transform
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Transform functions

void transform_translate(Transform& trans, const Vec3& pos);

void transform_rotate(Transform& trans, const Quat& rot);

void transform_rotate(Transform& trans, const Vec4& axis_angle);

void transform_rotate(Transform& trans, const Vec3& axis, const f32 angle);

void transform_scale(Transform& trans, const Vec3& scale);

/// Transform functions
/// ----------------------------------------------------------------------

/// *** Math transform ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Math vertex types ***

/// ----------------------------------------------------------------------
/// Vertex3D_PNUV (Position, Normal, U/V texture coords)
struct Vertex3D_PNUV {
  Vec3 position;
  Vec3 normal;
  Vec2 texture_coords;
};
/// Vertex3D_PNUV (Position, Normal, U/V texture coords)
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Vertex3D_PNUV (Position, U/V texture coords, Color)
struct Vertex3D_PUVC {
  Vec3 position;
  Vec2 texture_coords;
  Vec4 color;
};
/// Vertex3D_PNUV (Position, U/V texture coords, Color)
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Vertex3D_PNUVC (Position, Normal, U/V texture coords, Color (r, g, b, a))
struct Vertex3D_PNUVC {
  Vec3 position;
  Vec3 normal;
  Vec2 texture_coords;
  Vec4 color;
};
/// Vertex3D_PNUVC (Position, Normal, U/V texture coords, Color (r, g, b, a))
/// ----------------------------------------------------------------------

/// *** Math vertex types ***
/// ----------------------------------------------------------------------

/// *** Math ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Resources ***

/// ----------------------------------------------------------------------
/// Mesh 
struct Mesh {
  nikol::GfxBuffer* vertex_buffer = nullptr; 
  nikol::GfxBuffer* index_buffer  = nullptr;

  nikol::GfxPipeline* pipe         = nullptr;
  nikol::GfxPipelineDesc pipe_desc = {};
};
/// Mesh 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Material 
struct Material {
};
/// Material 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Model 
struct Model {

};
/// Model 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Font 
struct Font {

};
/// Font 
/// ----------------------------------------------------------------------

/// *** Resources ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Loaders ***

/// ----------------------------------------------------------------------
/// Texture loader functions
/// Texture loader functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Cubemap loader functions
/// Cubemap loader functions
/// ----------------------------------------------------------------------

/// *** Loaders ***
/// ----------------------------------------------------------------------

} // End of nikol

//////////////////////////////////////////////////////////////////////////
