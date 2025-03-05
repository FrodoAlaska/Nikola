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

#define NIKOLA_BUILD_DEBUG   1 
#define NIKOLA_BUILD_RELEASE 0

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

  /// Set the graphics context to be hardware accelerated (i.e either using OpenGL or DirectX).
  WINDOW_FLAGS_GFX_HARDWARE        = 1 << 10,
  
  /// Set the graphics context to be software rendered
  WINDOW_FLAGS_GFX_SOFTWARE        = 1 << 11,
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
///   - `WINDOW_FLAGS_GFX_HARDWARE`        = Set the graphics context to be hardware accelerated (i.e either using OpenGL or DirectX).
///   - `WINDOW_FLAGS_GFX_SOFTWARE`        = Set the graphics context to be software rendered
/// 
NIKOLA_API Window* window_open(const i8* title, const i32 width, const i32 height, i32 flags);

/// Closes the `window` context and clears up any memory.
NIKOLA_API void window_close(Window* window);

/// Poll events from the `window` context.
NIKOLA_API void window_poll_events(Window* window);

/// Swap the internal buffer of the `window` context. 
/// This might have no effect on some platforms.
NIKOLA_API void window_swap_buffers(Window* window);

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

/// ---------------------------------------------------------------------
/// *** Graphics ***

///---------------------------------------------------------------------------------------------------------------------
// Consts

/// The maximum amount of textures the GPU supports at a time. 
const sizei TEXTURES_MAX                = 32;

/// The maximum amount of attachments that can be attached to a framebuffer. 
const sizei FRAMEBUFFER_ATTACHMENTS_MAX = 8;

/// The maximum amount of textures the GPU supports at a time. 
const sizei CUBEMAPS_MAX                = 5;

/// The maximum amount of faces in a cubemap
const sizei CUBEMAP_FACES_MAX           = 6;

/// The maximum amount of uniform buffers to be created in a shader type.
const sizei UNIFORM_BUFFERS_MAX         = 16;

/// The maximum number of elements a buffer's layout can have.
const sizei LAYOUT_ELEMENTS_MAX         = 32;

/// The maximum number of render targets to be bound at once.
const sizei RENDER_TARGETS_MAX          = 8;

// Consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxStates
enum GfxStates {
  /// Enable the depth testing pass.
  GFX_STATE_DEPTH   = 2 << 0, 
  
  /// Enable the stencil testing pass. 
  GFX_STATE_STENCIL = 2 << 1, 

  /// Enable blending.
  GFX_STATE_BLEND   = 2 << 2, 

  /// Enable multisampling. 
  GFX_STATE_MSAA    = 2 << 3, 

  /// Enable face culling. 
  GFX_STATE_CULL    = 2 << 4,
};
/// GfxStates
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxCompareFunc
enum GfxCompareFunc {
  /// Always passes the comparison.
  GFX_COMPARE_ALWAYS        = 3 << 0,

  /// Never passes the comparison.
  GFX_COMPARE_NEVER         = 3 << 1,

  /// Passes the comparison when the `a < b`.
  GFX_COMPARE_LESS          = 3 << 2, 

  /// Passes the comparison when the `a <= b`.
  GFX_COMPARE_LESS_EQUAL    = 3 << 3, 

  /// Passes the comparison when the `a > b`.
  GFX_COMPARE_GREATER       = 3 << 4, 

  /// Passes the comparison when the `a >= b`.
  GFX_COMPARE_GREATER_EQUAL = 3 << 5, 

  /// Passes the comparison when the `a != b`.
  GFX_COMPARE_NOT_EQUAL     = 3 << 6,
};
/// GfxCompareFunc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxOperation 
enum GfxOperation {
  /// Always keep the value.
  GFX_OP_KEEP      = 4 << 0, 
  
  /// Always set the value to `0`.
  GFX_OP_ZERO      = 4 << 1, 

  /// Invert the value.
  GFX_OP_INVERT    = 4 << 2, 

  /// Replace the value `a` with the other value `b`.
  GFX_OP_REPLACE   = 4 << 3, 

  /// Increase the value by `1`.
  GFX_OP_INCR      = 4 << 4, 

  /// Decrease the value by `1`.
  GFX_OP_DECR      = 4 << 5, 

  /// Increase the value by `1` and wrap to the beginning when it reaches the maximum.
  GFX_OP_INCR_WRAP = 4 << 6, 

  /// Decrease the value by `1` and wrap to the end when it reaches the minimum.
  GFX_OP_DECR_WRAP = 4 << 7, 
};
/// GfxOperation 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBlendMode 
enum GfxBlendMode {
  /// Always keep the blended value at `0`.
  GFX_BLEND_ZERO               = 5 << 0,
  
