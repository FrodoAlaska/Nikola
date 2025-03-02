#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Renderer
struct Renderer {
  GfxContext* context = nullptr;
  GfxBuffer* matrices_buffer;

  Vec4 clear_color;
  Camera camera;

  u32 clear_flags = 0;

  DynamicArray<RenderCommand> render_queue;
};

static Renderer s_renderer;
/// Renderer
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static void render_mesh(const RenderCommand& command) {
  Mesh* mesh         = resource_storage_get_mesh(command.storage, command.renderable_id);
  Material* material = resource_storage_get_material(command.storage, command.material_id);

  // Setting uniforms
  material->model_matrix = command.transform.transform; 

  // Uploading the uniforms
  material_use(material);  

  // Setting up the pipeline
  mesh->pipe_desc.shader         = material->shader;
  mesh->pipe_desc.textures[0]    = material->diffuse_map;
  mesh->pipe_desc.textures_count = material->diffuse_map ? 1 : 0; // Only set a texutre if there's one in the material

  // Render the mesh
  gfx_context_apply_pipeline(s_renderer.context, mesh->pipe, mesh->pipe_desc);
  gfx_pipeline_draw_index(mesh->pipe);
}

static void render_skybox(const RenderCommand& command) {
  Skybox* skybox     = resource_storage_get_skybox(command.storage, command.renderable_id); 
  Material* material = resource_storage_get_material(command.storage, command.material_id);

  // Setting up the pipeline
  skybox->pipe_desc.shader = material->shader;

  // Render the skybox
  gfx_context_apply_pipeline(s_renderer.context, skybox->pipe, skybox->pipe_desc);
  gfx_pipeline_draw_vertex(skybox->pipe);
}

static void render_model(const RenderCommand& command) {
  Model* model  = resource_storage_get_model(command.storage, command.renderable_id);
  Material* mat = resource_storage_get_material(command.storage, command.material_id);

  // Set our "parent" transform
  mat->model_matrix = command.transform.transform; 

  for(sizei i = 0; i < model->meshes.size(); i++) {
    Mesh* mesh              = model->meshes[i];
    Material* mesh_material = model->materials[model->material_indices[i]]; 
    mesh_material->shader   = mat->shader; 

    // Setting uniforms
    //
    // @NOTE: Each material has its own valid colors and model. However, we also 
    // want OUR own materials to influence the model. So, we _apply_ our model matrix 
    // and colors to the material.
    mesh_material->ambient_color  = mat->ambient_color; 
    mesh_material->diffuse_color  = mat->diffuse_color; 
    mesh_material->specular_color = mat->specular_color; 

    // Upload the uniforms 
    material_use(mat);

    // Setting up the pipeline for each mesh
    mesh->pipe_desc.shader         = mesh_material->shader;
    mesh->pipe_desc.textures[0]    = mesh_material->diffuse_map;
    mesh->pipe_desc.textures_count = 1;

    // Render the mesh
    gfx_context_apply_pipeline(s_renderer.context, mesh->pipe, mesh->pipe_desc);
    gfx_pipeline_draw_index(mesh->pipe);
  }
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Renderer functions

void renderer_init(Window* window, const Vec4& clear_clear) {
  GfxContextDesc gfx_desc = {
    .window       = window,
    .states       = GFX_STATE_DEPTH | GFX_STATE_STENCIL,
    .pixel_format = GFX_TEXTURE_FORMAT_RGBA8,
  };
  
  s_renderer.context = gfx_context_init(gfx_desc);
  NIKOLA_ASSERT(s_renderer.context, "Failed to initialize the graphics context");

  GfxBufferDesc buff_desc = {
    .data  = nullptr, 
    .size  = sizeof(Mat4) * 2,
    .type  = GFX_BUFFER_UNIFORM,
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  s_renderer.matrices_buffer = gfx_buffer_create(s_renderer.context, buff_desc);

  s_renderer.clear_color = clear_clear;
  s_renderer.clear_flags = GFX_CONTEXT_FLAGS_CLEAR_COLOR_BUFFER |  
                           GFX_CONTEXT_FLAGS_CLEAR_STENCIL_BUFFER | 
                           GFX_CONTEXT_FLAGS_CLEAR_DEPTH_BUFFER;

  NIKOLA_LOG_INFO("Successfully initialized the renderer context");
}

void renderer_shutdown() {
  gfx_context_shutdown(s_renderer.context);
  NIKOLA_LOG_INFO("Successfully shutdown the renderer context");
}

const GfxContext* renderer_get_context() {
  return s_renderer.context;
}

void renderer_set_clear_color(const Vec4& clear_color) {
  s_renderer.clear_color = clear_color;
}

const GfxBuffer* renderer_default_matrices_buffer() {
  return s_renderer.matrices_buffer;
}

void renderer_pre_pass(Camera& cam) {
  s_renderer.camera = cam;

  // Updating the internal matrices buffer for each shader
  gfx_buffer_update(s_renderer.matrices_buffer, 0, sizeof(Mat4), mat4_raw_data(cam.view));
  gfx_buffer_update(s_renderer.matrices_buffer, sizeof(Mat4), sizeof(Mat4), mat4_raw_data(cam.projection));
}

void renderer_begin_pass() {
  Vec4 col = s_renderer.clear_color;
  gfx_context_clear(s_renderer.context, col.r, col.g, col.b, col.a, s_renderer.clear_flags);
}

void renderer_end_pass() {
  for(auto& command : s_renderer.render_queue) {
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

  s_renderer.render_queue.clear();
}

void renderer_post_pass() {
  gfx_context_present(s_renderer.context);
}

void renderer_queue_command(const RenderCommand& command) {
  s_renderer.render_queue.push_back(command);
}

/// Renderer functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
