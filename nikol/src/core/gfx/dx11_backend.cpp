#include "nikol/nikol_core.hpp"

//////////////////////////////////////////////////////////////////////////

#ifdef NIKOL_GFX_CONTEXT_DX11  // DirectX11 check

/// @TEMP:
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define WIN32_LEAN_AND_MEAN
#include <GLFW/glfw3native.h>
/// @TEMP:

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

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
  GfxContextDesc desc;

  // Flags
  u32 clear_flags = 0;
  bool has_msaa   = false;
  bool has_vsync  = false;
  f32 depth_mask  = 1.0f;

  // Devices
  ID3D11Device* device                  = nullptr; 
  ID3D11DeviceContext* device_ctx       = nullptr;
  IDXGISwapChain* swapchain             = nullptr; 
  ID3D11RenderTargetView* render_target = nullptr;

  // State-related variables
  ID3D11DepthStencilView* stencil_view   = nullptr;
  ID3D11Texture2D* stencil_buffer        = nullptr; 

  // Valid states
  ID3D11DepthStencilState* valid_stencil_state = nullptr;
  ID3D11RasterizerState* valid_raster_state    = nullptr;
  ID3D11BlendState* valid_blend_state          = nullptr;

  // Current active states
  ID3D11DepthStencilState* stencil_state = nullptr;
  ID3D11RasterizerState* raster_state    = nullptr;
  ID3D11BlendState* blend_state          = nullptr;

  // Other
  D3D11_VIEWPORT viewport;
  D3D_FEATURE_LEVEL dx_version;
};
/// GfxContext
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBuffer
struct GfxBuffer {
  GfxBufferDesc desc = {};
  GfxContext* gfx    = nullptr

  ID3D11Buffer* buffer         = nullptr;
  D3D11_BUFFER_DESC d3d11_desc = {};
};
/// GfxBuffer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxShader
struct GfxShader {
  GfxContext* gfx = nullptr;

  ID3DBlob* vertex_blob = nullptr;
  ID3DBlob* pixel_blob  = nullptr;

  ID3D11VertexShader* vertex_shader = nullptr;
  ID3D11PixelShader* pixel_shader   = nullptr;
  
  ID3D11Buffer* vertex_uniforms[UNIFORM_BUFFERS_MAX];
  ID3D11Buffer* pixel_uniforms[UNIFORM_BUFFERS_MAX];

  sizei vertex_uniforms_count = 0;
  sizei pixel_uniforms_count  = 0;
};
/// GfxShader
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTexture
struct GfxTexture {
  GfxTextureDesc desc = {};
  GfxContext* gfx     = nullptr;
  
  ID3D11Texture2D* handle            = nullptr;
  ID3D11SamplerState* sampler        = nullptr;
  ID3D11ShaderResourceView* resource = nullptr;
};
/// GfxTexture
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxPipeline
struct GfxPipeline {
  GfxPipelineDesc desc = {};
  GfxContext* gfx      = nullptr;

  ID3D11Buffer* vertex_buffer = nullptr;
  ID3D11Buffer* index_buffer  = nullptr;
  ID3D11InputLayout* layout   = nullptr;

  GfxDrawMode draw_mode;

  GfxShader* shader = nullptr;

