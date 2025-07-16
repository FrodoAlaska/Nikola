#include "nikola/nikola_resources.h"
#include "nikola/nikola_event.h"
#include "nikola/nikola_render.h"
#include "nikola/nikola_file.h"

#include <cstring>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// ResourceGroup 
struct ResourceGroup {
  String name; 
  FilePath parent_dir;
  ResourceGroupID id;

  DynamicArray<GfxBuffer*> buffers;
  DynamicArray<GfxTexture*> textures;
  DynamicArray<GfxCubemap*> cubemaps;
  DynamicArray<GfxShader*> shaders;
  DynamicArray<AudioBufferID> audio_buffers;
  
  DynamicArray<Mesh*> meshes;
  DynamicArray<Material*> materials;
  DynamicArray<ShaderContext*> shader_contexts;
  DynamicArray<Skybox*> skyboxes;
  DynamicArray<Model*> models;
  DynamicArray<Font*> fonts;

  HashMap<String, ResourceID> named_ids;
};
/// ResourceGroup 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ResourceManager 
struct ResourceManager {
  GfxContext* gfx_context = nullptr;
  HashMap<ResourceGroupID, ResourceGroup> groups;
};

static ResourceManager s_manager;
/// ResourceManager 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Macros (Unfortunately)

#define DESTROY_CORE_RESOURCE_MAP(group, map, clear_func) { \
  for(auto& res : group->map) {                             \
    clear_func(res);                                        \
  }                                                         \
}

#define DESTROY_COMP_RESOURCE_MAP(group, map) { \
  for(auto& res : group->map) {                 \
    delete res;                                 \
  }                                             \
}

#define PUSH_RESOURCE(group, resources, res, type, res_id) { \
  group->resources.push_back(res);                           \
  res_id._type = type;                                       \
  res_id._id   = (u16)group->resources.size() - 1;           \
  res_id.group = group->id;                                  \
}

#define GROUP_CHECK(group_id) NIKOLA_ASSERT((group_id != RESOURCE_GROUP_INVALID), "Cannot push a resource to an invalid group")

/// Macros (Unfortunately)
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

static bool open_and_check_nbr_file(const FilePath& parent_dir, const FilePath& nbr_path, File* file, NBRHeader* header) {
  FilePath path = filepath_append(parent_dir, nbr_path);

  // Open the NBR file 
  if(!file_open(file, path, (i32)(FILE_OPEN_READ | FILE_OPEN_BINARY))) {
    NIKOLA_LOG_ERROR("Cannot load NBR file at \'%s\'", path.c_str());
    return false;
  }

  // Read the header
  file_read_bytes(*file, header);

  // Check the validity of the reosurce type
  NIKOLA_ASSERT((header->resource_type != RESOURCE_TYPE_INVALID), 
                "Invalid resource type found in NBR file!");
  
  // Check for the validity of the identifier
  if(header->identifier != NBR_VALID_IDENTIFIER) {
    NIKOLA_LOG_ERROR("Invalid identifier found in NBR file at \'%s\'. Expected \'%i\' got \'%i\'", 
                      path.c_str(), NBR_VALID_IDENTIFIER, header->identifier);

    return false;
  }  

  // Check for the validity of the versions
  bool is_valid_version = ((header->major_version == NBR_VALID_MAJOR_VERSION) || 
                           (header->minor_version == NBR_VALID_MINOR_VERSION));
  if(!is_valid_version) {
    NIKOLA_LOG_ERROR("Invalid version found in NBR file at \'%s\'", path.c_str());
    return false;
  }

  return true;
}

static const char* buffer_type_str(const GfxBufferType type) {
  switch(type) {
    case GFX_BUFFER_VERTEX: 
      return "GFX_BUFFER_VERTEX";
    case GFX_BUFFER_INDEX: 
      return "GFX_BUFFER_INDEX";
    case GFX_BUFFER_UNIFORM: 
      return "GFX_BUFFER_UNIFORM";
    case GFX_BUFFER_SHADER_STORAGE: 
      return "GFX_BUFFER_SHADER_STORAGE";
    default:
      return "INVALID BUFFER TYPE";
  }
}

static const char* texture_type_str(const GfxTextureType type) {
  switch(type) {
    case GFX_TEXTURE_1D:
      return "GFX_TEXTURE_1D";
    case GFX_TEXTURE_2D:
      return "GFX_TEXTURE_2D";
    case GFX_TEXTURE_3D:
      return "GFX_TEXTURE_3D";
    case GFX_TEXTURE_DEPTH_STENCIL_TARGET:
      return "GFX_TEXTURE_DEPTH_STENCIL_TARGET";
    default:
      return "INVALID TEXTURE TYPE";
  }
}

static const char* geo_type_str(const GeometryType type) {
  switch(type) {
    case GEOMETRY_CUBE:
      return "GEOMETRY_CUBE";
    case GEOMETRY_PLANE:
      return "GEOMETRY_PLANE";
    case GEOMETRY_SKYBOX:
      return "GEOMETRY_SKYBOX";
    case GEOMETRY_CIRCLE:
      return "GEOMETRY_CIRCLE";
    default:
      return "INVALID GEOMETRY TYPE";
  }
}

