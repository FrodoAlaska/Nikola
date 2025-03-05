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

  GfxFramebufferDesc frame_desc = {};
  GfxFramebuffer* framebuffer   = nullptr;
  
  GfxPipelineDesc pipe_desc  = {};
  GfxPipeline* pipeline      = nullptr; 

  i32 effects_uniform_loc   = -1;
  u32 effect_value          = 0;
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

static GfxShaderDesc init_post_processing_shader() {
  GfxShaderDesc desc = {};

  desc.vertex_source = 
    "#version 460 core\n"
    "\n" 
    "// Layouts\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec2 aTextureCoords;\n"
    "\n" 
    "// Outputs\n"
    "out VS_OUT {\n"
    "  vec2 vertex_position;\n"
    "  vec2 tex_coords;\n"
    "} vs_out;\n"
    "\n" 
    "void main() {\n"
    "  vs_out.vertex_position = aPos;\n"
    "  vs_out.tex_coords = aTextureCoords;\n"
    "\n"
    "  gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "}"
    "\n";

  desc.pixel_source = 
    "#version 460 core\n"
    "\n" 
    "// Outputs\n"
    "layout (location = 0) out vec4 frag_color;\n"
    "\n" 
    "// Inputs\n"
    "in VS_OUT {\n"
    "  vec2 vertex_position;\n"
    "  vec2 tex_coords;\n"
    "} fs_in;\n"
    "\n" 
    "// Uniforms\n"
    "uniform sampler2D u_texture;\n"
    "uniform int u_effect_index;"
    "\n" 
    "// Functions\n"
    "vec4 greyscale();\n"
    "vec4 inversion();\n"
    "vec3 kernel(float[9] kernel_values);\n"
    "vec4 sharpen();\n"
    "vec4 blur();\n"
    "vec4 edge_detect();\n"
    "vec4 emboss();\n"
    "vec4 pixelize();\n"
    "\n" 
    "void main() {\n"
    "  vec4 effects[] = {\n"
    "    texture(u_texture, fs_in.tex_coords),"
    "    greyscale(), \n"
    "    inversion(), \n"
    "    sharpen(), \n"
    "    blur(), \n"
    "    emboss(), \n"
    "    edge_detect(),\n"
    "    pixelize(),\n"
    "  };\n"
    ""
    "  frag_color = effects[u_effect_index];\n"
    "};\n"
    "" 
    "vec4 greyscale() {\n"
    "  vec4 col = texture(u_texture, fs_in.tex_coords);\n"
    "  float average = (0.2126f * col.r) + (0.7152f * col.g) + (0.0722f * col.b);\n"
    "  return vec4(average, average, average, 1.0);\n"
    "}\n"
    "" 
    "vec4 inversion() {\n" 
    "  return vec4(vec3(1.0 - texture(u_texture, fs_in.tex_coords)), 1.0);\n" 
    "}\n"
    "" 
    "vec3 kernel(float[9] kernel_values) {\n"
    "  const float offset = 1 / 300.0f;\n"
    "  vec2 offsets[9] = vec2[](\n"
    "  vec2(-offset, offset), // Top-left\n"
    "  vec2(0.0f, offset),    // Top-center\n"
    "  vec2(offset, offset),  // Top-right\n"
    "" 
    "  vec2(-offset, 0.0f),   // Center-left\n"
    "  vec2(0.0f, 0.0f),      // Center\n"
    "  vec2(offset, 0.0f),    // Center-right\n"
    "" 
    "  vec2(-offset, -offset), // Bottom-left\n"
    "  vec2(0.0f, -offset),    // Bottom-center\n"
    "  vec2(offset, -offset)   // Bottom-right\n"
    "  );\n"
    "" 
    "  vec3 sample_tex[9];\n"
    "  for(int i = 0; i < 9; i++) {\n"
    "    sample_tex[i] = vec3(texture(u_texture, fs_in.tex_coords.st + offsets[i]));\n"
    "  }\n"
    "" 
    "  vec3 color = vec3(0.0f);\n"
    "  for(int i = 0; i < 9; i++) {\n"
    "    color += sample_tex[i] * kernel_values[i];\n"
    "  }\n"
    "  return color;\n"
    "}\n"
    "" 
    "vec4 sharpen() {\n"
    "  float k[9] = float[](\n"
    "    -1.0f, -1.0f, -1.0f, \n"
    "    -1.0f,  9.0f, -1.0f,\n"
    "    -1.0f, -1.0f, -1.0f\n"
    "  );\n"
    "  return vec4(kernel(k), 1.0f);\n"
    "}\n" 
    ""
    "vec4 blur() {\n"
    "  const float mul = 16.0f;\n"
    "  float k[9] = float[](\n"
    "    1.0f / mul, 2.0f / mul, 1.0f / mul,\n"
    "    2.0f / mul, 4.0f / mul, 2.0f / mul,\n"
    "    1.0f / mul, 2.0f / mul, 1.0f / mul \n"
    "  );\n"
    "  return vec4(kernel(k), 1.0f);\n"
    "}\n"
    "" 
    "vec4 edge_detect() {\n"
    "  float k[9] = float[](\n"
    "    1.0f,  1.0f, 1.0f, \n"
    "    1.0f, -8.0f, 1.0f, \n"
    "    1.0f,  1.0f, 1.0f  \n"
    "  );\n"
    "  return vec4(kernel(k), 1.0f);\n"
    "}\n"
    ""
    "vec4 emboss() {\n"
    "  float k[9] = float[](\n"
    "   -2.0f, -1.0f, 0.0f, \n"
    "   -1.0f,  1.0f, 1.0f, \n"
    "    0.0f,  1.0f, 2.0f  \n"
    "  );\n"
    "  return vec4(kernel(k), 1.0f);\n"
    "}\n"
    ""
    "vec4 pixelize() {\n"
    "  vec2 screen = vec2(1366, 720);\n"
    "  float pixels   = 16.0;\n"
    ""
    "  vec2 tex_coord = vec2(\n"
    "    pixels * (1.0 / screen.x),\n"
    "    pixels * (1.0 / screen.y)\n"
    "  );\n"
    ""
    "  float coord_x = tex_coord.x * floor(fs_in.tex_coords.x / tex_coord.x);\n"
    "  float coord_y = tex_coord.y * floor(fs_in.tex_coords.y / tex_coord.y);\n"
    ""
    "  vec3 color = texture(u_texture, vec2(coord_x, coord_y)).rgb;\n"
    ""
    "  return vec4(color, 1.0);\n"
    "}\n"
    "\n";

  return desc;
}

