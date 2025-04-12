#include "nikola/nikola_render.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_gfx.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
struct RenderPassEntry {
  RenderPass pass; 
  RenderPassFn func; 
  void* user_data  = nullptr;
};
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Renderer
struct Renderer {
  GfxContext* context = nullptr;

  Vec4 clear_color;
  Camera* camera;
  
  GfxPipelineDesc pipe_desc  = {};
  GfxPipeline* pipeline      = nullptr; 

  RendererDefaults defaults = {};
  DynamicArray<RenderPassEntry> render_passes;
};

static Renderer s_renderer{};
/// Renderer
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static void init_context(Window* window) { 
  GfxContextDesc gfx_desc = {
    .window       = window,
    .states       = GFX_STATE_DEPTH | GFX_STATE_STENCIL,
  };
  
  s_renderer.context = gfx_context_init(gfx_desc);
  NIKOLA_ASSERT(s_renderer.context, "Failed to initialize the graphics context");
}

static void init_defaults() {
  // Default texture init
  u32 pixels = 0x00000000; 
  GfxTextureDesc texture_desc = {
    .width     = 1, 
    .height    = 1, 
    .depth     = 0, 
    .mips      = 1, 
    .type      = GFX_TEXTURE_2D,
    .format    = GFX_TEXTURE_FORMAT_RGBA8, 
    .filter    = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
    .wrap_mode = GFX_TEXTURE_WRAP_MIRROR, 
    .data      = &pixels,
  };
  s_renderer.defaults.texture = gfx_texture_create(s_renderer.context, texture_desc);

  // Matrices buffer init
  GfxBufferDesc buff_desc = {
    .data  = nullptr, 
    .size  = sizeof(Mat4) * 2,
    .type  = GFX_BUFFER_UNIFORM,
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  s_renderer.defaults.matrices_buffer = gfx_buffer_create(s_renderer.context, buff_desc);
}

static void init_pipeline() {
  f32 vertices[] = {
    // Position    // Texture coords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
  };

  // Vertex buffer init 
  GfxBufferDesc vert_desc = {
    .data  = vertices,
    .size  = sizeof(vertices),
    .type  = GFX_BUFFER_VERTEX, 
    .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  s_renderer.pipe_desc.vertex_buffer  = gfx_buffer_create(s_renderer.context, vert_desc);
  s_renderer.pipe_desc.vertices_count = 4;
 
  // Index buffer init
  u32 indices[] = {
    0, 1, 2, 
    2, 3, 0,
  };
  GfxBufferDesc index_desc = {
    .data  = indices,
    .size  = sizeof(indices),
    .type  = GFX_BUFFER_INDEX, 
    .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  s_renderer.pipe_desc.index_buffer  = gfx_buffer_create(s_renderer.context, index_desc);
  s_renderer.pipe_desc.indices_count = 6;

  // Layout init
  s_renderer.pipe_desc.layout[0]     = GfxLayoutDesc{"POS", GFX_LAYOUT_FLOAT2, 0};
  s_renderer.pipe_desc.layout[1]     = GfxLayoutDesc{"TEX", GFX_LAYOUT_FLOAT2, 0};
  s_renderer.pipe_desc.layout_count  = 2;

  // Draw mode init 
  s_renderer.pipe_desc.draw_mode = GFX_DRAW_MODE_TRIANGLE;

  // Pipeline init
  s_renderer.pipeline = gfx_pipeline_create(s_renderer.context, s_renderer.pipe_desc);
}

static void render_mesh(RenderCommand& command) {
  Mesh* mesh = resources_get_mesh(command.renderable_id);

  // Setting uniforms 
  shader_context_set_uniform(command.shader_context_id, MATERIAL_UNIFORM_MODEL_MATRIX, command.transform->transform);

  // Using the shader 
  shader_context_use(command.shader_context_id);

  // Using the textures
  material_use(command.material_id);  

  // Draw the mesh
  gfx_pipeline_draw_index(mesh->pipe);
}

static void render_skybox(RenderCommand& command) {
  Skybox* skybox = resources_get_skybox(command.renderable_id); 

  // Using the shader 
  shader_context_use(command.shader_context_id);

  // Use the cubemap
  GfxCubemap* cube = resources_get_cubemap(skybox->cubemap);
  gfx_cubemap_use(&cube, 1);

  // Draw the skybox
  gfx_pipeline_draw_vertex(skybox->pipe);
}

static void render_model(RenderCommand& command) {
  Model* model = resources_get_model(command.renderable_id);

  for(sizei i = 0; i < model->meshes.size(); i++) {
    // For better visuals (and performance?)
    ResourceID mesh_id = model->meshes[i];
    ResourceID mat_id  = model->materials[model->material_indices[i]];
    
    // Build the sub-command for the mesh
    RenderCommand sub_cmd = {
     .transform         = command.transform,  
     .render_type       = nikola::RENDERABLE_TYPE_MESH, 
     .renderable_id     = mesh_id, 
     .material_id       = mat_id, 
     .shader_context_id = command.shader_context_id, 
    };

    // Render the sub-command
    render_mesh(sub_cmd);
  }
}

static void create_render_pass(RenderPass* pass, const RenderPassDesc& desc) {
  pass->frame_size        = desc.frame_size;
  pass->frame_desc        = {}; 
  pass->shader_context_id = desc.shader_context_id;

  // Clear color init
  pass->clear_color = desc.clear_color;

  // Clear flags init
  pass->frame_desc.clear_flags = desc.clear_flags;
 
  // Attachments init
  for(sizei i = 0; i < desc.targets.size(); i++) {
    GfxTextureDesc texture_desc = {
      .width     = (u32)pass->frame_size.x, 
      .height    = (u32)pass->frame_size.y, 
      .depth     = 0, 
      .mips      = 1, 
      .type      = GFX_TEXTURE_RENDER_TARGET,
      .format    = desc.targets[i],
      .filter    = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
      .wrap_mode = GFX_TEXTURE_WRAP_MIRROR, 
      .data      = nullptr,
    };
    pass->frame_desc.attachments[i] = gfx_texture_create(s_renderer.context, texture_desc);
    pass->frame_desc.attachments_count++;
  }

  // Every framebuffer will have a depth and stencil buffer by default no matter what 
  GfxTextureDesc texture_desc = {
    .width     = (u32)pass->frame_size.x, 
    .height    = (u32)pass->frame_size.y, 
    .depth     = 0, 
    .mips      = 1, 
    .type      = GFX_TEXTURE_DEPTH_STENCIL_TARGET,
    .format    = GFX_TEXTURE_FORMAT_DEPTH_STENCIL_24_8,
    .filter    = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
    .wrap_mode = GFX_TEXTURE_WRAP_MIRROR, 
    .data      = nullptr,
  };
  pass->frame_desc.attachments[desc.targets.size()] = gfx_texture_create(s_renderer.context, texture_desc);
  pass->frame_desc.attachments_count++;

  // Framebuffer init
  pass->frame = gfx_framebuffer_create(s_renderer.context, pass->frame_desc);
}

static void begin_pass(RenderPass& pass) {
  NIKOLA_ASSERT(RESOURCE_IS_VALID(pass.shader_context_id), "Invalid ShaderContext passed to the begin pass function");
  
  // Set the pass's target
  gfx_context_set_target(s_renderer.context, pass.frame);
  
  // Clear the framebuffer
  Vec4 col = pass.clear_color;
  gfx_context_clear(s_renderer.context, col.r, col.g, col.b, col.a);
  gfx_context_set_state(s_renderer.context, GFX_STATE_DEPTH, true); 
}

static void end_pass(RenderPass& pass) {
  NIKOLA_ASSERT(RESOURCE_IS_VALID(pass.shader_context_id), "Invalid ShaderContext passed to the end pass function");

  // Apply the shader from the pass
  shader_context_use(pass.shader_context_id);
  
  // Apply the textures from the pass
  gfx_texture_use(pass.frame_desc.attachments, pass.frame_desc.attachments_count); 
  
  // Render to the default framebuffer
  gfx_context_set_target(s_renderer.context, nullptr);
  
  // Clear the default target
  Vec4 col = s_renderer.clear_color;
  gfx_context_clear(s_renderer.context, col.r, col.g, col.b, col.a);
  gfx_context_set_state(s_renderer.context, GFX_STATE_DEPTH, false); 

  // Render the final render target
  gfx_pipeline_update(s_renderer.pipeline, s_renderer.pipe_desc);
  gfx_pipeline_draw_index(s_renderer.pipeline);
}


/// Private functions
/// ----------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderQueue functions

void render_queue_flush(RenderQueue& queue) {
  for(auto& command : queue) {
    switch(command.render_type) {
      case RENDERABLE_TYPE_MESH:
        render_mesh(command);
        break;
      case RENDERABLE_TYPE_MODEL:
        render_model(command);
        break;
      case RENDERABLE_TYPE_SKYBOX:
        render_skybox(command);
        break;
    }
  }
}

void render_queue_push(RenderQueue& queue, const RenderCommand& cmd) {
  queue.push_back(cmd);
}

/// RenderQueue functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Renderer functions

void renderer_init(Window* window, const Vec4& clear_color) {
  // Context init 
  init_context(window);
  s_renderer.clear_color = clear_color;

  // Defaults init
  init_defaults();

  // Pipeline init
  init_pipeline();

  NIKOLA_LOG_INFO("Successfully initialized the renderer context");
}

void renderer_shutdown() {
  for(auto& entry : s_renderer.render_passes) {
    gfx_framebuffer_destroy(entry.pass.frame);
  }

  gfx_pipeline_destroy(s_renderer.pipeline);
  gfx_context_shutdown(s_renderer.context);
  
  NIKOLA_LOG_INFO("Successfully shutdown the renderer context");
}

const GfxContext* renderer_get_context() {
  return s_renderer.context;
}

void renderer_set_clear_color(const Vec4& clear_color) {
  s_renderer.clear_color = clear_color;
}

const RendererDefaults& renderer_get_defaults() {
  return s_renderer.defaults;
}

void renderer_push_pass(const RenderPassDesc& desc, const RenderPassFn& func, const void* user_data) {
  RenderPassEntry entry;
  entry.func      = func; 
  entry.user_data = (void*)user_data;
  create_render_pass(&entry.pass, desc);

  s_renderer.render_passes.push_back(entry);
}

void renderer_begin(Camera& camera) {
  // Updating the internal matrices buffer for each shader
  s_renderer.camera = &camera;
  gfx_buffer_update(s_renderer.defaults.matrices_buffer, 0, sizeof(Mat4), mat4_raw_data(camera.view));
  gfx_buffer_update(s_renderer.defaults.matrices_buffer, sizeof(Mat4), sizeof(Mat4), mat4_raw_data(camera.projection));
}

void renderer_apply_passes() {
  /* @NOTE (28/3/2025, Mohamed):
  *
  * Since the first entry of the render passes does not have a "previous" 
  * entry, we use it here before entering the loop. We also want to avoid 
  * the inevitable `if` statements inside the loop.
  * 
  */
  RenderPassEntry* first_entry = &s_renderer.render_passes[0];
  
  begin_pass(first_entry->pass);
  first_entry->func(nullptr, &first_entry->pass, first_entry->user_data);
  end_pass(first_entry->pass);

  // Initiate all of the render passes 
  for(sizei i = 1; i < s_renderer.render_passes.size(); i++) {
    RenderPassEntry* entry = &s_renderer.render_passes[i];
  
    begin_pass(entry->pass);
    entry->func(&s_renderer.render_passes[i - 1].pass, &entry->pass, entry->user_data);
    end_pass(entry->pass);
  } 
}

void renderer_end() {
  gfx_context_present(s_renderer.context);
}

/// Renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
