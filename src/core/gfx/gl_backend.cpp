#include "nikol_core.h"

//////////////////////////////////////////////////////////////////////////

#ifdef NIKOL_GFX_CONTEXT_OPENGL  // OpenGL check

#include <glad/glad.h>

#include <cstring>

namespace nikol { // Start of nikol

/// ---------------------------------------------------------------------
/// *** Graphics ***

///---------------------------------------------------------------------------------------------------------------------
/// GfxContext
struct GfxContext {
  GfxContextFlags flags;

  sizei buffer_bits = 0;
};
/// GfxContext
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxShader
struct GfxShader {
  u32 id, vert_id, frag_id;

  i8* vert_src; 
  i8* frag_src;
  
  i32 vert_src_len, frag_src_len;
};
/// GfxShader
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTexture
struct GfxTexture {
  u32 id;
};
/// GfxTexture
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxPipeline
struct GfxPipeline {
  u32 vertex_array, vertex_buffer, index_buffer; 

  GfxShader* shader = nullptr; 
  GfxTexture* textures[TEXTURES_MAX] = {};
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

static void gl_check_error(const i8* func_name) {
  GLenum err = glGetError(); 

  switch(err) {
    case GL_INVALID_ENUM: 
      NIKOL_LOG_ERROR("GL_ERROR: FUNC = %s, ERR = GL_INVALID_ENUM", func_name);
      break;
    case GL_INVALID_VALUE: 
      NIKOL_LOG_ERROR("GL_ERROR: FUNC = %s, ERR = GL_INVALID_VALUE", func_name);
      break;
    case GL_INVALID_OPERATION: 
      NIKOL_LOG_ERROR("GL_ERROR: FUNC = %s, ERR = GL_INVALID_OPERATION", func_name);
      break;
    case GL_STACK_OVERFLOW: 
      NIKOL_LOG_ERROR("GL_ERROR: FUNC = %s, ERR = GL_STACK_OVERFLOW", func_name);
      break;
    case GL_STACK_UNDERFLOW: 
      NIKOL_LOG_ERROR("GL_ERROR: FUNC = %s, ERR = GL_STACK_UNDERFLOW", func_name);
      break;
    case GL_OUT_OF_MEMORY: 
      NIKOL_LOG_ERROR("GL_ERROR: FUNC = %s, ERR = GL_OUT_OF_MEMORY", func_name);
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION: 
      NIKOL_LOG_ERROR("GL_ERROR: FUNC = %s, ERR = GL_INVALID_FRAMEBUFFER_OPERATION", func_name);
      break;
    case GL_CONTEXT_LOST: 
      NIKOL_LOG_ERROR("GL_ERROR: FUNC = %s, ERR = GL_CONTEXT_LOST", func_name);
      break;
    case GL_NO_ERROR:
      break;
  }
}

static void set_gfx_flags(GfxContext* gfx) {
  if((gfx->flags & GFX_FLAGS_DEPTH) == GFX_FLAGS_DEPTH) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    gfx->buffer_bits |= GL_DEPTH_BUFFER_BIT; 
  }
  
  if((gfx->flags & GFX_FLAGS_STENCIL) == GFX_FLAGS_STENCIL) {
    glEnable(GL_STENCIL_TEST);
    gfx->buffer_bits |= GL_STENCIL_BUFFER_BIT; 
  }
  
