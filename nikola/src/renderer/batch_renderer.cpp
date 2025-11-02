#include "nikola/nikola_render.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_gfx.h"
#include "nikola/nikola_resources.h"
#include "nikola/nikola_timer.h"

#include "shaders/batch.glsl.h"

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
  SHAPE_TYPE_TEXT    = 3,

  SHAPE_TYPES_MAX = SHAPE_TYPE_TEXT + 1,
};

/// ShapeType 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Vertex2D 
struct Vertex2D {
  Vec2 position; 
  Vec4 color; 
  Vec2 texture_coords; 
  Vec2 shape_side;
};
/// Vertex2D 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// BatchCall
struct BatchCall {
  sizei vertices_count = 0;
  GfxTexture* texture = nullptr;

  DynamicArray<Vertex2D> vertices; 
};
/// BatchCall
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// BatchRenderer
struct BatchRenderer {
  GfxContext* context     = nullptr;
  GfxContextDesc ctx_desc = {}; 

  GfxShader* shader;

  GfxPipelineDesc pipe_desc = {};
  GfxPipeline* pipeline     = nullptr; 
  GfxTexture* white_texture = nullptr;

  DynamicArray<BatchCall> batches;
  HashMap<GfxTexture*, i32> textures_cache;
  
  Mat4 ortho = Mat4(1.0f);
};

static BatchRenderer s_batch;
/// BatchRenderer
/// ----------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void init_defaults() {
  s_batch.white_texture                         = renderer_get_defaults().albedo_texture;
  s_batch.textures_cache[s_batch.white_texture] = 0;
  
  // Default shader init
  s_batch.shader = resources_get_shader(resources_push_shader(RESOURCE_CACHE_ID, generate_batch_quad_shader()));
}

