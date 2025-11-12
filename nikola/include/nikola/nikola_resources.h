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

/// ----------------------------------------------------------------------
/// ** NBR (Nikola Binary Resource) ***

///---------------------------------------------------------------------------------------------------------------------
/// NBR consts

/// A value present at the top of each `.nbr` file to denote 
/// a valid `.nbr` file. 
///
/// @NOTE: The value is the average of the ASCII hex codes of `n`, `b`, and `r`.
const u8 NBR_VALID_IDENTIFIER     = 107;

/// The currently valid major version of any `.nbr` file
const i16 NBR_VALID_MAJOR_VERSION = 0;

/// The currently valid minor version of any `.nbr` file
const i16 NBR_VALID_MINOR_VERSION = 5;

/// The maximum number of weights a joint can have in an NBR file. 
const u8 NBR_JOINT_WEIGHTS_MAX    = 4;

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
  u8 material_index = 0;
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

  /// An array of all the possible textures.
  NBRTexture* textures;
};
/// NBRModel 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBRJoint
struct NBRJoint {
  /// The index of this joint's parent that can be 
  /// queried later by the renderer. 
  ///
  /// @NOTE: If this is the root joint, the parent index 
  /// will be `-1`.
  i16 parent_index = -1; 

  /// A 4x3 matrix defining the inverse bind pose of this 
  /// joint that transforms to the joint's local coordinates.
  f32 inverse_bind_pose[12];

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
/// NBRJoint
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBRAnimation
struct NBRAnimation {
  /// An array of joints with `joints_count` elements. 

  u16 joints_count;
  NBRJoint* joints;

  /// The total duration of the animation in frames. 
  f32 duration; 

  /// The total frame rate of each tick of the animation. 
  /// The duration value of the animation can be divided by 
  /// the frame rate to get the total duration of the animation  
  /// in seconds.
  f32 frame_rate;
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
  /// The amount of glyphs found in the `glyphs` array.
  u32 glyphs_count;

  /// An array of all the glyphs in this font.
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
  
  /// A predefined billboard geometry shape.
  GEOMETRY_BILLBOARD, 
  
  /// A much simpler cube shape for debug purposes.
  GEOMETRY_DEBUG_CUBE,
  
  /// A much simpler sphere shape for debug purposes.
  GEOMETRY_DEBUG_SPHERE,

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
/// Mesh 
struct Mesh {
  /// The vertices data of this mesh.  
  DynamicArray<f32> vertices; 
  
  /// The indices data of this mesh.  
  DynamicArray<u32> indices; 

  /// The index of the material to be used 
  /// with this mesh. 
  ///
  /// @NOTE: This is `0` by default, representing 
  /// the default material.
  sizei material_index = 0;
};
/// Mesh 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Material 
struct Material {
  /// Texture maps.

  GfxTexture* albedo_map    = nullptr;
  GfxTexture* metallic_map  = nullptr;
  GfxTexture* roughness_map = nullptr;
  GfxTexture* normal_map    = nullptr;
  GfxTexture* emissive_map  = nullptr;
 
  /// Useful surface-defining flags.

  Vec3 color       = Vec3(1.0f);
  f32 roughness    = 1.0f;
  f32 metallic     = 0.0f;
  f32 emissive     = 0.0f;
  f32 transparency = 1.0f;
 
  /// Pipeline-related flags 

  Vec4 blend_factor = Vec4(0.0f);
  bool depth_mask   = true;
  i32 stencil_ref   = 1;

  /// A bitwise flag, detemnining which texture 
  /// maps to use in the shader.
  i32 map_flags = 0;
};
/// Material 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ShaderContext
struct ShaderContext {
  /// The underlying shader pointer of the context.
  GfxShader* shader = nullptr; 

  /// A cache of uniforms where the key is the name of 
  /// the uniform in the shader and the value is the 
  /// uniform's location in the shader.
  HashMap<String, i32> uniforms_cache;
};
/// ShaderContext
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Skybox
struct Skybox {
  /// The underlying cubemap pointer of the skybox
  GfxCubemap* cubemap = nullptr;
 
  /// The vertices of this skybox.
  DynamicArray<f32> vertices;
};
/// Skybox
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Model 
struct Model {
  /// All of the raw meshes of the model.
  DynamicArray<Mesh*> meshes;

  /// All of the materials of the model.
  DynamicArray<Material*> materials;
};
/// Model 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Joint 
struct Joint {
  /// The parent index to refer to the joint's parent. 
  ///
  /// @NOTE: If the joint is the root of the skeleton, 
  /// the parent index will be `-1`.
  i32 parent_index = -1; 