  ID3D11ShaderResourceView* texture_views[TEXTURES_MAX] = {};
  ID3D11SamplerState* texture_samplers[TEXTURES_MAX]    = {};
  u32 textures_count = 0;

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

static bool fullscreen_callback(const Event& event, const void* dispatcher, const void* listener) {
  if(event.type != EVENT_WINDOW_FULLSCREEN) {
    return false;
  }

  GfxContext* gfx = (GfxContext*)listener;
  HRESULT res = gfx->swapchain->SetFullscreenState(false, NULL);
  check_error(res, "SetFullscreenState");

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

static D3D11_COMPARISON_FUNC get_d3d11_compare(const GfxCompareFunc func) {
  switch(func) {
    case GFX_COMPARE_ALWAYS:
      return D3D11_COMPARISON_ALWAYS;
    case GFX_COMPARE_NEVER:
      return D3D11_COMPARISON_NEVER;
    case GFX_COMPARE_LESS:
      return D3D11_COMPARISON_LESS;
    case GFX_COMPARE_LESS_EQUAL:
      return D3D11_COMPARISON_LESS_EQUAL;
    case GFX_COMPARE_GREATER:
      return D3D11_COMPARISON_GREATER;
    case GFX_COMPARE_GREATER_EQUAL:
      return D3D11_COMPARISON_GREATER_EQUAL;
    case GFX_COMPARE_NOT_EQUAL:
      return D3D11_COMPARISON_NOT_EQUAL;
    default: 
      return (D3D11_COMPARISON_FUNC)0;
  }
}

static D3D11_STENCIL_OP get_d3d11_operation(const GfxOperation op) {
  switch(op) {
    case GFX_OP_KEEP:
      return D3D11_STENCIL_OP_KEEP;
    case GFX_OP_ZERO:
      return D3D11_STENCIL_OP_ZERO;
    case GFX_OP_INVERT:
      return D3D11_STENCIL_OP_INVERT;
    case GFX_OP_REPLACE:
      return D3D11_STENCIL_OP_REPLACE;
    case GFX_OP_INCR:
      return D3D11_STENCIL_OP_INCR;
    case GFX_OP_DECR:
      return D3D11_STENCIL_OP_DECR;
    case GFX_OP_INCR_WRAP:
      return D3D11_STENCIL_OP_INCR_SAT;
    case GFX_OP_DECR_WRAP:
      return D3D11_STENCIL_OP_DECR_SAT;
    default: 
      return (D3D11_STENCIL_OP)0;
  }
}

static D3D11_BLEND get_d3d11_blend(const GfxBlendMode mode) {
  switch(mode) {
    case GFX_BLEND_ZERO:
      return D3D11_BLEND_ZERO;
    case GFX_BLEND_ONE:
      return D3D11_BLEND_ONE;
    case GFX_BLEND_SRC_COLOR:
      return D3D11_BLEND_SRC_COLOR;
    case GFX_BLEND_DEST_COLOR:
      return D3D11_BLEND_DEST_COLOR;
    case GFX_BLEND_SRC_ALPHA:
      return D3D11_BLEND_SRC_ALPHA;
    case GFX_BLEND_DEST_ALPHA:
      return D3D11_BLEND_DEST_ALPHA;
    case GFX_BLEND_INV_SRC_COLOR:
      return D3D11_BLEND_INV_SRC_COLOR;
    case GFX_BLEND_INV_DEST_COLOR:
      return D3D11_BLEND_INV_DEST_COLOR;
    case GFX_BLEND_INV_SRC_ALPHA:
      return D3D11_BLEND_INV_SRC_ALPHA;
    case GFX_BLEND_INV_DEST_ALPHA:
      return D3D11_BLEND_INV_DEST_ALPHA;
    case GFX_BLEND_SRC_ALPHA_SATURATE:
      return D3D11_BLEND_SRC_ALPHA_SAT;
    default: 
      return (D3D11_BLEND)0;
  }
}

static D3D11_CULL_MODE get_d3d11_cull(const GfxCullMode mode) {
  switch(mode) {
    case GFX_CULL_FRONT:
      return D3D11_CULL_FRONT;
    case GFX_CULL_BACK:
      return D3D11_CULL_BACK;
    case GFX_CULL_FRONT_AND_BACK:
      return D3D11_CULL_NONE;
    default: 
      return (D3D11_CULL_MODE)0;
  }
}

static DXGI_FORMAT get_pixel_format(const GfxTextureFormat format) {
  switch(format) {
    case GFX_TEXTURE_FORMAT_R8:
      return DXGI_FORMAT_R8_UNORM;
    case GFX_TEXTURE_FORMAT_R16:
      return DXGI_FORMAT_R16_UNORM;
    case GFX_TEXTURE_FORMAT_RG8:
      return DXGI_FORMAT_R8G8_UNORM;
    case GFX_TEXTURE_FORMAT_RG16:
      return DXGI_FORMAT_R16G16_UNORM;
    case GFX_TEXTURE_FORMAT_RGBA8:
      return DXGI_FORMAT_R8G8B8A8_UNORM;
    case GFX_TEXTURE_FORMAT_RGBA16:
      return DXGI_FORMAT_R16G16B16A16_UNORM;
    default:
      return DXGI_FORMAT_UNKNOWN;
  }
}

static void init_depth_buffer(GfxContext* gfx, int width, int height) {
  D3D11_TEXTURE2D_DESC stencil_buff_desc = {};
  stencil_buff_desc.Width     = width; 
  stencil_buff_desc.Height    = height; 
  stencil_buff_desc.MipLevels = 1; 
  stencil_buff_desc.ArraySize = 1; 
  stencil_buff_desc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT; 

  stencil_buff_desc.SampleDesc.Count   = gfx->desc.msaa_samples; 
  stencil_buff_desc.SampleDesc.Quality = gfx->desc.msaa_samples - 1; 
  
  stencil_buff_desc.Usage          = D3D11_USAGE_DEFAULT; 
  stencil_buff_desc.BindFlags      = D3D11_BIND_DEPTH_STENCIL; 
  stencil_buff_desc.CPUAccessFlags = 0; 
  stencil_buff_desc.MiscFlags      = 0;

  HRESULT res = gfx->device->CreateTexture2D(&stencil_buff_desc, NULL, &gfx->stencil_buffer);
  check_error(res, "CreateTexture2D");
}

static void init_stencil_state(GfxContext* gfx) {
  D3D11_DEPTH_STENCIL_DESC stencil_desc = {};

  // Setting the depth state if it is set
  if(IS_BIT_SET(gfx->desc.states, GFX_STATE_DEPTH)) {
    stencil_desc.DepthEnable    = gfx->desc.depth_desc.depth_write_enabled;
    stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    stencil_desc.DepthFunc      = get_d3d11_compare(gfx->desc.depth_desc.compare_func);
    gfx->clear_flags |= D3D11_CLEAR_DEPTH;
  }
  else {
    stencil_desc.DepthEnable = false;
  }
  
  // Setting the stencil state if it is set
  if(IS_BIT_SET(gfx->desc.states, GFX_STATE_STENCIL)) {
    stencil_desc.StencilEnable    = true;    
    stencil_desc.StencilReadMask  = gfx->desc.stencil_desc.mask;    
    stencil_desc.StencilWriteMask = gfx->desc.stencil_desc.mask;    
    
    gfx->clear_flags |= D3D11_CLEAR_STENCIL;
  }
  else {
    stencil_desc.StencilEnable = false;    
  }
 
  D3D11_STENCIL_OP depth_pass   = get_d3d11_operation(gfx->desc.stencil_desc.depth_pass_op);
  D3D11_STENCIL_OP depth_fail   = get_d3d11_operation(gfx->desc.stencil_desc.depth_fail_op);
  D3D11_STENCIL_OP stencil_fail = get_d3d11_operation(gfx->desc.stencil_desc.stencil_fail_op);

  D3D11_COMPARISON_FUNC compare_func = get_d3d11_compare(gfx->desc.stencil_desc.compare_func);

  // Determining which face to cull
  switch(gfx->desc.stencil_desc.polygon_face) {
    case GFX_CULL_FRONT: 
      // Front-facing pixel stencil test 
      stencil_desc.FrontFace.StencilFailOp      = stencil_fail;
      stencil_desc.FrontFace.StencilDepthFailOp = depth_fail;
      stencil_desc.FrontFace.StencilPassOp      = depth_pass;
      stencil_desc.FrontFace.StencilFunc        = compare_func;
      break;
    case GFX_CULL_BACK:
      // Back-facing pixel stencil test 
      stencil_desc.BackFace.StencilFailOp      = stencil_fail;
      stencil_desc.BackFace.StencilDepthFailOp = depth_fail;
      stencil_desc.BackFace.StencilPassOp      = depth_pass;
      stencil_desc.BackFace.StencilFunc        = compare_func;
      break;
    case GFX_CULL_FRONT_AND_BACK:
      // Front-facing pixel stencil test 
      stencil_desc.FrontFace.StencilFailOp      = stencil_fail;
      stencil_desc.FrontFace.StencilDepthFailOp = depth_fail;
      stencil_desc.FrontFace.StencilPassOp      = depth_pass;
      stencil_desc.FrontFace.StencilFunc        = compare_func;
      
      // Back-facing pixel stencil test 
      stencil_desc.BackFace.StencilFailOp      = stencil_fail;
      stencil_desc.BackFace.StencilDepthFailOp = depth_fail;
      stencil_desc.BackFace.StencilPassOp      = depth_pass;
      stencil_desc.BackFace.StencilFunc        = compare_func;
      break;
  }

  // Create the depth stencil state 
  HRESULT res = gfx->device->CreateDepthStencilState(&stencil_desc, &gfx->valid_stencil_state);
  check_error(res, "CreateDepthStencilState");

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

static void init_blend_state(GfxContext* gfx) {
  D3D11_BLEND src_color  = get_d3d11_blend(gfx->desc.blend_desc.src_color_blend);
  D3D11_BLEND dest_color = get_d3d11_blend(gfx->desc.blend_desc.dest_color_blend);
  D3D11_BLEND src_alpha  = get_d3d11_blend(gfx->desc.blend_desc.src_alpha_blend);
  D3D11_BLEND dest_alpha = get_d3d11_blend(gfx->desc.blend_desc.dest_alpha_blend);

  D3D11_RENDER_TARGET_BLEND_DESC render_target = {};
  render_target.BlendEnable           = true;
  render_target.SrcBlend              = src_color;
  render_target.DestBlend             = dest_color;
  render_target.BlendOp               = D3D11_BLEND_OP_ADD;
  render_target.SrcBlendAlpha         = src_alpha;
  render_target.DestBlendAlpha        = dest_alpha;
  render_target.BlendOpAlpha          = D3D11_BLEND_OP_ADD;
  render_target.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

  D3D11_BLEND_DESC blend_desc = {};
  blend_desc.AlphaToCoverageEnable  = true;
  blend_desc.IndependentBlendEnable = false;
  blend_desc.RenderTarget[0]        = render_target;
  
  // Creating the blend state
  HRESULT res = gfx->device->CreateBlendState(&blend_desc, &gfx->valid_blend_state);
  check_error(res, "CreateBlendState");
}

static void init_cull_state(GfxContext* gfx) {
  D3D11_RASTERIZER_DESC raster_desc = {};
  raster_desc.FillMode = D3D11_FILL_SOLID; 
  raster_desc.CullMode = get_d3d11_cull(gfx->desc.cull_desc.cull_mode); 

  bool is_ccw = gfx->desc.cull_desc.front_face == GFX_ORDER_COUNTER_CLOCKWISE;

  raster_desc.FrontCounterClockwise = is_ccw;

  raster_desc.DepthBias            = 0; 
  raster_desc.DepthBiasClamp       = 0.0f;
  raster_desc.SlopeScaledDepthBias = 0.0f;
  raster_desc.DepthClipEnable      = true;
  raster_desc.ScissorEnable        = false;

  raster_desc.AntialiasedLineEnable = gfx->has_msaa;
  raster_desc.MultisampleEnable     = gfx->has_msaa;

  // Create the rasterizer state
  HRESULT res = gfx->device->CreateRasterizerState(&raster_desc, &gfx->valid_raster_state);
  check_error(res, "CreateRasterizerState");
}

static void set_gfx_state(GfxContext* gfx, const GfxStates state, const bool value) {
  switch(state) {
    // Depth state
    case GFX_STATE_DEPTH: {
      if(value) {
        UNSET_BIT(gfx->clear_flags, D3D11_CLEAR_DEPTH);
        UNSET_BIT(gfx->desc.states, (u32)state);
        gfx->depth_mask = 0.0f;
      } 
      else {
        SET_BIT(gfx->clear_flags, D3D11_CLEAR_DEPTH);
        SET_BIT(gfx->desc.states, (u32)state);
        gfx->depth_mask = 1.0f;
      }
    } break;
    
    // Stencil state
    case GFX_STATE_STENCIL: {
      if(value) {
        UNSET_BIT(gfx->clear_flags, D3D11_CLEAR_STENCIL);
        UNSET_BIT(gfx->desc.states, (u32)state);
        gfx->desc.stencil_desc.ref = 0;
        gfx->stencil_state         = nullptr;
      } 
      else {
        SET_BIT(gfx->clear_flags, D3D11_CLEAR_STENCIL);
        SET_BIT(gfx->desc.states, (u32)state);
        gfx->desc.stencil_desc.ref = 1;
        gfx->stencil_state         = gfx->valid_stencil_state;
      }

      gfx->device_ctx->OMSetDepthStencilState(gfx->stencil_state, gfx->desc.stencil_desc.ref);
    } break;

    // Blend state
    case GFX_STATE_BLEND: {
      f32* blend_factor = nullptr;

      if(value) {
        UNSET_BIT(gfx->desc.states, (u32)state);
        blend_factor     = gfx->desc.blend_desc.blend_factor;
        gfx->blend_state = nullptr;
      } 
      else {
        SET_BIT(gfx->desc.states, (u32)state);
        gfx->blend_state = gfx->valid_blend_state;
      }

      gfx->device_ctx->OMSetBlendState(gfx->blend_state, blend_factor, 0xffffffff);
    } break;
    
    // Cull/rasterizer state
    case GFX_STATE_CULL: {
      if(value) {
        UNSET_BIT(gfx->desc.states, (u32)state);
        gfx->raster_state = nullptr;
      } 
      else {
        SET_BIT(gfx->desc.states, (u32)state);
        gfx->raster_state = gfx->valid_raster_state;
      }

      gfx->device_ctx->RSSetState(gfx->raster_state);
    } break;
    
    // MSAA state
    case GFX_STATE_MSAA:
      gfx->has_msaa = value;
      break;
    
    // VSYNC
    case GFX_STATE_VSYNC:
      gfx->has_vsync = value;
      break;
  }
}

static void set_d3d11_states(GfxContext* gfx) {
  i32 width, height; 
  window_get_size(gfx->desc.window, &width, &height);

  init_depth_buffer(gfx, width, height);
  init_stencil_state(gfx);
  init_blend_state(gfx);
  init_cull_state(gfx);
  
  if(IS_BIT_SET(gfx->desc.states, (GFX_STATE_DEPTH | GFX_STATE_STENCIL))) {
    gfx->stencil_state = gfx->valid_stencil_state;
    gfx->device_ctx->OMSetDepthStencilState(gfx->stencil_state, gfx->desc.stencil_desc.ref);
  }

  if(IS_BIT_SET(gfx->desc.states, GFX_STATE_CULL)) {
    gfx->raster_state = gfx->valid_raster_state;
    gfx->device_ctx->RSSetState(gfx->raster_state);
  }

  if(IS_BIT_SET(gfx->desc.states, GFX_STATE_BLEND)) {
    gfx->blend_state = gfx->valid_blend_state;
    gfx->device_ctx->OMSetBlendState(gfx->blend_state, gfx->desc.blend_desc.blend_factor, 0xffffffff);
  }
  
  if(IS_BIT_SET(gfx->desc.states, GFX_STATE_MSAA)) {
    gfx->has_msaa = true;
  }

  if(IS_BIT_SET(gfx->desc.states, GFX_STATE_VSYNC)) {
    gfx->has_vsync = true;
  }
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
  buffer_desc.Format                  = get_pixel_format(gfx->desc.pixel_format);
  buffer_desc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  buffer_desc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;

  // Sample desc
  DXGI_SAMPLE_DESC sample_desc = {};
  sample_desc.Count   = gfx->desc.msaa_samples; 
  sample_desc.Quality = gfx->desc.msaa_samples - 1;

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

  // Set each state of the D3D11 pipeline based on 
  // the currently active state flags.
  set_d3d11_states(gfx);

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
    case GFX_DRAW_MODE_LINE:
      return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    case GFX_DRAW_MODE_LINE_STRIP:
      return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
    default:
      return (D3D11_PRIMITIVE_TOPOLOGY)0;
  }
}

static void set_uniform_buffers(GfxContext* gfx, GfxShader* shader) {
  if(shader->vertex_uniforms_count > 0) {
    gfx->device_ctx->VSSetConstantBuffers(0, shader->vertex_uniforms_count, shader->vertex_uniforms);
  }
  
  if(shader->pixel_uniforms_count > 0) {
    gfx->device_ctx->PSSetConstantBuffers(0, shader->pixel_uniforms_count, shader->pixel_uniforms);
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

static u32 get_texture_channels(const GfxTextureFormat format) {
  switch(format) {
    case GFX_TEXTURE_FORMAT_R8:
    case GFX_TEXTURE_FORMAT_R16:
      return 1;
    case GFX_TEXTURE_FORMAT_RG8:
    case GFX_TEXTURE_FORMAT_RG16:
      return 2;
    case GFX_TEXTURE_FORMAT_RGBA8:
    case GFX_TEXTURE_FORMAT_RGBA16:
      return 4;
    default:
      return 0;
  }
}

static void send_shader_uniforms(GfxContext* gfx, GfxShader* shader) {
  if(shader->vertex_uniforms_count > 0) {
    gfx->device_ctx->VSSetConstantBuffers(0, shader->vertex_uniforms_count, shader->vertex_uniforms); 
  }
  
  if(shader->pixel_uniforms_count > 0) {
    gfx->device_ctx->PSSetConstantBuffers(0, shader->pixel_uniforms_count, shader->pixel_uniforms); 
  }
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Context functions 

GfxContext* gfx_context_init(const GfxContextDesc& desc) {
  NIKOL_ASSERT(desc.window, "Invalid window passed to context");

  GfxContext* gfx = (GfxContext*)memory_allocate(sizeof(GfxContext));
  memory_zero(gfx, sizeof(GfxContext));
 
  gfx->desc = desc;

  // D3D11 init
  init_d3d11(gfx, desc.window);

  // Listening to events
  event_listen(EVENT_WINDOW_FRAMEBUFFER_RESIZED, framebuffer_resize, gfx);
  event_listen(EVENT_WINDOW_FULLSCREEN, fullscreen_callback, gfx)

  i32 dx_version     = get_dx_version_num(gfx->dx_version);
  i32 shader_version = 11;
  NIKOL_ASSERT(dx_version >= NIKOL_D3D11_MINIMUM_MAJOR_VERSION, "Cannot support Direct3D versions less than 11");

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
  
  // Direct3D11 will seg fault if this is not done
  if(gfx->swapchain) {
    gfx->swapchain->SetFullscreenState(false, NULL);
  }
  
  // Releasing all the states
  gfx->valid_blend_state->Release();
  gfx->valid_raster_state->Release();
  gfx->stencil_view->Release();
  gfx->valid_stencil_state->Release();
  gfx->stencil_buffer->Release();
  gfx->render_target->Release();

  // Releasing the devices and the swapchain
  gfx->device_ctx->Release();
  gfx->device->Release();
  gfx->swapchain->Release();

  NIKOL_LOG_INFO("The graphics context was successfully destroyed");
  memory_free(gfx);
}

void gfx_context_clear(GfxContext* gfx, const f32 r, const f32 g, const f32 b, const f32 a) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  f32 color[4] = {r, g, b, a};

  gfx->device_ctx->ClearRenderTargetView(gfx->render_target, color);
  gfx->device_ctx->ClearDepthStencilView(gfx->stencil_view, gfx->clear_flags, gfx->depth_mask, gfx->desc.stencil_desc.ref);
}

void gfx_context_present(GfxContext* gfx) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  gfx->swapchain->Present(gfx->has_vsync, 0);
}

void gfx_context_set_state(GfxContext* gfx, const GfxStates state, const bool value) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");

  set_gfx_state(gfx, state, value);
}

void gfx_context_apply_pipeline(GfxContext* gfx, GfxPipeline* pipeline, const GfxPipelineDesc& pipe_desc) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  NIKOL_ASSERT(pipeline, "Invalid GfxPipeline struct passed");
  
  // Updating the desc
  pipeline->desc = pipe_desc; 

  // Updating the shader
  pipeline->shader = pipe_desc.shader;
  
  // Create a layout if there isn't one...
  if(!pipeline->layout) {
    pipeline->layout = set_layout(gfx, pipeline->shader, pipe_desc.layout, pipe_desc.layout_count, &pipeline->stride);
  }

  // Updating the buffers
  pipeline->vertex_buffer = pipe_desc.vertex_buffer->buffer;
  pipeline->index_buffer = pipe_desc.index_buffer->buffer;

  // Updating the textures
  pipeline->textures_count = pipe_desc.textures_count; 
  for(sizei i = 0; i < pipe_desc.textures_count; i++) {
    pipeline->texture_views[i]    = pipe_desc.textures[i]->resource;
    pipeline->texture_samplers[i] = pipe_desc.textures[i]->sampler;
  }

  // Set the rasterizer state
  gfx->device_ctx->RSSetState(gfx->raster_state); 
  gfx->device_ctx->RSSetViewports(1, &gfx->viewport);
  
  // Set the output merger state
  gfx->device_ctx->OMSetRenderTargets(1, &gfx->render_target, gfx->stencil_view);
  gfx->device_ctx->OMSetDepthStencilState(gfx->stencil_state, gfx->desc.stencil_desc.ref);
  gfx->device_ctx->OMSetBlendState(gfx->blend_state, gfx->desc.blend_desc.blend_factor, 0xffffffff);

  // Set the input assembly state
  gfx->device_ctx->IASetPrimitiveTopology(get_draw_mode(pipeline->draw_mode));
  gfx->device_ctx->IASetInputLayout(pipeline->layout);
}

/// Context functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Buffer functions 

GfxBuffer* gfx_buffer_create(GfxContext* gfx, const GfxBufferDesc& desc) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
 