  /// Always keep the blended value at `1`.
  GFX_BLEND_ONE                = 5 << 1, 
  
  /// Take the source's color as the blended value.
  GFX_BLEND_SRC_COLOR          = 5 << 2,
  
  /// Take the destination's color as the blended value.
  GFX_BLEND_DEST_COLOR         = 5 << 3, 
  
  /// Take the source's alpha value as the blended value.
  GFX_BLEND_SRC_ALPHA          = 5 << 4, 
  
  /// Take the destination's alpha value as the blended value.
  GFX_BLEND_DEST_ALPHA         = 5 << 5, 
  
  /// Take the inverse of the source's color as the blended value.
  GFX_BLEND_INV_SRC_COLOR      = 5 << 6, 
  
  /// Take the inverse of the destination's color as the blended value.
  GFX_BLEND_INV_DEST_COLOR     = 5 << 7, 
  
  /// Take the inverse of the source's alpha value as the blended value.
  GFX_BLEND_INV_SRC_ALPHA      = 5 << 8, 
  
  /// Take the inverse of the destination's alpha value as the blended value.
  GFX_BLEND_INV_DEST_ALPHA     = 5 << 9,
  
  /// Take the saturated alpha value of the source's color as the blended value.
  GFX_BLEND_SRC_ALPHA_SATURATE = 5 << 10,
};
/// GfxBlendMode 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxCullMode
enum GfxCullMode {
  /// Only cull the front faces.
  GFX_CULL_FRONT          = 6 << 0,
  
  /// Only cull the back faces.
  GFX_CULL_BACK           = 6 << 1,
  
  /// Cull both the front and back faces.
  GFX_CULL_FRONT_AND_BACK = 6 << 2,
};
/// GfxCullMode
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxCullOrder
enum GfxCullOrder {
  /// Clockwise vertices are the front faces.
  GFX_ORDER_CLOCKWISE         = 7 << 0, 
  
  /// Counter-clockwise vertices are the front faces.
  GFX_ORDER_COUNTER_CLOCKWISE = 7 << 1,
};
/// GfxCullOrder
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
enum GfxClearFlags {
  /// No buffers will be cleared.
  GFX_CLEAR_FLAGS_NONE                   = 3 << 0,
  
  /// Clear the color buffer of the current context. 
  GFX_CLEAR_FLAGS_COLOR_BUFFER     = 3 << 1,

  /// Clear the depth buffer of the current context. 
  ///
  /// @NOTE: This flag will be ignored if the depth state is disabled.
  GFX_CLEAR_FLAGS_DEPTH_BUFFER     = 3 << 2,
  
  /// Clear the stencil buffer of the current context. 
  ///
  /// @NOTE: This flag will be ignored if the stencil state is disabled.
  GFX_CLEAR_FLAGS_STENCIL_BUFFER   = 3 << 3,
};
/// GfxContextFlags 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBufferType
enum GfxBufferType {
  /// A vertex buffer.
  GFX_BUFFER_VERTEX  = 4 << 0, 

  /// An index buffer.
  GFX_BUFFER_INDEX   = 4 << 1, 

  /// A uniform buffer.
  GFX_BUFFER_UNIFORM = 4 << 2,
};
/// GfxBufferType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBufferUsage
enum GfxBufferUsage {
  /// Set the buffer to be dynamically written to.
  /// This will be used for frequently writing to the buffer.
  GFX_BUFFER_USAGE_DYNAMIC_DRAW = 5 << 0,
  
  /// Set the buffer to be dynamically read from.
  /// This will be used for frequent reading from the buffer.
  GFX_BUFFER_USAGE_DYNAMIC_READ = 5 << 1,

  /// Set the buffer to be statically written to.
  /// This will be used for writing to the buffer once or rarely.
  GFX_BUFFER_USAGE_STATIC_DRAW  = 5 << 2,

  /// Set the buffer to be statically read from.
  /// This will be used for reading from the buffer once or rarely.
  GFX_BUFFER_USAGE_STATIC_READ  = 5 << 3,
};
/// GfxBufferUsage
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxDrawMode
enum GfxDrawMode {
  /// Will set up the pipeline to draw points.
  GFX_DRAW_MODE_POINT          = 6 << 0,

  /// Will set up the pipeline to draw triangles.
  GFX_DRAW_MODE_TRIANGLE       = 6 << 1,
  
  /// Will set up the pipeline to draw triangle strips.
  GFX_DRAW_MODE_TRIANGLE_STRIP = 6 << 2,
  
  /// Will set up the pipeline to draw lines.
  GFX_DRAW_MODE_LINE           = 6 << 3,
  
