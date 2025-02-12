#pragma once

#include "nikola_core.hpp"

#include <glm/glm.hpp>
#include <glm/ext/quaternion_float.hpp>

#include <string>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <fstream>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Macros 

#if NIKOLA_PLATFORM_WINDOWS == 1
  
  // Since Windows likes to bloat the application 
  // with unnecessary crap, this define should disable 
  // that completely.
  #define WIN32_LEAN_AND_MEAN
 
  /// The entry point to a C++ application
  #define NIKOLA_MAIN(engine_main)                                                        \
  int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev_inst, LPSTR cmd_line, int cmd_show) { \
    return engine_main(0, &cmd_line);                                                     \
  }

#elif NIKOLA_PLATFORM_LINUX == 1
  
  /// The entry point to a C++ application
  #define NIKOLA_MAIN(engine_main)  \
  int main(int argc, char** argv) { \
    return engine_main(argc, argv); \
  }                                 \

#endif

/// Macros 
///---------------------------------------------------------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Typedefs ***

/// An ASCII string
using String       = std::string;

/// A dynamically-sized array
template<typename T>
using DynamicArray = std::vector<T>;

/// A key-value pair hash map
template<typename K, typename V> 
using HashMap      = std::unordered_map<K, V>;

/// *** Typedefs ***
/// ----------------------------------------------------------------------

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
/// VertexType 
enum VertexType {
  /// A vertex with a position, a normal, and a U/V coordinate.
  VERTEX_TYPE_PNUV = 13 << 0, 
  
  /// A vertex with a position, a color, and a U/V coordinate.
  VERTEX_TYPE_PCUV = 13 << 1, 
  
  /// A vertex with a position, a normal, a color, and a U/V coordinate.
  VERTEX_TYPE_PNCUV = 13 << 2, 
};
/// VertexType 
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
  Vec3 position  = Vec3(0.0f); 
  Vec3 scale     = Vec3(1.0f);
  Quat rotation  = Quat(0.0f, 0.0f, 0.0f, 0.0f);
  Mat4 transform = Mat4(1.0f);
};
/// Transform
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Vertex3D_PNUV (Position, Normal, U/V texture coords)
struct Vertex3D_PNUV {
  Vec3 position;
  Vec3 normal;
  Vec2 texture_coords;
};
/// Vertex3D_PNUV (Position, Normal, U/V texture coords)
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Vertex3D_PCUV (Position, Color, U/V texture coords)
struct Vertex3D_PCUV {
  Vec3 position;
  Vec4 color;
  Vec2 texture_coords;
};
/// Vertex3D_PCUV (Position, Color, U/V texture coords)
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Vertex3D_PNCUV (Position, Normal, Color (r, g, b, a), U/V texture coords)
struct Vertex3D_PNCUV {
  Vec3 position;
  Vec3 normal;
  Vec4 color;
  Vec2 texture_coords;
};
/// Vertex3D_PNCUV (Position, Normal, Color (r, g, b, a), U/V texture coords)
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

/// Returns the lesser vector between `v1` and `v2`
NIKOLA_API const Vec2 vec2_min(const Vec2& v1, const Vec2& v2);

/// Returns the greater vector between `v1` and `v2`
NIKOLA_API const Vec2 vec2_max(const Vec2& v1, const Vec2& v2);

/// Returns the dot product between `v1` and `v2`
NIKOLA_API const f32 vec2_dot(const Vec2& v1, const Vec2& v2);

/// Returns the distance between `v1` and `v2`
NIKOLA_API const f32 vec2_distance(const Vec2& v1, const Vec2& v2);

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

/// Generates a 4x4 matrix for a orthographic view frustrum, using the left hand coordinate system
NIKOLA_API const Mat4 mat4_ortho(const f32 left, const f32 right, const f32 bottom, const f32 top);

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

/// Returns the dot product between `q1` and `q2`
NIKOLA_API const f32 quat_dot(const Quat& q1, const Quat& q2);

