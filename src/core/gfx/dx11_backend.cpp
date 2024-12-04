#include "../nikol_core.hpp"

//////////////////////////////////////////////////////////////////////////

#if NIKOL_GFX_CONTEXT_DX11 == 1  // DirectX11 check

namespace nikol { // Start of nikol

/// ---------------------------------------------------------------------
/// *** Graphics ***

///---------------------------------------------------------------------------------------------------------------------
/// GfxContext
struct GfxContext {
    GfxContextFlags flags;
}
/// GfxContext
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
/// GfxPipeline
struct GfxPipeline;
/// GfxPipeline
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void set_gfx_flags(GfxContext* gfx) {
  if((gfx->flags & GFX_FLAGS_DEPTH) == GFX_FLAGS_DEPTH) {
  }
  
  if((gfx->flags & GFX_FLAGS_STENCIL) == GFX_FLAGS_STENCIL) {
  }
  
  if((gfx->flags & GFX_FLAGS_BLEND) == GFX_FLAGS_BLEND) {
  }
  
  if((gfx->flags & GFX_FLAGS_MSAA) == GFX_FLAGS_MSAA) {
  }

  if((gfx->flags & GFX_FLAGS_CULL_CW) == GFX_FLAGS_CULL_CW) {
  }
  
  if((gfx->flags & GFX_FLAGS_CULL_CCW) == GFX_FLAGS_CULL_CCW) {
  }
}

static sizei get_layout_size(const GfxBufferLayout layout) {
  switch(layout) {
    case GFX_LAYOUT_FLOAT1:
      return sizeof(f32);
    case GFX_LAYOUT_FLOAT2:
      return sizeof(f32) * 2;
    case GFX_LAYOUT_FLOAT3:
      return sizeof(f32) * 3;
    case GFX_LAYOUT_FLOAT4:
      return sizeof(f32) * 4;
    case GFX_LAYOUT_INT1:
      return sizeof(i32);
    case GFX_LAYOUT_INT2:
      return sizeof(i32) * 2;
    case GFX_LAYOUT_INT3:
      return sizeof(i32) * 3;
    case GFX_LAYOUT_INT4:
      return sizeof(i32) * 4;
    case GFX_LAYOUT_UINT1:
      return sizeof(u32);
    case GFX_LAYOUT_UINT2:
      return sizeof(u32) * 2;
    case GFX_LAYOUT_UINT3:
      return sizeof(u32) * 3;
    case GFX_LAYOUT_UINT4:
      return sizeof(u32) * 4;
    default: 
      return 0;
  }
}

static sizei get_layout_type(const GfxBufferLayout layout) {
  switch(layout) {
    case GFX_LAYOUT_FLOAT1:
    case GFX_LAYOUT_FLOAT2:
    case GFX_LAYOUT_FLOAT3:
    case GFX_LAYOUT_FLOAT4:
      return GL_FLOAT;
    case GFX_LAYOUT_INT1:
    case GFX_LAYOUT_INT2:
    case GFX_LAYOUT_INT3:
    case GFX_LAYOUT_INT4:
      return GL_INT;
    case GFX_LAYOUT_UINT1:
    case GFX_LAYOUT_UINT2:
    case GFX_LAYOUT_UINT3:
    case GFX_LAYOUT_UINT4:
      return GL_UNSIGNED_INT;
    default:
      return 0;
  }
}

static sizei get_layout_count(const GfxBufferLayout layout) {
  switch(layout) {
    case GFX_LAYOUT_FLOAT1:
    case GFX_LAYOUT_INT1:
    case GFX_LAYOUT_UINT1:
      return 1;
    case GFX_LAYOUT_FLOAT2:
    case GFX_LAYOUT_INT2:
    case GFX_LAYOUT_UINT2:
      return 2;
    case GFX_LAYOUT_FLOAT3:
    case GFX_LAYOUT_INT3:
    case GFX_LAYOUT_UINT3:
      return 3;
    case GFX_LAYOUT_FLOAT4:
    case GFX_LAYOUT_INT4:
    case GFX_LAYOUT_UINT4:
      return 4;
    default:
      return 0;
  }
}

static sizei calc_stride(const GfxBufferLayout* layout, const sizei count) {
  sizei stride = 0; 

  for(sizei i = 0; i < count; i++) {
    stride += get_layout_size(layout[i]);
  }

  return stride;
}

static bool is_buffer_dynamic(const GfxBufferUsage& usage) {
  return usage == GFX_BUFFER_USAGE_DYNAMIC_DRAW || 
         usage == GFX_BUFFER_USAGE_DYNAMIC_COPY || 
         usage == GFX_BUFFER_USAGE_DYNAMIC_READ;
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Context functions 

GfxContext* gfx_context_init(Window* window, const i32 flags) {
    GfxContext* gfx = (GfxContext*)memory_allocate(sizeof(GfxContext));
    memory_zero(gfx, sizeof(GfxContext));

    gfx->flags = (GfxContextFlags)flags;

    return gfx;
}

void gfx_context_shutdown(GfxContext* gfx) {
    if(!gfx) {
        return;
    }

    memory_free(gfx);
}

void gfx_context_clear(GfxContext* gfx, const f32 r, const f32 g, const f32 b, const f32 a) {

}

void gfx_context_set_flag(GfxContext* gfx, const i32 flag, const bool value) {
    // TODO:
}

const GfxContextFlags gfx_context_get_flags(GfxContext* gfx) {
    return gfx->flags;
}

/// Context functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Shader functions 

GfxShader* gfx_shader_create(const i8* src) {
    GfxShader* shader = (GfxShader*)memory_allocate(sizeof(GfxShader));
    memory_zero(shader, sizeof(GfxShader));

    return shader;
}

const i32 gfx_shader_get_uniform_location(GfxShader* shader, const i8* uniform_name) {

}

void gfx_shader_upload_uniform(GfxShader* shader, const GfxUniformDesc& desc) {

}

void gfx_shader_upload_uniform_batch(GfxShader* shader, const GfxUniformDesc* descs, const sizei count) {

}

/// Shader functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Texture functions 

GfxTexture* gfx_texture_create(const GfxTextureDesc& desc) {
    GfxTexture* texture = (GfxTexture*)memory_allocate(sizeof(GfxTexture));
    memory_zero(texture, sizeof(GfxTexture));

    return texture;
}

void gfx_texture_destroy(GfxTexture* texture) {
    if(!texture) {
        return;
    }

    memory_free(texture);
}

void gfx_texture_update(GfxTexture* texture, const GfxTextureDesc& desc) {

}

/// Texture functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Pipeline functions 

GfxPipeline* gfx_pipeline_create(GfxContext* gfx, const GfxPipelineDesc& desc) {
    GfxPipeline* pipe = (GfxPipeline*)memory_allocate(sizeof(GfxPipeline));
    memory_zero(pipe, sizeof(GfxPipeline));

    return pipe;
}

void gfx_pipeline_destroy(GfxPipeline* pipeline) {
    if(!pipeline) {
        return;
    }

    memory_free(pipeline);
}

void gfx_pipeline_begin(GfxContext* gfx, GfxPipeline* pipeline) {

}

void gfx_pipeline_draw_vertex(GfxContext* gfx, GfxPipeline* pipeline, const GfxPipelineDesc* desc) {

}

void gfx_pipeline_draw_index(GfxContext* gfx, GfxPipeline* pipeline, const GfxPipelineDesc* desc) {

}

/// Pipeline functions 
///---------------------------------------------------------------------------------------------------------------------

/// *** Graphics ***
/// ---------------------------------------------------------------------

} // End of nikol

#endif // DirectX11 check

//////////////////////////////////////////////////////////////////////////
