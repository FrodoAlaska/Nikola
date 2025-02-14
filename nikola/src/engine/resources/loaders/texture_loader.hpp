#pragma once

#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Texture loader functions

void texture_loader_load(GfxTextureDesc* desc, 
                         NBRTexture* nbr,
                         const GfxTextureFormat format = GFX_TEXTURE_FORMAT_RGBA8, 
                         const GfxTextureFilter filter = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
                         const GfxTextureWrap wrap     = GFX_TEXTURE_WRAP_CLAMP);

/// Texture loader functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
