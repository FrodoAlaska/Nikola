#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

#include "loaders/mesh_loader.hpp"
#include "loaders/material_loader.hpp"
#include "loaders/skybox_loader.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// StorageManager 
struct StorageManager {
  HashMap<String, ResourceStorage*> storages;

  ResourceStorage* cached_storage = nullptr;
  GfxContext* gfx_context         = nullptr;
};

static StorageManager s_manager;
/// StorageManager 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ResourceStorage 
struct ResourceStorage {
  String name; 
  FilePath parent_dir;

  HashMap<ResourceID, GfxBuffer*> buffers;
  HashMap<ResourceID, GfxTexture*> textures;
  HashMap<ResourceID, GfxCubemap*> cubemaps;
  HashMap<ResourceID, GfxShader*> shaders;
  
  HashMap<ResourceID, Mesh*> meshes;
  HashMap<ResourceID, Material*> materials;
  HashMap<ResourceID, Skybox*> skyboxes;
  HashMap<ResourceID, Model*> models;
  HashMap<ResourceID, Font*> fonts;
};
/// ResourceStorage 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Macros (Unfortunately)

#define DESTROY_CORE_RESOURCE_MAP(storage, map, clear_func) { \
  for(auto& [key, value] : storage->map) {                    \
    clear_func(value);                                        \
  }                                                           \
}

#define DESTROY_COMP_RESOURCE_MAP(storage, map) { \
  for(auto& [key, value] : storage->map) {        \
    delete value;                                 \
  }                                               \
}

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

static ResourceID generate_id() {
  return random_u64(); // @TODO: Make something more complex than this
}

template<typename T>
static T get_resource(ResourceStorage* storage, HashMap<ResourceID, T>& map, const ResourceID& id, const char* res_name) {
  // We cannot return a non-existing resource neither will 
  // we add it to the storage 
  if(map.find(id) == map.end()) {
    NIKOLA_LOG_ERROR("Resource id \'%i\' of type \'%s\' does not exist in storage \'%s\'", id, res_name, storage->name.c_str());
    return nullptr; 
  }

  return map[id];
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

static void convert_from_nbr(ResourceStorage* storage, const NBRModel* nbr, Model* model) {
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
  
    texture_ids.push_back(resource_storage_push_texture(storage, desc));
  }

  // Convert the material 
  for(sizei i = 0; i < nbr->materials_count; i++) {
    // Load the diffuse map
    ResourceID diffuse_id  = texture_ids[nbr->materials[i].diffuse_index];

    // The specular map can be invalid (depicted as -1) so we need 
    // to check for that.
    ResourceID specular_id = nbr->materials[i].specular_index == -1 ? INVALID_RESOURCE : texture_ids[nbr->materials[i].specular_index];

    // Create a new material 
    ResourceID mat_id = resource_storage_push_material(storage, diffuse_id, specular_id);
    Material* mat     = resource_storage_get_material(storage, mat_id);

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
    ResourceID vert_buff_id = resource_storage_push_buffer(storage, buff_desc);
    
    // Create a index buffer
    buff_desc = {
      .data  = (void*)nbr->meshes[i].indices,
      .size  = nbr->meshes[i].indices_count * sizeof(u32), 
      .type  = GFX_BUFFER_INDEX, 
      .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
    };
    ResourceID idx_buff_id = resource_storage_push_buffer(storage, buff_desc);
    
    // Create a new mesh 
    ResourceID mesh_id = resource_storage_push_mesh(storage, vert_buff_id, (VertexType)nbr->meshes[i].vertex_type, idx_buff_id, nbr->meshes[i].indices_count);
    
    Mesh* mesh         = storage->meshes[mesh_id];
    u8 material_index  = nbr->meshes[i].material_index; 

    // Add a new index
    model->material_indices.push_back(material_index);
   
    // Add the new mesh
    model->meshes.push_back(mesh);
  }
}

/// Private functions 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Resource manager functions