static const char* audio_format_str(const AudioBufferFormat format) {
  switch(format) {
    case AUDIO_BUFFER_FORMAT_U8:
      return "AUDIO_BUFFER_FORMAT_U8";
    case AUDIO_BUFFER_FORMAT_I16:
      return "AUDIO_BUFFER_FORMAT_I16";
    case AUDIO_BUFFER_FORMAT_F32:
      return "AUDIO_BUFFER_FORMAT_F32";
    default:
      return "INVALID AUDIO BUFFER FORMAT";
  }
}

template<typename T> 
static T get_resource(const ResourceID& id, DynamicArray<T>& res, const ResourceType type) {
  NIKOLA_ASSERT((id._type == type), "Invalid type when trying to retrieve a resource");
  NIKOLA_ASSERT((id._id >= 0 && id._id <= (u16)res.size()), "Invalid ID when trying to retrieve a resource");
  NIKOLA_ASSERT(RESOURCE_IS_VALID(id), "Cannot retrieve a resource from an invalid group");

  return res[id._id];
}

static void reload_texture(const ResourceID& id, File& file) {
  GfxTexture* texture = resources_get_texture(id);  

  // Read the NBR texture
  NBRTexture nbr_texture; 
  file_read_bytes(file, &nbr_texture);

  // Update the texture desc

  GfxTextureDesc text_desc = gfx_texture_get_desc(texture);
  
  text_desc.width    = nbr_texture.width; 
  text_desc.height   = nbr_texture.height; 
  text_desc.data     = nbr_texture.pixels;

  // Update the texture
  gfx_texture_upload_data(texture, 
                          text_desc.width, text_desc.height, 
                          text_desc.depth, text_desc.data);
}

static void reload_cubemap(const ResourceID& id, File& file) {
  GfxCubemap* cubemap = resources_get_cubemap(id);  
  
  // Read the NBR cubemap
  NBRCubemap nbr_cubemap; 
  file_read_bytes(file, &nbr_cubemap);

  // Update the cubemap desc

  GfxCubemapDesc cube_desc = gfx_cubemap_get_desc(cubemap);
  
  cube_desc.width       = nbr_cubemap.width; 
  cube_desc.height      = nbr_cubemap.height; 
  cube_desc.faces_count = nbr_cubemap.faces_count; 
  
  for(sizei i = 0; i < cube_desc.faces_count; i++) {
    cube_desc.data[i] = nbr_cubemap.pixels[i];
  }
 
  /* @NOTE (19/4/2025, Mohamed):
   *
   * Now, listen, is this ugly? Yes. Is it dangerous? Possibly, yes. 
   * But does it work, though? Yes! And that's the most important thing. 
   * It's not my fault that Bjarne decided that `void*[6]` does not 
   * equal `const void**`. Take it up with him. Don't blame me.
   *  
   */

  // Update the cubemap
  gfx_cubemap_upload_data(cubemap, 
                          cube_desc.width, cube_desc.height, 
                          (const void**)cube_desc.data, cube_desc.faces_count);
}

static void reload_shader(const ResourceID& id, File& file) {
  GfxShader* shader = resources_get_shader(id);  
  
  // Load the NBR shader
  NBRShader nbr_shader;
  file_read_bytes(file, &nbr_shader);

  // Update the shader desc
  GfxShaderDesc shader_desc = {};
  shader_desc.vertex_source = nbr_shader.vertex_source;
  shader_desc.pixel_source  = nbr_shader.pixel_source;
  
  // Update the shader
  gfx_shader_update(shader, shader_desc);
}

/// Private functions 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

static void resource_entry_iterate(const FilePath& base, const FilePath& path, void* user_data) {
  ResourceGroup* group = (ResourceGroup*)user_data;

  if(!filesystem_exists(path)) {
    NIKOLA_LOG_ERROR("Cannot push non-existent resource at \'%s\'", path.c_str());
    return;
  }
  
  // Load the NBR header
  NBRHeader header;
  File file;
  if(!open_and_check_nbr_file(base, filepath_filename(path), &file, &header)) {
    return;
  }
  file_close(file);

  switch (header.resource_type) {
    case RESOURCE_TYPE_TEXTURE:
      resources_push_texture(group->id, path);
      break;
    case RESOURCE_TYPE_CUBEMAP:
      resources_push_cubemap(group->id, path);
      break;
    case RESOURCE_TYPE_SHADER:
      resources_push_shader(group->id, path);
      break;
    case RESOURCE_TYPE_MODEL:
      resources_push_model(group->id, path);
      break;
    case RESOURCE_TYPE_FONT:
      resources_push_font(group->id, path);
      break;
    case RESOURCE_TYPE_AUDIO_BUFFER:
      resources_push_audio_buffer(group->id, path);
      break;
    default:
      NIKOLA_LOG_ERROR("Invalid resource type \'%s\'", path.c_str());
      break;
  }
}

