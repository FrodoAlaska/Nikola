#include "nikola/nikola_resources.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_gfx.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Material functions

void material_use(Material* mat) {
  NIKOLA_ASSERT(mat, "Invalid Material given to material_use");

  // @TODO (Renderer): Do we really need this here???

  GfxTexture* textures[2] = {
    mat->diffuse_map, 
    mat->specular_map, 
  };
  u32 textures_count = 1;
 
  // Use the specular texture (if they are valid)
  if(mat->specular_map) {
    textures_count++;
  }

  // Use all the valid textures
  gfx_texture_use(textures, textures_count);
}

/// Material functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
