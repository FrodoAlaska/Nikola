#include "nikol/nikol_core.hpp"

//////////////////////////////////////////////////////////////////////////

#ifdef NIKOL_GFX_CONTEXT_DX11  // DirectX11 check

/// @TEMP:
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
/// @TEMP:

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <cstring>

namespace nikol { // Start of nikol

/// ---------------------------------------------------------------------
/// *** Graphics ***

///---------------------------------------------------------------------------------------------------------------------
/// GfxContext
struct GfxContext {
  // Flags
  GfxContextFlags flags;
  u32 clear_flags = 0;
  bool has_msaa, has_stencil, has_depth;
  bool has_blend, has_cull_ccw, has_cull_cw, has_vsync;

  // Devices
  ID3D11Device* device                  = nullptr; 
  ID3D11DeviceContext* device_ctx       = nullptr;
  IDXGISwapChain* swapchain             = nullptr; 
  ID3D11RenderTargetView* render_target = nullptr;

  // States
  ID3D11Texture2D* stencil_buffer        = nullptr; 
  ID3D11DepthStencilState* stencil_state = nullptr;
  ID3D11DepthStencilView* stencil_view   = nullptr;
  ID3D11RasterizerState* raster_state    = nullptr;
  ID3D11BlendState* blend_state          = nullptr;

  // Other
  u32 msaa_samples = 0;
  D3D11_VIEWPORT viewport;
  D3D_FEATURE_LEVEL dx_version;
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
  
  ID3D11Buffer* vertex_uniform_buffers[UNIFORM_BUFFERS_MAX];
  ID3D11Buffer* pixel_uniform_buffers[UNIFORM_BUFFERS_MAX];

  sizei vertex_uniforms_count;
  sizei pixel_uniforms_count;
};
/// GfxShader
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTexture
struct GfxTexture {
  ID3D11Texture2D* handle            = nullptr;
  ID3D11SamplerState* sampler        = nullptr;
  ID3D11ShaderResourceView* resource = nullptr;

  GfxTextureDesc desc;
};
/// GfxTexture
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxPipeline
struct GfxPipeline {
  ID3D11Buffer* vertex_buffer = nullptr;
  ID3D11Buffer* index_buffer  = nullptr;
  ID3D11InputLayout* layout   = nullptr;

  GfxDrawMode draw_mode;

  GfxShader* shader = nullptr;

  ID3D11ShaderResourceView* texture_views[TEXTURES_MAX] = {};
  ID3D11SamplerState* texture_samplers[TEXTURES_MAX]    = {};

  u32 stride = 0;
  u32 offset = 0;
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
  gfx->viewport.Width  = event.window_framebuffer_width;
  gfx->viewport.Height = event.window_framebuffer_height;

  gfx->device_ctx->RSSetViewports(1, &gfx->viewport);

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

static void set_gfx_flags(GfxContext* gfx) {
  if((gfx->flags & GFX_FLAGS_DEPTH) == GFX_FLAGS_DEPTH) {
    gfx->has_depth = true;
    gfx->clear_flags |= D3D11_CLEAR_DEPTH;
  }
  
  if((gfx->flags & GFX_FLAGS_STENCIL) == GFX_FLAGS_STENCIL) {
    gfx->has_stencil = true;
    gfx->clear_flags |= D3D11_CLEAR_STENCIL;
  }
  
  if((gfx->flags & GFX_FLAGS_BLEND) == GFX_FLAGS_BLEND) {
    gfx->has_blend = true;
  }
  
  if((gfx->flags & GFX_FLAGS_MSAA) == GFX_FLAGS_MSAA) {
    gfx->has_msaa     = true;
    gfx->msaa_samples = 4;
  }

  if((gfx->flags & GFX_FLAGS_CULL_CW) == GFX_FLAGS_CULL_CW) {
    gfx->has_cull_cw = true;
  }
  
  if((gfx->flags & GFX_FLAGS_CULL_CCW) == GFX_FLAGS_CULL_CCW) {
    gfx->has_cull_ccw = true;
  }
  
  if((gfx->flags & GFX_FLAGS_ENABLE_VSYNC) == GFX_FLAGS_ENABLE_VSYNC) {
    gfx->has_vsync = true;
  }
}

static DXGI_RATIONAL get_refresh_rate(GfxContext* gfx, int width, int height) {
  // No need to go through everything below since VSYNC is off
  if(!gfx->has_vsync) {
    return DXGI_RATIONAL {
      .Numerator   = 0, 
      .Denominator = 1,
    };
  }

  DXGI_RATIONAL refresh_rate = {};
  u32 num_modes              = 0;

  IDXGIAdapter* adapter       = nullptr; 
  IDXGIFactory* factory       = nullptr; 
  IDXGIOutput* adapter_output = nullptr;
  DXGI_MODE_DESC* mode_list   = {nullptr};

  // Create a factory 
  HRESULT res = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
  check_error(res, "CreateDXGIFactory");

  // Create an adapter 
  res = factory->EnumAdapters(0, &adapter);
  check_error(res, "EnumAdapters");

  // Create an output 
  res = adapter->EnumOutputs(0, &adapter_output);
  check_error(res, "EnumOutputs");

  // Get the number modes that fit the required format 
  res = adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, NULL);
  check_error(res, "GetDisplayModeList");

