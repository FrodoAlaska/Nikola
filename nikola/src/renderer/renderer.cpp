#include "nikola/nikola_render.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_gfx.h"
#include "nikola/nikola_math.h"
#include "nikola/nikola_physics.h"

#include "render_passes/render_passes.h"

#include "shaders/skybox.glsl.h"
#include "shaders/screen_space.glsl.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// MatrixUniformBuffer
struct MatrixUniformBuffer {
  Mat4 view, projection; 
  Vec3 camera_position;
};
/// MatrixUniformBuffer
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Renderer
struct Renderer {
  // Context data

  GfxContext* context = nullptr;
  Vec4 clear_color;

  // Defaults data

  RendererDefaults defaults = {};
  
  GfxShader* screen_space_shader;
  GfxShader* skybox_shader;

  // Render data

  MatrixUniformBuffer matrix_uniform_buffer;
  
  FrameData* frame_data;

  DynamicArray<RenderPass> passes_pool;
  RenderPass* head_pass = nullptr; 
  RenderPass* tail_pass = nullptr;

  DynamicArray<GeometryPrimitive> queues[RENDER_QUEUES_MAX];
};

static Renderer s_renderer{};
/// Renderer
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static void init_defaults() {
  // Default textures init
  
  ResourceID default_texture_id = resources_push_texture(RESOURCE_CACHE_ID, MATERIAL_TEXTURE_DIFFUSE);
  s_renderer.defaults.texture   = resources_get_texture(default_texture_id);

  ResourceID specular_default_texture_id = resources_push_texture(RESOURCE_CACHE_ID, MATERIAL_TEXTURE_SPECULAR);
  s_renderer.defaults.specular_texture   = resources_get_texture(specular_default_texture_id);
 
  ResourceID normal_default_texture_id = resources_push_texture(RESOURCE_CACHE_ID, MATERIAL_TEXTURE_NORMAL);
  s_renderer.defaults.normal_texture   = resources_get_texture(normal_default_texture_id);

  // Matrices buffer init
  
  GfxBufferDesc buff_desc = {
    .data  = nullptr, 
    .size  = sizeof(MatrixUniformBuffer),
    .type  = GFX_BUFFER_UNIFORM,
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  s_renderer.defaults.matrices_buffer = resources_get_buffer(resources_push_buffer(RESOURCE_CACHE_ID, buff_desc));

  // Material init
  MaterialDesc mat_desc = {
    .diffuse_id  = default_texture_id, 
    .specular_id = specular_default_texture_id, 
    .normal_id   = normal_default_texture_id,
  };
  s_renderer.defaults.material = resources_get_material(resources_push_material(RESOURCE_CACHE_ID, mat_desc));

  // Cube mesh init
  s_renderer.defaults.cube_mesh = resources_get_mesh(resources_push_mesh(RESOURCE_CACHE_ID, GEOMETRY_CUBE));

  // Shaders init
  
  ResourceID skybox_shader_id       = resources_push_shader(RESOURCE_CACHE_ID, generate_skybox_shader());
  ResourceID screen_space_shader_id = resources_push_shader(RESOURCE_CACHE_ID, generate_screen_space_shader());

  s_renderer.skybox_shader       = resources_get_shader(skybox_shader_id);
  s_renderer.screen_space_shader = resources_get_shader(screen_space_shader_id);

  // Instance buffer init
  
  GfxBufferDesc inst_buff_desc = {
    .data  = nullptr,
    .size  = sizeof(Mat4) * RENDERER_MAX_INSTANCES,
    .type  = GFX_BUFFER_UNIFORM, 
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  s_renderer.defaults.instance_buffer = resources_get_buffer(resources_push_buffer(RESOURCE_CACHE_ID, inst_buff_desc));
}

static void init_pipeline() {
  GfxPipelineDesc pipe_desc = {};

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
  pipe_desc.vertex_buffer  = gfx_buffer_create(s_renderer.context, vert_desc);
  pipe_desc.vertices_count = 4;
 
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
  pipe_desc.index_buffer  = gfx_buffer_create(s_renderer.context, index_desc);
  pipe_desc.indices_count = 6;

  // Layout init
  pipe_desc.layouts[0].attributes[0]    = GFX_LAYOUT_FLOAT2;
  pipe_desc.layouts[0].attributes[1]    = GFX_LAYOUT_FLOAT2;
  pipe_desc.layouts[0].attributes_count = 2;

  // Draw mode init 
  pipe_desc.draw_mode = GFX_DRAW_MODE_TRIANGLE;

  // Pipeline init
  s_renderer.defaults.screen_quad = gfx_pipeline_create(s_renderer.context, pipe_desc);
}

static void queue_model(Model* model, Material* material, const Transform* transforms, const sizei count) {
  for(sizei i = 0; i < model->meshes.size(); i++) {
    Mesh* mesh    = model->meshes[i];
    Material* mat = model->materials[model->material_indices[i]];

    // Let the main given material "influence" the model's material 

    mat->shininess    = material->shininess;
    mat->transparency = material->transparency;
    mat->depth_mask   = material->depth_mask;

    // @TODO(Renderer): Have a transform parent-child relationship

    s_renderer.queues[RENDER_QUEUE_OPAQUE].emplace_back(transforms, mesh->pipe, mat, count);
  }  
}

/// Private functions
/// ----------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Renderer functions

void renderer_init(Window* window) {
  // Context init 
  
  GfxContextDesc gfx_desc = {
    .window       = window,
    .states       = GFX_STATE_DEPTH | GFX_STATE_STENCIL | GFX_STATE_BLEND | GFX_STATE_MSAA,
    .has_vsync    = false,
  };
  
  s_renderer.context = gfx_context_init(gfx_desc);
  NIKOLA_ASSERT(s_renderer.context, "Failed to initialize the graphics context");

  // Performance boost?
  s_renderer.passes_pool.reserve(4);

  // Defaults init
  
  init_defaults();
  init_pipeline();

  // Defaults render passes init
  
  shadow_pass_init(window);
  light_pass_init(window);
  billboard_pass_init(window);
  hdr_pass_init(window);

  // Batch renderer init
  batch_renderer_init();

  NIKOLA_LOG_INFO("Successfully initialized the renderer context");
}

void renderer_shutdown() {
  batch_renderer_shutdown();
  
  for(auto& pass : s_renderer.passes_pool) {
    gfx_framebuffer_destroy(pass.framebuffer);
  }
  s_renderer.passes_pool.clear();

  gfx_pipeline_destroy(s_renderer.defaults.screen_quad);
  gfx_context_shutdown(s_renderer.context);
  
  NIKOLA_LOG_INFO("Successfully shutdown the renderer context");
}

void renderer_begin(FrameData& data) {
  GfxBuffer* matrix_buffer = s_renderer.defaults.matrices_buffer;
  s_renderer.frame_data    = &data;
   
  // Updating the internal matrices buffer for each shader

  s_renderer.matrix_uniform_buffer = {
    .view            = data.camera.view, 
    .projection      = data.camera.projection, 
    .camera_position = data.camera.position,
  };
  gfx_buffer_upload_data(matrix_buffer, 0, sizeof(MatrixUniformBuffer), &s_renderer.matrix_uniform_buffer);
}

void renderer_end() {
  // Initiate all of the render passes in order
  
  RenderPass* current = s_renderer.head_pass;
  while(current) {
    // Prepare the context for the render pass

    gfx_context_set_target(current->gfx, current->framebuffer);
    gfx_context_set_viewport(current->gfx, 0, 0, current->frame_size.x, current->frame_size.y);

    Vec4 col = s_renderer.clear_color;
    gfx_context_clear(current->gfx, col.r, col.g, col.b, col.a);

    // Initiating the render pass callbacks

    if(current->prepare_func) {
      current->prepare_func(current, *s_renderer.frame_data);
    }

    current->sumbit_func(current, s_renderer.queues[current->queue_type]);
    
    // Advance to the next pass if it exists
    
    if(!current->next) {
      break;
    } 
    
    current = current->next;
  }
  
  // Render back to the default framebuffer
  
  gfx_context_set_target(s_renderer.context, nullptr);
  gfx_context_clear(s_renderer.context, 0.0f, 0.0f, 0.0f, 0.0f);

  GfxBindingDesc bind_desc = {
    .shader = s_renderer.screen_space_shader, 

    .textures       = current->outputs, 
    .textures_count = current->outputs_count,
  };
  gfx_context_use_bindings(s_renderer.context, bind_desc);
  
  // Draw the final result to the screen
  
  gfx_context_use_pipeline(s_renderer.context, s_renderer.defaults.screen_quad);
  gfx_context_draw(s_renderer.context, 0);
  
  // Clear the queues for the new frame
  
  for(sizei i = 0; i < RENDER_QUEUES_MAX; i++) {
    s_renderer.queues[i].clear();
  }
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

RenderPass* renderer_create_pass(const RenderPassDesc& desc) {
  // Allocate the pass

  s_renderer.passes_pool.push_back(RenderPass{});

  RenderPass* pass = &s_renderer.passes_pool[s_renderer.passes_pool.size() - 1];
  pass->gfx        = s_renderer.context; 

  // Callbacks init

  pass->prepare_func = desc.prepare_func;
  pass->sumbit_func  = desc.sumbit_func;
  pass->queue_type   = desc.queue_type;
  
  // Render targets init

  pass->frame_size = desc.frame_size;
  
  for(auto& target : desc.targets) {
    ResourceID tex_id   = resources_push_texture(desc.res_group_id, target);
    GfxTexture* texture = resources_get_texture(tex_id);

    switch(target.type) {
      case GFX_TEXTURE_DEPTH_TARGET:
      case GFX_TEXTURE_DEPTH_STENCIL_TARGET:
        pass->framebuffer_desc.depth_attachment = texture;
        break;
      case GFX_TEXTURE_STENCIL_TARGET:
        pass->framebuffer_desc.stencil_attachment = texture;
        break;
      default:
        pass->framebuffer_desc.color_attachments[pass->framebuffer_desc.attachments_count] = texture;
        pass->framebuffer_desc.attachments_count++;
        break;
    }
  }

  // Framebuffer init 
  
  pass->framebuffer_desc.clear_flags = desc.clear_flags; 
  pass->framebuffer                  = gfx_framebuffer_create(pass->gfx, pass->framebuffer_desc);
  pass->user_data                    = desc.user_data;

  // Retrieve the context
  pass->shader_context = resources_get_shader_context(desc.shader_context_id);

  return pass;
}

void renderer_append_pass(RenderPass* pass) {
  NIKOLA_ASSERT(pass, "Invalid RenderPass passed to renderer_append_pass");

  // The first pass. Make it the head.
  if(!s_renderer.head_pass) {
    s_renderer.head_pass = pass; 
    
    s_renderer.head_pass->previous = nullptr;
    s_renderer.head_pass->next     = s_renderer.tail_pass;

    return;
  }

  // Perhaps there is no tail? Make it the tail if so
  if(!s_renderer.tail_pass) {
    s_renderer.tail_pass = pass; 

    s_renderer.tail_pass->previous = s_renderer.head_pass;
    s_renderer.tail_pass->next     = nullptr;

    s_renderer.head_pass->next = s_renderer.tail_pass;

    return;
  }

  // Otherwise, append to the tail, 

  s_renderer.tail_pass->next = pass; 
  
  pass->previous = s_renderer.tail_pass;
  pass->next     = nullptr;

  s_renderer.tail_pass = pass;
}

void renderer_prepend_pass(RenderPass* pass) {
  NIKOLA_ASSERT(pass, "Invalid RenderPass passed to renderer_prepend_pass");

  // There is no head, so make it the head. 
  if(!s_renderer.head_pass) {
    s_renderer.head_pass = pass; 
    
    s_renderer.head_pass->previous = nullptr;
    s_renderer.head_pass->next     = s_renderer.tail_pass;

    return;
  }

  // Otherwise, prepend to the head
  
  s_renderer.head_pass->previous = pass;

  pass->next     = s_renderer.head_pass;
  pass->previous = nullptr;

  pass = s_renderer.head_pass;
}

void renderer_insert_pass(RenderPass* pass, const sizei index) {
  NIKOLA_ASSERT(pass, "Invalid RenderPass passed to renderer_insert_pass");
  NIKOLA_ASSERT(index > 0, "Out-of-bounds render pass insertion");

  sizei counter       = 0;
  RenderPass* current = s_renderer.head_pass; 

  // Iterate until you find the index or we run out 
  // of render passes.
  while(counter < index || current) {
    current = current->next;
    counter++;
  }

  // Do a cool insertion

  current->next->previous = pass;
  pass->next              = current->next;

  current->next  = pass;
  pass->previous = current;
}

RenderPass* renderer_peek_pass(const sizei index) {
  NIKOLA_ASSERT((index >= 0) && (index < s_renderer.passes_pool.size()), "Out-of-bounds render pass peek");
  return &s_renderer.passes_pool[index];
}

void renderer_queue_command_instanced(const RenderableType& type,    
                                      const ResourceID& res_id, 
                                      const Transform* transforms, 
                                      const sizei count, 
                                      const ResourceID& mat_id) {
  Material* material = s_renderer.defaults.material;
  if(RESOURCE_IS_VALID(mat_id)) {
    material = resources_get_material(mat_id);
  }

  switch(type) {
    case RENDERABLE_MESH:
      s_renderer.queues[RENDER_QUEUE_OPAQUE].emplace_back(transforms, 
                                                          resources_get_mesh(res_id)->pipe, 
                                                          material, 
                                                          count);
      break;
    case RENDERABLE_MODEL:
      queue_model(resources_get_model(res_id), material, transforms, count); 
      break;
    case RENDERABLE_BILLBOARD:
      s_renderer.queues[RENDER_QUEUE_BILLBOARD].emplace_back(transforms, 
                                                             resources_get_mesh(res_id)->pipe, 
                                                             material, 
                                                             count);
      break;
    default:
      NIKOLA_LOG_ERROR("Invalid or unsupported render command given... skiping");
      break;
  }
}

void renderer_queue_command(const RenderableType& type, 
                            const ResourceID& res_id, 
                            const Transform& transform, 
                            const ResourceID& mat_id) {
  renderer_queue_command_instanced(type, res_id, &transform, 1, mat_id);
}

void renderer_draw_geometry_primitive(const GeometryPrimitive& geo) {
  // Update the instance buffer
  gfx_buffer_upload_data(s_renderer.defaults.instance_buffer,
                         0, 
                         sizeof(Mat4) * geo.instance_count, 
                         geo.transforms);

  // Set pipeline-related flags from the material

  GfxPipelineDesc pipe_desc = gfx_pipeline_get_desc(geo.pipeline);
  pipe_desc.depth_mask      = geo.material->depth_mask;
  pipe_desc.stencil_ref     = geo.material->stencil_ref;
  pipe_desc.instance_buffer = s_renderer.defaults.instance_buffer;
  
  gfx_pipeline_update(geo.pipeline, pipe_desc);

  // Draw the mesh
  
  gfx_context_use_pipeline(s_renderer.context, geo.pipeline);

  if(geo.instance_count > 1) {
    gfx_context_draw_instanced(s_renderer.context, 0, geo.instance_count);
  }
  else {
    gfx_context_draw(s_renderer.context, 0);
  }
}

void renderer_draw_skybox(const ResourceID& skybox_id) {
  Skybox* skybox = resources_get_skybox(skybox_id); 

  // Using the resources 
  
  GfxBindingDesc bind_desc = {
    .shader = s_renderer.skybox_shader, 

    .cubemaps       = &skybox->cubemap, 
    .cubemaps_count = 1,
  };
  gfx_context_use_bindings(s_renderer.context, bind_desc);

  // Draw the skybox
  
  gfx_context_use_pipeline(s_renderer.context, skybox->pipe);
  gfx_context_draw(s_renderer.context, 0);
}

/// Renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
