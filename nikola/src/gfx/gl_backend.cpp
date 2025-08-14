#include "nikola/nikola_gfx.h"
#include "nikola/nikola_event.h"
#include "nikola/nikola_containers.h"

//////////////////////////////////////////////////////////////////////////

#include <glad/glad.h>

#include <cstring>

namespace nikola { // Start of nikola

/// ---------------------------------------------------------------------
/// *** Graphics ***

///---------------------------------------------------------------------------------------------------------------------
/// Macros

#define SET_GFX_STATE(value, state) { \
  if(value) {                         \
    glEnable(state);                  \
  }                                   \
  else {                              \
    glDisable(state);                 \
  }                                   \
}

#define SET_BUFFER_BIT(value, bits, buffer) { \
  if(value) {                                 \
    SET_BIT(bits, buffer);                    \
  }                                           \
  else {                                      \
    UNSET_BIT(bits, buffer);                  \
  }                                           \
}

/// Macros
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// _PipelineLayout
struct _PipelineLayout {
  sizei offsets[VERTEX_LAYOUTS_MAX];
  sizei strides[VERTEX_LAYOUTS_MAX]; 
};
/// _PipelineLayout
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxContext
struct GfxContext {
  GfxContextDesc desc = {};
  GfxStates states;

  u32 current_target = 0; 

  u32 default_clear_flags = 0;
  u32 current_clear_flags = 0;

  GfxPipeline* bound_pipeline = nullptr;
};
/// GfxContext
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxFramebuffer
struct GfxFramebuffer {
  GfxFramebufferDesc desc = {};
  
  u32 clear_flags;
  u32 id;

  u32 color_textures[FRAMEBUFFER_ATTACHMENTS_MAX] = {GL_NONE, GL_NONE, GL_NONE, GL_NONE};
  u32 depth_texture   = GL_NONE; 
  u32 stencil_texture = GL_NONE;
};
/// GfxFramebuffer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBuffer  
struct GfxBuffer {
  GfxBufferDesc desc = {};
  GfxContext* gfx    = nullptr;

  u32 id;

  GLenum gl_buff_type; 
  GLenum gl_buff_usage;
};
/// GfxBuffer  
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxShader
struct GfxShader {
  GfxContext* gfx    = nullptr;
  GfxShaderDesc desc = {};

  u32 id; 
  u32 vert_id, frag_id, compute_id;
};
/// GfxShader
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTexture
struct GfxTexture {
  GfxTextureDesc desc = {};
  GfxContext* gfx     = nullptr;

  u32 id;
};
/// GfxTexture
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxCubemap
struct GfxCubemap {
  GfxCubemapDesc desc = {};
  GfxContext* gfx     = nullptr;
  
  u32 id;
};
/// GfxCubemap
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxPipeline
struct GfxPipeline {
  GfxPipelineDesc desc = {};
  GfxContext* gfx      = nullptr;

  u32 vertex_array;

  GfxBuffer* vertex_buffer = nullptr;
  sizei vertex_count       = 0;

  GfxBuffer* index_buffer = nullptr; 
  sizei index_count       = 0;

  GfxBuffer* instance_buffer = nullptr;

  GfxDrawMode draw_mode;
};
/// GfxPipeline
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Callbacks 

static bool framebuffer_resize(const Event& event, const void* disp, const void* list) {
  if(event.type != EVENT_WINDOW_FRAMEBUFFER_RESIZED) {
    return false;
  }

 glViewport(0, 0, event.window_framebuffer_width, event.window_framebuffer_height);

  return true;
}

/// Callbacks 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions 

static void check_supported_gl_version(const i32 major, const i32 minor) {
  NIKOLA_ASSERT((major >= NIKOLA_GL_MINIMUM_MAJOR_VERSION) && (minor >= NIKOLA_GL_MINIMUM_MINOR_VERSION), 
               "OpenGL versions less than 4.2 are not supported");
}

static const char* gl_get_error_source(GLenum src) {
  switch(src) {
    case GL_DEBUG_SOURCE_API: 
      return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      return "WINDOW_SYSTEM";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      return "SHADER";
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      return "THIRD_PARTY";
    case GL_DEBUG_SOURCE_APPLICATION:
      return "APPLICATION";
    case GL_DEBUG_SOURCE_OTHER:
      return "OTHER";
    default:
      return "DEF";
  }
}

static const char* get_gl_error_type(GLenum type) {
  switch(type) {
    case GL_DEBUG_TYPE_ERROR:
      return "ERROR";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      return "DEPRECATED";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      return "UNDEFINED_BEHAVIOR";
    case GL_DEBUG_TYPE_PORTABILITY:
      return "PORTABILITY";
    case GL_DEBUG_TYPE_PERFORMANCE:
      return "PERFORMANCE";
    case GL_DEBUG_TYPE_OTHER:
      return "OTHER";
    default:
      return "DEF";
  }
}

static void gl_error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei len, const GLchar* msg, const void* usr_param) {
  switch(severity) {
    case GL_DEBUG_SEVERITY_HIGH: 
      NIKOLA_LOG_FATAL("GL-BACKEND: %s-%s (ID = %i): %s", gl_get_error_source(source), get_gl_error_type(type), id, msg);
      break;
    case GL_DEBUG_SEVERITY_MEDIUM: 
      NIKOLA_LOG_ERROR("GL-BACKEND: %s-%s (ID = %i): %s", gl_get_error_source(source), get_gl_error_type(type), id, msg);
      break;
    case GL_DEBUG_SEVERITY_LOW: 
      NIKOLA_LOG_WARN("GL-BACKEND: %s-%s (ID = %i): %s", gl_get_error_source(source), get_gl_error_type(type), id, msg);
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: 
      NIKOLA_LOG_DEBUG("GL-BACKEND: %s-%s (ID = %i): %s", gl_get_error_source(source), get_gl_error_type(type), id, msg);
      break;
    default:
      break;
  }
}

static GLbitfield get_gl_barrier(const GfxMemoryBarrierType func) {
  switch(func) {
    case GFX_MEMORY_BARRIER_VERTEX_ATTRIBUTE:
      return GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT;
    case GFX_MEMORY_BARRIER_INDEX_BUFFER:
      return GL_ELEMENT_ARRAY_BARRIER_BIT;
    case GFX_MEMORY_BARRIER_UNIFORM_BUFFER:
      return GL_UNIFORM_BARRIER_BIT;
    case GFX_MEMORY_BARRIER_SHADER_STORAGE_BUFFER:
      return GL_SHADER_STORAGE_BARRIER_BIT;
    case GFX_MEMORY_BARRIER_FRAMEBUFFER:
      return GL_FRAMEBUFFER_BARRIER_BIT;
    case GFX_MEMORY_BARRIER_BUFFER_UPDATE:
      return GL_BUFFER_UPDATE_BARRIER_BIT;
    case GFX_MEMORY_BARRIER_TEXTURE_FETCH:
      return GL_TEXTURE_FETCH_BARRIER_BIT;
    case GFX_MEMORY_BARRIER_TEXTURE_UPDATE:
      return GL_TEXTURE_UPDATE_BARRIER_BIT;
    case GFX_MEMORY_BARRIER_SHADER_IMAGE_ACCESS:
      return GL_SHADER_IMAGE_ACCESS_BARRIER_BIT;
    case GFX_MEMORY_BARRIER_ATOMIC_COUNTER:
      return GL_ATOMIC_COUNTER_BARRIER_BIT;
    case GFX_MEMORY_BARRIER_ALL:
      return GL_ALL_BARRIER_BITS;
    default:
      return 0;
  } 
}

static GLenum get_gl_compare_func(const GfxCompareFunc func) {
  switch(func) {
    case GFX_COMPARE_ALWAYS:
      return GL_ALWAYS;
    case GFX_COMPARE_NEVER:
      return GL_NEVER;
    case GFX_COMPARE_EQUAL:
      return GL_EQUAL;
    case GFX_COMPARE_LESS:
      return GL_LESS;
    case GFX_COMPARE_LESS_EQUAL:
      return GL_LEQUAL;
    case GFX_COMPARE_GREATER:
      return GL_GREATER;
    case GFX_COMPARE_GREATER_EQUAL:
      return GL_GEQUAL;
    case GFX_COMPARE_NOT_EQUAL:
      return GL_NOTEQUAL;
    default:
      return 0;
  } 
}

static GLenum get_gl_operation(const GfxOperation op) {
  switch(op) {
    case GFX_OP_KEEP:
      return GL_KEEP;
    case GFX_OP_ZERO:
      return GL_ZERO;
    case GFX_OP_INVERT:
      return GL_INVERT;
    case GFX_OP_REPLACE:
      return GL_REPLACE;
    case GFX_OP_INCR:
      return GL_INCR;
    case GFX_OP_DECR:
      return GL_DECR;
    case GFX_OP_INCR_WRAP:
      return GL_INCR_WRAP;
    case GFX_OP_DECR_WRAP:
      return GL_DECR_WRAP;
    default:
      return 0;
  }
}

static GLenum get_gl_blend_mode(const GfxBlendMode mode) {
  switch(mode) {
    case GFX_BLEND_ZERO:
      return GL_ZERO;
    case GFX_BLEND_ONE:
      return GL_ONE;
    case GFX_BLEND_SRC_COLOR:
      return GL_SRC_COLOR;
    case GFX_BLEND_DEST_COLOR:
      return GL_DST_COLOR;
    case GFX_BLEND_SRC_ALPHA:
      return GL_SRC_ALPHA;
    case GFX_BLEND_DEST_ALPHA:
      return GL_DST_ALPHA;
    case GFX_BLEND_INV_SRC_COLOR:
      return GL_ONE_MINUS_SRC_COLOR;
    case GFX_BLEND_INV_DEST_COLOR:
      return GL_ONE_MINUS_DST_COLOR;
    case GFX_BLEND_INV_SRC_ALPHA:
      return GL_ONE_MINUS_SRC_ALPHA;
    case GFX_BLEND_INV_DEST_ALPHA:
      return GL_ONE_MINUS_DST_ALPHA;
    case GFX_BLEND_SRC_ALPHA_SATURATE:
      return GL_SRC_ALPHA_SATURATE;
    default:
      return 0;
  }
}

static GLenum get_gl_cull_order(const GfxCullOrder order) {
  switch(order) {
    case GFX_ORDER_CLOCKWISE:
      return GL_CW;
    case GFX_ORDER_COUNTER_CLOCKWISE:
      return GL_CCW;
    default:
      return 0;
  }
}

static GLenum get_gl_cull_mode(const GfxCullMode mode) {
  switch(mode) {
    case GFX_CULL_FRONT:
      return GL_FRONT;
    case GFX_CULL_BACK:
      return GL_BACK;
    case GFX_CULL_FRONT_AND_BACK:
      return GL_FRONT_AND_BACK;
    default:
      return 0;
  }
}

