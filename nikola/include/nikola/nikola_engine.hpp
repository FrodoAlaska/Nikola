#pragma once

#include "nikola_core.hpp"

#include <glm/glm.hpp>
#include <glm/ext/quaternion_float.hpp>

#include <string>
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
/// FileOpenMode
enum FileOpenMode {
  /// Open a file in read-only mode.
  FILE_OPEN_READ       = 14 << 0,
 
  /// Open a file in write-only mode.
  FILE_OPEN_WRITE      = 14 << 1,
 
  /// Open a binary file.
  FILE_OPEN_BINARY     = 14 << 2,
 
  /// Open a file and append any extra data at the end of the file.
  FILE_OPEN_APPEND     = 14 << 3,
 
  /// Open a file and remove any existing data within.
  FILE_OPEN_TRUNCATE   = 14 << 4,
 
  /// Open a file and start at the end.
  FILE_OPEN_AT_END     = 14 << 5,

  /// Open a file in read and write mode.
  FILE_OPEN_READ_WRITE = 14 << 6
};
/// FileOpenMode
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FilePath
using FilePath = String;
/// FilePath
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// File
using File = std::fstream;
/// File
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FileIterateFunc callback
using FileIterateFunc = void(*)(const FilePath& base_dir, FilePath current_path, void* user_data);
/// FileIterateFunc callback
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Filesystem functions

/// Iterate through the given `dir`, calling `iter_func` for each entry and passing `user_data`.
NIKOLA_API void filesystem_directory_iterate(const FilePath& dir, const FileIterateFunc& iter_func, const void* user_data = nullptr);

/// Recursively iterate through the given `dir`, calling `iter_func` for each entry and passing `user_data`.
NIKOLA_API void filesystem_directory_recurse_iterate(const FilePath& dir, const FileIterateFunc& iter_func, const void* user_data = nullptr);

/// Get the current full path of the running process.
NIKOLA_API FilePath filesystem_current_path();

/// Check if the file at `path` exists.
NIKOLA_API bool filesystem_exists(const FilePath& path);

/// Filesystem functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FilePath functions

/// Append the given `other` to `base` and return the result.
///
/// @NOTE: This function will adhere to the specific operating systems's seperator.
NIKOLA_API FilePath filepath_append(FilePath& base, const FilePath& other);

/// Retrieve the root name (for example, `C:` on Windows) of `path`.
/// 
/// @EXAMPLE: The root namt would be `C:` on Windows.
NIKOLA_API FilePath filepath_root_name(const FilePath& path);

/// Retrieve the root directory of `path`.
/// 
/// @EXAMPLE: The root directory would be `\` on Windows.
NIKOLA_API FilePath filepath_root_dir(const FilePath& path);

/// Retrieve the full root path of `path`.
/// 
/// @EXAMPLE: The root path would be `C:\` on Windows.
NIKOLA_API FilePath filepath_root_path(const FilePath& path);

/// Retrieve the full relative path of `path`. 
///
/// @EXAMPLE: The relative path of `C:\res\shaders\shader.nbr` would be `res\shaders\shader.nbr`.
NIKOLA_API FilePath filepath_relative_path(const FilePath& path);

/// Retrieve the full parent path of `path`. 
///
/// @EXAMPLE: The parent path of `res/shaders/shader.nbr` would be `res/shaders/`.
NIKOLA_API FilePath filepath_parent_path(const FilePath& path);

/// Retrieve the filename of `path`.
///
/// @EXAMPLE: The filename of `res/shaders/shader.nbr` would be `shader.nbr`.
NIKOLA_API FilePath filepath_filename(const FilePath& path);

/// Retrieve the stem of `path`.
///
/// @EXAMPLE: The stem of `res/shaders/shader.nbr` would be `shader`.
NIKOLA_API FilePath filepath_stem(const FilePath& path);

/// Retrieve the extension of `path`.
///
/// @EXAMPLE: The extension of `res/shaders/shader.nbr` would be `.nbr`.
NIKOLA_API FilePath filepath_extension(const FilePath& path);

/// Set the filename of `path` to the given `name`.
NIKOLA_API void filepath_set_filename(FilePath& path, const FilePath& name);

/// Set the extension of `path` to the given `ext`.
NIKOLA_API void filepath_set_extension(FilePath& path, const FilePath& ext);

