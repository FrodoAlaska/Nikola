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
  
  const char* src =
  "#version 460 core\n"
  ""
  "layout (location = 0) in vec3 a_pos;\n"
  "layout (location = 1) in vec2 a_texture_coords;\n"
  "layout (location = 2) in vec3 a_normal;\n"
  ""
  "void main() {\n"
  " gl_Position = vec4(a_pos, 1.0f);\n"
  "}\n"
  "#version 460 core\n"
  "layout (location = 0) out vec4 out_color;\n"
  "void main() {\n"
  " out_color = vec4(1.0);\n"
  "}\n";
  nikol::GfxShader* shader = nikol::gfx_shader_create(src);

  nikol::GfxDrawCall* call = nikol::gfx_draw_call_create(gfx);

  nikol::f32 vertices[] = {
    -0.5f, -0.5f, 0.0f, 
     0.5f, -0.5f, 0.0f, 
     0.5f,  0.5f, 0.0f, 
  };
  nikol::GfxBufferDesc vert_buff = {
    .data           = vertices, 
    .size           = sizeof(vertices), 
    .elements_count = 9, 
    .type = nikol::GFX_BUFFER_VERTEX, 
    .mode = nikol::GFX_BUFFER_MODE_STATIC_DRAW,
  };

  nikol::GfxBufferLayout layout[] = {
    nikol::GFX_LAYOUT_FLOAT3,
  };

  nikol::gfx_draw_call_push_buffer(call, gfx, &vert_buff);
  nikol::gfx_draw_call_set_layout(call, gfx, &vert_buff, layout, 1);
  nikol::gfx_draw_call_push_shader(call, gfx, shader);

  // Main loop
  while(nikol::window_is_open(window)) {
    if(nikol::input_key_pressed(nikol::KEY_F1)) {
      break;
    }

    nikol::gfx_context_clear(gfx, 0.1f, 0.1f, 0.1f, 1.0f);
    nikol::gfx_context_draw(gfx, call);

    nikol::window_poll_events(window);
    nikol::window_swap_buffers(window);
  }

  // De-initialze
  nikol::gfx_draw_call_destroy(call); 

  nikol::gfx_context_destroy(gfx);
  nikol::window_close(window);
  nikol::shutdown();

}