  if((gfx->flags & GFX_FLAGS_BLEND) == GFX_FLAGS_BLEND) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  
  if((gfx->flags & GFX_FLAGS_MSAA) == GFX_FLAGS_MSAA) {
    glEnable(GL_MULTISAMPLE);
  }
}

static GLenum get_buffer_mode(const GfxBufferMode mode) {
  switch(mode) {
    case GFX_BUFFER_MODE_DYNAMIC_COPY:
      return GL_DYNAMIC_COPY;
    case GFX_BUFFER_MODE_DYNAMIC_DRAW:
      return GL_DYNAMIC_DRAW;
    case GFX_BUFFER_MODE_DYNAMIC_READ:
      return GL_DYNAMIC_READ;
    case GFX_BUFFER_MODE_STATIC_COPY:
      return GL_STATIC_COPY;
    case GFX_BUFFER_MODE_STATIC_DRAW:
      return GL_STATIC_DRAW;
    case GFX_BUFFER_MODE_STATIC_READ:
      return GL_STATIC_READ;
    case GFX_BUFFER_MODE_STREAM_COPY:
      return GL_STREAM_COPY;
    case GFX_BUFFER_MODE_STREAM_DRAW:
      return GL_STREAM_DRAW;
    case GFX_BUFFER_MODE_STREAM_READ:
      return GL_STREAM_READ;
    default:
      return 0;
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

static void seperate_shader_src(const i8* src, GfxShader* shader) {
  sizei src_len = strlen(src);
  
  sizei vert_start = 0;
  sizei frag_start = 0;

  // Find the vertex start
  for(sizei i = 0; i < src_len; i++) {
    if(src[i] != '#') { 
      continue;
    }

    // We found the start
    vert_start = i; 
    break;
  }

  // Find the frag start.
  // There's no point going from the beginning of the string. 
  // Just start from where we last stopped.
  for(sizei i = vert_start + 1; i < src_len; i++) {
    if(src[i] != '#') { 
      continue;
    }

    // We found the start
    frag_start = i; 
    break;
  }

  // Calculate where each shader ends
  sizei frag_end = src_len;

  shader->frag_src_len = frag_end - frag_start;
  shader->vert_src_len = frag_start - 1;

  // // Allocate new strings
  shader->vert_src = (i8*)memory_allocate(shader->vert_src_len);
  memory_zero(shader->vert_src, shader->vert_src_len);

  shader->frag_src = (i8*)memory_allocate(shader->frag_src_len);
  memory_zero(shader->frag_src, shader->frag_src_len);

  // Copying the correct strings over 
  memory_copy(shader->vert_src, src, shader->vert_src_len);
  memory_copy(shader->frag_src, &src[frag_start], shader->frag_src_len);
}

static void check_shader_compile_error(const sizei shader) {
  i32 success;
  i8 log_info[512];

  glGetShaderiv(shader, GL_COMPILE_STATUS, &success); 

  if(!success) {
    glGetShaderInfoLog(shader, 512, nullptr, log_info);
    NIKOL_LOG_WARN("SHADER-ERROR: %s", log_info);
  }
}

static void check_shader_linker_error(const GfxShader* shader) {
  i32 success;
  i8 log_info[512];

  glGetProgramiv(shader->id, GL_LINK_STATUS, &success); 

  if(!success) {
    glGetProgramInfoLog(shader->id, 512, nullptr, log_info);
    NIKOL_LOG_WARN("SHADER-ERROR: %s", log_info);
  }
}

static GLenum get_texture_format(const GfxTextureFormat format) {
  switch(format) {
    case GFX_TEXTURE_FORMAT_RED:
      return GL_RED;
    case GFX_TEXTURE_FORMAT_RG:
      return GL_RG;
    case GFX_TEXTURE_FORMAT_RGB:
      return GL_RGB;
    case GFX_TEXTURE_FORMAT_RGBA:
      return GL_RGBA;
    default:
      return 0;
  }
}

static GLenum get_texture_filter(const GfxTextureFilter filter) {
  switch(filter) {
    case GFX_TEXTURE_FILTER_LINEAR:
      return GL_LINEAR;
    case GFX_TEXTURE_FILTER_NEAREST:
      return GL_NEAREST;
    default:
      return 0;
  }
}

static void set_buffer_layout(const GfxBufferDesc* buff, const GfxBufferLayout* layout, const sizei layout_count) {
  sizei stride = calc_stride(layout, layout_count);
  sizei size   = get_layout_size(layout[0]);
  sizei offset = 0;

  for(sizei i = 0; i < layout_count; i++) {
    glEnableVertexAttribArray(i);

    GLenum gl_comp_type = get_layout_type(layout[i]);
    sizei comp_count    = get_layout_count(layout[i]);

    glVertexAttribPointer(i, comp_count, gl_comp_type, false, stride, (void*)offset);
    
    size    = get_layout_size(layout[i]);
    offset += size; 
  }
}

static bool is_buffer_dynamic(const GfxBufferMode& mode) {
  return mode == GFX_BUFFER_MODE_DYNAMIC_DRAW || 
         mode == GFX_BUFFER_MODE_DYNAMIC_COPY || 
         mode == GFX_BUFFER_MODE_DYNAMIC_READ;
}

/// Private functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Context functions 

GfxContext* gfx_context_init(Window* window, const i32 flags) {
  GfxContext* gfx = (GfxContext*)memory_allocate(sizeof(GfxContext));
  
  gfx->buffer_bits = GL_COLOR_BUFFER_BIT;

  // Glad init
  if(!gladLoadGL()) {
    NIKOL_LOG_FATAL("Could not create an OpenGL instance");
    return nullptr;
  }
  
  // Setting the window context to this OpenGL context 
  window_set_current_context(window);

  // Setting up the viewport for OpenGL
  i32 width, height; 
  window_get_size(window, &width, &height);
  glViewport(0, 0, width, height);

  // Setting the flags
  gfx->flags = (GfxContextFlags)flags;
  set_gfx_flags(gfx);

  // Listening to events 
  event_listen(EVENT_WINDOW_FRAMEBUFFER_RESIZED, framebuffer_resize);

  // Getting some OpenGL information
  const u8* vendor = glGetString(GL_VENDOR); 
  const u8* renderer = glGetString(GL_RENDERER); 
  const u8* gl_version = glGetString(GL_VERSION);
  const u8* glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);

  NIKOL_LOG_INFO("A graphics context was successfully created:\n" 
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

  NIKOL_LOG_INFO("The graphics context was successfully destroyed");
  memory_free(gfx);
}

void gfx_context_clear(GfxContext* gfx, const f32 r, const f32 g, const f32 b, const f32 a) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");

  glClear(gfx->buffer_bits);
  glClearColor(r, g, b, a);
}

void gfx_context_set_flag(GfxContext* gfx, const i32 flag, const bool value) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
}

const GfxContextFlags gfx_context_get_flags(GfxContext* gfx) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  return gfx->flags;
}

