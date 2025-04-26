#pragma once

#include "nikola_base.h"
#include "nikola_gfx.h"
#include "nikola_file.h"
#include "nikola_math.h"

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
const u16 RESOURCE_GROUP_INVALID         = ((u16)-1);

/// The ID of the group associated with the resource cache.
const u16 RESOURCE_CACHE_ID              = 0;

/// The maximum amount of declared uniform buffers in all shaders.
const sizei SHADER_UNIFORM_BUFFERS_MAX   = 1;

/// The index of the matrices uniform buffer within all shaders.
const sizei SHADER_MATRICES_BUFFER_INDEX = 0;

/// The maximum amount of preset uniforms. 
const u32 MATERIAL_UNIFORMS_MAX          = 6;

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
/// Macros 

/// Check if the given `res_id` is valid
#define RESOURCE_IS_VALID(id) (id.group != RESOURCE_GROUP_INVALID)

/// Macros 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ResourceType
enum ResourceType {
  /// A flag to denote a `GfxBuffer` resource
  RESOURCE_TYPE_BUFFER         = 16 << 0, 

  /// A flag to denote a `GfxTexture` resource
  RESOURCE_TYPE_TEXTURE        = 16 << 1, 
  
  /// A flag to denote a `GfxCubemap` resource
  RESOURCE_TYPE_CUBEMAP        = 16 << 2,
  
  /// A flag to denote a `GfxShader` resource
  RESOURCE_TYPE_SHADER         = 16 << 4,
  
  /// A flag to denote a `Mesh` resource
  RESOURCE_TYPE_MESH           = 16 << 5,
  
  /// A flag to denote a `Material` resource
  RESOURCE_TYPE_MATERIAL       = 16 << 6,
  
  /// A flag to denote a `Skybox` resource
  RESOURCE_TYPE_SKYBOX         = 16 << 7,
  
  /// A flag to denote a `Model` resource
  RESOURCE_TYPE_MODEL          = 16 << 8,
  
  /// A flag to denote a `Font` resource
  RESOURCE_TYPE_FONT           = 16 << 9,
  
  /// A flag to denote a `ShaderContext` resource
  RESOURCE_TYPE_SHADER_CONTEXT = 16 << 10,
};
/// ResourceType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// MeshType
enum MeshType {
  /// A predefined cube mesh
  MESH_TYPE_CUBE     = 17 << 0, 
  
  /// A predefined circle mesh
  MESH_TYPE_CIRCLE   = 17 << 1, 
  
  /// A predefined cylinder mesh
  MESH_TYPE_CYLINDER = 17 << 2, 
};
/// MeshType
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
  u16 group = RESOURCE_GROUP_INVALID;
};
/// ResourceID
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Mesh 
struct Mesh {
  ResourceID vertex_buffer = {}; 
  ResourceID index_buffer  = {};

  GfxPipeline* pipe         = nullptr;
  GfxPipelineDesc pipe_desc = {};
};
/// Mesh 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Material 
struct Material {
  ResourceID diffuse_map  = {};
  ResourceID specular_map = {};
  
  Vec3 ambient_color;
  Vec3 diffuse_color; 
  Vec3 specular_color;
  
  f32 shininess;
};
/// Material 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ShaderContext
struct ShaderContext {
  ResourceID shader = {}; 
  ResourceID uniform_buffers[SHADER_UNIFORM_BUFFERS_MAX];

  HashMap<String, i32> uniforms_cache;
};
/// ShaderContext
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Skybox
struct Skybox {
  ResourceID vertex_buffer = {};
  ResourceID cubemap       = {};
  
