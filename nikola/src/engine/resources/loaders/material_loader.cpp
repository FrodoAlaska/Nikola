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
  // Push and retrive the IDs of the resources
  loader->diffuse_map  = resource_storage_push(storage, diffuse); 
  loader->specular_map = resource_storage_push(storage, specular);
  loader->shader       = resource_storage_push(storage, shader_src);

  // Make sure to attach all of the uniform buffers as well
  GfxBufferDesc mat_buff_desc = {
    .data  = nullptr,
    .size  = sizeof(Mat4),
    .type  = GFX_BUFFER_UNIFORM,
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  loader->uniform_buffers[MATERIAL_MATRICES_BUFFER_INDEX] = resource_storage_push(storage, mat_buff_desc);
  
  GfxBufferDesc light_buff_desc = {
    .data  = nullptr,
    .size  = sizeof(Mat4), // @TEMP
    .type  = GFX_BUFFER_UNIFORM,
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  loader->uniform_buffers[MATERIAL_LIGHTING_BUFFER_INDEX] = resource_storage_push(storage, light_buff_desc);
}

/// Material loader functions
/// ----------------------------------------------------------------------


} // End of nikola

//////////////////////////////////////////////////////////////////////////
