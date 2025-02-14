#include "texture_loader.hpp"

#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Texture loader functions

void texture_loader_load(GfxTextureDesc* desc, NBRTexture* nbr, const GfxTextureFormat format, const GfxTextureFilter filter, const GfxTextureWrap wrap) {
  NIKOLA_ASSERT(desc, "Invalid GfxTextureDesc passed to texture loader function");
  NIKOLA_ASSERT(nbr, "Invalid NBRTexture passed to texture loader function");

  desc->width     = nbr->width; 
  desc->height    = nbr->height; 
  desc->depth     = 0; 
  desc->mips      = 1; 
  desc->type      = GFX_TEXTURE_2D; 
  desc->format    = format; 
  desc->filter    = filter; 
  desc->wrap_mode = wrap;
  desc->data      = nbr->pixels;
}

/// Texture loader functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