  // Make an array of the modes 
  mode_list = (DXGI_MODE_DESC*)memory_allocate(sizeof(DXGI_MODE_DESC) * num_modes);

  // Actually fill the array this time 
  res = adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, mode_list);
  check_error(res, "GetDisplayModeList");

  // Iterate through all of the screen modes and find the refresh rate 
  // that is suitable with our current display mode  
  for(sizei i = 0; i < num_modes; i++) {
    // Found it!
    if(mode_list[i].Width == width && mode_list[i].Height == height) {
      refresh_rate = mode_list[i].RefreshRate;
      break;
    }
  }

  memory_free(mode_list);
  adapter_output->Release();
  adapter->Release();
  factory->Release();

  return refresh_rate;
}

static DXGI_SAMPLE_DESC init_msaa(GfxContext* gfx) {
  DXGI_SAMPLE_DESC sample_desc = {};
  sample_desc.Count   = 1; 
  sample_desc.Quality = 0;

  // Enabling Anti-aliasing 
  if(gfx->has_msaa) {
    sample_desc.Count   = gfx->msaa_samples; // TODO: Make this configurable
    sample_desc.Quality = 1;
  }

  return sample_desc;
}

static void init_blend_state(GfxContext* gfx) {
  if(!gfx->has_blend) {
    return;
  }

  D3D11_RENDER_TARGET_BLEND_DESC render_target = {};
  render_target.BlendEnable           = true;
  render_target.SrcBlend              = D3D11_BLEND_SRC_COLOR;
  render_target.DestBlend             = D3D11_BLEND_DEST_COLOR;
  render_target.BlendOp               = D3D11_BLEND_OP_ADD;
  render_target.SrcBlendAlpha         = D3D11_BLEND_ONE;
  render_target.DestBlendAlpha        = D3D11_BLEND_SRC_ALPHA;
  render_target.BlendOpAlpha          = D3D11_BLEND_OP_SUBTRACT;
  render_target.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

  D3D11_BLEND_DESC blend_desc = {};
  blend_desc.AlphaToCoverageEnable  = true;
  blend_desc.IndependentBlendEnable = false;
  blend_desc.RenderTarget[0]        = render_target;

  HRESULT res = gfx->device->CreateBlendState(&blend_desc, &gfx->blend_state);
  check_error(res, "CreateBlendState");
}

static void init_depth_buffer(GfxContext* gfx, int width, int height) {
  D3D11_TEXTURE2D_DESC stencil_buff_desc = {};
  stencil_buff_desc.Width     = width; 
  stencil_buff_desc.Height    = height; 
  stencil_buff_desc.MipLevels = 1; 
  stencil_buff_desc.ArraySize = 1; 
  stencil_buff_desc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT; 

  stencil_buff_desc.SampleDesc.Count   = 1; 
  stencil_buff_desc.SampleDesc.Quality = 0; 
  
  stencil_buff_desc.Usage          = D3D11_USAGE_DEFAULT; 
  stencil_buff_desc.BindFlags      = D3D11_BIND_DEPTH_STENCIL; 
  stencil_buff_desc.CPUAccessFlags = 0; 
  stencil_buff_desc.MiscFlags      = 0;

  HRESULT res = gfx->device->CreateTexture2D(&stencil_buff_desc, NULL, &gfx->stencil_buffer);
  check_error(res, "CreateTexture2D");
}

static void init_stencil_buffer(GfxContext* gfx) {
  D3D11_DEPTH_STENCIL_DESC stencil_desc = {};

  if(gfx->has_depth) {
    stencil_desc.DepthEnable    = true;
    stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    stencil_desc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;
  }
  else {
    stencil_desc.DepthEnable = false;
  }

  if(gfx->has_stencil) {
    stencil_desc.StencilEnable    = true;    
    stencil_desc.StencilReadMask  = 0xff;    
    stencil_desc.StencilWriteMask = 0xff;    
  }
  else {
    stencil_desc.StencilEnable = false;    
  }

  // Front-facing pixel stencil test 
  stencil_desc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
  stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  stencil_desc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
  stencil_desc.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;
  
  // Back-facing pixel stencil test 
  stencil_desc.BackFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
  stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
  stencil_desc.BackFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
  stencil_desc.BackFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;

  // Create the depth stencil state 
  HRESULT res = gfx->device->CreateDepthStencilState(&stencil_desc, &gfx->stencil_state);
  check_error(res, "CreateDepthStencilState");

  // Set the depth stencil state 
  gfx->device_ctx->OMSetDepthStencilState(gfx->stencil_state, 1);

  D3D11_DEPTH_STENCIL_VIEW_DESC stencil_view_desc = {};
  stencil_view_desc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
  stencil_view_desc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
  stencil_view_desc.Texture2D.MipSlice = 0;

  // Create a stencil view 
  res = gfx->device->CreateDepthStencilView(gfx->stencil_buffer, &stencil_view_desc, &gfx->stencil_view);
  check_error(res, "CreateDepthStencilView");

  // Bind the render target to the depth stencil buffer
  gfx->device_ctx->OMSetRenderTargets(1, &gfx->render_target, gfx->stencil_view);
}

