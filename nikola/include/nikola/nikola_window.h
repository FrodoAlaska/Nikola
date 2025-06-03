#pragma once

#include "nikola_base.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ---------------------------------------------------------------------
/// *** Window ***

///---------------------------------------------------------------------------------------------------------------------
/// Window
struct Window;
/// Window
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// WindowFlags
enum WindowFlags {
  /// No flags will be set when the window gets created
  WINDOW_FLAGS_NONE                = 1 << 0,

  /// Set the window to be resizable.
  WINDOW_FLAGS_RESIZABLE           = 1 << 1,

  /// The window will gain focus on creation.
  WINDOW_FLAGS_FOCUS_ON_CREATE     = 1 << 2, 

  /// The window will always gain focus when shown.
  WINDOW_FLAGS_FOCUS_ON_SHOW       = 1 << 3, 

  /// Minimize the window on creation.
  WINDOW_FLAGS_MINIMIZE            = 1 << 4, 
  
  /// Maxmize the window on creation.
  WINDOW_FLAGS_MAXMIZE             = 1 << 5, 
  
  /// Disable window decoration such as borders, widgets, etc. 
  /// Decorations will be enabled by default.
  WINDOW_FLAGS_DISABLE_DECORATIONS = 1 << 6,
  
  /// Center the mouse position relative to the screen on startup. 
  WINDOW_FLAGS_CENTER_MOUSE        = 1 << 7,
  
  /// Hide the cursor at creation. 
  /// The cursos will be shown by default.
  WINDOW_FLAGS_HIDE_CURSOR         = 1 << 8,

  /// Set the window to be fullscreen on creation. 
  WINDOW_FLAGS_FULLSCREEN          = 1 << 9,
};
/// WindowFlags
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Window functions

/// 
/// Creates a new window context and return the result. 
/// Returns a `nullptr` if the window was failed to be opened. 
///
/// `title`: The title of the new window. 
/// `width`: The width of the new window.
/// `height`: The height of the new window.
/// `flags`:
///   - `WINDOW_FLAGS_NONE`                = No flags will be set when the window gets created.
///   - `WINDOW_FLAGS_RESIZABLE`           = Set the window to be resizable.
///   - `WINDOW_FLAGS_FOCUS_ON_CREATE`     = The window will gain focus on creation.  
///   - `WINDOW_FLAGS_FOCUS_ON_SHOW`       = The window will always gain focus when shown. 
///   - `WINDOW_FLAGS_MINIMIZE`            = Minimize the window on creation. 
///   - `WINDOW_FLAGS_MAXMIZE`             = Maxmize the window on creation. 
///   - `WINDOW_FLAGS_DISABLE_DECORATIONS` = Disable window decoration such as borders, widgets, etc. Decorations will be enabled by default.
///   - `WINDOW_FLAGS_CENTER_MOUSE`        = Center the mouse position relative to the screen on startup.
///   - `WINDOW_FLAGS_HIDE_CURSOR`         = Hide the cursor at creation. The cursos will be shown by default.
///   - `WINDOW_FLAGS_FULLSCREEN`          = Set the window to be fullscreen on creation. 
/// 
NIKOLA_API Window* window_open(const char* title, const i32 width, const i32 height, i32 flags);

/// Closes the `window` context and clears up any memory.
NIKOLA_API void window_close(Window* window);

/// Poll events from the `window` context.
NIKOLA_API void window_poll_events(Window* window);

/// Swap the internal buffer of the `window` context every `interval` count. 
/// 
/// @NOTE: The `interval` parametar can be set as `0` if VSYNC is not needed.
/// Otherwise, `interval` can be set as `1` (which waits exactly `1` screen update) 
/// to disable VSYNC.
///
/// @NOTE: This might have no effect on some platforms.
NIKOLA_API void window_swap_buffers(Window* window, const i32 interval);

/// Returns `true` if the `window` context is still actively open. 
NIKOLA_API const bool window_is_open(const Window* window);

/// Returns `true` if the `window` context is currently in fullscreen mode.
NIKOLA_API const bool window_is_fullscreen(const Window* window);

/// Returns `true` if the `window` context is currently focused.
NIKOLA_API const bool window_is_focused(const Window* window);

/// Returns `true` if the `window` context is currently shown.
NIKOLA_API const bool window_is_shown(const Window* window);

/// Retrieve the current size of the `window` context.
NIKOLA_API void window_get_size(const Window* window, i32* width, i32* height);

/// Retrieve the internal native handle of `window`.
NIKOLA_API void* window_get_handle(const Window* window);

/// Retrieve the current title of the `window` context.
NIKOLA_API const char* window_get_title(const Window* window);

/// Retrieve the current size of the monitor.
NIKOLA_API void window_get_monitor_size(const Window* window, i32* width, i32* height);

/// Retrieve the aspect ratio of the `window` context.
NIKOLA_API const f32 window_get_aspect_ratio(const Window* window);

/// Retrieve the refresh rate of the monitor.
NIKOLA_API const f32 window_get_refresh_rate(const Window* window);

/// Retrieve the set window flags of `window`.
NIKOLA_API const WindowFlags window_get_flags(const Window* window);

/// Retrieve the current position of the `window` context relative to the monitor.
NIKOLA_API void window_get_position(const Window* window, i32* x, i32* y);

/// Set the given `window` as the current active context.
NIKOLA_API void window_set_current_context(Window* window);

/// Either disable or enable fullscreen mode on the `window` context.
NIKOLA_API void window_set_fullscreen(Window* window, const bool fullscreen);

/// Either show or hide the `window` context.
NIKOLA_API void window_set_show(Window* window, const bool show);

/// Set the size of the `window` to `width` and `height`.
NIKOLA_API void window_set_size(Window* window, const i32 width, const i32 height);

/// Set the title of the `window` to `title`.
NIKOLA_API void window_set_title(Window* window, const char* title);

/// Set the position of the `window` to `x_pos` and `y_pos`.
NIKOLA_API void window_set_position(Window* window, const i32 x_pos, const i32 y_pos);

/// Window functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Window ***
/// ---------------------------------------------------------------------


} // End of nikola

//////////////////////////////////////////////////////////////////////////
