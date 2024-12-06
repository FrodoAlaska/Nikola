#include "../nikol_core.hpp"

//////////////////////////////////////////////////////////////////////////

#ifdef NIKOL_GFX_CONTEXT_DX11  // DirectX11 check

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>

namespace nikol { // Start of nikol

/// ---------------------------------------------------------------------
/// *** Graphics ***

///---------------------------------------------------------------------------------------------------------------------
/// GfxContext
struct GfxContext {
  GfxContextFlags flags;
  u32 clear_flags = 0;

  ID3D11Device* device                  = nullptr; 
  ID3D11DeviceContext* device_ctx       = nullptr;
  IDXGISwapChain* swapchain             = nullptr; 
  ID3D11RenderTargetView* render_target = nullptr;

  ID3D11Texture2D* stencil_buffer        = nullptr; 
  ID3D11DepthStencilState* stencil_state = nullptr;
  ID3D11DepthStencilView* stencil_view   = nullptr;
  ID3D11RasterizerState* raster_state    = nullptr;
};
/// GfxContext
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxShader
struct GfxShader {
  ID3DBlob* vertex_blob = nullptr;
  ID3DBlob* pixel_blob  = nullptr;

  ID3D11VertexShader* vertex_shader = nullptr;
  ID3D11PixelShader* pixel_shader   = nullptr;
};
/// GfxShader
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTexture
struct GfxTexture {
  ID3D11Texture2D* handle = nullptr;
};
/// GfxTexture
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxPipeline
struct GfxPipeline {
  ID3D11Buffer* vertex_buffer = nullptr;
  ID3D11Buffer* index_buffer = nullptr;
  
