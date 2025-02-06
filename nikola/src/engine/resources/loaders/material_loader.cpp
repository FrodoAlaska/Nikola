#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Material loader functions

void material_loader_load(ResourceStorage* storage, 
    MaterialLoader* loader, 
    const GfxTextureDesc& diffuse, 
    const GfxTextureDesc& specular, 
    const String& shader_src) {
  loader->diffuse_map  = resource_storage_push(storage, diffuse); 
  loader->specular_map = resource_storage_push(storage, specular);
  loader->shader       = resource_storage_push(storage, shader_src);
}

/// Material loader functions
/// ----------------------------------------------------------------------


} // End of nikola

//////////////////////////////////////////////////////////////////////////
