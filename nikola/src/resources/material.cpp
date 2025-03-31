#include "nikola/nikola_resources.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_gfx.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Material functions

void material_set_texture(ResourceID& mat_id, const MaterialTextureType type, const ResourceID& texture_id) {
  NIKOLA_ASSERT(RESOURCE_IS_VALID(mat_id), "Invalid Material passed");
  NIKOLA_ASSERT(RESOURCE_IS_VALID(texture_id), "Invalid texture ID passed to material");
  
  Material* mat = resources_get_material(mat_id); 

  switch(type) {
    case MATERIAL_TEXTURE_DIFFUSE:
      mat->diffuse_map = texture_id;
      break;
    case MATERIAL_TEXTURE_SPECULAR:
      mat->specular_map = texture_id;
      break;
  }
}

void material_use(ResourceID& mat_id) {
  Material* mat = resources_get_material(mat_id);

  GfxTexture* textures[2];
  u32 textures_count = 0;

  // Use the diffuse texture (if they are valid)
  if(RESOURCE_IS_VALID(mat->diffuse_map)) {
    textures[0] = resources_get_texture(mat->diffuse_map); 
    textures_count++;
  }
 
  // Use the specular texture (if they are valid)
  if(RESOURCE_IS_VALID(mat->specular_map)) {
    textures[1] = resources_get_texture(mat->specular_map); 
    textures_count++;
  }
 
  if(textures_count > 0) { 
    gfx_texture_use(textures, textures_count);
  }
}

/// Material functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