static void init_pipeline() {
  // Vertex buffer init 
  GfxBufferDesc vert_desc = {
    .data  = nullptr,
    .size  = sizeof(Vertex2D) * MAX_VERTICES,
    .type  = GFX_BUFFER_VERTEX, 
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  s_batch.pipe_desc.vertex_buffer  = resources_get_buffer(resources_push_buffer(RESOURCE_CACHE_ID, vert_desc));
  s_batch.pipe_desc.vertices_count = 0;

  // Layout init
  s_batch.pipe_desc.layouts[0].attributes[0]    = GFX_LAYOUT_FLOAT2;
  s_batch.pipe_desc.layouts[0].attributes[1]    = GFX_LAYOUT_FLOAT4;
  s_batch.pipe_desc.layouts[0].attributes[2]    = GFX_LAYOUT_FLOAT2;
  s_batch.pipe_desc.layouts[0].attributes[3]    = GFX_LAYOUT_FLOAT2;
  s_batch.pipe_desc.layouts[0].attributes_count = 4;

  // Draw mode init 
  s_batch.pipe_desc.draw_mode = GFX_DRAW_MODE_TRIANGLE;

  // Pipeline init
  s_batch.pipeline = gfx_pipeline_create(s_batch.context, s_batch.pipe_desc);
}

static void generate_quad_batch(BatchCall* batch, const Rect2D& src, const Rect2D& dest, const Vec4& color, const Vec2& shape_side) {
  // Top-left
  Vertex2D v1 = {
    .position       = s_batch.ortho * Vec4(dest.position.x, dest.position.y, 0.0f, 1.0f),
    .color          = color,
    .texture_coords = src.position / src.size,
    .shape_side     = shape_side,
  };
  batch->vertices.push_back(v1);

  // Top-right
  Vertex2D v2 = {
    .position       = s_batch.ortho * Vec4(dest.position.x + dest.size.x, dest.position.y, 0.0f, 1.0f),
    .color          = color,
    .texture_coords = Vec2((src.position.x + src.size.x) / src.size.x, src.position.y / src.size.y),
    .shape_side     = shape_side,
  };
  batch->vertices.push_back(v2);

  // Bottom-right
  Vertex2D v3 = {
    .position       = s_batch.ortho * Vec4(dest.position.x + dest.size.x, dest.position.y + dest.size.y, 0.0f, 1.0f),
    .color          = color,
    .texture_coords = (src.position + src.size) / src.size,
    .shape_side     = shape_side,
  };
  batch->vertices.push_back(v3);
  batch->vertices.push_back(v3);

  // Bottom-left
  Vertex2D v4 = {
    .position       = s_batch.ortho * Vec4(dest.position.x, dest.position.y + dest.size.y, 0.0f, 1.0f),
    .color          = color,
    .texture_coords = Vec2(src.position.x / src.size.x, (src.position.y + src.size.y) / src.size.y),
    .shape_side     = shape_side,
  };
  batch->vertices.push_back(v4);
  batch->vertices.push_back(v1);

  // New texture added!
  batch->vertices_count += 6;
}

static void generate_quad_batch(BatchCall* batch, const Vec2& pos, const Vec2& size, const Vec4& color, const Vec2& shape_side) {
  Rect2D src = {
    .size     = size, 
    .position = Vec2(0.0f),
  };
  
  Rect2D dest = {
    .size     = size, 
    .position = pos,
  };

  generate_quad_batch(batch, src, dest, color, shape_side);
}

static void flush_batch(BatchCall& batch, GfxShader* shader) {
  // An empty batch is no use for us... 
  if(batch.vertices.size() == 0) {
    return;
  }

  // Use the resources
  
  GfxBindingDesc bind_desc = {
    .shader = shader,

    .textures       = &batch.texture, 
    .textures_count = 1,
  };
  gfx_context_use_bindings(s_batch.context, bind_desc);

  // Update the required resources
  
  gfx_buffer_upload_data(s_batch.pipe_desc.vertex_buffer, 
                         0, 
                         sizeof(Vertex2D) * batch.vertices.size(), 
                         batch.vertices.data());
  
  s_batch.pipe_desc.vertices_count = batch.vertices_count;
  gfx_pipeline_update(s_batch.pipeline, s_batch.pipe_desc); 

  // Render the batch
  
  gfx_context_use_pipeline(s_batch.context, s_batch.pipeline); 
  gfx_context_draw(s_batch.context, 0);

  // Reset back to normal
  
  batch.vertices_count = 0;
  batch.vertices.clear();
}

static void texture_lookup(GfxTexture* texture) {
  if(s_batch.textures_cache.find(texture) == s_batch.textures_cache.end()) {
    s_batch.textures_cache[texture] = s_batch.textures_cache.size();

    BatchCall new_batch = {
      .vertices_count = 0, 
      .texture        = texture,
    };
    s_batch.batches.push_back(new_batch);
  }
}

static BatchCall* prepare_texture_batch(GfxTexture* texture) {
  // Looks up the texture in the cache and pushes a 
  // new batch if the given `texture` is new
  texture_lookup(texture); 

  // Retrieve the texture index from the cache
  i32 index        = s_batch.textures_cache[texture];
  BatchCall* batch = &s_batch.batches[index]; 

  // We cannot render more than the maximum number of vertices
  if(batch->vertices_count >= MAX_VERTICES) {
    flush_batch(*batch, s_batch.shader);
  }

  return batch;
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

  // Defaults init
  init_defaults();

  // Default batch init
  s_batch.batches.reserve(32);
  BatchCall default_batch = {
    .vertices_count = 0, 
    .texture        = s_batch.white_texture, 
  };
  s_batch.batches.push_back(default_batch);
}

void batch_renderer_shutdown() {
  gfx_pipeline_destroy(s_batch.pipeline);
  
  s_batch.batches.clear();
}

void batch_renderer_begin() {
  // Get the size of the window 
  
  i32 width, height; 
  window_get_size(s_batch.ctx_desc.window, &width, &height);

  // Calculate the orthographic camera view
  s_batch.ortho = mat4_ortho(0.0f, (f32)width, (f32)height, 0.0f);
}

void batch_renderer_end() {
  NIKOLA_PROFILE_FUNCTION();

  // Render all of the batches 
  
  for(auto& batch : s_batch.batches) {
    flush_batch(batch, s_batch.shader);
  }
}

void batch_render_texture(GfxTexture* texture, const Rect2D& src, const Rect2D& dest, const Vec4& tint) {
  NIKOLA_ASSERT(texture, "Trying to render a NULL texture in \'batch_render_texture\'");
 
  // Prepare the texture batch
  BatchCall* batch = prepare_texture_batch(texture);

  // Generate vertices of a quad 
  generate_quad_batch(batch, src, dest, tint, Vec2(SHAPE_TYPE_QUAD, 4.0f));
}

void batch_render_texture(GfxTexture* texture, const Vec2& position, const Vec2& size, const Vec4& tint) {
  Rect2D src = {
    .size     = size, 
    .position = Vec2(0.0f),
  };

  Rect2D dest = {
    .size     = size, 
    .position = position, 
  };

  batch_render_texture(texture, src, dest, tint);
}


void batch_render_quad(const Vec2& position, const Vec2& size, const Vec4& color) {
  // Retrieve the texture index from the cache
  BatchCall* batch = &s_batch.batches[0]; 

  // We cannot render more than the maximum number of vertices
  if(batch->vertices_count >= MAX_VERTICES) {
    flush_batch(*batch, s_batch.shader);
  }
  
  // Generate vertices of a quad 
  generate_quad_batch(batch, position, size, color, Vec2(SHAPE_TYPE_QUAD, 4.0f));
}

void batch_render_circle(const Vec2& center, const f32 radius, const Vec4& color) {
  // Retrieve the texture index from the cache
  BatchCall* batch = &s_batch.batches[0]; 

  // We cannot render more than the maximum number of vertices
  if(batch->vertices_count >= MAX_VERTICES) {
    flush_batch(*batch, s_batch.shader);
  }
  
  // Generate vertices of a quad 
  generate_quad_batch(batch, center, Vec2(radius), color, Vec2(SHAPE_TYPE_CIRCLE, 0.0f));
}

void batch_render_polygon(const Vec2& center, const f32 radius, const u32 sides, const Vec4& color) {
  // Retrieve the texture index from the cache
  BatchCall* batch = &s_batch.batches[0]; 

  // We cannot render more than the maximum number of vertices
  if(batch->vertices_count >= MAX_VERTICES) {
    flush_batch(*batch, s_batch.shader);
  }
  
  // Generate vertices of a quad 
  generate_quad_batch(batch, center, Vec2(radius), color, Vec2(SHAPE_TYPE_POLYGON, (f32)sides));
}

void batch_render_text(Font* font, const String& text, const Vec2& position, const f32 size, const Vec4& color) {
  NIKOLA_ASSERT(font, "Trying to render text using a NULL font in \'batch_render_text\'");
  
  Vec2 off         = Vec2(0.0f);
  f32 scale        = size / 256.0f;
  f32 prev_advance = 0.0f;

  // Render each character of the text
  for(sizei i = 0; i < text.size(); i++) {
    // Retrieve the "correct" glyph from the font
    i8 ch       = text[i]; 
    Glyph glyph = font->glyphs[ch];

    // Using the information in the glyph, add a new line for the next glyph
    if(ch == '\n') {
      off.x = 0.0f;
      off.y += size + 2.0f;
      continue;
    }
    // Since a space is not really a "glyph", we just add an imaginary space 
    // between this glyph and the next one.
    else if(ch == ' ' || ch == '\t') {
      off.x += prev_advance * scale;
      continue;
    }
    
    // Render the codepoint/glyph
    batch_render_codepoint(font, ch, position + off, size, color); 

    // Advance a little for the next glyph
    off.x += glyph.advance_x * scale;

    // This is all for the next character. 
    // Specially useful for spaces (' ').
    prev_advance = glyph.advance_x;
  }
}

void batch_render_codepoint(Font* font, const char codepoint, const Vec2& position, const f32 font_size, const Vec4& color) {
  NIKOLA_ASSERT(font, "Trying to render text using a NULL font in \'batch_render_codepoint\'");
  
  f32 scale = font_size / 256.0f;

  // Retrieve the "correct" glyph from the font
  Glyph glyph = font->glyphs[codepoint];

  // Set up the soruce and destination rectangles
  
  Rect2D src = {
    .size     = glyph.size * scale,
    .position = Vec2(0.0f), 
  };
  Rect2D dest = {
    .size     = src.size,
    .position = position + (glyph.offset * scale), 
  };

  // Prepare and render the glyph batch
  
  BatchCall* batch = prepare_texture_batch(glyph.texture);
  generate_quad_batch(batch, src, dest, color, Vec2(SHAPE_TYPE_TEXT, 4.0f));
}

void batch_render_fps(Font* font, const Vec2& position, const f32 size, const Vec4& color) {
  String fps_text = "FPS: " + std::to_string((i32)niclock_get_fps());
  batch_render_text(font, fps_text, position, size, color);
}

/// Batch renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