  GfxBuffer* buffer = (GfxBuffer*)memory_allocate(sizeof(GfxBuffer));
  memory_zero(buffer, sizeof(GfxBuffer));

  buffer->desc = desc;
  buffer->gfx  = gfx;

  D3D11_SUBRESOURCE_DATA data = {};
  data.pSysMem = desc.data;
  
  get_buffer_usage(desc.usage, &buffer->d3d11_desc.Usage, &buffer->d3d11_desc.CPUAccessFlags);
  buffer->d3d11_desc.ByteWidth           = desc.size;
  buffer->d3d11_desc.BindFlags           = get_buffer_type(desc.type);
  buffer->d3d11_desc.MiscFlags           = 0; 
  buffer->d3d11_desc.StructureByteStride = 0;
  
  HRESULT res;
  if(desc.data) {
    res = gfx->device->CreateBuffer(&buffer->d3d11_desc, &data, &buffer->buffer);
  }
  else {
    res = gfx->device->CreateBuffer(&buffer->d3d11_desc, NULL, &buffer->buffer);
  }
  check_error(res, "CreateBuffer"); 

  return buffer;
}

void gfx_buffer_destroy(GfxBuffer* buff) {
  if(!buff) {
    return;
  }
  
  buff->buffer->Release();
  memory_free(buff);
}