/// Remove the filename of `path`. 
///
/// @EXAMPLE: The path `res/shaders/shader.nbr` will become `res/shaders/`
NIKOLA_API void filepath_remove_filename(FilePath& path);

/// Check if `path` has a root name.
NIKOLA_API bool filepath_has_root_name(const FilePath& path);

/// Check if `path` has a root directory.
NIKOLA_API bool filepath_has_root_dir(const FilePath& path);

/// Check if `path` has a root path.
NIKOLA_API bool filepath_has_root_path(const FilePath& path);

/// Check if `path` has a ralative path.
NIKOLA_API bool filepath_has_relative_path(const FilePath& path);

/// Check if `path` has a parent path.
NIKOLA_API bool filepath_has_parent_path(const FilePath& path);

/// Check if `path` has a filename.
NIKOLA_API bool filepath_has_filename(const FilePath& path);

/// Check if `path` has a stem.
NIKOLA_API bool filepath_has_stem(const FilePath& path);

/// Check if `path` has an extension.
NIKOLA_API bool filepath_has_extension(const FilePath& path);

/// Check if `path` is an empty string.
NIKOLA_API bool filepath_is_empty(const FilePath& path);

/// Check if `path` is a relative path.
NIKOLA_API bool filepath_is_relative(const FilePath& path);

/// Check if `path` is an absolute path.
NIKOLA_API bool filepath_is_absolute(const FilePath& path);

/// FilePath functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// File functions

/// Open `file` with OR'd flags `mode` at C-string `path`, and return `true` if 
/// the operation was successfull and `false` otherwise.
///
/// `mode`:
///   - `FILE_OPEN_READ`       = Open `file` in read-only mode.
///   - `FILE_OPEN_WRITE`      = Open `file` in write-only mode.
///   - `FILE_OPEN_BINARY`     = Open `file` in binary mode.
///   - `FILE_OPEN_APPEND`     = Open `file` and append any new data at the end.
///   - `FILE_OPEN_TRUNCATE`   = Open `file` and delete any existing data.
///   - `FILE_OPEN_AT_END`     = Open `file` and start at the very end.
///   - `FILE_OPEN_READ_WRITE` = Open `file` in read and write mode.
NIKOLA_API bool file_open(File* file, const char* path, const i32 mode);

/// Open `file` with OR'd flags `mode` at `FilePath` `path`, and return `true` if 
/// the operation was successfull and `false` otherwise.
///
/// `mode`:
///   - `FILE_OPEN_READ`       = Open `file` in read-only mode.
///   - `FILE_OPEN_WRITE`      = Open `file` in write-only mode.
///   - `FILE_OPEN_BINARY`     = Open `file` in binary mode.
///   - `FILE_OPEN_APPEND`     = Open `file` and append any new data at the end.
///   - `FILE_OPEN_TRUNCATE`   = Open `file` and delete any existing data.
///   - `FILE_OPEN_AT_END`     = Open `file` and start at the very end.
///   - `FILE_OPEN_READ_WRITE` = Open `file` in read and write mode.
NIKOLA_API bool file_open(File* file, const FilePath& path, const i32 mode);

/// Close `file` if it is open.
NIKOLA_API void file_close(File& file);

/// Return `ture` if `file` is currently open and `false` otherwise.
NIKOLA_API bool file_is_open(File& file);

/// Seek the write pointer in `file` to `pos`.
NIKOLA_API void file_seek_write(File& file, const sizei pos);

/// Seek the read pointer in `file` to `pos`.
NIKOLA_API void file_seek_read(File& file, const sizei pos);

/// Return the current position of the write pointer in `file`.
NIKOLA_API const sizei file_tell_write(File& file);

/// Return the current position of the read pointer in `file`.
NIKOLA_API const sizei file_tell_read(File& file);

/// Return the current size in bytes of `file`.
NIKOLA_API const sizei file_get_size(File& file);

/// Return `true` if `file` is currently empty and `false` otherwise.
NIKOLA_API bool file_is_empty(File& file);

/// Write the `buff` of `buff_size` size in bytes with an `offset` in `file` and return 
/// the amount of bytes written (i.e `buff_size` + `offset`).
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API const sizei file_write_bytes(File& file, const void* buff, const sizei buff_size, const sizei offset = 0);

/// Read data into `out_buff` with `size` size in bytes with an `offset` in `file` 
/// and return the amount of bytes read (i.e `size` + `offset`).
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API const sizei file_read_bytes(File& file, void* out_buff, const sizei size, const sizei offset = 0);

