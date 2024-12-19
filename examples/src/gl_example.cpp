#include <nikol/nikol_core.hpp>
#include <stb/stb_image.h>

static nikol::GfxTextureDesc load_texture_from_file(const char* path) {
  nikol::GfxTextureDesc desc; 

  int width, height, channels;

  stbi_set_flip_vertically_on_load(true);
  nikol::u8* pixels = stbi_load(path, &width, &height, &channels, 4);

  desc.width     = width; 
  desc.height    = height;
  desc.depth     = 0; 
  desc.format    = nikol::GFX_TEXTURE_FORMAT_RGBA8; 
  desc.filter    = nikol::GFX_TEXTURE_FILTER_MIN_TRILINEAR_MAG_LINEAR;
  desc.wrap_mode = nikol::GFX_TEXTURE_WRAP_REPEAT;
  desc.data      = pixels;

  return desc;
}

int main() {
  // Initialze the library
  if(!nikol::init()) {
    return -1;
  }

  // Openinig the window
  nikol::i32 win_flags = nikol::WINDOW_FLAGS_FOCUS_ON_CREATE | nikol::WINDOW_FLAGS_GFX_HARDWARE;
  nikol::Window* window = nikol::window_open("Hello, Nikol", 1366, 768, win_flags);
  if(!window) {
    return -1;
  }

  // Creating a graphics context
  nikol::u32 gfx_flags =  nikol::GFX_FLAGS_BLEND | 
                          nikol::GFX_FLAGS_DEPTH | 
                          nikol::GFX_FLAGS_STENCIL | 
                          nikol::GFX_FLAGS_ENABLE_VSYNC;
  nikol::GfxContext* gfx = gfx_context_init(window, gfx_flags);
  if(!gfx) {
    return -1;
  }

  // Creating a shader and adding it to the draw call
  const char* src =
  "#version 460 core\n"
  "\n"
  "layout (location = 0) in vec3 a_pos;\n"
  "layout (location = 1) in vec2 a_texture_coords;\n"
  "layout (location = 2) in vec4 a_color;\n"
  "\n" 
  "out VS_OUT {\n"
  "   vec2 texture_coords;\n"
  "   vec4 vertex_color;\n"
  "} vs_out;\n"
  "\n" 
  "void main() {\n"
  "   gl_Position           = vec4(a_pos, 1.0);\n"
  "   vs_out.texture_coords = a_texture_coords;\n"
  "   vs_out.vertex_color   = a_color;\n"
  "}\n"
  "\n"
  "#version 460 core\n"
  "\n" 
  "layout (location = 0) out vec4 out_color;\n"
  "\n"
  "in VS_OUT {\n"
  "   vec2 texture_coords;\n"
  "   vec4 vertex_color;\n"
  "} fs_in;\n"
  "\n" 
  "uniform sampler2D container_texture;\n" 
  "\n" 
  "void main() {\n"
  "   out_color = texture(container_texture, fs_in.texture_coords);\n"
  "}\n";
  nikol::GfxShader* shader = nikol::gfx_shader_create(gfx, src);

  // Creating a vertex buffer and adding it to the draw call
  nikol::f32 vertices[] = {
    // Vertices         // Coords   // Color  
    -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
  };
  nikol::GfxBufferDesc vert_desc = {
    .data  = vertices, 
    .size  = sizeof(vertices), 
    .type  = nikol::GFX_BUFFER_VERTEX, 
    .usage = nikol::GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  nikol::GfxBuffer* vert_buff = nikol::gfx_buffer_create(gfx, vert_desc);

  // Creating an index buffer and adding it to the draw call
  nikol::u32 indices[] = {
    0, 1, 2, 
    2, 3, 0,
  };
  nikol::GfxBufferDesc index_desc = {
    .data             = indices, 
    .size             = sizeof(indices), 
    .type             = nikol::GFX_BUFFER_INDEX, 
    .usage            = nikol::GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  nikol::GfxBuffer* index_buff = nikol::gfx_buffer_create(gfx, index_desc);

  // Creating a texture and adding it to the draw call
  nikol::GfxTextureDesc texture_desc = load_texture_from_file("container.png");
  nikol::GfxTexture* container_texture = nikol::gfx_texture_create(gfx, texture_desc);
 
  nikol::GfxPipelineDesc desc = {};
  desc.vertex_buffer = vert_buff; 
  desc.vertices_count = 4;

  desc.index_buffer = index_buff; 
  desc.indices_count = 6; 

  desc.shader = shader; 

  desc.layout[0] = {"POS", nikol::GFX_LAYOUT_FLOAT3, 0};
  desc.layout[1] = {"TEX", nikol::GFX_LAYOUT_FLOAT2, 0};
  desc.layout[2] = {"COLOR", nikol::GFX_LAYOUT_FLOAT4, 0};
  desc.layout_count = 3;

  desc.draw_mode = nikol::GFX_DRAW_MODE_TRIANGLE, 

  desc.textures[0]   = container_texture;
  desc.texture_count = 1;

  nikol::GfxPipeline* pipeline = nikol::gfx_pipeline_create(gfx, desc);

  // Main loop
  while(nikol::window_is_open(window)) {
    // Will stop the application when ESCAPE is pressed
    if(nikol::input_key_pressed(nikol::KEY_ESCAPE)) {
      break;
    }
    
    // Clear the screen to a specified color
    nikol::gfx_context_clear(gfx, 0.3f, 0.3f, 0.3f, 1.0f);
    
    // Sumbit the draw call and render it to the screen
    nikol::gfx_pipeline_draw_index(gfx, pipeline, desc);

    // Swap the internal window buffer
    nikol::gfx_context_present(gfx); 

    // Poll the window events
    nikol::window_poll_events(window);
  }

  // De-initialze
  nikol::gfx_pipeline_destroy(pipeline);
  nikol::gfx_context_shutdown(gfx);
  nikol::window_close(window);
  nikol::shutdown();
}
