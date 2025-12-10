#pragma once

#include "nikola_gfx.h"
#include "nikola_file.h"
#include "nikola_math.h"
#include "nikola_audio.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

// Forward declarations

struct PointLight;
struct DirectionalLight;
struct SpotLight;
struct Mesh;
struct MaterialDesc; 
struct Material; 
struct ShaderContext; 
struct Skybox;
struct Model;
struct Skeleton;
struct Animation;
struct Font;

/// ----------------------------------------------------------------------
/// ** NBR (Nikola Binary Resource) ***

///---------------------------------------------------------------------------------------------------------------------
/// NBR consts

/// A value present at the top of each `.nbr` file to denote 
/// a valid `.nbr` file. 
///
/// @NOTE: The value is the average of the ASCII hex codes of `n`, `b`, and `r`.
const u8 NBR_VALID_IDENTIFIER      = 107;

/// The currently valid major version of any `.nbr` file
const i16 NBR_VALID_MAJOR_VERSION  = 0;

/// The currently valid minor version of any `.nbr` file
const i16 NBR_VALID_MINOR_VERSION  = 8;

/// The maximum number of weights a joint can have in an NBR file. 
const sizei NBR_JOINT_WEIGHTS_MAX  = 4;

/// The maximum number of characters a joint name can have.
const sizei NBR_JOINT_NAME_MAX     = 256;

/// The maximum number of characters an animation name can have.
const sizei NBR_ANIMATION_NAME_MAX = 256;

/// Some 3D models have their own unit of scales when being imported. 
/// In order to unify all the models, we apply this multiplier to the models 
/// before improting to the final NBR format.
const f32 NBR_MODEL_IMPORT_SCALE  = 0.1f;

/// NBR consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBRHeader 
struct NBRHeader {
  /// A 1-byte value to correctly identify an NBR file.
  u8 identifier;                 

  /// A 2-bytes value for the major version of the file.
  i16 major_version;
  
  /// A 2-bytes value for the minor version of the file. 
  i16 minor_version; 

  /// A 2-bytes value for the resource type to be parsed.
  u16 resource_type;                
};
/// NBRHeader
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBRTexture
struct NBRTexture {
  /// The width and height of the texture.
  u32 width, height; 

  /// The number of channel components per pixel.
  i8 channels; 

  /// The texture format of this texture. 
  /// The value of this memeber can be compared to 
  /// `GfxTextureFormat`. 
  u8 format; 

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

  /// The texture format of this cubemap. 
  /// The value of this memeber can be compared to 
  /// `GfxTextureFormat`. 
  u8 format; 
  
  /// The amount of faces in `pixels`.
  u8 faces_count;

  /// An array of raw pixel data for each face.
  ///
  /// @NOTE: This array can only go to `CUBEMAP_FACES_MAX`;
  void* pixels[CUBEMAP_FACES_MAX];
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
  
  /// The total amount of characters in the `compute_source` string.
  u16 compute_length; 
  
  /// The full string representation of the compute shader.
  i8* compute_source;
};
/// NBRShader
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBRMaterial
struct NBRMaterial {
  /// An RGB value array of the base color.
  f32 color[3];
  
  /// A floating-point value indicating the metallic factor of this material.
  f32 metallic  = 0.0f;
  
  /// A floating-point value indicating the roughness factor of this material.
  f32 roughness = 1.0f;
  
  /// A floating-point value indicating the emissive factor of this material.
  f32 emissive  = 0.0f;

  /// The albedo index into the `textures` array in `NBRModel`.
  ///
  /// @NOTE: This index will be `-1` if there is no albedo texture present.
  i8 albedo_index    = -1;
  
  /// The metallic index into the `textures` array in `NBRModel`.
  ///
  /// @NOTE: This index will be `-1` if there is no metallic texture present.
  i8 metallic_index  = -1;
  
  /// The roughness index into the `textures` array in `NBRModel`.
  ///
  /// @NOTE: This index will be `-1` if there is no roughness texture present.
  i8 roughness_index = -1;
  
  /// The normal index into the `textures` array in `NBRModel`.
  ///
  /// @NOTE: This index will be `-1` if there is no normal texture present.
  i8 normal_index    = -1;
  
  /// The emissive index into the `textures` array in `NBRModel`.
  ///
  /// @NOTE: This index will be `-1` if there is no emissive texture present.
  i8 emissive_index  = -1;
};
/// NBRMaterial
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBRMesh 
struct NBRMesh {
  /// A bitfield from the `VertexComponentType` enum 
  /// that determines the exact components found in `vertices`. 
  u8 vertex_component_bits; 

