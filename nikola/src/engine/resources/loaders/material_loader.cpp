#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Material loader functions

void material_loader_load(ResourceStorage* storage, 
                          MaterialLoader* loader, 
                          const ResourceID& diffuse_id, 
                          const ResourceID& specular_id, 
                          const ResourceID& shader_id) {
  NIKOLA_ASSERT(storage, "Cannot load with an invalid ResourceStorage");
  NIKOLA_ASSERT(loader, "Cannot load with an invalid loader");
  NIKOLA_ASSERT((diffuse_id != INVALID_RESOURCE), "Cannot load a material with an invalid diffuse texture ID");
  NIKOLA_ASSERT((shader_id != INVALID_RESOURCE), "Cannot load a material with an invalid shader ID");

  // Default initialize the loader
  memory_zero(loader, sizeof(MaterialLoader));

  // Diffuse texture init
  loader->diffuse_map = diffuse_id;

  // Specular texture init
  loader->specular_map = diffuse_id;

  // Shader init 
  loader->shader = shader_id;
  
  // Invalidate all of the uniform buffers as a starter
  for(sizei i = 0; i < MATERIAL_UNIFORM_BUFFERS_MAX; i++) {
    loader->uniform_buffers[i] = INVALID_RESOURCE;
  }
}

void material_loader_attach_uniform(ResourceStorage* storage, MaterialLoader& loader, const sizei index, const ResourceID& buffer_id) {
  NIKOLA_ASSERT(storage, "Cannot load with an invalid ResourceStorage");
  NIKOLA_ASSERT((buffer_id != INVALID_RESOURCE), "Cannot attach an invalid uniform buffer id to a material");
  NIKOLA_ASSERT(((index >= MATERIAL_MATRICES_BUFFER_INDEX) && (index <= MATERIAL_LIGHTING_BUFFER_INDEX)), "Invalid index passed as uniform buffer index of a material");

  loader.uniform_buffers[index] = buffer_id;
}

/// Material loader functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
