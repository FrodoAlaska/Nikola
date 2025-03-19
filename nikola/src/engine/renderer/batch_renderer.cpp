#include "nikola/nikola_render.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_gfx.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Consts

const sizei MAX_QUADS    = 10000;
const sizei MAX_VERTICES = MAX_QUADS * 4;
const sizei MAX_INDICES  = MAX_QUADS * 6;

/// Consts
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// BatchCall
struct BatchCall {
  sizei indices_count = 0;
  GfxTexture* texture = nullptr;
  DynamicArray<Vertex3D_PCUV> vertices; 
};
/// BatchCall
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// BatchRenderer
struct BatchRenderer {
  GfxContext* context     = nullptr;
  GfxContextDesc ctx_desc = {}; 

  GfxPipelineDesc pipe_desc = {};
  GfxPipeline* pipeline     = nullptr; 
  GfxTexture* white_texture = nullptr;

  DynamicArray<BatchCall> batches;
  HashMap<GfxTexture*, i32> textures_cache;

  Vec4 quad_vertices[4];
  Mat4 ortho = Mat4(1.0f);
};

static BatchRenderer s_batch;
/// BatchRenderer
/// ----------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static GfxShaderDesc init_batch_shader() {
  GfxShaderDesc desc;

  // Vertex shader init
  desc.vertex_source = 
    "#version 460 core\n"
    "\n"
    "// Layouts\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec4 aColor;\n"
    "layout (location = 2) in vec2 aTextureCoords;\n"
    "\n"
    "// Outputs\n"
    "out VS_OUT {\n"
    "  vec4 out_color;\n"
    "  vec2 tex_coords;\n"
    "} vs_out;\n"
    "\n"
    "void main() {\n"
    "  vs_out.out_color  = aColor;\n"
    "  vs_out.tex_coords = aTextureCoords;\n"
    "\n"
    "  gl_Position = vec4(aPos, 1.0f);\n"
    "}\n"
    "\n";
  
  // Fragment shader init
  desc.pixel_source = 
    "#version 460 core\n"
    "\n"
    "// Outputs\n"
    "layout (location = 0) out vec4 frag_color;\n"
    "\n"
    "// Inputs\n"
    "in VS_OUT {\n"
    "  vec4 out_color;\n"
    "  vec2 tex_coords;\n"
    "} fs_in;\n"
    "\n"
    "// Uniforms\n"
    "uniform sampler2D u_texture;\n"
    "\n"
    "void main() {\n"
    "  frag_color = texture(u_texture, fs_in.tex_coords) * fs_in.out_color;\n"
    "}\n";

  return desc;
}

static void init_default_texture() {
  u32 pixels = 0xffffffff;
  GfxTextureDesc desc = {
    .width     = 1, 
    .height    = 1, 
    .depth     = 0, 
    .mips      = 1,
    .type      = GFX_TEXTURE_2D, 
    .format    = GFX_TEXTURE_FORMAT_RGBA8, 
    .filter    = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
    .wrap_mode = GFX_TEXTURE_WRAP_CLAMP,
    .data      = &pixels,
  };

  // Create the texture and add it to the cache
  s_batch.white_texture = gfx_texture_create(s_batch.context, desc);
  s_batch.textures_cache[s_batch.white_texture] = 0;
}

