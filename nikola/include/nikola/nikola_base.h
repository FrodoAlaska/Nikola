#pragma once

#include <cstddef>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola
 
/// ----------------------------------------------------------------------
/// *** Typedefs ***

/// char
typedef char  i8; 

/// short
typedef short i16;

/// int
typedef int   i32;

/// long
typedef long  i64;

/// unsigned char
typedef unsigned char  u8;

/// unsigned short
typedef unsigned short u16;   

/// unsigned int
typedef unsigned int   u32;   

/// unsigned long
typedef unsigned long  u64;   

/// size_t
typedef size_t         sizei;

/// float
typedef float  f32;   

/// double
typedef double f64;   

/// *** Typedefs ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** DEFS ***

/// Nikola only supports OpenGL versions greater than these.
#define NIKOLA_GL_MINIMUM_MAJOR_VERSION 4
#define NIKOLA_GL_MINIMUM_MINOR_VERSION 2

// Exports
#ifdef NIKOLA_EXPORT 

  // Window exports
  #ifdef _MSC_VER
    #define NIKOLA_API __declspec(dllexport) 
  // Linux exports
  #else
    #define NIKOLA_API __attribute__((visibility("default"))) 
  #endif

#endif

// Window imports
#ifdef _MSC_VER 
  #define NIKOLA_API __declspec(dllexport) 
// Linux exports
#else
  #define NIKOLA_API __attribute__((visibility("default"))) 
#endif

/// *** DEFS ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Macros ***

/// Returns `true` if `other` is set in `bit`
#define IS_BIT_SET(bit, other) ((bit & other) == other)

/// Sets/adds `other` into `bit`
#define SET_BIT(bit, other)    (bit |= other)

/// Unsets/removes `other` from `bit`
#define UNSET_BIT(bit, other)  (bit &= ~(other))

/// *** Macros ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Build Types ***

#if defined(DEBUG) || defined(_DEBUG)
  #define NIKOLA_BUILD_DEBUG   1 
  #define NIKOLA_BUILD_RELEASE 0
#else
  #define NIKOLA_BUILD_DEBUG   0 
  #define NIKOLA_BUILD_RELEASE 1
#endif

/// *** Build Types ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Platform detection ***

/// Windows
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define NIKOLA_PLATFORM_WINDOWS 1
#define NIKOLA_GFX_CONTEXT_OPENGL
#ifndef _WIN64 
#error "[NIKOLA-FATAL]: Only support 64-bit machines\n"
#endif 

/// Linux
#elif defined(__linux__) || defined(__gnu_linux__)
#define NIKOLA_PLATFORM_LINUX     1 
#define NIKOLA_GFX_CONTEXT_OPENGL
#endif

/// *** Platform detection ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Library init ***

///---------------------------------------------------------------------------------------------------------------------
/// Library functions

/// Initialze various different subsystems of Nikola. 
NIKOLA_API const bool init();

/// Shutdown subsystems of the Nikola.
NIKOLA_API void shutdown();

/// Library functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Library init ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Memory ***

///---------------------------------------------------------------------------------------------------------------------
/// Memory functions 

/// Allocate a memory block of size `size`.
/// WARN: This function will assert if there's no suffient memory left.
NIKOLA_API void* memory_allocate(const sizei size);

/// Re-allocate a block of memory `ptr` with a new size of `new_size`.
/// WARN: This function will assert if `ptr` is a `nullptr`.
NIKOLA_API void* memory_reallocate(void* ptr, const sizei new_size);

/// Set the value of the memory block `ptr` with a size of `ptr_size` to `value`.
/// WARN: This function will assert if `ptr` is a `nullptr`.
NIKOLA_API void* memory_set(void* ptr, const i32 value, const sizei ptr_size);

/// Set the value of the memory block `ptr` of size `ptr_size` to 0.
/// NOTE: This is equivalent to `memory_set(ptr, 0, ptr_size)`.
/// WARN: This function will assert if `ptr` is a `nullptr`.
NIKOLA_API void* memory_zero(void* ptr, const sizei ptr_size);

/// Allocate `count` blocks of memory each with the size of `block_size`.
/// NOTE: This is equivalent to `memory_allocate(block_size * count)`.
NIKOLA_API void* memory_blocks_allocate(const sizei count, const sizei block_size);

