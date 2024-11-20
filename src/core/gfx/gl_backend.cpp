#include "nikol_core.h"

//////////////////////////////////////////////////////////////////////////

#if NIKOL_GFX_CONTEXT_OPENGL == 1  // OpenGL check

#include <glad/glad.h>

namespace nikol { // Start of nikol

/// ---------------------------------------------------------------------
/// *** Graphics ***

///---------------------------------------------------------------------------------------------------------------------
/// GfxContext
struct GfxContext {
  GfxContextFlags flags;
};
/// GfxContext
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBuffer
struct GfxBuffer {
  u32 id;

  void* data;
  sizei data_size;
};
/// GfxBuffer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxShader
struct GfxShader {
  u32 id, vert_id, frag_id;
};
/// GfxShader
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTexture
struct GfxTexture {
  u32 id;

  i32 width, height;
  i32 channels;
  void* pixels; 
};
/// GfxTexture
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions 

static void set_gfx_flags(GfxContext* gfx) {

}

/// Private functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Context functions 

GfxContext* gfx_context_create(Window* window, const i32 flags) {
  GfxContext* gfx = (GfxContext*)memory_allocate(sizeof(GfxContext));
 
  gfx->flags = (GfxContextFlags)flags;
  set_gfx_flags(gfx);

  NIKOL_LOG_INFO("An OpenGL 4.6 core graphics context was successfully created");
  return gfx;
}

void gfx_context_destroy(GfxContext* gfx) {
  if(!gfx) {
    return;
  }

  NIKOL_LOG_INFO("An OpenGL 4.6 core graphics context was successfully destroyed");
  memory_free(gfx);
}

void gfx_context_clear(GfxContext* gfx, const f32 r, const f32 g, const f32 b, const f32 a) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
}

void gfx_context_set_flag(GfxContext* gfx, const i32 flag, const bool value) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
}

const GfxContextFlags gfx_context_get_flags(GfxContext* gfx) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  return gfx->flags;
}

void gfx_context_draw(GfxContext* gfx, const GfxDrawCall& call) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
}

void gfx_context_draw_batch(GfxContext* gfx, GfxDrawCall* calls, const sizei count) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
}

/// Context functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Buffer functions 

GfxBuffer* gfx_buffer_create(GfxContext* gfx, const GfxBufferType type, const GfxBufferMode mode, void* data, const sizei data_size);

void gfx_buffer_set_layout(GfxContext* gfx, GfxBuffer* buff, GfxBufferLayout* layout, const sizei layout_count);

void gfx_buffer_destroy(GfxContext* gfx, GfxBuffer* buff);

/// Buffer functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Shader functions 

GfxShader* gfx_shader_create(const i8* src);

void gfx_shader_destroy(GfxShader* shader);

/// Shader functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Texture functions 

GfxTexture* gfx_texture_create(void* data, 
                               const sizei channels_count, 
                               const i32 width, 
                               const i32 height,
                               const GfxTextureFromat format, 
                               const GfxTextureFilter filter) {
  GfxTexture* texture = (GfxTexture*)memory_allocate(sizeof(GfxTexture)); 

  return texture;
}

void gfx_texture_destroy(GfxTexture* texture) {
  if(!texture) {
    return;
  }
    
  memory_free(texture);
}

void gfx_texture_get_size(GfxTexture* texture, i32* width, i32* height) {
  NIKOL_ASSERT(texture, "Could not retrieve the size of an invalid texture");
  
  *width  = texture->width;
  *height = texture->height; 
}

void gfx_texture_get_pixels(GfxTexture* texture, void* pixels) {
  NIKOL_ASSERT(texture, "Could not retrieve the size of an invalid texture");
  pixels = texture->pixels;
}

/// Texture functions 
///---------------------------------------------------------------------------------------------------------------------

/// *** Graphics ***
/// ---------------------------------------------------------------------


} // End of nikol

#endif // OpenGL check

//////////////////////////////////////////////////////////////////////////
