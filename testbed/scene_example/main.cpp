#include <nikola/nikola_app.h>

#include "app.hpp"

// Yeah, unfortunate...
#if NIKOLA_PLATFORM_WINDOWS == 1 
#include <windows.h>
#endif

int main(int argc, char** argv) {
  int win_flags = nikola::WINDOW_FLAGS_FOCUS_ON_CREATE | 
                  nikola::WINDOW_FLAGS_GFX_HARDWARE    | 
                  nikola::WINDOW_FLAGS_RESIZABLE       | 
                  nikola::WINDOW_FLAGS_CENTER_MOUSE;

  nikola::AppDesc app_desc {
    .init_fn     = app_init,
    .shutdown_fn = app_shutdown,
    .update_fn   = app_update, 
    .render_fn   = app_render, 

    .window_title  = "Hi. I'm window. Open me. ", 
    .window_width  = 1366, 
    .window_height = 768, 
    .window_flags  = win_flags,

    .args_values = argv, 
    .args_count  = argc,
  };

  nikola::engine_init(app_desc);
  nikola::engine_run();
  nikola::engine_shutdown();

  return 0;
}

// NIKOLA_MAIN(engine_run);
