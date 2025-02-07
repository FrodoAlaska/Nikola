#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

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

#define DESTROY_RESOURCE_MAP(storage, map, clear_func) { \
  for(auto& [key, value] : storage->map) {               \
    clear_func(value);                                   \
  }                                                      \
}

/// Macros (Unfortunately)
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

ResourceID generate_id() {
  return random_u64(); // @TODO: Make something more complex than this
}

template<typename T>
static T get_resource(ResourceStorage* storage, HashMap<ResourceID, T>& map, const ResourceID& id) {
  // We cannot return a non-existing resource neither will 
  // we add it to the storage 
  if(map.find(id) == map.end()) {
    NIKOLA_LOG_ERROR("Resource id \'%i\' does not exist in storage \'%s\'", id, storage->name.c_str());
    return nullptr; 
  }

  return map[id];
}

/// Private functions 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Resource manager functions

void resource_manager_init() {
  const GfxContext* gfx = renderer_get_context();
  NIKOLA_ASSERT(gfx, "Invalid graphics context passed to the resource manager");

  s_manager.gfx_context    = (GfxContext*)gfx;
  s_manager.cached_storage = resource_storage_create("cache", "resource_cache/");

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
  
  NIKOLA_LOG_INFO("Resource storage \'%s\' was successfully destroyed", storage->name.c_str());
}

void resource_storage_destroy(ResourceStorage* storage) {
  if(!storage) {
    return;
  }

  // Get rid of every resource in the storage
  DESTROY_RESOURCE_MAP(storage, buffers, gfx_buffer_destroy);
  DESTROY_RESOURCE_MAP(storage, textures, gfx_texture_destroy);
  DESTROY_RESOURCE_MAP(storage, cubemaps, gfx_cubemap_destroy);
  DESTROY_RESOURCE_MAP(storage, shaders, gfx_shader_destroy);

  s_manager.storages.erase(storage->name);
  delete storage; //memory_free(storage);
}

ResourceID resource_storage_push(ResourceStorage* storage, const GfxBufferDesc& buff_desc) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");

  ResourceID id        = generate_id();
  storage->buffers[id] = gfx_buffer_create(s_manager.gfx_context, buff_desc);
  
  return id;
}

ResourceID resource_storage_push(ResourceStorage* storage, const GfxTextureDesc& tex_desc) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");

  ResourceID id         = generate_id();
  storage->textures[id] = gfx_texture_create(s_manager.gfx_context, tex_desc);
  
  return id;
}

ResourceID resource_storage_push(ResourceStorage* storage, const GfxCubemapDesc& cubemap_desc) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");

  ResourceID id         = generate_id();
  storage->cubemaps[id] = gfx_cubemap_create(s_manager.gfx_context, cubemap_desc);
  
  return id;
}

ResourceID resource_storage_push(ResourceStorage* storage, const String& shader_src) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");

  ResourceID id        = generate_id();
  storage->shaders[id] = gfx_shader_create(s_manager.gfx_context, shader_src.c_str());
  
  return id;
}

ResourceID resource_storage_push(ResourceStorage* storage, const MeshLoader& loader) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");

  Mesh* mesh = (Mesh*)memory_allocate(sizeof(Mesh));
  memory_zero(mesh, sizeof(Mesh));

  mesh->vertex_buffer = loader.vertex_buffer; 
  mesh->index_buffer  = loader.index_buffer;
  mesh->pipe_desc     = loader.pipe_desc;
  mesh->pipe          = gfx_pipeline_create(s_manager.gfx_context, mesh->pipe_desc);
  mesh->resource_ref  = storage;

  ResourceID id       = generate_id();
  storage->meshes[id] = mesh;

  return id;
}

ResourceID resource_storage_push(ResourceStorage* storage, const MaterialLoader& loader) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");

  Material* material = (Material*)memory_allocate(sizeof(Material));
  memory_zero(material, sizeof(Material));

  material->diffuse_map  = loader.diffuse_map;
  material->specular_map = loader.specular_map;
  material->shader       = loader.shader;
  material->resource_ref = storage;

  ResourceID id          = generate_id();
  storage->materials[id] = material;

  return id;
}

ResourceID resource_storage_push(ResourceStorage* storage, const SkyboxLoader& loader) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");

  Skybox* skybox = (Skybox*)memory_allocate(sizeof(Skybox));
  memory_zero(skybox, sizeof(Skybox));

  skybox->vertex_buffer = loader.vertex_buffer;
  skybox->cubemap       = loader.cubemap;
  skybox->pipe_desc     = loader.pipe_desc;
  skybox->pipe          = gfx_pipeline_create(s_manager.gfx_context, skybox->pipe_desc);
  skybox->resource_ref  = storage;

  ResourceID id         = generate_id();
  storage->skyboxes[id] = skybox;

  return id;
}

GfxBuffer* resource_storage_get_buffer(ResourceStorage* storage, const ResourceID& id) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  NIKOLA_ASSERT((id != INVALID_RESOURCE), "Cannot retrieve an invalid resource");

  return get_resource(storage, storage->buffers, id);
}

GfxTexture* resource_storage_get_texture(ResourceStorage* storage, const ResourceID& id) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  NIKOLA_ASSERT((id != INVALID_RESOURCE), "Cannot retrieve an invalid resource");

  return get_resource(storage, storage->textures, id);
}

GfxCubemap* resource_storage_get_cubemap(ResourceStorage* storage, const ResourceID& id) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  NIKOLA_ASSERT((id != INVALID_RESOURCE), "Cannot retrieve an invalid resource");

  return get_resource(storage, storage->cubemaps, id);
}

GfxShader* resource_storage_get_shader(ResourceStorage* storage, const ResourceID& id) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  NIKOLA_ASSERT((id != INVALID_RESOURCE), "Cannot retrieve an invalid resource");

  return get_resource(storage, storage->shaders, id);
}

Mesh* resource_storage_get_mesh(ResourceStorage* storage, const ResourceID& id) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  NIKOLA_ASSERT((id != INVALID_RESOURCE), "Cannot retrieve an invalid resource");

  return get_resource(storage, storage->meshes, id);
}

Material* resource_storage_get_material(ResourceStorage* storage, const ResourceID& id) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  NIKOLA_ASSERT((id != INVALID_RESOURCE), "Cannot retrieve an invalid resource");

  return get_resource(storage, storage->materials, id);
}

Model* resource_storage_get_model(ResourceStorage* storage, const ResourceID& id) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  NIKOLA_ASSERT((id != INVALID_RESOURCE), "Cannot retrieve an invalid resource");

  return get_resource(storage, storage->models, id);
}

Skybox* resource_storage_get_skybox(ResourceStorage* storage, const ResourceID& id) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  NIKOLA_ASSERT((id != INVALID_RESOURCE), "Cannot retrieve an invalid resource");

  return get_resource(storage, storage->skyboxes, id);
}

Font* resource_storage_get_font(ResourceStorage* storage, const ResourceID& id) {
  NIKOLA_ASSERT(storage, "Cannot push a resource to an invalid storage");
  NIKOLA_ASSERT((id != INVALID_RESOURCE), "Cannot retrieve an invalid resource");

  return get_resource(storage, storage->fonts, id);
}

/// Resource storage functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