  /// Will set up the pipeline to draw line strips.
  GFX_DRAW_MODE_LINE_STRIP     = 6 << 4,
};
/// GfxDrawMode
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxLayoutType
enum GfxLayoutType {
  /// Equivalent to `float`.
  GFX_LAYOUT_FLOAT1 = 7 << 0,
  
  /// Equivalent to `fVector2`.
  GFX_LAYOUT_FLOAT2 = 7 << 1,
  
  /// Equivalent to `fVector3`.
  GFX_LAYOUT_FLOAT3 = 7 << 2,
  
  /// Equivalent to `fVector4`.
  GFX_LAYOUT_FLOAT4 = 7 << 3,
  
  /// Equivalent to `int`.
  GFX_LAYOUT_INT1   = 7 << 4,
  
  /// Equivalent to `iVector2`.
  GFX_LAYOUT_INT2   = 7 << 5,
  
  /// Equivalent to `iVector3`.
  GFX_LAYOUT_INT3   = 7 << 6,
  
  /// Equivalent to `iVector4`.
  GFX_LAYOUT_INT4   = 7 << 7,
  
  /// Equivalent to `unsigned int`.
  GFX_LAYOUT_UINT1  = 7 << 8,
  
  /// Equivalent to `uVector2`.
  GFX_LAYOUT_UINT2  = 7 << 9,
  
  /// Equivalent to `uVector3`.
  GFX_LAYOUT_UINT3  = 7 << 10,
  
  /// Equivalent to `uVector4`.
  GFX_LAYOUT_UINT4  = 7 << 11,

  /// A 2x2 matrix (or 4 `float`s).
  GFX_LAYOUT_MAT2   = 7 << 12,
  
  /// A 3x3 matrix (or 9 `float`s).
  GFX_LAYOUT_MAT3   = 7 << 13,
  
  /// A 4x4 matrix (or 16 `float`s).
  GFX_LAYOUT_MAT4   = 7 << 14,
};
/// GfxLayoutType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTextureType
enum GfxTextureType {
  /// Creates a 1D texture.
  GFX_TEXTURE_1D                   = 8 << 0,
  
  /// Creates a 2D texture.
  GFX_TEXTURE_2D                   = 8 << 1,
  
  /// Creates a 3D texture.
  GFX_TEXTURE_3D                   = 8 << 2,
  
  /// Creates a texture to be used as a render target.
  GFX_TEXTURE_RENDER_TARGET        = 8 << 3,
  
  /// Creates a texture to be used as the depth target.
  GFX_TEXTURE_DEPTH_TARGET         = 8 << 4,
  
  /// Creates a texture to be used as the stencil target.
  GFX_TEXTURE_STENCIL_TARGET       = 8 << 5,
  
  /// Creates a texture to be used as both the depth and stencil target.
  GFX_TEXTURE_DEPTH_STENCIL_TARGET = 8 << 6,
};
/// GfxTextureType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTextureFormat
enum GfxTextureFormat {
  /// An `unsigned char` per pixel red channel texture format.
  GFX_TEXTURE_FORMAT_R8                 = 9 << 0,
  
  /// An `unsigned short` per pixel red channel texture format.
  GFX_TEXTURE_FORMAT_R16                = 9 << 1,
  
  /// A `half float` per pixel red channel texture format.
  GFX_TEXTURE_FORMAT_R16F               = 9 << 2,
  
  /// A `float` per pixel red channel texture format.
  GFX_TEXTURE_FORMAT_R32F               = 9 << 3,

  /// An `unsigned char` per pixel red and green channel texture format.
  GFX_TEXTURE_FORMAT_RG8                = 9 << 4,
  
  /// An `unsigned short` per pixel red and green channel texture format.
  GFX_TEXTURE_FORMAT_RG16               = 9 << 5,
  
  /// A `half float` per pixel red and green channel texture format.
  GFX_TEXTURE_FORMAT_RG16F              = 9 << 6,
  
  /// A `float` per pixel red and green channel texture format.
  GFX_TEXTURE_FORMAT_RG32F              = 9 << 7,
  
  /// An `unsigned char` per pixel red, green, blue, and alpha channel texture format.
  GFX_TEXTURE_FORMAT_RGBA8              = 9 << 8,
  
  /// An `unsigned short` bits per pixel red, green, blue, and alpha channel texture format.
  GFX_TEXTURE_FORMAT_RGBA16             = 9 << 9,
  
  /// A `half float` per pixel red, green, blue, and alpha channel texture format.
  GFX_TEXTURE_FORMAT_RGBA16F            = 9 << 10,
  
  /// A `float` per pixel red, green, blue, and alpha channel texture format.
  GFX_TEXTURE_FORMAT_RGBA32F            = 9 << 11,

