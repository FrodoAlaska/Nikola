#include "nikola/nikola_render.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_gfx.h"

#include "render_shaders.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Consts

const sizei MAX_QUADS    = 10000;
const sizei MAX_VERTICES = MAX_QUADS * 4;

/// Consts
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ShapeType 
enum ShapeType {
  SHAPE_TYPE_QUAD    = 0, 
  SHAPE_TYPE_CIRCLE  = 1,
  SHAPE_TYPE_POLYGON = 2,

  SHAPE_TYPES_MAX = SHAPE_TYPE_POLYGON + 1,
};

/// ShapeType 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// BatchCall
struct BatchCall {
  sizei vertices_count = 0;
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

  GfxShader* shaders[SHAPE_TYPES_MAX];

  GfxPipelineDesc pipe_desc = {};
  GfxPipeline* pipeline     = nullptr; 
  GfxTexture* white_texture = nullptr;

  DynamicArray<BatchCall> batches[SHAPE_TYPES_MAX];
  HashMap<GfxTexture*, i32> textures_cache;
  
  Mat4 ortho = Mat4(1.0f);
};

static BatchRenderer s_batch;
/// BatchRenderer
/// ----------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void init_default_shaders() {
  s_batch.shaders[SHAPE_TYPE_QUAD]    = gfx_shader_create(s_batch.context, generate_batch_quad_shader());
  s_batch.shaders[SHAPE_TYPE_CIRCLE]  = gfx_shader_create(s_batch.context, generate_batch_circle_shader());
  s_batch.shaders[SHAPE_TYPE_POLYGON] = gfx_shader_create(s_batch.context, generate_batch_polygon_shader());
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
  s_batch.white_texture                         = gfx_texture_create(s_batch.context, desc); 
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

  // Layout init
  s_batch.pipe_desc.layout[0]     = GfxLayoutDesc{"POS", GFX_LAYOUT_FLOAT3, 0};
  s_batch.pipe_desc.layout[1]     = GfxLayoutDesc{"COLOR", GFX_LAYOUT_FLOAT4, 0};
  s_batch.pipe_desc.layout[2]     = GfxLayoutDesc{"TEX", GFX_LAYOUT_FLOAT2, 0};
  s_batch.pipe_desc.layout_count  = 3;

  // Draw mode init 
  s_batch.pipe_desc.draw_mode = GFX_DRAW_MODE_TRIANGLE;

  // Pipeline init
  s_batch.pipeline = gfx_pipeline_create(s_batch.context, s_batch.pipe_desc);
}

static void generate_quad_batch(BatchCall* batch, const Vec2& position, const Vec2& size, const Vec4& color, const f32 sides) {
  // Matrices 
  Mat4 model     = mat4_translate(Vec3(position.x, position.y, 0.0f)) * 
                   mat4_scale(Vec3(size.x, size.y, 0.0f));
  Mat4 world_mat = s_batch.ortho * model;

  // Top-left
  Vertex3D_PCUV v1 = {
    .position       = world_mat * Vec4(-0.5f, -0.5f, sides, 1.0f),
    .color          = color,
    .texture_coords = Vec2(0.0f),
  };
  batch->vertices.push_back(v1);

  // Top-right
  Vertex3D_PCUV v2 = {
    .position       = world_mat * Vec4( 0.5f, -0.5f, sides, 1.0f),
    .color          = color,
    .texture_coords = Vec2(1.0f, 0.0f),
  };
  batch->vertices.push_back(v2);

  // Bottom-right
  Vertex3D_PCUV v3 = {
    .position       = world_mat * Vec4( 0.5f,  0.5f, sides, 1.0f),
    .color          = color,
    .texture_coords = Vec2(1.0f),
  };
  batch->vertices.push_back(v3);
  batch->vertices.push_back(v3);

  // Bottom-left
  Vertex3D_PCUV v4 = {
    .position       = world_mat * Vec4(-0.5f,  0.5f, sides, 1.0f),
    .color          = color,
    .texture_coords = Vec2(0.0f, 1.0),
  };
  batch->vertices.push_back(v4);
  batch->vertices.push_back(v1);

  // New texture added!
  batch->vertices_count += 6;
}

static void flush_batch(BatchCall& batch, GfxShader* shader) {
  // An empty batch is no use for us... 
  if(batch.vertices.size() == 0) {
    return;
  }

  // Apply the batch
  s_batch.pipe_desc.vertices_count = batch.vertices_count;
  gfx_shader_use(shader);
  gfx_texture_use(&batch.texture, 1);

  // Update the vertex buffer
  gfx_buffer_update(s_batch.pipe_desc.vertex_buffer, 
                    0, 
                    sizeof(Vertex3D_PCUV) * batch.vertices.size(), 
                    batch.vertices.data());

  // Render the batch
  gfx_pipeline_update(s_batch.pipeline, s_batch.pipe_desc); 
  gfx_pipeline_draw_vertex(s_batch.pipeline); 

  // Reset back to normal
  batch.vertices_count = 0;
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

  // Texture init 
  init_default_texture();

  // Shaders init
  init_default_shaders();

  // Default batch init
  s_batch.batches[SHAPE_TYPE_QUAD].reserve(32);
  BatchCall default_batch = {
    .vertices_count = 0, 
    .texture        = s_batch.white_texture, 
  };
 
  s_batch.batches[SHAPE_TYPE_QUAD].push_back(default_batch);
  s_batch.batches[SHAPE_TYPE_CIRCLE].push_back(default_batch);
  s_batch.batches[SHAPE_TYPE_POLYGON].push_back(default_batch);

  NIKOLA_LOG_INFO("Successfully initialized the batch renderer");
}