static void init_rasterizer_state(GfxContext* gfx) {
  D3D11_RASTERIZER_DESC raster_desc = {};
  raster_desc.FillMode = D3D11_FILL_SOLID; 
  raster_desc.CullMode = D3D11_CULL_FRONT; 

  raster_desc.FrontCounterClockwise = gfx->has_cull_ccw;

  raster_desc.DepthBias            = 0; 
  raster_desc.DepthBiasClamp       = 0.0f;
  raster_desc.SlopeScaledDepthBias = 0.0f;
  raster_desc.DepthClipEnable      = (gfx->has_cull_ccw || gfx->has_cull_cw);
  raster_desc.ScissorEnable        = false;

  raster_desc.AntialiasedLineEnable = gfx->has_msaa;
  raster_desc.MultisampleEnable     = gfx->has_msaa;

  // Create the rasterizer state
  HRESULT res = gfx->device->CreateRasterizerState(&raster_desc, &gfx->raster_state);
  check_error(res, "CreateRasterizerState");

  // Set the rasterizer state
  gfx->device_ctx->RSSetState(gfx->raster_state);
}

static void init_viewport(GfxContext* gfx, int width, int height) {
  gfx->viewport = {};
  gfx->viewport.Width = width; 
  gfx->viewport.Height = height;
  gfx->viewport.MinDepth = 0.0f; 
  gfx->viewport.MaxDepth = 1.0f; 
  gfx->viewport.TopLeftX = 0.0f; 
  gfx->viewport.TopLeftY = 0.0f;

  gfx->device_ctx->RSSetViewports(1, &gfx->viewport); 
}

static void init_d3d11(GfxContext* gfx, Window* window) {
  D3D_FEATURE_LEVEL feature_level[2] = {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_11_1,
  };
  u32 flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; 

  // More debug information when in debug mode 
#if NIKOL_BUILD_DEBUG == 1 
  flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  i32 width, height; 
  window_get_size(window, &width, &height);

  // Buffer desc
  DXGI_MODE_DESC buffer_desc = {};
  buffer_desc.Width                   = width;
  buffer_desc.Height                  = height; 
  buffer_desc.RefreshRate             = get_refresh_rate(gfx, width, height),
  buffer_desc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
  buffer_desc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  buffer_desc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;

  // Sample desc
  DXGI_SAMPLE_DESC sample_desc = init_msaa(gfx);

  // @TODO: No. Absolutely not.
  GLFWwindow* wnd = (GLFWwindow*)window_get_native_handle(window);

  // Swap chain
  DXGI_SWAP_CHAIN_DESC swap_desc = {
    .BufferDesc   = buffer_desc, 
    .SampleDesc   = sample_desc, 
    .BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT, 
    .BufferCount  = 1, 
    .OutputWindow = glfwGetWin32Window(wnd), 
    .Windowed     = !window_is_fullscreen(window),
    .SwapEffect   = DXGI_SWAP_EFFECT_DISCARD,
    .Flags        = 0,
  };

  // Creating the device 
  HRESULT res = D3D11CreateDeviceAndSwapChain(
    NULL, 
    D3D_DRIVER_TYPE_HARDWARE, 
    NULL, 
    flags, 
    feature_level, 
    2, 
    D3D11_SDK_VERSION, 
    &swap_desc, 
    &gfx->swapchain, 
    &gfx->device, 
    &gfx->dx_version, 
    &gfx->device_ctx
  );
  
  check_error(res, "D3D11CreateDeviceAndSwapChain");
  NIKOL_ASSERT(gfx->swapchain && gfx->device && gfx->device_ctx, "Invalid Direct3D structs returned"); 

  // Creating the render target 
  ID3D11Texture2D* framebuffer = nullptr;
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

  // Blend state init 
  init_blend_state(gfx);

  // Depth state init 
  init_depth_buffer(gfx, width, height);

  // Stenctil state init
  init_stencil_buffer(gfx);

  // Rasterizer state init 
  init_rasterizer_state(gfx);

  // Viewport init 
  init_viewport(gfx, width, height);
}

static u32 get_dx_version_num(D3D_FEATURE_LEVEL level) {
  switch(level) {
    case D3D_FEATURE_LEVEL_1_0_CORE:
      return 1; 
    case D3D_FEATURE_LEVEL_9_1:
    case D3D_FEATURE_LEVEL_9_2:
    case D3D_FEATURE_LEVEL_9_3:
      return 9; 
    case D3D_FEATURE_LEVEL_10_0:
    case D3D_FEATURE_LEVEL_10_1:
      return 10; 
    case D3D_FEATURE_LEVEL_11_0:
    case D3D_FEATURE_LEVEL_11_1:
      return 11; 
    case D3D_FEATURE_LEVEL_12_0:
    case D3D_FEATURE_LEVEL_12_1:
    case D3D_FEATURE_LEVEL_12_2:
      return 12; 
    default:
      return 0;
  }
}

static void compile_shader_blob(const i8* src, const i8* entry_point, const i8* target, const u32 flags, ID3DBlob** blob) {
  ID3DBlob* err_msg;

  HRESULT res = D3DCompile(src, 
                           strlen(src), 
                           "shader", 
                           NULL, 
                           NULL, 
                           entry_point, 
                           target, 
                           flags, 
                           0, 
                           blob, 
                           &err_msg);
  check_error(res, "D3DCompile");

  if(err_msg) {
    NIKOL_LOG_FATAL("SHADER ERROR: %s", err_msg->GetBufferPointer());
  }
}