/// Write the given `string` into `file`.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API void file_write_string(File& file, const String& string);

/// Return the read string in `file`.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API String file_read_string(File& file);

/// File functions
///---------------------------------------------------------------------------------------------------------------------

/// *** File system ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ** NBR (Nikola Binary Resource) ***

///---------------------------------------------------------------------------------------------------------------------
/// NBR consts

/// A value present at the top of each `.nbr` file to denote 
/// a valid `.nbr` file. 
///
/// @NOTE: The value is the summed average of the ASCII hex codes of `n`, `b`, and `r`.
const u8 NBR_VALID_IDENTIFIER     = 107;

/// The currently valid major version of any `.nbr` file
const i16 NBR_VALID_MAJOR_VERSION = 0;

/// The currently valid minor version of any `.nbr` file
const i16 NBR_VALID_MINOR_VERSION = 1;

/// NBR consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBRTexture
struct NBRTexture {
  /// The width and height of the texture.
  u32 width, height; 

  /// The number of channel components per pixel.
  i8 channels; 

  /// The raw pixel data.
  void* pixels = nullptr;
};
/// NBRTexture
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBRCubemap
struct NBRCubemap {
  /// The width and height of all of the faces.
  u32 width, height; 
  
  /// The number of channel components per pixel.
  i8 channels; 
  
  /// The amount of faces in `pixels`.
  u8 faces_count;

  /// An array of raw pixel data for each face.
  ///
  /// @NOTE: This array can only go to `CUBEMAP_FACES_MAX`;
  u8* pixels[CUBEMAP_FACES_MAX];
};
/// NBRCubemap
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBRShader
struct NBRShader {
  /// The total amount of characters in the `vertex_source` string.
  u16 vertex_length; 

  /// The full string representation of the vertex shader.
  i8* vertex_source; 

  /// The total amount of characters in the `pixel_source` string.
  u16 pixel_length; 
  
  /// The full string representation of the pixel/fragment shader.
  i8* pixel_source;
};
/// NBRShader
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBRMaterial
struct NBRMaterial {
  /// An RGB value array of the ambient color.
  f32 ambient[3];
  
  /// An RGB value array of the diffuse color.
  f32 diffuse[3];
  
  /// An RGB value array of the specular color.
  f32 specular[3];

  /// The diffuse index into the `textures` array in `NBRModel`.
  ///
  /// @NOTE: This index will be `0` if there is no diffuse texture present
  i8 diffuse_index;
  
  /// The diffuse index into the `textures` array in `NBRModel`.
  ///
  /// @NOTE: This index will be `0` if there is no specular texture present
  i8 specular_index;
};
/// NBRMaterial
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBRMesh 
struct NBRMesh {
  /// A value from the `VertexType` enum to denote the 
  /// stride of `vertices`.
  u8 vertex_type; 

  /// The total number of vertices in `vertices`.
  u32 vertices_count; 
  
  /// A `float` array of all the vertices.
  f32* vertices;

  /// The total number of indices in `indices`. 
  u32 indices_count; 

  /// An `unsigned int` array of the indices.
  u32* indices;

  /// An index into the `matrices` array in `NBRModel`. 
  ///
  /// @NOTE: This value will be `0` if no materials are present 
  /// in this mesh. 
  u8 material_index;
};
/// NBRMesh 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBRModel 
struct NBRModel {
  /// The total number of meshes in the `meshes` array.
  u16 meshes_count;

  /// An array of `NBRMesh`.
  NBRMesh* meshes;

  /// The total number of materials in `materials`.
  u8 materials_count; 

  /// An array of `NBRMaterial`.
  NBRMaterial* materials;

  /// The total number of textures in `textures`.
  u8 textures_count;

  /// An array of `NBRTexture`.
  NBRTexture* textures;
};
/// NBRModel 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBRFile 
struct NBRFile {
  /// A reference to the initial given file path.
  FilePath path;

  /// The internal handle for the opened file.
  File file_handle;

  /// A 1-byte value to correctly identify an NBR file.
  u8 identifier;                 

  /// A 2-bytes value for the major version of the file.
  i16 major_version;
  
  /// A 2-bytes value for the minor version of the file. 
  i16 minor_version; 

  /// A 2-bytes value for the resource type to be parsed.
  i16 resource_type;                