static void resource_entry_update(const FileStatus status, const FilePath& path, void* user_data) {
  // We only care if the resource was modified 
  if(status != FILE_STATUS_MODIFIED) {
    return;
  }
  
  // Load the NBR header
  NBRHeader header;
  File file;
  if(!open_and_check_nbr_file(path, "", &file, &header)) {
    return;
  }
 
  // Get the filename without the extension
  FilePath filename = filepath_filename(path); 
  filepath_set_extension(filename, "");

  ResourceGroup* group = (ResourceGroup*)user_data;
  ResourceID res_id    = resources_get_id(group->id, filename);

  switch (header.resource_type) {
    case RESOURCE_TYPE_TEXTURE:
      reload_texture(res_id, file);
      break;
    case RESOURCE_TYPE_CUBEMAP:
      reload_cubemap(res_id, file);
      break;
    case RESOURCE_TYPE_SHADER:
      reload_shader(res_id, file);
      break;
    case RESOURCE_TYPE_MODEL:
      // @TODO (Resource)
      break;
    case RESOURCE_TYPE_FONT:
      // @TODO (Resource)
      break;
    case RESOURCE_TYPE_AUDIO_BUFFER:
      // @TODO (Resource)
      break;
    default:
      NIKOLA_LOG_ERROR("Unsupported resource type for reloading");
      break;
  }

  file_close(file);
}

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Resource manager functions

void resource_manager_init() {
  s_manager.groups[RESOURCE_CACHE_ID] = ResourceGroup {
    .name       = "cache", 
    .parent_dir = "resource_cache",
    .id         = RESOURCE_CACHE_ID,
  };

  NIKOLA_LOG_INFO("Successfully initialized the resource manager");
}

void resource_manager_shutdown() {
  // Get rid of any cache group
  resources_destroy_group(RESOURCE_CACHE_ID);
  
  NIKOLA_LOG_INFO("Successfully shutdown the resource manager");
}

u16 resources_create_group(const String& name, const FilePath& parent_dir) {
  ResourceGroupID group_id   = random_u32(RESOURCE_CACHE_ID + 1, RESOURCE_GROUP_INVALID - 1); 
  s_manager.groups[group_id] = ResourceGroup {
    .name       = name, 
    .parent_dir = parent_dir,
    .id         = group_id,
  };

  // Create the parent directory if it doesn't exist
  if(!nikola::filesystem_exists(parent_dir)) {
    nikola::filesystem_create_directory(parent_dir);
  }

  // Add a file watcher to the parent directory
  filewatcher_add_dir(parent_dir, resource_entry_update, &s_manager.groups[group_id]);

  NIKOLA_LOG_INFO("Successfully created a resource group \'%s\'", name.c_str());
  return group_id;
}

void resources_clear_group(const ResourceGroupID& group_id) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  group->buffers.clear();
  group->textures.clear();
  group->cubemaps.clear();
  group->shaders.clear();
  group->audio_buffers.clear();

  group->meshes.clear();
  group->materials.clear();
  group->shader_contexts.clear();
  group->skyboxes.clear();
  group->models.clear();
  group->fonts.clear();
  
  NIKOLA_LOG_INFO("Resource group \'%s\' was successfully cleared", group->name.c_str());
}

void resources_destroy_group(const ResourceGroupID& group_id) {
  if(group_id == RESOURCE_GROUP_INVALID) {
    NIKOLA_LOG_WARN("Cannot destroy an invalid resource group");
    return;
  }

  ResourceGroup* group = &s_manager.groups[group_id];

  // Destroy compound resources
  DESTROY_COMP_RESOURCE_MAP(group, meshes);
  DESTROY_COMP_RESOURCE_MAP(group, materials);
  DESTROY_COMP_RESOURCE_MAP(group, shader_contexts);
  DESTROY_COMP_RESOURCE_MAP(group, skyboxes);
  DESTROY_COMP_RESOURCE_MAP(group, models);
  DESTROY_COMP_RESOURCE_MAP(group, fonts);

  // Destroy core resources
  DESTROY_CORE_RESOURCE_MAP(group, buffers, gfx_buffer_destroy);
  DESTROY_CORE_RESOURCE_MAP(group, textures, gfx_texture_destroy);
  DESTROY_CORE_RESOURCE_MAP(group, cubemaps, gfx_cubemap_destroy);
  DESTROY_CORE_RESOURCE_MAP(group, shaders, gfx_shader_destroy);
  DESTROY_CORE_RESOURCE_MAP(group, audio_buffers, audio_buffer_destroy);

  NIKOLA_LOG_INFO("Resource group \'%s\' was successfully destroyed", group->name.c_str());
  s_manager.groups.erase(group_id);
}

ResourceID resources_push_buffer(const ResourceGroupID& group_id, const GfxBufferDesc& buff_desc) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Create the buffer
  ResourceID id; 
  GfxBuffer* buffer = gfx_buffer_create(renderer_get_context(), buff_desc);
  PUSH_RESOURCE(group, buffers, buffer, RESOURCE_TYPE_BUFFER, id);

  NIKOLA_LOG_DEBUG("Group \'%s\' pushed buffer:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Size = %zu", buff_desc.size);
  NIKOLA_LOG_DEBUG("     Type = %s", buffer_type_str(buff_desc.type));
  return id;
}

ResourceID resources_push_texture(const ResourceGroupID& group_id, const GfxTextureDesc& desc) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Create the texture
  ResourceID id; 
  GfxTexture* texture = gfx_texture_create(renderer_get_context(), desc);
  PUSH_RESOURCE(group, textures, texture, RESOURCE_TYPE_TEXTURE, id);

  NIKOLA_LOG_DEBUG("Group \'%s\' pushed texture:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Size = %i X %i", desc.width, desc.height);
  NIKOLA_LOG_DEBUG("     Type = %s", texture_type_str(desc.type));
  return id;
}