/// Returns the normalized quaternion of `q`
NIKOLA_API const Quat quat_normalize(const Quat& q);

/// Returns the linearly interpolated quaternion from `start` to `end` by `amount`
NIKOLA_API const Quat quat_lerp(const Quat& start, const Quat& end, const f32 amount);

/// Sets and returns the rotation of a quaternion using the given 3x3 `mat`
NIKOLA_API const Quat quat_set_mat3(const Mat3& mat);

/// Sets and returns the rotation of a quaternion using the given 4x4 `mat`
NIKOLA_API const Quat quat_set_mat4(const Mat4& mat);

/// Rotates and returns a quaternion using the given `axis` by `angle` in radians
NIKOLA_API const Quat quat_angle_axis(const Vec3& axis, const f32 angle);

/// Convert the given `q` to a 3x3 rotation matrix
NIKOLA_API const Mat3 quat_to_mat3(const Quat& q);

/// Convert the given `q` to a 4x4 rotation matrix
NIKOLA_API const Mat4 quat_to_mat4(const Quat& q);

/// Math Quat functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Transform functions

/// Translate the given `trans` by `pos`
NIKOLA_API void transform_translate(Transform& trans, const Vec3& pos);

/// Rotate the given `trans` by `rot`
NIKOLA_API void transform_rotate(Transform& trans, const Quat& rot);

/// Rotate the given `trans` by `axis_angle`, using `x, y, z` as the axis and 
/// `z` as the angle in radians.
NIKOLA_API void transform_rotate(Transform& trans, const Vec4& axis_angle);

/// Rotate the given `trans` by `angle` in radians around `axis` 
NIKOLA_API void transform_rotate(Transform& trans, const Vec3& axis, const f32 angle);

/// Scale the given `trans` by `scale`
NIKOLA_API void transform_scale(Transform& trans, const Vec3& scale);

/// Transform functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Math ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** File system ***

///---------------------------------------------------------------------------------------------------------------------
/// FilePath
using FilePath = std::filesystem::path;
/// FilePath
///---------------------------------------------------------------------------------------------------------------------


///---------------------------------------------------------------------------------------------------------------------
/// File
using File = std::fstream;
/// File
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FileOpenMode
enum FileOpenMode {
  FILE_OPEN_READ       = 14 << 0,
  
  FILE_OPEN_WRITE      = 14 << 1,
  
  FILE_OPEN_BINARY     = 14 << 2,
  
  FILE_OPEN_APPEND     = 14 << 3,
  
  FILE_OPEN_TRUNCATE   = 14 << 4,
  
  FILE_OPEN_AT_END     = 14 << 5,

  FILE_OPEN_READ_WRITE = 14 << 6
};
/// FileOpenMode
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// File functions

NIKOLA_API bool file_open(File* file, const char* path, const u32 mode);

NIKOLA_API bool file_open(File* file, const FilePath& path, const u32 mode);

NIKOLA_API void file_close(File& file);

NIKOLA_API void file_seek_write(File& file, const sizei pos);

NIKOLA_API void file_seek_read(File& file, const sizei pos);

NIKOLA_API const sizei file_tell_write(File& file);

NIKOLA_API const sizei file_tell_read(File& file);

NIKOLA_API sizei file_get_size(File& file);

NIKOLA_API bool file_is_empty(File& file);

NIKOLA_API void file_write_bytes(File& file, const void* buff, const sizei buff_size, const sizei offset = 0);

NIKOLA_API void file_read_bytes(File& file, void* out_buff, const sizei size, const sizei offset = 0);

NIKOLA_API void file_write_string(File& file, const String& string, const sizei offset = 0);

NIKOLA_API String file_read_string(File& file, const sizei offset = 0);

/// File functions
///---------------------------------------------------------------------------------------------------------------------

/// *** File system ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Resources ***

///---------------------------------------------------------------------------------------------------------------------
/// Resources consts

/// A value to indicate an unwanted or invalid resource
const i32 INVALID_RESOURCE                 = -1;

