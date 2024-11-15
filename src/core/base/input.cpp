#include "nikol_core.h"

//////////////////////////////////////////////////////////////////////////

namespace nikol { // Start of nikol

/// InputState
/// InputState

/// ---------------------------------------------------------------------
/// Input functions

void input_init();

void input_update(); 

const bool input_key_pressed(const Key key);

const bool input_key_released(const Key key);

const bool input_key_down(const Key key);

const bool input_key_up(const Key key);

const bool input_button_pressed(const MouseButton button);

const bool input_button_released(const MouseButton button);

const bool input_button_down(const MouseButton button);

const bool input_button_up(const MouseButton button);

const Vec2 input_mouse_position();

void input_mouse_set_position(const Vec2 position);

const Vec2 input_mouse_offset();

const f32 input_mouse_scroll_value();

void input_cursor_show(const bool show);

const bool input_cursor_on_screen();

const bool input_joystick_connected(const JoystickID id);

const Vec2 input_joystick_axis_value(const JoystickID id, const JoystickAxis axis);

const bool input_joystick_button_pressed(const JoystickID id, const JoystickButton button);

const bool input_joystick_button_released(const JoystickID id, const JoystickButton button);

const bool input_joystick_button_down(const JoystickID id, const JoystickButton button);

const bool input_joystick_button_up(const JoystickID id, const JoystickButton button);

const String input_joystick_get_name(const JoystickID id);

/// Input functions
/// ---------------------------------------------------------------------

} // End of nikol

//////////////////////////////////////////////////////////////////////////