static GLenum get_buffer_type(const GfxBufferType type) {
  switch(type) {
    case GFX_BUFFER_VERTEX:
      return GL_ARRAY_BUFFER;
    case GFX_BUFFER_INDEX:
      return GL_ELEMENT_ARRAY_BUFFER;
    case GFX_BUFFER_UNIFORM:
      return GL_UNIFORM_BUFFER;
    case GFX_BUFFER_SHADER_STORAGE:
      return GL_SHADER_STORAGE_BUFFER;
  } 
}

static GLenum get_buffer_usage(const GfxBufferUsage usage) {
  switch(usage) {
    case GFX_BUFFER_USAGE_DYNAMIC_DRAW:
      return GL_DYNAMIC_DRAW;
    case GFX_BUFFER_USAGE_DYNAMIC_READ:
      return GL_DYNAMIC_READ;
    case GFX_BUFFER_USAGE_STATIC_DRAW:
      return GL_STATIC_DRAW;
    case GFX_BUFFER_USAGE_STATIC_READ:
      return GL_STATIC_READ;
    default:
      return 0;
  }
}

static GLenum get_draw_mode(const GfxDrawMode mode) {
  switch(mode) {
    case GFX_DRAW_MODE_POINT:
      return GL_POINTS;
    case GFX_DRAW_MODE_TRIANGLE:
      return GL_TRIANGLES;
    case GFX_DRAW_MODE_TRIANGLE_STRIP:
      return GL_TRIANGLE_STRIP;
    case GFX_DRAW_MODE_LINE:
      return GL_LINES;
    case GFX_DRAW_MODE_LINE_STRIP:
      return GL_LINE_STRIP;
    default:
      return 0;
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

    case GFX_LAYOUT_BYTE1:
    case GFX_LAYOUT_UBYTE1:
      return sizeof(i8);
    case GFX_LAYOUT_BYTE2:
    case GFX_LAYOUT_UBYTE2:
      return sizeof(i8) * 2;
    case GFX_LAYOUT_BYTE3:
    case GFX_LAYOUT_UBYTE3:
      return sizeof(i8) * 3;
    case GFX_LAYOUT_BYTE4:
    case GFX_LAYOUT_UBYTE4:
      return sizeof(i8) * 4;

    case GFX_LAYOUT_SHORT1:
    case GFX_LAYOUT_USHORT1:
      return sizeof(i16);
    case GFX_LAYOUT_SHORT2:
    case GFX_LAYOUT_USHORT2:
      return sizeof(i16) * 2;
    case GFX_LAYOUT_SHORT3:
    case GFX_LAYOUT_USHORT3:
      return sizeof(i16) * 3;
    case GFX_LAYOUT_SHORT4:
    case GFX_LAYOUT_USHORT4:
      return sizeof(i16) * 4;

    case GFX_LAYOUT_INT1:
    case GFX_LAYOUT_UINT1:
      return sizeof(i32);
    case GFX_LAYOUT_INT2:
    case GFX_LAYOUT_UINT2:
      return sizeof(i32) * 2;
    case GFX_LAYOUT_INT3:
    case GFX_LAYOUT_UINT3:
      return sizeof(i32) * 3;
    case GFX_LAYOUT_INT4:
    case GFX_LAYOUT_UINT4:
      return sizeof(i32) * 4;

    default: 
      return 0;
  }
}