ResourceID resources_push_texture(const ResourceGroupID& group_id, 
                                  const FilePath& nbr_path,
                                  const GfxTextureFormat format, 
                                  const GfxTextureFilter filter, 
                                  const GfxTextureWrap wrap) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
 
  // Load the NBR file
  NBRHeader header;
  File file;
  if(!open_and_check_nbr_file(group->parent_dir, nbr_path, &file, &header)) {
    return ResourceID{};
  }

  // Make sure it is the correct resource type
  NIKOLA_ASSERT((header.resource_type == RESOURCE_TYPE_TEXTURE), "Expected RESOURCE_TYPE_TEXTURE");

  // Load the NBR texture type
  NBRTexture nbr_texture;
  file_read_bytes(file, &nbr_texture);

  // Convert the NBR format to a valid texture
  
  GfxTextureDesc tex_desc; 
  tex_desc.format    = format; 
  tex_desc.filter    = filter; 
  tex_desc.wrap_mode = wrap;
  tex_desc.width    = nbr_texture.width; 
  tex_desc.height   = nbr_texture.height; 
  tex_desc.depth    = 0; 
  tex_desc.mips     = 1; 
  tex_desc.type     = GFX_TEXTURE_2D; 
  tex_desc.data     = nbr_texture.pixels;

  // Create the texture 
  ResourceID id = resources_push_texture(group_id, tex_desc); 

  // Freeing NBR data
  memory_free(nbr_texture.pixels);
  file_close(file); 
  
  // Add the resource to the named resources
  
  FilePath filename_without_ext = filepath_filename(nbr_path);
  filepath_set_extension(filename_without_ext, "");
  group->named_ids[filename_without_ext] = id;

  // New texture added!
  NIKOLA_LOG_DEBUG("     Path = %s", nbr_path.c_str());
  return id;
}

ResourceID resources_push_cubemap(const ResourceGroupID& group_id, const GfxCubemapDesc& cubemap_desc) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Create the cubemap
  ResourceID id; 
  GfxCubemap* cubemap = gfx_cubemap_create(renderer_get_context(), cubemap_desc);
  PUSH_RESOURCE(group, cubemaps, cubemap, RESOURCE_TYPE_CUBEMAP, id);
  
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed cubemap:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Size  = %i X %i", cubemap_desc.width, cubemap_desc.height);
  NIKOLA_LOG_DEBUG("     Faces = %i", cubemap_desc.faces_count);
  return id;
}

ResourceID resources_push_cubemap(const ResourceGroupID& group_id, 
                                  const FilePath& nbr_path,
                                  const GfxTextureFormat format, 
                                  const GfxTextureFilter filter, 
                                  const GfxTextureWrap wrap) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Load the NBR file
  NBRHeader header;
  File file;
  if(!open_and_check_nbr_file(group->parent_dir, nbr_path, &file, &header)) {
    return ResourceID{};
  }

  // Make sure it is the correct resource type
  NIKOLA_ASSERT((header.resource_type == RESOURCE_TYPE_CUBEMAP), "Expected RESOURCE_TYPE_CUBEMAP");

  // Load the NBR cubemap
  NBRCubemap nbr_cubemap;
  file_read_bytes(file, &nbr_cubemap);

  // Convert the NBR format to a valid cubemap
  
  GfxCubemapDesc cube_desc; 
  cube_desc.format      = format; 
  cube_desc.filter      = filter; 
  cube_desc.wrap_mode   = wrap;
  cube_desc.width       = nbr_cubemap.width; 
  cube_desc.height      = nbr_cubemap.height; 
  cube_desc.mips        = 1; 
  cube_desc.faces_count = nbr_cubemap.faces_count; 

  for(sizei i = 0; i < cube_desc.faces_count; i++) {
    cube_desc.data[i] = nbr_cubemap.pixels[i];
  }
  
  // Create the cubemap 
  ResourceID id = resources_push_cubemap(group_id, cube_desc); 

  // Freeing NBR data
  
  for(sizei i = 0; i < nbr_cubemap.faces_count; i++) {
    memory_free(nbr_cubemap.pixels[i]);
  }
  
  file_close(file); 

  // Add the resource to the named resources
  FilePath filename_without_ext = filepath_filename(nbr_path);
  filepath_set_extension(filename_without_ext, "");
  group->named_ids[filename_without_ext] = id;

  // New cubemap added!
  NIKOLA_LOG_DEBUG("     Path  = %s", nbr_path.c_str());
  return id;
}

ResourceID resources_push_shader(const ResourceGroupID& group_id, const GfxShaderDesc& shader_desc) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Create the shader
  ResourceID id; 
  GfxShader* shader = gfx_shader_create(renderer_get_context(), shader_desc);
  PUSH_RESOURCE(group, shaders, shader, RESOURCE_TYPE_SHADER, id);

  NIKOLA_LOG_DEBUG("Group \'%s\' pushed shader:", group->name.c_str());
  if(shader_desc.vertex_source) {
    NIKOLA_LOG_DEBUG("     Vertex source length = %zu", strlen(shader_desc.vertex_source));
    NIKOLA_LOG_DEBUG("     Pixel source length  = %zu", strlen(shader_desc.pixel_source));
  }
  else {
    NIKOLA_LOG_DEBUG("     Compute source length = %zu", strlen(shader_desc.compute_source));
  }
  return id;
}

