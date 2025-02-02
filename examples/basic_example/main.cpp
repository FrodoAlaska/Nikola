#include "nikol/nikol_core.hpp"
#include "nikol/nikol_engine.hpp"

int main() {
  // Library init
  if(!nikol::init()) {
    return -1;
  }
  
  // Window init
  int win_flags         = nikol::WINDOW_FLAGS_FOCUS_ON_CREATE | 
                          nikol::WINDOW_FLAGS_GFX_HARDWARE | 
                          nikol::WINDOW_FLAGS_CENTER_MOUSE |
                          nikol::WINDOW_FLAGS_HIDE_CURSOR;
  nikol::Window* window = nikol::window_open("3D Example", 1366, 768, win_flags);
  NIKOL_ASSERT(window, "Could not open the window");
  
  // Renderer init
  nikol::renderer_init(window, nikol::Vec4(0.1f, 0.1f, 0.1f, 1.0f));  

  // Resource manager init
  nikol::resource_manager_init();

  // Camera init
  nikol::Camera cam;
  nikol::camera_create(&cam, (1366.0f / 768.0f), nikol::Vec3(10.0f, 0.0f, 10.0f), nikol::Vec3(-3.0f, 0.0f, 0.0f), nullptr);

  // Main loop
  while(nikol::window_is_open(window)) {
    if(nikol::input_key_pressed(nikol::KEY_ESCAPE)) {
      break;
    }

    nikol::renderer_begin(cam);

    nikol::renderer_end();

    nikol::window_poll_events(window);
  }

  // De-initialization
  nikol::resource_manager_shutdown();
  nikol::renderer_shutdown();
  nikol::window_close(window);
  nikol::shutdown();
}
