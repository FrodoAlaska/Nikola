#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

#include "loaders/mesh_loader.hpp"
#include "loaders/material_loader.hpp"
#include "loaders/skybox_loader.hpp"

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
  
  DynamicArray<Mesh*> meshes;
  DynamicArray<Material*> materials;
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

#define GROUP_CHECK(group_id) NIKOLA_ASSERT((group_id != RESOURCE_CACHE_ID), "Cannot push a resource to an invalid group")

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

static const char* vertex_type_str(const VertexType type) {
  switch(type) {
    case VERTEX_TYPE_PNUV:
      return "VERTEX_TYPE_PNUV";
    case VERTEX_TYPE_PCUV:
      return "VERTEX_TYPE_PCUV";
    case VERTEX_TYPE_PNCUV:
      return "VERTEX_TYPE_PNCUV";
    default:
      return "INVALID VERTEX TYPE";
  }
}

static const char* mesh_type_str(const MeshType type) {
  switch(type) {
    case MESH_TYPE_CUBE:
      return "MESH_TYPE_CUBE";
    case MESH_TYPE_CIRCLE:
      return "VERTEX_TYPE_PCUV";
    case MESH_TYPE_CYLINDER:
      return "MESH_TYPE_CYLINDER";
    default:
      return "INVALID MESH TYPE";
  }
}

template<typename T> 
static T get_resource(const ResourceID& id, DynamicArray<T>& res, const ResourceType type) {
  NIKOLA_ASSERT((id._type == type), "Invalid type when trying to retrieve a resource");
  NIKOLA_ASSERT((id._id >= 0 && id._id <= (u16)res.size()), "Invalid ID when trying to retrieve a resource");
  NIKOLA_ASSERT((id.group != RESOURCE_GROUP_INVALID), "Cannot retrieve a resource from an invalid group");

  return res[id._id];
}

static void convert_from_nbr(const NBRTexture* nbr, GfxTextureDesc* desc) {
  desc->width  = nbr->width; 
  desc->height = nbr->height; 
  desc->depth  = 0; 
  desc->mips   = 1; 
  desc->type   = GFX_TEXTURE_2D; 
  desc->data   = memory_allocate(nbr->width * nbr->height * nbr->channels);

  memory_copy(desc->data, nbr->pixels, nbr->width * nbr->height * nbr->channels);
}

static void convert_from_nbr(const NBRCubemap* nbr, GfxCubemapDesc* desc) {
  desc->width       = nbr->width; 
  desc->height      = nbr->height; 
  desc->mips        = 1; 
  desc->faces_count = nbr->faces_count; 

  for(sizei i = 0; i < desc->faces_count; i++) {
    desc->data[i] = nbr->pixels[i];
  }
}

static void convert_from_nbr(const ResourceGroup* group, const NBRModel* nbr, Model* model) {
  // Make some space for the arrays for some better performance  
  model->meshes.reserve(nbr->meshes_count);
  model->materials.reserve(nbr->materials_count);
  model->material_indices.reserve(nbr->meshes_count);
  
  nikola::DynamicArray<ResourceID> texture_ids; // @FIX (Resource): This is bad. Don't do this!

  // Convert the textures
  for(sizei i = 0; i < nbr->textures_count; i++) {
    GfxTextureDesc desc; 
    desc.format    = GFX_TEXTURE_FORMAT_RGBA8; 
    desc.filter    = GFX_TEXTURE_FILTER_MIN_MAG_LINEAR; 
    desc.wrap_mode = GFX_TEXTURE_WRAP_CLAMP;
    convert_from_nbr(&nbr->textures[i], &desc);
  
    texture_ids.push_back(resources_push_texture(group->id, desc));
  }

  // Convert the material 
  for(sizei i = 0; i < nbr->materials_count; i++) {
    // Create a new material 
    ResourceID mat_id = resources_push_material(group->id, ResourceID{});
    Material* mat     = resources_get_material(mat_id);

    // Insert a valid diffuse texture 
    mat->diffuse_map = resources_get_texture(texture_ids[nbr->materials[i].diffuse_index]);
    
    // Insert a valid specular texture 
    i8 specular_index = nbr->materials[i].specular_index;
    if(specular_index != -1) {
      mat->specular_map = resources_get_texture(texture_ids[specular_index]);
    }

    // Set the colors of the new material
    mat->ambient_color  = Vec3(nbr->materials[i].ambient[0], nbr->materials[i].ambient[1], nbr->materials[i].ambient[2]); 
    mat->diffuse_color  = Vec3(nbr->materials[i].diffuse[0], nbr->materials[i].diffuse[1], nbr->materials[i].diffuse[2]); 
    mat->specular_color = Vec3(nbr->materials[i].specular[0], nbr->materials[i].specular[1], nbr->materials[i].specular[2]); 

    // Add the material 
    model->materials.push_back(mat); 
  }
  
  // Convert the vertices 
  for(sizei i = 0; i < nbr->meshes_count; i++) {
    // Create a vertex buffer
    GfxBufferDesc buff_desc = {
      .data  = (void*)nbr->meshes[i].vertices,
      .size  = nbr->meshes[i].vertices_count * sizeof(f32), 
      .type  = GFX_BUFFER_VERTEX, 
      .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
    };
    ResourceID vert_buff_id = resources_push_buffer(group->id, buff_desc);
    
    // Create a index buffer
    buff_desc = {
      .data  = (void*)nbr->meshes[i].indices,
      .size  = nbr->meshes[i].indices_count * sizeof(u32), 
      .type  = GFX_BUFFER_INDEX, 
      .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
    };
    ResourceID idx_buff_id = resources_push_buffer(group->id, buff_desc);
    
    // Create a new mesh 
    ResourceID mesh_id = resources_push_mesh(group->id, vert_buff_id, (VertexType)nbr->meshes[i].vertex_type, idx_buff_id, nbr->meshes[i].indices_count);
    
    Mesh* mesh         = resources_get_mesh(mesh_id);
    u8 material_index  = nbr->meshes[i].material_index; 

    // Add a new index
    model->material_indices.push_back(material_index);
   
    // Add the new mesh
    model->meshes.push_back(mesh);
  }
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
}