void gfx_buffer_update(GfxBuffer* buff, const sizei offset, const sizei size, const void* data) {
  NIKOL_ASSERT(buff->gfx, "Invalid GfxContext struct passed");
  NIKOL_ASSERT(buff, "Invalid GfxBuffer struct passed");
 
  buff->desc.size = size; 
  buff->desc.data = (void*)data;

  D3D11_MAPPED_SUBRESOURCE map_res = {};
 
  // Begin the operation to map the new data into the buffer
  HRESULT res = buff->gfx->device_ctx->Map(buff->buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map_res);
  check_error(res, "Map");

  // Copy the memory to the buffer
  memory_copy(map_res.pData, data, size);

  // We're done here
  buff->gfx->device_ctx->Unmap(buff->buffer, 0);
}

/// Buffer functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Shader functions 

GfxShader* gfx_shader_create(GfxContext* gfx, const i8* src) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  GfxShader* shader = (GfxShader*)memory_allocate(sizeof(GfxShader));
  memory_zero(shader, sizeof(GfxShader));

  shader->gfx = gfx;

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
    shader->vertex_uniforms[i]->Release();
  }
  
  for(sizei i = 0; i < shader->pixel_uniforms_count; i++) {
    shader->pixel_uniforms[i]->Release();
  }

  shader->vertex_shader->Release();
  shader->vertex_blob->Release();
  shader->pixel_shader->Release();
  shader->pixel_blob->Release();

  memory_free(shader);
}