static sizei get_layout_size(const GfxLayoutType layout) {
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

static DXGI_FORMAT get_layout_type(const GfxLayoutType layout) {
  switch(layout) {
    case GFX_LAYOUT_FLOAT1:
      return DXGI_FORMAT_R32_FLOAT;
    case GFX_LAYOUT_FLOAT2:
    case GFX_LAYOUT_MAT2:
      return DXGI_FORMAT_R32G32_FLOAT;
    case GFX_LAYOUT_FLOAT3:
    case GFX_LAYOUT_MAT3:
      return DXGI_FORMAT_R32G32B32_FLOAT;
    case GFX_LAYOUT_FLOAT4:
    case GFX_LAYOUT_MAT4:
      return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case GFX_LAYOUT_INT1:
      return DXGI_FORMAT_R32_SINT;
    case GFX_LAYOUT_INT2:
      return DXGI_FORMAT_R32G32_SINT;
    case GFX_LAYOUT_INT3:
      return DXGI_FORMAT_R32G32B32_SINT;
    case GFX_LAYOUT_INT4:
      return DXGI_FORMAT_R32G32B32A32_SINT;
    case GFX_LAYOUT_UINT1:
      return DXGI_FORMAT_R32_UINT;
    case GFX_LAYOUT_UINT2:
      return DXGI_FORMAT_R32G32_UINT;
    case GFX_LAYOUT_UINT3:
      return DXGI_FORMAT_R32G32B32_UINT;
    case GFX_LAYOUT_UINT4:
      return DXGI_FORMAT_R32G32B32A32_UINT;
    default:
      return DXGI_FORMAT_UNKNOWN;
  }
}

static void get_buffer_usage(const GfxBufferUsage usage, D3D11_USAGE* dx_usage, u32* access) {
  switch(usage) {
    case GFX_BUFFER_USAGE_DYNAMIC_DRAW:
      *dx_usage = D3D11_USAGE_DYNAMIC; 
      *access   = D3D11_CPU_ACCESS_WRITE;
      break;
    case GFX_BUFFER_USAGE_DYNAMIC_READ:
      *dx_usage = D3D11_USAGE_DYNAMIC; 
      *access   = D3D11_CPU_ACCESS_READ;
      break;
    case GFX_BUFFER_USAGE_STATIC_DRAW:
      *dx_usage = D3D11_USAGE_DEFAULT; 
      *access   = 0;
      break;
    case GFX_BUFFER_USAGE_STATIC_READ:
      *dx_usage = D3D11_USAGE_DEFAULT; 
      *access   = 0;
      break;
  }
}

static u32 get_buffer_type(const GfxBufferType type) {
  switch(type) {
    case GFX_BUFFER_VERTEX:
      return D3D11_BIND_VERTEX_BUFFER;
    case GFX_BUFFER_INDEX:
      return D3D11_BIND_INDEX_BUFFER;
    case GFX_BUFFER_UNIFORM:
      return D3D11_BIND_CONSTANT_BUFFER;
    default:
      return 0;
  }
}

static u32 get_semantic_index(const GfxLayoutType type) {
  switch (type) {
    case GFX_LAYOUT_MAT2:
      return 2;
    case GFX_LAYOUT_MAT3:
      return 3;
    case GFX_LAYOUT_MAT4:
      return 4;
    default:
      return 0;
  } 
}

static ID3D11Buffer* create_buffer(GfxContext* gfx, const GfxBufferDesc* desc) {
  D3D11_SUBRESOURCE_DATA data = {};
  D3D11_BUFFER_DESC buff_desc = {};
  ID3D11Buffer* buff          = nullptr;
  
  data.pSysMem = desc->data;

  get_buffer_usage(desc->usage, &buff_desc.Usage, &buff_desc.CPUAccessFlags);
  buff_desc.ByteWidth           = desc->size;
  buff_desc.BindFlags           = get_buffer_type(desc->type);
  buff_desc.MiscFlags           = 0; 
  buff_desc.StructureByteStride = 0;

  HRESULT res;
  
  if(desc->data) {
    res = gfx->device->CreateBuffer(&buff_desc, &data, &buff);
  }
  else {
    res = gfx->device->CreateBuffer(&buff_desc, NULL, &buff);
  }

  check_error(res, "CreateBuffer"); 

  return buff;
}

static void update_buffer(GfxContext* gfx, ID3D11Buffer* buffer, const GfxBufferDesc* new_buff) {
  D3D11_MAPPED_SUBRESOURCE map_res = {};
 
  // Begin the operation to map the new data into the buffer
  HRESULT res = gfx->device_ctx->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map_res);
  check_error(res, "Map");

  // Copy the memory to the buffer
  memory_copy(map_res.pData, new_buff->data, new_buff->size);

  // We're done here
  gfx->device_ctx->Unmap(buffer, 0);
}