  /// A `float` array of vertices with `vertices_count` elements.
  
  u32 vertices_count; 
  f32* vertices;

  /// A `u32` array of indices with `indices_count` elements.
  
  u32 indices_count; 
  u32* indices;

  /// An index into the `matrices` array in `NBRModel`. 
  ///
  /// @NOTE: This value will be `0` if no materials are present 
  /// in this mesh. 
  u8 material_index = 0;
};
/// NBRMesh 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBRModel 
struct NBRModel {
  /// An array of `NBRMesh` with `meshes_count` elements.
  
  u16 meshes_count;
  NBRMesh* meshes;

  /// An array of `NBRMaterial` with `materials_count` elements.
  
  u8 materials_count; 
  NBRMaterial* materials;

  /// An array of textures with `textures_count` elements.
  /// These textures can be either albedo, roughness, metallic, or emissive. 

  u8 textures_count;
  NBRTexture* textures;
};
/// NBRModel 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBRSkeleton
struct NBRSkeleton {
  struct NBRJoint {
    /// An array of `children_count` that represent 
    /// the children of this joint in a tree manner. 
    /// Each child is an index that can be used with 
    /// the `joints` array of this skeleton.

    u16 children_count = 0;
    u16* children      = nullptr;

    /// The position of the joint (X, Y, Z) in local space.
    f32 position[3];

    /// The rotation of the joint (X, Y, Z, W) in local space.
    f32 rotation[4];

    /// The scale of the joint (X, Y, Z) in local space.
    f32 scale[3];

    /// A 4x3 (4 rows, 3 columns) matrix representing the 
    /// inverse bind matrix of this joint. 
    f32 inverse_bind_matrix[12];

    /// The optional name of the joint, with 
    /// `name_length` number of characters.
    ///
    /// @NOTE: This is a C-string, missing the null-terminated character.

    u8 name_length = 0;
    char name[NBR_JOINT_NAME_MAX];
  };
 
  /// All of the joints in the skeleton that can be 
  /// referred to by the indices in each joint.

  u16 joints_count = 0; 
  NBRJoint* joints = nullptr;
  
  /// The index of the root joint of the skeleton. 
  /// Usually, this is set to `0`.
  u16 root_index = 0;
};
/// NBRSkeleton
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBRAnimation
struct NBRAnimation {
  struct NBRJointTrack {
    /// An array of interleaved position samples of `positions_count` 
    /// where the first three values of each entry 
    /// are the X, Y, and Z position values and the last 
    /// component is the timestamp. 
    /// 
    /// [x, y, z, ts][x, y, z, ts][x, y, z, ts]...

    u16 positions_count;
    f32* position_samples;

    // An array of interleaved rotation samples of `rotations_count` 
    // where the first four values of each entry 
    // are the X, Y, Z, and W quaternion rotation values and the last 
    // component is the timestamp. 
    //
    // [x, y, z, w, ts][x, y, z, w, ts][x, y, z, w, ts]...

    u16 rotations_count;
    f32* rotation_samples;

    // An array of interleaved scale samples of `scales_count` 
    // where the first three values of each entry 
    // are the X, Y, and Z, scale values and the last 
    // component is the timestamp. 
    //
    // [x, y, z, ts][x, y, z, ts][x, y, z, ts]...

    u16 scales_count;
    f32* scale_samples;
  };

  /// The optional name of the animation, with 
  /// `name_length` number of characters. 
  ///
  /// @NOTE: This is a C-string, missing the null-terminated character.

  u8 name_length = 0;
  char name[NBR_ANIMATION_NAME_MAX];

  /// An array of joints with `joints_count` elements. 

  u16 tracks_count      = 0;
  NBRJointTrack* tracks = nullptr;

  /// The total duration of the animation in frames. 
  f32 duration; 
};
/// NBRAnimation
///---------------------------------------------------------------------------------------------------------------------
  
///---------------------------------------------------------------------------------------------------------------------
/// NBRGlyph
struct NBRGlyph {
  /// The unicode representation of the character.
  i8 unicode; 

  /// The size of the glyph.
  u16 width, height; 

  /// The bounding box around the glyph.
  i16 left, right, top, bottom;

  /// An offset from the origin of the glyph. 
  /// Starts at the top-left of the glyph. 
  i16 offset_x, offset_y; 