void gfx_shader_attach_uniform(GfxShader* shader, const GfxShaderType type, GfxBuffer* buffer) {
  NIKOL_ASSERT(shader->gfx, "Invalid GfxContext struct passed");
  NIKOL_ASSERT(shader, "Invalid GfxShader struct passed");
  NIKOL_ASSERT(buffer, "Invalid GfxBuffer struct passed");

  switch(type) {
    case GFX_SHADER_VERTEX:
      shader->vertex_uniforms[shader->vertex_uniforms_count] = buffer->buffer;
      shader->vertex_uniforms_count++;
      break;
    case GFX_SHADER_PIXEL:
      shader->pixel_uniforms[shader->pixel_uniforms_count] = buffer->buffer;
      shader->pixel_uniforms_count++;
      break;
    case GFX_SHADER_GEOMETRY:
      break;
  }
}

i32 gfx_glsl_get_uniform_location(GfxShader* shader, const i8* uniform_name) {
  /// No implementation for Direct3D11...
  return 0;
}

void gfx_glsl_upload_uniform_array(GfxShader* shader, const i32 location, const sizei count, const GfxLayoutType type, const void* data) {
  /// No implementation for Direct3D11...
}

void gfx_glsl_upload_uniform(GfxShader* shader, const i32 location, const GfxLayoutType type, const void* data) {
  /// No implementation for Direct3D11...
}