/// The maximum amount of declared uniform buffers in all materials
const sizei MATERIAL_UNIFORM_BUFFERS_MAX   = 2;

/// The index of the matrices uniform buffer within all materials
const sizei MATERIAL_MATRICES_BUFFER_INDEX = 0;

/// The index of the lighting uniform buffer within all materials
const sizei MATERIAL_LIGHTING_BUFFER_INDEX = 1;

/// Resources consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ResourceType
enum ResourceType {
  /// A flag to denote a `GfxBuffer` resource
  RESOURCE_TYPE_BUFFER   = 15 << 0, 

  /// A flag to denote a `GfxTexture` resource
  RESOURCE_TYPE_TEXTURE  = 15 << 1, 
  
  /// A flag to denote a `GfxCubemap` resource
  RESOURCE_TYPE_CUBEMAP  = 15 << 2,
  
  /// A flag to denote a `GfxShader` resource
  RESOURCE_TYPE_SHADER   = 15 << 4,
  
  /// A flag to denote a `Mesh` resource
  RESOURCE_TYPE_MESH     = 15 << 5,
  
  /// A flag to denote a `Material` resource
  RESOURCE_TYPE_MATERIAL = 15 << 6,
  
  /// A flag to denote a `Skybox` resource
  RESOURCE_TYPE_SKYBOX   = 15 << 7,
  
  /// A flag to denote a `Model` resource
  RESOURCE_TYPE_MODEL    = 15 << 8,
  
  /// A flag to denote a `Font` resource
  RESOURCE_TYPE_FONT     = 15 << 9,
};
/// ResourceType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// MeshType
enum MeshType {
  /// A predefined cube mesh
  MESH_TYPE_CUBE     = 16 << 0, 
  
  /// A predefined circle mesh
  MESH_TYPE_CIRCLE   = 16 << 1, 
  
  /// A predefined cylinder mesh
  MESH_TYPE_CYLINDER = 16 << 2, 
};
/// MeshType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderableType 
enum RenderableType {
  /// Will commence a mesh rendering operation
  RENDERABLE_TYPE_MESH   = 17 << 0,
  
  /// Will commence a model rendering operation
  RENDERABLE_TYPE_MODEL  = 17 << 1,
  
  /// Will commence a skybox rendering operation
  RENDERABLE_TYPE_SKYBOX = 17 << 2,
};
/// RenderableType 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ResourceID
typedef u64 ResourceID;
/// ResourceID
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ResourceStorage
struct ResourceStorage;
/// ResourceStorage
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Mesh 
struct Mesh {
  GfxBuffer* vertex_buffer = nullptr; 
  GfxBuffer* index_buffer  = nullptr;

  GfxPipeline* pipe         = nullptr;
  GfxPipelineDesc pipe_desc = {};

  ResourceStorage* storage_ref;
};
/// Mesh 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Material 
struct Material {
  GfxTexture* diffuse_map  = nullptr;
  GfxTexture* specular_map = nullptr;
  GfxShader* shader        = nullptr; 
  GfxBuffer* uniform_buffers[MATERIAL_UNIFORM_BUFFERS_MAX];
  
  Vec4 color; 
  Mat4 model_matrix;

  ResourceStorage* storage_ref;
};
/// Material 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Skybox
struct Skybox {
  GfxBuffer* vertex_buffer = nullptr;
  GfxCubemap* cubemap      = nullptr;
  
  GfxPipeline* pipe         = nullptr;
  GfxPipelineDesc pipe_desc = {};

  ResourceStorage* storage_ref;
};
/// Skybox
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Model 
struct Model {
  DynamicArray<Mesh*> meshes;
  DynamicArray<Material*> materials;

  ResourceStorage* storage_ref;
};
/// Model 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Font 
struct Font {
  struct Glyph {
    i8 unicode; 
    GfxTexture* texture = nullptr;

    u32 width, height;
    u32 left, right, top, bottom;

    i32 offset_x, offset_y;
    i32 advance_x, kern, left_bearing;
  };

