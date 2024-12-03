#include "../nikol_core.hpp"

//////////////////////////////////////////////////////////////////////////

#if NIKOL_GFX_CONTEXT_DX11 == 1  // DirectX11 check

namespace nikol { // Start of nikol

/// ---------------------------------------------------------------------
/// *** Graphics ***

///---------------------------------------------------------------------------------------------------------------------
/// GfxContext
struct GfxContext; 
/// GfxContext
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBuffer
struct GfxBuffer;
/// GfxBuffer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxShader
struct GfxShader;
/// GfxShader
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTexture
struct GfxTexture;
/// GfxTexture
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Context functions 

GfxContext* gfx_context_create(Window* window, const i32 flags);
void gfx_context_destroy(GfxContext* gfx);
void gfx_context_clear(GfxContext* gfx, const f32 r, const f32 g, const f32 b, const f32 a);
void gfx_context_set_flag(GfxContext* gfx, const i32 flag, const bool value);
const GfxContextFlags gfx_context_get_flags(GfxContext* gfx);
void gfx_context_draw(GfxContext* gfx, const GfxDrawCall& call); 
void gfx_context_draw_batch(GfxContext* gfx, GfxDrawCall* calls, const sizei count);

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
                               const GfxTextureFilter filter);
void gfx_texture_destroy(GfxTexture* texture);
void gfx_texture_get_size(GfxTexture* texture, i32* width, i32* height);
void gfx_texture_get_pixels(GfxTexture* texture, void* pixels);

/// Texture functions 
///---------------------------------------------------------------------------------------------------------------------

/// *** Graphics ***
/// ---------------------------------------------------------------------


} // End of nikol

#endif // DirectX11 check

//////////////////////////////////////////////////////////////////////////
