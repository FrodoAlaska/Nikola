#include <nikol_core.h>

int main(int argc, char** argv) {
  // Initialze 
  nikol::logger_init("logs/");
  nikol::event_init();
  nikol::input_init();

  // Openinig the window
  nikol::i32 win_flags = nikol::WINDOW_FLAGS_FOCUS_ON_CREATE;
  nikol::Window* window = nikol::window_open("Hello, Nikol", 1366, 768, win_flags);
  if(!window) {
    return -1;
  }

  // Main loop
  while(nikol::window_is_open(window)) {
    if(nikol::input_key_pressed(nikol::KEY_F1)) {
      break;
    }
 
    nikol::window_poll_events(window);
    nikol::window_swap_buffers(window);
  }

  // De-initialze
  nikol::window_close(window);
  nikol::event_shutdown();
  nikol::logger_shutdown();
}
