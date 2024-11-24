#include <nikol_core.h>

int main(int argc, char** argv) {
  // Initialze 
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
  nikol::GfxContext* gfx = gfx_context_create(window, nikol::GFX_FLAGS_BLEND | nikol::GFX_FLAGS_DEPTH | nikol::GFX_FLAGS_STENCIL);
  if(!gfx) {
    return -1;
  }

  nikol::GfxDrawCall* call = nikol::gfx_draw_call_create(gfx);
  
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
  "\n" 
  "void main() {\n"
  " out_color = texture(u_texture, texture_coords);\n"
  "}\n";
  nikol::GfxShader* shader = nikol::gfx_shader_create(src);
  nikol::gfx_draw_call_push_shader(call, gfx, shader);

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
  nikol::gfx_draw_call_push_buffer(call, gfx, &vert_buff);

  nikol::GfxBufferLayout layout[] = {
    nikol::GFX_LAYOUT_FLOAT3,
    nikol::GFX_LAYOUT_FLOAT2,
  };
  nikol::gfx_draw_call_set_layout(call, gfx, &vert_buff, layout, 2);

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
  nikol::gfx_draw_call_push_buffer(call, gfx, &index_buff);

  nikol::u32 pixels = 0xff00ffff;
  nikol::GfxTextureDesc texture = {
    .width    = 1, 
    .height   = 1, 
    .channels = 4, 
    .depth    = 0, 
    .format   = nikol::GFX_TEXTURE_FORMAT_RGBA, 
    .filter   = nikol::GFX_TEXTURE_FILTER_LINEAR, 
    .data     = &pixels,
  };
  nikol::gfx_draw_call_push_texture(call, gfx, &texture);

  // nikol::u32 color_loc = nikol::gfx_shader_get_uniform_location(shader, "u_color");
  // float color[] = {1.0f, 0.0f, 0.0f, 1.0f};

  // Main loop
  while(nikol::window_is_open(window)) {
    if(nikol::input_key_pressed(nikol::KEY_F1)) {
      break;
    }

    nikol::gfx_context_clear(gfx, 0.1f, 0.1f, 0.1f, 1.0f);
    nikol::gfx_context_sumbit_begin(gfx, call);

    // nikol::gfx_shader_set_uniform_data(shader, color_loc, nikol::GFX_UNIFORM_TYPE_VEC4, color);
    
    nikol::gfx_context_sumbit(gfx, call);

    nikol::window_poll_events(window);
    nikol::window_swap_buffers(window);
  }

  // De-initialze
  nikol::gfx_shader_destroy(shader);
  nikol::gfx_draw_call_destroy(call); 
  nikol::gfx_context_destroy(gfx);
  nikol::window_close(window);
  nikol::shutdown();

}
