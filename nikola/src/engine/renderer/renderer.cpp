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

static void render_primitive(Mesh* mesh, Material* material, Transform& transform) {
  // Setting uniforms
  material->model_matrix = transform.transform; 

  // Setting up the pipeline
  mesh->pipe_desc.shader      = resources_get_shader(material->shader);
  mesh->pipe_desc.textures[0] = resources_get_texture(material->diffuse_map); 
  mesh->pipe_desc.textures[1] = resources_get_texture(material->specular_map); 

  // @NOTE: Even though the `material` might not have a specular or diffuse map, 
  // the resource manager will append a default texture in place of these 
  // to ensure the pipeline keeps moving without checking for `nullptr`s all the time.
  // Hence, there are _always_ textures available with each model.
  mesh->pipe_desc.textures_count = 2; 

  // Render the mesh
  gfx_context_apply_pipeline(s_renderer.context, mesh->pipe, mesh->pipe_desc);
  gfx_pipeline_draw_index(mesh->pipe);
}

static void render_mesh(RenderCommand& command) {
  Mesh* mesh         = resources_get_mesh(command.renderable_id);
  Material* material = resources_get_material(command.material_id);

  // Uploading the uniforms
  material_use(command.material_id);  
  
  render_primitive(mesh, material, command.transform);
}

static void render_skybox(RenderCommand& command) {
  Skybox* skybox     = resources_get_skybox(command.renderable_id); 
  Material* material = resources_get_material(command.material_id);

  // Uploading the uniforms
  material_use(command.material_id);  

  // Setting up the pipeline
  skybox->pipe_desc.shader      = resources_get_shader(material->shader);
  skybox->pipe_desc.cubemaps[0] = resources_get_cubemap(skybox->cubemap);

  // Render the skybox
  gfx_context_apply_pipeline(s_renderer.context, skybox->pipe, skybox->pipe_desc);
  gfx_pipeline_draw_vertex(skybox->pipe);
}

static void render_model(RenderCommand& command) {
  Model* model  = resources_get_model(command.renderable_id);
  Material* mat = resources_get_material(command.material_id);

  mat->model_matrix = command.transform.transform;

  for(sizei i = 0; i < model->meshes.size(); i++) {
    Mesh* mesh              = resources_get_mesh(model->meshes[i]);
    Material* mesh_material = resources_get_material(model->materials[model->material_indices[i]]); 

    // Setting textures
    mat->diffuse_map  = mesh_material->diffuse_map;
    mat->specular_map = mesh_material->specular_map;

    // Setting uniforms
    //
    // @NOTE: Each material has its own valid colors and model. However, we also 
    // want OUR own materials to influence the model. So, we _apply_ our model matrix 
    // and colors to the material.
    // mesh_material->shader            = mat->shader; 
    // mesh_material->ambient_color     = mat->ambient_color; 
    // mesh_material->diffuse_color     = mat->diffuse_color; 
    // mesh_material->specular_color    = mat->specular_color; 

    // Uploading the uniforms
    material_use(command.material_id);  

    // Render the model's primitive
    render_primitive(mesh, mat, command.transform);
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

void render_pass_create(RenderPass* pass, const Vec2& size, const ResourceID& material_id) {
  pass->material   = material_id;
  pass->frame_size = (Vec2)size;
  
  pass->frame_desc = {}; 

  // Clear color init
  pass->frame_desc.clear_color[0] = s_renderer.clear_color.r;
  pass->frame_desc.clear_color[1] = s_renderer.clear_color.g;
  pass->frame_desc.clear_color[2] = s_renderer.clear_color.b;
  pass->frame_desc.clear_color[3] = s_renderer.clear_color.a;

  // Clear flags init
  pass->frame_desc.clear_flags = GFX_CLEAR_FLAGS_COLOR_BUFFER | GFX_CLEAR_FLAGS_DEPTH_BUFFER;

  // Render target init
  GfxTextureDesc texture_desc = {
    .width     = (u32)size.x, 
    .height    = (u32)size.y, 
    .depth     = 0, 
    .mips      = 1, 
    .type      = GFX_TEXTURE_RENDER_TARGET,
    .format    = GFX_TEXTURE_FORMAT_RGBA8, 
    .filter    = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
    .wrap_mode = GFX_TEXTURE_WRAP_MIRROR, 
    .data      = nullptr,
  };
  pass->frame_desc.attachments[0] = gfx_texture_create(s_renderer.context, texture_desc);
  pass->frame_desc.attachments_count++;

  // Depth-Stencil texture init
  texture_desc = {
    .width     = (u32)size.x, 
    .height    = (u32)size.y, 
    .depth     = 0, 
    .mips      = 1, 
    .type      = GFX_TEXTURE_DEPTH_STENCIL_TARGET,
    .format    = GFX_TEXTURE_FORMAT_DEPTH_STENCIL_24_8, 
    .filter    = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
    .wrap_mode = GFX_TEXTURE_WRAP_CLAMP, 
    .data      = nullptr,
  };
  pass->frame_desc.attachments[1] = gfx_texture_create(s_renderer.context, texture_desc);
  pass->frame_desc.attachments_count++;
  
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

void render_pass_end(RenderPass& pass) {
  // Render to the default framebuffer
  gfx_context_clear(s_renderer.context, nullptr);
  gfx_context_set_state(s_renderer.context, GFX_STATE_DEPTH, false); 

  // Apply the shader from the pass
  s_renderer.pipe_desc.shader = resources_get_shader(resources_get_material(pass.material)->shader);

  // Apply the textures from the pass
  for(sizei i = 0; i < pass.frame_desc.attachments_count; i++) {
    s_renderer.pipe_desc.textures[i] = pass.frame_desc.attachments[i];
  }
  s_renderer.pipe_desc.textures_count = pass.frame_desc.attachments_count;

  // Render the final render target
  gfx_context_apply_pipeline(s_renderer.context, s_renderer.pipeline, s_renderer.pipe_desc);
  gfx_pipeline_draw_index(s_renderer.pipeline);
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

/// Renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
