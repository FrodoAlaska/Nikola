#include "nikola/nikola_gfx.h"
#include "nikola/nikola_base.h"

//////////////////////////////////////////////////////////////////////////

#ifdef NIKOLA_GFX_CONTEXT_OPENGL  // OpenGL check

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
/// GfxContext
struct GfxContext {
  GfxContextDesc desc = {};
  GfxStates states;

  u32 current_target = 0; 

  u32 default_clear_flags = 0;
  u32 current_clear_flags = 0;

  bool has_vsync      = false;
};
/// GfxContext
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxFramebuffer
struct GfxFramebuffer {
  GfxFramebufferDesc desc = {};
  
  u32 clear_flags;
  u32 id;

  u32 targets[RENDER_TARGETS_MAX];
  u32 color_buffers_count = 0;
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

  u32 id, vert_id, frag_id;
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

  GfxBuffer* index_buffer  = nullptr; 
  sizei index_count        = 0;

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

static GLenum get_gl_compare_func(const GfxCompareFunc func) {
  switch(func) {
    case GFX_COMPARE_ALWAYS:
      return GL_ALWAYS;
    case GFX_COMPARE_NEVER:
      return GL_NEVER;
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
  set_blend_state(gfx);
  set_cull_state(gfx);

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
}

static u32 get_gl_clear_flags(const u32 flags) {
  u32 gl_flags = 0;

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

static void framebuffer_attach(GfxFramebuffer* framebuffer, GfxTexture* attachment) {
  NIKOLA_ASSERT(framebuffer, "Invalid GfxFramebuffer struct passed");
  NIKOLA_ASSERT(attachment, "Invalid GfxTexture struct passed for attachment");
  
  switch(attachment->desc.type) {
    case GFX_TEXTURE_RENDER_TARGET:
      glNamedFramebufferTexture(framebuffer->id, 
                                GL_COLOR_ATTACHMENT0 + framebuffer->color_buffers_count, 
                                attachment->id, 
                                0);
      framebuffer->targets[framebuffer->color_buffers_count] = GL_COLOR_ATTACHMENT0 + framebuffer->color_buffers_count;
      framebuffer->color_buffers_count++;
      break;
    case GFX_TEXTURE_DEPTH_STENCIL_TARGET:
      glNamedFramebufferRenderbuffer(framebuffer->id, 
                                     GL_DEPTH_STENCIL_ATTACHMENT, 
                                     GL_RENDERBUFFER, 
                                     attachment->id);
      break;
    case GFX_TEXTURE_DEPTH_TARGET:
      glNamedFramebufferRenderbuffer(framebuffer->id, 
                                     GL_DEPTH_ATTACHMENT, 
                                     GL_RENDERBUFFER, 
                                     attachment->id);
      break;
    case GFX_TEXTURE_STENCIL_TARGET:
      glNamedFramebufferRenderbuffer(framebuffer->id, 
                                     GL_STENCIL_ATTACHMENT, 
                                     GL_RENDERBUFFER, 
                                     attachment->id);
      break;
    default:
      NIKOLA_LOG_FATAL("Cannot attach a non render target texture type");
      return;
  }

  if(glCheckNamedFramebufferStatus(framebuffer->id, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    NIKOLA_LOG_WARN("GL-ERROR: Framebuffer %i is incomplete", framebuffer->id);
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
    case GFX_LAYOUT_MAT2:
      return sizeof(f32) * 4;
    case GFX_LAYOUT_MAT3:
      return sizeof(f32) * 9;
    case GFX_LAYOUT_MAT4:
      return sizeof(f32) * 16;
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
    case GFX_LAYOUT_MAT2:
    case GFX_LAYOUT_MAT3:
    case GFX_LAYOUT_MAT4:
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

static sizei get_layout_count(const GfxLayoutType layout) {
  switch(layout) {
    case GFX_LAYOUT_FLOAT1:
    case GFX_LAYOUT_INT1:
    case GFX_LAYOUT_UINT1:
      return 1;
    case GFX_LAYOUT_FLOAT2:
    case GFX_LAYOUT_INT2:
    case GFX_LAYOUT_UINT2:
    case GFX_LAYOUT_MAT2:
      return 2;
    case GFX_LAYOUT_FLOAT3:
    case GFX_LAYOUT_INT3:
    case GFX_LAYOUT_UINT3:
    case GFX_LAYOUT_MAT3:
      return 3;
    case GFX_LAYOUT_FLOAT4:
    case GFX_LAYOUT_INT4:
    case GFX_LAYOUT_UINT4:
    case GFX_LAYOUT_MAT4:
      return 4;
    default:
      return 0;
  }
}

static sizei get_semantic_count(const GfxLayoutType layout) {
  switch(layout) {
    case GFX_LAYOUT_MAT2:
      return 2;
    case GFX_LAYOUT_MAT3:
      return 3;
    case GFX_LAYOUT_MAT4:
      return 4;
    default:
      return 1;
  }
}

static sizei get_semantic_size(const GfxLayoutType layout) {
  switch(layout) {
    case GFX_LAYOUT_MAT2:
      return sizeof(f32) * 2;
    case GFX_LAYOUT_MAT3:
      return sizeof(f32) * 3;
    case GFX_LAYOUT_MAT4:
      return sizeof(f32) * 4;
    default: // Other types don't have semantics 
      return 0;
  }
}

static sizei calc_stride(const GfxLayoutDesc* layout, const sizei count) {
  sizei stride = 0; 

  for(sizei i = 0; i < count; i++) {
    stride += get_layout_size(layout[i].type);
  }

  return stride;
}

static bool is_semantic_attrib(const GfxLayoutType layout) {
  return layout == GFX_LAYOUT_MAT2 || 
         layout == GFX_LAYOUT_MAT3 || 
         layout == GFX_LAYOUT_MAT4;
}

static void set_vertex_attrib(const u32 vao, const GfxLayoutDesc& layout, const sizei index, sizei* offset) {
  glEnableVertexArrayAttrib(vao, index);

  GLenum gl_comp_type = get_layout_type(layout.type);
  sizei comp_count    = get_layout_count(layout.type);
  sizei size          = get_layout_size(layout.type);

  glVertexArrayAttribFormat(vao, index, comp_count, gl_comp_type, false, *offset);
  glVertexArrayBindingDivisor(vao, index, layout.instance_rate);
  glVertexArrayAttribBinding(vao, index, 0);

  *offset += size;
}

static sizei set_semantic_attrib(const u32 vao, const GfxLayoutDesc& layout, const sizei index, sizei* offset) {
  sizei semantic_count = get_semantic_count(layout.type);  
  sizei semantic_size  = get_semantic_size(layout.type); 
  sizei semantic_index = 0;
  
  GLenum gl_comp_type = get_layout_type(layout.type);
  sizei comp_count    = get_layout_count(layout.type);

  for(sizei j = 0; j < semantic_count; j++) {
    semantic_index = j + index;

    glEnableVertexArrayAttrib(vao, semantic_index);
  
    glVertexArrayAttribFormat(vao, semantic_index, comp_count, gl_comp_type, false, *offset);
    glVertexArrayBindingDivisor(vao, semantic_index, layout.instance_rate);
    glVertexArrayAttribBinding(vao, semantic_index, 0);

    *offset += semantic_size;
  }

  return semantic_index;
}

static sizei set_buffer_layout(const u32 vao, const GfxLayoutDesc* layout, const sizei layout_count) {
  sizei stride         = calc_stride(layout, layout_count);
  sizei offset         = 0;
  sizei semantic_index = 0;

  for(sizei i = 0; i < layout_count; i++) {
    /// @NOTE: A "semantic" is the inner value of an attribute. 
    /// For example, the semantic of a 'Mat4' is just a `Vec4` or, rather, 
    /// 4 `Vec4`s. In that case `semantic_count` would be `4` and `semantic_size`
    /// would be `16` since it's just a `FLOAT4` under the hood.
    
    // Different configuration if the current layout is a semantic or not
    if(is_semantic_attrib(layout[i].type)) {
      semantic_index = set_semantic_attrib(vao, layout[i], semantic_index, &offset); 
    }
    else {
      set_vertex_attrib(vao, layout[i], semantic_index, &offset);
    }

    semantic_index += 1;
  }
  
  return stride;
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

static GLenum create_gl_texture(const GfxTextureType type) {
  u32 id = 0;

  switch(type) {
    case GFX_TEXTURE_1D:
      glCreateTextures(GL_TEXTURE_1D, 1, &id);
      break;
    case GFX_TEXTURE_2D:
    case GFX_TEXTURE_RENDER_TARGET:
      glCreateTextures(GL_TEXTURE_2D, 1, &id);
      break;
    case GFX_TEXTURE_3D:
      glCreateTextures(GL_TEXTURE_3D, 1, &id);
      break;
    case GFX_TEXTURE_DEPTH_STENCIL_TARGET:
      glCreateRenderbuffers(1, &id);
      break;
    default:
      break;
  } 

  return id;
}

static void update_gl_texture_pixels(GfxTexture* texture, GLenum gl_format, GLenum gl_pixel_type) {
  switch(texture->desc.type) {
    case GFX_TEXTURE_1D: 
      glTextureSubImage1D(texture->id, 
                          texture->desc.mips, 
                          0, 
                          texture->desc.width, 
                          gl_format, 
                          gl_pixel_type, 
                          texture->desc.data);
      break;
    case GFX_TEXTURE_2D:
      glTextureSubImage2D(texture->id, 
                          0, 
                          0, 0,
                          texture->desc.width, texture->desc.height,
                          gl_format, 
                          gl_pixel_type, 
                          texture->desc.data);
      break;
    case GFX_TEXTURE_3D:
      glTextureSubImage3D(texture->id, 
                          0, 
                          0, 0, 0,
                          texture->desc.width, texture->desc.height, texture->desc.depth,
                          gl_format, 
                          gl_pixel_type, 
                          texture->desc.data);
      break;
    case GFX_TEXTURE_RENDER_TARGET:
      glTextureSubImage2D(texture->id, 
                          0, 
                          0, 0,
                          texture->desc.width, texture->desc.height,
                          gl_format, 
                          gl_pixel_type, 
                          nullptr);
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
      glTextureStorage1D(texture->id, texture->desc.mips, in_format, texture->desc.width);
      break;
    case GFX_TEXTURE_2D:
      glTextureStorage2D(texture->id, texture->desc.mips, in_format, texture->desc.width, texture->desc.height);
      break;
    case GFX_TEXTURE_3D:
      glTextureStorage3D(texture->id, texture->desc.mips, in_format, texture->desc.width, texture->desc.height, texture->desc.height);
      break;
    case GFX_TEXTURE_RENDER_TARGET:
      glTextureStorage2D(texture->id, texture->desc.mips, in_format, texture->desc.width, texture->desc.height);
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

void gfx_context_set_target(GfxContext* gfx, GfxFramebuffer* framebuffer) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");

  // Set the default values for when binding to the default framebuffer
  gfx->current_clear_flags = gfx->default_clear_flags;
  gfx->current_target      = 0;

  if(framebuffer) {
    gfx->current_clear_flags = framebuffer->clear_flags;
    gfx->current_target      = framebuffer->id; 

    // Set the number render targets to draw to
    glNamedFramebufferDrawBuffers(framebuffer->id, framebuffer->color_buffers_count, framebuffer->targets);
  }
}

void gfx_context_clear(GfxContext* gfx, const f32 r, const f32 g, const f32 b, const f32 a) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");
  
  glBindFramebuffer(GL_FRAMEBUFFER, gfx->current_target);
  glClear(gfx->current_clear_flags);
  glClearColor(r, g, b, a);
}

void gfx_context_present(GfxContext* gfx) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");
  window_swap_buffers(gfx->desc.window);
}

/// Context functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Framebuffer functions

GfxFramebuffer* gfx_framebuffer_create(GfxContext* gfx, const GfxFramebufferDesc& desc) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");

  GfxFramebuffer* buff = (GfxFramebuffer*)memory_allocate(sizeof(GfxFramebuffer));
  memory_zero(buff, sizeof(GfxFramebuffer));

  buff->desc        = desc; 
  buff->clear_flags = get_gl_clear_flags(desc.clear_flags);

  glCreateFramebuffers(1, &buff->id);

  // Attach all of the given attachments
  for(sizei i = 0; i < desc.attachments_count; i++) {
    framebuffer_attach(buff, desc.attachments[i]);
  }

  return buff;
}

void gfx_framebuffer_destroy(GfxFramebuffer* framebuffer) {
  if(!framebuffer) {
    return;
  }

  glDeleteFramebuffers(1, &framebuffer->id);
  memory_free(framebuffer);
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

  // Attach all of the given attachments
  for(sizei i = 0; i < desc.attachments_count; i++) {
    framebuffer_attach(framebuffer, desc.attachments[i]);
  }
}

/// Framebuffer functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Buffer functions 

GfxBuffer* gfx_buffer_create(GfxContext* gfx, const GfxBufferDesc& desc) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");

  GfxBuffer* buff = (GfxBuffer*)memory_allocate(sizeof(GfxBuffer));
  memory_zero(buff, sizeof(GfxBuffer));
  
  buff->desc          = desc;
  buff->gfx           = gfx; 
  buff->gl_buff_type  = get_buffer_type(desc.type);
  buff->gl_buff_usage = get_buffer_usage(desc.usage);

  glCreateBuffers(1, &buff->id);
  glNamedBufferData(buff->id, desc.size, desc.data, buff->gl_buff_usage);
  
  buff->desc = desc;
  return buff;
}

void gfx_buffer_destroy(GfxBuffer* buff) {
  if(!buff) {
    return;
  }

  glDeleteBuffers(1, &buff->id);
  memory_free(buff);
}

GfxBufferDesc& gfx_buffer_get_desc(GfxBuffer* buffer) {
  NIKOLA_ASSERT(buffer, "Invalid GfxBuffer struct passed");

  return buffer->desc;
}

void gfx_buffer_update(GfxBuffer* buff, const sizei offset, const sizei size, const void* data) {
  NIKOLA_ASSERT(buff->gfx, "Invalid GfxContext struct passed");
  NIKOLA_ASSERT(buff, "Invalid GfxBuffer struct passed");

  buff->desc.size = size;
  buff->desc.data = (void*)data;

  glNamedBufferSubData(buff->id, offset, size, data);
}

/// Buffer functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Shader functions 

GfxShader* gfx_shader_create(GfxContext* gfx, const GfxShaderDesc& desc) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");
  NIKOLA_ASSERT(desc.vertex_source, "Invalid Vertex source passed to the shader");
  NIKOLA_ASSERT(desc.pixel_source, "Invalid Pixel source passed to the shader");

  GfxShader* shader = (GfxShader*)memory_allocate(sizeof(GfxShader));
  memory_zero(shader, sizeof(GfxShader));

  shader->gfx  = gfx;
  shader->desc = desc;

  i32 vert_src_len = strlen(shader->desc.vertex_source);
  i32 frag_src_len = strlen(shader->desc.pixel_source);

  // Vertex shader
  shader->vert_id = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(shader->vert_id, 1, &shader->desc.vertex_source, &vert_src_len); 
  glCompileShader(shader->vert_id);
  check_shader_compile_error(shader->vert_id);
    
  // Fragment shader
  shader->frag_id = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(shader->frag_id, 1, &shader->desc.pixel_source, &frag_src_len); 
  glCompileShader(shader->frag_id);
  check_shader_compile_error(shader->frag_id);

  // Linking
  shader->id = glCreateProgram();
  glAttachShader(shader->id, shader->vert_id);
  glAttachShader(shader->id, shader->frag_id);
  glLinkProgram(shader->id);
  check_shader_linker_error(shader);
  
  // Detaching
  glDetachShader(shader->id, shader->vert_id);
  glDetachShader(shader->id, shader->frag_id);

  return shader;
}

void gfx_shader_destroy(GfxShader* shader) {
  if(!shader) {
    return;
  }
  
  glDeleteProgram(shader->id);
  memory_free(shader);
}

GfxShaderDesc& gfx_shader_get_source(GfxShader* shader) {
  return shader->desc;
}

void gfx_shader_use(GfxShader* shader) {
  NIKOLA_ASSERT(shader->gfx, "Invalid GfxContext struct passed");
  NIKOLA_ASSERT(shader, "Invalid GfxShader struct passed");

  glUseProgram(shader->id);
}

void gfx_shader_update(GfxShader* shader, const GfxShaderDesc& desc) {
  NIKOLA_ASSERT(shader->gfx, "Invalid GfxContext struct passed");
  NIKOLA_ASSERT(shader, "Invalid GfxShader struct passed");
  NIKOLA_ASSERT(desc.vertex_source, "Invalid Vertex source passed to the shader");
  NIKOLA_ASSERT(desc.pixel_source, "Invalid Pixel source passed to the shader");

  shader->desc = desc;
  
  i32 vert_src_len = strlen(shader->desc.vertex_source);
  i32 frag_src_len = strlen(shader->desc.pixel_source);
  
  // Vertex shader
  glShaderSource(shader->vert_id, 1, &shader->desc.vertex_source, &vert_src_len); 
  glCompileShader(shader->vert_id);
  check_shader_compile_error(shader->vert_id);
  
  // Fragment shader
  glShaderSource(shader->frag_id, 1, &shader->desc.pixel_source, &frag_src_len); 
  glCompileShader(shader->frag_id);
  check_shader_compile_error(shader->frag_id);

  // Linking
  glAttachShader(shader->id, shader->vert_id);
  glAttachShader(shader->id, shader->frag_id);
  glLinkProgram(shader->id);
  check_shader_linker_error(shader);
  
  // Detaching
  glDetachShader(shader->id, shader->vert_id);
  glDetachShader(shader->id, shader->frag_id);
}

void gfx_shader_attach_uniform(GfxShader* shader, const GfxShaderType type, GfxBuffer* buffer, const u32 bind_point) {
  NIKOLA_ASSERT(shader->gfx, "Invalid GfxContext struct passed");
  NIKOLA_ASSERT(shader, "Invalid GfxShader struct passed");
   
  glBindBufferBase(GL_UNIFORM_BUFFER, bind_point, buffer->id);
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
  }
}

void gfx_shader_upload_uniform(GfxShader* shader, const i32 location, const GfxLayoutType type, const void* data) {
  gfx_shader_upload_uniform_array(shader, location, 1, type, data);
}

/// Shader functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Texture functions 

GfxTexture* gfx_texture_create(GfxContext* gfx, const GfxTextureDesc& desc) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");

  GfxTexture* texture = (GfxTexture*)memory_allocate(sizeof(GfxTexture));
  memory_zero(texture, sizeof(GfxTexture));
 
  texture->desc = desc;
  texture->gfx  = gfx;
  
  // Getting the appropriate GL pixel format
  GLenum in_format, gl_format, gl_pixel_type;
  get_texture_gl_format(desc.format, &in_format, &gl_format, &gl_pixel_type);

  // Getting the appropriate GL addressing modes
  GLenum gl_wrap_format = get_texture_gl_wrap(desc.wrap_mode);
  
  // Getting the appropriate GL filters
  GLenum min_filter, mag_filter;
  get_texture_gl_filter(desc.filter, &min_filter, &mag_filter); 
  
  // Creating the texutre based on its type
  texture->id = create_gl_texture(desc.type);

  // Setting texture parameters
  glTextureParameteri(texture->id, GL_TEXTURE_WRAP_S, gl_wrap_format);
  glTextureParameteri(texture->id, GL_TEXTURE_WRAP_T, gl_wrap_format);
  glTextureParameteri(texture->id, GL_TEXTURE_MIN_FILTER, min_filter);
  glTextureParameteri(texture->id, GL_TEXTURE_MAG_FILTER, mag_filter);

  // Filling the texture with the data based on its type
  update_gl_texture_storage(texture, in_format);
  update_gl_texture_pixels(texture, gl_format, gl_pixel_type);

  // Generating some mipmaps
  glGenerateTextureMipmap(texture->id);

  return texture;
}

void gfx_texture_destroy(GfxTexture* texture) {
  if(!texture) {
    return;
  }
  
  glDeleteTextures(1, &texture->id);
  memory_free(texture);
}

void gfx_texture_use(GfxTexture** textures, const sizei count) {
  NIKOLA_ASSERT(textures, "Invalid GfxTexture array passed to gfx_texture_use");
  NIKOLA_ASSERT(((count >= 0) && (count <= TEXTURES_MAX)), "The count parametar in gfx_texture_use is invalid");
  
  u32 gl_textures[TEXTURES_MAX];
  for(sizei i = 0; i < count; i++) {
    gl_textures[i] = textures[i]->id;
  }

  glBindTextures(0, count, gl_textures);
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
  get_texture_gl_format(desc.format, &in_format, &gl_format, &gl_pixel_type);

  // Updating the addressing mode
  GLenum gl_wrap_format = get_texture_gl_wrap(desc.wrap_mode);
  
  // Updating the filters
  GLenum min_filter, mag_filter;
  get_texture_gl_filter(desc.filter, &min_filter, &mag_filter); 

  // Set texture parameters again
  glTextureParameteri(texture->id, GL_TEXTURE_WRAP_S, gl_wrap_format);
  glTextureParameteri(texture->id, GL_TEXTURE_WRAP_T, gl_wrap_format);
  glTextureParameteri(texture->id, GL_TEXTURE_MIN_FILTER, min_filter);
  glTextureParameteri(texture->id, GL_TEXTURE_MAG_FILTER, mag_filter);
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

GfxCubemap* gfx_cubemap_create(GfxContext* gfx, const GfxCubemapDesc& desc) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");

  GfxCubemap* cubemap = (GfxCubemap*)memory_allocate(sizeof(GfxCubemap));
  memory_zero(cubemap, sizeof(GfxCubemap));

  cubemap->gfx  = gfx;
  cubemap->desc = desc;
  
  // Getting the appropriate GL pixel format
  GLenum in_format, gl_format, gl_pixel_type;
  get_texture_gl_format(desc.format, &in_format, &gl_format, &gl_pixel_type);

  // Getting the appropriate GL addressing modes
  GLenum gl_wrap_format = get_texture_gl_wrap(desc.wrap_mode);
  
  // Getting the appropriate GL filters
  GLenum min_filter, mag_filter;
  get_texture_gl_filter(desc.filter, &min_filter, &mag_filter); 

  glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &cubemap->id);
  
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

