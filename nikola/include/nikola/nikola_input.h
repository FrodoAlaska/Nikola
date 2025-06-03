#pragma once

#include "nikola_base.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola
 
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
  /// X and Y of the left axis
  GAMEPAD_AXIS_LEFT    = 0, 

  /// X and Y of the right axis
  GAMEPAD_AXIS_RIGHT   = 2,

  /// Left and Right trigger
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

/// Initialze the input system.
NIKOLA_API void input_init();

/// Update the internal state of the input system.
NIKOLA_API void input_update(); 

/// Returns `true` if `key` was pressed this frame.
NIKOLA_API const bool input_key_pressed(const Key key);

/// Returns `true` if `key` was released this frame.
NIKOLA_API const bool input_key_released(const Key key);

/// Returns `true` if `key` is currently held down.
NIKOLA_API const bool input_key_down(const Key key);

/// Returns `true` if `key` is currently held up.
NIKOLA_API const bool input_key_up(const Key key);

/// Returns `true` if the mouse `button` was pressed this frame.
NIKOLA_API const bool input_button_pressed(const MouseButton button);

/// Returns `true` if the mouse `button` was released this frame.
NIKOLA_API const bool input_button_released(const MouseButton button);

/// Returns `true` if the mouse `button` is currently held down.
NIKOLA_API const bool input_button_down(const MouseButton button);

/// Returns `true` if the mouse `button` is currently held up.
NIKOLA_API const bool input_button_up(const MouseButton button);

/// The current position of the mouse relative to the screen.
NIKOLA_API void input_mouse_position(f32* x, f32* y);

/// The amount the mouse moved by since the last frame.
NIKOLA_API void input_mouse_offset(f32* x, f32* y);

/// Get the mouse's scroll wheel value.
NIKOLA_API const f32 input_mouse_scroll_value();

/// Enable/disable the mouse cursor.
NIKOLA_API void input_cursor_show(const bool show);

/// Returns `true` if the cursor has entered the window surface and `false` 
/// if the cursor left the window surface. 
NIKOLA_API const bool input_cursor_on_screen();

/// Returns the _current_ connection status of the given joystick `id`.
NIKOLA_API const bool input_gamepad_connected(const JoystickID id);

/// Returns a number between `-1.0f` and `1.0f` for the joystick's axes or bumbers.
///
/// @NOTE: This functions will fill in the given `x` and `y` floats. The X and Y  
/// corresponds with the axis's directions if the given `axis` is either `GAMEPAD_AXIS_LEFT` or `GAMEPAD_AXIS_RIGHT`. 
/// However, for `GAMEPAD_AXIS_TRIGGER`, the X and Y correspond to the left (X) and right (Y) triggers/bumpers.
NIKOLA_API void input_gamepad_axis_value(const JoystickID id, const GamepadAxis axis, f32* x, f32* y);

/// Returns `true` if `button` of the gamepad `id` was pressed this frame.  
NIKOLA_API const bool input_gamepad_button_pressed(const JoystickID id, const GamepadButton button);

/// Returns `true` if `button` of the gamepad `id` was released this frame.  
NIKOLA_API const bool input_gamepad_button_released(const JoystickID id, const GamepadButton button);

/// Returns `true` if `button` of the gamepad `id` is being pressed.
NIKOLA_API const bool input_gamepad_button_down(const JoystickID id, const GamepadButton button);

/// Returns `true` if `button` of the jgamepad `id` is not pressed currently.
NIKOLA_API const bool input_gamepad_button_up(const JoystickID id, const GamepadButton button);

/// Get the name of the given gamepad `id` as a string if available.
NIKOLA_API const char* input_gamepad_get_name(const JoystickID id);

/// Input functions 
///---------------------------------------------------------------------------------------------------------------------

/// *** Input ***
/// ---------------------------------------------------------------------


} // End of nikola

//////////////////////////////////////////////////////////////////////////
