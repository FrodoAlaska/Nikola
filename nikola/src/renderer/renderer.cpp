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
  SHADER_CONTEXT_SCREEN_SPACE, 
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
/// InstanceData
struct InstanceData {
  Mat4 model; 
};
/// InstanceData
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
/// MeshRenderInstanceCommand
struct MeshRenderInstanceCommand {
  InstanceData instance_data[RENDERER_MAX_INSTANCES]; 
  
  Mesh* mesh         = nullptr;
  Material* material = nullptr;

  sizei instance_count = 1;

  MeshRenderInstanceCommand(Mesh* mesh, const Transform* transforms, Material* mat, const sizei count) 
    :mesh(mesh), material(mat), instance_count(count) {
    for(sizei i = 0; i < count; i++) {
      instance_data[i].model = transforms[i].transform;
    }
  }
};
/// MeshRenderInstanceCommand
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

  InstanceData instance_data[RENDERER_MAX_INSTANCES];
  sizei instance_count = 0;
 
  GfxBuffer* instance_buffer = nullptr;
  
  // Defaults data

  RendererDefaults defaults = {};
  ResourceID shader_contexts[SHADER_CONTEXTS_MAX];
  
  // Render data

  GfxPipelineDesc pipe_desc = {};
  GfxPipeline* pipeline     = nullptr; 
  
  FrameData* frame_data;
  DynamicArray<RenderPass> render_passes;

  DynamicArray<MeshRenderCommand> queues[RENDER_QUEUES_MAX];
  DynamicArray<MeshRenderInstanceCommand> instance_queue;
};

static Renderer s_renderer{};
/// Renderer
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

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
  
  ResourceID default_shader      = resources_push_shader(RESOURCE_CACHE_ID, generate_default_shader());
  ResourceID skybox_shader       = resources_push_shader(RESOURCE_CACHE_ID, generate_skybox_shader());
  ResourceID screen_space_shader = resources_push_shader(RESOURCE_CACHE_ID, generate_screen_space_shader());
  ResourceID hdr_shader          = resources_push_shader(RESOURCE_CACHE_ID, generate_hdr_shader());
  ResourceID inst_shader         = resources_push_shader(RESOURCE_CACHE_ID, generate_instance_shader());
  ResourceID blinn_phong_shader  = resources_push_shader(RESOURCE_CACHE_ID, generate_blinn_phong_shader());

  // Shader contexts init
  
  s_renderer.shader_contexts[SHADER_CONTEXT_DEFAULT]      = resources_push_shader_context(RESOURCE_CACHE_ID, default_shader);
  s_renderer.shader_contexts[SHADER_CONTEXT_SKYBOX]       = resources_push_shader_context(RESOURCE_CACHE_ID, skybox_shader);
  s_renderer.shader_contexts[SHADER_CONTEXT_SCREEN_SPACE] = resources_push_shader_context(RESOURCE_CACHE_ID, screen_space_shader);
  s_renderer.shader_contexts[SHADER_CONTEXT_HDR]          = resources_push_shader_context(RESOURCE_CACHE_ID, hdr_shader);
  s_renderer.shader_contexts[SHADER_CONTEXT_INSTANCE]     = resources_push_shader_context(RESOURCE_CACHE_ID, inst_shader);
  s_renderer.shader_contexts[SHADER_CONTEXT_BLINN]        = resources_push_shader_context(RESOURCE_CACHE_ID, blinn_phong_shader);

  // Instance buffer init
  
  GfxBufferDesc inst_buff_desc = {
    .data  = nullptr,
    .size  = sizeof(InstanceData) * RENDERER_MAX_INSTANCES,
    .type  = GFX_BUFFER_VERTEX, 
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  s_renderer.instance_buffer = resources_get_buffer(resources_push_buffer(RESOURCE_CACHE_ID, inst_buff_desc));

  // @TODO (Renderer): Maybe have this somewhere else?
  shader_context_set_uniform_buffer(resources_get_shader_context(s_renderer.shader_contexts[SHADER_CONTEXT_INSTANCE]), 
                                    SHADER_INSTANCE_BUFFER_INDEX, 
                                    s_renderer.instance_buffer);
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

static void queue_model_instanced(Model* model, Material* material, const Transform* transforms, const sizei inst_count) {
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

    s_renderer.instance_queue.emplace_back(mesh, transforms, mat, inst_count);
  }  
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

  // Reset the pass's state
  pass.input_textures_count = 0;
}