static ID3D11InputLayout* set_layout(GfxContext* gfx, GfxShader* shader, const GfxLayoutDesc* layout, const sizei count, u32* stride) {
  ID3D11InputLayout* dx_layout = nullptr; 
  D3D11_INPUT_ELEMENT_DESC* descs = (D3D11_INPUT_ELEMENT_DESC*)memory_allocate(sizeof(D3D11_INPUT_ELEMENT_DESC) * count); 

  u32 input_align = 0;

  for(sizei i = 0; i < count; i++) {
    descs[i].SemanticName         = layout[i].name; 
    descs[i].SemanticIndex        = get_semantic_index(layout[i].type); 
    descs[i].Format               = get_layout_type(layout[i].type);
    descs[i].InputSlot            = 0; 
    descs[i].AlignedByteOffset    = input_align;
    descs[i].InputSlotClass       = layout[i].instance_rate == 0 ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA;
    descs[i].InstanceDataStepRate = layout[i].instance_rate;

    input_align = D3D11_APPEND_ALIGNED_ELEMENT;

    *stride += get_layout_size(layout[i].type);
  }

  // Creating the layout
  HRESULT res = gfx->device->CreateInputLayout(descs, 
                                               count, 
                                               shader->vertex_blob->GetBufferPointer(), 
                                               shader->vertex_blob->GetBufferSize(), 
                                               &dx_layout);
  check_error(res, "CreateInputLayout");

  memory_free(descs);
  return dx_layout;
}

static D3D11_PRIMITIVE_TOPOLOGY get_draw_mode(const GfxDrawMode mode) {
  switch(mode) {
    case GFX_DRAW_MODE_POINT:
      return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
    case GFX_DRAW_MODE_TRIANGLE:
      return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case GFX_DRAW_MODE_TRIANGLE_STRIP:
      return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    case GFX_DRAW_MODE_TRIANGLE_FAN:
      return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED; // TODO
    case GFX_DRAW_MODE_LINE:
      return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    case GFX_DRAW_MODE_LINE_STRIP:
      return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
    default:
      return (D3D11_PRIMITIVE_TOPOLOGY)0;
  }
}

static bool is_buffer_dynamic(const GfxBufferUsage& usage) {
  return usage == GFX_BUFFER_USAGE_DYNAMIC_DRAW || 
         usage == GFX_BUFFER_USAGE_DYNAMIC_READ;
}

static void create_uniform_buffer_by_type(GfxContext* gfx, GfxShader* shader, const GfxShaderType& type, const sizei buff_size, sizei* index) {
  GfxBufferDesc buff_desc = {
    .data  = nullptr, 
    .size  = buff_size, 
    .type  = GFX_BUFFER_UNIFORM, 
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };

  switch(type) {
    case GFX_SHADER_VERTEX:
      shader->vertex_uniforms_count++;
      NIKOL_ASSERT((shader->vertex_uniforms_count < UNIFORM_BUFFERS_MAX), "Cannot create more than UNIFORM_BUFFERS_MAX");

      *index                                 = shader->vertex_uniforms_count - 1;
      shader->vertex_uniform_buffers[*index] = create_buffer(gfx, &buff_desc);
      break;
    case GFX_SHADER_PIXEL:
      shader->pixel_uniforms_count++;
      NIKOL_ASSERT((shader->pixel_uniforms_count < UNIFORM_BUFFERS_MAX), "Cannot create more than UNIFORM_BUFFERS_MAX");
      
      *index                                = shader->pixel_uniforms_count - 1;
      shader->pixel_uniform_buffers[*index] = create_buffer(gfx, &buff_desc);
      break;
    case GFX_SHADER_GEOMETRY:
      break;
    default:
      break;
  }
}

static ID3D11Buffer* get_uniform_buffer_by_type(GfxShader* shader, const GfxShaderType& type, const sizei index) {
  switch(type) {
    case GFX_SHADER_VERTEX:
      return shader->vertex_uniform_buffers[index];
    case GFX_SHADER_PIXEL:
      return shader->pixel_uniform_buffers[index];
    case GFX_SHADER_GEOMETRY:
      return nullptr;
    default:
      return nullptr;
  }
}

static void set_uniform_buffers(GfxContext* gfx, GfxShader* shader) {
  if(shader->vertex_uniforms_count > 0) {
    gfx->device_ctx->VSSetConstantBuffers(0, shader->vertex_uniforms_count, shader->vertex_uniform_buffers);
  }
  
  if(shader->pixel_uniforms_count > 0) {
    gfx->device_ctx->PSSetConstantBuffers(0, shader->pixel_uniforms_count, shader->pixel_uniform_buffers);
  }
}

static D3D11_FILTER get_texture_filter(GfxTextureFilter filter) {
  switch(filter) {
    case GFX_TEXTURE_FILTER_MIN_MAG_LINEAR:
      return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    case GFX_TEXTURE_FILTER_MIN_MAG_NEAREST:
      return D3D11_FILTER_MIN_MAG_MIP_POINT;
    case GFX_TEXTURE_FILTER_MIN_LINEAR_MAG_NEAREST:
      return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
    case GFX_TEXTURE_FILTER_MIN_NEAREST_MAG_LINEAR:
      return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
    case GFX_TEXTURE_FILTER_MIN_TRILINEAR_MAG_LINEAR:
      return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    case GFX_TEXTURE_FILTER_MIN_TRILINEAR_MAG_NEAREST:
      return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
    default:
      return (D3D11_FILTER)0;
  }
}

