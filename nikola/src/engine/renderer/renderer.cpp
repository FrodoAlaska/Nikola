#include "nikola/nikola_render.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_gfx.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Renderer
struct Renderer {
  GfxContext* context = nullptr;

  Vec4 clear_color;
  Camera* camera;
  
  GfxPipelineDesc pipe_desc  = {};
  GfxPipeline* pipeline      = nullptr; 

  RendererDefaults defaults = {};
};

static Renderer s_renderer;
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
  Mesh* mesh         = resources_get_mesh(command.renderable_id);
  Material* material = resources_get_material(command.material_id);

  // Setting preset uniforms
  material->model_matrix = command.transform.transform;

  // Uploading the uniforms
  material_use(command.material_id);  

  // Draw the mesh
  gfx_pipeline_draw_index(mesh->pipe);
}

static void render_skybox(RenderCommand& command) {
  Skybox* skybox     = resources_get_skybox(command.renderable_id); 
  Material* material = resources_get_material(command.material_id);

  // Uploading the uniforms
  material_use(command.material_id);  

  // Use the cubemap
  GfxCubemap* cube = resources_get_cubemap(skybox->cubemap);
  gfx_cubemap_use(&cube, 1);

  // Draw the skybox
  gfx_pipeline_draw_vertex(skybox->pipe);
}

static void render_model(RenderCommand& command) {
  Model* model  = resources_get_model(command.renderable_id);
  Material* mat = resources_get_material(command.material_id);

  for(sizei i = 0; i < model->meshes.size(); i++) {
    // For better visuals (and performance?)
    Mesh* mesh              = resources_get_mesh(model->meshes[i]);
    ResourceID mat_id       = model->materials[model->material_indices[i]];
    Material* mesh_material = resources_get_material(mat_id); 

    // Setting preset uniforms
    mesh_material->model_matrix = command.transform.transform;

    // Setting the shader
    material_set_shader(mat_id, mat->shader);
    material_set_uniform(mat_id, "u_model", command.transform.transform); // @TODO(Renderer): Perhaps there is a better way to set the transform of a model?

    // Uploading the uniforms
    material_use(mat_id);  

    // Draw the material's mesh
    gfx_pipeline_draw_index(mesh->pipe);
  }
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

  queue.clear();
}

void render_queue_push(RenderQueue& queue, const RenderCommand& cmd) {
  queue.push_back(cmd);
}

/// RenderQueue functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderPass functions

void render_pass_create(RenderPass* pass, const Vec2& size, u32 clear_flags) {
  pass->frame_size = (Vec2)size;
  
  pass->frame_desc = {}; 

  // Clear color init
  pass->frame_desc.clear_color[0] = s_renderer.clear_color.r;
  pass->frame_desc.clear_color[1] = s_renderer.clear_color.g;
  pass->frame_desc.clear_color[2] = s_renderer.clear_color.b;
  pass->frame_desc.clear_color[3] = s_renderer.clear_color.a;

  // Clear flags init
  pass->frame_desc.clear_flags = clear_flags;
  
  // Framebuffer init
  pass->frame = gfx_framebuffer_create(s_renderer.context, pass->frame_desc);
}

void render_pass_destroy(RenderPass& pass) {
  gfx_framebuffer_destroy(pass.frame);
}

void render_pass_begin(RenderPass& pass) {
  // @NOTE: An annoying way to set the clear color 
  Vec4 col = Vec4(pass.frame_desc.clear_color[0], 
                  pass.frame_desc.clear_color[1], 
                  pass.frame_desc.clear_color[2], 
                  pass.frame_desc.clear_color[3]);
  
  // Clear the framebuffer
  gfx_context_clear(s_renderer.context, pass.frame);
  gfx_context_set_state(s_renderer.context, GFX_STATE_DEPTH, true); 
}

void render_pass_end(RenderPass& pass, const ResourceID& material_id) {
  NIKOLA_ASSERT((material_id.group != RESOURCE_GROUP_INVALID), "Invalid Material passed to render_pass_end function");

  // Apply the textures from the pass
  gfx_texture_use(pass.frame_desc.attachments, pass.frame_desc.attachments_count); 

  // Apply the shader from the pass
  gfx_shader_use(resources_get_shader(resources_get_material(material_id)->shader));
}

void render_pass_push_target(RenderPass& pass, const GfxTextureType type, const GfxTextureFormat format) {
  GfxTextureDesc texture_desc = {
    .width     = (u32)pass.frame_size.x, 
    .height    = (u32)pass.frame_size.y, 
    .depth     = 0, 
    .mips      = 1, 
    .type      = type,
    .format    = format, 
    .filter    = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
    .wrap_mode = GFX_TEXTURE_WRAP_MIRROR, 
    .data      = nullptr,
  };

  pass.frame_desc.attachments[pass.frame_desc.attachments_count] = gfx_texture_create(s_renderer.context, texture_desc);
  pass.frame_desc.attachments_count++;

  gfx_framebuffer_update(pass.frame, pass.frame_desc);
}

/// RenderPass functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Renderer functions

void renderer_init(Window* window, const Vec4& clear_clear) {
  // Context init 
  init_context(window);
  s_renderer.clear_color = clear_clear;

  // Defaults init
  init_defaults();

  // Pipeline init
  init_pipeline();

  NIKOLA_LOG_INFO("Successfully initialized the renderer context");
}

void renderer_shutdown() {
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

void renderer_begin(Camera& camera) {
  // Updating the internal matrices buffer for each shader
  s_renderer.camera = &camera;
  gfx_buffer_update(s_renderer.defaults.matrices_buffer, 0, sizeof(Mat4), mat4_raw_data(camera.view));
  gfx_buffer_update(s_renderer.defaults.matrices_buffer, sizeof(Mat4), sizeof(Mat4), mat4_raw_data(camera.projection));
}

void renderer_end() {
  gfx_context_present(s_renderer.context);
}

void renderer_apply_pass(RenderPass& pass) {
  // Render to the default framebuffer
  gfx_context_clear(s_renderer.context, nullptr);
  gfx_context_set_state(s_renderer.context, GFX_STATE_DEPTH, false); 

  // Render the final render target
  gfx_pipeline_update(s_renderer.pipeline, s_renderer.pipe_desc);
  gfx_pipeline_draw_index(s_renderer.pipeline);
}

/// Renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