/// Private functions 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

static void resource_entry_iterate(const FilePath& base, FilePath path, void* user_data) {
  ResourceGroup* group = (ResourceGroup*)user_data;

  if(!filesystem_exists(path)) {
    NIKOLA_LOG_ERROR("Cannot push non-existent resource at \'%s\'", path.c_str());
    return;
  }
  
  ResourceType type = get_resource_extension_type(path);
  FilePath filename = filepath_filename(path);
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
    default:
      NIKOLA_LOG_ERROR("Invalid resource type \'%s\'", path.c_str());
      break;
  }
}

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Resource manager functions

void resource_manager_init() {
  const GfxContext* gfx = renderer_get_context();
  NIKOLA_ASSERT(gfx, "Invalid graphics context passed to the resource manager");

  s_manager.gfx_context               = (GfxContext*)gfx;
  s_manager.groups[RESOURCE_CACHE_ID] = ResourceGroup {
    .name       = "cache", 
    .parent_dir = "resource_cache",
    .id         = RESOURCE_CACHE_ID,
  };

  const RendererDefaults render_defaults = renderer_get_defaults();

  //
  // @FIX (Resource/Renderer): Perhaps there is a better way than this to add 
  // the default resources to the resource cache.
  // 

  ResourceGroup* group = &s_manager.groups[RESOURCE_CACHE_ID];

  // Add the default matrices buffer
  ResourceID matrix_id;
  PUSH_RESOURCE(group, buffers, render_defaults.matrices_buffer, RESOURCE_TYPE_BUFFER, matrix_id);
  
  // Add the default texture 
  ResourceID texture_id;
  PUSH_RESOURCE(group, textures, render_defaults.texture, RESOURCE_TYPE_TEXTURE, texture_id);

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

  group->meshes.clear();
  group->materials.clear();
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

  // Destroy core resources
  DESTROY_CORE_RESOURCE_MAP(group, buffers, gfx_buffer_destroy);
  DESTROY_CORE_RESOURCE_MAP(group, textures, gfx_texture_destroy);
  DESTROY_CORE_RESOURCE_MAP(group, cubemaps, gfx_cubemap_destroy);
  DESTROY_CORE_RESOURCE_MAP(group, shaders, gfx_shader_destroy);

  // Destroy compound resources
  DESTROY_COMP_RESOURCE_MAP(group, meshes);
  DESTROY_COMP_RESOURCE_MAP(group, materials);
  DESTROY_COMP_RESOURCE_MAP(group, skyboxes);
  DESTROY_COMP_RESOURCE_MAP(group, models);
  DESTROY_COMP_RESOURCE_MAP(group, fonts);

  NIKOLA_LOG_INFO("Resource group \'%s\' was successfully destroyed", group->name.c_str());
  s_manager.groups.erase(group_id);
}

ResourceID resources_push_buffer(const u16 group_id, const GfxBufferDesc& buff_desc) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Create the buffer
  ResourceID id; 
  GfxBuffer* buffer = gfx_buffer_create(s_manager.gfx_context, buff_desc);
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
  GfxTexture* texture = gfx_texture_create(s_manager.gfx_context, desc);
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
  convert_from_nbr(nbr_texture, &tex_desc);

  // Create the texture 
  ResourceID id; 
  GfxTexture* texture = gfx_texture_create(s_manager.gfx_context, tex_desc);
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
  GfxCubemap* cubemap = gfx_cubemap_create(s_manager.gfx_context, cubemap_desc);
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

  // Convert the NBR format to a valid cubemap
  convert_from_nbr(nbr_cubemap, &cube_desc);

  // Create the cubemap
  ResourceID id; 
  GfxCubemap* cubemap = gfx_cubemap_create(s_manager.gfx_context, cube_desc);
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
  GfxShader* shader = gfx_shader_create(s_manager.gfx_context, shader_desc);
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
  NBRShader* nbr_shader  = (NBRShader*)nbr.body_data;
  GfxShaderDesc shader_desc = {
    .vertex_source = nbr_shader->vertex_source,
    .pixel_source  = nbr_shader->pixel_source,
  };

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