  /// This value determines the start of the next glyph. 
  /// Usually, it starts at the origin of the current glyph 
  /// and continues till the origin of the next glyph.
  i16 advance_x;

  /// A small value that can be applied to make certain 
  /// characters appear better when next to each other. 
  i16 kern;

  /// Some left padding for certain characters.
  i16 left_bearing;
  
  /// The pixels that will be given to the texture to be 
  /// rendered later.
  u8* pixels; 
};
/// NBRGlyph
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBRFont
struct NBRFont {
  /// An array of glyphs with `glyphs_count` elements in this font.
  
  u32 glyphs_count;
  NBRGlyph* glyphs;

  /// This value is the top-most pixel of the first row. 
  i16 ascent;
  
  /// This value is the bottom-most pixel of the last row. 
  i16 descent;
  
  /// This value is used to determine the advance on the vertical axis.
  i16 line_gap;
};
/// NBRFont
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBRAudio
struct NBRAudio {
  /// The format, or rather, the bits per sample 
  /// of the audio samples. Can be any value from the 
  /// `AudioBufferFormat` enum.
  u8 format;

  /// The sample rate of the audio buffer. 
  u32 sample_rate; 

  /// The number of channels of the audio buffer. 
  u8 channels;

  /// The size in bytes of the `samples` array.
  u32 size; 

  /// The raw PCM data/samples of the audio buffer.
  i16* samples;
};
/// NBRAudio
///---------------------------------------------------------------------------------------------------------------------

/// ** NBR (Nikola Binary Resource) ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Resources ***

///---------------------------------------------------------------------------------------------------------------------
/// Resources consts

/// A value to indicate an invalid resource group.
const u16 RESOURCE_GROUP_INVALID = ((u16)-1);

/// The ID of the group associated with the resource cache.
const u16 RESOURCE_CACHE_ID      = 0;

/// The maximum amount of joints that can processed.
const sizei JOINTS_MAX           = 256;

/// Resources consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Macros 

/// Check if the given `res_id` is valid
#define RESOURCE_IS_VALID(res_id) (res_id.group != nikola::RESOURCE_GROUP_INVALID)

/// Macros 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ResourceGroupID 
typedef u16 ResourceGroupID;
/// ResourceGroupID 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ResourceType
enum ResourceType {
  /// Indicated an invalid resource.
  RESOURCE_TYPE_INVALID,

  /// A flag to denote a `GfxBuffer` resource.
  RESOURCE_TYPE_BUFFER, 

  /// A flag to denote a `GfxTexture` resource.
  RESOURCE_TYPE_TEXTURE, 
  
  /// A flag to denote a `GfxCubemap` resource.
  RESOURCE_TYPE_CUBEMAP,
  
  /// A flag to denote a `GfxShader` resource.
  RESOURCE_TYPE_SHADER,
  
  /// A flag to denote a `Mesh` resource.
  RESOURCE_TYPE_MESH,
  
  /// A flag to denote a `Material` resource.
  RESOURCE_TYPE_MATERIAL,
  
  /// A flag to denote a `Skybox` resource.
  RESOURCE_TYPE_SKYBOX,
  
  /// A flag to denote a `Model` resource.
  RESOURCE_TYPE_MODEL,
  
  /// A flag to denote a `Skeleton` resource.
  RESOURCE_TYPE_SKELETON,
  
  /// A flag to denote an `Animation` resource.
  RESOURCE_TYPE_ANIMATION,
  
  /// A flag to denote a `Font` resource.
  RESOURCE_TYPE_FONT,
  
  /// A flag to denote a `ShaderContext` resource.
  RESOURCE_TYPE_SHADER_CONTEXT,
  
  /// A flag to denote a `AudioBuffer` resource.
  RESOURCE_TYPE_AUDIO_BUFFER,

  /// The maximum number of resource types in this enum.
  RESOURCE_TYPES_MAX,
};
/// ResourceType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GeometryType
enum GeometryType {
  /// A predefined cube geometry shape.
  GEOMETRY_CUBE, 
  
  /// A predefined sphere geometry shape.
  GEOMETRY_SPHERE, 
  
  /// A predefined skybox geometry shape.
  GEOMETRY_SKYBOX, 
  
  /// A predefined 2D quad geometry shape.
  GEOMETRY_QUAD, 
  
  /// A much simpler cube shape (position, normal, and texture coordinates).
  GEOMETRY_SIMPLE_CUBE,
  
