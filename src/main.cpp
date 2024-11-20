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

  // Main loop
  while(nikol::window_is_open(window)) {
    if(nikol::input_key_pressed(nikol::KEY_F1)) {
      break;
    }

    nikol::gfx_context_clear(gfx, 1.0f, 1.0f, 1.0f, 1.0f);

    nikol::window_poll_events(window);
    nikol::window_swap_buffers(window);
  }

  // De-initialze
  nikol::gfx_context_destroy(gfx);
  nikol::window_close(window);
  nikol::shutdown();

}