/// Shader functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Texture functions 

GfxTexture* gfx_texture_create(GfxContext* gfx, const GfxTextureDesc& desc) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  GfxTexture* texture = (GfxTexture*)memory_allocate(sizeof(GfxTexture));
  memory_zero(texture, sizeof(GfxTexture));

  texture->gfx = gfx;

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

  u32 channels = get_texture_channels(desc.format);
  
  // Generate a mipmap if the texture has depth
  UINT tex_flags = desc.depth <= 0 ? 0 : D3D11_RESOURCE_MISC_GENERATE_MIPS;

  // D3D11 Texture desc
  D3D11_TEXTURE2D_DESC texture_desc = {
    .Width          = desc.width, 
    .Height         = desc.height, 
    .MipLevels      = 1, // @TODO: Need to apply depth somehow
    .ArraySize      = 1, // There's only one texture in a 2D texure
    .Format         = get_pixel_format(desc.format),
    .SampleDesc     = {gfx->desc.msaa_samples, gfx->desc.msaa_samples - 1}, 
    .Usage          = D3D11_USAGE_DEFAULT, 
    .BindFlags      = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
    .CPUAccessFlags = 0, 
    .MiscFlags      = tex_flags,
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

  // Generate the mipmap levels of the texture (if requested)
  if(desc.depth > 0) {
    gfx->device_ctx->GenerateMips(texture->resource);
  }

  texture->desc = desc;
  return texture;
}