void resource_manager_init() {
  const GfxContext* gfx = renderer_get_context();
  NIKOLA_ASSERT(gfx, "Invalid graphics context passed to the resource manager");

  s_manager.gfx_context     = (GfxContext*)gfx;
  s_manager.cached_storage  = resource_storage_create("cache", "resource_cache/");

  const RendererDefaults render_defaults = renderer_get_defaults();

  //
  // @FIX (Resource/Renderer): Perhaps there is a better way than this to add 
  // the default resources to the resource cache.
  // 

  // Add the default matrices buffer
  s_manager.cached_storage->buffers[generate_id()] = (GfxBuffer*)render_defaults.matrices_buffer;
  
  // Add the default white texture 
  s_manager.cached_storage->textures[generate_id()] = (GfxTexture*)render_defaults.texture;

  NIKOLA_LOG_INFO("Successfully initialized the resource manager");
}

void resource_manager_shutdown() {
  resource_storage_destroy(s_manager.cached_storage);
  
  // Get rid of any remaining storages
  for(auto& [key, value] : s_manager.storages) {
    resource_storage_destroy(value);
  }
  s_manager.storages.clear();
  
  NIKOLA_LOG_INFO("Successfully shutdown the resource manager");
}

const ResourceStorage* resource_manager_cache() {
  return s_manager.cached_storage;
}

/// Resource manager functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Resource storage functions

ResourceStorage* resource_storage_create(const String& name, const FilePath& parent_dir) {
  ResourceStorage* res = new ResourceStorage; //memory_allocate(sizeof(ResourceStorage));

  res->name                     = name; 
  res->parent_dir               = parent_dir;
  s_manager.storages[res->name] = res; 

  NIKOLA_LOG_INFO("Successfully created a resource storage \'%s\'", res->name.c_str());
  return res;
}

void resource_storage_clear(ResourceStorage* storage) {
  NIKOLA_ASSERT(storage, "Cannot clear an invalid storage");
  
  storage->buffers.clear();
  storage->textures.clear();
  storage->cubemaps.clear();
  storage->shaders.clear();

  storage->meshes.clear();
  storage->materials.clear();
  storage->skyboxes.clear();
  storage->models.clear();
  storage->fonts.clear();
  
  NIKOLA_LOG_INFO("Resource storage \'%s\' was successfully cleared", storage->name.c_str());
}

void resource_storage_destroy(ResourceStorage* storage) {
  if(!storage) {
    return;
  }

  String storage_name = storage->name; // @FIX (String): Copying around strings? Great.

  // Destroy core resources
  DESTROY_CORE_RESOURCE_MAP(storage, buffers, gfx_buffer_destroy);
  DESTROY_CORE_RESOURCE_MAP(storage, textures, gfx_texture_destroy);
  DESTROY_CORE_RESOURCE_MAP(storage, cubemaps, gfx_cubemap_destroy);
  DESTROY_CORE_RESOURCE_MAP(storage, shaders, gfx_shader_destroy);

  // Destroy compound resources
  DESTROY_COMP_RESOURCE_MAP(storage, meshes);
  DESTROY_COMP_RESOURCE_MAP(storage, materials);
  DESTROY_COMP_RESOURCE_MAP(storage, skyboxes);
  DESTROY_COMP_RESOURCE_MAP(storage, models);
  DESTROY_COMP_RESOURCE_MAP(storage, fonts);

  s_manager.storages.erase(storage->name);
  delete storage;
  
  NIKOLA_LOG_INFO("Resource storage \'%s\' was successfully destroyed", storage_name.c_str());
}

ResourceID resource_storage_push_buffer(ResourceStorage* storage, const GfxBufferDesc& buff_desc) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");

  ResourceID id        = generate_id();
  storage->buffers[id] = gfx_buffer_create(s_manager.gfx_context, buff_desc);
 
  NIKOLA_LOG_INFO("Storage \'%s\' pushed buffer:", storage->name.c_str());
  NIKOLA_LOG_INFO("     Size = %zu", buff_desc.size);
  NIKOLA_LOG_INFO("     Type = %s", buffer_type_str(buff_desc.type));
  return id;
}

ResourceID resource_storage_push_texture(ResourceStorage* storage, const GfxTextureDesc& desc) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");

  ResourceID id         = generate_id();
  storage->textures[id] = gfx_texture_create(s_manager.gfx_context, desc);
  
  NIKOLA_LOG_INFO("Storage \'%s\' pushed texture:", storage->name.c_str());
  NIKOLA_LOG_INFO("     Size = %i X %i", desc.width, desc.height);
  NIKOLA_LOG_INFO("     Type = %s", texture_type_str(desc.type));
  return id;
}