  /// A much simpler sphere shape (position, normal, and texture coordinates).
  GEOMETRY_SIMPLE_SPHERE,

  /// The maximum number of geometry types. 
  GEOMETRY_TYPES_MAX,
};
/// GeometryType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// MaterialTextureType
enum MaterialTextureType {
  /// Used to indicate an albedo texture in a `Material`.
  MATERIAL_TEXTURE_ALBEDO    = 7 << 0,
  
  /// Used to indicate a metallic texture in a `Material`.
  MATERIAL_TEXTURE_METALLIC  = 7 << 1,
  
  /// Used to indicate a roughness texture in a `Material`.
  MATERIAL_TEXTURE_ROUGHNESS = 7 << 2,
  
  /// Used to indicate an normal texture in a `Material`.
  MATERIAL_TEXTURE_NORMAL    = 7 << 3,
  
  /// Used to indicate an emissive texture in a `Material`.
  MATERIAL_TEXTURE_EMISSIVE  = 7 << 4,
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
  ResourceGroupID group = RESOURCE_GROUP_INVALID;
};
/// ResourceID
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBR functions

NIKOLA_API bool nbr_file_is_valid(File& file, const FilePath& path, const ResourceType res_type);

/// NBR functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Geometry functions

/// Fill the given `vertices` and `indices` arrays with the relavant data given the `type` geometry.
NIKOLA_API void geometry_loader_load(DynamicArray<f32>& vertices, DynamicArray<u32>& indices, const GeometryType type);

/// Fill the given vertex `layout` with attributes depending on `type`.
NIKOLA_API void geometry_loader_set_vertex_layout(GfxVertexLayout& layout, const GeometryType type);

/// Geometry functions
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
NIKOLA_API ResourceGroupID resources_create_group(const String& name, const FilePath& parent_dir);

/// Clear all of resources in `group_id`.
NIKOLA_API void resources_clear_group(const ResourceGroupID& group_id);

/// Clear and destroy all of resources in `group_id`.
NIKOLA_API void resources_destroy_group(const ResourceGroupID& group_id);

/// Allocate a new `GfxBuffer` using `buff_desc`, store it in `group_id`, and return a `ResourceID` 
/// to identify it.
NIKOLA_API ResourceID resources_push_buffer(const ResourceGroupID& group_id, const GfxBufferDesc& buff_desc);

/// Allocate a new `GfxTexture` using `desc`, store it in `group`,
/// and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_texture(const ResourceGroupID& group_id, const GfxTextureDesc& desc);

/// Allocate a new `GfxTexture` using the texture retrieved from the `nbr_path`, 
/// store it in `group_id`, and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_texture(const ResourceGroupID& group_id, const FilePath& nbr_path);

/// Allocate a new default `GfxTexture` of type `MaterialTextureType`,
/// store it in `group_id`, and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_texture(const ResourceGroupID& group_id, const MaterialTextureType& type);

/// Allocate a new `GfxCubemap` using `desc`, store it in `group_id`,
/// and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_cubemap(const ResourceGroupID& group_id, const GfxCubemapDesc& desc);

/// Allocate a new `GfxCubemap` using the cubemap retrieved from the `nbr_path`, 
/// store it in `group_id`, and return a `ResourceID` to identify it.
///
/// @NOTE: The number of faces of the cubemap will be determined inside the NBR file.
NIKOLA_API ResourceID resources_push_cubemap(const ResourceGroupID& group_id, const FilePath& nbr_path);

/// Allocate a new `GfxShader` using `shader_desc`, store it in `group_id`, and return a `ResourceID` 
/// to identify it.
NIKOLA_API ResourceID resources_push_shader(const ResourceGroupID& group_id, const GfxShaderDesc& shader_desc);

/// Allocate a new `GfxShader` using the shader retrieved from the `nbr_path`, 
/// store it in `group_id`, and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_shader(const ResourceGroupID& group_id, const FilePath& nbr_path);

/// Allocate a new `ShaderContext` using the previously-added shader `shader_id`, 
/// store it in `group_id`, and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_shader_context(const ResourceGroupID& group_id, const ResourceID& shader_id);

/// Allocate a new `ShaderContext` using the shader at `shader_path`, 
/// store it in `group_id`, and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_shader_context(const ResourceGroupID& group_id, const FilePath& shader_path);