static D3D11_TEXTURE_ADDRESS_MODE get_texture_wrap(GfxTextureWrap wrap) {
  switch(wrap) {
    case GFX_TEXTURE_WRAP_REPEAT:
      return D3D11_TEXTURE_ADDRESS_WRAP;
    case GFX_TEXTURE_WRAP_MIRROR:
      return D3D11_TEXTURE_ADDRESS_MIRROR;
    case GFX_TEXTURE_WRAP_CLAMP:
      return D3D11_TEXTURE_ADDRESS_CLAMP;
    case GFX_TEXTURE_WRAP_BORDER_COLOR:
      return D3D11_TEXTURE_ADDRESS_BORDER;
    default:
      return (D3D11_TEXTURE_ADDRESS_MODE)0;
  }
}

static DXGI_FORMAT get_texture_format(u32* channels, const GfxTextureFormat format) {
  switch(format) {
    case GFX_TEXTURE_FORMAT_R8:
      *channels = 1;
      return DXGI_FORMAT_R8_UNORM;
    case GFX_TEXTURE_FORMAT_R16:
      *channels = 1;
      return DXGI_FORMAT_R16_UNORM;
    case GFX_TEXTURE_FORMAT_RG8:
      *channels = 2;
      return DXGI_FORMAT_R8G8_UNORM;
    case GFX_TEXTURE_FORMAT_RG16:
      *channels = 2;
      return DXGI_FORMAT_R16G16_UNORM;
    case GFX_TEXTURE_FORMAT_RGBA8:
      *channels = 4;
      return DXGI_FORMAT_R8G8B8A8_UNORM;
    case GFX_TEXTURE_FORMAT_RGBA16:
      *channels = 4;
      return DXGI_FORMAT_R16G16B16A16_UNORM;
    default:
      return DXGI_FORMAT_UNKNOWN;
  }
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Context functions 

GfxContext* gfx_context_init(Window* window, const i32 flags) {
  NIKOL_ASSERT(window, "Invalid window passed to context");

  GfxContext* gfx = (GfxContext*)memory_allocate(sizeof(GfxContext));
  memory_zero(gfx, sizeof(GfxContext));
  
  // Setting the flags
  gfx->flags = (GfxContextFlags)flags;
  set_gfx_flags(gfx);

  // D3D11 init
  init_d3d11(gfx, window);

  // Listening to events
  event_listen(EVENT_WINDOW_FRAMEBUFFER_RESIZED, framebuffer_resize, gfx);

  i32 dx_version     = get_dx_version_num(gfx->dx_version);
  i32 shader_version = 11;

  NIKOL_LOG_INFO("A Direct3D11 graphics context was successfully created:\n" 
                 "              DIRECT3D VERSION: %i\n" 
                 "              SHADER VERSION: %i", 
                 dx_version, shader_version);

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
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  f32 color[4] = {r, g, b, a};

  gfx->device_ctx->ClearRenderTargetView(gfx->render_target, color);
  gfx->device_ctx->ClearDepthStencilView(gfx->stencil_view, gfx->clear_flags, 1.0f, 0);

  // @TODO: Fix
  f32 blend_factor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  gfx->device_ctx->OMSetBlendState(gfx->blend_state, blend_factor, 0xffffffff);

  gfx->device_ctx->RSSetViewports(1, &gfx->viewport);
  gfx->device_ctx->OMSetRenderTargets(1, &gfx->render_target, gfx->stencil_view);
}

void gfx_context_present(GfxContext* gfx) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  gfx->swapchain->Present(gfx->has_vsync, 0);
}

void gfx_context_set_flag(GfxContext* gfx, const i32 flag, const bool value) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  // TODO:
}

const GfxContextFlags gfx_context_get_flags(GfxContext* gfx) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  return gfx->flags;
}

/// Context functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Shader functions 

GfxShader* gfx_shader_create(GfxContext* gfx, const i8* src) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  GfxShader* shader = (GfxShader*)memory_allocate(sizeof(GfxShader));
  memory_zero(shader, sizeof(GfxShader));

  u32 flags = D3DCOMPILE_ENABLE_STRICTNESS;

#if NIKOL_BUILD_DEBUG == 1
  flags |= D3DCOMPILE_DEBUG;
#endif

  // Compile the `src` data
  compile_shader_blob(src, "vs_main", "vs_5_0", flags, &shader->vertex_blob);
  compile_shader_blob(src, "ps_main", "ps_5_0", flags, &shader->pixel_blob);

  // Creating the vertex shader
  HRESULT res = gfx->device->CreateVertexShader(shader->vertex_blob->GetBufferPointer(), 
                                                shader->vertex_blob->GetBufferSize(), 
                                                NULL, 
                                                &shader->vertex_shader);
  check_error(res, "CreateVertexShader");

  // Creating the pixel shader 
  res = gfx->device->CreatePixelShader(shader->pixel_blob->GetBufferPointer(), 
                                       shader->pixel_blob->GetBufferSize(), 
                                       NULL, 
                                       &shader->pixel_shader);
  check_error(res, "CreatePixelShader");

  shader->vertex_uniforms_count = 0;
  shader->pixel_uniforms_count  = 0;

  return shader;
}