ResourceID resource_storage_push_texture(ResourceStorage* storage, 
                                         const FilePath& nbr_path,
                                         const GfxTextureFormat format, 
                                         const GfxTextureFilter filter, 
                                         const GfxTextureWrap wrap) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
 
  // Load the NBR file
  NBRFile nbr;
  nbr_file_load(&nbr, filepath_append(storage->parent_dir, nbr_path));

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
  ResourceID id         = generate_id();
  storage->textures[id] = gfx_texture_create(s_manager.gfx_context, tex_desc);

  // Remember to close the NBR
  nbr_file_unload(nbr);

  // New texture added!
  NIKOLA_LOG_INFO("Storage \'%s\' pushed texture:", storage->name.c_str());
  NIKOLA_LOG_INFO("     Size = %i X %i", tex_desc.width, tex_desc.height);
  NIKOLA_LOG_INFO("     Type = %s", texture_type_str(tex_desc.type));
  NIKOLA_LOG_INFO("     Path = %s", nbr_path.c_str());
  return id;
}

ResourceID resource_storage_push_cubemap(ResourceStorage* storage, const GfxCubemapDesc& cubemap_desc) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");

  ResourceID id         = generate_id();
  storage->cubemaps[id] = gfx_cubemap_create(s_manager.gfx_context, cubemap_desc);
  
  NIKOLA_LOG_INFO("Storage \'%s\' pushed cubemap:", storage->name.c_str());
  NIKOLA_LOG_INFO("     Size  = %i X %i", cubemap_desc.width, cubemap_desc.height);
  NIKOLA_LOG_INFO("     Faces = %i", cubemap_desc.faces_count);
  return id;
}

ResourceID resource_storage_push_cubemap(ResourceStorage* storage, 
                                         const FilePath& nbr_path,
                                         const GfxTextureFormat format, 
                                         const GfxTextureFilter filter, 
                                         const GfxTextureWrap wrap) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");

  // Load the NBR file
  NBRFile nbr;
  nbr_file_load(&nbr, filepath_append(storage->parent_dir, nbr_path));

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
  ResourceID id         = generate_id();
  storage->cubemaps[id] = gfx_cubemap_create(s_manager.gfx_context, cube_desc);

  // Remember to close the NBR
  nbr_file_unload(nbr);

  // New cubemap added!
  NIKOLA_LOG_INFO("Storage \'%s\' pushed cubemap:", storage->name.c_str());
  NIKOLA_LOG_INFO("     Size  = %i X %i", cube_desc.width, cube_desc.height);
  NIKOLA_LOG_INFO("     Faces = %i", cube_desc.faces_count);
  NIKOLA_LOG_INFO("     Path  = %s", nbr_path.c_str());
  return id;
}

ResourceID resource_storage_push_shader(ResourceStorage* storage, const GfxShaderDesc& shader_desc) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");

  ResourceID id        = generate_id();
  storage->shaders[id] = gfx_shader_create(s_manager.gfx_context, shader_desc);

  NIKOLA_LOG_INFO("Storage \'%s\' pushed shader:", storage->name.c_str());
  NIKOLA_LOG_INFO("     Vertex source length = %zu", strlen(shader_desc.vertex_source));
  NIKOLA_LOG_INFO("     Pixel source length  = %zu", strlen(shader_desc.pixel_source));
  return id;
}

ResourceID resource_storage_push_shader(ResourceStorage* storage, const FilePath& nbr_path) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  
  // Load the NBR file
  NBRFile nbr;
  nbr_file_load(&nbr, filepath_append(storage->parent_dir, nbr_path));
  
  // Make sure it is the correct resource type
  NIKOLA_ASSERT((nbr.resource_type == RESOURCE_TYPE_SHADER), "Expected RESOURCE_TYPE_SHADER");

  // Convert the NBR format to a valid shader
  NBRShader* nbr_shader  = (NBRShader*)nbr.body_data;
  GfxShaderDesc shader_desc = {
    .vertex_source = nbr_shader->vertex_source,
    .pixel_source  = nbr_shader->pixel_source,
  };

  // Create the shader
  ResourceID id = resource_storage_push_shader(storage, shader_desc);

  // Remember to close the NBR
  nbr_file_unload(nbr);

  // New shader added!
  NIKOLA_LOG_INFO("     Path = %s", nbr_path.c_str());
  return id;
}

