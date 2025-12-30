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

const sizei MAX_DRAW_CALLS          = 1024;
const sizei MATERIAL2D_BUFFER_INDEX = 5;

/// Consts
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ShapeType 
enum ShapeType {
  SHAPE_TYPE_QUAD = 0, 
  SHAPE_TYPE_CIRCLE,
  SHAPE_TYPE_POLYGON,
  SHAPE_TYPE_TEXT,
};

/// ShapeType 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Vertex2D 
struct Vertex2D {
  Vec2 position; 
  Vec2 texture_coords; 
  Vec4 color; 
  f32 material_index;
};
/// Vertex2D 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Material2D
struct Material2D {
  Vec2 size; 
  Vec2 __padding0;

  f32 radius       = 1.0f; 
  f32 sides_count  = 4.0f;
  f32 shape_type = 0.0f; 
  
  f32 __padding1;
};
/// Material2D
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Batch
struct Batch {
  DynamicArray<Vertex2D> vertices; 
  DynamicArray<Material2D> materials;
};
/// Batch
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// BatchRenderer
struct BatchRenderer {
  GfxContext* context     = nullptr;
  GfxContextDesc ctx_desc = {}; 

  ShaderContext* batch_shader;

  GfxPipelineDesc pipe_desc   = {};
  GfxPipeline* pipeline       = nullptr; 
  GfxBuffer* materials_buffer = nullptr;

  DynamicArray<GfxTexture*> textures;
  HashMap<GfxTexture*, Batch> textures_table;
  
  Mat4 ortho = Mat4(1.0f);
};

static BatchRenderer s_batch;
/// BatchRenderer
/// ----------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void init_defaults() {
  // Vertex buffer init 
  
  GfxBufferDesc buff_desc = {
    .data  = nullptr,
    .size  = sizeof(Vertex2D) * (MAX_DRAW_CALLS * 6),
    .type  = GFX_BUFFER_VERTEX, 
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };

  s_batch.pipe_desc.vertex_buffer  = resources_get_buffer(resources_push_buffer(RESOURCE_CACHE_ID, buff_desc));
  s_batch.pipe_desc.vertices_count = 0;

  // Layout init
  
  s_batch.pipe_desc.layouts[0].attributes[0]    = GFX_LAYOUT_FLOAT2;
  s_batch.pipe_desc.layouts[0].attributes[1]    = GFX_LAYOUT_FLOAT2;
  s_batch.pipe_desc.layouts[0].attributes[2]    = GFX_LAYOUT_FLOAT4;
  s_batch.pipe_desc.layouts[0].attributes[3]    = GFX_LAYOUT_FLOAT1;
  s_batch.pipe_desc.layouts[0].attributes_count = 4;

  // Draw mode init 
  s_batch.pipe_desc.draw_mode = GFX_DRAW_MODE_TRIANGLE;

  // Pipeline init
  s_batch.pipeline = gfx_pipeline_create(s_batch.context, s_batch.pipe_desc);

  // Materials buffer init

  buff_desc = {
    .data  = nullptr,
    .size  = sizeof(Material2D) * MAX_DRAW_CALLS,
    .type  = GFX_BUFFER_SHADER_STORAGE, 
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  s_batch.materials_buffer = resources_get_buffer(resources_push_buffer(RESOURCE_CACHE_ID, buff_desc));
  
  // Default texture init

  GfxTexture* default_texture = renderer_get_defaults().albedo_texture;

  s_batch.textures.push_back(default_texture);
  s_batch.textures_table[default_texture] = Batch{};
  
  // Batch shader init
  
  ResourceID batch_shader      = resources_push_shader(RESOURCE_CACHE_ID, generate_batch_quad_shader());
  ResourceID shader_context_id = resources_push_shader_context(RESOURCE_CACHE_ID, batch_shader);

  s_batch.batch_shader = resources_get_shader_context(shader_context_id);

  // Attach the material buffer
  gfx_buffer_bind_point(s_batch.materials_buffer, MATERIAL2D_BUFFER_INDEX);
}