  return cubemap;
}

void gfx_cubemap_destroy(GfxCubemap* cubemap) {
  if(!cubemap) {
    return;
  }
  
  glDeleteTextures(1, &cubemap->id);
  memory_free(cubemap);
}

void gfx_cubemap_use(GfxCubemap** cubemaps, const sizei count) {
  NIKOLA_ASSERT(cubemaps, "Invalid GfxCubemap array passed to gfx_cubemap_use");
  NIKOLA_ASSERT(((count >= 0) && (count <= CUBEMAPS_MAX)), "The count parametar in gfx_cubemap_use is invalid");

  u32 gl_cubemaps[CUBEMAPS_MAX];
  for(sizei i = 0; i < count; i++) {
    gl_cubemaps[i] = cubemaps[i]->id;
  }

  glBindTextures(0, count, gl_cubemaps);
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

GfxPipeline* gfx_pipeline_create(GfxContext* gfx, const GfxPipelineDesc& desc) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext struct passed");

  GfxPipeline* pipe = (GfxPipeline*)memory_allocate(sizeof(GfxPipeline));
  memory_zero(pipe, sizeof(GfxPipeline));

  pipe->desc = desc;
  pipe->gfx  = gfx;

  // VAO init
  glCreateVertexArrays(1, &pipe->vertex_array);

  // Layout init 
  sizei stride = set_buffer_layout(pipe->vertex_array, desc.layout, desc.layout_count); 
  NIKOLA_ASSERT(desc.vertex_buffer, "Must have a vertex buffer to create a GfxPipeline struct");

  // VBO init
  pipe->vertex_buffer = desc.vertex_buffer; 
  pipe->vertex_count  = desc.vertices_count; 

  glVertexArrayVertexBuffer(pipe->vertex_array, 0, pipe->vertex_buffer->id, 0, stride);

  // EBO init
  if(desc.index_buffer) {
    pipe->index_buffer = desc.index_buffer;
    pipe->index_count  = desc.indices_count;

    glVertexArrayElementBuffer(pipe->vertex_array, pipe->index_buffer->id);
  }
  
  // Set the draw mode for the whole pipeline
  pipe->draw_mode = desc.draw_mode;
   
  return pipe;
}