ResourceID resource_storage_push_mesh(ResourceStorage* storage, 
                                      const ResourceID& vertex_buffer_id, 
                                      const VertexType vertex_type, 
                                      const ResourceID& index_buffer_id, 
                                      const sizei indices_count) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");

  // Allocate the mesh
  Mesh* mesh = new Mesh{};

  // Use the loader to set up the mesh
  mesh_loader_load(storage, mesh, vertex_buffer_id, vertex_type, index_buffer_id, indices_count);

  // Create the pipeline
  mesh->pipe = gfx_pipeline_create(s_manager.gfx_context, mesh->pipe_desc);

  // Create the mesh
  mesh->storage_ref   = storage; 
  ResourceID id       = generate_id();
  storage->meshes[id] = mesh;

  // New mesh added!
  NIKOLA_LOG_INFO("Storage \'%s\' pushed mesh:", storage->name.c_str());
  NIKOLA_LOG_INFO("     Vertex type  = %s", vertex_type_str(vertex_type));
  NIKOLA_LOG_INFO("     Vertices     = %zu", mesh->pipe_desc.vertices_count);
  NIKOLA_LOG_INFO("     Indices      = %zu", indices_count);
  return id;
}

ResourceID resource_storage_push_mesh(ResourceStorage* storage, const MeshType type) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");

  // Allocate the mesh
  Mesh* mesh = new Mesh{};

  // Use the loader to set up the mesh
  mesh_loader_load(storage, mesh, type);

  // Create the pipeline
  mesh->pipe = gfx_pipeline_create(s_manager.gfx_context, mesh->pipe_desc);

  // Create mesh
  mesh->storage_ref   = storage; 
  ResourceID id       = generate_id();
  storage->meshes[id] = mesh;
 
  // New mesh added!
  NIKOLA_LOG_INFO("Storage \'%s\' pushed mesh:", storage->name.c_str());
  NIKOLA_LOG_INFO("     Mesh type  = %s", mesh_type_str(type));
  NIKOLA_LOG_INFO("     Vertices   = %zu", mesh->pipe_desc.vertices_count);
  NIKOLA_LOG_INFO("     Indices    = %zu", mesh->pipe_desc.indices_count);
  return id;
}

ResourceID resource_storage_push_material(ResourceStorage* storage,
                                          const ResourceID& diffuse_id, 
                                          const ResourceID& specular_id, 
                                          const ResourceID& shader_id) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  NIKOLA_ASSERT((diffuse_id != INVALID_RESOURCE), "Cannot load a material with an invalid diffuse texture ID");
  
  // Allocate the material
  Material* material = new Material{};

  // Use the loader to set up the material
  material_loader_load(storage, material, diffuse_id, specular_id, shader_id);

  // Create material
  material->storage_ref  = storage; 
  ResourceID id          = generate_id();
  storage->materials[id] = material;

  // New material added
  NIKOLA_LOG_INFO("Storage \'%s\' pushed material:", storage->name.c_str());
  NIKOLA_LOG_INFO("     Uniforms count = \'%zu\'", material->uniform_locations.size());
  NIKOLA_LOG_INFO("     Ambient color  = \'%s\'", vec3_to_string(material->ambient_color).c_str());
  NIKOLA_LOG_INFO("     Diffuse color  = \'%s\'", vec3_to_string(material->diffuse_color).c_str());
  NIKOLA_LOG_INFO("     Specular color = \'%s\'", vec3_to_string(material->specular_color).c_str());
  return id;
}