static void generate_quad_batch(Batch* batch, const Rect2D& src, const Rect2D& dest, const Vec4& color, const Material2D& material) {
  // Top-left
 
  Vertex2D v1 = {
    .position       = dest.position,
    .texture_coords = src.position / src.size,
    .color          = color,
    .material_index = (f32)batch->materials.size(),
  };
  batch->vertices.push_back(v1);

  // Top-right
  
  Vertex2D v2 = {
    .position       = Vec2(dest.position.x + dest.size.x, dest.position.y),
    .texture_coords = Vec2((src.position.x + src.size.x) / src.size.x, src.position.y / src.size.y),
    .color          = color,
    .material_index = (f32)batch->materials.size(),
  };
  batch->vertices.push_back(v2);

  // Bottom-right
  
  Vertex2D v3 = {
    .position       = dest.position + dest.size,
    .texture_coords = (src.position + src.size) / src.size,
    .color          = color,
    .material_index = (f32)batch->materials.size(),
  };
  batch->vertices.push_back(v3);
  batch->vertices.push_back(v3);

  // Bottom-left
  
  Vertex2D v4 = {
    .position       = Vec2(dest.position.x, dest.position.y + dest.size.y),
    .texture_coords = Vec2(src.position.x / src.size.x, (src.position.y + src.size.y) / src.size.y),
    .color          = color,
    .material_index = (f32)batch->materials.size(),
  };
  batch->vertices.push_back(v4);
  batch->vertices.push_back(v1);

  // New material added!
  batch->materials.push_back(material);
}

static void generate_quad_batch(Batch* batch, const Vec2& pos, const Vec2& size, const Vec4& color, const Material2D& material) {
  Rect2D src = {
    .size     = size, 
    .position = Vec2(0.0f),
  };
  
  Rect2D dest = {
    .size     = size, 
    .position = pos,
  };

  generate_quad_batch(batch, src, dest, color, material);
}

static void flush_batch(GfxTexture* texture) {
  Batch& batch = s_batch.textures_table[texture];

  // Update the vertex buffer
  
  gfx_buffer_upload_data(s_batch.pipe_desc.vertex_buffer, 
                         0, 
                         sizeof(Vertex2D) * batch.vertices.size(), 
                         batch.vertices.data());
  
  s_batch.pipe_desc.vertices_count = batch.vertices.size();
  gfx_pipeline_update(s_batch.pipeline, s_batch.pipe_desc); 

  // Update the materials buffer
  
  gfx_buffer_upload_data(s_batch.materials_buffer, 
                         0, 
                         sizeof(Material2D) * batch.materials.size(), 
                         batch.materials.data());

  // Use the resources
  
  GfxBindingDesc bind_desc = {
    .shader = s_batch.batch_shader->shader,

    .textures       = &texture, 
    .textures_count = 1,
  };
  gfx_context_use_bindings(s_batch.context, bind_desc);

  // Render the batch
  
  gfx_context_use_pipeline(s_batch.context, s_batch.pipeline); 
  gfx_context_draw(s_batch.context, 0);

  // Reset back to normal
  
  batch.vertices.clear();
  batch.materials.clear();
}