/// Context functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Shader functions 

GfxShader* gfx_shader_create(const i8* src) {
  GfxShader* shader = (GfxShader*)memory_allocate(sizeof(GfxShader));
  memory_zero(shader, sizeof(GfxShader));

  seperate_shader_src(src, shader);

  // Vertex shader
  shader->vert_id = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(shader->vert_id, 1, &shader->vert_src, &shader->vert_src_len); 
  glCompileShader(shader->vert_id);
  check_shader_compile_error(shader->vert_id);
    
  // Fragment shader
  shader->frag_id = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(shader->frag_id, 1, &shader->frag_src, &shader->frag_src_len); 
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
  glDeleteShader(shader->vert_id);
  glDeleteShader(shader->frag_id);

  // // We don't need the strings anymore
  return shader;
}

void gfx_shader_destroy(GfxShader* shader) {
  if(!shader) {
    return;
  }
 
  memory_free(shader->vert_src);
  memory_free(shader->frag_src);
  memory_free(shader);
}

GfxTexture* gfx_texture_create(const GfxTextureDesc& desc) {
  GfxTexture* texture = (GfxTexture*)memory_allocate(sizeof(GfxTexture));
  memory_zero(texture, sizeof(GfxTexture));
  
  GLenum gl_tex_format    = get_texture_format(desc.format);
  GLenum gl_filter_format = get_texture_filter(desc.filter);

  glGenTextures(1, &texture->id);
  glBindTexture(GL_TEXTURE_2D, texture->id);
  
  glTexImage2D(GL_TEXTURE_2D,
               desc.depth, 
               gl_tex_format,
               desc.width, 
               desc.height, 
               0, 
               gl_tex_format, 
               GL_UNSIGNED_BYTE, 
               desc.data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter_format);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_format);

  return texture;
}

void gfx_texture_destroy(GfxTexture* texture) {
  if(!texture) {
    return;
  }

  memory_free(texture);
}

void gfx_texture_update(GfxTexture* texture, const GfxTextureDesc& desc) {
  glBindTexture(GL_TEXTURE_2D, texture->id);
  
  GLenum gl_tex_format    = get_texture_format(desc.format);
  GLenum gl_filter_format = get_texture_filter(desc.filter);
  
  glTexImage2D(GL_TEXTURE_2D,
               desc.depth, 
               gl_tex_format,
               desc.width, 
               desc.height, 
               0, 
               gl_tex_format, 
               GL_UNSIGNED_BYTE, 
               desc.data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter_format);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_format);
}

const i32 gfx_shader_get_uniform_location(GfxShader* shader, const i8* uniform_name) {
  i32 loc = 0; 

  loc = glGetUniformLocation(shader->id, uniform_name);
  if(loc == UNIFORM_INVALID) {
    NIKOL_LOG_WARN("Could not find uniform \'%s\' in shader", uniform_name);
  }

  return loc;
}

