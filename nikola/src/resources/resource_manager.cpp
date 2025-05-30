#include "nikola/nikola_resources.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_gfx.h"
#include "nikola/nikola_file.h"
#include "nikola/nikola_render.h"
#include "nikola/nikola_audio.h"

#include "loaders/geomatry_loader.h"

#include <cstring>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// ResourceGroup 
struct ResourceGroup {
  String name; 
  FilePath parent_dir;
  u16 id;

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
  HashMap<u16, ResourceGroup> groups;
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

static const char* buffer_type_str(const GfxBufferType type) {
  switch(type) {
    case GFX_BUFFER_VERTEX: 
      return "GFX_BUFFER_VERTEX";
    case GFX_BUFFER_INDEX: 
      return "GFX_BUFFER_INDEX";
    case GFX_BUFFER_UNIFORM: 
      return "GFX_BUFFER_UNIFORM";
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
    case GFX_TEXTURE_RENDER_TARGET:
      return "GFX_TEXTURE_RENDER_TARGET";
    case GFX_TEXTURE_DEPTH_STENCIL_TARGET:
      return "GFX_TEXTURE_DEPTH_STENCIL_TARGET";
    default:
      return "INVALID TEXTURE TYPE";
  }
}

static const char* geo_type_str(const GeomatryType type) {
  switch(type) {
    case GEOMATRY_CUBE:
      return "GEOMATRY_CUBE";
    case GEOMATRY_PLANE:
      return "GEOMATRY_PLANE";
    case GEOMATRY_SKYBOX:
      return "GEOMATRY_SKYBOX";
    case GEOMATRY_CIRCLE:
      return "GEOMATRY_CIRCLE";
    default:
      return "INVALID GEOMATRY TYPE";
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

static ResourceType get_resource_extension_type(const FilePath& path) {
  FilePath ext = filepath_extension(path); 

  if(ext == ".nbrtexture") {
    return RESOURCE_TYPE_TEXTURE;
  }
  else if(ext == ".nbrcubemap") {
    return RESOURCE_TYPE_CUBEMAP;
  }
  else if(ext == ".nbrshader") {
    return RESOURCE_TYPE_SHADER;
  }
  else if(ext == ".nbrmodel") {
    return RESOURCE_TYPE_MODEL;
  }
  else if(ext == ".nbrfont") {
    return RESOURCE_TYPE_FONT;
  }
  else if(ext == ".nbraudio") {
    return RESOURCE_TYPE_AUDIO_BUFFER;
  }
}

static void reload_texture(NBRFile& file, const ResourceID& id) {
  GfxTexture* texture = resources_get_texture(id);  

  // Create the new texture
  NBRTexture* nbr_texture     = (NBRTexture*)file.body_data;
  GfxTextureDesc texture_desc = gfx_texture_get_desc(texture);
  nbr_import_texture(nbr_texture, &texture_desc);
  
  // Update the texture
  gfx_texture_update(texture, texture_desc);
  gfx_texture_upload_data(texture, texture_desc.width, texture_desc.height, texture_desc.depth, texture_desc.data);
}

static void reload_cubemap(NBRFile& file, const ResourceID& id) {
  GfxCubemap* cubemap = resources_get_cubemap(id);  

  // Create the new cubemap
  NBRCubemap* nbr_cubemap     = (NBRCubemap*)file.body_data;
  GfxCubemapDesc cubemap_desc = gfx_cubemap_get_desc(cubemap);
  nbr_import_cubemap(nbr_cubemap, &cubemap_desc);
 
  /* @NOTE (19/4/2025, Mohamed):
   *
   * Now, listen, is this ugly? Yes. Is it dangerous? Possibly, yes. 
   * But does it work, though? Yes! And that's the most important thing. 
   * It's not my fault that Bjarne decided that `void*[6]` does not 
   * equal `const void**`. Take it up with him. Don't blame me.
   *  
   */

  // Update the cubemap
  gfx_cubemap_update(cubemap, cubemap_desc);
  gfx_cubemap_upload_data(cubemap, cubemap_desc.width, cubemap_desc.height, (const void**)cubemap_desc.data, cubemap_desc.faces_count);
}

static void reload_shader(NBRFile& file, const ResourceID& id) {
  GfxShader* shader = resources_get_shader(id);  

  // Create the new shader
  NBRShader* nbr_shader     = (NBRShader*)file.body_data;
  GfxShaderDesc shader_desc = {};
  nbr_import_shader(nbr_shader, &shader_desc);
  
  // Update the shader
  gfx_shader_update(shader, shader_desc);
}

static void reload_core_resource(const ResourceGroup* group, const ResourceID& id, const FilePath& nbr_path) {
  // Load the NBR file
  NBRFile nbr;
  nbr_file_load(&nbr, nbr_path);

  switch (id._type) {
    case RESOURCE_TYPE_TEXTURE:
      reload_texture(nbr, id);
      break;
    case RESOURCE_TYPE_CUBEMAP:
      reload_cubemap(nbr, id);
      break;
    case RESOURCE_TYPE_SHADER:
      reload_shader(nbr, id);
      break;
    default:
      break;
  }

  nbr_file_unload(nbr);
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
  
  FilePath filename = filepath_filename(path);
  ResourceType type = get_resource_extension_type(filename);
  filepath_set_extension(filename, ""); // Need to remove the extension

  switch (type) {
    case RESOURCE_TYPE_TEXTURE:
      group->named_ids[filename] = resources_push_texture(group->id, path);
      break;
    case RESOURCE_TYPE_CUBEMAP:
      group->named_ids[filename] = resources_push_cubemap(group->id, path);
      break;
    case RESOURCE_TYPE_SHADER:
      group->named_ids[filename] = resources_push_shader(group->id, path);
      break;
    case RESOURCE_TYPE_MODEL:
      group->named_ids[filename] = resources_push_model(group->id, path);
      break;
    case RESOURCE_TYPE_FONT:
      group->named_ids[filename] = resources_push_font(group->id, path);
      break;
    case RESOURCE_TYPE_AUDIO_BUFFER:
      group->named_ids[filename] = resources_push_audio_buffer(group->id, path);
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
 
  // Get the filename without the extension
  FilePath filename = filepath_filename(path); 
  filepath_set_extension(filename, "");

  ResourceGroup* group = (ResourceGroup*)user_data;
  ResourceID res_id    = resources_get_id(group->id, filename);

  switch (res_id._type) {
    case RESOURCE_TYPE_TEXTURE:
    case RESOURCE_TYPE_CUBEMAP:
    case RESOURCE_TYPE_SHADER:
      reload_core_resource(group, res_id, path);
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
      NIKOLA_LOG_ERROR("Unknown resource type");
      break;
  }
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
  u16 group_id               = random_u32(RESOURCE_CACHE_ID + 1, RESOURCE_GROUP_INVALID - 1); 
  s_manager.groups[group_id] = ResourceGroup {
    .name       = name, 
    .parent_dir = parent_dir,
    .id         = group_id,
  };

  // A default resource for later
  ResourceGroup* group = &s_manager.groups[group_id]; 
  group->named_ids["invalid"] = ResourceID{}; 
 
  // Add a file watcher to the parent directory
  filewatcher_add_dir(parent_dir, resource_entry_update, group);

  NIKOLA_LOG_INFO("Successfully created a resource group \'%s\'", name.c_str());
  return group_id;
}

void resources_clear_group(const u16 group_id) {
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

void resources_destroy_group(const u16 group_id) {
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

ResourceID resources_push_buffer(const u16 group_id, const GfxBufferDesc& buff_desc) {
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

ResourceID resources_push_texture(const u16 group_id, const GfxTextureDesc& desc) {
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

ResourceID resources_push_texture(const u16 group_id, 
                                  const FilePath& nbr_path,
                                  const GfxTextureFormat format, 
                                  const GfxTextureFilter filter, 
                                  const GfxTextureWrap wrap) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
 
  // Load the NBR file
  NBRFile nbr;
  nbr_file_load(&nbr, filepath_append(group->parent_dir, nbr_path));

  // Make sure it is the correct resource type
  NIKOLA_ASSERT((nbr.resource_type == RESOURCE_TYPE_TEXTURE), "Expected RESOURCE_TYPE_TEXTURE");

  NBRTexture* nbr_texture = (NBRTexture*)nbr.body_data;
  GfxTextureDesc tex_desc; 
  tex_desc.format    = format; 
  tex_desc.filter    = filter; 
  tex_desc.wrap_mode = wrap;

  // Convert the NBR format to a valid texture
  nbr_import_texture(nbr_texture, &tex_desc);

  // Create the texture 
  ResourceID id; 
  GfxTexture* texture = gfx_texture_create(renderer_get_context(), tex_desc);
  PUSH_RESOURCE(group, textures, texture, RESOURCE_TYPE_TEXTURE, id);

  // Remember to close the NBR
  nbr_file_unload(nbr);

  // Add the resource to the named resources
  FilePath filename_without_ext = filepath_filename(nbr_path);
  filepath_set_extension(filename_without_ext, "");
  group->named_ids[filename_without_ext] = id;

  // New texture added!
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed texture:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Size = %i X %i", tex_desc.width, tex_desc.height);
  NIKOLA_LOG_DEBUG("     Type = %s", texture_type_str(tex_desc.type));
  NIKOLA_LOG_DEBUG("     Path = %s", nbr_path.c_str());
  return id;
}

ResourceID resources_push_cubemap(const u16 group_id, const GfxCubemapDesc& cubemap_desc) {
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

ResourceID resources_push_cubemap(const u16 group_id, 
                                  const FilePath& nbr_path,
                                  const GfxTextureFormat format, 
                                  const GfxTextureFilter filter, 
                                  const GfxTextureWrap wrap) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Load the NBR file
  NBRFile nbr;
  nbr_file_load(&nbr, filepath_append(group->parent_dir, nbr_path));

  // Make sure it is the correct resource type
  NIKOLA_ASSERT((nbr.resource_type == RESOURCE_TYPE_CUBEMAP), "Expected RESOURCE_TYPE_CUBEMAP");

  // Convert the NBR format to a valid cubemap
  NBRCubemap* nbr_cubemap = (NBRCubemap*)nbr.body_data;
  GfxCubemapDesc cube_desc; 
  cube_desc.format    = format; 
  cube_desc.filter    = filter; 
  cube_desc.wrap_mode = wrap;
  nbr_import_cubemap(nbr_cubemap, &cube_desc);

  // Create the cubemap
  ResourceID id; 
  GfxCubemap* cubemap = gfx_cubemap_create(renderer_get_context(), cube_desc);
  PUSH_RESOURCE(group, cubemaps, cubemap, RESOURCE_TYPE_CUBEMAP, id);

  // Remember to close the NBR
  nbr_file_unload(nbr);

  // Add the resource to the named resources
  FilePath filename_without_ext = filepath_filename(nbr_path);
  filepath_set_extension(filename_without_ext, "");
  group->named_ids[filename_without_ext] = id;

  // New cubemap added!
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed cubemap:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Size  = %i X %i", cube_desc.width, cube_desc.height);
  NIKOLA_LOG_DEBUG("     Faces = %i", cube_desc.faces_count);
  NIKOLA_LOG_DEBUG("     Path  = %s", nbr_path.c_str());
  return id;
}

ResourceID resources_push_shader(const u16 group_id, const GfxShaderDesc& shader_desc) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Create the shader
  ResourceID id; 
  GfxShader* shader = gfx_shader_create(renderer_get_context(), shader_desc);
  PUSH_RESOURCE(group, shaders, shader, RESOURCE_TYPE_SHADER, id);

  NIKOLA_LOG_DEBUG("Group \'%s\' pushed shader:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Vertex source length = %zu", strlen(shader_desc.vertex_source));
  NIKOLA_LOG_DEBUG("     Pixel source length  = %zu", strlen(shader_desc.pixel_source));
  return id;
}

ResourceID resources_push_shader(const u16 group_id, const FilePath& nbr_path) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
  
  // Load the NBR file
  NBRFile nbr;
  nbr_file_load(&nbr, filepath_append(group->parent_dir, nbr_path));
  
  // Make sure it is the correct resource type
  NIKOLA_ASSERT((nbr.resource_type == RESOURCE_TYPE_SHADER), "Expected RESOURCE_TYPE_SHADER");

  // Convert the NBR format to a valid shader
  NBRShader* nbr_shader     = (NBRShader*)nbr.body_data;
  GfxShaderDesc shader_desc = {};
  nbr_import_shader(nbr_shader, &shader_desc);

  // Create the shader
  ResourceID id = resources_push_shader(group_id, shader_desc);

  // Remember to close the NBR
  nbr_file_unload(nbr);

  // Add the resource to the named resources
  FilePath filename_without_ext = filepath_filename(nbr_path);
  filepath_set_extension(filename_without_ext, "");
  group->named_ids[filename_without_ext] = id;

  // New shader added!
  NIKOLA_LOG_DEBUG("     Path = %s", nbr_path.c_str());
  return id;
}

ResourceID resources_push_shader_context(const u16 group_id, const ResourceID& shader_id) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
  
  // Allocate the context
  ShaderContext* ctx = new ShaderContext{};
  ctx->shader        = resources_get_shader(shader_id);
  
  // Create the context
  ResourceID id; 
  PUSH_RESOURCE(group, shader_contexts, ctx, RESOURCE_TYPE_SHADER_CONTEXT, id);
 
  // Set a default matrices buffer 
  GfxBuffer* matrix_buffer = renderer_get_defaults().matrices_buffer;
  shader_context_set_uniform_buffer(ctx, SHADER_MATRICES_BUFFER_INDEX, matrix_buffer);

  // New context added!
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed shader context:", group->name.c_str());
  return id;
}

ResourceID resources_push_shader_context(const u16 group_id, const FilePath& shader_path) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
 
  // Get the shader first
  ResourceID shader_id = resources_push_shader(group_id, shader_path);

  // New context added!
  return resources_push_shader_context(group_id, shader_id);
}

ResourceID resources_push_mesh(const u16 group_id, NBRMesh& nbr_mesh) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Allocate the mesh
  Mesh* mesh = new Mesh{};

  // Convert the NBR mesh into the engine's mesh format 
  nbr_import_mesh(&nbr_mesh, group_id, mesh);

  // Create the pipeline
  mesh->pipe = gfx_pipeline_create(renderer_get_context(), mesh->pipe_desc);

  // Create the mesh
  ResourceID id; 
  PUSH_RESOURCE(group, meshes, mesh, RESOURCE_TYPE_MESH, id);

  // New mesh added!
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed mesh:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Vertex type   = %s", vertex_type_str((VertexType)nbr_mesh.vertex_type));
  NIKOLA_LOG_DEBUG("     Vertices      = %zu", mesh->pipe_desc.vertices_count);
  NIKOLA_LOG_DEBUG("     Indices       = %zu", mesh->pipe_desc.indices_count);
  return id;
}

ResourceID resources_push_mesh(const u16 group_id, const GeomatryType type) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
  
  // Allocate the mesh
  Mesh* mesh = new Mesh{};

  // Use the loader to set up the mesh
  geomatry_loader_load(group_id, &mesh->pipe_desc, type);

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

ResourceID resources_push_material(const u16 group_id, const ResourceID& diffuse_map) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
  
  // Allocate the material
  Material* material = new Material{};

  // Set default values for the material
  material->color     = Vec4(1.0f); 
  material->shininess = 1.0f;

  // Default textures init
  GfxTexture* default_texture = renderer_get_defaults().texture;
  material->diffuse_map       = RESOURCE_IS_VALID(diffuse_map) ? resources_get_texture(diffuse_map) : default_texture;
  material->specular_map      = default_texture;

  // Create material
  ResourceID id;
  PUSH_RESOURCE(group, materials, material, RESOURCE_TYPE_MATERIAL, id);

  // New material added
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed material:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Color     = \'%s\'", vec3_to_string(material->color).c_str());
  NIKOLA_LOG_DEBUG("     Shininess = \'%f\'", material->shininess);
  return id;
}

ResourceID resources_push_material(const u16 group_id, const FilePath& diffuse_path) {
  // Get the texture first
  ResourceID diffuse_id = resources_push_texture(group_id, diffuse_path);
  
  // New material added!
  return resources_push_material(group_id, diffuse_id);
}

ResourceID resources_push_skybox(const u16 group_id, const ResourceID& cubemap_id) {
  NIKOLA_ASSERT(RESOURCE_IS_VALID(cubemap_id), "Cannot push a new skybox with an invalid cubemap");
  GROUP_CHECK(group_id);
  
  // Get the group
  ResourceGroup* group = &s_manager.groups[group_id];

  // Allocate the skybox
  Skybox* skybox = new Skybox{};
  
  // Use the loader to set up the skybox
  geomatry_loader_load(group_id, &skybox->pipe_desc, GEOMATRY_SKYBOX);

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

ResourceID resources_push_skybox(const u16 group_id, const FilePath& cubemap_path) {
  // Get the cubemap first
  ResourceID cubemap_id = resources_push_cubemap(group_id, cubemap_path);

  // New context added!
  return resources_push_skybox(group_id, cubemap_id);
}

ResourceID resources_push_model(const u16 group_id, const FilePath& nbr_path) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
  
  // Load the NBR file
  NBRFile nbr;
  nbr_file_load(&nbr, filepath_append(group->parent_dir, nbr_path));

  // Allocate the model
  Model* model = new Model{};
  
  // Convert the NBR format to a valid model
  NBRModel* nbr_model = (NBRModel*)nbr.body_data; 
  nbr_import_model(nbr_model, group_id, model);

  // New model added!
  ResourceID id;
  PUSH_RESOURCE(group, models, model, RESOURCE_TYPE_MODEL, id);

  // Remember to close the NBR
  nbr_file_unload(nbr);

  // Add the resource to the named resources
  FilePath filename_without_ext = filepath_filename(nbr_path);
  filepath_set_extension(filename_without_ext, "");
  group->named_ids[filename_without_ext] = id;

  NIKOLA_LOG_DEBUG("Group \'%s\' pushed model:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Meshes    = %zu", model->meshes.size());
  NIKOLA_LOG_DEBUG("     Materials = %zu", model->materials.size());
  NIKOLA_LOG_DEBUG("     Textures  = %i", nbr_model->textures_count);
  NIKOLA_LOG_DEBUG("     Path      = %s", nbr_path.c_str());
  return id;
}

ResourceID resources_push_font(const u16 group_id, const FilePath& nbr_path) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
  
