#include "nikola/nikola_resources.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_gfx.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Material functions

void material_use(Material* mat) {
  NIKOLA_ASSERT(mat, "Invalid Material given to material_use");

  GfxTexture* textures[2] = {
    mat->diffuse_map, 
    mat->specular_map, 
  };
  u32 textures_count = 0;

  // Use the diffuse texture (if they are valid)
  if(mat->diffuse_map) {
    textures_count++;
  }
 
  // Use the specular texture (if they are valid)
  if(mat->specular_map) {
    textures_count++;
  }

  // Use all the valid textures
  if(textures_count > 0) { 
    gfx_texture_use(textures, textures_count);
  }
}

/// Material functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
