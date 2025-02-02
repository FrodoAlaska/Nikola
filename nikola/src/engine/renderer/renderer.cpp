#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Renderer
struct Renderer {
  GfxContext* context = nullptr;
  Vec4 clear_color;
  Camera cam;

  u32 clear_flags = 0;

  DynamicArray<GfxPipeline*> render_queue;
};

static Renderer s_renderer;
/// Renderer
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static void queue_mesh(ResourceStorage* storage, const RenderCommand& mesh_command) {
  Mesh* mesh           = resource_storage_get_mesh(storage, mesh_command.renderable_id);
  Material* material   = resource_storage_get_material(storage, mesh_command.material_id);
  GfxShader* shader    = resource_storage_get_shader(storage, material->shader);
  GfxTexture* diffuse  = resource_storage_get_texture(storage, material->diffuse_map);

  mesh->pipe_desc.shader = shader;
  
  mesh->pipe_desc.textures[0]    = diffuse;
  mesh->pipe_desc.textures_count = 1;

  i32 uni_loc = gfx_glsl_get_uniform_location(shader, "u_model");
  gfx_glsl_upload_uniform(shader, uni_loc, GFX_LAYOUT_MAT4, mat4_raw_data(mesh_command.transform.transform));

  gfx_context_apply_pipeline(s_renderer.context, mesh->pipe, mesh->pipe_desc);
  s_renderer.render_queue.push_back(mesh->pipe);
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

void renderer_begin(Camera& cam) {
  Vec4 col = s_renderer.clear_color;
  gfx_context_clear(s_renderer.context, col.r, col.g, col.b, col.a, s_renderer.clear_flags);

  s_renderer.cam = cam;
  s_renderer.render_queue.clear();
}

void renderer_end() {
  for(auto& pipe : s_renderer.render_queue) {
    gfx_pipeline_draw_vertex(pipe); 
  }

  gfx_context_present(s_renderer.context);
}

void renderer_queue_command(ResourceStorage* res_storage, const RenderCommand& command) {
  NIKOLA_ASSERT(res_storage, "Cannot render with an invalid ResourceStorage");

  switch(command.render_type) {
    case RENDERABLE_TYPE_MESH:
      queue_mesh(res_storage, command);
      break;
    case RENDERABLE_TYPE_MODEL:
      // @TODO
      break;
    case RENDERABLE_TYPE_SKYBOX:
      // @TODO
      break;
  }
}

/// Renderer functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