static void init_framebuffer(Window* window) {
  s_renderer.frame_desc = {}; 

  // Clear color init
  s_renderer.frame_desc.clear_color[0] = 0.1f;
  s_renderer.frame_desc.clear_color[1] = 0.1f;
  s_renderer.frame_desc.clear_color[2] = 0.1f;
  s_renderer.frame_desc.clear_color[3] = 1.0f;

  // Clear flags init
  s_renderer.frame_desc.clear_flags = GFX_CLEAR_FLAGS_COLOR_BUFFER | GFX_CLEAR_FLAGS_DEPTH_BUFFER;

  // Render target init
  i32 width, height; 
  window_get_size(window, &width, &height);
  
  GfxTextureDesc texture_desc = {
    .width     = (u32)width, 
    .height    = (u32)height, 
    .depth     = 0, 
    .mips      = 1, 
    .type      = GFX_TEXTURE_RENDER_TARGET,
    .format    = GFX_TEXTURE_FORMAT_RGBA8, 
    .filter    = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
    .wrap_mode = GFX_TEXTURE_WRAP_MIRROR, 
    .data      = nullptr,
  };
  s_renderer.frame_desc.attachments[0] = gfx_texture_create(s_renderer.context, texture_desc);
  s_renderer.frame_desc.attachments_count++;

  // Depth-Stencil texture init
  texture_desc = {
    .width     = (u32)width, 
    .height    = (u32)height, 
    .depth     = 0, 
    .mips      = 1, 
    .type      = GFX_TEXTURE_DEPTH_STENCIL_TARGET,
    .format    = GFX_TEXTURE_FORMAT_DEPTH_STENCIL_24_8, 
    .filter    = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
    .wrap_mode = GFX_TEXTURE_WRAP_CLAMP, 
    .data      = nullptr,
  };
  s_renderer.frame_desc.attachments[1] = gfx_texture_create(s_renderer.context, texture_desc);
  s_renderer.frame_desc.attachments_count++;
  
  // Framebuffer init
  s_renderer.framebuffer = gfx_framebuffer_create(s_renderer.context, s_renderer.frame_desc);
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

  // Shader init
  GfxShaderDesc shader_desc    = init_post_processing_shader();
  s_renderer.pipe_desc.shader  = gfx_shader_create(s_renderer.context, shader_desc); 

  // Uniform location init
  s_renderer.effects_uniform_loc = gfx_shader_uniform_lookup(s_renderer.pipe_desc.shader, "u_effect_index");

  // Layout init
  s_renderer.pipe_desc.layout[0]     = GfxLayoutDesc{"POS", GFX_LAYOUT_FLOAT2, 0};
  s_renderer.pipe_desc.layout[1]     = GfxLayoutDesc{"TEX", GFX_LAYOUT_FLOAT2, 0};
  s_renderer.pipe_desc.layout_count  = 2;

  // Draw mode init 
  s_renderer.pipe_desc.draw_mode = GFX_DRAW_MODE_TRIANGLE;

  // Textures init
  s_renderer.pipe_desc.textures[0] = s_renderer.frame_desc.attachments[0];
  s_renderer.pipe_desc.textures_count++;

  // Pipeline init
  s_renderer.pipeline = gfx_pipeline_create(s_renderer.context, s_renderer.pipe_desc);
}

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