static sizei get_layout_type(const GfxLayoutType layout) {
  switch(layout) {
    case GFX_LAYOUT_FLOAT1:
    case GFX_LAYOUT_FLOAT2:
    case GFX_LAYOUT_FLOAT3:
    case GFX_LAYOUT_FLOAT4:
    case GFX_LAYOUT_MAT3:
    case GFX_LAYOUT_MAT4:
      return GL_FLOAT;
    case GFX_LAYOUT_BYTE1:
    case GFX_LAYOUT_BYTE2:
    case GFX_LAYOUT_BYTE3:
    case GFX_LAYOUT_BYTE4:
      return GL_BYTE;
    case GFX_LAYOUT_UBYTE1:
    case GFX_LAYOUT_UBYTE2:
    case GFX_LAYOUT_UBYTE3:
    case GFX_LAYOUT_UBYTE4:
      return GL_UNSIGNED_BYTE;
    case GFX_LAYOUT_SHORT1:
    case GFX_LAYOUT_SHORT2:
    case GFX_LAYOUT_SHORT3:
    case GFX_LAYOUT_SHORT4:
      return GL_SHORT;
    case GFX_LAYOUT_USHORT1:
    case GFX_LAYOUT_USHORT2:
    case GFX_LAYOUT_USHORT3:
    case GFX_LAYOUT_USHORT4:
      return GL_UNSIGNED_SHORT;
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

static sizei get_layout_count(const GfxLayoutType layout) {
  switch(layout) {
    case GFX_LAYOUT_FLOAT1:
    case GFX_LAYOUT_BYTE1:
    case GFX_LAYOUT_UBYTE1:
    case GFX_LAYOUT_SHORT1:
    case GFX_LAYOUT_USHORT1:
    case GFX_LAYOUT_INT1:
    case GFX_LAYOUT_UINT1:
      return 1;
    case GFX_LAYOUT_FLOAT2:
    case GFX_LAYOUT_BYTE2:
    case GFX_LAYOUT_UBYTE2:
    case GFX_LAYOUT_SHORT2:
    case GFX_LAYOUT_USHORT2:
    case GFX_LAYOUT_INT2:
    case GFX_LAYOUT_UINT2:
      return 2;
    case GFX_LAYOUT_FLOAT3:
    case GFX_LAYOUT_BYTE3:
    case GFX_LAYOUT_UBYTE3:
    case GFX_LAYOUT_SHORT3:
    case GFX_LAYOUT_USHORT3:
    case GFX_LAYOUT_INT3:
    case GFX_LAYOUT_UINT3:
      return 3;
    case GFX_LAYOUT_FLOAT4:
    case GFX_LAYOUT_BYTE4:
    case GFX_LAYOUT_UBYTE4:
    case GFX_LAYOUT_SHORT4:
    case GFX_LAYOUT_USHORT4:
    case GFX_LAYOUT_INT4:
    case GFX_LAYOUT_UINT4:
      return 4;
    default:
      return 0;
  }
}

static void get_texture_gl_format(const GfxTextureFormat format, GLenum* in_format, GLenum* gl_format, GLenum* gl_type) {
  switch(format) {
    case GFX_TEXTURE_FORMAT_R8:
      *in_format = GL_R8;
      *gl_format = GL_RED;
      *gl_type   = GL_UNSIGNED_BYTE;
      break;
    case GFX_TEXTURE_FORMAT_R16:
      *in_format = GL_R16;
      *gl_format = GL_RED;
      *gl_type   = GL_UNSIGNED_SHORT;
      break;
    case GFX_TEXTURE_FORMAT_R16F:
      *in_format = GL_R16F;
      *gl_format = GL_RED;
      *gl_type   = GL_FLOAT;
      break;
    case GFX_TEXTURE_FORMAT_R32F:
      *in_format = GL_R32F;
      *gl_format = GL_RED;
      *gl_type   = GL_FLOAT;
      break;
    case GFX_TEXTURE_FORMAT_RG8:
      *in_format = GL_RG8;
      *gl_format = GL_RG;
      *gl_type   = GL_UNSIGNED_BYTE;
      break;
    case GFX_TEXTURE_FORMAT_RG16:
      *in_format = GL_RG16;
      *gl_format = GL_RG;
      *gl_type   = GL_UNSIGNED_SHORT;
      break;
    case GFX_TEXTURE_FORMAT_RG16F:
      *in_format = GL_RG16F;
      *gl_format = GL_RG;
      *gl_type   = GL_FLOAT;
      break;
    case GFX_TEXTURE_FORMAT_RG32F:
      *in_format = GL_RG32F;
      *gl_format = GL_RG;
      *gl_type   = GL_FLOAT;
      break;
    case GFX_TEXTURE_FORMAT_RGBA8:
      *in_format = GL_RGBA8;
      *gl_format = GL_RGBA;
      *gl_type   = GL_UNSIGNED_BYTE;
      break;
    case GFX_TEXTURE_FORMAT_RGBA16:
      *in_format = GL_RGBA16;
      *gl_format = GL_RGBA;
      *gl_type   = GL_UNSIGNED_SHORT;
      break;
    case GFX_TEXTURE_FORMAT_RGBA16F:
      *in_format = GL_RGBA16F;
      *gl_format = GL_RGBA;
      *gl_type   = GL_FLOAT;
      break;
    case GFX_TEXTURE_FORMAT_RGBA32F:
      *in_format = GL_RGBA32F;
      *gl_format = GL_RGBA;
      *gl_type   = GL_FLOAT;
      break;
    case GFX_TEXTURE_FORMAT_DEPTH16:
      *in_format = GL_DEPTH_COMPONENT16;
      *gl_format = GL_DEPTH_COMPONENT;
      *gl_type   = GL_UNSIGNED_SHORT;
    case GFX_TEXTURE_FORMAT_DEPTH24:
      *in_format = GL_DEPTH_COMPONENT24;
      *gl_format = GL_DEPTH_COMPONENT;
      *gl_type   = GL_UNSIGNED_INT;
    case GFX_TEXTURE_FORMAT_DEPTH32F:
      *in_format = GL_DEPTH_COMPONENT32F;
      *gl_format = GL_DEPTH_COMPONENT;
      *gl_type   = GL_FLOAT;
    case GFX_TEXTURE_FORMAT_STENCIL8:
      *in_format = GL_STENCIL_INDEX8;
      *gl_format = GL_STENCIL_INDEX;
      *gl_type   = GL_UNSIGNED_BYTE;
    case GFX_TEXTURE_FORMAT_DEPTH_STENCIL_24_8:
      *in_format = GL_DEPTH24_STENCIL8;
      *gl_format = GL_DEPTH_STENCIL;
      *gl_type   = GL_UNSIGNED_INT_24_8;
      break;
    default:
      break;
  }
}

static void get_texture_gl_filter(const GfxTextureFilter filter, GLenum* min, GLenum* mag) {
  switch(filter) {
    case GFX_TEXTURE_FILTER_MIN_MAG_LINEAR:
      *min = GL_LINEAR; 
      *mag = GL_LINEAR;
      break;
    case GFX_TEXTURE_FILTER_MIN_MAG_NEAREST:
      *min = GL_NEAREST; 
      *mag = GL_NEAREST;
      break;
    case GFX_TEXTURE_FILTER_MIN_LINEAR_MAG_NEAREST:
      *min = GL_LINEAR; 
      *mag = GL_NEAREST;
      break;
    case GFX_TEXTURE_FILTER_MIN_NEAREST_MAG_LINEAR:
      *min = GL_NEAREST; 
      *mag = GL_LINEAR;
      break;
    case GFX_TEXTURE_FILTER_MIN_TRILINEAR_MAG_LINEAR:
      *min = GL_LINEAR_MIPMAP_LINEAR; 
      *mag = GL_LINEAR;
      break;
    case GFX_TEXTURE_FILTER_MIN_TRILINEAR_MAG_NEAREST:
      *min = GL_LINEAR_MIPMAP_LINEAR; 
      *mag = GL_NEAREST;
      break;
    default:
      break;
  }
}

static GLenum get_texture_gl_wrap(const GfxTextureWrap wrap) {
  switch(wrap) {
    case GFX_TEXTURE_WRAP_REPEAT: 
      return GL_REPEAT;
    case GFX_TEXTURE_WRAP_MIRROR: 
      return GL_MIRRORED_REPEAT;
    case GFX_TEXTURE_WRAP_CLAMP: 
      return GL_CLAMP_TO_EDGE;
    case GFX_TEXTURE_WRAP_BORDER_COLOR:
      return GL_CLAMP_TO_BORDER;
    default:
      return 0;
  }
}

static GLenum get_texture_gl_access(const GfxTextureAccess access) {
  switch(access) {
    case GFX_TEXTURE_ACCESS_READ: 
      return GL_READ_ONLY;
    case GFX_TEXTURE_ACCESS_WRITE: 
      return GL_WRITE_ONLY;
    case GFX_TEXTURE_ACCESS_READ_WRITE: 
      return GL_READ_WRITE;
    default:
      return 0;
  }
}

static GfxUniformType get_shader_type(const GLenum gl_type) {
  switch(gl_type) {
    case GL_FLOAT: 
      return GFX_UNIFORM_FLOAT1;
    case GL_FLOAT_VEC2: 
      return GFX_UNIFORM_FLOAT2;
    case GL_FLOAT_VEC3: 
      return GFX_UNIFORM_FLOAT3;
    case GL_FLOAT_VEC4: 
      return GFX_UNIFORM_FLOAT4;
    
    case GL_INT: 
      return GFX_UNIFORM_INT1;
    case GL_INT_VEC2: 
      return GFX_UNIFORM_INT2;
    case GL_INT_VEC3: 
      return GFX_UNIFORM_INT3;
    case GL_INT_VEC4: 
      return GFX_UNIFORM_INT4;
    
    case GL_UNSIGNED_INT: 
      return GFX_UNIFORM_UINT1;
    case GL_UNSIGNED_INT_VEC2: 
      return GFX_UNIFORM_UINT2;
    case GL_UNSIGNED_INT_VEC3: 
      return GFX_UNIFORM_UINT3;
    case GL_UNSIGNED_INT_VEC4: 
      return GFX_UNIFORM_UINT4;
    
    case GL_BOOL: 
      return GFX_UNIFORM_BOOL1;
    case GL_BOOL_VEC2: 
      return GFX_UNIFORM_BOOL2;
    case GL_BOOL_VEC3: 
      return GFX_UNIFORM_BOOL3;
    case GL_BOOL_VEC4: 
      return GFX_UNIFORM_BOOL4;
    
    case GL_FLOAT_MAT2: 
      return GFX_UNIFORM_MAT2;
    case GL_FLOAT_MAT3: 
      return GFX_UNIFORM_MAT3;
    case GL_FLOAT_MAT4: 
      return GFX_UNIFORM_MAT4;
    case GL_FLOAT_MAT2x3: 
      return GFX_UNIFORM_MAT2X3;
    case GL_FLOAT_MAT2x4: 
      return GFX_UNIFORM_MAT2X4;
    case GL_FLOAT_MAT3x2: 
      return GFX_UNIFORM_MAT3X2;
    case GL_FLOAT_MAT3x4: 
      return GFX_UNIFORM_MAT3X4;
    case GL_FLOAT_MAT4x2: 
      return GFX_UNIFORM_MAT4X2;
    case GL_FLOAT_MAT4x3: 
      return GFX_UNIFORM_MAT4X3;
    
    case GL_SAMPLER_1D: 
      return GFX_UNIFORM_SAMPLER_1D;
    case GL_SAMPLER_2D: 
      return GFX_UNIFORM_SAMPLER_2D;
    case GL_SAMPLER_3D: 
      return GFX_UNIFORM_SAMPLER_3D;
    case GL_SAMPLER_CUBE: 
      return GFX_UNIFORM_SAMPLER_CUBE;
    
    case GL_SAMPLER_1D_SHADOW: 
      return GFX_UNIFORM_SAMPLER_1D_SHADOW;
    case GL_SAMPLER_2D_SHADOW: 
      return GFX_UNIFORM_SAMPLER_2D_SHADOW;
    case GL_SAMPLER_CUBE_SHADOW: 
      return GFX_UNIFORM_SAMPLER_CUBE_SHADOW;
    
    case GL_SAMPLER_1D_ARRAY: 
      return GFX_UNIFORM_SAMPLER_1D_ARRAY;
    case GL_SAMPLER_2D_ARRAY: 
      return GFX_UNIFORM_SAMPLER_2D_ARRAY;
    case GL_SAMPLER_1D_ARRAY_SHADOW: 
      return GFX_UNIFORM_SAMPLER_1D_ARRAY_SHADOW;
    case GL_SAMPLER_2D_ARRAY_SHADOW: 
      return GFX_UNIFORM_SAMPLER_2D_ARRAY_SHADOW;
    
    case GL_IMAGE_1D: 
      return GFX_UNIFORM_IMAGE_1D;
    case GL_IMAGE_2D: 
      return GFX_UNIFORM_IMAGE_2D;
    case GL_IMAGE_3D: 
      return GFX_UNIFORM_IMAGE_3D;
    case GL_IMAGE_CUBE: 
      return GFX_UNIFORM_IMAGE_CUBE;
    
    case GL_IMAGE_1D_ARRAY: 
      return GFX_UNIFORM_IMAGE_1D_ARRAY;
    case GL_IMAGE_2D_ARRAY: 
      return GFX_UNIFORM_IMAGE_2D_ARRAY;

    default: 
      return (GfxUniformType)-1;
  }
}

static void set_state(GfxContext* gfx, const GfxStates state, const bool value) {
  switch(state) {
    case GFX_STATE_DEPTH:
      SET_GFX_STATE(value, GL_DEPTH_TEST);
      break;
    case GFX_STATE_STENCIL:
      SET_GFX_STATE(value, GL_STENCIL_TEST);
      break;
    case GFX_STATE_BLEND:
      SET_GFX_STATE(value, GL_BLEND);
      break;
    case GFX_STATE_MSAA:
      SET_GFX_STATE(value, GL_MULTISAMPLE);
      break;
    case GFX_STATE_CULL:
      SET_GFX_STATE(value, GL_CULL_FACE);
    case GFX_STATE_SCISSOR:
      SET_GFX_STATE(value, GL_SCISSOR_TEST);
      break;
  }
}

static void set_depth_state(GfxContext* gfx) {
  GLenum func = get_gl_compare_func(gfx->desc.depth_desc.compare_func);

  glDepthFunc(func);
  glDepthMask(gfx->desc.depth_desc.depth_write_enabled);
}

static void set_stencil_state(GfxContext* gfx) {
  GLenum func  = get_gl_compare_func(gfx->desc.stencil_desc.compare_func);
  GLenum face  = get_gl_cull_mode(gfx->desc.stencil_desc.polygon_face); 
  GLenum sfail = get_gl_operation(gfx->desc.stencil_desc.stencil_fail_op); 
  GLenum dfail = get_gl_operation(gfx->desc.stencil_desc.depth_fail_op); 
  GLenum dpass = get_gl_operation(gfx->desc.stencil_desc.depth_pass_op); 

  glStencilFuncSeparate(face, func, gfx->desc.stencil_desc.ref, gfx->desc.stencil_desc.mask);
  glStencilOpSeparate(face, sfail, dfail, dpass);
  glStencilMaskSeparate(face, gfx->desc.stencil_desc.mask);
}

static void set_blend_state(GfxContext* gfx) {
  GLenum src_color = get_gl_blend_mode(gfx->desc.blend_desc.src_color_blend);
  GLenum dst_color = get_gl_blend_mode(gfx->desc.blend_desc.dest_color_blend);

  GLenum src_alpha = get_gl_blend_mode(gfx->desc.blend_desc.src_alpha_blend);
  GLenum dst_alpha = get_gl_blend_mode(gfx->desc.blend_desc.dest_alpha_blend);

  f32* factor = gfx->desc.blend_desc.blend_factor;
  
  glBlendFuncSeparate(src_color, dst_color, src_alpha, dst_alpha);
  glBlendColor(factor[0], factor[1], factor[2], factor[3]);
}

static void set_cull_state(GfxContext* gfx) {
  GLenum front_face = get_gl_cull_order(gfx->desc.cull_desc.front_face);
  GLenum face       = get_gl_cull_mode(gfx->desc.cull_desc.cull_mode);
  
  glCullFace(face);
  glFrontFace(front_face);
}

static void set_gfx_states(GfxContext* gfx) {
  set_depth_state(gfx);
  set_stencil_state(gfx);
  set_cull_state(gfx);
  set_blend_state(gfx);

  if(IS_BIT_SET(gfx->states, GFX_STATE_DEPTH)) {
    set_state(gfx, GFX_STATE_DEPTH, true);   
    gfx->default_clear_flags |= GL_DEPTH_BUFFER_BIT;
  }
  
  if(IS_BIT_SET(gfx->states, GFX_STATE_STENCIL)) {
    set_state(gfx, GFX_STATE_STENCIL, true);   
    gfx->default_clear_flags |= GL_STENCIL_BUFFER_BIT;
  }
  
  if(IS_BIT_SET(gfx->states, GFX_STATE_BLEND)) {
    set_state(gfx, GFX_STATE_BLEND, true);   
  }
  
  if(IS_BIT_SET(gfx->states, GFX_STATE_MSAA)) {
    set_state(gfx, GFX_STATE_MSAA, true);   
  }

  if(IS_BIT_SET(gfx->states, GFX_STATE_CULL)) {
    set_state(gfx, GFX_STATE_CULL, true);   
  }
  
  if(IS_BIT_SET(gfx->states, GFX_STATE_SCISSOR)) {
    set_state(gfx, GFX_STATE_SCISSOR, true);   
  }
}

static u32 get_gl_clear_flags(const u32 flags) {
  u32 gl_flags = 0;
  
  if(IS_BIT_SET(flags, GFX_CLEAR_FLAGS_NONE)) {
    return 0;
  }

  if(IS_BIT_SET(flags, GFX_CLEAR_FLAGS_COLOR_BUFFER)) {
    gl_flags |= GL_COLOR_BUFFER_BIT;    
  }
  
  if(IS_BIT_SET(flags, GFX_CLEAR_FLAGS_DEPTH_BUFFER)) {
    gl_flags |= GL_DEPTH_BUFFER_BIT;    
  }
  
  if(IS_BIT_SET(flags, GFX_CLEAR_FLAGS_STENCIL_BUFFER)) {
    gl_flags |= GL_STENCIL_BUFFER_BIT;    
  }

  return gl_flags;
}

static void init_pipeline_layout(const GfxPipeline* pipe, _PipelineLayout* layout) {
  sizei stride = 0;

  // Set the layouts of the buffers

  for(sizei i = 0; i < VERTEX_LAYOUTS_MAX; i++) {
    NIKOLA_ASSERT((pipe->desc.layouts[i].attributes_count >= 0) && (pipe->desc.layouts[i].attributes_count < VERTEX_ATTRIBUTES_MAX), 
                   "Attributes count cannot exceed VERTEX_ATTRIBUTES_MAX");

    // Set the attributes of the buffer

    sizei start = pipe->desc.layouts[i].start_index;

    for(sizei j = start; j < start + pipe->desc.layouts[i].attributes_count; j++) {
      GfxLayoutType attribute = pipe->desc.layouts[i].attributes[j];
      
      GLenum gl_comp_type = get_layout_type(attribute);
      sizei comp_count    = get_layout_count(attribute);
      sizei size          = get_layout_size(attribute);

      glEnableVertexArrayAttrib(pipe->vertex_array, j);
      glVertexArrayAttribFormat(pipe->vertex_array, j, comp_count, gl_comp_type, GL_FALSE, stride);
      glVertexArrayAttribBinding(pipe->vertex_array, j, i);
      
      // Increase the stride for the next round
      stride += size;
    }
 
    layout->strides[i] = stride;
    glVertexArrayBindingDivisor(pipe->vertex_array, i, pipe->desc.layouts[i].instance_rate);
  
    stride = 0;
  }
}

static void check_shader_compile_error(const sizei shader) {
  i32 success;
  i8 log_info[512];

  glGetShaderiv(shader, GL_COMPILE_STATUS, &success); 

  if(!success) {
    glGetShaderInfoLog(shader, 512, nullptr, log_info);
    NIKOLA_LOG_WARN("SHADER-ERROR: %s", log_info);
  }
}

static void check_shader_linker_error(const GfxShader* shader) {
  i32 success;
  i8 log_info[512];

  glGetProgramiv(shader->id, GL_LINK_STATUS, &success); 

  if(!success) {
    glGetProgramInfoLog(shader->id, 512, nullptr, log_info);
    NIKOLA_LOG_WARN("SHADER-ERROR: %s", log_info);
  }
}

static GLenum create_gl_texture(const GfxTextureType type) {
  u32 id = 0;

  switch(type) {
    case GFX_TEXTURE_1D:
    case GFX_TEXTURE_IMAGE_1D:
      glCreateTextures(GL_TEXTURE_1D, 1, &id);
      break;
    case GFX_TEXTURE_1D_ARRAY:
      glCreateTextures(GL_TEXTURE_1D_ARRAY, 1, &id);
      break;
    case GFX_TEXTURE_2D:
    case GFX_TEXTURE_IMAGE_2D:
      glCreateTextures(GL_TEXTURE_2D, 1, &id);
      break;
    case GFX_TEXTURE_2D_ARRAY:
      glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &id);
      break;
    case GFX_TEXTURE_3D:
    case GFX_TEXTURE_IMAGE_3D:
      glCreateTextures(GL_TEXTURE_3D, 1, &id);
      break;
    case GFX_TEXTURE_DEPTH_TARGET:
    case GFX_TEXTURE_STENCIL_TARGET:
    case GFX_TEXTURE_DEPTH_STENCIL_TARGET:
      glCreateRenderbuffers(1, &id);
      break;
    default:
      break;
  } 

  return id;
}

static void set_texture_pixel_align(const GfxTextureFormat format) {
  switch(format) {
    case GFX_TEXTURE_FORMAT_R8:
    case GFX_TEXTURE_FORMAT_R16:
    case GFX_TEXTURE_FORMAT_R16F:
    case GFX_TEXTURE_FORMAT_R32F:
      glPixelStorei(GL_PACK_ALIGNMENT, 1);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      break;
    case GFX_TEXTURE_FORMAT_RG8:
    case GFX_TEXTURE_FORMAT_RG16:
    case GFX_TEXTURE_FORMAT_RG16F:
    case GFX_TEXTURE_FORMAT_RG32F:
      glPixelStorei(GL_PACK_ALIGNMENT, 2);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
      break;
    case GFX_TEXTURE_FORMAT_RGBA8:
    case GFX_TEXTURE_FORMAT_RGBA16:
    case GFX_TEXTURE_FORMAT_RGBA16F:
    case GFX_TEXTURE_FORMAT_RGBA32F:
      glPixelStorei(GL_PACK_ALIGNMENT, 4);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
      break;
    default:
      break;
  }
}

static void update_gl_texture_pixels(GfxTexture* texture, GLenum gl_format, GLenum gl_pixel_type) {
  switch(texture->desc.type) {
    case GFX_TEXTURE_1D: 
    case GFX_TEXTURE_IMAGE_1D:
      glTextureSubImage1D(texture->id, 
                          texture->desc.mips, 
                          0, 
                          texture->desc.width, 
                          gl_format, 
                          gl_pixel_type, 
                          texture->desc.data);
      break;
    case GFX_TEXTURE_2D:
    case GFX_TEXTURE_IMAGE_2D:
    case GFX_TEXTURE_1D_ARRAY:
      glTextureSubImage2D(texture->id, 
                          0, 
                          0, 0,
                          texture->desc.width, texture->desc.height,
                          gl_format, 
                          gl_pixel_type, 
                          texture->desc.data);
      break;
    case GFX_TEXTURE_3D:
    case GFX_TEXTURE_IMAGE_3D:
    case GFX_TEXTURE_2D_ARRAY:
      glTextureSubImage3D(texture->id, 
                          0, 
                          0, 0, 0,
                          texture->desc.width, texture->desc.height, texture->desc.depth,
                          gl_format, 
                          gl_pixel_type, 
                          texture->desc.data);
      break;
      break;
    case GFX_TEXTURE_DEPTH_TARGET:
    case GFX_TEXTURE_STENCIL_TARGET:
    case GFX_TEXTURE_DEPTH_STENCIL_TARGET:
      break;
    default:
      break;
  }
}

static void update_gl_texture_storage(GfxTexture* texture, GLenum in_format) {
  switch(texture->desc.type) {
    case GFX_TEXTURE_1D: 
    case GFX_TEXTURE_IMAGE_1D:
      glTextureStorage1D(texture->id, texture->desc.mips, in_format, texture->desc.width);
      break;
    case GFX_TEXTURE_2D:
    case GFX_TEXTURE_IMAGE_2D:
    case GFX_TEXTURE_1D_ARRAY:
      glTextureStorage2D(texture->id, texture->desc.mips, in_format, texture->desc.width, texture->desc.height);
      break;
    case GFX_TEXTURE_3D:
    case GFX_TEXTURE_IMAGE_3D:
    case GFX_TEXTURE_2D_ARRAY:
      glTextureStorage3D(texture->id, texture->desc.mips, in_format, texture->desc.width, texture->desc.height, texture->desc.depth);
      break;
    case GFX_TEXTURE_DEPTH_TARGET:
    case GFX_TEXTURE_STENCIL_TARGET:
    case GFX_TEXTURE_DEPTH_STENCIL_TARGET:
      glNamedRenderbufferStorage(texture->id, in_format, texture->desc.width, texture->desc.height);
      break;
    default:
      break;
  }
}

/// Private functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Context functions 

GfxContext* gfx_context_init(const GfxContextDesc& desc) {
  GfxContext* gfx = (GfxContext*)memory_allocate(sizeof(GfxContext));
  memory_zero(gfx, sizeof(GfxContext)); 
  
  gfx->desc                = desc;
  gfx->default_clear_flags = GL_COLOR_BUFFER_BIT;
  gfx->current_clear_flags = gfx->default_clear_flags;

  // Glad init
  if(!gladLoadGL()) {
    NIKOLA_LOG_FATAL("Could not create an OpenGL instance");
    return nullptr;
  }

  // Enabling debug output on debug builds only
#if NIKOLA_BUILD_DEBUG == 1 
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(gl_error_callback, nullptr);
#endif

  // Setting the window context to this OpenGL context 
  window_set_current_context(desc.window);

  // Setting up the viewport for OpenGL
  i32 width, height; 
  window_get_size(desc.window, &width, &height);
  glViewport(0, 0, width, height);

  // Setting the flags
  gfx->states = (GfxStates)desc.states;
  set_gfx_states(gfx);

  // Listening to events 
  event_listen(EVENT_WINDOW_FRAMEBUFFER_RESIZED, framebuffer_resize);

  // Getting some OpenGL information
  
  const u8* vendor       = glGetString(GL_VENDOR); 
  const u8* renderer     = glGetString(GL_RENDERER); 
  const u8* gl_version   = glGetString(GL_VERSION);
  const u8* glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);

  // Getting the version number
  
  i32 major_ver, minor_ver;
  glGetIntegerv(GL_MAJOR_VERSION, &major_ver);
  glGetIntegerv(GL_MINOR_VERSION, &minor_ver);
  check_supported_gl_version(major_ver, minor_ver);

  // Getting maximum values

  NIKOLA_LOG_INFO("An OpenGL graphics context was successfully created:\n" 
                 "              VENDOR: %s\n" 
                 "              RENDERER: %s\n" 
                 "              GL VERSION: %s\n" 
                 "              GLSL VERSION: %s", 
                 vendor, renderer, gl_version, glsl_version);
  return gfx;
}

