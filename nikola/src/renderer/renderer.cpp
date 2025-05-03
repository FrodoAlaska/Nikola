#include "nikola/nikola_render.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_gfx.h"

#include "render_shaders.h"
#include "light_shaders.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// ShaderContextID
enum ShaderContextID {
  SHADER_CONTEXT_DEFAULT     = 0, 
  SHADER_CONTEXT_SKYBOX      = 1, 
  SHADER_CONTEXT_FRAEMBUFFER = 2, 
  SHADER_CONTEXT_HDR         = 3, 
  SHADER_CONTEXT_BLINN       = 4, 

  SHADER_CONTEXTS_MAX        = SHADER_CONTEXT_BLINN + 1,
};
/// ShaderContextID
/// ----------------------------------------------------------------------

/// RenderPassEntry
struct RenderPassEntry {
  RenderPass pass; 
  RenderPassFn func; 
  void* user_data  = nullptr;
};
/// RenderPassEntry
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Renderer
struct Renderer {
  GfxContext* context = nullptr;

  Vec4 clear_color;

  GfxPipelineDesc pipe_desc  = {};
  GfxPipeline* pipeline      = nullptr; 

  RendererDefaults defaults = {};
  ResourceID shader_contexts[SHADER_CONTEXTS_MAX];
  
  RenderQueue current_queue;  
  FrameData* frame_data;
  ResourceID current_skybox;
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
    .states       = GFX_STATE_DEPTH | GFX_STATE_STENCIL | GFX_STATE_BLEND,
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
  s_renderer.defaults.texture = resources_push_texture(RESOURCE_CACHE_ID, texture_desc);

  // Matrices buffer init
  GfxBufferDesc buff_desc = {
    .data  = nullptr, 
    .size  = sizeof(Mat4) * 2,
    .type  = GFX_BUFFER_UNIFORM,
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  s_renderer.defaults.matrices_buffer = resources_push_buffer(RESOURCE_CACHE_ID, buff_desc);

  // Shaders init
  ResourceID default_shader     = resources_push_shader(RESOURCE_CACHE_ID, generate_default_shader());
  ResourceID skybox_shader      = resources_push_shader(RESOURCE_CACHE_ID, generate_skybox_shader());
  ResourceID framebuffer_shader = resources_push_shader(RESOURCE_CACHE_ID, generate_framebuffer_shader());
  ResourceID hdr_shader         = resources_push_shader(RESOURCE_CACHE_ID, generate_hdr_shader());
  ResourceID blinn_phong_shader = resources_push_shader(RESOURCE_CACHE_ID, generate_blinn_phong_shader());

  // Shader contexts init
  s_renderer.shader_contexts[SHADER_CONTEXT_DEFAULT]     = resources_push_shader_context(RESOURCE_CACHE_ID, default_shader);
  s_renderer.shader_contexts[SHADER_CONTEXT_SKYBOX]      = resources_push_shader_context(RESOURCE_CACHE_ID, skybox_shader);
  s_renderer.shader_contexts[SHADER_CONTEXT_FRAEMBUFFER] = resources_push_shader_context(RESOURCE_CACHE_ID, framebuffer_shader);
  s_renderer.shader_contexts[SHADER_CONTEXT_HDR]         = resources_push_shader_context(RESOURCE_CACHE_ID, hdr_shader);
  s_renderer.shader_contexts[SHADER_CONTEXT_BLINN]       = resources_push_shader_context(RESOURCE_CACHE_ID, blinn_phong_shader);
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

static void render_mesh(RenderCommand& command, ResourceID& shader_context) {
  Mesh* mesh = resources_get_mesh(command.renderable_id);

  // Setting uniforms 
  shader_context_set_uniform(shader_context, MATERIAL_UNIFORM_MODEL_MATRIX, command.transform.transform);

  // Using the shader 
  shader_context_use(shader_context);

  // Using the textures
  material_use(command.material_id);  

  // Draw the mesh
  gfx_pipeline_draw_index(mesh->pipe);
}

static void render_skybox(RenderCommand& command) {
  Skybox* skybox = resources_get_skybox(command.renderable_id); 

  // Using the shader 
  shader_context_use(s_renderer.shader_contexts[SHADER_CONTEXT_SKYBOX]);

  // Use the cubemap
  GfxCubemap* cube = resources_get_cubemap(skybox->cubemap);
  gfx_cubemap_use(&cube, 1);

  // Draw the skybox
  gfx_pipeline_draw_vertex(skybox->pipe);
}

static void render_model(RenderCommand& command, ResourceID& shader_context) {
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
    };

    // Render the sub-command
    render_mesh(sub_cmd, shader_context);
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
      .type      = desc.targets[i].type,
      .format    = desc.targets[i].format,
      .filter    = desc.targets[i].filter, 
      .wrap_mode = desc.targets[i].wrap_mode, 
      .data      = nullptr,
    };
    pass->frame_desc.attachments[i] = gfx_texture_create(s_renderer.context, texture_desc);
    pass->frame_desc.attachments_count++;
  }

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