void gfx_shader_destroy(GfxShader* shader) {
  if(!shader) {
    return;
  }

  for(sizei i = 0; i < shader->vertex_uniforms_count; i++) {
    shader->vertex_uniform_buffers[i]->Release();
  }
  
  for(sizei i = 0; i < shader->pixel_uniforms_count; i++) {
    shader->pixel_uniform_buffers[i]->Release();
  }

  shader->vertex_shader->Release();
  shader->vertex_blob->Release();
  shader->pixel_shader->Release();
  shader->pixel_blob->Release();

  memory_free(shader);
}

const sizei gfx_shader_create_uniform(GfxContext* gfx, GfxShader* shader, const GfxShaderType type, const sizei uniform_buff_size) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  NIKOL_ASSERT(shader, "Invalid GfxShader struct passed");
  
  sizei index = 0;
  create_uniform_buffer_by_type(gfx, shader, type, uniform_buff_size, &index);

  return index;
}

void gfx_shader_queue_uniform(GfxContext* gfx, GfxShader* shader, const GfxUniformDesc& desc) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  NIKOL_ASSERT(shader, "Invalid GfxShader struct passed");
  NIKOL_ASSERT((desc.index >= 0 && desc.index < UNIFORM_BUFFERS_MAX), "Invalid index passed to \'gfx_shader_query_uniform\'");

  ID3D11Buffer* cbuff = get_uniform_buffer_by_type(shader, desc.shader_type, desc.index); 

  GfxBufferDesc buff_desc = {
    .data = desc.data, 
    .size = desc.size, 
  };
  update_buffer(gfx, cbuff, &buff_desc);
}

/// Shader functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Texture functions 

GfxTexture* gfx_texture_create(GfxContext* gfx, const GfxTextureDesc& desc) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  GfxTexture* texture = (GfxTexture*)memory_allocate(sizeof(GfxTexture));
  memory_zero(texture, sizeof(GfxTexture));

  // Creating the sampler 
  D3D11_SAMPLER_DESC sampler_desc = {
    .Filter         = get_texture_filter(desc.filter),
    .AddressU       = get_texture_wrap(desc.wrap_mode), 
    .AddressV       = get_texture_wrap(desc.wrap_mode), 
    .AddressW       = get_texture_wrap(desc.wrap_mode), 
    .MipLODBias     = 0.0f, 
    .MaxAnisotropy  = 1, 
    .ComparisonFunc = D3D11_COMPARISON_ALWAYS, 
    .BorderColor    = {0.0f, 0.0f, 0.0f, 0.0f},
    .MinLOD         = 0.0f, 
    .MaxLOD         = D3D11_FLOAT32_MAX,
  };
  HRESULT res = gfx->device->CreateSamplerState(&sampler_desc, &texture->sampler);
  check_error(res, "CreateSamplerState");

  u32 channels = 0;

  // D3D11 Texture desc
  D3D11_TEXTURE2D_DESC texture_desc = {
    .Width          = desc.width, 
    .Height         = desc.height, 
    .MipLevels      = desc.depth, 
    .ArraySize      = desc.depth <= 0 ? 1 : desc.depth, // If this is left to 0, it will seg fault... for some reason.
    .Format         = get_texture_format(&channels, desc.format),
    .SampleDesc     = {1, 0}, 
    .Usage          = D3D11_USAGE_DEFAULT, 
    .BindFlags      = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
    .CPUAccessFlags = 0, 
    .MiscFlags      = D3D11_RESOURCE_MISC_GENERATE_MIPS,
  }; 

  // No data set in the texture.
  // NOTE: We'll get an error if we don't do this.
  if(!desc.data) {
    res = gfx->device->CreateTexture2D(&texture_desc, NULL, &texture->handle);
    check_error(res, "CreateTexture2D");

    return texture;
  }
  
  // Setting the data
  D3D11_SUBRESOURCE_DATA data = {
    .pSysMem     = desc.data,
    .SysMemPitch = desc.width * channels,
  };

  // Creating the texture
  res = gfx->device->CreateTexture2D(&texture_desc, &data, &texture->handle);
  check_error(res, "CreateTexture2D");

  // Setting the shader resource view 
  D3D11_SHADER_RESOURCE_VIEW_DESC view_desc = {};
  view_desc.Format                    = texture_desc.Format; 
  view_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
  view_desc.Texture2D.MostDetailedMip = 0; 
  view_desc.Texture2D.MipLevels       = -1; 

  // Creating the shader resource to be used later to actually 
  // render the texture 
  res = gfx->device->CreateShaderResourceView(texture->handle, &view_desc, &texture->resource);
  check_error(res, "CreateShaderResourceView");

  // Generate the mipmap levels of the texture 
  gfx->device_ctx->GenerateMips(texture->resource);

  texture->desc = desc;
  return texture;
}

void gfx_texture_destroy(GfxTexture* texture) {
  if(!texture) {
    return;
  }

  memory_free(texture->desc.data);
  texture->resource->Release();
  texture->handle->Release();
  texture->sampler->Release();

  memory_free(texture);
}

void gfx_texture_update(GfxContext* gfx, GfxTexture* texture, const GfxTextureDesc& desc) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  NIKOL_ASSERT(texture, "Invalid GfxTexture struct passed");
}

/// Texture functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Pipeline functions 