static void end_pass(RenderPass& pass) {
  // Render back to the default framebuffer
  
  gfx_context_set_target(s_renderer.context, nullptr);
  
  Vec4 col = s_renderer.clear_color;
  gfx_context_clear(s_renderer.context, col.r, col.g, col.b, col.a);
  gfx_context_set_state(s_renderer.context, GFX_STATE_DEPTH, false); 
  
  // Apply the shader from the pass
  shader_context_use(pass.shader_context);
  
  // Apply the textures from the pass
  gfx_texture_use(pass.input_textures, pass.input_textures_count); 

  // Render the final render target
  gfx_pipeline_update(s_renderer.pipeline, s_renderer.pipe_desc);
  gfx_pipeline_use(s_renderer.pipeline);
  gfx_context_draw(s_renderer.context, 0);
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
  
  current->input_textures[0]    = current->frame_desc.color_attachments[0];
  current->input_textures_count++;
}

static void hdr_pass_fn(const RenderPass* previous, RenderPass* current, void* user_data) {
  current->input_textures[0]    = (GfxTexture*)previous->frame_desc.color_attachments[0];
  current->input_textures_count++;

  // Updating some HDR uniforms
  shader_context_set_uniform(resources_get_shader_context(s_renderer.shader_contexts[SHADER_CONTEXT_HDR]), 
                             "u_exposure", 
                             s_renderer.frame_data->camera.exposure);
}

/// Callbacks 
/// ----------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Renderer functions

void renderer_init(Window* window) {
  // Context init 
  
  GfxContextDesc gfx_desc = {
    .window       = window,
    .states       = GFX_STATE_DEPTH | GFX_STATE_STENCIL | GFX_STATE_BLEND,
    .has_vsync    = false,
  };
  
  s_renderer.context = gfx_context_init(gfx_desc);
  NIKOLA_ASSERT(s_renderer.context, "Failed to initialize the graphics context");

  // Defaults init
  init_defaults();

  // Pipeline init
  init_pipeline();

  // Render passes init
  
  i32 width, height;
  window_get_size(window, &width, &height); 

  // Light pass

  RenderPassDesc light_pass = {
    .frame_size        = IVec2(width, height), 
    .clear_flags       = (GFX_CLEAR_FLAGS_COLOR_BUFFER | GFX_CLEAR_FLAGS_DEPTH_BUFFER | GFX_CLEAR_FLAGS_STENCIL_BUFFER),
    .shader_context_id = s_renderer.shader_contexts[SHADER_CONTEXT_SCREEN_SPACE],
  };
  light_pass.targets.push_back(GFX_TEXTURE_FORMAT_RGBA8);
  light_pass.targets.push_back(GFX_TEXTURE_FORMAT_DEPTH_STENCIL_24_8);
  
  renderer_push_pass(light_pass, light_pass_fn);

  // HDR pass

  RenderPassDesc hdr_pass = {
    .frame_size        = IVec2(width, height), 
    .clear_flags       = GFX_CLEAR_FLAGS_COLOR_BUFFER,
    .shader_context_id = s_renderer.shader_contexts[SHADER_CONTEXT_HDR],
  };
  hdr_pass.targets.push_back(GFX_TEXTURE_FORMAT_RGBA32F);

  renderer_push_pass(hdr_pass, hdr_pass_fn);
  
  NIKOLA_LOG_INFO("Successfully initialized the renderer context");
}