void gfx_shader_set_uniform_data(GfxShader* shader, const i32 location, const GfxUniformType type, const void* data) {
  NIKOL_ASSERT(shader, "Invalid GfxShader struct passed"); 

  // Will not waste time on an invalid uniform location 
  if(location == UNIFORM_INVALID) {
    return;
  }

  glUseProgram(shader->id);

  switch(type) {
    case GFX_UNIFORM_TYPE_FLOAT: 
      glUniform1fv(location, 1, (f32*)data);
    break;  
    case GFX_UNIFORM_TYPE_DOUBLE: 
      glUniform1dv(location, 1, (f64*)data);
    break;
    case GFX_UNIFORM_TYPE_INT: 
      glUniform1iv(location, 1, (i32*)data);
    break;
    case GFX_UNIFORM_TYPE_UINT: 
      glUniform1uiv(location, 1, (u32*)data);
    break;
    case GFX_UNIFORM_TYPE_VEC2: 
      glUniform2fv(location, 1, (f32*)data);
    break;
    case GFX_UNIFORM_TYPE_VEC3: 
      glUniform3fv(location, 1, (f32*)data);
    break;
    case GFX_UNIFORM_TYPE_VEC4: 
      glUniform4fv(location, 1, (f32*)data);
    break;
    case GFX_UNIFORM_TYPE_MAT2: 
      glUniformMatrix2fv(location, 1, false, (f32*)data);
    break;
    case GFX_UNIFORM_TYPE_MAT3: 
      glUniformMatrix3fv(location, 1, false, (f32*)data);
    break;
    case GFX_UNIFORM_TYPE_MAT4: 
      glUniformMatrix4fv(location, 1, false, (f32*)data);
    break;
  }

  gl_check_error("glUniform");
}

void gfx_shader_set_uniform_data_array(GfxShader* shader, const i32 location, const GfxUniformType type, const void* array, const sizei count) {
  NIKOL_ASSERT(shader, "Invalid GfxShader struct passed"); 
  
  // Don't be bothered with invalid uniform locations
  if(location == UNIFORM_INVALID) {
    return;
  }
  
  glUseProgram(shader->id);

  switch(type) {
    case GFX_UNIFORM_TYPE_FLOAT: 
      glUniform1fv(location, count, (f32*)array);
    break;  
    case GFX_UNIFORM_TYPE_DOUBLE: 
      glUniform1dv(location, count, (f64*)array);
    break;
    case GFX_UNIFORM_TYPE_INT: 
      glUniform1iv(location, count, (i32*)array);
    break;
    case GFX_UNIFORM_TYPE_UINT: 
      glUniform1uiv(location, count, (u32*)array);
    break;
    case GFX_UNIFORM_TYPE_VEC2: 
      glUniform2fv(location, count, (f32*)array);
    break;
    case GFX_UNIFORM_TYPE_VEC3: 
      glUniform3fv(location, count, (f32*)array);
    break;
    case GFX_UNIFORM_TYPE_VEC4: 
      glUniform4fv(location, count, (f32*)array);
    break;
    case GFX_UNIFORM_TYPE_MAT2: 
      glUniformMatrix2fv(location, count, false, (f32*)array);
    break;
    case GFX_UNIFORM_TYPE_MAT3: 
      glUniformMatrix3fv(location, count, false, (f32*)array);
    break;
    case GFX_UNIFORM_TYPE_MAT4: 
      glUniformMatrix4fv(location, count, false, (f32*)array);
    break;
  }

  gl_check_error("glUniform");
}

/// Shader functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Pipeline functions 

GfxPipeline* gfx_pipeline_create(GfxContext* gfx, const GfxPipelineDesc& desc) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");

  GfxPipeline* pipe = (GfxPipeline*)memory_allocate(sizeof(GfxPipeline));
  memory_zero(pipe, sizeof(GfxPipeline));

  // VAO init
  glGenVertexArrays(1, &pipe->vertex_array);
  glBindVertexArray(pipe->vertex_array);

  // VBO init
  if(desc.vertex_buffer) {
    GLenum gl_buffer_mode = get_buffer_mode(desc.vertex_buffer->mode);

    glGenBuffers(1, &pipe->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, pipe->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, desc.vertex_buffer->size, desc.vertex_buffer->data, gl_buffer_mode);
  }

  // Layout init 
  set_buffer_layout(desc.vertex_buffer, desc.layout, desc.layout_count); 

  // EBO init
  if(desc.index_buffer) {
    GLenum gl_buffer_mode = get_buffer_mode(desc.index_buffer->mode);
    
    glGenBuffers(1, &pipe->index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pipe->index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, desc.index_buffer->size, desc.index_buffer->data, gl_buffer_mode);
  }
  
  // Shader init 
  pipe->shader = desc.shader; 

  // Textures init
  if(desc.texture_count > 0) {
    for(sizei i = 0; i < desc.texture_count; i++) {
      pipe->textures[i] = desc.textures[i];
    }
  }

  return pipe;
}

