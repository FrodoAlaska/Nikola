#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

#include <stb/stb_image.h>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Texture loader functions

void texture_loader_load(GfxTextureDesc* desc, const FilePath& path, const GfxTextureFormat format, const GfxTextureFilter filter, const GfxTextureWrap wrap) {
  i32 width, height, channels;
  desc->data = stbi_load(path.c_str(), &width, &height, &channels, 4);
  
  if(!desc->data) {
    NIKOLA_LOG_ERROR("Could not load texture at \'%s\'", path.c_str());
    return;
  }

  desc->width     = width; 
  desc->height    = height; 
  desc->depth     = 0; 
  desc->mips      = 1; 
  desc->type      = GFX_TEXTURE_2D; 
  desc->format    = format; 
  desc->filter    = filter; 
  desc->wrap_mode = wrap;
}

void texture_loader_unload(GfxTextureDesc& desc) {
  stbi_image_free(desc.data);
}

/// Texture loader functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
