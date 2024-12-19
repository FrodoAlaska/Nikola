#pragma once

#include <cstddef>

#include <glm/glm.hpp>

//////////////////////////////////////////////////////////////////////////

namespace nikol { // Start of nikol
 
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
/// DEFS

/// Nikol only supports OpenGL versions greater than these.
#define NIKOL_GL_MINIMUM_MAJOR_VERSION 4
#define NIKOL_GL_MINIMUM_MINOR_VERSION 2

/// Nikol only supports Direct3D11 versions greater than these.
#define NIKOL_D3D11_MINIMUM_MAJOR_VERSION 11 
#define NIKOL_D3D11_MINIMUM_MINOR_VERSION 0 

/// DEFS
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Build Types ***

#define NIKOL_BUILD_DEBUG   1 
#define NIKOL_BUILD_RELEASE 0

/// *** Build Types ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Platform detection ***

/// Windows
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define NIKOL_PLATFORM_WINDOWS 1
#define NIKOL_GFX_CONTEXT_DX11
#define WIN32_LEAN_AND_MEAN
#ifndef _WIN64 
#error "[NIKOL-FATAL]: Only support 64-bit machines\n"
#endif 

/// Linux
#elif defined(__linux__) || defined(__gnu_linux__)
#define NIKOL_PLATFORM_LINUX     1 
#define NIKOL_GFX_CONTEXT_OPENGL
#endif

/// *** Platform detection ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Library init ***

///---------------------------------------------------------------------------------------------------------------------
/// Library functions

/// Initialze various different subsystems of Nikol. 
const bool init();

/// Shutdown subsystems of the Nikol.
void shutdown();

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
void* memory_allocate(const sizei size);

/// Re-allocate a block of memory `ptr` with a new size of `new_size`.
/// WARN: This function will assert if `ptr` is a `nullptr`.
void* memory_reallocate(void* ptr, const sizei new_size);

/// Set the value of the memory block `ptr` with a size of `ptr_size` to `value`.
/// WARN: This function will assert if `ptr` is a `nullptr`.
void* memory_set(void* ptr, const i32 value, const sizei ptr_size);

/// Set the value of the memory block `ptr` of size `ptr_size` to 0.
/// NOTE: This is equivalent to `memory_set(ptr, 0, ptr_size)`.
/// WARN: This function will assert if `ptr` is a `nullptr`.
void* memory_zero(void* ptr, const sizei ptr_size);

/// Allocate `count` blocks of memory each with the size of `block_size`.
/// NOTE: This is equivalent to `memory_allocate(block_size * count)`.
void* memory_blocks_allocate(const sizei count, const sizei block_size);

/// Copy `src_size` bytes of `src` to the memory block `dest`. 
/// WARN: This function will assert if `dest` or `src` are a `nullptr`.
void* memory_copy(void* dest, const void* src, const sizei src_size);

/// Free/reclaim the memory of the given `ptr`.
/// WARN: This function will assert if `ptr` is a `nullptr`.
void memory_free(void* ptr);

/// Retrieve the amount of allocations made so far.
const sizei memory_get_allocations_count();

/// Retrieve the amount of frees made so far. 
const sizei memory_get_frees_count();

/// Retrieve how many bytes have been allocated so far. 
const sizei memory_get_allocation_bytes();

/// Memory functions 
///---------------------------------------------------------------------------------------------------------------------

/// *** Memory ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Logger ***

#define NIKOL_LOG_INFO_ACTIVE 1 
#define NIKOL_LOG_WARN_ACTIVE 1 

// Only activate trace and debug logs on debug builds
#if NIKOL_BUILD_RELEASE == 1
#define NIKOL_LOG_TRACE_ACTIVE  0
#define NIKOL_LOG_DEBUG_ACTIVE 0
#else 
#define NIKOL_LOG_TRACE_ACTIVE  1
#define NIKOL_LOG_DEBUG_ACTIVE 1
#endif

///---------------------------------------------------------------------------------------------------------------------
/// Log level
enum LogLevel {
  LOG_LEVEL_TRACE = 0,
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
void logger_log_assert(const i8* expr, const i8* msg, const i8* file, const u32 line_num);

/// Log a specific log level with the given `msg` and any other parametars.
void logger_log(const LogLevel lvl, const i8* msg, ...);

/// Logger functions
///---------------------------------------------------------------------------------------------------------------------

/// Trace log
#if NIKOL_LOG_TRACE_ACTIVE == 1
#define NIKOL_LOG_TRACE(msg, ...) nikol::logger_log(nikol::LOG_LEVEL_TRACE, msg, ##__VA_ARGS__)
#else
#define NIKOL_LOG_TRACE(msg, ...)
#endif
/// Trace log

/// Debug log
#if NIKOL_LOG_DEBUG_ACTIVE == 1
#define NIKOL_LOG_DEBUG(msg, ...) nikol::logger_log(nikol::LOG_LEVEL_DEBUG, msg, ##__VA_ARGS__)
#else
#define NIKOL_LOG_DEBUG(msg, ...)
#endif
/// Debug log

/// Info log
#if NIKOL_LOG_INFO_ACTIVE == 1
#define NIKOL_LOG_INFO(msg, ...) nikol::logger_log(nikol::LOG_LEVEL_INFO, msg, ##__VA_ARGS__)
#else
#define NIKOL_LOG_INFO(msg, ...)
#endif
/// Info log

/// Warn log
#if NIKOL_LOG_WARN_ACTIVE == 1
#define NIKOL_LOG_WARN(msg, ...) nikol::logger_log(nikol::LOG_LEVEL_WARN, msg, ##__VA_ARGS__)
#else
#define NIKOL_LOG_WARN(msg, ...)
#endif
/// Warn log

/// Error log
#define NIKOL_LOG_ERROR(msg, ...) nikol::logger_log(nikol::LOG_LEVEL_ERROR, msg, ##__VA_ARGS__)
/// Error log

/// Fatal log
#define NIKOL_LOG_FATAL(msg, ...) nikol::logger_log(nikol::LOG_LEVEL_FATAL, msg, ##__VA_ARGS__)
/// Fatal log

/// *** Logger ***
/// ---------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// *** Asserts ***

#define NIKOL_ASSERTS_ENABLED // @TEMP 

#ifdef NIKOL_ASSERTS_ENABLED

// Debug break 
#if NIKOL_PLATFORM_WINDOWS == 1

/// Windows debug break
#include <intrin.h>
#define DEBUG_BREAK() __debugbreak()
/// Windows debug break

/// Linux debug break
#elif NIKOL_PLATFORM_LINUX == 1
#define DEBUG_BREAK() __builtin_trap()
/// Linux debug break

#endif

///---------------------------------------------------------------------------------------------------------------------
/// Assert macro 
#define NIKOL_ASSERT(expr, msg)                                         \
        {                                                               \
          if(expr) {                                                    \
          }                                                             \
          else {                                                        \
            nikol::logger_log_assert(#expr, msg, __FILE__, __LINE__);   \
            DEBUG_BREAK();                                              \
          }                                                             \
        }
/// Assert macro 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Debug asserts
#if NIKOL_DEBUG_BUILD == 1 
#define NIKOL_ASSERT_DEBUG(expr, msg) {                               \
        if(expr) {                                                    \
        }                                                             \
        else {                                                        \
          nikol::logger_log_assert(#expr, msg, __FILE__, __LINE__);   \
          DEBUG_BREAK();                                              \
        }                                                             \
}                                                       
#endif
/// Debug asserts
///---------------------------------------------------------------------------------------------------------------------

#endif

/// *** Asserts ***
/// ---------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// *** Math ***

/// ---------------------------------------------------------------------
/// Vec2
typedef glm::vec2 Vec2;
/// Vec2
/// ---------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// Vec3
typedef glm::vec3 Vec3;
/// Vec3
/// ---------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// Vec4
typedef glm::vec4 Vec4;
/// Vec4
/// ---------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// Mat3(3x3)
typedef glm::mat3 Mat3;
/// Mat3(3x3)
/// ---------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// Mat4(4x4)
typedef glm::mat4 Mat4;
/// Mat4(4x4)
/// ---------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// Math functions 

/// Return a random signed int.
const i32 random_i32();

/// Return a random signed int between `min` and `max`.
const i32 random_i32(const i32 min, const i32 max);

/// Return a random unsigned int.
const u32 random_u32();

/// Return a random unsigned int between `min` and `max`.
const u32 random_u32(const u32 min, const u32 max);

/// Return a random signed long.
const i64 random_i64();

/// Return a random signed long between `min` and `max`.
const i64 random_i64(const i64 min, const i64 max);

/// Return a random unsigned long.
const u64 random_u64();

/// Return a random unsigned long between `min` and `max`.
const u64 random_u64(const u64 min, const u64 max);

/// Return a random float.
const f32 random_f32();

/// Return a random float between `min` and `max`.
const f32 random_f32(const f32 min, const f32 max);

/// Return a random double.
const f64 random_f64();

/// Return a random double between `min` and `max`.
const f64 random_f64(const f64 min, const f64 max);

/// Return a random Vec2.
const Vec2 random_vec2();

/// Return a random Vec2 between `min` and `max`.
const Vec2 random_vec2(const Vec2 min, const Vec2 max);

/// Return a random Vec3.
const Vec3 random_vec3();

/// Return a random Vec3 between `min` and `max`.
const Vec3 random_vec3(const Vec3 min, const Vec3 max);

/// Return a random Vec4.
const Vec4 random_vec4();

/// Return a random Vec4 between `min` and `max`.
const Vec4 random_vec4(const Vec4 min, const Vec4 max);

/// Math functions 
/// ---------------------------------------------------------------------

/// *** Math ***
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
void event_init();

/// Shutdown the event system and reclaim some memory 
void event_shutdown();

/// Attach the given `func` callback to an event of type `type`, passing in the `listener` as well.
void event_listen(const EventType type, const EventFireFn& func, const void* listener = nullptr);

/// Call all callbacks associated with `event.type` and pass in the given `event` and the `dispatcher`. 
/// Returns `true` on success.
const bool event_dispatch(const Event& event, const void* dispatcher = nullptr);

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
void input_init();

/// Copying around input buffers
void input_update(); 

/// Returns `true` if `key` was pressed this frame
const bool input_key_pressed(const Key key);

/// Returns `true` if `key` was released this frame
const bool input_key_released(const Key key);

/// Returns `true` if `key` is currently held down
const bool input_key_down(const Key key);

/// Returns `true` if `key` is currently held up
const bool input_key_up(const Key key);

/// Returns `true` if the mouse `button` was pressed this frame
const bool input_button_pressed(const MouseButton button);

/// Returns `true` if the mouse `button` was released this frame
const bool input_button_released(const MouseButton button);

/// Returns `true` if the mouse `button` is currently held up
const bool input_button_down(const MouseButton button);

/// Returns `true` if the mouse `button` is currently held up
const bool input_button_up(const MouseButton button);

/// The current position of the mouse relative to the screen
void input_mouse_position(f32* x, f32* y);

/// The amount the mouse moved by since the last frame
void input_mouse_offset(f32* x, f32* y);

/// Get the mouse's scroll wheel value
const f32 input_mouse_scroll_value();

/// Enable/disable the mouse cursor
void input_cursor_show(const bool show);

/// Returns _true_ if the cursor has entered the window surface and _false_ 
/// if the cursor left the window surface. 
const bool input_cursor_on_screen();

/// Returns the _current_ connection status of the given joystick `id`
const bool input_gamepad_connected(const JoystickID id);

/// Returns a number between -1.0f and 1.0f for the joystick's axes or bumbers.
///
/// NOTE: This functions returns will fill in the given `x` and `y` floats. The X and Y  
/// corresponds with the axis's directions. However, for the triggers, the X and Y 
/// correspond to the left (X) and right (Y) triggers.
void input_gamepad_axis_value(const JoystickID id, const GamepadAxis axis, f32* x, f32* y);

/// Returns `true` if `button` of the gamepad `id` was pressed this frame  
const bool input_gamepad_button_pressed(const JoystickID id, const GamepadButton button);

/// Returns `true` if `button` of the gamepad `id` was released this frame  
const bool input_gamepad_button_released(const JoystickID id, const GamepadButton button);

/// Returns `true` if `button` of the gamepad `id` is being pressed
const bool input_gamepad_button_down(const JoystickID id, const GamepadButton button);

/// Returns `true` if `button` of the jgamepad `id` is not pressed currently
const bool input_gamepad_button_up(const JoystickID id, const GamepadButton button);

/// Get the name of the given gamepad `id` as a string if available
const i8* input_gamepad_get_name(const JoystickID id);

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
Window* window_open(const i8* title, const i32 width, const i32 height, i32 flags);

/// Closes the `window` context and clears up any memory.
void window_close(Window* window);

/// Poll events from the `window` context.
void window_poll_events(Window* window);

/// Swap the internal buffer of the `window` context. 
/// This might have no effect on some platforms.
void window_swap_buffers(Window* window);

/// Returns `true` if the `window` context is still actively open. 
const bool window_is_open(Window* window);

/// Returns `true` if the `window` context is currently in fullscreen mode
const bool window_is_fullscreen(Window* window);

/// Returns `true` if the `window` context is currently focused
const bool window_is_focused(Window* window);

/// Returns `true` if the `window` context is currently shown
const bool window_is_shown(Window* window);

/// Return the platform-specific window handle.
/// For example, this function will return `HWND` in a Windows enviornment.
///
/// NOTE: This function will return the native window handle as a pointer. 
/// Defrefrence the returned pointer if it is to be used.
void* window_get_native_handle(Window* window);

/// Retrieve the current size of the `window` context
void window_get_size(Window* window, i32* width, i32* height);

/// Retrieve the current title of the `window` context
const i8* window_get_title(Window* window);

/// Retrieve the current size of the monitor
void window_get_monitor_size(Window* window, i32* width, i32* height);

/// Retrieve the aspect ratio of the `window` context
const f32 window_get_aspect_ratio(Window* window);

/// Retrieve the refresh rate of the monitor 
const f32 window_get_refresh_rate(Window* window);

/// Retrieve the set window flags of `window`
const WindowFlags window_get_flags(Window* window);

/// Retrieve the current position of the `window` context relative to the monitor
void window_get_position(Window* window, i32* x, i32* y);

/// Set the given `window` as the current active context
void window_set_current_context(Window* window);

/// Either disable or enable fullscreen mode on the `window` context.
void window_set_fullscreen(Window* window, const bool fullscreen);

/// Either show or hide the `window` context.
void window_set_show(Window* window, const bool show);

/// Set the size of the `window` to `width` and `height`.
void window_set_size(Window* window, const i32 width, const i32 height);

/// Set the title of the `window` to `title`.
void window_set_title(Window* window, const i8* title);

/// Set the position of the `window` to `x_pos` and `y_pos`..
void window_set_position(Window* window, const i32 x_pos, const i32 y_pos);

/// Window functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Window ***
/// ---------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// *** Clock ***

///---------------------------------------------------------------------------------------------------------------------
/// Clock functions

/// NOTE: Every function has an "ni" prefix to avoid any confusion with the C clock library.

/// Updates the values of the time, FPS (frames per second), and the delta time.
/// NOTE: This must be called every frame.
void niclock_update();

/// Retrieve the time passed since the CPU was turned on.
const f64 niclock_get_time(); 

/// Retrieve the current FPS (frames per second) of the application
const f64 niclock_get_fps();

/// Retrieve the time passed between each frame. 
const f64 niclock_get_delta_time();

/// Clock functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Clock ***
/// ---------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// *** Graphics ***

///---------------------------------------------------------------------------------------------------------------------
// Consts

/// The max amount of textures the GPU supports at a time. 
const sizei TEXTURES_MAX = 32;

/// The max amount of uniform buffers to be created in a shader type.
const sizei UNIFORM_BUFFERS_MAX = 16;

/// The max number of elments a buffer's layout can have.
const sizei LAYOUT_ELEMENTS_MAX = 32;

// Consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxContextFlags
enum GfxContextFlags {
  /// Enable the depth testing pass.
  GFX_FLAGS_DEPTH        = 2 << 0, 
  
  /// Enable the stencil testing pass. 
  GFX_FLAGS_STENCIL      = 2 << 1, 
  
  /// Enable blending.
  GFX_FLAGS_BLEND        = 2 << 2, 
  
  /// Enable multisampling. 
  GFX_FLAGS_MSAA         = 2 << 3, 
  
  /// Clockwise culling order. 
  GFX_FLAGS_CULL_CW      = 2 << 4,

  /// Counter-Clockwise culling order. 
  GFX_FLAGS_CULL_CCW     = 2 << 5, 

  /// Enable vertical synchronization (VSYNC).
  GFX_FLAGS_ENABLE_VSYNC = 2 << 6, 
};
/// GfxContextFlags
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBufferType
enum GfxBufferType {
  GFX_BUFFER_VERTEX  = 3 << 0, 
  GFX_BUFFER_INDEX   = 3 << 1, 
  GFX_BUFFER_UNIFORM = 3 << 2,
};
/// GfxBufferType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBufferUsage
enum GfxBufferUsage {
  GFX_BUFFER_USAGE_DYNAMIC_DRAW = 4 << 0,
  GFX_BUFFER_USAGE_DYNAMIC_READ = 4 << 1,
  
  GFX_BUFFER_USAGE_STATIC_DRAW  = 4 << 2,
  GFX_BUFFER_USAGE_STATIC_READ  = 4 << 3,
};
/// GfxBufferUsage
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxDrawMode
enum GfxDrawMode {
  GFX_DRAW_MODE_POINT          = 5 << 0,
  GFX_DRAW_MODE_TRIANGLE       = 5 << 1,
  GFX_DRAW_MODE_TRIANGLE_STRIP = 5 << 2,
  GFX_DRAW_MODE_TRIANGLE_FAN   = 5 << 3,
  GFX_DRAW_MODE_LINE           = 5 << 4,
  GFX_DRAW_MODE_LINE_STRIP     = 5 << 5,
};
/// GfxDrawMode
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxLayoutType
enum GfxLayoutType {
  GFX_LAYOUT_FLOAT1 = 6 << 0,
  GFX_LAYOUT_FLOAT2 = 6 << 1,
  GFX_LAYOUT_FLOAT3 = 6 << 2,
  GFX_LAYOUT_FLOAT4 = 6 << 3,
  
  GFX_LAYOUT_INT1 = 6 << 4,
  GFX_LAYOUT_INT2 = 6 << 5,
  GFX_LAYOUT_INT3 = 6 << 6,
  GFX_LAYOUT_INT4 = 6 << 7,
  
  GFX_LAYOUT_UINT1 = 6 << 8,
  GFX_LAYOUT_UINT2 = 6 << 9,
  GFX_LAYOUT_UINT3 = 6 << 10,
  GFX_LAYOUT_UINT4 = 6 << 11,

  GFX_LAYOUT_MAT2  = 6 << 12,
  GFX_LAYOUT_MAT3  = 6 << 13,
  GFX_LAYOUT_MAT4  = 6 << 14,
};
/// GfxLayoutType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTextureFormat
enum GfxTextureFormat {
  GFX_TEXTURE_FORMAT_R8      = 8 << 0,
  GFX_TEXTURE_FORMAT_R16     = 8 << 1,

  GFX_TEXTURE_FORMAT_RG8     = 8 << 2,
  GFX_TEXTURE_FORMAT_RG16    = 8 << 3,
  
  GFX_TEXTURE_FORMAT_RGBA8   = 8 << 4,
  GFX_TEXTURE_FORMAT_RGBA16  = 8 << 5,
};
/// GfxTextureFromat
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTextureFilter
enum GfxTextureFilter {
  /// Uses linear filtering on both modes.
  GFX_TEXTURE_FILTER_MIN_MAG_LINEAR            = 9 << 0,

  /// Uses nearest filtering on both modes.
  GFX_TEXTURE_FILTER_MIN_MAG_NEAREST           = 9 << 1,
 
  /// Uses linear filtering on minification and nearest filtering magnification. 
  GFX_TEXTURE_FILTER_MIN_LINEAR_MAG_NEAREST    = 9 << 2,
  
  /// Uses nearest filtering on minification and linear filtering magnification. 
  GFX_TEXTURE_FILTER_MIN_NEAREST_MAG_LINEAR    = 9 << 3,

  /// Uses trilinear filtering (the weighted average of the two closes mipmaps)
  /// on minification and linear filtering on magnification.
  GFX_TEXTURE_FILTER_MIN_TRILINEAR_MAG_LINEAR  = 9 << 4,
  
  /// Uses trilinear filtering (the weighted average of the two closes mipmaps)
  /// on minification and nearest filtering on magnification.
  GFX_TEXTURE_FILTER_MIN_TRILINEAR_MAG_NEAREST = 9 << 5,
};
/// GfxTextureFilter
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTextureWrap
enum GfxTextureWrap {
  GFX_TEXTURE_WRAP_REPEAT       = 10 << 0, 
  GFX_TEXTURE_WRAP_MIRROR       = 10 << 1, 
  GFX_TEXTURE_WRAP_CLAMP        = 10 << 2, 
  GFX_TEXTURE_WRAP_BORDER_COLOR = 10 << 3,
};
/// GfxTextureWrap
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxShaderType
enum GfxShaderType {
  GFX_SHADER_VERTEX   = 11 << 0, 
  GFX_SHADER_PIXEL    = 11 << 1, 
  GFX_SHADER_GEOMETRY = 11 << 2,
};
/// GfxShaderType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxContext
struct GfxContext; 
/// GfxContext
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
/// GfxPipeline
struct GfxPipeline;
/// GfxPipeline
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBufferDesc
struct GfxBufferDesc {
  void* data = nullptr; 
  sizei size;

  GfxBufferType type;  
  GfxBufferUsage usage;
};
/// GfxBufferDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxLayoutDesc
struct GfxLayoutDesc {
  const i8* name; 
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
  u32 width, height; 
  u32 depth; 

  GfxTextureFormat format;
  GfxTextureFilter filter;
  GfxTextureWrap wrap_mode;

  void* data;
};
/// GfxTextureDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxPipelineDesc
struct GfxPipelineDesc {
  GfxBuffer* vertex_buffer = nullptr; 
  sizei vertices_count     = 0;

  GfxBuffer* index_buffer  = nullptr;
  sizei indices_count      = 0;

  GfxShader* shader        = nullptr;

  GfxLayoutDesc layout[LAYOUT_ELEMENTS_MAX];
  sizei layout_count       = 0; 

  GfxDrawMode draw_mode;

  GfxTexture* textures[TEXTURES_MAX];
  sizei texture_count      = 0;
};
/// GfxPipelineDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Context functions 

/// Initialize the `GfxContext` struct and return it. 
/// This function will handle the initialization of the underlying graphics APIs.
/// A refrence to the window will need to be passed to retrieve the size of the current 
/// window, to know which graphics API to use, and set the newly created context as the window 
/// context for rendering.
///
/// `flags`:
/// `GFX_FLAGS_DEPTH`        = Enable the depth testing pass. This is enabled by default.
/// `GFX_FLAGS_STENCIL`      = Enable the stencil testing pass. This is enabled by default.
/// `GFX_FLAGS_BLEND`        = Enable blending. This is disabled by default.
/// `GFX_FLAGS_MSAA`         = Enable multisampling. This is disabled by default.
/// `GFX_FLAGS_CULL_CW`      = Clockwise culling order. This is selected by default. 
/// `GFX_FLAGS_CULL_CCW`     = Counter-Clockwise culling order. 
/// `GFX_FLAGS_ENABLE_VSYNC` = Enable vertical synchronization (VSYNC).
/// 
/// @NOTE: All flags by default are turned off. None are enabled initially. 
/// 
/// @NOTE: Later on, with any function, if an instance of `GfxContext` 
/// is passed as a `nullptr`, the function will assert. 
GfxContext* gfx_context_init(Window* window, const i32 flags);

/// Free/reclaim any memory the graphics context has consumed. 
/// This function will do any required de-initialization by the graphics API.
void gfx_context_shutdown(GfxContext* gfx);

/// Clear the buffers and set the clear color as `{r, g, b, a}`.
void gfx_context_clear(GfxContext* gfx, const f32 r, const f32 g, const f32 b, const f32 a);

/// Switch to the back buffer or, rather, present the back buffer. 
/// 
/// @NOTE: This function will be effected by vsync. 
void gfx_context_present(GfxContext* gfx);

/// Set any `flag` of the context `gfx` to `value`. 
void gfx_context_set_flag(GfxContext* gfx, const i32 flag, const bool value);

/// Retrive the set flags of the `gfx` context.
const GfxContextFlags gfx_context_get_flags(GfxContext* gfx);

/// Context functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Buffer functions 

/// Allocate and return a `GfxBuffer` object, using the information in `desc`.
GfxBuffer* gfx_buffer_create(GfxContext* gfx, const GfxBufferDesc& desc);

/// Free/reclaim any memory taken by `buff`.
void gfx_buffer_destroy(GfxBuffer* buff);

/// Update the contents of `buff` starting at `offset` with `data` of size `size`.
void gfx_buffer_update(GfxContext* gfx, GfxBuffer* buff, const sizei offset, const sizei size, const void* data);

/// Buffer functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Shader functions 

/// Allocate and return a `GfxShader`, passing the given `src`. 
///
/// @NOTE: For glsl (OpenGL), both the vertex and the fragment shader should be combined into one. 
/// The function will look for the `#version` declarative in order to seperate the two or more shaders. The first 
/// one will be the vertex shader, the second will be the geometry shader, and the third will be 
/// the fragment shader. 
///
/// Check the examples for more information.
GfxShader* gfx_shader_create(GfxContext* gfx, const i8* src);

/// Free/reclaim any memory the graphics context has consumed. 
void gfx_shader_destroy(GfxShader* shader);

/// Attaches the uniform `buffer` to the `shader` of type `type`. 
/// Later, when the pipeline initiates the draw phase, the contents of `buffer` will be updated and sent to the shader.
/// This function only queues the uniform buffer.
///
/// @NOTE: For GLSL (OpenGL), you _need_ to specify the binding point of the uniform buffer in the shader itself. For example, 
/// do something like, `layout (std140, binding = 0)`. Now that uniform buffer will be bound to the point `0` and the shader 
/// can easily find it. Also, make sure to have the binding points increase like an index, since that's how this function 
/// will look for them. 
void gfx_shader_attach_uniform(GfxContext* gfx, GfxShader* shader, const GfxShaderType type, GfxBuffer* buffer);

/// Shader functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Texture functions 

/// Allocate and return a `GfxTexture` from the information provided by `desc`. 
GfxTexture* gfx_texture_create(GfxContext* gfx, const GfxTextureDesc& desc);

/// Reclaim/free any memory allocated by `texture`.
void gfx_texture_destroy(GfxTexture* texture);

/// Update the `texture`'s information from the given `desc`.
void gfx_texture_update(GfxContext* gfx, GfxTexture* texture, const GfxTextureDesc& desc);

/// Texture functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Pipeline functions 

/// Allocate and return a `GfxPipeline` from the information provided by `desc`.
GfxPipeline* gfx_pipeline_create(GfxContext* gfx, const GfxPipelineDesc& desc);

/// Reclaim/free any memory allocated by `pipeline`.
void gfx_pipeline_destroy(GfxPipeline* pipeline);

/// Draw the contents of the `vertex_buffer` in `pipeline`, using the information provided by `desc`.
void gfx_pipeline_draw_vertex(GfxContext* gfx, GfxPipeline* pipeline, const GfxPipelineDesc& desc);

/// Draw the contents of the `vertex_buffer` using the `index_buffer` 
/// in `pipeline`, using the information provided by `desc`.
void gfx_pipeline_draw_index(GfxContext* gfx, GfxPipeline* pipeline, const GfxPipelineDesc& desc);

/// Pipeline functions 
///---------------------------------------------------------------------------------------------------------------------

/// *** Graphics ***
/// ---------------------------------------------------------------------

} // End of nikol

//////////////////////////////////////////////////////////////////////////