  f32 base_size;
  f32 ascent, descent, line_gap;
  f32 glyph_padding;

  DynamicArray<Glyph> glyphs;

  ResourceStorage* storage_ref;
};
/// Font 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// MeshLoader
struct MeshLoader {
  ResourceID vertex_buffer = INVALID_RESOURCE; 
  ResourceID index_buffer  = INVALID_RESOURCE; 
  ResourceID uniform_buffers[MATERIAL_UNIFORM_BUFFERS_MAX];

  GfxPipelineDesc pipe_desc;
};
/// MeshLoader
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// MaterialLoader 
struct MaterialLoader {
  ResourceID diffuse_map  = INVALID_RESOURCE;
  ResourceID specular_map = INVALID_RESOURCE;
  ResourceID shader       = INVALID_RESOURCE;
  ResourceID uniform_buffers[MATERIAL_UNIFORM_BUFFERS_MAX];
};
/// MaterialLoader 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// SkyboxLoader 
struct SkyboxLoader {
  ResourceID vertex_buffer = INVALID_RESOURCE; 
  ResourceID cubemap       = INVALID_RESOURCE; 

  GfxPipelineDesc pipe_desc;
};
/// SkyboxLoader 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Material functions

/// Set the color value of the associated shader in `mat` to `color`
NIKOLA_API void material_set_color(Material* mat, const Vec4& color);

/// Set the transform value of the associated shader in `mat` to `transform`
NIKOLA_API void material_set_transform(Material* mat, const Transform& transform);

/// Set the matrices uniform buffer of the associated shader in `mat` to `view_projection`
NIKOLA_API void material_set_matrices_buffer(Material* mat, const Mat4& view_projection);

/// Material functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Texture loader functions

/// Fill the information of `desc` by the texture loaded from `path` as well as 
/// `format`, `filter`, and `wrap`. 
///
/// Default values: 
///   - `format` = `GFX_TEXTURE_FORMAT_RGBA8`.
///   - `filter` = `GFX_TEXTURE_FILTER_MIN_MAG_NEAREST`.
///   - `wrap`   = `GFX_TEXTURE_WRAP_CLAMP`.
NIKOLA_API void texture_loader_load(GfxTextureDesc* desc, 
                                    const FilePath& path, 
                                    const GfxTextureFormat format = GFX_TEXTURE_FORMAT_RGBA8, 
                                    const GfxTextureFilter filter = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
                                    const GfxTextureWrap wrap     = GFX_TEXTURE_WRAP_CLAMP);

/// Free the previously loaded pixels by `desc`
NIKOLA_API void texture_loader_unload(GfxTextureDesc& desc);

/// Texture loader functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Cubemap loader functions

/// Fill the information of `desc` by the `faces_count` amount of cube faces loaded from 
/// all the given `path`s as well as `format`, `filter`, and `wrap`. 
///
/// Default values: 
///   - `format` = `GFX_TEXTURE_FORMAT_RGBA8`.
///   - `filter` = `GFX_TEXTURE_FILTER_MIN_MAG_NEAREST`.
///   - `wrap`   = `GFX_TEXTURE_WRAP_CLAMP`.
NIKOLA_API void cubemap_loader_load(GfxCubemapDesc* desc, 
                                    const FilePath path[CUBEMAP_FACES_MAX], 
                                    const sizei faces_count,
                                    const GfxTextureFormat format = GFX_TEXTURE_FORMAT_RGBA8, 
                                    const GfxTextureFilter filter = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
                                    const GfxTextureWrap wrap     = GFX_TEXTURE_WRAP_CLAMP);