void gfx_context_shutdown(GfxContext* gfx) {
  if(!gfx) {
    return;
  }

  NIKOLA_LOG_INFO("The graphics context was successfully destroyed");
  memory_free(gfx);
}

GfxContextDesc& gfx_context_get_desc(GfxContext* gfx) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  return gfx->desc;
}

void gfx_context_set_state(GfxContext* gfx, const GfxStates state, const bool value) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");
  set_state(gfx, state, value);
}

void gfx_context_set_depth_state(GfxContext* gfx, const GfxDepthDesc& depth_desc) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  gfx->desc.depth_desc = depth_desc; 
  set_depth_state(gfx);
}

void gfx_context_set_stencil_state(GfxContext* gfx, const GfxStencilDesc& stencil_desc) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  gfx->desc.stencil_desc = stencil_desc; 
  set_stencil_state(gfx);
}

void gfx_context_set_cull_state(GfxContext* gfx, const GfxCullDesc& cull_desc) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  gfx->desc.cull_desc = cull_desc; 
  set_cull_state(gfx);
}

void gfx_context_set_blend_state(GfxContext* gfx, const GfxBlendDesc& blend_desc) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  gfx->desc.blend_desc = blend_desc; 
  set_blend_state(gfx);
} 

void gfx_context_set_scissor_rect(GfxContext* gfx, const i32 x, const i32 y, const i32 width, const i32 height) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  glScissor(x, y, width, height);
}