  /// The actual data of the file.
  void* body_data; 
};
/// NBRFile
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBR file functions

/// Open and load the appropriate data found at `path` into the given `nbr`.
NIKOLA_API void nbr_file_load(NBRFile* nbr, const FilePath& path);

/// Reclaim/free any memory consumed by `nbr`.
NIKOLA_API void nbr_file_unload(NBRFile& nbr);

/// Returns `true` if the given `nbr_path` has a valid NBR extension. 
NIKOLA_API const bool nbr_file_valid_extension(const FilePath& nbr_path);

/// Save the given `texture` at `path` using `nbr`'s information.
NIKOLA_API void nbr_file_save(NBRFile& nbr, const NBRTexture& texture, const FilePath& path);

/// Save the given `cubemap` at `path` using `nbr`'s information.
NIKOLA_API void nbr_file_save(NBRFile& nbr, const NBRCubemap& cubemap, const FilePath& path);

/// Save the given `shader` at `path` using `nbr`'s information.
NIKOLA_API void nbr_file_save(NBRFile& nbr, const NBRShader& shader, const FilePath& path);

/// Save the given `model` at `path` using `nbr`'s information.
NIKOLA_API void nbr_file_save(NBRFile& nbr, const NBRModel& model, const FilePath& path);

/// NBR file functions
///---------------------------------------------------------------------------------------------------------------------

/// ** NBR (Nikola Binary Resource) ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Resources ***

///---------------------------------------------------------------------------------------------------------------------
/// Resources consts

/// A value to indicate an invalid resource group.
const u16 RESOURCE_GROUP_INVALID           = ((u16)-1);

/// The ID of the group associated with the resource cache.
const u16 RESOURCE_CACHE_ID                = 0;

/// The maximum amount of declared uniform buffers in all materials.
const sizei MATERIAL_UNIFORM_BUFFERS_MAX   = 2;

/// The index of the matrices uniform buffer within all materials.
const sizei MATERIAL_MATRICES_BUFFER_INDEX = 0;

/// The index of the lighting uniform buffer within all materials.
const sizei MATERIAL_LIGHTING_BUFFER_INDEX = 1;

/// The maximum amount of preset uniforms. 
const u32 MATERIAL_UNIFORMS_MAX            = 6;

/// The name of the ambient color uniform in materials. 
#define MATERIAL_UNIFORM_AMBIENT_COLOR  "u_material.ambient" 

/// The name of the diffuse color uniform in materials. 
#define MATERIAL_UNIFORM_DIFFUSE_COLOR  "u_material.diffuse" 

/// The name of the specular color uniform in materials. 
#define MATERIAL_UNIFORM_SPECULAR_COLOR "u_material.specular"

/// The name of the shininess uniform in materials. 
#define MATERIAL_UNIFORM_SHININESS      "u_material.shininess" 

/// The name of the screen size uniform in materials. 
#define MATERIAL_UNIFORM_SCREEN_SIZE    "u_material.screen_size" 

/// The name of the model transform uniform in materials. 
#define MATERIAL_UNIFORM_MODEL_MATRIX   "u_model" 

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
/// MaterialTextureType
enum MaterialTextureType {
  /// Indicate the diffuse texture in a `Material`.
  MATERIAL_TEXTURE_DIFFUSE  = 17 << 0,
  
  /// Indicate the specular texture in a `Material`.
  MATERIAL_TEXTURE_SPECULAR = 17 << 1,
  
  /// Indicate the normal texture in a `Material`.
  MATERIAL_TEXTURE_NORMAL   = 17 << 2,
};
/// MaterialTextureType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ResourceID
struct ResourceID {
  //
  // @NOTE: These are internal variables and should NOT be changed!
  //

  /// The type of the resource 
  ResourceType _type; 

  /// The underlying ID that will be generated upon the 
  /// resource's creation.
  u16 _id;

  /// The parent resource group that will be used to 
  /// retrieve the resource later. 
  u16 group = RESOURCE_GROUP_INVALID;
};
/// ResourceID
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Mesh 
struct Mesh {
  GfxBuffer* vertex_buffer = nullptr; 
  GfxBuffer* index_buffer  = nullptr;

  GfxPipeline* pipe         = nullptr;
  GfxPipelineDesc pipe_desc = {};
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
  
  Vec3 ambient_color;
  Vec3 diffuse_color; 
  Vec3 specular_color;
  f32 shininess;
  Mat4 model_matrix;
  Vec2 screen_size;

