#pragma once

#include "nikola_window.h"
#include "nikola_containers.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Macros 

#if NIKOLA_PLATFORM_WINDOWS == 1
  
  // Since Windows likes to bloat the application 
  // with unnecessary crap, this define should disable 
  // that completely.
  #define WIN32_LEAN_AND_MEAN
 
  /// The entry point to a C++ application
  #define NIKOLA_MAIN(engine_main)                                                        \
  int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev_inst, LPSTR cmd_line, int cmd_show) { \
    return engine_main(0, &cmd_line);                                                     \
  }

#elif NIKOLA_PLATFORM_LINUX == 1
  
  /// The entry point to a C++ application
  #define NIKOLA_MAIN(engine_main)  \
  int main(int argc, char** argv) { \
    return engine_main(argc, argv); \
  }                                 \

#endif

/// Macros 
///---------------------------------------------------------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Engine ***

///---------------------------------------------------------------------------------------------------------------------
/// App
struct App;
/// App
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Args 
using Args = DynamicArray<String>;  
/// Args 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// App callbacks

/// A function callback to allocate and initialize a `App` struct.
using AppInitFn       = App*(*)(const Args& args, Window* window);

/// A function callback to free/reclaim any memory consumed by a `App` struct.
using AppShutdownFn   = void(*)(App* app);

/// A function callback to update a `App` struct, passing in the `delta_time`.
using AppUpdateFn     = void(*)(App* app, const f64 delta_time);

/// A function callback to render a `App` struct.
using AppRenderPassFn = void(*)(App* app);

/// App callbacks
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// App description
struct AppDesc {
  AppInitFn init_fn         = nullptr;
  AppShutdownFn shutdown_fn = nullptr;
  AppUpdateFn update_fn     = nullptr;
  
  AppRenderPassFn render_fn     = nullptr;
  AppRenderPassFn render_gui_fn = nullptr;
 
  String window_title;
  i32 window_width, window_height;
  i32 window_flags;

  char** args_values = nullptr; 
  i32 args_count     = 0;
};
/// App description 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Engine functions

/// Initialize all of the engine sub-systems in order as well as 
/// allocate and initialize a new `App` struct using the information 
/// given in `desc`.
NIKOLA_API void engine_init(const AppDesc& desc);

/// Run a loop, updating and rendering the `App` struct allocated earlier 
/// as well as any engine sub-systems.
NIKOLA_API void engine_run();

/// Free/reclaim and shutdown any and all engine sub-systems as well 
/// as the `App` struct.
NIKOLA_API void engine_shutdown();

/// Engine functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Engine ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
