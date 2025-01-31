#include "nikol/nikol_core.hpp"
#include "nikol/nikol_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikol { // Start of nikol

/// ----------------------------------------------------------------------
/// StorageManager 
struct StorageManager {
  HashMap<ResourceUUID, ResourceStorage*> storages;

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

  HashMap<ResourceUUID, GfxBuffer*> buffers;
  HashMap<ResourceUUID, GfxTexture*> textures;
  HashMap<ResourceUUID, GfxCubemap*> cubemaps;
  HashMap<ResourceUUID, GfxShader*> shaders;
  
  HashMap<ResourceUUID, Mesh*> meshes;
  HashMap<ResourceUUID, Material*> materials;
  HashMap<ResourceUUID, Skybox*> skyboxes;
  HashMap<ResourceUUID, Model*> models;
};
/// ResourceStorage 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Macros (Unfortunately)

#define DESTROY_RESOURCE_MAP(storage, map, clear_func) { \
  for(auto& [key, value] : storage->map) {               \
    clear_func(value);                                   \
  }                                                      \
  storage->map.clear();                                  \
}

/// Macros (Unfortunately)
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

template<typename T>
ResourceUUID check_resource_exists(ResourceStorage* storage, HashMap<ResourceUUID, T>& map, const String& sid) {
  ResourceID id(sid);

  // Make sure that every resource that is pushed to the storage is unique
  if(map.find(id.uuid) != map.end()) {
    NIKOL_LOG_WARN("Resource \'%s\' already exists in storage \'%s\'", sid.c_str(), storage->name.c_str());
    return id.uuid; 
  }

  return id.uuid;
}

template<typename T>
static T get_resource(ResourceStorage* storage, HashMap<ResourceUUID, T>& map, const ResourceUUID& id) {
  // We cannot return a non-existing resource neither will 
  // we add it to the storage 
  if(map.find(id) == map.end()) {
    NIKOL_LOG_ERROR("Resource id \'%i\' does not exist in storage \'%s\'", id, storage->name.c_str());
    return nullptr; 
  }

  return map[id];
}

/// Private functions 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Resource storage functions

void resource_manager_init(GfxContext* gfx) {
  NIKOL_ASSERT(gfx, "Invalid graphics context passed to the resource manager");

  s_manager.gfx_context    = gfx;
  s_manager.cached_storage = resource_storage_create("cache", "resource_cache/");
}

void resource_manager_destroy() {
  resource_storage_destroy(s_manager.cached_storage);
  
  // Get rid of any remaining storages
  for(auto& [key, value] : s_manager.storages) {
    resource_storage_destroy(value);
  }
  s_manager.storages.clear();
}

const ResourceStorage* resource_manager_cache() {
  return s_manager.cached_storage;
}

void resource_manager_clear_cache() {
  resource_storage_clear(s_manager.cached_storage); 
}

/// Resource storage functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Resource storage functions

ResourceStorage* resource_storage_create(const String& name, const FilePath& parent_dir) {
  ResourceStorage* res = (ResourceStorage*)memory_allocate(sizeof(ResourceStorage));
  memory_zero(res, sizeof(ResourceStorage));

  res->name       = name; 
  res->parent_dir = parent_dir;
  s_manager.storages[ResourceID(name).uuid] = res; 

  return res;
}

void resource_storage_clear(ResourceStorage* storage) {
  NIKOL_ASSERT(storage, "Cannot clear an invalid storage");
  
  storage->buffers.clear();
  storage->textures.clear();
  storage->cubemaps.clear();
  storage->shaders.clear();
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

  s_manager.storages.erase(ResourceID(storage->name).uuid);
  memory_free(storage);
}

ResourceUUID resource_storage_push_buffer(ResourceStorage* storage, const String& sid, const GfxBufferDesc& buff_desc) {
  NIKOL_ASSERT(storage, "Cannot push a resource to an invalid storage");

  ResourceUUID id      = check_resource_exists(storage, storage->buffers, sid);
  storage->buffers[id] = gfx_buffer_create(s_manager.gfx_context, buff_desc);
  
  return id;
}

ResourceUUID resource_storage_push_texture(ResourceStorage* storage, const String& sid, const GfxTextureDesc& tex_desc) {
  NIKOL_ASSERT(storage, "Cannot push a resource to an invalid storage");

  ResourceUUID id       = check_resource_exists(storage, storage->textures, sid);
  storage->textures[id] = gfx_texture_create(s_manager.gfx_context, tex_desc);
  
  return id;
}

ResourceUUID resource_storage_push_cubemap(ResourceStorage* storage, const String& sid, const GfxCubemapDesc& cubemap_desc) {
  NIKOL_ASSERT(storage, "Cannot push a resource to an invalid storage");

  ResourceUUID id       = check_resource_exists(storage, storage->cubemaps, sid);
  storage->cubemaps[id] = gfx_cubemap_create(s_manager.gfx_context, cubemap_desc);
  
  return id;
}

ResourceUUID resource_storage_push_shader(ResourceStorage* storage, const String& sid, const String& shader_src) {
  NIKOL_ASSERT(storage, "Cannot push a resource to an invalid storage");

  ResourceUUID id      = check_resource_exists(storage, storage->shaders, sid);
  storage->shaders[id] = gfx_shader_create(s_manager.gfx_context, shader_src.c_str());
  
  return id;
}

GfxBuffer* resource_storage_get_buffer(ResourceStorage* storage, const ResourceUUID& id) {
  NIKOL_ASSERT(storage, "Cannot push a resource to an invalid storage");
  return get_resource(storage, storage->buffers, id);
}

GfxTexture* resource_storage_get_texture(ResourceStorage* storage, const ResourceUUID& id) {
  NIKOL_ASSERT(storage, "Cannot push a resource to an invalid storage");
  return get_resource(storage, storage->textures, id);
}

GfxCubemap* resource_storage_get_cubemap(ResourceStorage* storage, const ResourceUUID& id) {
  NIKOL_ASSERT(storage, "Cannot push a resource to an invalid storage");
  return get_resource(storage, storage->cubemaps, id);
}

GfxShader* resource_storage_get_shader(ResourceStorage* storage, const ResourceUUID& id) {
  NIKOL_ASSERT(storage, "Cannot push a resource to an invalid storage");
  return get_resource(storage, storage->shaders, id);
}

/// Resource storage functions
/// ----------------------------------------------------------------------

} // End of nikol

//////////////////////////////////////////////////////////////////////////
