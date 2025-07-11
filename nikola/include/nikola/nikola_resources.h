#pragma once

#include "nikola_gfx.h"
#include "nikola_file.h"
#include "nikola_math.h"
#include "nikola_audio.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

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
const i16 NBR_VALID_MINOR_VERSION = 3;

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
const u16 RESOURCE_GROUP_INVALID         = ((u16)-1);

/// The ID of the group associated with the resource cache.
const u16 RESOURCE_CACHE_ID              = 0;

/// The maximum amount of declared uniform buffers in all shaders.
const sizei SHADER_UNIFORM_BUFFERS_MAX   = 1;

/// The index of the matrices uniform buffer within all shaders.
const sizei SHADER_MATRICES_BUFFER_INDEX = 0;

/// The maximum amount of preset uniforms. 
const u32 MATERIAL_UNIFORMS_MAX          = 4;

/// The name of the color uniform in materials. 
#define MATERIAL_UNIFORM_COLOR        "u_material.color" 

/// The name of the shininess uniform in materials. 
#define MATERIAL_UNIFORM_SHININESS    "u_material.shininess" 

/// The name of the screen size uniform in materials. 
#define MATERIAL_UNIFORM_SCREEN_SIZE  "u_material.screen_size" 

/// The name of the model transform uniform in materials. 
#define MATERIAL_UNIFORM_MODEL_MATRIX "u_model" 

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
  RESOURCE_TYPE_INVALID        = 16 << -1,

  /// A flag to denote a `GfxBuffer` resource.
  RESOURCE_TYPE_BUFFER         = 16 << 0, 

  /// A flag to denote a `GfxTexture` resource.
  RESOURCE_TYPE_TEXTURE        = 16 << 1, 
  
  /// A flag to denote a `GfxCubemap` resource.
  RESOURCE_TYPE_CUBEMAP        = 16 << 2,
  
  /// A flag to denote a `GfxShader` resource.
  RESOURCE_TYPE_SHADER         = 16 << 4,
  
  /// A flag to denote a `Mesh` resource.
  RESOURCE_TYPE_MESH           = 16 << 5,
  
  /// A flag to denote a `Material` resource.
  RESOURCE_TYPE_MATERIAL       = 16 << 6,
  
  /// A flag to denote a `Skybox` resource.
  RESOURCE_TYPE_SKYBOX         = 16 << 7,
  
  /// A flag to denote a `Model` resource.
  RESOURCE_TYPE_MODEL          = 16 << 8,
  
  /// A flag to denote a `Font` resource.
  RESOURCE_TYPE_FONT           = 16 << 9,
  
  /// A flag to denote a `ShaderContext` resource.
  RESOURCE_TYPE_SHADER_CONTEXT = 16 << 10,
  
  /// A flag to denote a `AudioBuffer` resource.
  RESOURCE_TYPE_AUDIO_BUFFER   = 16 << 11,
};
/// ResourceType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GeometryType
enum GeometryType {
  /// A predefined cube geometry shape
  GEOMETRY_CUBE   = 17 << 0, 
  
  /// A predefined plane geometry shape
  GEOMETRY_PLANE  = 17 << 1, 
  
  /// A predefined skybox geometry shape
  GEOMETRY_SKYBOX = 17 << 2, 
  
  /// A predefined cube geometry shape
  GEOMETRY_CIRCLE = 17 << 3, 
};
/// GeometryType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// MaterialTextureType
enum MaterialTextureType {
  /// Indicate the diffuse texture in a `Material`.
  MATERIAL_TEXTURE_DIFFUSE  = 18 << 0,
  
  /// Indicate the specular texture in a `Material`.
  MATERIAL_TEXTURE_SPECULAR = 18 << 1,
  
  /// Indicate the normal texture in a `Material`.
  MATERIAL_TEXTURE_NORMAL   = 18 << 2,
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
  /// Texture maps.

  GfxTexture* diffuse_map  = nullptr;
  GfxTexture* specular_map = nullptr;
 
  /// Useful surface-defining variables.

  Vec3 color       = Vec3(1.0f);
  f32 shininess    = 1.0f;
  f32 transparency = 1.0f;
  
  /// A bitwise flag, detemnining which texture 
  /// maps to use in the shader.
  i32 map_flags = 0;
};
/// Material 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ShaderContext
struct ShaderContext {
  GfxShader* shader = nullptr; 
  GfxBuffer* uniform_buffers[SHADER_UNIFORM_BUFFERS_MAX];

  HashMap<String, i32> uniforms_cache;
};
/// ShaderContext
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Skybox
struct Skybox {
  GfxCubemap* cubemap       = nullptr;
  