void batch_renderer_shutdown() {
  gfx_pipeline_destroy(s_batch.pipeline);

  for(sizei i = 0; i < SHAPE_TYPES_MAX; i++) {
    gfx_shader_destroy(s_batch.shaders[i]);
    s_batch.batches[i].clear();
  }
  
  NIKOLA_LOG_INFO("Batch renderer was successfully shutdown");
}

void batch_renderer_begin() {
  // Get the size of the window 
  i32 width, height; 
  window_get_size(s_batch.ctx_desc.window, &width, &height);

  // Calculate the orthographic camera view
  s_batch.ortho = mat4_ortho(0.0f, (f32)width, (f32)height, 0.0f);
}

void batch_renderer_end() {
  // Render all of the quads 
  for(auto& batch : s_batch.batches[SHAPE_TYPE_QUAD]) {
    flush_batch(batch, s_batch.shaders[SHAPE_TYPE_QUAD]);
  }
  
  // Render all of the circles 
  for(auto& batch : s_batch.batches[SHAPE_TYPE_CIRCLE]) {
    flush_batch(batch, s_batch.shaders[SHAPE_TYPE_CIRCLE]);
  }
  
  // Render all of the polygons 
  for(auto& batch : s_batch.batches[SHAPE_TYPE_POLYGON]) {
    flush_batch(batch, s_batch.shaders[SHAPE_TYPE_POLYGON]);
  }
}

void batch_render_texture(GfxTexture* texture, const Vec2& position, const Vec2& size, const Vec4& tint) {
  // The texture is new and therefore should be added to the cache
  if(s_batch.textures_cache.find(texture) == s_batch.textures_cache.end()) {
    s_batch.textures_cache[texture] = s_batch.textures_cache.size();

    BatchCall new_batch = {
      .vertices_count = 0, 
      .texture        = texture,
    };
    s_batch.batches[SHAPE_TYPE_QUAD].push_back(new_batch);
  }

  // Retrieve the texture index from the cache
  i32 index        = s_batch.textures_cache[texture];
  BatchCall* batch = &s_batch.batches[SHAPE_TYPE_QUAD][index]; 

  // We cannot render more than the maximum number of vertices
  if(batch->vertices_count >= MAX_VERTICES) {
    flush_batch(*batch, s_batch.shaders[SHAPE_TYPE_QUAD]);
  }
  
  // Generate vertices of a quad 
  generate_quad_batch(batch, position, size, tint, 4.0f);
}


void batch_render_quad(const Vec2& position, const Vec2& size, const Vec4& color) {
  // Retrieve the texture index from the cache
  BatchCall* batch = &s_batch.batches[SHAPE_TYPE_QUAD][0]; 

  // We cannot render more than the maximum number of vertices
  if(batch->vertices_count >= MAX_VERTICES) {
    flush_batch(*batch, s_batch.shaders[SHAPE_TYPE_QUAD]);
  }
  
  // Generate vertices of a quad 
  generate_quad_batch(batch, position, size, color, 4.0f);
}

void batch_render_circle(const Vec2& center, const f32 radius, const Vec4& color) {
  // Retrieve the texture index from the cache
  BatchCall* batch = &s_batch.batches[SHAPE_TYPE_CIRCLE][0]; 

  // We cannot render more than the maximum number of vertices
  if(batch->vertices_count >= MAX_VERTICES) {
    flush_batch(*batch, s_batch.shaders[SHAPE_TYPE_CIRCLE]);
  }
  
  // Generate vertices of a quad 
  generate_quad_batch(batch, center, Vec2(radius), color, 0.0f);
}

void batch_render_polygon(const Vec2& center, const f32 radius, const u32 sides, const Vec4& color) {
  // Retrieve the texture index from the cache
  BatchCall* batch = &s_batch.batches[SHAPE_TYPE_POLYGON][0]; 

  // We cannot render more than the maximum number of vertices
  if(batch->vertices_count >= MAX_VERTICES) {
    flush_batch(*batch, s_batch.shaders[SHAPE_TYPE_POLYGON]);
  }
  
  // Generate vertices of a quad 
  NIKOLA_LOG_TRACE("SIDES = %f", (f32)sides);
  generate_quad_batch(batch, center, Vec2(radius), color, (f32)sides);
}

/// Batch renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