static void flush_queue(ResourceID& shader_context) {
  for(auto& command : s_renderer.current_queue) {
    switch(command.render_type) {
      case RENDERABLE_TYPE_MESH:
        render_mesh(command, shader_context);
        break;
      case RENDERABLE_TYPE_MODEL:
        render_model(command, shader_context);
        break;
      case RENDERABLE_TYPE_SKYBOX:
        render_skybox(command);
        break;
    }
  }
}

static void use_directional_light(DirectionalLight& light) {
  shader_context_set_uniform(s_renderer.shader_contexts[SHADER_CONTEXT_BLINN], "u_dir_light.direction", light.direction); 
  shader_context_set_uniform(s_renderer.shader_contexts[SHADER_CONTEXT_BLINN], "u_dir_light.color", light.color); 
}

static void use_point_lights(DynamicArray<PointLight>& lights) {
  i32 index = 0;

  for(auto& point : lights) {
    String point_index = "u_point_lights[" + std::to_string(index) + "].";

    shader_context_set_uniform(s_renderer.shader_contexts[SHADER_CONTEXT_BLINN], (point_index + "position"), point.position); 
    shader_context_set_uniform(s_renderer.shader_contexts[SHADER_CONTEXT_BLINN], (point_index + "color"), point.color); 
    
    shader_context_set_uniform(s_renderer.shader_contexts[SHADER_CONTEXT_BLINN], ( point_index + "linear"), point.linear); 
    shader_context_set_uniform(s_renderer.shader_contexts[SHADER_CONTEXT_BLINN], ( point_index + "quadratic"), point.quadratic); 
  
    index++;
  }
}