ResourceID resources_push_shader(const ResourceGroupID& group_id, const FilePath& nbr_path) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
  
  // Load the NBR file
  NBRHeader header;
  File file;
  if(!open_and_check_nbr_file(group->parent_dir, nbr_path, &file, &header)) {
    return ResourceID{};
  }
  
  // Make sure it is the correct resource type
  NIKOLA_ASSERT((header.resource_type == RESOURCE_TYPE_SHADER), "Expected RESOURCE_TYPE_SHADER");

  // Load the NBR shader type
  NBRShader nbr_shader;
  file_read_bytes(file, &nbr_shader);

  // Convert the NBR format to a valid shader
 
  GfxShaderDesc shader_desc = {};
  shader_desc.vertex_source  = nbr_shader.vertex_source;
  shader_desc.pixel_source   = nbr_shader.pixel_source;
  shader_desc.compute_source = nbr_shader.compute_source;

  // Create the shader
  ResourceID id = resources_push_shader(group_id, shader_desc);

  // Freeing NBR data
  if(nbr_shader.vertex_source) {
    memory_free(nbr_shader.vertex_source);
    memory_free(nbr_shader.pixel_source);
  }
  else {
    memory_free(nbr_shader.compute_source);
  }
  
  file_close(file); 

  // Add the resource to the named resources
  FilePath filename_without_ext = filepath_filename(nbr_path);
  filepath_set_extension(filename_without_ext, "");
  group->named_ids[filename_without_ext] = id;

  // New shader added!
  NIKOLA_LOG_DEBUG("     Path = %s", nbr_path.c_str());
  return id;
}

ResourceID resources_push_shader_context(const ResourceGroupID& group_id, const ResourceID& shader_id) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
  
  // Allocate the context
  
  ShaderContext* ctx = new ShaderContext{};
  ctx->shader        = resources_get_shader(shader_id);

  // Create the context
  
  ResourceID id; 
  PUSH_RESOURCE(group, shader_contexts, ctx, RESOURCE_TYPE_SHADER_CONTEXT, id);

  // @TODO (Resource): Query the shader for uniform information
  // @TODO (Resource): Do something with the query information...

  // Set a default matrices buffer 
  
  GfxBuffer* matrix_buffer = renderer_get_defaults().matrices_buffer;
  shader_context_set_uniform_buffer(ctx, SHADER_MATRICES_BUFFER_INDEX, matrix_buffer);

  // New context added!
  
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed shader context:", group->name.c_str());
  return id;
}

ResourceID resources_push_shader_context(const ResourceGroupID& group_id, const FilePath& shader_path) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
 
  // Get the shader first
  ResourceID shader_id = resources_push_shader(group_id, shader_path);

  // New context added!
  return resources_push_shader_context(group_id, shader_id);
}

ResourceID resources_push_mesh(const ResourceGroupID& group_id, NBRMesh& nbr_mesh) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Allocate the mesh
  
  Mesh* mesh      = new Mesh{};
  mesh->pipe_desc = {}; 

  // Convert the NBR mesh into the engine's mesh format 
  
  GfxBufferDesc buff_desc = {
    .data  = (void*)nbr_mesh.vertices,
    .size  = nbr_mesh.vertices_count * sizeof(f32), 
    .type  = GFX_BUFFER_VERTEX, 
    .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  mesh->vertex_buffer = resources_get_buffer(resources_push_buffer(group_id, buff_desc));
    
  buff_desc = {
    .data  = (void*)nbr_mesh.indices,
    .size  = nbr_mesh.indices_count * sizeof(u32), 
    .type  = GFX_BUFFER_INDEX, 
    .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  mesh->index_buffer = resources_get_buffer(resources_push_buffer(group_id, buff_desc));

  mesh->pipe_desc.vertex_buffer  = mesh->vertex_buffer;
  mesh->pipe_desc.vertices_count = nbr_mesh.vertices_count;  
  mesh->pipe_desc.index_buffer   = mesh->index_buffer;
  mesh->pipe_desc.indices_count  = nbr_mesh.indices_count;  
  mesh->pipe_desc.draw_mode      = GFX_DRAW_MODE_TRIANGLE;

  vertex_type_layout((VertexType)nbr_mesh.vertex_type, &mesh->pipe_desc.layouts[0]);
  mesh->pipe = gfx_pipeline_create(renderer_get_context(), mesh->pipe_desc);

  // Create the mesh
  
  ResourceID id; 
  PUSH_RESOURCE(group, meshes, mesh, RESOURCE_TYPE_MESH, id);

  // Freeing NBR data

  // New mesh added!
  
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed mesh:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Vertex type   = %s", vertex_type_str((VertexType)nbr_mesh.vertex_type));
  NIKOLA_LOG_DEBUG("     Vertices      = %zu", mesh->pipe_desc.vertices_count);
  NIKOLA_LOG_DEBUG("     Indices       = %zu", mesh->pipe_desc.indices_count);
  return id;
}

ResourceID resources_push_mesh(const ResourceGroupID& group_id, const GeometryType type) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
  
  // Allocate the mesh
  Mesh* mesh = new Mesh{};

  // Use the loader to set up the mesh
  geometry_loader_load(group_id, &mesh->pipe_desc, type);

  // Setting the buffers
  mesh->vertex_buffer = mesh->pipe_desc.vertex_buffer;
  mesh->index_buffer  = mesh->pipe_desc.index_buffer;

  // Create the pipeline
  mesh->pipe = gfx_pipeline_create(renderer_get_context(), mesh->pipe_desc);

  // Create the mesh
  ResourceID id; 
  PUSH_RESOURCE(group, meshes, mesh, RESOURCE_TYPE_MESH, id);

  // New mesh added!
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed mesh:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Vertices      = %zu", mesh->pipe_desc.vertices_count);
  NIKOLA_LOG_DEBUG("     Indices       = %zu", mesh->pipe_desc.indices_count);
  NIKOLA_LOG_DEBUG("     Geomatry type = %s", geo_type_str(type));
  return id;
}