void gfx_context_set_viewport(GfxContext* gfx, const i32 x, const i32 y, const i32 width, const i32 height) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");
 
  glViewport(x, y, width, height);
}

void gfx_context_set_target(GfxContext* gfx, GfxFramebuffer* framebuffer) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");

  // Set the default values for when binding to the default framebuffer
  gfx->current_clear_flags = gfx->default_clear_flags;
  gfx->current_target      = 0;

  if(framebuffer) {
    gfx->current_clear_flags = framebuffer->clear_flags;
    gfx->current_target      = framebuffer->id; 
  }
}

void gfx_context_clear(GfxContext* gfx, const f32 r, const f32 g, const f32 b, const f32 a) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");

  glBindFramebuffer(GL_FRAMEBUFFER, gfx->current_target);
  glClear(gfx->current_clear_flags);
  glClearColor(r, g, b, a);
}

void gfx_context_use_bindings(GfxContext* gfx, const GfxBindingDesc& binding_desc) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");
  NIKOLA_ASSERT(binding_desc.shader, "Must have a valid GfxShader to bind resources");

  // Bind the shader
  glUseProgram(binding_desc.shader->id);

  // Bind the textures 

  NIKOLA_ASSERT(((binding_desc.textures_count >= 0) && (binding_desc.textures_count < TEXTURES_MAX)), 
      "Textures count in gfx_context_use_bindings exceeding TEXTURES_MAX");

  for(sizei i = 0; i < binding_desc.textures_count; i++) {
    NIKOLA_ASSERT(binding_desc.textures[i], "An invalid texture found in texutres array");
    glBindTextureUnit(i, binding_desc.textures[i]->id);
  }

  // Bind the images
  
  NIKOLA_ASSERT(((binding_desc.images_count >= 0) && (binding_desc.images_count < TEXTURES_MAX)), 
                "Images count in gfx_context_use_bindings exceeding TEXTURES_MAX");
  
  for(sizei i = 0; i < binding_desc.images_count; i++) {
    NIKOLA_ASSERT(binding_desc.images[i], "An invalid texture found in texutres array");
  
    GLenum access = get_texture_gl_access(binding_desc.images[i]->desc.access);
    GLenum in_format, gl_format, gl_pixel_type;
    get_texture_gl_format(binding_desc.images[i]->desc.format, &in_format, &gl_format, &gl_pixel_type);

    glBindImageTexture(i,                          // Image unit
                       binding_desc.images[i]->id, // Image ID 
                       0,                          // Level
                       false,                      // Layered state (for texture arrays)
                       0,                          // Layer index
                       access,                     // Image access type
                       in_format);                 // Image format            
  }

  // Bind the cubemaps
  
  NIKOLA_ASSERT(((binding_desc.cubemaps_count >= 0) && (binding_desc.cubemaps_count < CUBEMAPS_MAX)), 
                "Cubemaps count in gfx_context_use_bindings exceeding CUBEMAPS_MAX");

  for(sizei i = 0; i < binding_desc.cubemaps_count; i++) {
    glBindTextureUnit(i, binding_desc.cubemaps[i]->id);
  }
}

void gfx_context_use_pipeline(GfxContext* gfx, GfxPipeline* pipeline) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");
  NIKOLA_ASSERT(pipeline, "Invalid GfxPipeline struct passed");
  NIKOLA_ASSERT(pipeline->vertex_buffer, "Must at least have a valid vertex buffer to draw");

  // Use the pipline data to setup for the next draw call

  pipeline->gfx->bound_pipeline = pipeline;

  glDepthMask(pipeline->desc.depth_mask);
  glStencilMask(pipeline->desc.stencil_ref);

  const f32* blend_color = pipeline->desc.blend_factor;
  glBlendColor(blend_color[0], blend_color[1], blend_color[2], blend_color[3]);

  // Bind the new bound pipeline
  glBindVertexArray(pipeline->vertex_array);
}

void gfx_context_draw(GfxContext* gfx, const u32 start_element) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");
  NIKOLA_ASSERT(gfx->bound_pipeline, "Cannot draw using an invalid bound pipeline");

  GfxPipeline* pipe = gfx->bound_pipeline;
  GLenum draw_mode = get_draw_mode(pipe->desc.draw_mode);

  // Draw the index buffer (if it is valid)
  if(pipe->index_buffer) {
    GLenum index_type = get_layout_type(pipe->desc.indices_type);
    glDrawElements(draw_mode, pipe->index_count, index_type, 0);
  }
  // Draw the vertex buffer instead
  else {
    glDrawArrays(draw_mode, start_element, pipe->vertex_count);
  }

  // Unbind the vertex array for better debugging.
  glBindVertexArray(0);
}

void gfx_context_draw_instanced(GfxContext* gfx, const u32 start_element, const u32 instance_count) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");
  NIKOLA_ASSERT(gfx->bound_pipeline, "Cannot draw using an invalid bound pipeline");
  NIKOLA_ASSERT(gfx->bound_pipeline->instance_buffer, "Must have a valid instance buffer to instance draw");

  GfxPipeline* pipe = gfx->bound_pipeline;
  GLenum draw_mode = get_draw_mode(pipe->desc.draw_mode);
  
  // Draw the index buffer (if it is valid)
  if(pipe->index_buffer) {
    GLenum index_type = get_layout_type(pipe->desc.indices_type);
    glDrawElementsInstanced(draw_mode, pipe->index_count, index_type, 0, instance_count);
  }
  // Draw the vertex buffer instead
  else {
    glDrawArraysInstanced(draw_mode, start_element, pipe->vertex_count, instance_count);
  }
  
  glBindVertexArray(0);
}

void gfx_context_dispatch(GfxContext* gfx, const u32 work_group_x, const u32 work_group_y, const u32 work_group_z) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");

  // Some bookkeeping...

  bool is_group_x_count_valid = (work_group_x >= 1) && (work_group_x < MAX_COMPUTE_WORK_GROUPS_COUNT);
  bool is_group_y_count_valid = (work_group_y >= 1) && (work_group_y < MAX_COMPUTE_WORK_GROUPS_COUNT);
  bool is_group_z_count_valid = (work_group_z >= 1) && (work_group_z < MAX_COMPUTE_WORK_GROUPS_COUNT);
  NIKOLA_ASSERT((is_group_x_count_valid && is_group_y_count_valid && is_group_z_count_valid), 
                "Invalid work group counts given to gfx_context_dispatch");

  glDispatchCompute(work_group_x, work_group_y, work_group_z);
}

void gfx_context_memory_barrier(GfxContext* gfx, const i32 barrier_bits) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  GLbitfield barriers = 0;
 
  // Unset and check the buffer bits and set them to OpenGL equivalents
 
  // 10 = the maximum number of barriers
  // @FIX (GL-Backend): A magic number like that is probably not the best idea...
  for(sizei i = 0; i < 10; i++) {
    i32 type = (GFX_MEMORY_BARRIER_VERTEX_ATTRIBUTE << i);

    if(IS_BIT_SET(barriers, type)) {
      barriers |= get_gl_barrier((GfxMemoryBarrierType)type);
    }
  }

  glMemoryBarrier(barriers);
}

void gfx_context_present(GfxContext* gfx) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");
  window_swap_buffers(gfx->desc.window, gfx->desc.has_vsync);
}

/// Context functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Framebuffer functions

GfxFramebuffer* gfx_framebuffer_create(GfxContext* gfx, const GfxFramebufferDesc& desc, const AllocateMemoryFn& alloc_fn) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");

  bool is_count_valid = (desc.attachments_count >= 0) && (desc.attachments_count < FRAMEBUFFER_ATTACHMENTS_MAX);
  NIKOLA_ASSERT(is_count_valid, "Attachments count in GfxFramebuffer cannot exceed FRAMEBUFFER_ATTACHMENTS_MAX");

  GfxFramebuffer* buff = (GfxFramebuffer*)alloc_fn(sizeof(GfxFramebuffer));

  buff->desc        = desc; 
  buff->clear_flags = get_gl_clear_flags(desc.clear_flags);

  glCreateFramebuffers(1, &buff->id);

  // Attach color attachments

  for(sizei i = 0; i < desc.attachments_count; i++) {
    glNamedFramebufferTexture(buff->id, 
                              GL_COLOR_ATTACHMENT0 + i, 
                              desc.color_attachments[i]->id, 
                              0);
    buff->color_textures[i] = GL_COLOR_ATTACHMENT0 + i;
  }

  // Attach the depth attachments (if it exists)
  
  if(desc.depth_attachment) {
    GLenum depth_type = GL_DEPTH_ATTACHMENT; 
    if(desc.depth_attachment->desc.type == GFX_TEXTURE_DEPTH_STENCIL_TARGET) {
      depth_type = GL_DEPTH_STENCIL_ATTACHMENT;
    }

    glNamedFramebufferRenderbuffer(buff->id, 
                                   depth_type, 
                                   GL_RENDERBUFFER, 
                                   desc.depth_attachment->id);
    buff->depth_texture = depth_type;
  }
  
  // Attach the stencil attachments (if it exists)
  
  if(desc.stencil_attachment) {
    glNamedFramebufferRenderbuffer(buff->id, 
                                   GL_STENCIL_ATTACHMENT, 
                                   GL_RENDERBUFFER, 
                                   desc.stencil_attachment->id);
    buff->stencil_texture = GL_STENCIL_ATTACHMENT;
  }

  glNamedFramebufferDrawBuffers(buff->id,  
                                buff->desc.attachments_count, 
                                buff->color_textures);

  if(glCheckNamedFramebufferStatus(buff->id, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    NIKOLA_LOG_WARN("GL-ERROR: Framebuffer %i is incomplete", buff->id);
  }

  return buff;
}

void gfx_framebuffer_destroy(GfxFramebuffer* framebuffer, const FreeMemoryFn& free_fn) {
  if(!framebuffer) {
    return;
  }

  glDeleteFramebuffers(1, &framebuffer->id);
  free_fn(framebuffer);
}