/// Recursively go through `directory` to attain all of the `faces_count` amount of 
/// cube faces to fill `desc`, while providing values for `format`, `filter`, and `wrap`.
///
/// Default values: 
///   - `format` = `GFX_TEXTURE_FORMAT_RGBA8`.
///   - `filter` = `GFX_TEXTURE_FILTER_MIN_MAG_NEAREST`.
///   - `wrap`   = `GFX_TEXTURE_WRAP_CLAMP`.
NIKOLA_API void cubemap_loader_load(GfxCubemapDesc* desc, 
                                    const FilePath& directory,
                                    const sizei faces_count,
                                    const GfxTextureFormat format = GFX_TEXTURE_FORMAT_RGBA8, 
                                    const GfxTextureFilter filter = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
                                    const GfxTextureWrap wrap     = GFX_TEXTURE_WRAP_CLAMP);

/// Free the previously loaded pixels by `desc`
NIKOLA_API void cubemap_loader_unload(GfxCubemapDesc& desc);

/// Cubemap loader functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Mesh loader functions

/// Use the given `storage` to retrieve the buffers `vertex_buffer_id` and `index_buffer_id`
/// in order to fill the information in `loader`. A `vertex_type` must be provided to 
/// calculate the stride, while `indices_count` will be used in case `index_buffer_id` 
/// is not set to `INVALID_RESOURCE`.
///
/// @NOTE: The value of `index_buffer_id` can be set to `INVALID_RESOURCE` to be ignored.
/// The same cannot be said for `vertex_buffer_id`.
NIKOLA_API void mesh_loader_load(ResourceStorage* storage, 
                                 MeshLoader* loader, 
                                 const ResourceID& vertex_buffer_id, 
                                 const VertexType vertex_type, 
                                 const ResourceID& index_buffer_id, 
                                 const sizei indices_count);

/// Use the given `storage` to fill the information in `loader` by a predefined mesh of `type`.
NIKOLA_API void mesh_loader_load(ResourceStorage* storage, MeshLoader* loader, const MeshType type);

/// Mesh loader functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Material loader functions

/// Use the given `storage` to check for the availability and retrieve `diffuse_id`, 
/// `specular_id`, and `shader_id` in order to fill the information in `loader`.
///
/// @NOTE: The resource `specular_id` is allowed to be set to `INVALID_RESOURCE` 
/// but not the others.
NIKOLA_API void material_loader_load(ResourceStorage* storage, 
                                     MaterialLoader* loader, 
                                     const ResourceID& diffuse_id, 
                                     const ResourceID& specular_id, 
                                     const ResourceID& shader_id);

/// Use the given `storage` to check for the availability and retrieve uniform `buffer_id` in order 
/// to attach it at `index` in the associated shader in `loader`.
NIKOLA_API void material_loader_attach_uniform(ResourceStorage* storage, MaterialLoader& loader, const sizei index, const ResourceID& buffer_id);

/// Material loader functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Skybox loader functions

/// Use the given `storage` to check for the availability and retrieve `cubemap_id` in 
/// order to fill the information in `loader`.
NIKOLA_API void skybox_loader_load(ResourceStorage* storage, SkyboxLoader* loader, const ResourceID& cubemap_id);

/// Skybox loader functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Resource manager functions

/// Initialize the global resource manager as well as the global cache.
NIKOLA_API void resource_manager_init();

/// Free/reclaim any memory consumed by the global resource manager.
NIKOLA_API void resource_manager_shutdown();

/// Retrieve the internal global cache of the resource manager.
NIKOLA_API const ResourceStorage* resource_manager_cache();

/// Resource manager functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Resource storage functions

/// Allocate and return a `ResourceStorage` with `name` and `parent_dir`.
NIKOLA_API ResourceStorage* resource_storage_create(const String& name, const FilePath& parent_dir);

/// Clear all of resources in `storage`.
NIKOLA_API void resource_storage_clear(ResourceStorage* storage);

/// Clear and destroy all of resources in `storage`.
NIKOLA_API void resource_storage_destroy(ResourceStorage* storage);

/// Allocate a new `GfxBuffer` using `buff_desc`, store it in `storage`, and return a `ResourceID` 
/// to identify it.
NIKOLA_API ResourceID resource_storage_push(ResourceStorage* storage, const GfxBufferDesc& buff_desc);