static void flush_queue() {
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

static i32 get_effect_value(const RenderEffectType effect) {
  switch (effect) {
    case RENDER_EFFECT_NONE:
      return 0;
    case RENDER_EFFECT_GREYSCALE:
      return 1; 
    case RENDER_EFFECT_INVERSION:
      return 2; 
    case RENDER_EFFECT_SHARPEN:
      return 3; 
    case RENDER_EFFECT_BLUR:
      return 4; 
    case RENDER_EFFECT_EMBOSS:
      return 5; 
    case RENDER_EFFECT_EDGE_DETECT:
      return 6; 
    case RENDER_EFFECT_PIXELIZE:
      return 7; 
    default:
      return -1;
  }
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Renderer functions

void renderer_init(Window* window, const Vec4& clear_clear) {
  // Context init 
  init_context(window);
  s_renderer.clear_color = clear_clear;

  // Framebuffer init
  init_framebuffer(window);

  // Pipeline init
  init_pipeline();

  // Matrices buffer init
  GfxBufferDesc buff_desc = {
    .data  = nullptr, 
    .size  = sizeof(Mat4) * 2,
    .type  = GFX_BUFFER_UNIFORM,
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  s_renderer.matrices_buffer = gfx_buffer_create(s_renderer.context, buff_desc);

  NIKOLA_LOG_INFO("Successfully initialized the renderer context");
}

void renderer_shutdown() {
  gfx_pipeline_destroy(s_renderer.pipeline);
  gfx_framebuffer_destroy(s_renderer.framebuffer);
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

void renderer_begin_pass(RenderData& data) {
  // Updating the internal matrices buffer for each shader
  s_renderer.camera = data.camera;
  gfx_buffer_update(s_renderer.matrices_buffer, 0, sizeof(Mat4), mat4_raw_data(data.camera.view));
  gfx_buffer_update(s_renderer.matrices_buffer, sizeof(Mat4), sizeof(Mat4), mat4_raw_data(data.camera.projection));

  // Clear the post-processing framebuffer
  Vec4 col = s_renderer.clear_color;
  gfx_context_clear(s_renderer.context, s_renderer.framebuffer);
  gfx_context_set_state(s_renderer.context, GFX_STATE_DEPTH, true); 
}

void renderer_end_pass() {
  // Render everything in the queue
  flush_queue();

  // Render to the default framebuffer
  gfx_context_clear(s_renderer.context, nullptr);
  gfx_context_set_state(s_renderer.context, GFX_STATE_DEPTH, false); 
 
  // Send the correct effects index to the post-processing shader
  gfx_shader_upload_uniform(s_renderer.pipe_desc.shader, s_renderer.effects_uniform_loc, GFX_LAYOUT_INT1, &s_renderer.effect_value);

  // Render the final render target
  gfx_context_apply_pipeline(s_renderer.context, s_renderer.pipeline, s_renderer.pipe_desc);
  gfx_pipeline_draw_index(s_renderer.pipeline);
}

void renderer_present() {
  gfx_context_present(s_renderer.context);
}

void renderer_apply_effect(const RenderEffectType effect) {
  s_renderer.effect_value = get_effect_value(effect);
}

RenderEffectType renderer_current_effect() {
  RenderEffectType effects[] = {
    RENDER_EFFECT_NONE,
    RENDER_EFFECT_GREYSCALE,
    RENDER_EFFECT_INVERSION,
    RENDER_EFFECT_SHARPEN,
    RENDER_EFFECT_BLUR,
    RENDER_EFFECT_EMBOSS,
    RENDER_EFFECT_EDGE_DETECT,
    RENDER_EFFECT_PIXELIZE,
  };

  return effects[s_renderer.effect_value];
}

void renderer_queue_command(const RenderCommand& command) {
  s_renderer.render_queue.push_back(command);
}

/// Renderer functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