  GfxPipeline* pipe         = nullptr;
  GfxPipelineDesc pipe_desc = {};
};
/// Skybox
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Model 
struct Model {
  DynamicArray<ResourceID> meshes;
  DynamicArray<ResourceID> materials;
  DynamicArray<u8> material_indices;
};
/// Model 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Font 
struct Font {
  struct Glyph {
    i8 unicode; 
    ResourceID texture = {};

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
/// ShaderContext functions

/// Cache the location of the uniform with the name `uniform_name` to the given `ctx_id`.
/// 
/// @NOTE: If the uniform's name is not found within the context, the function will throw a warning. 
NIKOLA_API void shader_context_cache_uniform(ResourceID& ctx_id, const i8* uniform_name);

/// Set a uniform of type `i32` with the name `uniform_name` in `ctx_id` to the given `value`. 
NIKOLA_API void shader_context_set_uniform(ResourceID& ctx_id, const String& uniform_name, const i32 value);

/// Set a uniform of type `f32` with the name `uniform_name` in `ctx_id` to the given `value`. 
NIKOLA_API void shader_context_set_uniform(ResourceID& ctx_id, const String& uniform_name, const f32 value);

/// Set a uniform of type `Vec2` with the name `uniform_name` in `ctx_id` to the given `value`. 
NIKOLA_API void shader_context_set_uniform(ResourceID& ctx_id, const String& uniform_name, const Vec2& value);

/// Set a uniform of type `Vec3` with the name `uniform_name` in `ctx_id` to the given `value`. 
NIKOLA_API void shader_context_set_uniform(ResourceID& ctx_id, const String& uniform_name, const Vec3& value);

/// Set a uniform of type `Vec4` with the name `uniform_name` in `ctx_id` to the given `value`. 
NIKOLA_API void shader_context_set_uniform(ResourceID& ctx_id, const String& uniform_name, const Vec4& value);

/// Set a uniform of type `Mat4` with the name `uniform_name` in `ctx_id` to the given `value`. 
NIKOLA_API void shader_context_set_uniform(ResourceID& ctx_id, const String& uniform_name, const Mat4& value);

/// Set a uniform of type `Material` with the name `material_name` in `ctx_id` to the given `mat_id`. 
///
/// @NOTE: In order for this operation to succeed, the shader needs to include a `struct` with the same 
/// members as the engine's `Material`'s `struct`. 
NIKOLA_API void shader_context_set_uniform(ResourceID& ctx_id, const String& material_name, const ResourceID& mat_id);

/// Set the data of the uniform buffer at `index` of the associated shader in `ctx_id` to `buffer_id`
NIKOLA_API void shader_context_set_uniform_buffer(ResourceID& ctx_id, const sizei index, const ResourceID& buffer_id);

/// Set the shader with `shader_id` in `ctx_id`. 
/// 
/// @NOTE: If either `shader_id` or `ctx_id` are invalid, this function will assert. 
NIKOLA_API void shader_context_set_shader(ResourceID& ctx_id, const ResourceID& shader_id);

/// Use the shader currently binded to `ctx_id`. If the shader in `ctx_id` is invalid, 
/// the function will simply return and do nothing.
NIKOLA_API void shader_context_use(ResourceID& ctx_id);

/// ShaderContext functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Material functions

/// Set the texture of `type` in `mat_id`, using `texture_id`. 
/// 
/// @NOTE: If either `texture_id` or `mat_id` are invalid, this function will assert. 
NIKOLA_API void material_set_texture(ResourceID& mat_id, const MaterialTextureType type, const ResourceID& texture_id);

/// Use the textures that are currently valid in `mat_id`.
NIKOLA_API void material_use(ResourceID& mat_id);

/// Material functions
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
NIKOLA_API void nbr_import_mesh(NBRMesh* nbr, const u16 group_id, Mesh* mesh);

/// Convert the `nbr` material into a `Material`, using the `group_id`.
NIKOLA_API void nbr_import_material(NBRMaterial* nbr, const u16 group_id, Material* material);

/// Convert the `nbr` model into a `Model`, using the `group_id`.
NIKOLA_API void nbr_import_model(NBRModel* nbr, const u16 group_id, Model* model);

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

/// Allocate a new `ShaderContext` using the previously-added shader `shader_id`, 
/// store it in `group_id`, and return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_shader_context(const u16 group_id, const ResourceID& shader_id);

/// Allocate a new `Mesh` using the given `nbr_mesh`,
/// store it in `group_id`, return a `ResourceID` to identified it. 
///
/// @NOTE: This function is usually meant for loading a mesh from an NBR format. 
/// Often it is used to load Models, for example.
NIKOLA_API ResourceID resources_push_mesh(const u16 group_id, NBRMesh& nbr_mesh);

/// Allocate a new `Mesh` using a predefined mesh `type`, 
/// store it in `group_id`, return a `ResourceID` to identified it. 
NIKOLA_API ResourceID resources_push_mesh(const u16 group_id, const MeshType type);

/// Allocate a new `Material` store it in `group_id`, and 
/// return a `ResourceID` to identify it.
NIKOLA_API ResourceID resources_push_material(const u16 group_id);

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

/// Resource manager functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Resources ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