/// Copy `src_size` bytes of `src` to the memory block `dest`. 
/// WARN: This function will assert if `dest` or `src` are a `nullptr`.
NIKOLA_API void* memory_copy(void* dest, const void* src, const sizei src_size);

/// Free/reclaim the memory of the given `ptr`.
/// WARN: This function will assert if `ptr` is a `nullptr`.
NIKOLA_API void memory_free(void* ptr);

/// Retrieve the amount of allocations made so far.
NIKOLA_API const sizei memory_get_allocations_count();

/// Retrieve the amount of frees made so far. 
NIKOLA_API const sizei memory_get_frees_count();

/// Retrieve how many bytes have been allocated so far. 
NIKOLA_API const sizei memory_get_allocation_bytes();

/// Memory functions 
///---------------------------------------------------------------------------------------------------------------------

/// *** Memory ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Logger ***

#define NIKOLA_LOG_INFO_ACTIVE 1 
#define NIKOLA_LOG_WARN_ACTIVE 1 

// Only activate trace and debug logs on debug builds
#if NIKOLA_BUILD_RELEASE == 1
#define NIKOLA_LOG_TRACE_ACTIVE  0
#define NIKOLA_LOG_DEBUG_ACTIVE 0
#else 
#define NIKOLA_LOG_TRACE_ACTIVE  1
#define NIKOLA_LOG_DEBUG_ACTIVE 1
#endif

///---------------------------------------------------------------------------------------------------------------------
/// Log level
enum LogLevel {
  LOG_LEVEL_TRACE,
  LOG_LEVEL_DEBUG, 
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR, 
  LOG_LEVEL_FATAL,
};
/// Log level
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Logger functions

/// Log an assertion with the given information.
NIKOLA_API void logger_log_assert(const i8* expr, const i8* msg, const i8* file, const u32 line_num);

/// Log a specific log level with the given `msg` and any other parametars.
NIKOLA_API void logger_log(const LogLevel lvl, const i8* msg, ...);

/// Logger functions
///---------------------------------------------------------------------------------------------------------------------

/// Trace log
#if NIKOLA_LOG_TRACE_ACTIVE == 1
#define NIKOLA_LOG_TRACE(msg, ...) nikola::logger_log(nikola::LOG_LEVEL_TRACE, msg, ##__VA_ARGS__)
#else
#define NIKOLA_LOG_TRACE(msg, ...)
#endif
/// Trace log

/// Debug log
#if NIKOLA_LOG_DEBUG_ACTIVE == 1
#define NIKOLA_LOG_DEBUG(msg, ...) nikola::logger_log(nikola::LOG_LEVEL_DEBUG, msg, ##__VA_ARGS__)
#else
#define NIKOLA_LOG_DEBUG(msg, ...)
#endif
/// Debug log

/// Info log
#if NIKOLA_LOG_INFO_ACTIVE == 1
#define NIKOLA_LOG_INFO(msg, ...) nikola::logger_log(nikola::LOG_LEVEL_INFO, msg, ##__VA_ARGS__)
#else
#define NIKOLA_LOG_INFO(msg, ...)
#endif
/// Info log

/// Warn log
#if NIKOLA_LOG_WARN_ACTIVE == 1
#define NIKOLA_LOG_WARN(msg, ...) nikola::logger_log(nikola::LOG_LEVEL_WARN, msg, ##__VA_ARGS__)
#else
#define NIKOLA_LOG_WARN(msg, ...)
#endif
/// Warn log

/// Error log
#define NIKOLA_LOG_ERROR(msg, ...) nikola::logger_log(nikola::LOG_LEVEL_ERROR, msg, ##__VA_ARGS__)
/// Error log

/// Fatal log
#define NIKOLA_LOG_FATAL(msg, ...) nikola::logger_log(nikola::LOG_LEVEL_FATAL, msg, ##__VA_ARGS__)
/// Fatal log

/// *** Logger ***
/// ---------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// *** Asserts ***

#define NIKOLA_ASSERTS_ENABLED // @TEMP 

#ifdef NIKOLA_ASSERTS_ENABLED

