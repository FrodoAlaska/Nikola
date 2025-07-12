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
  SHADER_CONTEXT_DEFAULT = 0, 
  SHADER_CONTEXT_SKYBOX, 
  SHADER_CONTEXT_HDR, 
  SHADER_CONTEXT_INSTANCE, 
  SHADER_CONTEXT_BLINN, 

  SHADER_CONTEXTS_MAX = SHADER_CONTEXT_BLINN + 1,
};
/// ShaderContextID
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// RenderQueueID
enum RenderQueueID {
  RENDER_QUEUE_OPAQUE = 0, 
  RENDER_QUEUE_DEBUG,

  RENDER_QUEUES_MAX = RENDER_QUEUE_DEBUG + 1,
};
/// RenderQueueID
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// MeshRenderCommand
struct MeshRenderCommand {
  Transform transform = {};

  Mesh* mesh         = nullptr; 
  Material* material = nullptr; 

  MeshRenderCommand(Mesh* mesh, const Transform& trans, Material* mat) 
    :mesh(mesh), transform(trans), material(mat)
  {}
};
/// MeshRenderCommand
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// InstanceData
struct InstanceData {
  Mat4 model; 
  Vec4 color;
};
/// InstanceData
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// RenderUniformBuffer
struct RenderUniformBuffer {
  Mat4 view, projection; 
  Vec3 camera_position;
};
/// RenderUniformBuffer
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Renderer
struct Renderer {
  // Context data

  GfxContext* context = nullptr;
  Vec4 clear_color;

  // Instance data

  InstanceData instance_data[1000];
  sizei instance_count = 0;
  
  GfxPipelineDesc inst_pipe_desc = {};
  GfxPipeline* instance_pipe     = nullptr;
  
  // Defaults data

  RendererDefaults defaults = {};
  ResourceID shader_contexts[SHADER_CONTEXTS_MAX];
  
  // Render data

  GfxPipelineDesc pipe_desc  = {};
  GfxPipeline* pipeline      = nullptr; 
  
  FrameData* frame_data;
  DynamicArray<RenderPass> render_passes;

  DynamicArray<MeshRenderCommand> queues[RENDER_QUEUES_MAX];
};