void gfx_pipeline_destroy(GfxPipeline* pipeline) {
  NIKOL_ASSERT(pipeline, "Attempting to free an invalid GfxPipeline");

  // Deleting the buffers
  glDeleteBuffers(1, &pipeline->vertex_buffer);
  glDeleteBuffers(1, &pipeline->index_buffer);
  glDeleteVertexArrays(1, &pipeline->vertex_array);

  // Deleting the shader
  memory_free(pipeline->shader->vert_src);
  memory_free(pipeline->shader->frag_src);
  glDeleteProgram(pipeline->shader->id);
  memory_free(pipeline->shader);
  
  memory_free(pipeline);
}

void gfx_pipeline_begin(GfxContext* gfx, GfxPipeline* pipeline) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  NIKOL_ASSERT(pipeline, "Invalid GfxPipeline struct passed");
  NIKOL_ASSERT(pipeline->shader, "Must pass a shader to the pipeline");

  // Bind the shader
  glUseProgram(pipeline->shader->id);

  // Bind the vertex array
  glBindVertexArray(pipeline->vertex_array);
}

void gfx_pipeline_draw_vertex(GfxContext* gfx, GfxPipeline* pipeline, const GfxPipelineDesc* desc) {

  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  NIKOL_ASSERT(pipeline, "Invalid GfxPipeline struct passed");
  NIKOL_ASSERT(desc, "Invalid GfxPipelineDesc struct passed");
  NIKOL_ASSERT(desc->vertex_buffer, "Must have a valid vertex buffer to draw");

  // Draw the textures 
  if(desc->texture_count > 0) {
    for(sizei i = 0; i < desc->texture_count; i++) {
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, desc->textures[i]->id);
    }
  }

  // The buffer data will be re-sumbitted if the buffer is dynamic 
  if(is_buffer_dynamic(desc->vertex_buffer->mode)) {
    GLenum gl_buffer_mode = get_buffer_mode(desc->vertex_buffer->mode);
    
    glBindBuffer(GL_ARRAY_BUFFER, pipeline->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, desc->vertex_buffer->size, desc->vertex_buffer->data, gl_buffer_mode);
  }

  // Draw the vertices
  glDrawArrays(GL_TRIANGLES, 0, desc->vertex_buffer->elements_count);
}

void gfx_pipeline_draw_index(GfxContext* gfx, GfxPipeline* pipeline, const GfxPipelineDesc* desc) {
  NIKOL_ASSERT(gfx, "Invalid GfxContext struct passed");
  NIKOL_ASSERT(pipeline, "Invalid GfxPipeline struct passed");
  NIKOL_ASSERT(desc, "Invalid GfxPipelineDesc struct passed");
  NIKOL_ASSERT(desc->vertex_buffer, "Must have a valid vertex buffer to draw");
  NIKOL_ASSERT(desc->index_buffer, "Must have a valid index buffer to draw");

  // Draw the textures 
  if(desc->texture_count > 0) {
    for(sizei i = 0; i < desc->texture_count; i++) {
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, desc->textures[i]->id);
    }
  }
  
  // The buffer data will be re-sumbitted if the buffer is dynamic 
  if(is_buffer_dynamic(desc->index_buffer->mode)) {
    GLenum gl_buffer_mode = get_buffer_mode(desc->index_buffer->mode);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pipeline->index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, desc->index_buffer->size, desc->index_buffer->data, gl_buffer_mode);
  }

  // Do the same thing with the vertex buffer 
  if(is_buffer_dynamic(desc->vertex_buffer->mode)) {
    GLenum gl_buffer_mode = get_buffer_mode(desc->vertex_buffer->mode);
    
    glBindBuffer(GL_ARRAY_BUFFER, pipeline->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, desc->vertex_buffer->size, desc->vertex_buffer->data, gl_buffer_mode);
  }

  // Draw with the indices
  glDrawElements(GL_TRIANGLES, desc->index_buffer->elements_count, GL_UNSIGNED_INT, 0);
}

/// Pipeline functions 
///---------------------------------------------------------------------------------------------------------------------

/// *** Graphics ***
/// ---------------------------------------------------------------------

} // End of nikol

#endif // OpenGL check

//////////////////////////////////////////////////////////////////////////
