#include "nikol/nikol_core.hpp"
#include "nikol/nikol_engine.hpp"

int main() {
  if(!nikol::init()) {
    return -1;
  }

  int win_flags         = nikol::WINDOW_FLAGS_FOCUS_ON_CREATE | 
                          nikol::WINDOW_FLAGS_GFX_HARDWARE | 
                          nikol::WINDOW_FLAGS_CENTER_MOUSE |
                          nikol::WINDOW_FLAGS_HIDE_CURSOR;
  nikol::Window* window = nikol::window_open("Mesh Viewer", 1366, 768, win_flags);
  NIKOL_ASSERT(window, "Could not open the window");

  while(nikol::window_is_open(window)) {
    if(nikol::input_key_pressed(nikol::KEY_ESCAPE)) {
      break;
    }

    nikol::window_swap_buffers(window);
    nikol::window_poll_events(window);
  }

  nikol::window_close(window);
  nikol::shutdown();
}