void renderer_shutdown() {
  for(auto& entry : s_renderer.render_passes) {
    gfx_framebuffer_destroy(entry.frame);
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

const u32 renderer_push_pass(const RenderPassDesc& desc, const RenderPassFn& func) {
  RenderPass pass = {};

  // Framebuffer init

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
      .width     = (u32)desc.frame_size.x, 
      .height    = (u32)desc.frame_size.y, 
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

void renderer_pass_set_active(const u32& pass_id, const bool active) {
  bool is_valid_pass_id = (pass_id >= 0) && (pass_id < (u32)s_renderer.render_passes.size());
  NIKOLA_ASSERT(is_valid_pass_id, "Out of range pass id given to renderer_pass_set_active");

  s_renderer.render_passes[pass_id].is_active = active;
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

void renderer_queue_command(const RenderInstanceCommand& command) {
  NIKOLA_ASSERT(RESOURCE_IS_VALID(command.renderable_id), "Cannot render an invalid resource");

  Material* material = s_renderer.defaults.material;
  if(RESOURCE_IS_VALID(command.material_id)) {
    material = resources_get_material(command.material_id);
  }

  switch(command.type) {
    case RENDERABLE_MESH:
      s_renderer.instance_queue.emplace_back(resources_get_mesh(command.renderable_id), 
                                             command.transforms, 
                                             material, 
                                             command.instance_count);
      break;
    case RENDERABLE_MODEL:
      queue_model_instanced(resources_get_model(command.renderable_id), 
                            material, 
                            command.transforms, 
                            command.instance_count); 
      break;
    case RENDERABLE_DEBUG:
      break;
    default:
      NIKOLA_LOG_ERROR("Invalid or unsupported render command given... skiping");
      break;
  }
}

void renderer_flush_queue_command(const ResourceID& shader_context_id) {
  ResourceID ctx_id  = RESOURCE_IS_VALID(shader_context_id) ? shader_context_id : s_renderer.shader_contexts[SHADER_CONTEXT_BLINN];
  
  ShaderContext* default_ctx = resources_get_shader_context(ctx_id);
  ShaderContext* debug_ctx   = resources_get_shader_context(s_renderer.shader_contexts[SHADER_CONTEXT_DEFAULT]);
  ShaderContext* inst_ctx    = resources_get_shader_context(s_renderer.shader_contexts[SHADER_CONTEXT_INSTANCE]);

  // Flush the opaque queue

  for(auto& cmd : s_renderer.queues[RENDER_QUEUE_OPAQUE]) {
    // Setting and using shader uniforms 

    shader_context_set_uniform(default_ctx, "u_model", cmd.transform.transform);
    shader_context_use(default_ctx);

    // Using the internal material data

    material_use(cmd.material);  
    shader_context_set_uniform(default_ctx, "u_material.color", cmd.material->color);
    shader_context_set_uniform(default_ctx, "u_material.shininess", cmd.material->shininess);
    shader_context_set_uniform(default_ctx, "u_material.transparency", cmd.material->transparency);

    // Set pipeline-related flags from the material

    cmd.mesh->pipe_desc.depth_mask  = cmd.material->depth_mask;
    cmd.mesh->pipe_desc.stencil_ref = cmd.material->stencil_ref;
    gfx_pipeline_update(cmd.mesh->pipe, cmd.mesh->pipe_desc);

    // Draw the mesh

    gfx_pipeline_use(cmd.mesh->pipe);
    gfx_context_draw(s_renderer.context, 0);
  }

  // Flush the debug queue
  // @TODO (Renderer): Do we need this here?

  for(auto& cmd : s_renderer.queues[RENDER_QUEUE_DEBUG]) {
    // Setting and using shader uniforms 

    shader_context_set_uniform(debug_ctx, "u_model", cmd.transform.transform);
    shader_context_use(debug_ctx);

    // Using the internal material data

    shader_context_set_uniform(debug_ctx, "u_material.color", cmd.material->color);
    shader_context_set_uniform(debug_ctx, "u_material.transparency", cmd.material->transparency);

    // Draw the mesh

    gfx_pipeline_use(cmd.mesh->pipe);
    gfx_context_draw(s_renderer.context, 0);
  }

  // Flush the instance queue

  for(auto& cmd : s_renderer.instance_queue) {
    // Setting and using shader uniforms 

    shader_context_use(inst_ctx);

    // Using the internal material data

    material_use(cmd.material);  
    shader_context_set_uniform(inst_ctx, "u_material.color", cmd.material->color);
    shader_context_set_uniform(inst_ctx, "u_material.transparency", cmd.material->transparency);

    // Set pipeline-related flags from the material

    cmd.mesh->pipe_desc.depth_mask      = cmd.material->depth_mask;
    cmd.mesh->pipe_desc.stencil_ref     = cmd.material->stencil_ref;
    cmd.mesh->pipe_desc.instance_buffer = s_renderer.instance_buffer;

    gfx_pipeline_update(cmd.mesh->pipe, cmd.mesh->pipe_desc);

    // Update the instance buffer
    gfx_buffer_upload_data(s_renderer.instance_buffer, 0, sizeof(InstanceData) * cmd.instance_count, cmd.instance_data);

    // Draw the mesh

    gfx_pipeline_use(cmd.mesh->pipe);
    gfx_context_draw_instanced(s_renderer.context, 0, cmd.instance_count);
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
  
  for(sizei i = 1; i < s_renderer.render_passes.size(); i++) {
    RenderPass* entry = &s_renderer.render_passes[i];

    if(entry->is_active) {
      begin_pass(*entry);
      entry->pass_func(&s_renderer.render_passes[i - 1], entry, entry->user_data);
      end_pass(*entry);
    } 
  } 
  
  // Clear the queues
  
  for(sizei i = 0; i < RENDER_QUEUES_MAX; i++) {
    s_renderer.queues[i].clear();
  }

  s_renderer.instance_queue.clear();
}

/// Renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