void gfx_framebuffer_copy(const GfxFramebuffer* src_frame, 
                          GfxFramebuffer* dest_frame, 
                          i32 src_x, i32 src_y, 
                          i32 src_width, i32 src_height, 
                          i32 dest_x, i32 dest_y, 
                          i32 dest_width, i32 dest_height, 
                          i32 buffer_mask) {
  NIKOLA_ASSERT((src_frame || dest_frame), "Cannot have both framebuffers as NULL in copy operation");

  u32 src_id   = src_frame ? src_frame->id : 0;
  u32 dest_id  = dest_frame ? dest_frame->id : 0;
  u32 gl_masks = get_gl_clear_flags(buffer_mask);

  glBlitNamedFramebuffer(src_id, dest_id, 
                         src_x, src_y, 
                         src_width, src_height, 
                         dest_x, dest_y, 
                         dest_width, dest_height, 
                         gl_masks, 
                         GL_NEAREST);
}

GfxFramebufferDesc& gfx_framebuffer_get_desc(GfxFramebuffer* framebuffer) {
  NIKOLA_ASSERT(framebuffer, "Invalid GfxFramebuffer struct passed");

  return framebuffer->desc;
}

void gfx_framebuffer_update(GfxFramebuffer* framebuffer, const GfxFramebufferDesc& desc) {
  NIKOLA_ASSERT(framebuffer, "Invalid GfxFramebuffer struct passed");
  
  framebuffer->desc        = desc; 
  framebuffer->clear_flags = get_gl_clear_flags(desc.clear_flags);

  // Attach color attachments

  for(sizei i = 0; i < desc.attachments_count; i++) {
    glNamedFramebufferTexture(framebuffer->id, 
                              GL_COLOR_ATTACHMENT0 + i, 
                              desc.color_attachments[i]->id, 
                              0);
    framebuffer->color_textures[i] = GL_COLOR_ATTACHMENT0 + i;
  }

  // Attach the depth attachments (if it exists)
  
  if(desc.depth_attachment) {
    GLenum depth_type = GL_DEPTH_ATTACHMENT; 
    if(desc.depth_attachment->desc.type == GFX_TEXTURE_DEPTH_STENCIL_TARGET) {
      depth_type = GL_DEPTH_STENCIL_ATTACHMENT;
    }

    glNamedFramebufferRenderbuffer(framebuffer->id, 
                                   depth_type, 
                                   GL_RENDERBUFFER, 
                                   desc.depth_attachment->id);
    framebuffer->depth_texture = depth_type;
  }
  
  // Attach the stencil attachments (if it exists)
  
  if(desc.stencil_attachment) {
    glNamedFramebufferRenderbuffer(framebuffer->id, 
                                   GL_STENCIL_ATTACHMENT, 
                                   GL_RENDERBUFFER, 
                                   desc.stencil_attachment->id);
    framebuffer->stencil_texture = GL_STENCIL_ATTACHMENT;
  }

  glNamedFramebufferDrawBuffers(framebuffer->id,  
                                framebuffer->desc.attachments_count, 
                                framebuffer->color_textures);

  if(glCheckNamedFramebufferStatus(framebuffer->id, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    NIKOLA_LOG_WARN("GL-ERROR: Framebuffer %i is incomplete", framebuffer->id);
  }
}

/// Framebuffer functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Buffer functions 

GfxBuffer* gfx_buffer_create(GfxContext* gfx, const AllocateMemoryFn& alloc_fn) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");

  GfxBuffer* buff = (GfxBuffer*)alloc_fn(sizeof(GfxBuffer));
  
  buff->desc = {};
  buff->gfx  = gfx; 

  glCreateBuffers(1, &buff->id);
  return buff;
}

const bool gfx_buffer_load(GfxBuffer* buffer, const GfxBufferDesc& desc) {
  NIKOLA_ASSERT(buffer, "Trying to load data into an invalid resource");
  
  buffer->desc          = desc;
  buffer->gl_buff_type  = get_buffer_type(desc.type);
  buffer->gl_buff_usage = get_buffer_usage(desc.usage);
  
  glNamedBufferData(buffer->id, desc.size, desc.data, buffer->gl_buff_usage);
  return true;
}

void gfx_buffer_destroy(GfxBuffer* buff, const FreeMemoryFn& free_fn) {
  if(!buff) {
    return;
  }

  glDeleteBuffers(1, &buff->id);
  free_fn(buff);
}

GfxBufferDesc& gfx_buffer_get_desc(GfxBuffer* buffer) {
  NIKOLA_ASSERT(buffer, "Invalid GfxBuffer struct passed");

  return buffer->desc;
}

void gfx_buffer_update(GfxBuffer* buff, const GfxBufferDesc& desc) {
  NIKOLA_ASSERT(buff, "Invalid GfxBuffer struct passed");
  NIKOLA_ASSERT(buff->gfx, "Invalid GfxContext struct passed");

  buff->desc = desc;
}

void gfx_buffer_upload_data(GfxBuffer* buff, const sizei offset, const sizei size, const void* data) {
  NIKOLA_ASSERT(buff, "Invalid GfxBuffer struct passed");
  NIKOLA_ASSERT(buff->gfx, "Invalid GfxContext struct passed");
  NIKOLA_ASSERT((offset + size) <= buff->desc.size, "The GfxBuffer does not have enough memory to upload this data");

  glNamedBufferSubData(buff->id, offset, size, data);
}

/// Buffer functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Shader functions 

GfxShader* gfx_shader_create(GfxContext* gfx, const AllocateMemoryFn& alloc_fn) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");

  GfxShader* shader = (GfxShader*)alloc_fn(sizeof(GfxShader));

  shader->desc = {};
  shader->gfx  = gfx;
  shader->id   = glCreateProgram();

  return shader;
}

const bool gfx_shader_load(GfxShader* shader, const GfxShaderDesc& desc) {
  NIKOLA_ASSERT(shader, "Trying to load data into an invalid resource");
  
  shader->desc = desc;

  // Create the compute shader if the source exists. 
  // Do not continue if this is true, since compute 
  // shaders are quite special flakes. 

  if(desc.compute_source) {
    i32 compute_src_len = strlen(shader->desc.compute_source);
    shader->compute_id  = glCreateShader(GL_COMPUTE_SHADER);

    glShaderSource(shader->compute_id, 1, &shader->desc.compute_source, &compute_src_len); 
    glCompileShader(shader->compute_id);
    check_shader_compile_error(shader->compute_id);
    glAttachShader(shader->id, shader->compute_id);

    // Linking

    glLinkProgram(shader->id);
    check_shader_linker_error(shader);

    return true;
  }

  // Necessary asserts

  NIKOLA_ASSERT(desc.vertex_source, "Invalid Vertex source passed to the shader");
  NIKOLA_ASSERT(desc.pixel_source, "Invalid Pixel source passed to the shader");

  // Vertex shader
  
  i32 vert_src_len = strlen(shader->desc.vertex_source);
  shader->vert_id  = glCreateShader(GL_VERTEX_SHADER);
  
  glShaderSource(shader->vert_id, 1, &shader->desc.vertex_source, &vert_src_len); 
  glCompileShader(shader->vert_id);
  check_shader_compile_error(shader->vert_id);
  glAttachShader(shader->id, shader->vert_id);
   
  // Fragment shader
  
  i32 frag_src_len = strlen(shader->desc.pixel_source);
  shader->frag_id  = glCreateShader(GL_FRAGMENT_SHADER);
  
  glShaderSource(shader->frag_id, 1, &shader->desc.pixel_source, &frag_src_len); 
  glCompileShader(shader->frag_id);
  check_shader_compile_error(shader->frag_id);
  glAttachShader(shader->id, shader->frag_id);

  // Linking
  
  glLinkProgram(shader->id);
  check_shader_linker_error(shader);
  
  return true;
}

void gfx_shader_destroy(GfxShader* shader, const FreeMemoryFn& free_fn) {
  if(!shader) {
    return;
  }

  glDeleteProgram(shader->id);
  free_fn(shader);
}

GfxShaderDesc& gfx_shader_get_source(GfxShader* shader) {
  NIKOLA_ASSERT(shader, "Invalid GfxShader struct passed to gfx_shader_get_source");
  
  return shader->desc;
}

void gfx_shader_update(GfxShader* shader, const GfxShaderDesc& desc) {
  NIKOLA_ASSERT(shader->gfx, "Invalid GfxContext struct passed");
  NIKOLA_ASSERT(shader, "Invalid GfxShader struct passed");
  NIKOLA_ASSERT(desc.vertex_source, "Invalid Vertex source passed to the shader");
  NIKOLA_ASSERT(desc.pixel_source, "Invalid Pixel source passed to the shader");

  shader->desc = desc;

  // Create the compute shader if the source exists. 
  // Do not continue if this is true, since compute 
  // shaders are quite special flakes. 

  if(desc.compute_source) {
    i32 compute_src_len = strlen(shader->desc.compute_source);
    glShaderSource(shader->compute_id, 1, &shader->desc.compute_source, &compute_src_len); 
    glCompileShader(shader->compute_id);
    check_shader_compile_error(shader->compute_id);
    glAttachShader(shader->id, shader->compute_id);

    // Linking

    glLinkProgram(shader->id);
    check_shader_linker_error(shader);

    return;
  }

  // Necessary asserts

  NIKOLA_ASSERT(desc.vertex_source, "Invalid Vertex source passed to the shader");
  NIKOLA_ASSERT(desc.pixel_source, "Invalid Pixel source passed to the shader");

  // Vertex shader
  
  i32 vert_src_len = strlen(shader->desc.vertex_source);
  glShaderSource(shader->vert_id, 1, &shader->desc.vertex_source, &vert_src_len); 
  glCompileShader(shader->vert_id);
  check_shader_compile_error(shader->vert_id);
  glAttachShader(shader->id, shader->vert_id);
   
  // Fragment shader
  
  i32 frag_src_len = strlen(shader->desc.pixel_source);
  glShaderSource(shader->frag_id, 1, &shader->desc.pixel_source, &frag_src_len); 
  glCompileShader(shader->frag_id);
  check_shader_compile_error(shader->frag_id);
  glAttachShader(shader->id, shader->frag_id);

  // Linking
  
  glLinkProgram(shader->id);
  check_shader_linker_error(shader);
}

