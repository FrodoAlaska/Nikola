#include <nikol_core.h>

int NIKOL_MAIN() {
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
  nikol::GfxContext* gfx = gfx_context_init(window, nikol::GFX_FLAGS_BLEND | nikol::GFX_FLAGS_DEPTH | nikol::GFX_FLAGS_STENCIL);
  if(!gfx) {
    return -1;
  }

  nikol::GfxPipelineDesc desc = {

  };

  // Creating a shader and adding it to the draw call
  const char* src =
  "#version 450 core\n"
  "\n"
  "layout (location = 0) in vec3 a_pos;\n"
  "layout (location = 1) in vec2 a_texture_coords;\n"
  "\n" 
  "out vec2 texture_coords;\n"
  "\n" 
  "void main() {\n"
  " gl_Position    = vec4(a_pos, 1.0);\n"
  " texture_coords = a_texture_coords;\n"
  "}\n"
  "\n"
  "#version 450 core\n"
  "\n" 
  "layout (location = 0) out vec4 out_color;\n"
  "\n" 
  "in vec2 texture_coords;\n"
  "\n" 
  "uniform sampler2D u_texture;\n"
  "\n" 
  "void main() {\n"
  " out_color = texture(u_texture, texture_coords);\n"
  "}\n";
  desc.shader = nikol::gfx_shader_create(src);

  // Creating a vertex buffer and adding it to the draw call
  nikol::f32 vertices[] = {
    -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
     0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
     0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
  };
  nikol::GfxBufferDesc vert_buff = {
    .data           = vertices, 
    .size           = sizeof(vertices), 
    .elements_count = 20, 
    .type = nikol::GFX_BUFFER_VERTEX, 
    .mode = nikol::GFX_BUFFER_MODE_STATIC_DRAW,
  };
  desc.vertex_buffer = &vert_buff;

  // Setting the layout of the vertex buffer
  desc.layout[0] = nikol::GFX_LAYOUT_FLOAT3;
  desc.layout[1] = nikol::GFX_LAYOUT_FLOAT2;
  desc.layout_count = 2;

  // Creating an index buffer and adding it to the draw call
  nikol::u32 indices[] = {
    0, 1, 2, 
    2, 3, 0,
  };
  nikol::GfxBufferDesc index_buff = {
    .data             = indices, 
    .size             = sizeof(indices), 
    .elements_count   = 6, 
    .type             = nikol::GFX_BUFFER_INDEX, 
    .mode             = nikol::GFX_BUFFER_MODE_STATIC_DRAW,
  };
  desc.index_buffer = &index_buff;

  // Creating a texture and adding it to the draw call
  nikol::u32 pixels = 0xaaffaaff;
  nikol::GfxTextureDesc texture = {
    .width    = 1, 
    .height   = 1, 
    .channels = 4, 
    .depth    = 0, 
    .format   = nikol::GFX_TEXTURE_FORMAT_RGBA, 
    .filter   = nikol::GFX_TEXTURE_FILTER_LINEAR, 
    .data     = &pixels,
  };
  desc.textures[0] = &texture;
  desc.texture_count++;
  
  nikol::GfxPipeline* pipeline = nikol::gfx_pipeline_create(gfx, desc);

  // Main loop
  while(nikol::window_is_open(window)) {
    // Will stop the application when F1 is pressed
    if(nikol::input_key_pressed(nikol::KEY_F1)) {
      break;
    }
    
    // Clear the screen to black
    nikol::gfx_context_clear(gfx, 0.3f, 0.3f, 0.3f, 1.0f);
    
    // Begin the drawing pass
    nikol::gfx_pipeline_begin(gfx, pipeline);

    // Sumbit the draw call and render it to the screen
    nikol::gfx_pipeline_draw_index(gfx, pipeline, &desc);

    // Poll the window events
    nikol::window_poll_events(window);
    
    // Swap the internal window buffer
    nikol::window_swap_buffers(window);
  }

  // De-initialze
  nikol::gfx_pipeline_destroy(pipeline);
  nikol::gfx_context_shutdown(gfx);
  nikol::window_close(window);
  nikol::shutdown();
}