  /// A format to be used with the depth and stencil buffers where 
  /// the depth buffer gets 24 bits and the stencil buffer gets 8 bits.
  GFX_TEXTURE_FORMAT_DEPTH_STENCIL_24_8 = 9 << 12,
};
/// GfxTextureFromat
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTextureFilter
enum GfxTextureFilter {
  /// Uses linear filtering in both modes.
  GFX_TEXTURE_FILTER_MIN_MAG_LINEAR            = 10 << 0,
  
  /// Uses nearest filtering on both modes.
  GFX_TEXTURE_FILTER_MIN_MAG_NEAREST           = 10 << 1,
  
  /// Uses linear filtering on minification and nearest filtering magnification. 
  GFX_TEXTURE_FILTER_MIN_LINEAR_MAG_NEAREST    = 10 << 2,
  
  /// Uses nearest filtering on minification and linear filtering magnification. 
  GFX_TEXTURE_FILTER_MIN_NEAREST_MAG_LINEAR    = 10 << 3,
  
  /// Uses trilinear filtering (the weighted average of the two closest mipmaps)
  /// on minification and linear filtering on magnification.
  GFX_TEXTURE_FILTER_MIN_TRILINEAR_MAG_LINEAR  = 10 << 4,
  
  /// Uses trilinear filtering (the weighted average of the two closest mipmaps)
  /// on minification and nearest filtering on magnification.
  GFX_TEXTURE_FILTER_MIN_TRILINEAR_MAG_NEAREST = 10 << 5,
};
/// GfxTextureFilter
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTextureWrap
enum GfxTextureWrap {
  /// Repeat the pixel when wrapped.
  GFX_TEXTURE_WRAP_REPEAT       = 11 << 0, 
  
  /// Mirror the pixel when wrapped.
  GFX_TEXTURE_WRAP_MIRROR       = 11 << 1, 
  
  /// Clamp the pixel when wrapped.
  GFX_TEXTURE_WRAP_CLAMP        = 11 << 2, 
  
  /// Use the border color when wrapped.
  GFX_TEXTURE_WRAP_BORDER_COLOR = 11 << 3,
};
/// GfxTextureWrap
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxShaderType
enum GfxShaderType {
  /// A vertex shader.
  GFX_SHADER_VERTEX   = 12 << 0, 

  /// A pixel/fragment shader.
  GFX_SHADER_PIXEL    = 12 << 1, 

  /// A geometry shader.
  GFX_SHADER_GEOMETRY = 12 << 2,
};
/// GfxShaderType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxContext
struct GfxContext; 
/// GfxContext
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxFramebuffer
struct GfxFramebuffer;
/// GfxFramebuffer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBuffer
struct GfxBuffer;
/// GfxBuffer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxShader
struct GfxShader;
/// GfxShader
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTexture
struct GfxTexture;
/// GfxTexture
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxCubemap
struct GfxCubemap;
/// GfxCubemap
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxPipeline
struct GfxPipeline;
/// GfxPipeline
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxDepthDesc
struct GfxDepthDesc {
  /// The comparison funcion of the depth buffer.
  /// The default value is `GFX_COMPARE_LESS_EQUAL`.
  GfxCompareFunc compare_func = GFX_COMPARE_LESS_EQUAL;

  /// Enables/disables writing to the depth buffer.
  /// The default value is `true`.
  bool depth_write_enabled    = true;
};
/// GfxDepthDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxStencilDesc
struct GfxStencilDesc {
  /// Which of the polygon faces will the below operations
  /// affect on. 
  /// The default value is "GFX_FACE_FRONT_AND_BACK".
  GfxCullMode polygon_face     = GFX_CULL_FRONT_AND_BACK; 

  /// The comparison function of the stencil buffer.
  /// The default value is `GFX_COMPARE_ALWAYS`.
  GfxCompareFunc compare_func  = GFX_COMPARE_ALWAYS;

  /// The operation to carry when the stencil test fails. 
  /// The default value is `GFX_OP_KEEP`.
  GfxOperation stencil_fail_op = GFX_OP_KEEP;

  /// The operation to be carried out when the depth test succeeds. 
  /// The default value is `GFX_OP_KEEP`.
  GfxOperation depth_pass_op   = GFX_OP_KEEP;

  /// The operation to be carried out when the depth test fails. 
  /// The default value is `GFX_OP_KEEP`.
  GfxOperation depth_fail_op   = GFX_OP_KEEP;

  /// The reference value of the stencil test. 
  /// The default value is `1`.
  i32 ref                      = 1;