static void init_pipeline() {
  // Vertex buffer init 
  GfxBufferDesc vert_desc = {
    .data  = nullptr,
    .size  = sizeof(Vertex3D_PCUV) * MAX_VERTICES,
    .type  = GFX_BUFFER_VERTEX, 
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  s_batch.pipe_desc.vertex_buffer  = gfx_buffer_create(s_batch.context, vert_desc);
  s_batch.pipe_desc.vertices_count = 0;
 
  // Index buffer init
  u32 indices[MAX_INDICES];
  u32 offset = 0;
  for(sizei i = 0; i < MAX_INDICES; i += 6) {
    indices[i + 0] = 0 + offset;
    indices[i + 1] = 1 + offset;
    indices[i + 2] = 2 + offset;
    
    indices[i + 3] = 2 + offset;
    indices[i + 4] = 3 + offset;
    indices[i + 5] = 0 + offset;

    offset += 4; 
  }
  GfxBufferDesc index_desc = {
    .data  = indices,
    .size  = sizeof(u32) * MAX_INDICES,
    .type  = GFX_BUFFER_INDEX, 
    .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  s_batch.pipe_desc.index_buffer  = gfx_buffer_create(s_batch.context, index_desc);
  s_batch.pipe_desc.indices_count = 0;

  // Shader init
  GfxShaderDesc shader_desc = init_batch_shader();
  s_batch.pipe_desc.shader  = gfx_shader_create(s_batch.context, shader_desc); 

  // Layout init
  s_batch.pipe_desc.layout[0]     = GfxLayoutDesc{"POS", GFX_LAYOUT_FLOAT3, 0};
  s_batch.pipe_desc.layout[1]     = GfxLayoutDesc{"COLOR", GFX_LAYOUT_FLOAT4, 0};
  s_batch.pipe_desc.layout[2]     = GfxLayoutDesc{"TEX", GFX_LAYOUT_FLOAT2, 0};
  s_batch.pipe_desc.layout_count  = 3;

  // Draw mode init 
  s_batch.pipe_desc.draw_mode = GFX_DRAW_MODE_TRIANGLE;

  // Texture init 
  init_default_texture();

  // Pipeline init
  s_batch.pipeline = gfx_pipeline_create(s_batch.context, s_batch.pipe_desc);
}

static void flush_batch(BatchCall& batch) {
  // An empty batch is no use for us... 
  if(batch.vertices.size() == 0) {
    return;
  }

  // Apply the batch
  s_batch.pipe_desc.indices_count  = batch.indices_count;
  s_batch.pipe_desc.textures[0]    = batch.texture;
  s_batch.pipe_desc.textures_count = 1;

  // Update the vertex buffer
  gfx_buffer_update(s_batch.pipe_desc.vertex_buffer, 
                    0, 
                    sizeof(Vertex3D_PCUV) * batch.vertices.size(), 
                    batch.vertices.data());

  // Render the batch
  gfx_context_apply_pipeline(s_batch.context, s_batch.pipeline, s_batch.pipe_desc);
  gfx_pipeline_draw_index(s_batch.pipeline); 

  // Reset back to normal
  batch.indices_count = 0;
  batch.vertices.clear();
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Batch renderer functions

void batch_renderer_init() {
  // Context init
  s_batch          = {};
  s_batch.context  = (GfxContext*)renderer_get_context();
  s_batch.ctx_desc = gfx_context_get_desc(s_batch.context);

  // Pipeline init
  init_pipeline(); 

  // Default batch init
  s_batch.batches.reserve(32);
  BatchCall default_batch = {
    .indices_count = 0, 
    .texture       = s_batch.white_texture, 
  };
  s_batch.batches.push_back(default_batch);

  // Preset quad vertices init
  s_batch.quad_vertices[0] = Vec4(-0.5f, -0.5f, 0.0f, 1.0f);
  s_batch.quad_vertices[1] = Vec4( 0.5f, -0.5f, 0.0f, 1.0f);
  s_batch.quad_vertices[2] = Vec4( 0.5f,  0.5f, 0.0f, 1.0f);
  s_batch.quad_vertices[3] = Vec4(-0.5f,  0.5f, 0.0f, 1.0f);

  NIKOLA_LOG_INFO("Successfully initialized the batch renderer");
}

void batch_renderer_shutdown() {
  gfx_pipeline_destroy(s_batch.pipeline);
  gfx_shader_destroy(s_batch.pipe_desc.shader);
  gfx_texture_destroy(s_batch.white_texture);

  s_batch.batches.clear();
  
  NIKOLA_LOG_INFO("Batch renderer was successfully shutdown");
}

void batch_renderer_begin() {
  i32 width, height; 
  window_get_size(s_batch.ctx_desc.window, &width, &height);

  s_batch.ortho = mat4_ortho(0.0f, (f32)width, (f32)height, 0.0f);
}

void batch_renderer_end() {
  // Render all of the batches 
  for(auto& batch : s_batch.batches) {
    flush_batch(batch);
  }
}

void batch_render_quad(const Vec2& position, const Vec2& size, const Vec4& color) {
  batch_render_texture(s_batch.white_texture, position, size, color); 
}

void batch_render_texture(GfxTexture* texture, const Vec2& position, const Vec2& size, const Vec4& tint) {
  // The texture is new and therefore should be added to the cache
  if(s_batch.textures_cache.find(texture) == s_batch.textures_cache.end()) {
    s_batch.textures_cache[texture] = s_batch.textures_cache.size();

    BatchCall new_batch = {
      .indices_count = 0, 
      .texture       = texture,
    };
    s_batch.batches.push_back(new_batch);
  }

  // Retrieve the texture index from the cache
  i32 index        = s_batch.textures_cache[texture];
  BatchCall* batch = &s_batch.batches[index]; 

  // We cannot render more than the maximum number of indices
  if(batch->indices_count >= MAX_INDICES) {
    flush_batch(*batch);
  }

  // Matrices 
  Mat4 model     = mat4_translate(Vec3(position.x, position.y, 0.0f)) * 
                   mat4_scale(Vec3(size.x, size.y, 0.0f));
  Mat4 world_mat = s_batch.ortho * model;

  // Top-left
  Vertex3D_PCUV v1 = {
    .position       = world_mat * s_batch.quad_vertices[0],
    .color          = tint,
    .texture_coords = Vec2(0.0f),
  };
  batch->vertices.push_back(v1);

  // Top-right
  Vertex3D_PCUV v2 = {
    .position       = world_mat * s_batch.quad_vertices[1],
    .color          = tint,
    .texture_coords = Vec2(1.0f, 0.0f),
  };
  batch->vertices.push_back(v2);

  // Bottom-right
  Vertex3D_PCUV v3 = {
    .position       = world_mat * s_batch.quad_vertices[2],
    .color          = tint,
    .texture_coords = Vec2(1.0f),
  };
  batch->vertices.push_back(v3);

  // Bottom-left
  Vertex3D_PCUV v4 = {
    .position       = world_mat * s_batch.quad_vertices[3],
    .color          = tint,
    .texture_coords = Vec2(0.0f, 1.0),
  };
  batch->vertices.push_back(v4);

  // New texture added!
  batch->indices_count += 6;
}

/// Batch renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