  GfxTexture* textures[TEXTURES_MAX] = {};
};
/// GfxPipeline
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Callbacks 

static bool framebuffer_resize(const Event& event, const void* dispatcher, const void* listener) {
  if(event.type != EVENT_WINDOW_FRAMEBUFFER_RESIZED) {
    return false;
  }

  GfxContext* gfx = (GfxContext*)listener;

  return true;
}

/// Callbacks 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions
static void check_error(HRESULT res, const i8* func) {
  // Seriously, Bill, you didn't have to be THIS verbose...

  switch(res) {
    case D3D11_ERROR_FILE_NOT_FOUND:
      NIKOL_LOG_FATAL("D3D11 ERROR in %s: D3D11_ERROR_FILE_NOT_FOUND", func);
      break;
    case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
      NIKOL_LOG_FATAL("D3D11 ERROR in %s: D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS", func);
      break;
    case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS:
      NIKOL_LOG_FATAL("D3D11 ERROR in %s: D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS", func);
      break;
    case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD:
      NIKOL_LOG_FATAL("D3D11 ERROR in %s: D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD", func);
      break;
    case DXGI_ERROR_INVALID_CALL:
      NIKOL_LOG_FATAL("D3D11 ERROR in %s: DXGI_ERROR_INVALID_CALL", func);
      break;
    case DXGI_ERROR_WAS_STILL_DRAWING:
      NIKOL_LOG_FATAL("D3D11 ERROR in %s: DXGI_ERROR_WAS_STILL_DRAWING", func);
      break;
    case E_FAIL:
      NIKOL_LOG_FATAL("D3D11 ERROR in %s: Debug layer is not installed", func);
      break;
    case E_INVALIDARG:
      NIKOL_LOG_FATAL("D3D11 ERROR in %s: Invalid argument was passed", func);
      break;
    case E_OUTOFMEMORY:
      NIKOL_LOG_FATAL("D3D11 ERROR in %s: Ran out of memory", func);
      break;
    case E_NOTIMPL:
      NIKOL_LOG_FATAL("D3D11 ERROR in %s: Passed wrong parameter(s)", func);
      break;
    case S_FALSE:
      NIKOL_LOG_FATAL("D3D11 ERROR in %s: Good luck...", func);
      break;
    case S_OK:
      break;
  }
}

static DXGI_SAMPLE_DESC init_msaa(GfxContext* gfx) {
  DXGI_SAMPLE_DESC sample_desc = {};
  sample_desc.Count   = 1; 
  sample_desc.Quality = 0;

  // Enabling Anti-aliasing 
  if((gfx->flags & GFX_FLAGS_MSAA) == GFX_FLAGS_MSAA) {
    sample_desc.Count   = 4; // TODO: Make this configurable
    sample_desc.Quality = 1;
  }

  return sample_desc;
}

static void init_depth_buffer(GfxContext* gfx, int width, int height) {
  ID3D11_TEXTURE2D_DESC stencil_buff_desc = {};
  stencil_buff_desc.Width     = width; 
  stencil_buff_desc.height    = height; 
  stencil_buff_desc.MipLevels = 1; 
  stencil_buff_desc.ArraySize = 1; 
  stencil_buff_desc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT; 

  stencil_buff_desc.SampleDesc.Count = 1; 
  stencil_buff_desc.SampleDesc.Count = 0; 
  
  stencil_buff_desc.Usage          = D3D11_USAGE_DEFAULT; 
  stencil_buff_desc.BindFlags      = D3D11_BIND_DEPTH_STENCIL; 
  stencil_buff_desc.CPUAccessFlags = 0; 
  stencil_buff_desc.MiscFlags      = 0;

  HRESULT res = gfx->device->CreateTexture2D(&stencil_buff_desc, NULL, &gfx->stencil_buffer);
  check_error(res, "CreateTexture2D");
}

static void init_stencil_buffer(GfxContext* gfx) {
  bool has_depth   = (gfx->flags & GFX_FLAGS_DEPTH) == GFX_FLAGS_DEPTH;
  bool has_stencil = (gfx->flags & GFX_FLAGS_STENCIL) == GFX_FLAGS_STENCIL;

  ID3D11_DEPTH_STENCIL_DESC stencil_desc = {};

  if(has_depth) {
    stencil_desc.DepthEnable    = true;
    stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    stencil_desc.DepthFunc      = D3D11_COMPARISON_FUNC_LESS_EQUAL;

    gfx->clear_flags |= D3D11_CLEAR_DEPTH;
  }
  else {
    stencil_desc.DepthEnable = false;
  }

  if(has_stencil) {
    stencil_desc.StencilEnable    = true;    
    stencil_desc.StencilReadMask  = 0xff;    
    stencil_desc.StencilWriteMask = 0xff;    
    
    gfx->clear_flags |= D3D11_CLEAR_STENCIL;
  }
  else {
    stencil_desc.StencilEnable = false;    
  }

  // Front-facing pixel stencil test 
  stencil_desc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
  stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  stencil_desc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
  stencil_desc.FrontFace.StencilFunc        = D3D11_COMPARISON_FUNC_ALWAYS;
  
  // Back-facing pixel stencil test 
  stencil_desc.BackFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
  stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
  stencil_desc.BackFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
  stencil_desc.BackFace.StencilFunc        = D3D11_COMPARISON_FUNC_ALWAYS;

  // Create the depth stencil state 
  HRESULT res = gfx->device->CreateDepthStencilState(&stencil_desc, &gfx->stencil_state);
  check_error(res, "CreateDepthStencilState");

  // Set the depth stencil state 
  gfx->device_ctx->OMSetDepthStencilState(gfx->stencil_state, 1);

  ID3D11_DEPTH_STENCIL_VIEW_DESC stencil_view_desc = {};
  stencil_view_desc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
  stencil_view_desc.ViewDimensions     = D3D11_DSV_DIMENSION_TEXTURE2D;
  stencil_view_desc.Texture2D.MipSlice = 0;

  // Create a stencil view 
  res = gfx->device->CreateDepthStencilView(gfx->stencil_buffer, &stencil_view_desc, &gfx->stencil_view);
  check_error(res, "CreateDepthStencilView");

  // Bind the render target to the depth stencil buffer
  gfx->device_ctx->OMSetRenderTargets(1, &gfx->render_target, gfx->stencil_view);
}

static void init_rasterizer_state(GfxContext* gfx) {
  bool has_msaa = (gfx->flags & GFX_FLAGS_MSAA) == GFX_FLAGS_MSAA;
  bool has_ccw = (gfx->flags & GFX_FLAGS_CULL_CCW) == GFX_FLAGS_CULL_CCW;
  bool has_cw = (gfx->flags & GFX_FLAGS_CULL_CW) == GFX_FLAGS_CULL_CW;

  ID3D11_RASTERIZER_DESC raster_desc = {};
  raster_desc.FillMode = D3D11_FILL_MODE_SOLID; 
  raster_desc.CullMode = D3D11_CULL_MODE_FRONT; 

  raster_desc.FrontCounterClockwise = has_ccw : true ? false;

  raster_desc.DepthBias            = 0; 
  raster_desc.DepthBiasClamp       = 0.0f;
  raster_desc.SlopeScaledDepthBias = 0.0f;
  raster_desc.DepthClipEnable      = (has_ccw || has_cw);
  raster_desc.ScissorEnable        = false;

  raster_desc.AntialiasedLineEnable = has_msaa;
  raster_desc.MultisampleEnable     = has_msaa;

  // Create the rasterizer state
  HRESULT res = gfx->device->CreateRasterizerState(&raster_desc, &gfx->raster_state);
  check_error(res, "CreateRasterizerState");

  // Set the rasterizer state
  gfx->device_ctx->RSSetState(gfx->raster_state);
}

static void init_viewport(GfxContext* gfx, int width, int height) {
  D3D11_VIEWPORT view = {};
  view.Width = width; 
  view.Height = height;
  view.MinDepth = 0.0f; 
  view.MaxDepth = 1.0f; 
  view.TopLeftX = 0.0f; 
  view.TopLeftY = 0.0f;

  gfx->device_ctx->RSSetViewports(1, &view); 
}

static void init_d3d11(GfxContext* gfx, Window* window) {
  D3D_FEATURE_LEVEL feature_level;
  u32 flags = D3D11_CREATE_DEVICE_SINGLETHREADED; 

  // More debug information when in debug mode 
#if NIKOL_DEBUG_BUILD == 1 
  flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  i32 width, height; 
  window_get_size(window, &width, &height);

  // Buffer desc
  DXGI_MODE_DESC buffer_desc = {};
  buffer_desc.Width                   = width;
  buffer_desc.Height                  = height; 

  // TODO: This disables vsync by default. 
  buffer_desc.RefreshRate.Numerator   = 0;
  buffer_desc.RefreshRate.Denominator = 1;

  buffer_desc.Format                  = DXGI_FORMAT_R8G8B8A8_UINT;
  buffer_desc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  buffer_desc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;

  // Sample desc
  DXGI_SAMPLE_DESC sample_desc = init_msaa(gfx);

  // Swap chain
  DXGI_SWAP_CHAIN_DESC swap_desc = {
    .BufferDesc   = buffer_desc, 
    .SampleDesc   = sample_desc, 
    .BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT, 
    .BufferCount  = 1, 
    .OutputWindow = NULL, 
    .Windowed     = !window_is_fullscreen(window),
  };

  // Creating the device 
  HRESULT res = D3D11CreateDeviceAndSwapChain(
    NULL, 
    D3D_DRIVER_TYPE_HARDWARE, 
    NULL, 
    flags, 
    NULL, 
    0, 
    D3D11_SDK_VERSION, 
    &swap_desc, 
    &gfx->swapchain, 
    &gfx->device, 
    &feature_level, 
    &gfx->device_ctx
  );
  check_error(res, "D3D11CreateDeviceAndSwapChain");

  // Creating the render target 
  ID3D11Texture2D* framebuffer;
  res = gfx->swapchain->GetBuffer(0,
                                  __uuidof(ID3D11Texture2D), 
                                  (void**)&framebuffer);
  check_error(res, "GetBuffer");  

  res = gfx->device->CreateRenderTargetView(
    framebuffer, 
    0, 
    &gfx->render_target
  );
  check_error(res, "CreateRenderTargetView");  

  framebuffer->Release();

  // Depth state init 
  init_depth_buffer(gfx, width, height);

  // Stenctil state init
  init_stencil_buffer(gfx);

  // Rasterizer state init 
  init_rasterizer_state(gfx);

  // Viewport init 
  init_viewport(gfx, width, height);
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
      return 0;
    case GFX_LAYOUT_INT1:
    case GFX_LAYOUT_INT2:
    case GFX_LAYOUT_INT3:
    case GFX_LAYOUT_INT4:
      return 0;
    case GFX_LAYOUT_UINT1:
    case GFX_LAYOUT_UINT2:
    case GFX_LAYOUT_UINT3:
    case GFX_LAYOUT_UINT4:
      return 0;
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
  
  // Setting the flags
  gfx->flags = (GfxContextFlags)flags;

  // D3D11 init
  init_d3d11(gfx, window);

  // Listening to events
  event_listen(EVENT_WINDOW_FRAMEBUFFER_RESIZED, framebuffer_resize, gfx);

  const i8* vendor         = "VEN";
  const i8* renderer       = "REN";
  i32 dx_version           = D3D11_SDK_VERSION;
  const i8* shader_version = "1.3";

  NIKOL_LOG_INFO("A Direct3D11 graphics context was successfully created:\n" 
                 "              VENDOR: %s\n" 
                 "              RENDERER: %s\n" 
                 "              DIRECT3D VERSION: %i\n" 
                 "              SHADER VERSION: %s", 
                 vendor, renderer, dx_version, shader_version);

  return gfx;
}

void gfx_context_shutdown(GfxContext* gfx) {
  if(!gfx) {
      return;
  }

  if(gfx->swapchain) {
    gfx->swapchain->SetFullscreenState(false, NULL);
  }

  if(gfx->raster_state) {
    gfx->raster_state->Release();
  }
  
  if(gfx->stencil_view) {
    gfx->stencil_view->Release();
  }
  
  if(gfx->stencil_state) {
    gfx->stencil_state->Release();
  }
  
  if(gfx->stencil_buffer) {
    gfx->stencil_buffer->Release();
  }
  
  if(gfx->render_target) {
    gfx->render_target->Release();
  }
  
  if(gfx->device_ctx) {
    gfx->device_ctx->Release();
  }

  if(gfx->device) {
    gfx->device->Release();
  }
  
  if(gfx->swapchain) {
    gfx->swapchain->Release();
  }

  NIKOL_LOG_INFO("The graphics context was successfully destroyed");
  memory_free(gfx);
}

void gfx_context_clear(GfxContext* gfx, const f32 r, const f32 g, const f32 b, const f32 a) {
  f32 color[4] = {r, g, b, a};

  gfx->device_ctx->ClearRenderTarget(gfx->render_target, color);
  gfx->device_ctx->ClearDepthStencilView(gfx->stencil_view, gfx->clear_flags, 1.0f, 0);
}

void gfx_context_present(GfxContext* gfx) {
  gfx->swapchain->Present(1, 0); // TODO: This enables vsync
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
  return 0;
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