  /// The mask that will be bitwise `AND`ed with the `ref` and the stencil value 
  /// currently in the buffer. The two resulting `AND`ded values will then be compared 
  /// to determine the outcome of that pixel.
  ///
  /// The default value is `0xff`.
  u32 mask                     = 0xff;
};
/// GfxStencilDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBlendDesc
struct GfxBlendDesc {
  /// The blend mode of the RGB of the source's color. 
  /// The default value is `GFX_BLEND_ONE`.
  GfxBlendMode src_color_blend  = GFX_BLEND_ONE; 

  /// The blend mode of the RGB of the destination's color. 
  /// The default value is `GFX_BLEND_ZERO`.
  GfxBlendMode dest_color_blend = GFX_BLEND_ZERO; 

  /// The blend mode of the Alpha value of the source's color. 
  /// The default value is `GFX_BLEND_SRC_ALPHA`.
  GfxBlendMode src_alpha_blend  = GFX_BLEND_SRC_ALPHA; 

  /// The blend mode of the Alpha value of the destination's color. 
  /// The default value is `GFX_BLEND_INV_SRC_ALPHA`.
  GfxBlendMode dest_alpha_blend = GFX_BLEND_INV_SRC_ALPHA; 

  /// The default blend factor. 
  /// The default values are `R = 0, G = 0, B = 0, A = 0`.
  f32 blend_factor[4]           = {0, 0, 0, 0};
};
/// GfxBlendDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxCullDesc 
struct GfxCullDesc {
  /// The face to cull. 
  /// If this value is set to `GFX_CULL_FRONT_AND_BACK`, then 
  /// all the polygons will be culled. 
  /// 
  /// @NOTE: By default, this value is set to `GFX_CULL_FRONT`.
  GfxCullMode cull_mode  = GFX_CULL_FRONT;

  /// Determines the front-facing triangle. 
  /// If this value is set to `GFX_CULL_CLOCKWISE`, then
  /// the triangles with clockwise-ordering vertices are considered 
  /// front-facing and back-facing otherwise. The opposite is true 
  /// with `GFX_CULL_COUNTER_CLOCKWISE`.
  /// 
  /// @NOTE: The default value is `GFX_CULL_CLOCKWISE`.
  GfxCullOrder front_face = GFX_ORDER_CLOCKWISE; 
};
/// GfxCullDesc 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxContextDesc 
struct GfxContextDesc {
  /// A reference to the window.
  /// 
  /// @NOTE: This _must_ be set to a valid value.
  Window* window                = nullptr;

  /// A bitwise ORed value from `GfxStates` determining the 
  /// states to create/enable.
  /// 
  /// @NOTE: By default, no states are set. 
  u32 states                    = 0;

  /// When set to `true`, the context will enable vsync. 
  /// Otherwise, vsync will be turned off.
  ///
  /// @NOTE: By default, this value is set to `false`.
  bool has_vsync                = false;

  /// The subsamples of the MSAA buffer. 
  /// 
  /// @NOTE: By default, this is set to `1`.
  u32 msaa_samples              = 1;

  /// The description of the depth state. 
  /// 
  /// @NOTE: Check `GfxDepthDesc` to know the default values
  /// of each member.
  GfxDepthDesc depth_desc       = {}; 

  /// The description of the stencil state. 
  /// 
  /// @NOTE: Check `GfxStencilDesc` to know the default values
  /// of each member.
  GfxStencilDesc stencil_desc   = {};

  /// The description of the blend state. 
  /// 
  /// @NOTE: Check `GfxBlendDesc` to know the default values
  /// of each member.
  GfxBlendDesc blend_desc       = {};

  /// The description of the cull state. 
  /// 
  /// @NOTE: Check `GfxCullDesc` to know the default values
  /// of each member.
  GfxCullDesc cull_desc         = {};
};
/// GfxContextDesc 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxFramebufferDesc
struct GfxFramebufferDesc {
  /// The RGBA values of the clear color.
  ///
  /// @NOTE: By default, the clear color is set to `{0, 0, 0, 0}`.
  f32 clear_color[4] = {0, 0, 0, 0};

  /// A bitwise ORed value `GfxClearFlags` that determine 
  /// which buffers to clear every frame. 
  u32 clear_flags       = 0;

  /// An array of attachments up to `FRAMEBUFFER_ATTACHMENTS_MAX`. 
  ///
  /// @NOTE: Each texture in this array have one of the texture types `GFX_TEXTURE_*_TARGET`. 
  GfxTexture* attachments[FRAMEBUFFER_ATTACHMENTS_MAX];

  /// The amount of attachments in the `attachments` array.
  sizei attachments_count = 0;
};
/// GfxFramebufferDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBufferDesc
struct GfxBufferDesc {
  /// The data that will be sent to the GPU.
  void* data = nullptr; 

  /// The size of `data` in bytes.
  sizei size;
  