/// Allocate a new `Mesh` using the given `nbr_mesh`,
/// store it in `group_id`, return a `ResourceID` to identified it. 
///
/// @NOTE: This function is usually meant for loading a mesh from an NBR format. 
/// Often it is used to load Models, for example.
NIKOLA_API ResourceID resources_push_mesh(const ResourceGroupID& group_id, NBRMesh& nbr_mesh);

/// Allocate a new `Mesh` using a predefined geometry `type`, 
/// store it in `group_id`, return a `ResourceID` to identified it. 
NIKOLA_API ResourceID resources_push_mesh(const ResourceGroupID& group_id, const GeometryType type);

/// Allocate a new `Material` store it in `group_id` using the information in the given `desc`, 
/// and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_material(const ResourceGroupID& group_id, const MaterialDesc& desc);

/// Allocate a new `Skybox` using the previously-added `cubemap_id`, store it in `group_id`, 
/// and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_skybox(const ResourceGroupID& group_id, const ResourceID& cubemap_id);

/// Allocate a new `Skybox` using the cubemap at `cubemap_path`, store it in `group_id`, 
/// and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_skybox(const ResourceGroupID& group_id, const FilePath& cubemap_path);

/// Allocate a new `Model` using the `NBRModel` retrieved from the `nbr_path`, 
/// store it in `group_id`, and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_model(const ResourceGroupID& group_id, const FilePath& nbr_path);

/// Allocate a new `Skeleton` using the `NBRSkeleton` retrieved from the `nbr_path`, and 
//  store it in `group_id`, and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_skeleton(const ResourceGroupID& group_id, const FilePath& nbr_path);

/// Allocate a new `Animation` using the `NBRAnimation` retrieved from the `nbr_path`, and 
//  store it in `group_id`, and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_animation(const ResourceGroupID& group_id, const FilePath& nbr_path);

/// Allocate a new `Font` using the `NBRFont` retrieved from the `nbr_path`, 
/// store it in `group_id`, and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_font(const ResourceGroupID& group_id, const FilePath& nbr_path);

/// Allocate a new `AudioBufferID` using the given `AudioBufferDesc` , 
/// store it in `group_id`, and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_audio_buffer(const ResourceGroupID& group_id, const AudioBufferDesc& desc);

/// Allocate a new `AudioBufferID` using the `NBRAudio` retrieved from the `nbr_path`, 
/// store it in `group_id`, and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_audio_buffer(const ResourceGroupID& group_id, const FilePath& nbr_path);

/// Retrieve all of the valid resources from `dir` and store the resulting entries in an
/// internal list (where the key is the file name of the resource and the value is its ID) 
/// while ensuring that each entry is pushed into `group_id` with an ID. The IDs can be retrieved 
/// later using the function `resources_get_id`. 
///
/// If the given `async` flag is set to `true`, the function will initiate a series of threads 
/// to retrieve each resource.
///
/// @NOTE: The given `dir` is prepended with the `parent_dir` given when `group_id` was created.
NIKOLA_API void resources_push_dir(const ResourceGroupID& group_id, const FilePath& dir, const bool async = false);

/// Search and retrieve the ID of the resource `filename` in `group_id`. 
/// If `filename` was not found in `group_id`, a default `ResourceID` will be returned. 
NIKOLA_API ResourceID& resources_get_id(const ResourceGroupID& group_id, const String& filename);

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

/// Retrieve `ShaderContext` identified by `id` in `id.group`. 
///
/// @NOTE: This function will assert if `id` is not found in `id.group`.
NIKOLA_API ShaderContext* resources_get_shader_context(const ResourceID& id);

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

/// Retrieve `Skeleton` identified by `id` in `id.group`. 
///
/// @NOTE: This function will assert if `id` is not found in `id.group`.
NIKOLA_API Skeleton* resources_get_skeleton(const ResourceID& id);

/// Retrieve `Animation` identified by `id` in `id.group`. 
///
/// @NOTE: This function will assert if `id` is not found in `id.group`.
NIKOLA_API Animation* resources_get_animation(const ResourceID& id);

/// Retrieve `Font` identified by `id` in `id.group`. 
///
/// @NOTE: This function will assert if `id` is not found in `id.group`.
NIKOLA_API Font* resources_get_font(const ResourceID& id);

/// Retrieve `AudioBufferID` identified by `id` in `id.group`. 
///
/// @NOTE: This function will assert if `id` is not found in `id.group`.
NIKOLA_API AudioBufferID resources_get_audio_buffer(const ResourceID& id);

/// Resource manager functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Resources ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