  HashMap<String, i32> uniform_locations;
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
};
/// Skybox
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Model 
struct Model {
  DynamicArray<Mesh*> meshes;
  DynamicArray<Material*> materials;
  DynamicArray<u8> material_indices;
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
};
/// Font 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Material functions

/// Set a uniform of type `i32` with the name `uniform_name` in `mat_id` to the given `value`. 
NIKOLA_API void material_set_uniform(ResourceID& mat_id, const i8* uniform_name, const i32 value);

/// Set a uniform of type `f32` with the name `uniform_name` in `mat_id` to the given `value`. 
NIKOLA_API void material_set_uniform(ResourceID& mat_id, const i8* uniform_name, const f32 value);

/// Set a uniform of type `Vec2` with the name `uniform_name` in `mat_id` to the given `value`. 
NIKOLA_API void material_set_uniform(ResourceID& mat_id, const i8* uniform_name, const Vec2& value);

/// Set a uniform of type `Vec3` with the name `uniform_name` in `mat_id` to the given `value`. 
NIKOLA_API void material_set_uniform(ResourceID& mat_id, const i8* uniform_name, const Vec3& value);

/// Set a uniform of type `Vec4` with the name `uniform_name` in `mat_id` to the given `value`. 
NIKOLA_API void material_set_uniform(ResourceID& mat_id, const i8* uniform_name, const Vec4& value);

/// Set a uniform of type `Mat4` with the name `uniform_name` in `mat_id` to the given `value`. 
NIKOLA_API void material_set_uniform(ResourceID& mat_id, const i8* uniform_name, const Mat4& value);

/// Set the data of the uniform buffer at `index` of the associated shader in `mat_id` to `buffer`
NIKOLA_API void material_set_uniform_buffer(ResourceID& mat_id, const sizei index, GfxBuffer* buffer);

/// Set the texture of `type` in `mat_id`, using `texture_id`. 
/// 
/// @NOTE: If either `texture_id` or `mat_id` are invalid, this function will assert. 
NIKOLA_API void material_set_texture(ResourceID& mat_id, const MaterialTextureType type, const ResourceID& texture_id);

/// Go over all of the available uniforms in `uniform_locations` in `mat_id` and send the appropriate data.
///
/// @NOTE: This will ONLY send the uniforms with the `MATERIAL_UNIFORM_*` constants.
/// The `material_set_uniform` functions, however, will send any other data.
NIKOLA_API void material_use(ResourceID& mat_id);

/// Material functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Resource manager functions

/// Initialize the global resource manager as well as the global cache.
NIKOLA_API void resource_manager_init();

/// Free/reclaim any memory consumed by the global resource manager.
NIKOLA_API void resource_manager_shutdown();

/// Create and return a new resource group (a.k.a `unsigned short`) with `name` and `parent_dir`. 
///
/// @NOTE: Any `_push` function that takes a `path` will be prefixed with the given `parent_dir`.
NIKOLA_API u16 resources_create_group(const String& name, const FilePath& parent_dir);

/// Clear all of resources in `group_id`.
NIKOLA_API void resources_clear_group(const u16 group_id);

/// Clear and destroy all of resources in `group_id`.
NIKOLA_API void resources_destroy_group(const u16 group_id);

/// Allocate a new `GfxBuffer` using `buff_desc`, store it in `group_id`, and return a `ResourceID` 
/// to identify it.
NIKOLA_API ResourceID resources_push_buffer(const u16 group_id, const GfxBufferDesc& buff_desc);

/// Allocate a new `GfxTexture` using `desc`, store it in `group`,
/// and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_texture(const u16 group_id, const GfxTextureDesc& desc);

/// Allocate a new `GfxTexture` using the texture retrieved from the `nbr_path`, 
/// store it in `group_id`, and return a `ResourceID` to identify it.
///
/// Default values: 
///   - `format` = `GFX_TEXTURE_FORMAT_RGBA8`.
///   - `filter` = `GFX_TEXTURE_FILTER_MIN_MAG_NEAREST`.
///   - `wrap`   = `GFX_TEXTURE_WRAP_CLAMP`.
NIKOLA_API ResourceID resources_push_texture(const u16 group_id, 
                                             const FilePath& nbr_path,
                                             const GfxTextureFormat format = GFX_TEXTURE_FORMAT_RGBA8, 
                                             const GfxTextureFilter filter = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
                                             const GfxTextureWrap wrap     = GFX_TEXTURE_WRAP_CLAMP);