  /// Notify the type of the buffer to the GPU.
  GfxBufferType type;  

  /// Set up how the buffer will be used depending on the usage. 
  GfxBufferUsage usage;
};
/// GfxBufferDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxShaderDesc
struct GfxShaderDesc {
  /// The full source code for the vertex shader. 
  const i8* vertex_source = nullptr;

  /// The full source code for the pixel/fragment shader. 
  const i8* pixel_source  = nullptr;
};
/// GfxShaderDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxLayoutDesc
struct GfxLayoutDesc {
  /// The name of the layout attribute. 
  ///
  /// @NOTE: This can be left blank for OpenGL.
  const i8* name; 

  /// The type of the layout.
  GfxLayoutType type; 

  /// If this value is set to `0`, the layout will 
  /// be sent immediately to the shader. However, 
  /// if it is set to a value >= `1`, the layout 
  /// will be sent after the nth instance. 
  u32 instance_rate = 0;
};
/// GfxLayoutDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxUniformDesc
struct GfxUniformDesc {
  /// The type of the shader the uniform will be 
  /// uploaded to. 
  GfxShaderType shader_type;

  /// The index of the uniform buffer. 
  sizei index; 

  /// The data that will be sent to the shader.
  void* data;

  /// The size of `data`.
  sizei size;
};
/// GfxUniformDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTextureDesc
struct GfxTextureDesc {
  /// The overall size of the texture.
  u32 width, height; 
  
  /// The depth on the Z-axis of the texture. 
  ///
  /// If the texture `type` is anything other than `GFX_TEXTURE_3D`, 
  /// the `depth` member will be ignored.
  u32 depth;

  /// The mipmap level of the texture. 
  ///
  /// @NOTE: Leave this as `1` if the mipmap levels are not important.
  u32 mips; 

  /// The type of the texture to be used.
  GfxTextureType type;

  /// The pixel format of the texture.
  GfxTextureFormat format;

  /// The filter to be used on the texture when magnified or minified.
  GfxTextureFilter filter;

  /// The addressing mode of the texture.
  GfxTextureWrap wrap_mode;
  
  /// The pixels that will be sent to the GPU.
  void* data = nullptr;
};
/// GfxTextureDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxCubemapDesc
struct GfxCubemapDesc {
  /// The overall size of the cubemap.
  u32 width, height; 

  /// The mipmap level of the cubemap. 
  ///
  /// @NOTE: Leave this as `0` if the depth is not important.
  u32 mips; 

  /// The pixel format of the cubemap.
  GfxTextureFormat format;

  /// The filter to be used on the cubemap when magnified or minified.
  GfxTextureFilter filter;

  /// The addressing mode of the cubemap.
  GfxTextureWrap wrap_mode;
  
  /// An array of pixels (up to `CUBEMAP_FACES_MAX`) of each face of the cubemap.
  void* data[CUBEMAP_FACES_MAX]; 

  /// The amount of faces of the cubemap to use in `data`.
  sizei faces_count = 0;
};
/// GfxCubemapDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxPipelineDesc
struct GfxPipelineDesc {
  /// The vertex buffer to be used in a `draw_vertex` command.
  ///
  /// @NOTE: This buffer _must_ be set. It cannot be left a `nullptr`.
  /// Even if `draw_index` is being used.
  GfxBuffer* vertex_buffer           = nullptr; 

  /// The amount of vertices in the `vertex_buffer` to be drawn. 
  sizei vertices_count               = 0;
  
  /// The index buffer to be used in a `draw_index` command.
  ///
  /// @NOTE: This buffer _must_ be set if a `draw_index` command is used.
  /// Otherwise, it can be left as `nullptr`.
  GfxBuffer* index_buffer            = nullptr;

  /// The amount of indices in the `index_buffer` to be drawn.
  sizei indices_count                = 0;

  /// The shader to be used in the draw command.
  GfxShader* shader                  = nullptr;
  
  /// Layout array up to `LAYOUT_ELEMENTS_MAX` describing each layout attribute.
  GfxLayoutDesc layout[LAYOUT_ELEMENTS_MAX];

  /// The amount of layouts to be set in `layout`.
  sizei layout_count                 = 0; 

  /// The draw mode of the entire pipeline.
  ///
  /// @NOTE: This can be changed at anytime before the draw command.
  GfxDrawMode draw_mode;
 
  /// Array of cubmaps up to `CUBEMAPS_MAX` to be used in a draw command.
  GfxCubemap* cubemaps[CUBEMAPS_MAX] = {nullptr};

  /// The amout of cubemaps to be used in `cubemaps`.
  sizei cubemaps_count               = 0;

  /// Array of textures up to `TEXTURES_MAX` to be used in a draw command. 
  GfxTexture* textures[TEXTURES_MAX] = {nullptr};