  /// The inverse bind pose matrix of the joint to 
  /// transform the joint to its local coordinates 
  /// (sometimes known as "bone space").
  Mat4 inverse_bind_pose;

  /// A collection of arrays of the transform samples of the mesh

  DynamicArray<VectorAnimSample> position_samples;
  DynamicArray<QuatAnimSample> rotation_samples;
  DynamicArray<VectorAnimSample> scale_samples;

  /// An index for each of the samples array 
  /// referring to the current sample being processed.

  sizei current_position_sample = 0;
  sizei current_rotation_sample = 0;
  sizei current_scale_sample    = 0;

  /// The current calculated transform of the joint. 
  /// This trasform will later be taken and transformed 
  /// into bone space and then sent to the shader.
  Transform current_transform;
};
/// Joint 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Animation
struct Animation {
  /// A list of all the available joints of the animation.
  DynamicArray<Joint*> joints; 

  /// The skinned model that will be used for the animation.
  Model* skinned_model; 

  /// Timing-related information of the animation 
  ///
  /// @NOTE: Refer to `NBRAnimation` for more information 
  /// about each of the values.
  
  f32 duration   = 0.0f; 
  f32 frame_rate = 0.0f;

  /// The final output matrix of the animation with all 
  /// of the parent/child transformations applied as well as the 
  /// inverse bind matrix of each joint. 
  /// 
  /// This is essentially ready to be sent to the shader.
  Mat4 skinning_palette[JOINTS_MAX];
};
/// Animation
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Glyph
struct Glyph {
  i8 unicode; 
  GfxTexture* texture = nullptr;

  Vec2 size;
  Vec2 offset;

  u32 left, right, top, bottom;
  i32 advance_x, kern, left_bearing;
};
/// Glyph
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Font 
struct Font {
  f32 ascent, descent, line_gap;
  HashMap<i8, Glyph> glyphs;
};
/// Font 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// MaterialDesc
struct MaterialDesc {
  ResourceID albedo_id    = {};
  ResourceID roughness_id = {};
  ResourceID metallic_id  = {};
  ResourceID normal_id    = {};
  ResourceID emissive_id  = {};

  Vec3 color = Vec3(1.0f); 

  f32 roughness    = 1.0f; 
  f32 metallic     = 0.0f; 
  f32 emissive     = 0.0f; 
  f32 transparency = 1.0f;

  bool depth_mask  = true;
  u32 stencil_ref  = 0xFF;
};
/// MaterialDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBR functions

NIKOLA_API bool nbr_file_is_valid(File& file, const FilePath& path, const ResourceType res_type);

/// NBR functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ShaderContext functions

/// Cache the location of the uniform with the name `uniform_name` to the given `ctx`.
/// 
/// @NOTE: If the uniform's name is not found within the context, the function will throw a warning. 
NIKOLA_API void shader_context_cache_uniform(ShaderContext* ctx, const i8* uniform_name);

/// Set a uniform of type `i32` with the name `uniform_name` in `ctx` to the given `value`. 
NIKOLA_API void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const i32 value);

/// Set a uniform of type `f32` with the name `uniform_name` in `ctx` to the given `value`. 
NIKOLA_API void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const f32 value);

/// Set a uniform of type `Vec2` with the name `uniform_name` in `ctx` to the given `value`. 
NIKOLA_API void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const Vec2& value);

/// Set a uniform of type `Vec3` with the name `uniform_name` in `ctx` to the given `value`. 
NIKOLA_API void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const Vec3& value);

/// Set a uniform of type `Vec4` with the name `uniform_name` in `ctx` to the given `value`. 
NIKOLA_API void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const Vec4& value);

/// Set a uniform of type `Mat4` with the name `uniform_name` in `ctx` to the given `value`. 
NIKOLA_API void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const Mat4& value);

/// Set a uniform of type `Material` with the name `uniform_name` in `ctx` to the given `value`. 
NIKOLA_API void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const Material* value);

/// Set a uniform of type `PointLight` with the name `uniform_name` in `ctx` to the given `value`. 
NIKOLA_API void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const PointLight& value);

/// Set a uniform of type `DirectionalLight` with the name `uniform_name` in `ctx` to the given `value`. 
NIKOLA_API void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const DirectionalLight& value);

/// Set a uniform of type `SpotLight` with the name `uniform_name` in `ctx` to the given `value`. 
NIKOLA_API void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const SpotLight& value);

/// ShaderContext functions
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
