#include "cubemap_loader.hpp"

#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Cubemap loader functions

void cubemap_loader_load(GfxCubemapDesc* desc, 
                         NBRCubemap* nbr,
                         const GfxTextureFormat format, 
                         const GfxTextureFilter filter, 
                         const GfxTextureWrap wrap) {
  NIKOLA_ASSERT(desc, "Invalid GfxCubemapDesc passed to texture loader function");
  NIKOLA_ASSERT(nbr, "Invalid NBRCubemap passed to texture loader function");
  
  desc->width       = nbr->width; 
  desc->height      = nbr->height; 
  desc->mips        = 1; 
  desc->format      = format; 
  desc->filter      = filter; 
  desc->wrap_mode   = wrap; 
  desc->faces_count = nbr->faces_count; 

  for(sizei i = 0; i < desc->faces_count; i++) {
    desc->data[i] = nbr->pixels[i];
  }
}

/// Cubemap loader functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
