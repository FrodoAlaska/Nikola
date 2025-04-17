#include "nikola/nikola_app.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_render.h"
#include "nikola/nikola_resources.h"
#include "nikola/nikola_file.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola {

/// ----------------------------------------------------------------------
/// Engine
struct Engine {
  AppDesc app_desc;
  App* app;
  Window* window;

  bool is_running;
};

static Engine s_engine;
/// Engine
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Macros

#define CHECK_VALID_CALLBACK(func, ...) if(func) func(__VA_ARGS__); 

/// Macros
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

static bool quit_app_callback(const Event& event, const void* dispatcher, const void* listener) {
  if(event.type != EVENT_APP_QUIT) {
    return false;
  }

  s_engine.is_running = false;
  return true;
}

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Engine functions

void engine_init(const AppDesc& desc) {
  // Engine init
  s_engine.app_desc   = desc; 
  s_engine.is_running = true;

  // Library init 
  NIKOLA_ASSERT(init(), "Failed to initialize Nikola");
 
  // Window init 
  s_engine.window = window_open(desc.window_title.c_str(), desc.window_width, desc.window_height, desc.window_flags);
  NIKOLA_ASSERT(s_engine.window, "Failed to open a window");

  // Resource manager init 
  resource_manager_init();

  // Renderer init 
  renderer_init(s_engine.window);

  // Batch renderer init
  batch_renderer_init();
  
  // Listening to any interesting events
  event_listen(EVENT_APP_QUIT, quit_app_callback);

  // Check for any command line arguments
  Args cli_args; 
  for(u32 i = 0; i < desc.args_count; i++) {
    cli_args.push_back(desc.args_values[i]);
  }

  // App init 
  NIKOLA_ASSERT(s_engine.app_desc.init_fn, "Cannot start the engine with an invalid application initialization callback");
  s_engine.app = s_engine.app_desc.init_fn(cli_args, s_engine.window);

  NIKOLA_LOG_INFO("Successfully initialized the application \'%s\'", desc.window_title.c_str());
}

void engine_run() {
  while(s_engine.is_running) {
    // Update
    filewatcher_update(); 
    CHECK_VALID_CALLBACK(s_engine.app_desc.update_fn, s_engine.app, niclock_get_delta_time());

    // Render
    CHECK_VALID_CALLBACK(s_engine.app_desc.render_fn, s_engine.app);

    // Poll for window events
    window_poll_events(s_engine.window);
  }
}

void engine_shutdown() {
  CHECK_VALID_CALLBACK(s_engine.app_desc.shutdown_fn, s_engine.app);
  
  resource_manager_shutdown();
  filewatcher_shutdown(); 
  renderer_shutdown();
  batch_renderer_shutdown();

  window_close(s_engine.window);
  shutdown();
  
  NIKOLA_LOG_INFO("Appication \'%s\' was successfully shutdown", s_engine.app_desc.window_title.c_str());
}

/// Engine functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