void gfx_texture_destroy(GfxTexture* texture) {
  if(!texture) {
    return;
  }

  texture->resource->Release();
  texture->handle->Release();
  texture->sampler->Release();

  memory_free(texture);
}

void gfx_texture_update(GfxTexture* texture, const GfxTextureDesc& desc) {
  NIKOL_ASSERT(texture->gfx, "Invalid GfxContext struct passed");
  NIKOL_ASSERT(texture, "Invalid GfxTexture struct passed");

  // @TODO:
}

/// Texture functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Pipeline functions 

GfxPipeline* gfx_pipeline_create(GfxContext* gfx, const GfxPipelineDesc& desc) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  GfxPipeline* pipeline = (GfxPipeline*)memory_allocate(sizeof(GfxPipeline));
  memory_zero(pipeline, sizeof(GfxPipeline));

  pipeline->desc = desc;
  pipeline->gfx  = gfx;

  // Create the vertex buffer 
  pipeline->vertex_buffer = desc.vertex_buffer->buffer; 

  // Create the index buffer
  pipeline->index_buffer = desc.index_buffer->buffer; 

  // Setting the shader 
  pipeline->shader = desc.shader;

  // Only create the layout if there's a shader
  if(pipeline->shader) {
    pipeline->layout = set_layout(gfx, pipeline->shader, desc.layout, desc.layout_count, &pipeline->stride);
  }

  // Set the draw mode for the pipeline 
  pipeline->draw_mode = desc.draw_mode;

  // Setting the textures 
  pipeline->textures_count = desc.textures_count; 
  for(sizei i = 0; i < desc.textures_count; i++) {
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

  // Free the pipeline
  memory_free(pipeline);
}

