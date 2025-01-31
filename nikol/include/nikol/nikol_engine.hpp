#pragma once

#include "nikol_core.hpp"

#include <glm/glm.hpp>
#include <glm/ext/quaternion_float.hpp>

#include <string>
#include <filesystem>
#include <vector>
#include <unordered_map>

//////////////////////////////////////////////////////////////////////////

namespace nikol { // Start of nikol

/// ----------------------------------------------------------------------
/// *** Typedefs ***

using String       = std::string;

using FilePath     = std::filesystem::path;

template<typename T>
using DynamicArray = std::vector<T>;

template<typename K, typename V> 
using HashMap      = std::unordered_map<K, V>;

/// *** Typedefs ***
/// ----------------------------------------------------------------------

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
/// *** Math ***

/// ----------------------------------------------------------------------
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
/// Vertex3D_PCUV (Position, Color, U/V texture coords)
struct Vertex3D_PCUV {
  Vec3 position;
  Vec4 color;
  Vec2 texture_coords;
};
/// Vertex3D_PCUV (Position, Color, U/V texture coords)
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Vertex3D_PNCUV (Position, Normal, Color (r, g, b, a), U/V texture coords)
struct Vertex3D_PNCUV {
  Vec3 position;
  Vec3 normal;
  Vec4 color;
  Vec2 texture_coords;
};
/// Vertex3D_PNCUV (Position, Normal, Color (r, g, b, a), U/V texture coords)
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
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
/// ----------------------------------------------------------------------

/// *** Math ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Resources ***

/// ----------------------------------------------------------------------
/// ResourceID
typedef u64 ResourceUUID;

struct ResourceID {
  ResourceUUID uuid;

  // Hashing the given `str` to a `ResourceUUID`
  ResourceID(const String& str) {
    u32 hash  = 2166136261u;
    sizei len = str.length();

    for(sizei i = 0; i < len; i++) {
      hash ^= (u8)str[i];
      hash *= 1677719;
    }

    uuid = hash;
  }
};
/// ResourceID
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ResourceType
enum ResourceType {
  RESOURCE_TYPE_BUFFER   = 14 << 0, 
  RESOURCE_TYPE_TEXTURE  = 14 << 1, 
  RESOURCE_TYPE_CUBEMAP  = 14 << 2,
  RESOURCE_TYPE_SHADER   = 14 << 4,
  RESOURCE_TYPE_MESH     = 14 << 5,
  RESOURCE_TYPE_MATERIAL = 14 << 6,
  RESOURCE_TYPE_SKYBOX   = 14 << 7,
  RESOURCE_TYPE_MODEL    = 14 << 8,
  RESOURCE_TYPE_FONT     = 14 << 9,
};
/// ResourceType
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Mesh 
struct Mesh {
  ResourceID vertex_buffer; 
  ResourceID index_buffer;

  GfxPipeline* pipe         = nullptr;
  GfxPipelineDesc pipe_desc = {};
};
/// Mesh 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Material 
struct Material {
  ResourceID diffuse_map;
  ResourceID specular_map;
  ResourceID shader; 
  
  ResourceID uniform_buffers[UNIFORM_BUFFERS_MAX];
  sizei uniform_buffers_count = 0; 
};
/// Material 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Skybox
struct Skybox {
  ResourceID vertex_buffer;
  ResourceID cubemap;
  
  GfxPipeline* pipe         = nullptr;
  GfxPipelineDesc pipe_desc = {};
};
/// Skybox
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Model 
struct Model {
  DynamicArray<ResourceID> meshes;
  DynamicArray<ResourceID> materials;
};
/// Model 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Font 
struct Font {
  struct Glyph {
    i8 unicode; 
    ResourceID texture;

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
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Texture loader functions

void texture_loader_load(GfxTextureDesc* desc, 
                         const FilePath& path, 
                         const GfxTextureFormat format = GFX_TEXTURE_FORMAT_RGBA8, 
                         const GfxTextureFilter filter = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
                         const GfxTextureWrap wrap     = GFX_TEXTURE_WRAP_CLAMP);

void texture_loader_unload(GfxTextureDesc& desc);

/// Texture loader functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Cubemap loader functions

void cubemap_loader_load(GfxCubemapDesc* desc, 
                         const FilePath path[CUBEMAP_FACES_MAX], 
                         const sizei faces_count,
                         const GfxTextureFormat format = GFX_TEXTURE_FORMAT_RGBA8, 
                         const GfxTextureFilter filter = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
                         const GfxTextureWrap wrap     = GFX_TEXTURE_WRAP_CLAMP);

void cubemap_loader_load(GfxCubemapDesc* desc, 
                         const FilePath& directory,
                         const GfxTextureFormat format = GFX_TEXTURE_FORMAT_RGBA8, 
                         const GfxTextureFilter filter = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
                         const GfxTextureWrap wrap     = GFX_TEXTURE_WRAP_CLAMP);

void cubemap_loader_unload(GfxCubemapDesc& desc);

/// Cubemap loader functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ResourceStorage
struct ResourceStorage;
/// ResourceStorage
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Resource storage functions

void resource_manager_init(GfxContext* gfx);

void resource_manager_destroy();

const ResourceStorage* resource_manager_cache();

void resource_manager_clear_cache();

/// Resource storage functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Resource storage functions

ResourceStorage* resource_storage_create(const String& name, const FilePath& parent_dir);

void resource_storage_clear(ResourceStorage* storage);

void resource_storage_destroy(ResourceStorage* storage);

ResourceUUID resource_storage_push_buffer(ResourceStorage* storage, const String& sid, const GfxBufferDesc& buff_desc);

ResourceUUID resource_storage_push_texture(ResourceStorage* storage, const String& sid, const GfxTextureDesc& tex_desc);

ResourceUUID resource_storage_push_cubemap(ResourceStorage* storage, const String& sid, const GfxCubemapDesc& cubemap_desc);

ResourceUUID resource_storage_push_shader(ResourceStorage* storage, const String& sid, const String& shader_src);

GfxBuffer* resource_storage_get_buffer(ResourceStorage* storage, const ResourceUUID& id);

GfxTexture* resource_storage_get_texture(ResourceStorage* storage, const ResourceUUID& id);

GfxCubemap* resource_storage_get_cubemap(ResourceStorage* storage, const ResourceUUID& id);

GfxShader* resource_storage_get_shader(ResourceStorage* storage, const ResourceUUID& id);

/// Resource storage functions
/// ----------------------------------------------------------------------

/// *** Resources ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Renderer ***

/// ----------------------------------------------------------------------
/// Renderer functions

void renderer_init(Window* window);

void renderer_shutdown();

const GfxContext* renderer_get_context();

/// Renderer functions
/// ----------------------------------------------------------------------

/// *** Renderer ***
/// ----------------------------------------------------------------------

} // End of nikol

//////////////////////////////////////////////////////////////////////////