/// Allocate a new `GfxCubemap` using `desc`, store it in `group_id`,
/// and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_cubemap(const u16 group_id, const GfxCubemapDesc& desc);

/// Allocate a new `GfxCubemap` using the cubemap retrieved from the `nbr_path`, 
/// store it in `group_id`, and return a `ResourceID` to identify it.
///
/// Default values: 
///   - `format` = `GFX_TEXTURE_FORMAT_RGBA8`.
///   - `filter` = `GFX_TEXTURE_FILTER_MIN_MAG_NEAREST`.
///   - `wrap`   = `GFX_TEXTURE_WRAP_CLAMP`.
///
/// @NOTE: The number of faces of the cubemap will be determined inside the NBR file.
NIKOLA_API ResourceID resources_push_cubemap(const u16 group_id, 
                                             const FilePath& nbr_path,
                                             const GfxTextureFormat format = GFX_TEXTURE_FORMAT_RGBA8, 
                                             const GfxTextureFilter filter = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
                                             const GfxTextureWrap wrap     = GFX_TEXTURE_WRAP_CLAMP);

/// Allocate a new `GfxShader` using `shader_desc`, store it in `group_id`, and return a `ResourceID` 
/// to identify it.
NIKOLA_API ResourceID resources_push_shader(const u16 group_id, const GfxShaderDesc& shader_desc);

/// Allocate a new `GfxShader` using the shader retrieved from the `nbr_path`, 
/// store it in `group_id`, and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_shader(const u16 group_id, const FilePath& nbr_path);

/// Allocate a new `Mesh` using `vertex_buffer_id` and `index_buffer_id`, 
/// store it in `group_id`, return a `ResourceID` to identified it. 
///
/// A `vertex_type` must be provided to 
/// calculate the stride, while `indices_count` will be used in case `index_buffer_id` 
/// is not set to a default.
///
/// @NOTE: The value of `index_buffer_id` can be set to a default value to be ignored.
/// The same cannot be said for `vertex_buffer_id`.
NIKOLA_API ResourceID resources_push_mesh(const u16 group_id, 
                                          const ResourceID& vertex_buffer_id, 
                                          const VertexType vertex_type, 
                                          const ResourceID& index_buffer_id, 
                                          const sizei indices_count);

/// Allocate a new `Mesh` using a predefined mesh `type`, 
/// store it in `group_id`, return a `ResourceID` to identified it. 
NIKOLA_API ResourceID resources_push_mesh(const u16 group_id, const MeshType type);

/// Allocate a new `Material` using the shader `shader_id`, store it in `group_id`, and 
/// return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_material(const u16 group_id, const ResourceID& shader_id);

/// Allocate a new `Skybox` using the previously-added `cubemap_id`, store it in `group_id`, 
/// and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_skybox(const u16 group_id, const ResourceID& cubemap_id);

/// Allocate a new `Model` using the `NBRModel` retrieved from the `nbr_path`, 
/// store it in `group_id`, and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_model(const u16 group_id, const FilePath& nbr_path);

/// Retrieve all of the valid resources from `dir` and store the resulting entries in an
/// internal list (where the key is the file name of the resource and the value is its ID) 
/// while ensuring that each entry is pushed into `group_id` with an ID. The IDs can be retrieved 
/// later using the function `resources_get_id`.
///
/// @NOTE: The given `dir` is prepended with the `parent_dir` given when `group_id` was created.
NIKOLA_API void resources_push_dir(const u16 group_id, const FilePath& dir);

/// Search and retrieve the ID of the resource `filename` in `group_id`. 
/// If `filename` was not found in `group_id`, a default `ResourceID` will be returned. 
NIKOLA_API ResourceID& resources_get_id(const u16 group_id, const nikola::String& filename);

/// Retrieve `GfxBuffer` identified by `id` in `group`. 
///
/// @NOTE: This function will assert if `id` is not found in `group`.
NIKOLA_API GfxBuffer* resources_get_buffer(const ResourceID& id);

/// Retrieve `GfxTexture` identified by `id` in `id.group`. 
///
/// @NOTE: This function will assert if `id` is not found in `id.group`.
NIKOLA_API GfxTexture* resources_get_texture(const ResourceID& id);