void gfx_pipeline_draw_vertex(GfxPipeline* pipeline) {
  NIKOL_ASSERT(pipeline->gfx, "Invalid GfxContext struct passed");
  NIKOL_ASSERT(pipeline, "Invalid GfxPipeline struct passed");
  NIKOL_ASSERT(pipeline->desc.vertex_buffer, "Must have a valid vertex buffer to draw");
  
  // Set the buffer
  pipeline->gfx->device_ctx->IASetVertexBuffers(0, 1, &pipeline->vertex_buffer, &pipeline->stride, &pipeline->offset);
  
  // Set the constant buffers in the shader
  send_shader_uniforms(pipeline->gfx, pipeline->shader); 
  
  // Set the shaders
  pipeline->gfx->device_ctx->VSSetShader(pipeline->shader->vertex_shader, NULL, 0);
  pipeline->gfx->device_ctx->PSSetShader(pipeline->shader->pixel_shader, NULL, 0);

  // Setting the textures (if there are any)
  if(pipeline->textures_count > 0) {
    pipeline->gfx->device_ctx->PSSetShaderResources(0, pipeline->textures_count, pipeline->texture_views);
    pipeline->gfx->device_ctx->PSSetSamplers(0, pipeline->textures_count, pipeline->texture_samplers);
  }

  // Draw the vertex buffer
  pipeline->gfx->device_ctx->Draw(pipeline->desc.vertices_count, 0);
}

void gfx_pipeline_draw_index(GfxPipeline* pipeline) {
  NIKOL_ASSERT(pipeline->gfx, "Invalid GfxContext struct passed");
  NIKOL_ASSERT(pipeline, "Invalid GfxPipeline struct passed");
  NIKOL_ASSERT(pipeline->desc.vertex_buffer, "Must have a valid vertex buffer to draw");
  NIKOL_ASSERT(pipeline->desc.index_buffer, "Must have a valid index buffer to draw");
  
  // Set the buffers
  pipeline->gfx->device_ctx->IASetIndexBuffer(pipeline->index_buffer, DXGI_FORMAT_R32_UINT, 0); 
  pipeline->gfx->device_ctx->IASetVertexBuffers(0, 1, &pipeline->vertex_buffer, &pipeline->stride, &pipeline->offset);
  
  // Set the constant buffers in the shader
  send_shader_uniforms(pipeline->gfx, pipeline->shader); 

  // Set the shaders
  pipeline->gfx->device_ctx->VSSetShader(pipeline->shader->vertex_shader, NULL, 0);
  pipeline->gfx->device_ctx->PSSetShader(pipeline->shader->pixel_shader, NULL, 0);
  
  // Setting the textures (if there are any)
  if(pipeline->textures_count > 0) {
    pipeline->gfx->device_ctx->PSSetShaderResources(0, pipeline->textures_count, pipeline->texture_views);
    pipeline->gfx->device_ctx->PSSetSamplers(0, pipeline->textures_count, pipeline->texture_samplers);
    
    pipeline->gfx->device_ctx->VSSetShaderResources(0, pipeline->textures_count, pipeline->texture_views);
    pipeline->gfx->device_ctx->VSSetSamplers(0, pipeline->textures_count, pipeline->texture_samplers);
  }

  // Draw the index buffer
  pipeline->gfx->device_ctx->DrawIndexed(pipeline->desc.indices_count, 0, 0);
}

/// Pipeline functions 
///---------------------------------------------------------------------------------------------------------------------

/// *** Graphics ***
/// ---------------------------------------------------------------------

} // End of nikol

#endif // DirectX11 check

//////////////////////////////////////////////////////////////////////////