GfxPipeline* gfx_pipeline_create(GfxContext* gfx, const GfxPipelineDesc& desc) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  GfxPipeline* pipeline = (GfxPipeline*)memory_allocate(sizeof(GfxPipeline));
  memory_zero(pipeline, sizeof(GfxPipeline));

  // Create the vertex buffer 
  pipeline->vertex_buffer = create_buffer(gfx, desc.vertex_buffer); 

  // Create the index buffer
  pipeline->index_buffer = create_buffer(gfx, desc.index_buffer); 

  // Setting the shader 
  pipeline->shader = desc.shader;

  // Setting the layout 
  pipeline->layout = set_layout(gfx, pipeline->shader, desc.layout, desc.layout_count, &pipeline->stride);

  // Set the draw mode for the pipeline 
  pipeline->draw_mode = desc.draw_mode;

  // Setting the textures 
  for(sizei i = 0; i < desc.texture_count; i++) {
    pipeline->texture_views[i]    = desc.textures[i]->resource;
    pipeline->texture_samplers[i] = desc.textures[i]->sampler;
  }

  return pipeline;
}

void gfx_pipeline_destroy(GfxPipeline* pipeline) {
  if(!pipeline) {
      return;
  }

  // Free layout 
  pipeline->layout->Release();

  // Free the buffers
  pipeline->vertex_buffer->Release();
  pipeline->index_buffer->Release();

  // Free the pipeline
  memory_free(pipeline);
}

void gfx_pipeline_draw_vertex(GfxContext* gfx, GfxPipeline* pipeline, const GfxPipelineDesc& desc) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  NIKOL_ASSERT(pipeline, "Invalid GfxPipeline struct passed");
  NIKOL_ASSERT(desc.vertex_buffer, "Must have a valid vertex buffer to draw");
  
  // Set the topology
  gfx->device_ctx->IASetPrimitiveTopology(get_draw_mode(desc.draw_mode));
  
  // Set the layout  
  gfx->device_ctx->IASetInputLayout(pipeline->layout);
  
  // Update the buffer if it is dynamic
  if(is_buffer_dynamic(desc.vertex_buffer->usage)) {
    update_buffer(gfx, pipeline->vertex_buffer, desc.vertex_buffer);
  }
  
  // Set the buffer
  gfx->device_ctx->IASetVertexBuffers(0, 1, &pipeline->vertex_buffer, &pipeline->stride, &pipeline->offset);
  
  // Set the constant buffers in the shader
  set_uniform_buffers(gfx, desc.shader); 
  
  // Set the shaders
  gfx->device_ctx->VSSetShader(desc.shader->vertex_shader, NULL, 0);
  gfx->device_ctx->PSSetShader(desc.shader->pixel_shader, NULL, 0);

  // Setting the textures (if there are any)
  if(desc.texture_count > 0) {
    gfx->device_ctx->PSSetShaderResources(0, desc.texture_count, pipeline->texture_views);
    gfx->device_ctx->PSSetSamplers(0, desc.texture_count, pipeline->texture_samplers);
  }

  // Draw the vertex buffer
  gfx->device_ctx->Draw(desc.vertices_count, 0);
}

void gfx_pipeline_draw_index(GfxContext* gfx, GfxPipeline* pipeline, const GfxPipelineDesc& desc) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  NIKOL_ASSERT(pipeline, "Invalid GfxPipeline struct passed");
  NIKOL_ASSERT(desc.vertex_buffer, "Must have a valid vertex buffer to draw");
  NIKOL_ASSERT(desc.index_buffer, "Must have a valid index buffer to draw");
  
  // Set the topology
  gfx->device_ctx->IASetPrimitiveTopology(get_draw_mode(desc.draw_mode));
  
  // Set the layout  
  gfx->device_ctx->IASetInputLayout(pipeline->layout);
 
  // Update the buffers if they are dynamic
  if(is_buffer_dynamic(desc.vertex_buffer->usage)) {
    update_buffer(gfx, pipeline->vertex_buffer, desc.vertex_buffer);
  }
  
  if(is_buffer_dynamic(desc.index_buffer->usage)) {
    update_buffer(gfx, pipeline->index_buffer, desc.index_buffer);
  }

  // Set the buffers
  gfx->device_ctx->IASetIndexBuffer(pipeline->index_buffer, DXGI_FORMAT_R32_UINT, 0); 
  gfx->device_ctx->IASetVertexBuffers(0, 1, &pipeline->vertex_buffer, &pipeline->stride, &pipeline->offset);
  
  // Set the constant buffers in the shader
  set_uniform_buffers(gfx, desc.shader); 
  
  // Set the shaders
  gfx->device_ctx->VSSetShader(desc.shader->vertex_shader, NULL, 0);
  gfx->device_ctx->PSSetShader(desc.shader->pixel_shader, NULL, 0);
  
  // Setting the textures (if there are any)
  if(desc.texture_count > 0) {
    gfx->device_ctx->PSSetShaderResources(0, desc.texture_count, pipeline->texture_views);
    gfx->device_ctx->PSSetSamplers(0, desc.texture_count, pipeline->texture_samplers);
  }

  // Draw the index buffer
  gfx->device_ctx->DrawIndexed(desc.indices_count, 0, 0);
}

/// Pipeline functions 
///---------------------------------------------------------------------------------------------------------------------

/// *** Graphics ***
/// ---------------------------------------------------------------------

} // End of nikol

#endif // DirectX11 check

//////////////////////////////////////////////////////////////////////////