ResourceID resources_push_material(const ResourceGroupID& group_id, const MaterialDesc& desc) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
  
  // Allocate the material
  Material* material = new Material{};
  
  material->diffuse_map  = renderer_get_defaults().texture;
  material->specular_map = renderer_get_defaults().texture;
  
  material->color        = desc.color;
  material->shininess    = desc.shininess;
  material->transparency = desc.transparency;

  material->depth_mask  = desc.depth_mask;
  material->stencil_ref = desc.stencil_ref;

  // Textures init
  
  if(RESOURCE_IS_VALID(desc.diffuse_id)) {
    material->diffuse_map = resources_get_texture(desc.diffuse_id);
    material->map_flags  |= MATERIAL_TEXTURE_DIFFUSE;
  }
  
  if(RESOURCE_IS_VALID(desc.specular_id)) {
    material->specular_map = resources_get_texture(desc.specular_id);
    material->map_flags   |= MATERIAL_TEXTURE_SPECULAR;
  }

  // Create material
  ResourceID id;
  PUSH_RESOURCE(group, materials, material, RESOURCE_TYPE_MATERIAL, id);

  // New material added
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed material:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Color        = \'%s\'", vec3_to_string(material->color).c_str());
  NIKOLA_LOG_DEBUG("     Shininess    = \'%f\'", material->shininess);
  NIKOLA_LOG_DEBUG("     Transparency = \'%f\'", material->transparency);
  return id;
}

ResourceID resources_push_skybox(const ResourceGroupID& group_id, const ResourceID& cubemap_id) {
  NIKOLA_ASSERT(RESOURCE_IS_VALID(cubemap_id), "Cannot push a new skybox with an invalid cubemap");
  GROUP_CHECK(group_id);
  
  // Get the group
  ResourceGroup* group = &s_manager.groups[group_id];

  // Allocate the skybox
  Skybox* skybox = new Skybox{};
  
  // Use the loader to set up the skybox
  geometry_loader_load(group_id, &skybox->pipe_desc, GEOMETRY_SKYBOX);

  // Set the cubemap
  skybox->cubemap = resources_get_cubemap(cubemap_id);

  // Create the pipeline 
  skybox->pipe = gfx_pipeline_create(renderer_get_context(), skybox->pipe_desc);

  // Create skybox
  ResourceID id;
  PUSH_RESOURCE(group, skyboxes, skybox, RESOURCE_TYPE_SKYBOX, id);

  // New skybox added!
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed skybox:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Vertices = %zu", skybox->pipe_desc.vertices_count);
  NIKOLA_LOG_DEBUG("     Indices  = %zu", skybox->pipe_desc.indices_count);
  return id;
}

ResourceID resources_push_skybox(const ResourceGroupID& group_id, const FilePath& cubemap_path) {
  // Get the cubemap first
  ResourceID cubemap_id = resources_push_cubemap(group_id, cubemap_path);

  // New context added!
  return resources_push_skybox(group_id, cubemap_id);
}