static Batch* prepare_texture_batch(GfxTexture* texture) {
  // Looks up the texture in the cache and pushes a 
  // new batch if the given `texture` is new
  
  if(s_batch.textures_table.find(texture) == s_batch.textures_table.end()) {
    s_batch.textures.push_back(texture);
    s_batch.textures_table[texture] = Batch{};
  }

  // Retrieve the batch from the texture table
  Batch* batch = &s_batch.textures_table[texture]; 

  // We cannot render more than the maximum number of calls
  
  if(batch->materials.size() >= MAX_DRAW_CALLS) {
    flush_batch(texture);
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

  // Defaults init
  init_defaults();
}

void batch_renderer_shutdown() {
  gfx_pipeline_destroy(s_batch.pipeline);
  
  s_batch.textures.clear();
  s_batch.textures_table.clear();
}

void batch_renderer_begin() {
  // Get the size of the window 
  
  i32 width, height; 
  window_get_size(s_batch.ctx_desc.window, &width, &height);

  // Calculate the orthographic camera view and send it to the shader
  
  s_batch.ortho = mat4_ortho(0.0f, (f32)width, (f32)height, 0.0f);
  shader_context_set_uniform(s_batch.batch_shader, "u_ortho", s_batch.ortho);
}

void batch_renderer_end() {
  NIKOLA_PROFILE_FUNCTION();

  // Render all of the batches 
  
  for(auto& texture : s_batch.textures) {
    flush_batch(texture);
  }

  // Reset the textures state

  s_batch.textures.clear();
  s_batch.textures_table.clear();
  
  GfxTexture* default_texture = renderer_get_defaults().albedo_texture;
  s_batch.textures.push_back(default_texture);

  s_batch.textures_table[default_texture] = Batch{};
}

void batch_render_texture(GfxTexture* texture, const Rect2D& src, const Rect2D& dest, const Vec4& tint) {
  NIKOLA_ASSERT(texture, "Trying to render a NULL texture in batch_render_texture");
 
  // Prepare the texture batch
  Batch* batch = prepare_texture_batch(texture);

  // Generate vertices of a quad 
  
  Material2D material = {
    .size       = dest.size, 
    .shape_type = (f32)SHAPE_TYPE_QUAD, 
  };
  generate_quad_batch(batch, src, dest, tint, material);
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
  // Prepare the texture batch
  Batch* batch = prepare_texture_batch(s_batch.textures[0]);
  
  // Generate vertices of a quad 
  
  Material2D material = {
    .size       = size, 
    .shape_type = (f32)SHAPE_TYPE_QUAD, 
  };
  generate_quad_batch(batch, position, size, color, material);
}

void batch_render_circle(const Vec2& center, const f32 radius, const Vec4& color) {
  // Prepare the texture batch
  Batch* batch = prepare_texture_batch(s_batch.textures[0]);
  
  // Generate vertices of a quad 
  
  Material2D material = {
    .size       = Vec2(radius), 
    .radius     = radius,
    .shape_type = (f32)SHAPE_TYPE_CIRCLE, 
  };
  generate_quad_batch(batch, center, Vec2(radius), color, material);
}

void batch_render_polygon(const Vec2& center, const f32 radius, const u32 sides, const Vec4& color) {
  // Prepare the texture batch
  Batch* batch = prepare_texture_batch(s_batch.textures[0]);
  
  // Generate vertices of a quad 
  
  Material2D material = {
    .size        = Vec2(radius), 
    .radius      = radius,
    .sides_count = (f32)sides,
    .shape_type  = (f32)SHAPE_TYPE_POLYGON, 
  };
  generate_quad_batch(batch, center, Vec2(radius), color, material);
}

void batch_render_text(Font* font, const String& text, const Vec2& position, const f32 size, const Vec4& color) {
  NIKOLA_ASSERT(font, "Trying to render text using a NULL font in batch_render_text");
  
  f32 scale = size / NBR_FONT_IMPORT_SCALE;
  Vec2 off  = Vec2(0.0f);

  // Render each character of the text
  for(sizei i = 0; i < text.size(); i++) {
    // Retrieve the "correct" glyph from the font
    
    i8 ch              = text[i]; 
    Font::Glyph& glyph = font->glyphs[ch];

    // Using the information in the glyph, add a new line for the next glyph
    
    if(ch == '\n') {
      off.x  = 0.0f;
      off.y += (font->ascent - font->descent + font->line_gap) * scale;

      continue;
    }

    // Since a space is not really a "glyph", we just add an imaginary space 
    // between this glyph and the next one.
    
    if(ch == ' ' || ch == '\t') {
      off.x += (size * scale) * 2;
      continue;
    }
    
    // Render the codepoint/glyph
    batch_render_codepoint(font, ch, position + off, size, color); 

    // Advance a little for the next glyph
    off.x += glyph.advance_x * scale;
  }
}

void batch_render_codepoint(Font* font, const char codepoint, const Vec2& position, const f32 font_size, const Vec4& color) {
  NIKOLA_ASSERT(font, "Trying to render text using a NULL font in batch_render_codepoint");
  
  f32 scale = font_size / NBR_FONT_IMPORT_SCALE;

  // Retrieve the "correct" glyph from the font
  Font::Glyph& glyph = font->glyphs[codepoint];

  // Set up the soruce and destination rectangles

  Vec2 dest_pos;
  dest_pos.x = position.x + ((glyph.left_bearing + glyph.offset.x) * scale);
  dest_pos.y = position.y + ((glyph.offset.y) * scale);

  Rect2D src = {
    .size     = glyph.size * scale,
    .position = Vec2(0.0f), 
  };
  Rect2D dest = {
    .size     = src.size,
    .position = dest_pos,
  };

  // Prepare the texture batch
  Batch* batch = prepare_texture_batch(glyph.texture);
  
  // Generate vertices of a quad 
  
  Material2D material = {
    .size       = Vec2(font_size), 
    .shape_type = (f32)SHAPE_TYPE_TEXT, 
  };
  generate_quad_batch(batch, src, dest, color, material);
}

void batch_render_fps(Font* font, const Vec2& position, const f32 size, const Vec4& color) {
  String fps_text = "FPS: " + std::to_string((i32)niclock_get_fps());
  batch_render_text(font, fps_text, position, size, color);
}

/// Batch renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
