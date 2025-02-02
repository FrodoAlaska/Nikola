#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

int main() {
  // Library init
  if(!nikola::init()) {
    return -1;
  }
  
  // Window init
  int win_flags         = nikola::WINDOW_FLAGS_FOCUS_ON_CREATE | 
                          nikola::WINDOW_FLAGS_GFX_HARDWARE | 
                          nikola::WINDOW_FLAGS_CENTER_MOUSE |
                          nikola::WINDOW_FLAGS_HIDE_CURSOR;
  nikola::Window* window = nikola::window_open("3D Example", 1366, 768, win_flags);
  NIKOLA_ASSERT(window, "Could not open the window");
  
  // Renderer init
  nikola::renderer_init(window, nikola::Vec4(0.1f, 0.1f, 0.1f, 1.0f));  

  // Resource manager init
  nikola::resource_manager_init();

  // Camera init
  nikola::Camera cam;
  nikola::camera_create(&cam, (1366.0f / 768.0f), nikola::Vec3(10.0f, 0.0f, 10.0f), nikola::Vec3(-3.0f, 0.0f, 0.0f), nullptr);

  // Main loop
  while(nikola::window_is_open(window)) {
    if(nikola::input_key_pressed(nikola::KEY_ESCAPE)) {
      break;
    }

    nikola::renderer_begin(cam);

    nikola::renderer_end();

    nikola::window_poll_events(window);
  }

  // De-initialization
  nikola::resource_manager_shutdown();
  nikola::renderer_shutdown();
  nikola::window_close(window);
  nikola::shutdown();
}