static Renderer s_renderer{};
/// Renderer
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static void init_context(Window* window) { 
  GfxContextDesc gfx_desc = {
    .window       = window,
    .states       = GFX_STATE_DEPTH | GFX_STATE_STENCIL | GFX_STATE_BLEND | GFX_STATE_SCISSOR,
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
    .size  = sizeof(RenderUniformBuffer),
    .type  = GFX_BUFFER_UNIFORM,
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  s_renderer.defaults.matrices_buffer = resources_get_buffer(resources_push_buffer(RESOURCE_CACHE_ID, buff_desc));

  // Material init
  MaterialDesc mat_desc = {
    .diffuse_id = default_texture_id,
  };
  s_renderer.defaults.material = resources_get_material(resources_push_material(RESOURCE_CACHE_ID, mat_desc));

  // Cube mesh init
  s_renderer.defaults.cube_mesh = resources_get_mesh(resources_push_mesh(RESOURCE_CACHE_ID, GEOMETRY_CUBE));

  // Shaders init
  ResourceID default_shader     = resources_push_shader(RESOURCE_CACHE_ID, generate_default_shader());
  ResourceID skybox_shader      = resources_push_shader(RESOURCE_CACHE_ID, generate_skybox_shader());
  ResourceID hdr_shader         = resources_push_shader(RESOURCE_CACHE_ID, generate_hdr_shader());
  ResourceID inst_shader        = resources_push_shader(RESOURCE_CACHE_ID, generate_instance_shader());
  ResourceID blinn_phong_shader = resources_push_shader(RESOURCE_CACHE_ID, generate_blinn_phong_shader());

  // Shader contexts init
  s_renderer.shader_contexts[SHADER_CONTEXT_DEFAULT]  = resources_push_shader_context(RESOURCE_CACHE_ID, default_shader);
  s_renderer.shader_contexts[SHADER_CONTEXT_SKYBOX]   = resources_push_shader_context(RESOURCE_CACHE_ID, skybox_shader);
  s_renderer.shader_contexts[SHADER_CONTEXT_HDR]      = resources_push_shader_context(RESOURCE_CACHE_ID, hdr_shader);
  s_renderer.shader_contexts[SHADER_CONTEXT_INSTANCE] = resources_push_shader_context(RESOURCE_CACHE_ID, inst_shader);
  s_renderer.shader_contexts[SHADER_CONTEXT_BLINN]    = resources_push_shader_context(RESOURCE_CACHE_ID, blinn_phong_shader);

  // @TEMP
  // Instance pipeline init
 
  geometry_loader_load(RESOURCE_CACHE_ID, &s_renderer.inst_pipe_desc, GEOMETRY_CUBE);

  sizei start = s_renderer.inst_pipe_desc.layouts[0].attributes_count;

  s_renderer.inst_pipe_desc.layouts[1].start_index   = start;
  s_renderer.inst_pipe_desc.layouts[1].instance_rate = 1;

  for(sizei i = start; i < start + 5; i++) {
    s_renderer.inst_pipe_desc.layouts[1].attributes[i] = GFX_LAYOUT_FLOAT4;
    s_renderer.inst_pipe_desc.layouts[1].attributes_count++;
  }

  GfxBufferDesc inst_buff_desc = {
    .data  = nullptr,
    .size  = sizeof(InstanceData) * 1000,
    .type  = GFX_BUFFER_VERTEX, 
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  s_renderer.inst_pipe_desc.instance_buffer = resources_get_buffer(resources_push_buffer(RESOURCE_CACHE_ID, inst_buff_desc));

  s_renderer.instance_pipe = gfx_pipeline_create(s_renderer.context, s_renderer.inst_pipe_desc);
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
  s_renderer.pipe_desc.layouts[0].attributes[0]    = GFX_LAYOUT_FLOAT2;
  s_renderer.pipe_desc.layouts[0].attributes[1]    = GFX_LAYOUT_FLOAT2;
  s_renderer.pipe_desc.layouts[0].attributes_count = 2;

  // Draw mode init 
  s_renderer.pipe_desc.draw_mode = GFX_DRAW_MODE_TRIANGLE;

  // Pipeline init
  s_renderer.pipeline = gfx_pipeline_create(s_renderer.context, s_renderer.pipe_desc);
}

static void queue_model(Model* model, Material* material, const Transform& transform) {
  for(sizei i = 0; i < model->meshes.size(); i++) {
    Mesh* mesh    = model->meshes[i];
    Material* mat = model->materials[model->material_indices[i]];

    // Let the main given material "influence" the model's material 

    mat->color       *= material->color; 
    mat->shininess    = material->shininess;
    mat->transparency = material->transparency;
    mat->depth_mask   = material->depth_mask;

    // @TODO(Renderer): Make sure to include transparent objects
    // @TODO(Renderer): Have a transform parent-child relationship

    s_renderer.queues[RENDER_QUEUE_OPAQUE].emplace_back(mesh, transform, mat);
  }  
}

static void render_mesh(MeshRenderCommand& command, ShaderContext* ctx) {
  // Setting and using shader uniforms 
  
  shader_context_set_uniform(ctx, "u_model", command.transform.transform);
  shader_context_use(ctx);

  // Using the internal material data
  
  material_use(command.material);  
  shader_context_set_uniform(ctx, "u_material.color", command.material->color);
  shader_context_set_uniform(ctx, "u_material.shininess", command.material->shininess);
  shader_context_set_uniform(ctx, "u_material.transparency", command.material->transparency);

  // Set pipeline-related flags from the material

  command.mesh->pipe_desc.depth_mask  = command.material->depth_mask;
  command.mesh->pipe_desc.stencil_ref = command.material->stencil_ref;

  gfx_pipeline_update(command.mesh->pipe, command.mesh->pipe_desc);

  // Draw the mesh
  
  gfx_pipeline_use(command.mesh->pipe);
  gfx_context_draw(s_renderer.context, 0);
}

static void render_skybox(const ResourceID& skybox_id) {
  Skybox* skybox     = resources_get_skybox(skybox_id); 
  ShaderContext* ctx = resources_get_shader_context(s_renderer.shader_contexts[SHADER_CONTEXT_SKYBOX]);

  // Using the shader 
  shader_context_use(ctx);

  // Use the cubemap
  gfx_cubemap_use(&skybox->cubemap, 1);

  // Draw the skybox
  gfx_pipeline_use(skybox->pipe);
  gfx_context_draw(s_renderer.context, 0);
}

static void begin_pass(RenderPass& pass) {
  // Set the pass's target
  gfx_context_set_target(s_renderer.context, pass.frame);
  
  // Clear the framebuffer
  Vec4 col = s_renderer.clear_color;
  gfx_context_clear(s_renderer.context, col.r, col.g, col.b, col.a);
  gfx_context_set_state(s_renderer.context, GFX_STATE_DEPTH, true); 
}

static void end_pass(RenderPass& pass) {
  // Render to the default framebuffer
  
  gfx_context_set_target(s_renderer.context, nullptr);
  
  Vec4 col = s_renderer.clear_color;
  gfx_context_clear(s_renderer.context, col.r, col.g, col.b, col.a);
  gfx_context_set_state(s_renderer.context, GFX_STATE_DEPTH, false); 
  
  // Apply the shader from the pass
  shader_context_use(pass.shader_context);
  
  // Apply the textures from the pass

  GfxTexture* textures[6]; // FRAMEBUFFER_ATTACHMENTS_MAX (4) + depth (1) + stencil (1)
  u32 textures_count = 0; 

  for(sizei i = 0; i < pass.frame_desc.attachments_count; i++) {
    textures[i] = pass.frame_desc.color_attachments[i];
    textures_count++;
  }

  if(pass.frame_desc.depth_attachment) {
    textures[textures_count] = pass.frame_desc.depth_attachment;
    textures_count++;
  }

  if(pass.frame_desc.stencil_attachment) {
    textures[textures_count] = pass.frame_desc.stencil_attachment;
    textures_count++;
  }
  
  gfx_texture_use(textures, textures_count); 

  // Render the final render target
  gfx_pipeline_update(s_renderer.pipeline, s_renderer.pipe_desc);
  gfx_pipeline_use(s_renderer.pipeline);
  gfx_context_draw(s_renderer.context, 0);
}

static void setup_light_enviornment(FrameData& data) {
  ShaderContext* ctx = resources_get_shader_context(s_renderer.shader_contexts[SHADER_CONTEXT_BLINN]);
 
  // Set globals

  shader_context_set_uniform(ctx, "u_ambient", data.ambient); 
  shader_context_set_uniform(ctx, "u_point_lights_count", (i32)data.point_lights.size()); 
  shader_context_set_uniform(ctx, "u_dir_light.direction", data.dir_light.direction); 
  shader_context_set_uniform(ctx, "u_dir_light.color", data.dir_light.color); 

  // Set point lights

  i32 index = 0;

  for(auto& point : data.point_lights) {
    String point_index = "u_point_lights[" + std::to_string(index) + "].";

    shader_context_set_uniform(ctx, (point_index + "position"), point.position); 
    shader_context_set_uniform(ctx, (point_index + "color"), point.color); 

    shader_context_set_uniform(ctx, (point_index + "linear"), point.linear); 
    shader_context_set_uniform(ctx, (point_index + "quadratic"), point.quadratic); 
  
    index++;
  }
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

  // Flush our queues 

  renderer_flush_queue_command();

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
    .frame_size  = Vec2(width, height), 
    .clear_flags = (GFX_CLEAR_FLAGS_COLOR_BUFFER | GFX_CLEAR_FLAGS_DEPTH_BUFFER | GFX_CLEAR_FLAGS_STENCIL_BUFFER),
    .shader_context_id = s_renderer.shader_contexts[SHADER_CONTEXT_HDR],
  };
  light_pass.targets.push_back(GFX_TEXTURE_FORMAT_RGBA32F);
  light_pass.targets.push_back(GFX_TEXTURE_FORMAT_DEPTH_STENCIL_24_8);

  renderer_push_pass(light_pass, light_pass_fn);
  
  NIKOLA_LOG_INFO("Successfully initialized the renderer context");
}

void renderer_shutdown() {
  for(auto& entry : s_renderer.render_passes) {
    gfx_framebuffer_destroy(entry.frame);
  }

  gfx_pipeline_destroy(s_renderer.pipeline);
  gfx_pipeline_destroy(s_renderer.instance_pipe);
  
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

const u32 renderer_push_pass(const RenderPassDesc& desc, const RenderPassFn& func) {
  RenderPass pass = {};

  // Framebuffer init

  pass.frame_size     = desc.frame_size;
  pass.shader_context = resources_get_shader_context(desc.shader_context_id);

  pass.frame_desc              = {}; 
  pass.frame_desc.clear_flags = desc.clear_flags;
 
  pass.user_data = (void*)desc.user_data;
  pass.pass_func = func;

  // Render targets init
  
  for(sizei i = 0; i < desc.targets.size(); i++) {
    GfxTextureType type = GFX_TEXTURE_RENDER_TARGET;
    switch(desc.targets[i]) {
      case GFX_TEXTURE_FORMAT_DEPTH16:
      case GFX_TEXTURE_FORMAT_DEPTH24:
      case GFX_TEXTURE_FORMAT_DEPTH32F:
        type = GFX_TEXTURE_DEPTH_TARGET;
        break;
      case GFX_TEXTURE_FORMAT_STENCIL8:
        type = GFX_TEXTURE_STENCIL_TARGET;
        break;
      case GFX_TEXTURE_FORMAT_DEPTH_STENCIL_24_8:
        type = GFX_TEXTURE_DEPTH_STENCIL_TARGET;
        break;
      default:
        break;
    }

    GfxTextureDesc texture_desc = {
      .width     = (u32)pass.frame_size.x, 
      .height    = (u32)pass.frame_size.y, 
      .depth     = 0, 
      .mips      = 1, 
      .type      = type,
      .format    = desc.targets[i],
      .filter    = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST,
      .wrap_mode = GFX_TEXTURE_WRAP_MIRROR,
      .data      = nullptr,
    };

    switch(type) {
      case GFX_TEXTURE_RENDER_TARGET:
        pass.frame_desc.color_attachments[i] = gfx_texture_create(s_renderer.context, texture_desc);
        pass.frame_desc.attachments_count++;
        break;
      case GFX_TEXTURE_DEPTH_TARGET:
      case GFX_TEXTURE_DEPTH_STENCIL_TARGET:
        pass.frame_desc.depth_attachment = gfx_texture_create(s_renderer.context, texture_desc);
        break;
      case GFX_TEXTURE_STENCIL_TARGET:
        pass.frame_desc.stencil_attachment = gfx_texture_create(s_renderer.context, texture_desc);
        break;
    }
  }

  // Welcome, new render pass

  pass.frame = gfx_framebuffer_create(s_renderer.context, pass.frame_desc);
  s_renderer.render_passes.push_back(pass);

  return (u32)s_renderer.render_passes.size() - 1;
}

void renderer_queue_command(const RenderCommand& command) {
  NIKOLA_ASSERT(RESOURCE_IS_VALID(command.renderable_id), "Cannot render an invalid resource");

  Material* material = s_renderer.defaults.material;
  if(RESOURCE_IS_VALID(command.material_id)) {
    material = resources_get_material(command.material_id);
  }

  switch(command.type) {
    case RENDERABLE_MESH:
      s_renderer.queues[RENDER_QUEUE_OPAQUE].emplace_back(resources_get_mesh(command.renderable_id), 
                                                          command.transform, 
                                                          material);
      break;
    case RENDERABLE_MODEL:
      queue_model(resources_get_model(command.renderable_id), material, command.transform);
      break;
    case RENDERABLE_DEBUG:
      s_renderer.queues[RENDER_QUEUE_DEBUG].emplace_back(resources_get_mesh(command.renderable_id), 
                                                         command.transform, 
                                                         material);
      break;
    default:
      NIKOLA_LOG_ERROR("Invalid or unsupported render command given... skiping");
      break;
  }

  // @TODO (Renderer): Direct certain objects towards certain render queue. 
  // i.e, transparent objects need a `transparent_queue` and so on.
}

void renderer_flush_queue_command(const ResourceID& shader_context_id) {
  ResourceID ctx_id  = RESOURCE_IS_VALID(shader_context_id) ? shader_context_id : s_renderer.shader_contexts[SHADER_CONTEXT_BLINN];
  
  ShaderContext* default_ctx = resources_get_shader_context(ctx_id);
  ShaderContext* debug_ctx   = resources_get_shader_context(s_renderer.shader_contexts[SHADER_CONTEXT_DEFAULT]);

  // Flush the opaque queue

  for(auto& cmd : s_renderer.queues[RENDER_QUEUE_OPAQUE]) {
    render_mesh(cmd, default_ctx);
  }

  // Flush the debug queue
  // @TODO (Renderer): Do we need this here?

  for(auto& cmd : s_renderer.queues[RENDER_QUEUE_DEBUG]) {
    render_mesh(cmd, debug_ctx);
  }
}

void renderer_begin(FrameData& data) {
  GfxBuffer* matrix_buffer = s_renderer.defaults.matrices_buffer;
  s_renderer.frame_data    = &data;
   
  // Updating the internal matrices buffer for each shader
 
  RenderUniformBuffer uni_buff = {
    .view            = data.camera.view, 
    .projection      = data.camera.projection, 
    .camera_position = data.camera.position,
  };
  gfx_buffer_upload_data(matrix_buffer, 0, sizeof(RenderUniformBuffer), &uni_buff);

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
  RenderPass* light_entry  = &s_renderer.render_passes[0];

  begin_pass(*light_entry);
  light_entry->pass_func(nullptr, light_entry, light_entry->user_data);
  end_pass(*light_entry);

  // Initiate all of the custrom render passes 
  
  sizei previous_index = 0;

  for(sizei i = 1; i < s_renderer.render_passes.size(); i++) {
    RenderPass* entry = &s_renderer.render_passes[i];
    if(!entry->is_active) { 
      continue;
    } 

    begin_pass(*entry);
    entry->pass_func(&s_renderer.render_passes[previous_index], entry, entry->user_data);
    end_pass(*entry);

    previous_index++;
  } 
  
  // Clear the queues
  
  for(sizei i = 0; i < RENDER_QUEUES_MAX; i++) {
    s_renderer.queues[i].clear();
  }
}

/// Renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