ResourceID resources_push_model(const ResourceGroupID& group_id, const FilePath& nbr_path) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
  
  // Load the NBR file
  NBRHeader header;
  File file;
  if(!open_and_check_nbr_file(group->parent_dir, nbr_path, &file, &header)) {
    return ResourceID{};
  }
  
  // Make sure it is the correct resource type
  NIKOLA_ASSERT((header.resource_type == RESOURCE_TYPE_MODEL), "Expected RESOURCE_TYPE_MODEL");

  // Allocate the model
  Model* model = new Model{};
  
  // Load the NBR model
  NBRModel nbr_model;
  file_read_bytes(file, &nbr_model);
  
  // Convert the NBR format to a valid model
  
  // Make some space for the arrays for some better performance  
  model->meshes.reserve(nbr_model.meshes_count);
  model->materials.reserve(nbr_model.materials_count);
  model->material_indices.reserve(nbr_model.meshes_count);
  
  DynamicArray<ResourceID> texture_ids; // @FIX (Resource): This is bad. Don't do this!

  // Convert the textures
  
  for(sizei i = 0; i < nbr_model.textures_count; i++) {
    NBRTexture* nbr_texture = &nbr_model.textures[i];

    GfxTextureDesc desc; 
    desc.format    = GFX_TEXTURE_FORMAT_RGBA8; 
    desc.filter    = GFX_TEXTURE_FILTER_MIN_MAG_LINEAR; 
    desc.wrap_mode = GFX_TEXTURE_WRAP_CLAMP;
    desc.width     = nbr_texture->width; 
    desc.height    = nbr_texture->height; 
    desc.depth     = 0; 
    desc.mips      = 1; 
    desc.type      = GFX_TEXTURE_2D; 
    desc.data      = nbr_texture->pixels;
  
    texture_ids.push_back(resources_push_texture(group_id, desc));
  }
  
  // Convert the material 
  
  for(sizei i = 0; i < nbr_model.materials_count; i++) {
    Vec3 color        = Vec3(nbr_model.materials[i].diffuse[0], 
                             nbr_model.materials[i].diffuse[1], 
                             nbr_model.materials[i].diffuse[2]);
    
    i8 diffuse_index  = nbr_model.materials[i].diffuse_index;
    i8 specular_index = nbr_model.materials[i].specular_index;

    MaterialDesc mat_desc = {
      .diffuse_id  = diffuse_index != -1 ? texture_ids[diffuse_index] : ResourceID{}, 
      .specular_id = specular_index != -1 ? texture_ids[specular_index] : ResourceID{},

      .color = color,
    };
    ResourceID mat_id = resources_push_material(group_id, mat_desc);

    model->materials.push_back(resources_get_material(mat_id)); 
  }
  
  // Convert the vertices 
  
  for(sizei i = 0; i < nbr_model.meshes_count; i++) {
    ResourceID mesh_id = resources_push_mesh(group_id, nbr_model.meshes[i]);
    model->meshes.push_back(resources_get_mesh(mesh_id));

    model->material_indices.push_back(nbr_model.meshes[i].material_index);
  }

  // New model added!
  
  ResourceID id;
  PUSH_RESOURCE(group, models, model, RESOURCE_TYPE_MODEL, id);

  // Freeing NBR data
  
  for(sizei i = 0; i < nbr_model.meshes_count; i++) {
    memory_free(nbr_model.meshes[i].vertices);
    memory_free(nbr_model.meshes[i].indices);
  }

  for(sizei i = 0; i < nbr_model.textures_count; i++) {
    memory_free(nbr_model.textures[i].pixels);
  }

  memory_free(nbr_model.meshes);
  memory_free(nbr_model.materials);
  memory_free(nbr_model.textures);
  
  file_close(file); 

  // Add the resource to the named resources
  
  FilePath filename_without_ext = filepath_filename(nbr_path);
  filepath_set_extension(filename_without_ext, "");
  group->named_ids[filename_without_ext] = id;

  NIKOLA_LOG_DEBUG("Group \'%s\' pushed model:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Meshes    = %zu", model->meshes.size());
  NIKOLA_LOG_DEBUG("     Materials = %zu", model->materials.size());
  NIKOLA_LOG_DEBUG("     Textures  = %i", nbr_model.textures_count);
  NIKOLA_LOG_DEBUG("     Path      = %s", nbr_path.c_str());
  return id;
}

ResourceID resources_push_font(const ResourceGroupID& group_id, const FilePath& nbr_path) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
  
  // Load the NBR file
  NBRHeader header;
  File file;
  if(!open_and_check_nbr_file(group->parent_dir, nbr_path, &file, &header)) {
    return ResourceID{};
  }

  // Make sure it is the correct resource type
  NIKOLA_ASSERT((header.resource_type == RESOURCE_TYPE_FONT), "Expected RESOURCE_TYPE_FONT");

  // Allocate the model
  Font* font = new Font{};
  
  // Load the NBR font
  NBRFont nbr_font;
  file_read_bytes(file, &nbr_font);
  
  // Convert the NBR format to a valid model

  // Import the font information
  
  font->ascent   = (f32)nbr_font.ascent;
  font->descent  = (f32)nbr_font.descent;
  font->line_gap = (f32)nbr_font.line_gap;

  // Import the glyphs 
  
  for(sizei i = 0; i < nbr_font.glyphs_count; i++) {
    Glyph glyph;

    glyph.unicode = nbr_font.glyphs[i].unicode;

    glyph.size.x = nbr_font.glyphs[i].width;
    glyph.size.y = nbr_font.glyphs[i].height;
    
    glyph.offset.x = nbr_font.glyphs[i].offset_x;
    glyph.offset.y = nbr_font.glyphs[i].offset_y;
    
    glyph.left   = nbr_font.glyphs[i].left;
    glyph.top    = nbr_font.glyphs[i].top;
    glyph.right  = nbr_font.glyphs[i].right;
    glyph.bottom = nbr_font.glyphs[i].bottom;
    
    glyph.advance_x    = nbr_font.glyphs[i].advance_x;
    glyph.kern         = nbr_font.glyphs[i].kern;
    glyph.left_bearing = nbr_font.glyphs[i].left_bearing;

    // We don't care about glyphs that have a "non-size"
    if(glyph.size.x <= 0) {
      continue;
    }
  
    // Importing the texture
    
    GfxTextureDesc face_desc {
      .width  = (u32)nbr_font.glyphs[i].width,
      .height = (u32)nbr_font.glyphs[i].height,
      .depth  = 0, 
      .mips   = 1,

      .type      = GFX_TEXTURE_2D, 
      .format    = GFX_TEXTURE_FORMAT_R8, 
      .filter    = GFX_TEXTURE_FILTER_MIN_MAG_LINEAR, 
      .wrap_mode = GFX_TEXTURE_WRAP_CLAMP,
      
      .data = (void*)nbr_font.glyphs[i].pixels,
    };
    glyph.texture = resources_get_texture(resources_push_texture(group_id, face_desc));

    font->glyphs[glyph.unicode] = glyph;
  }

  // New font added!
  
  ResourceID id;
  PUSH_RESOURCE(group, fonts, font, RESOURCE_TYPE_FONT, id);

  // Freeing NBR data
  
  for(u32 i = 0; i < nbr_font.glyphs_count; i++) {
    memory_free(nbr_font.glyphs[i].pixels);
  }

  memory_free(nbr_font.glyphs);
  
  file_close(file); 

  // Add the resource to the named resources
  
  FilePath filename_without_ext = filepath_filename(nbr_path);
  filepath_set_extension(filename_without_ext, "");
  group->named_ids[filename_without_ext] = id;

  NIKOLA_LOG_DEBUG("Group \'%s\' pushed font:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Glyphs   = %zu", font->glyphs.size());
  NIKOLA_LOG_DEBUG("     Ascent   = %0.3f", font->ascent);
  NIKOLA_LOG_DEBUG("     Descent  = %0.3f", font->descent);
  NIKOLA_LOG_DEBUG("     Line gap = %0.3f", font->line_gap);
  NIKOLA_LOG_DEBUG("     Path     = %s", nbr_path.c_str());
  return id;
}

