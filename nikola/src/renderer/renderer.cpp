#include "nikola/nikola_render.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_gfx.h"
#include "nikola/nikola_math.h"
#include "nikola/nikola_physics.h"

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
/// MeshRenderCommand
struct MeshRenderCommand {
  Transform transform = {};

  Mesh* mesh                    = nullptr; 
  Material* material            = nullptr; 
  ShaderContext* shader_context = nullptr;

  // @TODO (Renderer): Temporary
  Vec4 color;

  MeshRenderCommand(Mesh* mesh, const Transform& trans, Material* mat, ShaderContext* ctx, const Vec4& color = Vec4(1.0f)) 
    :mesh(mesh), transform(trans), material(mat), shader_context(ctx), color(color)
  {}
};
/// MeshRenderCommand
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
  
  FrameData* frame_data;
  DynamicArray<RenderPassEntry> render_passes;

  DynamicArray<MeshRenderCommand> render_queue;
  DynamicArray<MeshRenderCommand> debug_queue;
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
    .has_vsync    = false,
  };
  
  s_renderer.context = gfx_context_init(gfx_desc);
  NIKOLA_ASSERT(s_renderer.context, "Failed to initialize the graphics context");
}

static void init_defaults() {
  // Default texture init
  u32 pixels = 0xffffffff; 
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

  ResourceID default_texture_id = resources_push_texture(RESOURCE_CACHE_ID, texture_desc);
  s_renderer.defaults.texture   = resources_get_texture(default_texture_id);

  // Matrices buffer init
  GfxBufferDesc buff_desc = {
    .data  = nullptr, 
    .size  = sizeof(Mat4) * 2,
    .type  = GFX_BUFFER_UNIFORM,
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  s_renderer.defaults.matrices_buffer = resources_get_buffer(resources_push_buffer(RESOURCE_CACHE_ID, buff_desc));

  // Material init
  s_renderer.defaults.material = resources_get_material(resources_push_material(RESOURCE_CACHE_ID, default_texture_id));

  // Cube mesh init
  s_renderer.defaults.cube_mesh = resources_get_mesh(resources_push_mesh(RESOURCE_CACHE_ID, GEOMETRY_CUBE));

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

static void render_mesh(MeshRenderCommand& command) {
  // Setting uniforms 
  shader_context_set_uniform(command.shader_context, MATERIAL_UNIFORM_MODEL_MATRIX, command.transform.transform);
  shader_context_set_uniform(command.shader_context, MATERIAL_UNIFORM_COLOR, command.color);

  // Using the shader 
  shader_context_use(command.shader_context);

  // Using the internal material data
  material_use(command.material);  

  // Draw the mesh
  gfx_pipeline_draw_index(command.mesh->pipe);
}

static void render_skybox(const ResourceID& skybox_id) {
  Skybox* skybox     = resources_get_skybox(skybox_id); 
  ShaderContext* ctx = resources_get_shader_context(s_renderer.shader_contexts[SHADER_CONTEXT_SKYBOX]);

  // Using the shader 
  shader_context_use(ctx);

  // Use the cubemap
  gfx_cubemap_use(&skybox->cubemap, 1);

  // Draw the skybox
  gfx_pipeline_draw_vertex(skybox->pipe);
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
  shader_context_use(resources_get_shader_context(pass.shader_context_id));
  
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

static void flush_queue(DynamicArray<MeshRenderCommand>& queue) {
  for(auto& command : queue) {
    render_mesh(command);
  }
}

static void use_directional_light(DirectionalLight& light, ShaderContext* ctx) {
  shader_context_set_uniform(ctx, "u_dir_light.direction", light.direction); 
  shader_context_set_uniform(ctx, "u_dir_light.color", light.color); 
}

static void use_point_lights(DynamicArray<PointLight>& lights, ShaderContext* ctx) {
  i32 index = 0;

  for(auto& point : lights) {
    String point_index = "u_point_lights[" + std::to_string(index) + "].";

    shader_context_set_uniform(ctx, (point_index + "position"), point.position); 
    shader_context_set_uniform(ctx, (point_index + "color"), point.color); 

    shader_context_set_uniform(ctx, (point_index + "linear"), point.linear); 
    shader_context_set_uniform(ctx, (point_index + "quadratic"), point.quadratic); 
  
    index++;
  }
}

static void setup_light_enviornment(FrameData& data) {
  ShaderContext* ctx = resources_get_shader_context(s_renderer.shader_contexts[SHADER_CONTEXT_BLINN]);
  
  shader_context_set_uniform(ctx, "u_ambient", data.ambient); 
  shader_context_set_uniform(ctx, "u_point_lights_count", (i32)data.point_lights.size()); 
  shader_context_set_uniform(ctx, "u_view_pos", data.camera.direction); 

  use_directional_light(data.dir_light, ctx);
  use_point_lights(data.point_lights, ctx);
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks 

static void light_pass_fn(const RenderPass* previous, RenderPass* current, void* user_data) {
  // Render the current set skybox first (if it exists)
  if(RESOURCE_IS_VALID(s_renderer.frame_data->skybox_id)) {
    render_skybox(s_renderer.frame_data->skybox_id);
  }

  // Render the frame with the light data 
  flush_queue(s_renderer.render_queue);

  // @TODO (Renderer): Probably better not to flush 
  // the debug queue here. But, oh well. 
  flush_queue(s_renderer.debug_queue);

  // Updating some HDR uniforms
  shader_context_set_uniform(resources_get_shader_context(s_renderer.shader_contexts[SHADER_CONTEXT_HDR]), "u_exposure", s_renderer.frame_data->camera.exposure);
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
  RenderPassDesc light_pass = {
    .frame_size        = Vec2(width, height), 
    .clear_color       = Vec4(1.0f),
    .clear_flags       = (GFX_CLEAR_FLAGS_COLOR_BUFFER | GFX_CLEAR_FLAGS_DEPTH_BUFFER | GFX_CLEAR_FLAGS_STENCIL_BUFFER),
    .shader_context_id = s_renderer.shader_contexts[SHADER_CONTEXT_HDR],
  };
  light_pass.targets.push_back(RenderTarget{
      .type   = GFX_TEXTURE_RENDER_TARGET, 
      .format = GFX_TEXTURE_FORMAT_RGBA32F,
  });
  light_pass.targets.push_back(RenderTarget{
      .type   = GFX_TEXTURE_DEPTH_TARGET, 
      .format = GFX_TEXTURE_FORMAT_DEPTH24
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

Vec4& renderer_get_clear_color() {
  return s_renderer.clear_color;
}

void renderer_push_pass(const RenderPassDesc& desc, const RenderPassFn& func, const void* user_data) {
  RenderPassEntry entry;
  entry.func      = func; 
  entry.user_data = (void*)user_data;
  create_render_pass(&entry.pass, desc);

  s_renderer.render_passes.push_back(entry);
}

void renderer_queue_mesh(const ResourceID& mesh_id, const Transform& transform, const ResourceID& mat_id, const ResourceID& shader_context_id) {
  Mesh* mesh         = resources_get_mesh(mesh_id);
  Material* mat      = RESOURCE_IS_VALID(mat_id) ? resources_get_material(mat_id) : s_renderer.defaults.material; 
  ShaderContext* ctx = RESOURCE_IS_VALID(shader_context_id) ? resources_get_shader_context(shader_context_id) : resources_get_shader_context(s_renderer.shader_contexts[SHADER_CONTEXT_BLINN]);

  s_renderer.render_queue.emplace_back(mesh, transform, mat, ctx);
}

void renderer_queue_model(const ResourceID& model_id, const Transform& transform, const ResourceID& shader_context_id) {
  Model* model       = resources_get_model(model_id);
  ShaderContext* ctx = RESOURCE_IS_VALID(shader_context_id) ? resources_get_shader_context(shader_context_id) : resources_get_shader_context(s_renderer.shader_contexts[SHADER_CONTEXT_BLINN]);

  // Breaking up the model into multiple mesh render commands
  for(sizei i = 0; i < model->meshes.size(); i++) {
    Mesh* mesh    = model->meshes[i];
    Material* mat = model->materials[model->material_indices[i]]; 

    s_renderer.render_queue.emplace_back(mesh, transform, mat, ctx);
  }
}

void renderer_debug_cube(const Transform& transform, const Vec4& color) {
  ShaderContext* shader_context = resources_get_shader_context(s_renderer.shader_contexts[SHADER_CONTEXT_DEFAULT]);
  s_renderer.debug_queue.emplace_back(s_renderer.defaults.cube_mesh, transform, s_renderer.defaults.material, shader_context, color);
}

void renderer_debug_collider(const Collider* coll, const Vec3& color) {
  Transform transform = collider_get_world_transform(coll);
  transform_scale(transform, collider_get_extents(coll));

  renderer_debug_cube(transform, Vec4(color, 0.2f));
}

void renderer_begin(FrameData& data) {
  GfxBuffer* matrix_buffer = s_renderer.defaults.matrices_buffer;

  // Updating the internal matrices buffer for each shader
  s_renderer.frame_data = &data;
  gfx_buffer_update(matrix_buffer, 0, sizeof(Mat4), mat4_raw_data(data.camera.view));
  gfx_buffer_update(matrix_buffer, sizeof(Mat4), sizeof(Mat4), mat4_raw_data(data.camera.projection));

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
  
  // Clear the queues
  s_renderer.render_queue.clear();
  s_renderer.debug_queue.clear();
}

/// Renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