/// Retrieve `GfxCubemap` identified by `id` in `id.group`. 
///
/// @NOTE: This function will assert if `id` is not found in `id.group`.
NIKOLA_API GfxCubemap* resources_get_cubemap(const ResourceID& id);

/// Retrieve `GfxShader` identified by `id` in `id.group`. 
///
/// @NOTE: This function will assert if `id` is not found in `id.group`.
NIKOLA_API GfxShader* resources_get_shader(const ResourceID& id);

/// Retrieve `Mesh` identified by `id` in `id.group`. 
///
/// @NOTE: This function will assert if `id` is not found in `id.group`.
NIKOLA_API Mesh* resources_get_mesh(const ResourceID& id);

/// Retrieve `Material` identified by `id` in `id.group`. 
///
/// @NOTE: This function will assert if `id` is not found in `id.group`.
NIKOLA_API Material* resources_get_material(const ResourceID& id);

/// Retrieve `Skybox` identified by `id` in `id.group`. 
///
/// @NOTE: This function will assert if `id` is not found in `id.group`.
NIKOLA_API Skybox* resources_get_skybox(const ResourceID& id);

/// Retrieve `Model` identified by `id` in `id.group`. 
///
/// @NOTE: This function will assert if `id` is not found in `id.group`.
NIKOLA_API Model* resources_get_model(const ResourceID& id);

/// Retrieve `Font` identified by `id` in `id.group`. 
///
/// @NOTE: This function will assert if `id` is not found in `id.group`.
NIKOLA_API Font* resources_get_font(const ResourceID& id);

/// Resource manager functions
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
/// RenderableType 
enum RenderableType {
  /// Will commence a mesh rendering operation
  RENDERABLE_TYPE_MESH   = 18 << 0,
  
  /// Will commence a model rendering operation
  RENDERABLE_TYPE_MODEL  = 18 << 1,
  
  /// Will commence a skybox rendering operation
  RENDERABLE_TYPE_SKYBOX = 18 << 2,
};
/// RenderableType 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderEffectType
enum RenderEffectType {
  RENDER_EFFECT_NONE        = 19 << 0, 
 
  RENDER_EFFECT_GREYSCALE   = 19 << 1, 
 
  RENDER_EFFECT_INVERSION   = 19 << 2, 
 
  RENDER_EFFECT_SHARPEN     = 19 << 3, 
 
  RENDER_EFFECT_BLUR        = 19 << 4, 
  
  RENDER_EFFECT_EMBOSS      = 19 << 5,

  RENDER_EFFECT_EDGE_DETECT = 19 << 6, 
  
  RENDER_EFFECT_PIXELIZE    = 19 << 7, 

  RENDER_EFFECTS_MAX        = 8,
};
/// RenderEffectType
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
/// RendererDefaults 
struct RendererDefaults {
  GfxTexture* texture        = nullptr;
  GfxBuffer* matrices_buffer = nullptr;
};
/// RendererDefaults 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderData
struct RenderData {
  Camera camera;
};
/// RenderData
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderCommand
struct RenderCommand {
  RenderableType render_type;

  ResourceID renderable_id;
  ResourceID material_id;
  
  Transform transform;
};
/// RenderCommand
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

/// Retrieve the internal default values of the renderer
NIKOLA_API const RendererDefaults& renderer_get_defaults();

NIKOLA_API void renderer_begin_pass(RenderData& data);

NIKOLA_API void renderer_end_pass();

NIKOLA_API void renderer_present();

NIKOLA_API void renderer_apply_effect(const RenderEffectType effect);

NIKOLA_API RenderEffectType renderer_current_effect();

NIKOLA_API void renderer_queue_command(RenderCommand& command);

/// Renderer functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Batch renderer functions

NIKOLA_API void batch_renderer_init();

NIKOLA_API void batch_renderer_shutdown();

NIKOLA_API void batch_renderer_begin();

NIKOLA_API void batch_renderer_end();

NIKOLA_API void batch_render_quad(const Vec2& position, const Vec2& size, const Vec4& color);

NIKOLA_API void batch_render_texture(GfxTexture* texture, const Vec2& position, const Vec2& size, const Vec4& tint = Vec4(1.0f));

/// Batch renderer functions
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

/// A function callback to update a `App` struct, passing in the `delta_time`.
using AppUpdateFn     = void(*)(App* app, const f64 delta_time);

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
