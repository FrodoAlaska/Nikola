#include <nikol_core.hpp>

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
  nikol::GfxContext* gfx = gfx_context_init(window, nikol::GFX_FLAGS_BLEND | nikol::GFX_FLAGS_DEPTH | nikol::GFX_FLAGS_STENCIL | nikol::GFX_FLAGS_ENABLE_VSYNC);
  if(!gfx) {
    return -1;
  }

  nikol::GfxPipelineDesc desc;

  // Creating a shader and adding it to the draw call
  const char* src =
  "#version 460 core\n"
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
  "#version 460 core\n"
  "\n" 
  "layout (location = 0) out vec4 out_color;\n"
  "\n" 
  "in vec2 texture_coords;\n"
  "\n" 
  "uniform sampler2D u_texture;\n"
  "uniform vec4 u_color;\n"
  "\n" 
  "void main() {\n"
  " out_color = u_color;//texture(u_texture, texture_coords);"
  "\n}\n";
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
    .usage = nikol::GFX_BUFFER_USAGE_STATIC_DRAW,
    .draw_mode = nikol::GFX_BUFFER_MODE_TRIANGLE,
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
    .usage            = nikol::GFX_BUFFER_USAGE_STATIC_DRAW,
    .draw_mode        = nikol::GFX_BUFFER_MODE_TRIANGLE,
  };
  desc.index_buffer = &index_buff;

  // Creating a texture and adding it to the draw call
  nikol::u32 pixels = 0xaaffaaff;
  const nikol::GfxTextureDesc texture = {
    .width    = 1, 
    .height   = 1, 
    .channels = 4, 
    .depth    = 0, 
    .format   = nikol::GFX_TEXTURE_FORMAT_RGBA8, 
    .min_filter   = nikol::GFX_TEXTURE_FILTER_TRILINEAR,
    .mag_filter   = nikol::GFX_TEXTURE_FILTER_LINEAR,
    .wrap_mode    = nikol::GFX_TEXTURE_WRAP_REPEAT,
    .data     = &pixels,
  };
  nikol::GfxTexture* white_texture = nikol::gfx_texture_create(texture);
  desc.textures[0] = white_texture;
  desc.texture_count++;
  
  nikol::GfxPipeline* pipeline = nikol::gfx_pipeline_create(gfx, desc);

  nikol::i32 color_loc = nikol::gfx_shader_get_uniform_location(desc.shader, "u_color");
  nikol::f32 color[4] = {1.0f, 0.0f, 0.0f, 1.0f}; 

  // Main loop
  while(nikol::window_is_open(window)) {
    // Will stop the application when ESCAPE is pressed
    if(nikol::input_key_pressed(nikol::KEY_ESCAPE)) {
      break;
    }
    
    // Clear the screen to black
    nikol::gfx_context_clear(gfx, 0.3f, 0.3f, 0.3f, 1.0f);
    
    // Begin the drawing pass
    nikol::gfx_pipeline_begin(gfx, pipeline);
   
    // Uploading uniform data to the shader
    const nikol::GfxUniformDesc uni_desc = {
      .type = nikol::GFX_UNIFORM_TYPE_VEC4, 
      .location = color_loc,
      .data = &color,
      .count = 1,
    };
    nikol::gfx_shader_upload_uniform(desc.shader,  uni_desc);

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
