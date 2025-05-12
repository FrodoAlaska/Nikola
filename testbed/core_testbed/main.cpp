#include <nikola/nikola.h>

int main() {
  // Initialze the library
  if(!nikola::init()) {
    return -1;
  }

  // Openinig the window
  nikola::i32 win_flags = nikola::WINDOW_FLAGS_FOCUS_ON_CREATE | nikola::WINDOW_FLAGS_RESIZABLE;
  nikola::Window* window = nikola::window_open("Hello, Nikola", 1366, 768, win_flags);
  if(!window) {
    return -1;
  }

  // Creating a graphics context
  nikola::GfxContextDesc gfx_desc = {
    .window = window,
    .states = nikola::GFX_STATE_DEPTH | nikola::GFX_STATE_STENCIL,
  };
  nikola::GfxContext* gfx = nikola::gfx_context_init(gfx_desc);
  if(!gfx) {
    return -1;
  }

  // Main loop
  while(nikola::window_is_open(window)) {
    // Will stop the application when F1 is pressed
    if(nikola::input_key_pressed(nikola::KEY_F1)) {
      break;
    }
    
    // Clear the screen to a color
    nikola::f32 r = 16.0f / 255.0f; 
    nikola::f32 g = 14.0f / 255.0f; 
    nikola::f32 b = 28.0f / 255.0f; 
    nikola::gfx_context_clear(gfx, r, g, b, 1.0f);
    
    // Swap the internal window buffer
    nikola::gfx_context_present(gfx);
    
    // Poll the window events
    nikola::window_poll_events(window);
  }

  // De-initialze
  nikola::gfx_context_shutdown(gfx);
  nikola::window_close(window);
  nikola::shutdown();
}