/// Allocate a new `GfxTexture` using `tex_desc`, store it in `storage`, and return a `ResourceID` 
/// to identify it.
NIKOLA_API ResourceID resource_storage_push(ResourceStorage* storage, const GfxTextureDesc& tex_desc);

/// Allocate a new `GfxCubemap` using `cubemap_desc`, store it in `storage`, and return a `ResourceID` 
/// to identify it.
NIKOLA_API ResourceID resource_storage_push(ResourceStorage* storage, const GfxCubemapDesc& cubemap_desc);

/// Allocate a new `GfxShader` using `shader_src`, store it in `storage`, and return a `ResourceID` 
/// to identify it.
NIKOLA_API ResourceID resource_storage_push(ResourceStorage* storage, const String& shader_src);

/// Allocate a new `Mesh` using `loader`, store it in `storage`, and return a `ResourceID` 
/// to identify it.
NIKOLA_API ResourceID resource_storage_push(ResourceStorage* storage, const MeshLoader& loader);

/// Allocate a new `Material` using `loader`, store it in `storage`, and return a `ResourceID` 
/// to identify it.
NIKOLA_API ResourceID resource_storage_push(ResourceStorage* storage, const MaterialLoader& loader);

/// Allocate a new `Skybox` using `loader`, store it in `storage`, and return a `ResourceID` 
/// to identify it.
NIKOLA_API ResourceID resource_storage_push(ResourceStorage* storage, const SkyboxLoader& loader);

/// Retrieve `GfxBuffer` identified by `id` in `storage`. 
///
/// @NOTE: This function will assert if `id` is not found in `storage`.
NIKOLA_API GfxBuffer* resource_storage_get_buffer(ResourceStorage* storage, const ResourceID& id);

/// Retrieve `GfxTexture` identified by `id` in `storage`. 
///
/// @NOTE: This function will assert if `id` is not found in `storage`.
NIKOLA_API GfxTexture* resource_storage_get_texture(ResourceStorage* storage, const ResourceID& id);

/// Retrieve `GfxCubemap` identified by `id` in `storage`. 
///
/// @NOTE: This function will assert if `id` is not found in `storage`.
NIKOLA_API GfxCubemap* resource_storage_get_cubemap(ResourceStorage* storage, const ResourceID& id);

/// Retrieve `GfxShader` identified by `id` in `storage`. 
///
/// @NOTE: This function will assert if `id` is not found in `storage`.
NIKOLA_API GfxShader* resource_storage_get_shader(ResourceStorage* storage, const ResourceID& id);

/// Retrieve `Mesh` identified by `id` in `storage`. 
///
/// @NOTE: This function will assert if `id` is not found in `storage`.
NIKOLA_API Mesh* resource_storage_get_mesh(ResourceStorage* storage, const ResourceID& id);

/// Retrieve `Material` identified by `id` in `storage`. 
///
/// @NOTE: This function will assert if `id` is not found in `storage`.
NIKOLA_API Material* resource_storage_get_material(ResourceStorage* storage, const ResourceID& id);

/// Retrieve `Model` identified by `id` in `storage`. 
///
/// @NOTE: This function will assert if `id` is not found in `storage`.
NIKOLA_API Model* resource_storage_get_model(ResourceStorage* storage, const ResourceID& id);

/// Retrieve `Skybox` identified by `id` in `storage`. 
///
/// @NOTE: This function will assert if `id` is not found in `storage`.
NIKOLA_API Skybox* resource_storage_get_skybox(ResourceStorage* storage, const ResourceID& id);

/// Retrieve `Font` identified by `id` in `storage`. 
///
/// @NOTE: This function will assert if `id` is not found in `storage`.
NIKOLA_API Font* resource_storage_get_font(ResourceStorage* storage, const ResourceID& id);

/// Resource storage functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Resources ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Renderer ***

///---------------------------------------------------------------------------------------------------------------------
/// Camera consts 

/// The maximum degrees the camera can achieve 
const f32 CAMERA_MAX_DEGREES = 89.0f;