void gfx_pipeline_destroy(GfxPipeline* pipeline) {
  NIKOLA_ASSERT(pipeline, "Attempting to free an invalid GfxPipeline");

  // Deleting the buffers
  glDeleteVertexArrays(1, &pipeline->vertex_array);

  // Free the pipeline
  memory_free(pipeline);
}

GfxPipelineDesc& gfx_pipeline_get_desc(GfxPipeline* pipeline) {
  NIKOLA_ASSERT(pipeline, "Invalid GfxPipeline struct passed");

  return pipeline->desc;
}

void gfx_pipeline_update(GfxPipeline* pipeline, const GfxPipelineDesc& desc) {
  NIKOLA_ASSERT(pipeline, "Invalid GfxPipeline struct passed to gfx_pipeline_update");

  pipeline->desc = desc;
}

void gfx_pipeline_draw_vertex(GfxPipeline* pipeline) {
  NIKOLA_ASSERT(pipeline->gfx, "Invalid GfxContext struct passed");
  NIKOLA_ASSERT(pipeline, "Invalid GfxPipeline struct passed");
  NIKOLA_ASSERT(pipeline->vertex_buffer, "Must have a valid vertex buffer to draw");

  // Setting the depth mask state of the pipeline 
  glDepthMask(pipeline->desc.depth_mask);

  // Setting the stencil mask of the pipeline state
  glStencilMask(pipeline->desc.stencil_ref);

  // Setting the blend color of the pipeline state
  const f32* blend_color = pipeline->desc.blend_factor;
  glBlendColor(blend_color[0], blend_color[1], blend_color[2], blend_color[3]);

  // Bind the vertex array
  glBindVertexArray(pipeline->vertex_array);

  // Draw the vertices
  GLenum draw_mode = get_draw_mode(pipeline->desc.draw_mode); 
  glDrawArrays(draw_mode, 0, pipeline->desc.vertices_count);

  // Unbind the vertex array for debugging purposes
  glBindVertexArray(0);
}