ResourceID resources_push_audio_buffer(const ResourceGroupID& group_id, const AudioBufferDesc& desc) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Create a new audio buffer
  AudioBufferID buffer = audio_buffer_create(desc);

  // New audio buffer added!
  ResourceID id;
  PUSH_RESOURCE(group, audio_buffers, buffer, RESOURCE_TYPE_AUDIO_BUFFER, id);

  NIKOLA_LOG_DEBUG("Group \'%s\' pushed an audio buffer:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Format      = %s", audio_format_str(desc.format));
  NIKOLA_LOG_DEBUG("     Channels    = %i", desc.channels);
  NIKOLA_LOG_DEBUG("     Size        = %zu", desc.size);
  NIKOLA_LOG_DEBUG("     Sample Rate = %zu", desc.sample_rate);
  return id;
}

ResourceID resources_push_audio_buffer(const ResourceGroupID& group_id, const FilePath& nbr_path) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Load the NBR file
  NBRHeader header;
  File file;
  if(!open_and_check_nbr_file(group->parent_dir, nbr_path, &file, &header)) {
    return ResourceID{};
  }
  
  // Make sure it is the correct resource type
  NIKOLA_ASSERT((header.resource_type == RESOURCE_TYPE_AUDIO_BUFFER), "Expected RESOURCE_TYPE_AUDIO_BUFFER");
  
  // Load the NBR audio
  NBRAudio nbr_audio;
  file_read_bytes(file, &nbr_audio);
  
  // Convert the NBR format to a valid audio buffer desc
  
  AudioBufferDesc desc = {};
  desc.format      = (AudioBufferFormat)nbr_audio.format; 
  desc.channels    = nbr_audio.channels; 
  desc.sample_rate = nbr_audio.sample_rate;
  desc.size        = nbr_audio.size;
  desc.data        = (void*)nbr_audio.samples;

  // New audio buffer added!
  ResourceID id = resources_push_audio_buffer(group_id, desc);
 
  // Freeing NBR data
  memory_free(nbr_audio.samples);
  file_close(file); 

  // Add the resource to the named resources
  
  FilePath filename_without_ext = filepath_filename(nbr_path);
  filepath_set_extension(filename_without_ext, "");
  group->named_ids[filename_without_ext] = id;
  
  NIKOLA_LOG_DEBUG("     Name        = %s", filename_without_ext.c_str());
  return id;
}

void resources_push_dir(const ResourceGroupID& group_id, const FilePath& dir) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
 
  // Retrieve all of the paths
  filesystem_directory_iterate(filepath_append(group->parent_dir, dir), resource_entry_iterate, group);
}

ResourceID& resources_get_id(const ResourceGroupID& group_id, const nikola::String& filename) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
 
  // The resource was not found
  if(group->named_ids.find(filename) == group->named_ids.end()) {
    NIKOLA_LOG_ERROR("Could not find resource \'%s\' in resource group \'%s\'", filename.c_str(), group->name.c_str());
    return group->named_ids["invalid"];
  }

  return group->named_ids[filename];
}

GfxBuffer* resources_get_buffer(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->buffers, RESOURCE_TYPE_BUFFER);
}

GfxTexture* resources_get_texture(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->textures, RESOURCE_TYPE_TEXTURE);
}

GfxCubemap* resources_get_cubemap(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->cubemaps, RESOURCE_TYPE_CUBEMAP);
}

GfxShader* resources_get_shader(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->shaders, RESOURCE_TYPE_SHADER);
}

ShaderContext* resources_get_shader_context(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->shader_contexts, RESOURCE_TYPE_SHADER_CONTEXT);
}

Mesh* resources_get_mesh(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->meshes, RESOURCE_TYPE_MESH);
}

Material* resources_get_material(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->materials, RESOURCE_TYPE_MATERIAL);
}

Skybox* resources_get_skybox(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->skyboxes, RESOURCE_TYPE_SKYBOX);
}

Model* resources_get_model(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->models, RESOURCE_TYPE_MODEL);
}

Font* resources_get_font(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->fonts, RESOURCE_TYPE_FONT);
}

AudioBufferID resources_get_audio_buffer(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->audio_buffers, RESOURCE_TYPE_AUDIO_BUFFER);
}

/// Resource manager functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