// Debug break 
#if NIKOLA_PLATFORM_WINDOWS == 1

/// Windows debug break
#define DEBUG_BREAK() __debugbreak()
/// Windows debug break

/// Linux debug break
#elif NIKOLA_PLATFORM_LINUX == 1
#define DEBUG_BREAK() __builtin_trap()
/// Linux debug break

#endif

///---------------------------------------------------------------------------------------------------------------------
/// Assert macro 
#define NIKOLA_ASSERT(expr, msg)                                        \
        {                                                               \
          if(expr) {                                                    \
          }                                                             \
          else {                                                        \
            nikola::logger_log_assert(#expr, msg, __FILE__, __LINE__);  \
            DEBUG_BREAK();                                              \
          }                                                             \
        }
/// Assert macro 
///---------------------------------------------------------------------------------------------------------------------

#endif

/// *** Asserts ***
/// ---------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// *** Event ***

///---------------------------------------------------------------------------------------------------------------------
/// EventType
enum EventType {
  /// App events 
  EVENT_APP_QUIT = 0,

  /// Window events 
  EVENT_WINDOW_MOVED,
  EVENT_WINDOW_MINIMIZED, 
  EVENT_WINDOW_MAXIMIZED,
  EVENT_WINDOW_FOCUSED, 
  EVENT_WINDOW_RESIZED, 
  EVENT_WINDOW_FRAMEBUFFER_RESIZED, 
  EVENT_WINDOW_CLOSED, 
  EVENT_WINDOW_FULLSCREEN,

  /// Mouse events 
  EVENT_MOUSE_MOVED, 
  EVENT_MOUSE_BUTTON_PRESSED, 
  EVENT_MOUSE_BUTTON_RELEASED,
  EVENT_MOUSE_SCROLL_WHEEL,
  EVENT_MOUSE_CURSOR_SHOWN,
  EVENT_MOUSE_ENTER, 
  EVENT_MOUSE_LEAVE,

  /// Keyboard events 
  EVENT_KEY_PRESSED, 
  EVENT_KEY_RELEASED, 

  /// Joystick events
  EVENT_JOYSTICK_CONNECTED, 
  EVENT_JOYSTICK_DISCONNECTED, 

  EVENTS_MAX = EVENT_JOYSTICK_DISCONNECTED + 1,
};
/// EventType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Event
struct Event {
  /// The event's type 
  EventType type;
  
  /// Window events 
  
  /// New poisition of the window
  i32 window_new_pos_x, window_new_pos_y;

  /// The current focus state of the window
  bool window_has_focus;       

  /// The window's new size
  i32 window_new_width, window_new_height;        

  /// The window's new size of the framebuffer
  i32 window_framebuffer_width, window_framebuffer_height; 
 
  /// The window's new fullscreen state
  bool window_is_fullscreen;

  /// Window events 
  
  /// Key events
  
  /// The key that was just pressed 
  i32 key_pressed; 

  /// The key that was just released
  i32 key_released;
  
  /// Key events
  
  /// Mouse events
  
  /// The current mouse position (relative to the screen) 
  f32 mouse_pos_x, mouse_pos_y;    
  
  /// By how much did the mouse move since the last frame? 
  f32 mouse_offset_x, mouse_offset_y; 
   
  /// The mouse button that was just pressed
  i32 mouse_button_pressed; 
 
  /// The mouse button that was just released
  i32 mouse_button_released;
 
  /// The value the scroll mouse's wheel moved by 
  f32 mouse_scroll_value; 

  /// Is the mouse cursor currently visible?
  bool cursor_shown;
  
  /// Mouse events
  
  /// Joystick events
  
  i32 joystick_id; 
  
  /// Joystick events
};
/// Event
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Event callback

using EventFireFn = bool(*)(const Event&, const void* dispatcher, const void* listener);

/// Event callback
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Event functions

/// Initialze the event system 
NIKOLA_API void event_init();

/// Shutdown the event system and reclaim some memory 
NIKOLA_API void event_shutdown();

/// Attach the given `func` callback to an event of type `type`, passing in the `listener` as well.
NIKOLA_API void event_listen(const EventType type, const EventFireFn& func, const void* listener = nullptr);

/// Call all callbacks associated with `event.type` and pass in the given `event` and the `dispatcher`. 
/// Returns `true` on success.
NIKOLA_API const bool event_dispatch(const Event& event, const void* dispatcher = nullptr);

/// Event functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Event ***
/// ---------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// *** Input ***

///---------------------------------------------------------------------------------------------------------------------
/// Key 
enum Key {
  UNKNOWN       = -1, 

  KEY_SPACE         = 32,
  KEY_APOSTROPHE    = 39, 
  KEY_COMMA         = 44, 
  KEY_MINUS         = 45, 
  KEY_PERIOD        = 46, 
  KEY_SLASH         = 47, 
  KEY_ZERO          = 48,
  KEY_1             = 49,
  KEY_2             = 50,
  KEY_3             = 51,
  KEY_4             = 52,
  KEY_5             = 53,
  KEY_6             = 54,
  KEY_7             = 55,
  KEY_8             = 56,
  KEY_9             = 57,
  KEY_SEMICOLON     = 59,
  KEY_EQUAL         = 61,
  KEY_A             = 65,
  KEY_B             = 66,
  KEY_C             = 67,
  KEY_D             = 68,
  KEY_E             = 69,
  KEY_F             = 70,
  KEY_G             = 71,
  KEY_H             = 72,
  KEY_I             = 73,
  KEY_J             = 74,
  KEY_K             = 75,
  KEY_L             = 76,
  KEY_M             = 77,
  KEY_N             = 78,
  KEY_O             = 79,
  KEY_P             = 80,
  KEY_Q             = 81,
  KEY_R             = 82,
  KEY_S             = 83,
  KEY_T             = 84,
  KEY_U             = 85,
  KEY_V             = 86,
  KEY_W             = 87,
  KEY_X             = 88,
  KEY_Y             = 89,
  KEY_Z             = 90,
  KEY_LEFT_BRACKET  = 91, 
  KEY_BACKSLASH     = 92, 
  KEY_RIGHT_BRACKET = 93, 
  KEY_GRAVE_ACCENT  = 96, 
  KEY_WORLD_1       = 161,
  KEY_WORLD_2       = 162,

  KEY_ESCAPE        = 256, 
  KEY_ENTER         = 257, 
  KEY_TAB           = 258, 
  KEY_BACKSPACE     = 259, 
  KEY_INSERT        = 260, 
  KEY_DELETE        = 261, 
  KEY_RIGHT         = 262, 
  KEY_LEFT          = 263, 
  KEY_DOWN          = 264, 
  KEY_UP            = 265, 
  KEY_PAGE_UP       = 266, 
  KEY_PAGE_DOW      = 267, 
  KEY_HOME          = 268, 
  KEY_END           = 269, 
  KEY_CAPS_LOCK     = 280, 
  KEY_SCROLL_LOCK   = 281, 
  KEY_NUM_LOCK      = 282, 
  KEY_PRINT_SCREEN  = 283, 
  KEY_PAUSE         = 284, 
  
  KEY_F1            = 290, 
  KEY_F2            = 291, 
  KEY_F3            = 292, 
  KEY_F4            = 293, 
  KEY_F5            = 294, 
  KEY_F6            = 295, 
  KEY_F7            = 296, 
  KEY_F8            = 297, 
  KEY_F9            = 298, 
  KEY_F10           = 299, 
  KEY_F11           = 300, 
  KEY_F12           = 301, 
  KEY_F13           = 302, 
  KEY_F14           = 303, 
  KEY_F15           = 304, 
  KEY_F16           = 305, 
  KEY_F17           = 306, 
  KEY_F18           = 307, 
  KEY_F19           = 308, 
  KEY_F20           = 309, 
  KEY_F21           = 310, 
  KEY_F22           = 311, 
  KEY_F23           = 312, 
  KEY_F24           = 313, 
  KEY_F25           = 314, 
  
  KEY_KP_0          = 320, 
  KEY_KP_1          = 321, 
  KEY_KP_2          = 322, 
  KEY_KP_3          = 323, 
  KEY_KP_4          = 324, 
  KEY_KP_5          = 325, 
  KEY_KP_6          = 326, 
  KEY_KP_7          = 327, 
  KEY_KP_8          = 328, 
  KEY_KP_9          = 329, 
  KEY_KP_DECIMAL    = 330, 
  KEY_KP_DIVIDE     = 331, 
  KEY_KP_MULTIPLY   = 332, 
  KEY_KP_SUBTRACT   = 333, 
  KEY_KP_ADD        = 334, 
  KEY_KP_ENTER      = 335, 
  KEY_KP_EQUAL      = 336, 
  
  KEY_LEFT_SHIFT    = 340, 
  KEY_LEFT_CONTROL  = 341, 
  KEY_LEFT_ALT      = 342, 
  KEY_LEFT_SUPER    = 343, 
  KEY_RIGHT_SHIFT   = 344, 
  KEY_RIGHT_CONTROL = 345, 
  KEY_RIGHT_ALT     = 346, 
  KEY_RIGHT_SUPER   = 347, 
  KEY_MENU          = 348, 

  KEYS_MAX = 349
};
/// Key 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// MouseButton 
enum MouseButton {
  MOUSE_BUTTON_1 = 0, 
  MOUSE_BUTTON_2 = 1, 
  MOUSE_BUTTON_3 = 2, 
  MOUSE_BUTTON_4 = 3, 
  MOUSE_BUTTON_5 = 4, 
  MOUSE_BUTTON_6 = 5, 
  MOUSE_BUTTON_7 = 6, 
  MOUSE_BUTTON_8 = 7, 

  MOUSE_BUTTON_LAST   = MOUSE_BUTTON_8,
  MOUSE_BUTTON_LEFT   = MOUSE_BUTTON_1,
  MOUSE_BUTTON_RIGHT  = MOUSE_BUTTON_2,
  MOUSE_BUTTON_MIDDLE = MOUSE_BUTTON_3,
  
  MOUSE_BUTTONS_MAX = 8,
};
/// MouseButton 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// JoystickID 
enum JoystickID {
  JOYSTICK_ID_0 = 0, 
  JOYSTICK_ID_1, 
  JOYSTICK_ID_2, 
  JOYSTICK_ID_3, 
  JOYSTICK_ID_4, 
  JOYSTICK_ID_5, 
  JOYSTICK_ID_6, 
  JOYSTICK_ID_7, 
  JOYSTICK_ID_8, 
  JOYSTICK_ID_9, 
  JOYSTICK_ID_10, 
  JOYSTICK_ID_11, 
  JOYSTICK_ID_12, 
  JOYSTICK_ID_13, 
  JOYSTICK_ID_14, 
  JOYSTICK_ID_15, 
  JOYSTICK_ID_16, 
  
  JOYSTICK_ID_LAST = JOYSTICK_ID_16, 
};
/// JoystickID 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GamepadAxis 
enum GamepadAxis {
  // X and Y of the left axis
  GAMEPAD_AXIS_LEFT    = 0, 

  // X and Y of the right axis
  GAMEPAD_AXIS_RIGHT   = 2,

  // Left and Right trigger
  GAMEPAD_AXIS_TRIGGER = 4, 
};
/// GamepadAxis 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GamepadButton 
enum GamepadButton {
  GAMEPAD_BUTTON_A = 0,
  GAMEPAD_BUTTON_B,
  GAMEPAD_BUTTON_X,
  GAMEPAD_BUTTON_Y,
  
  GAMEPAD_BUTTON_LEFT_BUMPER,
  GAMEPAD_BUTTON_RIGHT_BUMPER,
  
  GAMEPAD_BUTTON_BACK, 
  GAMEPAD_BUTTON_START, 
  GAMEPAD_BUTTON_GUIDE, 
  
  GAMEPAD_BUTTON_LEFT_THUMB, 
  GAMEPAD_BUTTON_RIGHT_THUMB, 
  
  GAMEPAD_BUTTON_DPAD_UP, 
  GAMEPAD_BUTTON_DPAD_RIGHT, 
  GAMEPAD_BUTTON_DPAD_DOWN, 
  GAMEPAD_BUTTON_DPAD_LEFT, 
  
  GAMEPAD_BUTTON_LAST = GAMEPAD_BUTTON_DPAD_LEFT,
  GAMEPAD_BUTTONS_MAX = GAMEPAD_BUTTON_LAST + 1,

  GAMEPAD_BUTTON_CROSS    = GAMEPAD_BUTTON_A,
  GAMEPAD_BUTTON_CIRCLE   = GAMEPAD_BUTTON_B,
  GAMEPAD_BUTTON_SQUARE   = GAMEPAD_BUTTON_X,
  GAMEPAD_BUTTON_TRIANGLE = GAMEPAD_BUTTON_Y,
};
/// GamepadButton 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Input functions 

/// Initialze the input system 
NIKOLA_API void input_init();

/// Copying around input buffers
NIKOLA_API void input_update(); 

/// Returns `true` if `key` was pressed this frame
NIKOLA_API const bool input_key_pressed(const Key key);

/// Returns `true` if `key` was released this frame
NIKOLA_API const bool input_key_released(const Key key);

/// Returns `true` if `key` is currently held down
NIKOLA_API const bool input_key_down(const Key key);

/// Returns `true` if `key` is currently held up
NIKOLA_API const bool input_key_up(const Key key);

/// Returns `true` if the mouse `button` was pressed this frame
NIKOLA_API const bool input_button_pressed(const MouseButton button);

/// Returns `true` if the mouse `button` was released this frame
NIKOLA_API const bool input_button_released(const MouseButton button);

/// Returns `true` if the mouse `button` is currently held up
NIKOLA_API const bool input_button_down(const MouseButton button);

/// Returns `true` if the mouse `button` is currently held up
NIKOLA_API const bool input_button_up(const MouseButton button);

/// The current position of the mouse relative to the screen
NIKOLA_API void input_mouse_position(f32* x, f32* y);

/// The amount the mouse moved by since the last frame
NIKOLA_API void input_mouse_offset(f32* x, f32* y);

/// Get the mouse's scroll wheel value
NIKOLA_API const f32 input_mouse_scroll_value();

/// Enable/disable the mouse cursor
NIKOLA_API void input_cursor_show(const bool show);

/// Returns _true_ if the cursor has entered the window surface and _false_ 
/// if the cursor left the window surface. 
NIKOLA_API const bool input_cursor_on_screen();

/// Returns the _current_ connection status of the given joystick `id`
NIKOLA_API const bool input_gamepad_connected(const JoystickID id);

/// Returns a number between -1.0f and 1.0f for the joystick's axes or bumbers.
///
/// NOTE: This functions returns will fill in the given `x` and `y` floats. The X and Y  
/// corresponds with the axis's directions. However, for the triggers, the X and Y 
/// correspond to the left (X) and right (Y) triggers.
NIKOLA_API void input_gamepad_axis_value(const JoystickID id, const GamepadAxis axis, f32* x, f32* y);

/// Returns `true` if `button` of the gamepad `id` was pressed this frame  
NIKOLA_API const bool input_gamepad_button_pressed(const JoystickID id, const GamepadButton button);

/// Returns `true` if `button` of the gamepad `id` was released this frame  
NIKOLA_API const bool input_gamepad_button_released(const JoystickID id, const GamepadButton button);

/// Returns `true` if `button` of the gamepad `id` is being pressed
NIKOLA_API const bool input_gamepad_button_down(const JoystickID id, const GamepadButton button);

/// Returns `true` if `button` of the jgamepad `id` is not pressed currently
NIKOLA_API const bool input_gamepad_button_up(const JoystickID id, const GamepadButton button);

/// Get the name of the given gamepad `id` as a string if available
NIKOLA_API const i8* input_gamepad_get_name(const JoystickID id);

/// Input functions 
///---------------------------------------------------------------------------------------------------------------------

/// *** Input ***
/// ---------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// *** Window ***

///---------------------------------------------------------------------------------------------------------------------
/// Window

/// An opaque `Window` struct
struct Window;

/// Window
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// WindowFlags
enum WindowFlags {
  /// No flags will be set when the window gets created
  WINDOW_FLAGS_NONE                = 1 << 0,

  /// Set the window to be resizable
  WINDOW_FLAGS_RESIZABLE           = 1 << 1,

  /// The window will gain focus on creation 
  WINDOW_FLAGS_FOCUS_ON_CREATE     = 1 << 2, 

  /// The window will always gain focus when shown 
  WINDOW_FLAGS_FOCUS_ON_SHOW       = 1 << 3, 

  /// Minimize the window on creation 
  WINDOW_FLAGS_MINIMIZE            = 1 << 4, 
  
  /// Maxmize the window on creation 
  WINDOW_FLAGS_MAXMIZE             = 1 << 5, 
  
  /// Disable window decoration such as borders, widgets, etc. 
  /// Decorations will be set to enabled by default.
  WINDOW_FLAGS_DISABLE_DECORATIONS = 1 << 6,
  
  /// Center the mouse position relative to the screen on startup 
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
/// Creates a new window context and returns the result. 
/// Returns a `nullptr` if the window was failed to be opened. 
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
///   - `WINDOW_FLAGS_DISABLE_DECORATIONS` = Disable window decoration such as borders, widgets, etc. Decorations will be set to enabled by default.
///   - `WINDOW_FLAGS_CENTER_MOUSE`        = Center the mouse position relative to the screen on startup.
///   - `WINDOW_FLAGS_HIDE_CURSOR`         = Hide the cursor at creation. The cursos will be shown by default.
///   - `WINDOW_FLAGS_FULLSCREEN`          = Set the window to be fullscreen on creation. 
/// 
NIKOLA_API Window* window_open(const i8* title, const i32 width, const i32 height, i32 flags);

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

/// Returns `true` if the `window` context is currently in fullscreen mode
NIKOLA_API const bool window_is_fullscreen(const Window* window);

/// Returns `true` if the `window` context is currently focused
NIKOLA_API const bool window_is_focused(const Window* window);

/// Returns `true` if the `window` context is currently shown
NIKOLA_API const bool window_is_shown(const Window* window);

/// Retrieve the current size of the `window` context
NIKOLA_API void window_get_size(const Window* window, i32* width, i32* height);

/// Retrieve the internal native handle of `window`.
NIKOLA_API void* window_get_handle(const Window* window);

/// Retrieve the current title of the `window` context
NIKOLA_API const i8* window_get_title(const Window* window);

/// Retrieve the current size of the monitor
NIKOLA_API void window_get_monitor_size(const Window* window, i32* width, i32* height);

/// Retrieve the aspect ratio of the `window` context
NIKOLA_API const f32 window_get_aspect_ratio(const Window* window);

/// Retrieve the refresh rate of the monitor 
NIKOLA_API const f32 window_get_refresh_rate(const Window* window);

/// Retrieve the set window flags of `window`
NIKOLA_API const WindowFlags window_get_flags(const Window* window);

/// Retrieve the current position of the `window` context relative to the monitor
NIKOLA_API void window_get_position(const Window* window, i32* x, i32* y);

/// Set the given `window` as the current active context
NIKOLA_API void window_set_current_context(Window* window);

/// Either disable or enable fullscreen mode on the `window` context.
NIKOLA_API void window_set_fullscreen(Window* window, const bool fullscreen);

/// Either show or hide the `window` context.
NIKOLA_API void window_set_show(Window* window, const bool show);

/// Set the size of the `window` to `width` and `height`.
NIKOLA_API void window_set_size(Window* window, const i32 width, const i32 height);

/// Set the title of the `window` to `title`.
NIKOLA_API void window_set_title(Window* window, const i8* title);

/// Set the position of the `window` to `x_pos` and `y_pos`..
NIKOLA_API void window_set_position(Window* window, const i32 x_pos, const i32 y_pos);

/// Window functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Window ***
/// ---------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// *** Clock ***

///---------------------------------------------------------------------------------------------------------------------
/// Clock functions

/// @NOTE: Every function has an "ni" prefix to avoid any confusion with the C clock library.

/// Updates the values of the time, FPS (frames per second), and the delta time.
/// 
/// @NOTE: This must be called every frame.
NIKOLA_API void niclock_update();

/// Retrieve the time passed since the CPU was turned on.
NIKOLA_API const f64 niclock_get_time(); 

/// Retrieve the current FPS (frames per second) of the application
NIKOLA_API const f64 niclock_get_fps();

/// Retrieve the time passed between each frame. 
NIKOLA_API const f64 niclock_get_delta_time();

/// Clock functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Clock ***
/// ---------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