void gfx_pipeline_draw_index(GfxPipeline* pipeline) {
  NIKOLA_ASSERT(pipeline->gfx, "Invalid GfxContext struct passed");
  NIKOLA_ASSERT(pipeline, "Invalid GfxPipeline struct passed");
  NIKOLA_ASSERT(pipeline->vertex_buffer, "Must have a valid vertex buffer to draw");
  NIKOLA_ASSERT(pipeline->index_buffer, "Must have a valid index buffer to draw");

  // Setting the depth mask state of the pipeline 
  glDepthMask(pipeline->desc.depth_mask);

  // Setting the stencil mask of the pipeline state
  glStencilMask(pipeline->desc.stencil_ref);

  // Setting the blend color of the pipeline state
  const f32* blend_color = pipeline->desc.blend_factor;
  glBlendColor(blend_color[0], blend_color[1], blend_color[2], blend_color[3]);

  // Bind the vertex array
  glBindVertexArray(pipeline->vertex_array);

  // Draw the indices
  GLenum draw_mode = get_draw_mode(pipeline->desc.draw_mode); 
  glDrawElements(draw_mode, pipeline->desc.indices_count, GL_UNSIGNED_INT, 0);
  
  // Unbind the vertex array for debugging purposes
  glBindVertexArray(0);
}

/// Pipeline functions 
///---------------------------------------------------------------------------------------------------------------------

/// *** Graphics ***
/// ---------------------------------------------------------------------

} // End of nikola

#endif // OpenGL check

//////////////////////////////////////////////////////////////////////////