  /// The amount of textures to be used in `textures`.
  sizei textures_count               = 0;
 
  /// A flag to indicate if the pipeline can 
  /// or cannot write to the depth buffer. 
  ///
  /// @NOTE: By default, this value is `true`.
  bool depth_mask                    = true;

  /// The stencil reference value of the pipeline. 
  ///
  /// @NOTE: This is `1` by default.
  u32 stencil_ref                    = 1;
  
  /// The blend factor to be used in the pipeline. 
  ///
  /// @NOTE: This is `{0, 0, 0, 0}` by default.
  f32 blend_factor[4]                = {0, 0, 0, 0};
};
/// GfxPipelineDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Context functions 

/// Initialize the `GfxContext` struct and return it. 
/// This function will handle the initialization of the underlying graphics API.
/// Setting up any states, creating the graphics devices, setting the pixel format of the swapchain, and so on.
///  
/// Any information describing the many states will be taken from `desc`.
/// 
/// If any errors occur during initialization, this function will return a `nullptr`.
/// 
/// Check out the `GfxContextDesc` struct for more details on its members 
/// that will be passed into this function. All of the members have default values. 
/// However, two members _have_ to be set. Mainly, `window` and `states`.
/// 
/// The `window` is a pointer to the previously created `Window` struct.
/// The `states` flags of type `GfxStates` can be `OR`ed together which will 
/// indicate the initial active states of the context.
/// 
/// This function will also save a reference internally to `desc`.
/// 
/// @NOTE: Later on, with any function, if an instance of `GfxContext` 
/// is passed as a `nullptr`, the function will assert. 
NIKOLA_API GfxContext* gfx_context_init(const GfxContextDesc& desc);

/// Free/reclaim any memory `gfx` has consumed. 
/// This function will do any required de-initialization of the graphics API.
NIKOLA_API void gfx_context_shutdown(GfxContext* gfx);

/// Retrieve the internal `GfxContextDesc` of `gfx`
NIKOLA_API GfxContextDesc& gfx_context_get_desc(GfxContext* gfx);

/// Set any `state` of the context `gfx` to `value`. 
/// i.e, this function can turn on or off the `state` in the given `gfx` context.
NIKOLA_API void gfx_context_set_state(GfxContext* gfx, const GfxStates state, const bool value);

/// Clear the context given the information in `framebuffer`. The clear flags 
/// as well as the clear color will be sampled from `framebuffer`. However, 
/// if `framebuffer` is set to `nullptr`, the context will clear the default 
/// framebuffer instead.
NIKOLA_API void gfx_context_clear(GfxContext* gfx, GfxFramebuffer* framebuffer);

/// Apply the `pipeline` using the updated `pipe_desc` in the current `gfx`. 
/// This function will update the references of the shader, buffers, and textures in `pipeline` using `pipe_desc` 
/// as well as set up all the currently active states.
NIKOLA_API void gfx_context_apply_pipeline(GfxContext* gfx, GfxPipeline* pipeline, const GfxPipelineDesc& pipe_desc);

/// Switch to the back buffer or, rather, present the back buffer to the screen. 
/// 
/// @NOTE: This function will be affected by vsync. 
NIKOLA_API void gfx_context_present(GfxContext* gfx);

/// Context functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Framebuffer functions

/// Allocate and return a `GfxFramebuffer` object, using the information in `desc`. 
NIKOLA_API GfxFramebuffer* gfx_framebuffer_create(GfxContext* gfx, const GfxFramebufferDesc& desc);

/// Free/reclaim any memory taken by `framebuffer`.
NIKOLA_API void gfx_framebuffer_destroy(GfxFramebuffer* framebuffer);

/// Retrieve the internal `GfxFramebufferDesc` of `framebuffer`
NIKOLA_API GfxFramebufferDesc& gfx_framebuffer_get_desc(GfxFramebuffer* framebuffer);

/// Framebuffer functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Buffer functions 

/// Allocate and return a `GfxBuffer` object, using the information in `desc`.
NIKOLA_API GfxBuffer* gfx_buffer_create(GfxContext* gfx, const GfxBufferDesc& desc);

/// Free/reclaim any memory taken by `buff`.
NIKOLA_API void gfx_buffer_destroy(GfxBuffer* buff);

/// Retrieve the internal `GfxBufferDesc` of `buffer`
NIKOLA_API GfxBufferDesc& gfx_buffer_get_desc(GfxBuffer* buffer);

/// Update the contents of `buff` starting at `offset` with `data` of size `size`.
NIKOLA_API void gfx_buffer_update(GfxBuffer* buff, const sizei offset, const sizei size, const void* data);

/// Buffer functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Shader functions 