void gfx_shader_query(GfxShader* shader, GfxShaderQueryDesc* out_desc) {
  NIKOLA_ASSERT(shader->gfx, "Invalid GfxContext struct passed");
  NIKOLA_ASSERT(shader, "Invalid GfxShader struct passed");
 
  // Check if the shader is actually linked properly
 
  i32 success; 
  char log_info[512];

  glGetProgramiv(shader->id, GL_LINK_STATUS, &success); 
  if(!success) {
    glGetProgramInfoLog(shader->id, 512, nullptr, log_info);
    NIKOLA_LOG_WARN("SHADER-ERROR: %s", log_info);

    return;
  }
  
  i32 max_name_len = 0;

  // Retrieve the attributes information

  glGetProgramiv(shader->id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_name_len); 
  glGetProgramiv(shader->id, GL_ACTIVE_ATTRIBUTES, &out_desc->attributes_count);
  
  for(i32 i = 0; i < out_desc->attributes_count; i++) {
    GLenum gl_type = (GLenum)0;
    i32 comp_count = 0; 
    char attr_name[512];

    glGetActiveAttrib(shader->id,   // Program
                      i,            // Attribute index
                      max_name_len, // Max name length
                      nullptr,      // Out name length
                      &comp_count,  // Attribute component count
                      &gl_type,     // Attribute type
                      attr_name);   // Attribute name
  
    out_desc->active_attributes[i] = get_shader_type(gl_type);
  } 
  
  // Retrieve the uniform blocks information
  
  glGetProgramiv(shader->id, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &max_name_len); 
  glGetProgramiv(shader->id, GL_ACTIVE_UNIFORM_BLOCKS, &out_desc->uniform_blocks_count);

  i32 uniform_blocks_active_uniforms = 0;

  for(i32 i = 0; i < out_desc->uniform_blocks_count; i++) {
    glGetActiveUniformBlockiv(shader->id,                           // Program
                              i,                                    // Block index
                              GL_UNIFORM_BLOCK_BINDING,             // Query block binding
                              &out_desc->active_uniform_blocks[i]); // Block param
    
    glGetActiveUniformBlockiv(shader->id,                       // Program
                              i,                                // Block index
                              GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, // Query block active uniforms
                              &uniform_blocks_active_uniforms); // Block param
  }
  
  // Retrieve the uniforms information
  
  glGetProgramiv(shader->id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len); 
  glGetProgramiv(shader->id, GL_ACTIVE_UNIFORMS, &out_desc->uniforms_count);

  for(i32 i = 0; i < out_desc->uniforms_count; i++) {
    GLsizei name_len = 0; 
    i32 comp_count   = 0;
    GLenum gl_type   = (GLenum)0;

    GfxUniformDesc uniform_desc = {};

    glGetActiveUniform(shader->id,        // Program
                      i,                  // Uniform index
                      max_name_len,       // Max name length
                      &name_len,          // Out name length
                      &comp_count,        // Uniform component count
                      &gl_type,           // Uniform type
                      uniform_desc.name); // Uniform name

    uniform_desc.type            = get_shader_type(gl_type);
    uniform_desc.location        = gfx_shader_uniform_lookup(shader, uniform_desc.name);
    uniform_desc.component_count = comp_count;
    out_desc->active_uniforms[i] = uniform_desc;
  }

  // Retrieve the compute shader work groups information (if a compute shader exists)
 
  i32 shaders_count = 0; 
  glGetProgramiv(shader->id, GL_ATTACHED_SHADERS, &shaders_count);
  if(shader->compute_id <= 0) { // The compute shader ID has not been generated
    return;
  }

  i32 groups[3]; 
  glGetProgramiv(shader->id, GL_COMPUTE_WORK_GROUP_SIZE, groups);

  out_desc->work_group_x = groups[0];
  out_desc->work_group_y = groups[1];
  out_desc->work_group_z = groups[2];
}

void gfx_shader_attach_uniform(GfxShader* shader, const GfxShaderType type, GfxBuffer* buffer, const u32 bind_point) {
  NIKOLA_ASSERT(shader->gfx, "Invalid GfxContext struct passed");
  NIKOLA_ASSERT(shader, "Invalid GfxShader struct passed");

  bool is_valid_buffer = (buffer->desc.type == GFX_BUFFER_UNIFORM) || (buffer->desc.type == GFX_BUFFER_SHADER_STORAGE);
  NIKOLA_ASSERT(is_valid_buffer, "Cannot attach a non-uniform or non-shader storage buffer to shader");

  glBindBufferBase(get_buffer_type(buffer->desc.type), bind_point, buffer->id);
}

i32 gfx_shader_uniform_lookup(GfxShader* shader, const i8* uniform_name) {
  NIKOLA_ASSERT(shader, "Invalid GfxShader struct passed");
  
  return glGetUniformLocation(shader->id, uniform_name);
}

void gfx_shader_upload_uniform_array(GfxShader* shader, const i32 location, const sizei count, const GfxLayoutType type, const void* data) {
  NIKOLA_ASSERT(shader, "Invalid GfxShader struct passed");

  // Will not do anything with an invalid uniform
  if(location == -1) {
    NIKOLA_LOG_WARN("Cannot set uniform with location -1");
    return;
  }

  glUseProgram(shader->id);

  switch(type) {
    case GFX_LAYOUT_FLOAT1:
      glUniform1fv(location, count, (f32*)data);
      break;
    case GFX_LAYOUT_FLOAT2:
      glUniform2fv(location, count, (f32*)data);
      break;
    case GFX_LAYOUT_FLOAT3:
      glUniform3fv(location, count, (f32*)data);
      break;
    case GFX_LAYOUT_FLOAT4:
      glUniform4fv(location, count, (f32*)data);
      break;
    case GFX_LAYOUT_INT1:
      glUniform1iv(location, count, (i32*)data);
      break;
    case GFX_LAYOUT_INT2:
      glUniform2iv(location, count, (i32*)data);
      break;
    case GFX_LAYOUT_INT3:
      glUniform3iv(location, count, (i32*)data);
      break;
    case GFX_LAYOUT_INT4:
      glUniform4iv(location, count, (i32*)data);
      break;
    case GFX_LAYOUT_UINT1:
      glUniform1uiv(location, count, (u32*)data);
      break;
    case GFX_LAYOUT_UINT2:
      glUniform2uiv(location, count, (u32*)data);
      break;
    case GFX_LAYOUT_UINT3:
      glUniform3uiv(location, count, (u32*)data);
      break;
    case GFX_LAYOUT_UINT4:
      glUniform4uiv(location, count, (u32*)data);
      break;
    case GFX_LAYOUT_MAT2:
      glUniformMatrix2fv(location, count, GL_FALSE, (f32*)data);
      break;
    case GFX_LAYOUT_MAT3:
      glUniformMatrix3fv(location, count, GL_FALSE, (f32*)data);
      break;
    case GFX_LAYOUT_MAT4:
      glUniformMatrix4fv(location, count, GL_FALSE, (f32*)data);
      break;
    default:
      NIKOLA_LOG_WARN("Shader layout type not supported %i", (u32)type);
      break;
  }
}

void gfx_shader_upload_uniform(GfxShader* shader, const i32 location, const GfxLayoutType type, const void* data) {
  gfx_shader_upload_uniform_array(shader, location, 1, type, data);
}

/// Shader functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Texture functions 

GfxTexture* gfx_texture_create(GfxContext* gfx, const GfxTextureType tex_type, const AllocateMemoryFn& alloc_fn) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");

  GfxTexture* texture = (GfxTexture*)alloc_fn(sizeof(GfxTexture));

  texture->desc.type = tex_type; 
  texture->desc      = {};
  texture->gfx       = gfx;
  
  // Creating the texutre based on its type
  texture->id = create_gl_texture(tex_type);

  return texture;
}

const bool gfx_texture_load(GfxTexture* texture, const GfxTextureDesc& desc) {
  NIKOLA_ASSERT(texture, "Trying to load data into an invalid resource");

  texture->desc = desc;

  // Convert the GFX types into valid GL ones.

  GLenum in_format, gl_format, gl_pixel_type;
  get_texture_gl_format(desc.format, &in_format, &gl_format, &gl_pixel_type);

  GLenum gl_wrap_format = get_texture_gl_wrap(desc.wrap_mode);
  
  GLenum min_filter, mag_filter;
  get_texture_gl_filter(desc.filter, &min_filter, &mag_filter); 

  // Setting texture parameters
  
  glTextureParameteri(texture->id, GL_TEXTURE_MIN_FILTER, min_filter);
  glTextureParameteri(texture->id, GL_TEXTURE_MAG_FILTER, mag_filter);
  glTextureParameteri(texture->id, GL_TEXTURE_WRAP_S, gl_wrap_format);
  glTextureParameteri(texture->id, GL_TEXTURE_WRAP_T, gl_wrap_format);
  glTextureParameteri(texture->id, GL_TEXTURE_WRAP_R, gl_wrap_format);

  GLint compare_func = (gl_format == GL_DEPTH_COMPONENT) ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE;

  glTextureParameteri(texture->id, GL_TEXTURE_COMPARE_MODE, compare_func);
  glTextureParameteri(texture->id, GL_TEXTURE_COMPARE_FUNC, get_gl_compare_func(desc.compare_func));

  glTextureParameterfv(texture->id, GL_TEXTURE_BORDER_COLOR, desc.border_color);

  // Setting the pixel store alignment
  set_texture_pixel_align(desc.format);

  // Filling the texture with the data based on its type
  update_gl_texture_storage(texture, in_format);
  update_gl_texture_pixels(texture, gl_format, gl_pixel_type);

  // Generating some mipmaps
  glGenerateTextureMipmap(texture->id);
 
  return true;
}

void gfx_texture_destroy(GfxTexture* texture, const FreeMemoryFn& free_fn) {
  if(!texture) {
    return;
  }
  
  glDeleteTextures(1, &texture->id);
  free_fn(texture);
}

GfxTextureDesc& gfx_texture_get_desc(GfxTexture* texture) {
  NIKOLA_ASSERT(texture, "Invalid GfxTexture struct passed");

  return texture->desc;
}

void gfx_texture_update(GfxTexture* texture, const GfxTextureDesc& desc) {
  NIKOLA_ASSERT(texture->gfx, "Invalid GfxContext struct passed");
  NIKOLA_ASSERT(texture, "Invalid GfxTexture struct passed");
 
  texture->desc = desc;
 
  // Updating the formats
  
  GLenum in_format, gl_format, gl_pixel_type;
  get_texture_gl_format(texture->desc.format, &in_format, &gl_format, &gl_pixel_type);

  // Updating the addressing mode
 
  GLenum gl_wrap_format = get_texture_gl_wrap(desc.wrap_mode);
  
  glTextureParameteri(texture->id, GL_TEXTURE_WRAP_S, gl_wrap_format);
  glTextureParameteri(texture->id, GL_TEXTURE_WRAP_T, gl_wrap_format);
  
  // Updating the filters
  
  GLenum min_filter, mag_filter;
  get_texture_gl_filter(desc.filter, &min_filter, &mag_filter); 
  
  glTextureParameteri(texture->id, GL_TEXTURE_MIN_FILTER, min_filter);
  glTextureParameteri(texture->id, GL_TEXTURE_MAG_FILTER, mag_filter);

  // Updating the comparison parametars

  int compare_func = (gl_format == GL_DEPTH_COMPONENT) ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE;
  glTextureParameteri(texture->id, GL_TEXTURE_COMPARE_MODE, compare_func);
  glTextureParameteri(texture->id, GL_TEXTURE_COMPARE_FUNC, get_gl_compare_func(desc.compare_func));

  // Setting the pixel store alignment
  set_texture_pixel_align(desc.format);
}