ResourceID resources_push_mesh(const u16 group_id, 
                               const ResourceID& vertex_buffer_id, 
                               const VertexType vertex_type, 
                               const ResourceID& index_buffer_id, 
                               const sizei indices_count) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Allocate the mesh
  Mesh* mesh = new Mesh{};

  // Use the loader to set up the mesh
  mesh_loader_load(group_id, mesh, vertex_buffer_id, vertex_type, index_buffer_id, indices_count);

  // Create the pipeline
  mesh->pipe = gfx_pipeline_create(s_manager.gfx_context, mesh->pipe_desc);

  // Create the mesh
  ResourceID id; 
  PUSH_RESOURCE(group, meshes, mesh, RESOURCE_TYPE_MESH, id);

  // New mesh added!
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed mesh:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Vertex type  = %s", vertex_type_str(vertex_type));
  NIKOLA_LOG_DEBUG("     Vertices     = %zu", mesh->pipe_desc.vertices_count);
  NIKOLA_LOG_DEBUG("     Indices      = %zu", indices_count);
  return id;
}

ResourceID resources_push_mesh(const u16 group_id, const MeshType type) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Allocate the mesh
  Mesh* mesh = new Mesh{};

  // Use the loader to set up the mesh
  mesh_loader_load(group_id, mesh, type);

  // Create the pipeline
  mesh->pipe = gfx_pipeline_create(s_manager.gfx_context, mesh->pipe_desc);

  // Create mesh
  ResourceID id; 
  PUSH_RESOURCE(group, meshes, mesh, RESOURCE_TYPE_MESH, id);
 
  // New mesh added!
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed mesh:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Mesh type  = %s", mesh_type_str(type));
  NIKOLA_LOG_DEBUG("     Vertices   = %zu", mesh->pipe_desc.vertices_count);
  NIKOLA_LOG_DEBUG("     Indices    = %zu", mesh->pipe_desc.indices_count);
  return id;
}

ResourceID resources_push_material(const u16 group_id, const ResourceID& shader_id) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
  
  // Allocate the material
  Material* material = new Material{};

  // Use the loader to set up the material
  material_loader_load(group_id, material, shader_id);

  // Create material
  ResourceID id;
  PUSH_RESOURCE(group, materials, material, RESOURCE_TYPE_MATERIAL, id);

  // New material added
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed material:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Uniforms count = \'%zu\'", material->uniform_locations.size());
  NIKOLA_LOG_DEBUG("     Ambient color  = \'%s\'", vec3_to_string(material->ambient_color).c_str());
  NIKOLA_LOG_DEBUG("     Diffuse color  = \'%s\'", vec3_to_string(material->diffuse_color).c_str());
  NIKOLA_LOG_DEBUG("     Specular color = \'%s\'", vec3_to_string(material->specular_color).c_str());
  return id;
}

ResourceID resources_push_skybox(const u16 group_id, const ResourceID& cubemap_id) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Allocate the skybox
  Skybox* skybox = new Skybox{};
  
  // Use the loader to set up the skybox
  skybox_loader_load(group_id, skybox, cubemap_id);

  // Create the pipeline 
  skybox->pipe = gfx_pipeline_create(s_manager.gfx_context, skybox->pipe_desc);

  // Create skybox
  ResourceID id;
  PUSH_RESOURCE(group, skyboxes, skybox, RESOURCE_TYPE_SKYBOX, id);

  // New skybox added!
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed skybox:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Vertices = %zu", skybox->pipe_desc.vertices_count);
  NIKOLA_LOG_DEBUG("     Indices  = %zu", skybox->pipe_desc.indices_count);
  return id;
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
  convert_from_nbr(group, nbr_model, model);

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

void resources_push_dir(const u16 group_id, const FilePath& dir) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Create the full path 
  FilePath full_path = filepath_append(group->parent_dir, dir);

  // Cannot proceed if the directory is invalid
  if(!filesystem_exists(full_path)) {
    NIKOLA_LOG_ERROR("Resource directory \'%s\' does not exist", dir.c_str());
    return;
  }
 
  // Retrieve all of the paths
  filesystem_directory_iterate(full_path, resource_entry_iterate, group);
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

/// Resource manager functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