/// Allocate and return a `GfxShader`, using the information given in `desc`. 
NIKOLA_API GfxShader* gfx_shader_create(GfxContext* gfx, const GfxShaderDesc& desc);

/// Free/reclaim any memory consumed by `shader`.
NIKOLA_API void gfx_shader_destroy(GfxShader* shader);

/// Retrieve the internal `GfxShaderDesc` of `shader`.
NIKOLA_API GfxShaderDesc& gfx_shader_get_source(GfxShader* shader);

/// Attaches the uniform `buffer` to the `shader` of type `type` to point `bind_point`. 
/// Any updates to `buffer` will have an effect on the `shader`.
/// 
/// @NOTE: For GLSL (OpenGL), you _need_ to specify the binding point of the uniform buffer in the shader itself. For example, 
/// do something like, `layout (std140, binding = 0)`. Now the uniform buffer will be bound to the point `0` and the shader 
/// can easily find it. 
NIKOLA_API void gfx_shader_attach_uniform(GfxShader* shader, const GfxShaderType type, GfxBuffer* buffer, const u32 bind_point);

/// Lookup the `uniform_name` in `shader` and retrieve its location. 
///
/// @NOTE: If `uniform_name` does NOT exist in `shader`, the returned value will be `-1`.
NIKOLA_API i32 gfx_shader_uniform_lookup(GfxShader* shader, const i8* uniform_name);

/// Upload a uniform array with `count` elements of type `type` with `data` at `location` to `shader`. 
NIKOLA_API void gfx_shader_upload_uniform_array(GfxShader* shader, const i32 location, const sizei count, const GfxLayoutType type, const void* data);

/// Upload a uniform of type `type` with `data` at `location` to `shader`. 
NIKOLA_API void gfx_shader_upload_uniform(GfxShader* shader, const i32 location, const GfxLayoutType type, const void* data);

/// Shader functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Texture functions 

/// Allocate and return a `GfxTexture` from the information provided by `desc`. 
NIKOLA_API GfxTexture* gfx_texture_create(GfxContext* gfx, const GfxTextureDesc& desc);

/// Reclaim/free any memory allocated by `texture`.
NIKOLA_API void gfx_texture_destroy(GfxTexture* texture);

/// Retrieve the internal `GfxTextureDesc` of `texture`
NIKOLA_API GfxTextureDesc& gfx_texture_get_desc(GfxTexture* texture);

/// Update the `texture`'s information from the given `desc`.
///
/// @NOTE: This will resend the pixels of `texture` to the GPU with the new information provided by `desc`.
NIKOLA_API void gfx_texture_update(GfxTexture* texture, const GfxTextureDesc& desc);

/// Texture functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Cubemap functions 

/// Allocate and return a `GfxCubemap` from the information provided by `desc`. 
NIKOLA_API GfxCubemap* gfx_cubemap_create(GfxContext* gfx, const GfxCubemapDesc& desc);

/// Reclaim/free any memory allocated by `texture`.
NIKOLA_API void gfx_cubemap_destroy(GfxCubemap* cubemap);

/// Retrieve the internal `GfxCubemapDesc` of `cubemap`
NIKOLA_API GfxCubemapDesc& gfx_cubemap_get_desc(GfxCubemap* cubemap);

/// Update the `cubemap`'s information from the given `desc`.
///
/// @NOTE: This will resend the pixels of `cubemap` to the GPU with the new information provided by `desc`.
NIKOLA_API void gfx_cubemap_update(GfxCubemap* cubemap, const GfxCubemapDesc& desc);

/// Cubemap functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Pipeline functions 

/// Allocate and return a `GfxPipeline` from the information provided by `desc`.
NIKOLA_API GfxPipeline* gfx_pipeline_create(GfxContext* gfx, const GfxPipelineDesc& desc);

/// Reclaim/free any memory allocated by `pipeline`.
NIKOLA_API void gfx_pipeline_destroy(GfxPipeline* pipeline);

/// Retrieve the internal `GfxPipelineDesc` of `pipeline`
NIKOLA_API GfxPipelineDesc& gfx_pipeline_get_desc(GfxPipeline* pipeline);

/// Draw the contents of the `vertex_buffer` in `pipeline`.
NIKOLA_API void gfx_pipeline_draw_vertex(GfxPipeline* pipeline);

/// Draw the contents of the `vertex_buffer` using the `index_buffer` in `pipeline`.
NIKOLA_API void gfx_pipeline_draw_index(GfxPipeline* pipeline);

/// Pipeline functions 
///---------------------------------------------------------------------------------------------------------------------

/// *** Graphics ***
/// ---------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// *** Audio ***
/// *** Audio ***
/// ---------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