void gfx_texture_upload_data(GfxTexture* texture, 
                             const i32 width, const i32 height, const i32 depth, 
                             const void* data) {
  NIKOLA_ASSERT(texture->gfx, "Invalid GfxContext struct passed to gfx_texture_upload_data");
  NIKOLA_ASSERT(texture, "Invalid GfxTexture struct passed to gfx_texture_upload_data");
  NIKOLA_ASSERT(data, "Invalid texture data passed to gfx_texture_upload_data");
 
  // Updating the formats
  
  GLenum in_format, gl_format, gl_pixel_type;
  get_texture_gl_format(texture->desc.format, &in_format, &gl_format, &gl_pixel_type);
  
  // Update the data
  texture->desc.width  = width; 
  texture->desc.height = height; 
  texture->desc.depth  = depth; 
  texture->desc.data   = (void*)data; 

  // Updating the internal texture pixels
  update_gl_texture_storage(texture, in_format);
  update_gl_texture_pixels(texture, gl_format, gl_pixel_type);

  // Re-generate some mipmaps
  glGenerateTextureMipmap(texture->id);
}

/// Texture functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Cubemap functions 

GfxCubemap* gfx_cubemap_create(GfxContext* gfx, const AllocateMemoryFn& alloc_fn) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");

  GfxCubemap* cubemap = (GfxCubemap*)alloc_fn(sizeof(GfxCubemap));

  cubemap->desc = {};
  cubemap->gfx  = gfx;

  glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &cubemap->id);
  return cubemap;
}

const bool gfx_cubemap_load(GfxCubemap* cubemap, const GfxCubemapDesc& desc) {
  NIKOLA_ASSERT(cubemap, "Trying to load data into an invalid resource");
 
  cubemap->desc = desc;

  // Convert the GFX types into valid GL ones.

  GLenum in_format, gl_format, gl_pixel_type;
  get_texture_gl_format(desc.format, &in_format, &gl_format, &gl_pixel_type);

  GLenum gl_wrap_format = get_texture_gl_wrap(desc.wrap_mode);
  
  GLenum min_filter, mag_filter;
  get_texture_gl_filter(desc.filter, &min_filter, &mag_filter); 
  
  // Setting some parameters
  
  glTextureParameteri(cubemap->id, GL_TEXTURE_MIN_FILTER, min_filter);
  glTextureParameteri(cubemap->id, GL_TEXTURE_MAG_FILTER, mag_filter);
  glTextureParameteri(cubemap->id, GL_TEXTURE_WRAP_S, gl_wrap_format);
  glTextureParameteri(cubemap->id, GL_TEXTURE_WRAP_T, gl_wrap_format);
  glTextureParameteri(cubemap->id, GL_TEXTURE_WRAP_R, gl_wrap_format);
   
  glTextureStorage2D(cubemap->id, desc.mips, in_format, desc.width, desc.height);

  // Set the texture for each face in the cubemap
  
  for(sizei i = 0; i < desc.faces_count; i++) {
    glTextureSubImage3D(cubemap->id,                 // Texture
                        0,                           // Levels
                        0, 0, i,                     // Offset (x, y, z)
                        desc.width, desc.height, 1,  // Size (width, height, depth)
                        gl_format,                   // Format
                        gl_pixel_type,               // Type
                        desc.data[i]);               // Pixels
  }
  
  return true;
}

void gfx_cubemap_destroy(GfxCubemap* cubemap, const FreeMemoryFn& free_fn) {
  if(!cubemap) {
    return;
  }
  
  glDeleteTextures(1, &cubemap->id);
  free_fn(cubemap);
}

GfxCubemapDesc& gfx_cubemap_get_desc(GfxCubemap* cubemap) {
  NIKOLA_ASSERT(cubemap, "Invalid GfxCubemap struct passed");

  return cubemap->desc;
}

void gfx_cubemap_update(GfxCubemap* cubemap, const GfxCubemapDesc& desc) {
  NIKOLA_ASSERT(cubemap->gfx, "Invalid GfxContext struct passed");
  NIKOLA_ASSERT(cubemap, "Invalid GfxCubemap struct passed");
  
  cubemap->desc = desc;
  
  // Updating the format
  GLenum in_format, gl_format, gl_pixel_type;
  get_texture_gl_format(desc.format, &in_format, &gl_format, &gl_pixel_type);

  // Updating the addressing mode
  GLenum gl_wrap_format = get_texture_gl_wrap(desc.wrap_mode);
  
  // Updating the filters
  GLenum min_filter, mag_filter;
  get_texture_gl_filter(desc.filter, &min_filter, &mag_filter); 
  
  // Re-setting the parameters
  glTextureParameteri(cubemap->id, GL_TEXTURE_MIN_FILTER, min_filter);
  glTextureParameteri(cubemap->id, GL_TEXTURE_MAG_FILTER, mag_filter);
  glTextureParameteri(cubemap->id, GL_TEXTURE_WRAP_S, gl_wrap_format);
  glTextureParameteri(cubemap->id, GL_TEXTURE_WRAP_T, gl_wrap_format);
  glTextureParameteri(cubemap->id, GL_TEXTURE_WRAP_R, gl_wrap_format);
}

void gfx_cubemap_upload_data(GfxCubemap* cubemap, 
                             const i32 width, const i32 height,
                             const void** faces, const sizei count) {
  NIKOLA_ASSERT(cubemap->gfx, "Invalid GfxContext struct passed in gfx_cubemap_upload_data");
  NIKOLA_ASSERT(cubemap, "Invalid GfxCubemap struct passed in gfx_cubemap_upload_data");
  NIKOLA_ASSERT(((count >= 0) && (count <= CUBEMAPS_MAX)), "The count parametar in gfx_cubemap_upload_data is invalid");
  NIKOLA_ASSERT(faces, "Invalid cubemap faces passed to gfx_cubemap_upload_data");
  
  // Updating the format
  GLenum in_format, gl_format, gl_pixel_type;
  get_texture_gl_format(cubemap->desc.format, &in_format, &gl_format, &gl_pixel_type);

  // Updating the information
  cubemap->desc.faces_count = count;
  cubemap->desc.width       = width;
  cubemap->desc.height      = height;

  // Updating the faces
  glTextureStorage2D(cubemap->id, cubemap->desc.mips, in_format, width, height);
  for(sizei i = 0; i < count; i++) {
    cubemap->desc.data[i] = (void*)faces[i];
    glTextureSubImage3D(cubemap->id,                                   // Texture
                        0,                                             // Levels 
                        0, 0, i,                                       // Offset (x, y, z)
                        cubemap->desc.width, cubemap->desc.height, 1,  // Size (width, height, depth)
                        gl_format,                                     // Format
                        gl_pixel_type,                                 // Type
                        faces[i]);                                     // Pixels
  }
}

/// Cubemap functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Pipeline functions 

GfxPipeline* gfx_pipeline_create(GfxContext* gfx, const GfxPipelineDesc& desc, const AllocateMemoryFn& alloc_fn) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");
  NIKOLA_ASSERT(desc.vertex_buffer, "Must have a vertex buffer to create a GfxPipeline struct");

  GfxPipeline* pipe = (GfxPipeline*)alloc_fn(sizeof(GfxPipeline));

  pipe->desc = desc;
  pipe->gfx  = gfx;

  // VAO init
  glCreateVertexArrays(1, &pipe->vertex_array);

  // Pipeline layout init
  _PipelineLayout layout; 
  init_pipeline_layout(pipe, &layout);

  // VBO init
  pipe->vertex_buffer = desc.vertex_buffer; 
  pipe->vertex_count  = desc.vertices_count; 
  glVertexArrayVertexBuffer(pipe->vertex_array,      // VAO
                            0,                       // Binding index
                            pipe->vertex_buffer->id, // Buffer ID
                            0,       // Starting offset
                            layout.strides[0]);      // Buffer stride

  // EBO init
  if(desc.index_buffer) {
    pipe->index_buffer = desc.index_buffer;
    pipe->index_count  = desc.indices_count;

    glVertexArrayElementBuffer(pipe->vertex_array, pipe->index_buffer->id);
  }

  // Instance buffer init
  if(desc.instance_buffer) {
    pipe->instance_buffer = desc.instance_buffer; 
    glVertexArrayVertexBuffer(pipe->vertex_array,        // VAO
                              1,                         // Binding index
                              pipe->instance_buffer->id, // Buffer ID
                              0,         // Starting offset
                              layout.strides[1]);        // Buffer stride
  }

  // Set the draw mode for the whole pipeline
  pipe->draw_mode = desc.draw_mode;
   
  return pipe;
}

void gfx_pipeline_destroy(GfxPipeline* pipeline, const FreeMemoryFn& free_fn) {
  NIKOLA_ASSERT(pipeline, "Attempting to free an invalid GfxPipeline");

  // Deleting the buffers
  glDeleteVertexArrays(1, &pipeline->vertex_array);

  // Free the pipeline
  free_fn(pipeline);
}

GfxPipelineDesc& gfx_pipeline_get_desc(GfxPipeline* pipeline) {
  NIKOLA_ASSERT(pipeline, "Invalid GfxPipeline struct passed");

  return pipeline->desc;
}

void gfx_pipeline_update(GfxPipeline* pipeline, const GfxPipelineDesc& desc) {
  NIKOLA_ASSERT(pipeline, "Invalid GfxPipeline struct passed to gfx_pipeline_update");
  
  // Update the internal pipeline
  
  pipeline->desc = desc;

  pipeline->vertex_buffer = desc.vertex_buffer;
  pipeline->index_buffer  = desc.index_buffer;
  
  pipeline->vertex_count = desc.vertices_count;
  pipeline->index_count  = desc.indices_count;
  
  pipeline->instance_buffer = desc.instance_buffer;
  
  pipeline->draw_mode = desc.draw_mode;
}

/// Pipeline functions 
///---------------------------------------------------------------------------------------------------------------------

/// *** Graphics ***
/// ---------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
