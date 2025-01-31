#include "nikol/nikol_core.hpp"
#include "nikol/nikol_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikol { // Start of nikol

/// ----------------------------------------------------------------------
/// Renderer
struct Renderer {
  GfxContext* context = nullptr;
};

static Renderer s_renderer;
/// Renderer
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Renderer functions

void renderer_init(Window* window) {
  GfxContextDesc gfx_desc = {
    .window       = window,
    .states       = GFX_STATE_DEPTH | GFX_STATE_STENCIL | GFX_STATE_BLEND,
    .pixel_format = GFX_TEXTURE_FORMAT_RGBA8,
  };
  
  s_renderer.context = gfx_context_init(gfx_desc);
  NIKOL_ASSERT(s_renderer.context, "Failed to initialize the graphics context");


  NIKOL_LOG_INFO("Successfully initialized the renderer context");
}

void renderer_shutdown() {
  gfx_context_shutdown(s_renderer.context);
  NIKOL_LOG_INFO("Successfully shutdown the renderer context");
}

const GfxContext* renderer_get_context() {
  return s_renderer.context;
}

/// Renderer functions
/// ----------------------------------------------------------------------

} // End of nikol

//////////////////////////////////////////////////////////////////////////