ResourceID resource_storage_push_skybox(ResourceStorage* storage, const ResourceID& cubemap_id) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");

  // Allocate the skybox
  Skybox* skybox = new Skybox{};
  
  // Use the loader to set up the skybox
  skybox_loader_load(storage, skybox, cubemap_id);

  // Create the pipeline 
  skybox->pipe = gfx_pipeline_create(s_manager.gfx_context, skybox->pipe_desc);

  // Create skybox
  skybox->storage_ref   = storage; 
  ResourceID id         = generate_id();
  storage->skyboxes[id] = skybox;

  // New skybox added!
  NIKOLA_LOG_INFO("Storage \'%s\' pushed skybox:", storage->name.c_str());
  NIKOLA_LOG_INFO("     Vertices = %zu", skybox->pipe_desc.vertices_count);
  NIKOLA_LOG_INFO("     Indices  = %zu", skybox->pipe_desc.indices_count);
  return id;
}

ResourceID resource_storage_push_model(ResourceStorage* storage, const FilePath& nbr_path) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  
  // Load the NBR file
  NBRFile nbr;
  nbr_file_load(&nbr, filepath_append(storage->parent_dir, nbr_path));

  // Allocate the model
  Model* model = new Model{};
  
  // Convert the NBR format to a valid model
  NBRModel* nbr_model = (NBRModel*)nbr.body_data; 
  convert_from_nbr(storage, nbr_model, model);

  // New model added!
  model->storage_ref  = storage; 
  ResourceID id       = generate_id();
  storage->models[id] = model;

  // Remember to close the NBR
  nbr_file_unload(nbr);

  NIKOLA_LOG_INFO("Storage \'%s\' pushed model:", storage->name.c_str());
  NIKOLA_LOG_INFO("     Meshes    = %zu", model->meshes.size());
  NIKOLA_LOG_INFO("     Materials = %zu", model->materials.size());
  NIKOLA_LOG_INFO("     Textures  = %i", nbr_model->textures_count);
  NIKOLA_LOG_INFO("     Path      = %s", nbr_path.c_str());
  return id;
}

GfxBuffer* resource_storage_get_buffer(ResourceStorage* storage, const ResourceID& id) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  NIKOLA_ASSERT((id != INVALID_RESOURCE), "Cannot retrieve an invalid resource");

  return get_resource(storage, storage->buffers, id, "GfxBuffer");
}

GfxTexture* resource_storage_get_texture(ResourceStorage* storage, const ResourceID& id) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  NIKOLA_ASSERT((id != INVALID_RESOURCE), "Cannot retrieve an invalid resource");

  return get_resource(storage, storage->textures, id, "GfxTexture");
}

GfxCubemap* resource_storage_get_cubemap(ResourceStorage* storage, const ResourceID& id) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  NIKOLA_ASSERT((id != INVALID_RESOURCE), "Cannot retrieve an invalid resource");

  return get_resource(storage, storage->cubemaps, id, "GfxCubemap");
}

GfxShader* resource_storage_get_shader(ResourceStorage* storage, const ResourceID& id) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  NIKOLA_ASSERT((id != INVALID_RESOURCE), "Cannot retrieve an invalid resource");

  return get_resource(storage, storage->shaders, id, "GfxShader");
}

Mesh* resource_storage_get_mesh(ResourceStorage* storage, const ResourceID& id) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  NIKOLA_ASSERT((id != INVALID_RESOURCE), "Cannot retrieve an invalid resource");

  return get_resource(storage, storage->meshes, id, "Mesh");
}

Material* resource_storage_get_material(ResourceStorage* storage, const ResourceID& id) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  NIKOLA_ASSERT((id != INVALID_RESOURCE), "Cannot retrieve an invalid resource");

  return get_resource(storage, storage->materials, id, "Material");
}

Model* resource_storage_get_model(ResourceStorage* storage, const ResourceID& id) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  NIKOLA_ASSERT((id != INVALID_RESOURCE), "Cannot retrieve an invalid resource");

  return get_resource(storage, storage->models, id, "Model");
}

Skybox* resource_storage_get_skybox(ResourceStorage* storage, const ResourceID& id) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  NIKOLA_ASSERT((id != INVALID_RESOURCE), "Cannot retrieve an invalid resource");

  return get_resource(storage, storage->skyboxes, id, "Skybox");
}

Font* resource_storage_get_font(ResourceStorage* storage, const ResourceID& id) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  NIKOLA_ASSERT((id != INVALID_RESOURCE), "Cannot retrieve an invalid resource");

  return get_resource(storage, storage->fonts, id, "Font");
}

/// Resource storage functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