  // Load the NBR file
  NBRFile nbr;
  nbr_file_load(&nbr, filepath_append(group->parent_dir, nbr_path));

  // Allocate the model
  Font* font = new Font{};
  
  // Convert the NBR format to a valid model
  NBRFont* nbr_font = (NBRFont*)nbr.body_data; 
  nbr_import_font(nbr_font, group_id, font);

  // New font added!
  ResourceID id;
  PUSH_RESOURCE(group, fonts, font, RESOURCE_TYPE_FONT, id);

  // Remember to close the NBR
  nbr_file_unload(nbr);

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

ResourceID resources_push_audio_buffer(const u16 group_id, const AudioBufferDesc& desc) {
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

ResourceID resources_push_audio_buffer(const u16 group_id, const FilePath& nbr_path) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Load the NBR file
  NBRFile nbr;
  nbr_file_load(&nbr, filepath_append(group->parent_dir, nbr_path));
  
  // Convert the NBR format to a valid audio buffer desc
  AudioBufferDesc desc = {};
  NBRAudio* nbr_audio  = (NBRAudio*)nbr.body_data; 
  nbr_import_audio(nbr_audio, group_id, &desc);

  // New audio buffer added!
  ResourceID id = resources_push_audio_buffer(group_id, desc);
 
  // Remember to close the NBR
  nbr_file_unload(nbr);
  
  // Add the resource to the named resources
  FilePath filename_without_ext = filepath_filename(nbr_path);
  filepath_set_extension(filename_without_ext, "");
  group->named_ids[filename_without_ext] = id;
  
  NIKOLA_LOG_DEBUG("     Name        = %s", filename_without_ext.c_str());
  return id;
}

void resources_push_dir(const u16 group_id, const FilePath& dir) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
 
  // Retrieve all of the paths
  filesystem_directory_iterate(filepath_append(group->parent_dir, dir), resource_entry_iterate, group);
}

ResourceID& resources_get_id(const u16 group_id, const nikola::String& filename) {
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