  GfxPipelineDesc pipe_desc = {};
  GfxPipeline* pipe         = nullptr;
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
  ResourceID diffuse_id  = {};
  ResourceID specular_id = {};

  Vec3 color = Vec3(1.0f); 

  f32 shininess    = 0.1f; 
  f32 transparency = 1.0f;
};
/// MaterialDesc
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

/// Set the data of the uniform buffer at `index` of the associated shader in `ctx` to `buffer`
NIKOLA_API void shader_context_set_uniform_buffer(ShaderContext* ctx, const sizei index, const GfxBuffer* buffer);

/// Use the shader currently binded to `ctx`. If the shader in `ctx` is invalid, 
/// the function will simply return and do nothing.
NIKOLA_API void shader_context_use(ShaderContext* ctx_id);

/// ShaderContext functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Material functions

/// Use the textures that are currently valid in `mat`.
NIKOLA_API void material_use(Material* mat);

/// Material functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Geometry functions

/// Fill the given `pipe_desc` structure with geometry data based on the give `type`. 
NIKOLA_API void geometry_loader_load(const ResourceGroupID& group_id, GfxPipelineDesc* pipe_desc, const GeometryType type);

/// Geometry functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBR importer functions

/// Convert the `nbr` texture into a `GfxTextureDesc`.
NIKOLA_API void nbr_import_texture(NBRTexture* nbr, GfxTextureDesc* desc);

/// Convert the `nbr` cubemap into a `GfxCubemapDesc`.
NIKOLA_API void nbr_import_cubemap(NBRCubemap* nbr, GfxCubemapDesc* desc);

/// Convert the `nbr` shader into a `GfxShaderDesc`.
NIKOLA_API void nbr_import_shader(NBRShader* nbr, GfxShaderDesc* desc);

/// Convert the `nbr` mesh into a `Mesh`, using the `group_id`.
NIKOLA_API void nbr_import_mesh(NBRMesh* nbr, const ResourceGroupID& group_id, Mesh* mesh);

/// Convert the `nbr` model into a `Model`, using the `group_id`.
NIKOLA_API void nbr_import_model(NBRModel* nbr, const ResourceGroupID&, Model* model);

/// Convert the `nbr` font into a `Font`, using the `group_id`.
NIKOLA_API void nbr_import_font(NBRFont* nbr, const ResourceGroupID& group_id, Font* font);

/// Convert the `nbr` audio into an `AudioBuffer`, using the `group_id`.
NIKOLA_API void nbr_import_audio(NBRAudio* nbr, const ResourceGroupID& group_id, AudioBufferDesc* desc);

/// NBR importer functions
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
///
/// Default values: 
///   - `format` = `GFX_TEXTURE_FORMAT_RGBA8`.
///   - `filter` = `GFX_TEXTURE_FILTER_MIN_MAG_NEAREST`.
///   - `wrap`   = `GFX_TEXTURE_WRAP_CLAMP`.
NIKOLA_API ResourceID resources_push_texture(const ResourceGroupID& group_id, 
                                             const FilePath& nbr_path,
                                             const GfxTextureFormat format = GFX_TEXTURE_FORMAT_RGBA8, 
                                             const GfxTextureFilter filter = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
                                             const GfxTextureWrap wrap     = GFX_TEXTURE_WRAP_CLAMP);

/// Allocate a new `GfxCubemap` using `desc`, store it in `group_id`,
/// and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_cubemap(const ResourceGroupID& group_id, const GfxCubemapDesc& desc);

/// Allocate a new `GfxCubemap` using the cubemap retrieved from the `nbr_path`, 
/// store it in `group_id`, and return a `ResourceID` to identify it.
///
/// Default values: 
///   - `format` = `GFX_TEXTURE_FORMAT_RGBA8`.
///   - `filter` = `GFX_TEXTURE_FILTER_MIN_MAG_NEAREST`.
///   - `wrap`   = `GFX_TEXTURE_WRAP_CLAMP`.
///
/// @NOTE: The number of faces of the cubemap will be determined inside the NBR file.
NIKOLA_API ResourceID resources_push_cubemap(const ResourceGroupID& group_id, 
                                             const FilePath& nbr_path,
                                             const GfxTextureFormat format = GFX_TEXTURE_FORMAT_RGBA8, 
                                             const GfxTextureFilter filter = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
                                             const GfxTextureWrap wrap     = GFX_TEXTURE_WRAP_CLAMP);

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
/// @NOTE: The given `dir` is prepended with the `parent_dir` given when `group_id` was created.
NIKOLA_API void resources_push_dir(const ResourceGroupID& group_id, const FilePath& dir);

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
