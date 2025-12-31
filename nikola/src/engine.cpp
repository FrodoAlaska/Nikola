#include "nikola/nikola.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola {

/// ----------------------------------------------------------------------
/// Engine
struct Engine {
  AppDesc app_desc;
  App* app;

  Window* window;
  GfxContext* gfx_context;

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
/// Engine functions

void engine_init(const AppDesc& desc) {
  NIKOLA_PROFILE_FUNCTION();

  // Engine init
  
  s_engine.app_desc   = desc; 
  s_engine.is_running = true;

  // Library init 
  NIKOLA_ASSERT(init(), "Failed to initialize Nikola");
 
  // Window init 
  
  s_engine.window = window_open(desc.window_title.c_str(), desc.window_width, desc.window_height, desc.window_flags);
  NIKOLA_ASSERT(s_engine.window, "Failed to open a window");

  // Useful input actions init
  
  InputAction action_desc = {
    .key_bind     = KEY_ENTER, 
    .mouse_bind   = MOUSE_BUTTON_LEFT, 
    .gamepad_bind = GAMEPAD_BUTTON_CROSS,
  };
  input_action_bind("ui-click", action_desc);
  
  action_desc = {
    .key_bind     = KEY_DOWN, 
    .gamepad_bind = GAMEPAD_BUTTON_DPAD_DOWN,
  };
  input_action_bind("ui-nav-down", action_desc);
  
  action_desc = {
    .key_bind     = KEY_UP, 
    .gamepad_bind = GAMEPAD_BUTTON_DPAD_UP,
  };
  input_action_bind("ui-nav-up", action_desc);

  // Audio init
  audio_device_init(nullptr);

  // Resource manager init 
  resource_manager_init();

  // Renderers init 
  
  renderer_init(s_engine.window);
  s_engine.gfx_context = renderer_get_context();
  
  ui_renderer_init(s_engine.gfx_context);

  // Physics world init

  PhysicsWorldDesc world_desc{};
  world_desc.layers_matrix[PHYSICS_OBJECT_LAYER_0][0] = PHYSICS_OBJECT_LAYER_0;
  world_desc.layers_matrix[PHYSICS_OBJECT_LAYER_0][1] = PHYSICS_OBJECT_LAYER_1;
  world_desc.layers_matrix[PHYSICS_OBJECT_LAYER_1][0] = PHYSICS_OBJECT_LAYER_1;
  world_desc.layers_matrix[PHYSICS_OBJECT_LAYER_1][1] = PHYSICS_OBJECT_LAYER_0;

  physics_world_init(world_desc);

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
  while(window_is_open(s_engine.window)) {
    // Poll for input events
    window_poll_events(s_engine.window);
    
    // Update 
    
    physics_world_step(1 / 60.0f, 1); 
    CHECK_VALID_CALLBACK(s_engine.app_desc.update_fn, s_engine.app, niclock_get_delta_time());

    // Render
    
    CHECK_VALID_CALLBACK(s_engine.app_desc.render_fn, s_engine.app);
    CHECK_VALID_CALLBACK(s_engine.app_desc.render_gui_fn, s_engine.app);
    
    // Update the internal systems

    input_update();
    niclock_update();

    // Present
    gfx_context_present(s_engine.gfx_context); 
  }
}

void engine_shutdown() {
  CHECK_VALID_CALLBACK(s_engine.app_desc.shutdown_fn, s_engine.app);

  physics_world_shutdown();
  ui_renderer_shutdown();
  renderer_shutdown();
  resource_manager_shutdown();
  audio_device_shutdown();

  window_close(s_engine.window);
  shutdown();
  
  NIKOLA_LOG_INFO("Appication \'%s\' was successfully shutdown", s_engine.app_desc.window_title.c_str());
}

/// Engine functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