/// The maximum amount of zoom the camera can achieve
const f32 CAMERA_MAX_ZOOM    = 180.0f;

/// Camera consts 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Camera function pointers

// Have to do this to fix underfined variable errors in the callback.
struct Camera;

/// A function callback to move the camera every frame.
using CameraMoveFn = void(*)(Camera& camera);

/// Camera function pointers
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Camera 
struct Camera {
  f32 yaw, pitch;
  f32 zoom, aspect_ratio;

  f32 near        = 0.1f; 
  f32 far         = 100.0f;
  f32 sensitivity = 0.1f;

  Vec3 position, up, direction, front;
  Mat4 view, projection, view_projection;

  CameraMoveFn move_fn;
};
/// Camera 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Camera functions

/// The default function callback to use in order to move `camera`.
NIKOLA_API void camera_default_move_func(Camera& camera);

/// Fill the information in `cam` using the given values.
NIKOLA_API void camera_create(Camera* cam, const f32 aspect_ratio, const Vec3& pos, const Vec3& target, const CameraMoveFn& move_fn = camera_default_move_func);

/// Update the internal matrices of `cam` and call the associated `CameraMoveFn`. 
NIKOLA_API void camera_update(Camera& cam);

/// Camera functions
///---------------------------------------------------------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// RenderCommand
struct RenderCommand {
  RenderableType render_type;

  ResourceID renderable_id;
  ResourceID material_id;
  
  Transform transform;
  ResourceStorage* storage;
};
/// RenderCommand
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Renderer functions

/// Initialize the global renderer using the given `window` for dimensions 
/// and `clear_color` as the default background color.
NIKOLA_API void renderer_init(Window* window, const Vec4& clear_color);

/// Free/reclaim any memory consumed by the global renderer
NIKOLA_API void renderer_shutdown();

/// Retrieve the internal `GfxContext` of the global renderer.
NIKOLA_API const GfxContext* renderer_get_context();

/// Set the background color of the global renderer to `clear_color`
NIKOLA_API void renderer_set_clear_color(const Vec4& clear_color);

NIKOLA_API void renderer_pre_pass(Camera& cam);

NIKOLA_API void renderer_begin_pass();

NIKOLA_API void renderer_end_pass();

NIKOLA_API void renderer_post_pass();

NIKOLA_API void renderer_queue_command(const RenderCommand& command);

/// Renderer functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Renderer ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Engine ***

///---------------------------------------------------------------------------------------------------------------------
/// App
struct App;
/// App
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Args 
using Args = DynamicArray<String>;  
/// Args 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// App callbacks

/// A function callback to allocate and initialize a `App` struct.
using AppInitFn       = App*(*)(const Args& args, Window* window);

/// A function callback to free/reclaim any memory consumed by a `App` struct.
using AppShutdownFn   = void(*)(App* app);

/// A function callback to update a `App` struct.
using AppUpdateFn     = void(*)(App* app);

/// A function callback to render a `App` struct.
using AppRenderPassFn = void(*)(App* app);

/// App callbacks
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// App description
struct AppDesc {
  AppInitFn init_fn         = nullptr;
  AppShutdownFn shutdown_fn = nullptr;
  AppUpdateFn update_fn     = nullptr;
  AppRenderPassFn render_fn = nullptr;
 
  String window_title;
  i32 window_width, window_height;
  i32 window_flags;

  char** args_values = nullptr; 
  i32 args_count     = 0;
};
/// App description 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Engine functions

/// Initialize all of the engine sub-systems in order as well as 
/// allocate and initialize a new `App` struct using the information 
/// given in `desc`.
NIKOLA_API void engine_init(const AppDesc& desc);

/// Run a loop, updating and rendering the `App` struct allocated earlier 
/// as well as any engine sub-systems.
NIKOLA_API void engine_run();

/// Free/reclaim and shutdown any and all engine sub-systems as well 
/// as the `App` struct.
NIKOLA_API void engine_shutdown();

/// Engine functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Engine ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
