#include "material_loader.hpp"

#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Material loader functions

void material_loader_load(ResourceStorage* storage, 
                          Material* mat, 
                          const ResourceID& diffuse_id, 
                          const ResourceID& specular_id, 
                          const ResourceID& shader_id) {
  NIKOLA_ASSERT(storage, "Cannot load with an invalid ResourceStorage");
  NIKOLA_ASSERT(mat, "Invalid Material passed to material loader function");
  NIKOLA_ASSERT((diffuse_id != INVALID_RESOURCE), "Cannot load a material with an invalid diffuse texture ID");

  // Default initialize the loader
  memory_zero(mat, sizeof(Material));

  // Diffuse texture init
  mat->diffuse_map = resource_storage_get_texture(storage, diffuse_id);

  // Specular texture init
  if(specular_id != INVALID_RESOURCE) {
    mat->specular_map = resource_storage_get_texture(storage, specular_id);
  } 

  // Shader init 
  if(shader_id != INVALID_RESOURCE) {
    mat->shader = resource_storage_get_shader(storage, shader_id);
  } 
}

/// Material loader functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