static void setup_light_enviornment(FrameData& data) {
  shader_context_set_uniform(s_renderer.shader_contexts[SHADER_CONTEXT_BLINN], "u_ambient", data.ambient); 
  shader_context_set_uniform(s_renderer.shader_contexts[SHADER_CONTEXT_BLINN], "u_point_lights_count", (i32)data.point_lights.size()); 
  // @TODO (Renderer): shader_context_set_uniform(s_renderer.shader_contexts[SHADER_CONTEXT_BLINN], "u_view_pos", data.camera.direction); 

  use_directional_light(data.dir_light);
  use_point_lights(data.point_lights);
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks 

static void light_pass_fn(const RenderPass* previous, RenderPass* current, void* user_data) {
  // Render the current set skybox first (if it exists)
  if(RESOURCE_IS_VALID(s_renderer.current_skybox)) {
    RenderCommand skybox_cmd = {
      .render_type   = RENDERABLE_TYPE_SKYBOX, 
      .renderable_id = s_renderer.current_skybox,
    };
    render_skybox(skybox_cmd);
  }

  // Render the frame with the light data 
  flush_queue(s_renderer.shader_contexts[SHADER_CONTEXT_BLINN]);

  // Updating some HDR uniforms
  shader_context_set_uniform(s_renderer.shader_contexts[SHADER_CONTEXT_HDR], "u_exposure", s_renderer.frame_data->camera.exposure);
}

/// Callbacks 
/// ----------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Renderer functions

void renderer_init(Window* window) {
  // Context init 
  init_context(window);
  
  i32 width, height;
  window_get_size(window, &width, &height); 

  // Defaults init
  init_defaults();

  // Pipeline init
  init_pipeline();

  // Light pass init
  nikola::RenderPassDesc light_pass = {
    .frame_size        = Vec2(width, height), 
    .clear_color       = Vec4(1.0f),
    .clear_flags       = (GFX_CLEAR_FLAGS_COLOR_BUFFER | GFX_CLEAR_FLAGS_DEPTH_BUFFER),
    .shader_context_id = s_renderer.shader_contexts[SHADER_CONTEXT_HDR],
  };
  light_pass.targets.push_back(RenderTarget{
      .type = GFX_TEXTURE_RENDER_TARGET, 
      .format = GFX_TEXTURE_FORMAT_RGBA32F,
  });
  light_pass.targets.push_back(RenderTarget{
      .type = GFX_TEXTURE_DEPTH_STENCIL_TARGET, 
      .format = GFX_TEXTURE_FORMAT_DEPTH_STENCIL_24_8
  });
  renderer_push_pass(light_pass, light_pass_fn, nullptr);
  
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

GfxContext* renderer_get_context() {
  return s_renderer.context;
}

const RendererDefaults& renderer_get_defaults() {
  return s_renderer.defaults;
}

void renderer_set_clear_color(const Vec4& clear_color) {
  s_renderer.clear_color = clear_color;
}

void renderer_push_pass(const RenderPassDesc& desc, const RenderPassFn& func, const void* user_data) {
  RenderPassEntry entry;
  entry.func      = func; 
  entry.user_data = (void*)user_data;
  create_render_pass(&entry.pass, desc);

  s_renderer.render_passes.push_back(entry);
}

void renderer_sumbit_queue(RenderQueue& queue) {
  s_renderer.current_queue = queue;
}

void renderer_begin(FrameData& data) {
  GfxBuffer* matrix_buffer = resources_get_buffer(s_renderer.defaults.matrices_buffer);

  // Updating the internal matrices buffer for each shader
  gfx_buffer_update(matrix_buffer, 0, sizeof(Mat4), mat4_raw_data(data.camera.view));
  gfx_buffer_update(matrix_buffer, sizeof(Mat4), sizeof(Mat4), mat4_raw_data(data.camera.projection));
  s_renderer.frame_data = &data;

  // Render the skybox (if avaliable)
  s_renderer.current_skybox = data.skybox_id;

  // Setup some lighting
  setup_light_enviornment(data);
}

void renderer_end() {
  /* @NOTE (16/4/2025, Mohamed):
  *
  * Since the first entry of the render passes will almost always 
  * be the preset light pass, we might as well initiate it 
  * seperately.
  * 
  */
  RenderPassEntry* light_entry  = &s_renderer.render_passes[0];
  light_entry->pass.clear_color = s_renderer.clear_color; // Update the default clear color

  begin_pass(light_entry->pass);
  light_entry->func(nullptr, &light_entry->pass, light_entry->user_data);
  end_pass(light_entry->pass);

  // Initiate all of the custrom render passes 
  for(sizei i = 1; i < s_renderer.render_passes.size(); i++) {
    RenderPassEntry* entry = &s_renderer.render_passes[i];
  
    begin_pass(entry->pass);
    entry->func(&s_renderer.render_passes[i - 1].pass, &entry->pass, entry->user_data);
    end_pass(entry->pass);
  } 
  
  // Clear the current render queue (if there's any there)
  if(!s_renderer.current_queue.empty()) {
    s_renderer.current_queue.clear();
  }
}

/// Renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
