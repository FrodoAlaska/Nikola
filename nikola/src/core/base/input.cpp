#include "nikola/nikola_base.h"

#include <GLFW/glfw3.h>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// InputState
struct InputState {
  // Keyboard state
  bool current_key_state[KEYS_MAX];
  bool previous_key_state[KEYS_MAX];

  // Mouse state
  bool current_mouse_state[MOUSE_BUTTONS_MAX];
  bool previous_mouse_state[MOUSE_BUTTONS_MAX];

  f32 mouse_position_x, mouse_position_y;
  f32 mouse_offset_x, mouse_offset_y; 
  f32 scroll_value;

  bool cursor_entered;

  // Joystick state
  bool connected_joysticks[JOYSTICK_ID_LAST + 1];
  bool current_gamepad_state[JOYSTICK_ID_LAST + 1][GAMEPAD_BUTTONS_MAX];
  bool previous_gamepad_state[JOYSTICK_ID_LAST + 1][GAMEPAD_BUTTONS_MAX];
};

static InputState s_state;
/// InputState

/// Callbacks
static bool key_callback(const Event& event, const void* dispatcher, const void* listener) {
  switch(event.type) {
    case EVENT_KEY_PRESSED: 
      s_state.current_key_state[event.key_pressed] = true;
      return true;
    case EVENT_KEY_RELEASED: 
      s_state.current_key_state[event.key_released] = false;
      return true;
    default:
      return false;
  }
}

static bool mouse_callback(const Event& event, const void* dispatcher, const void* listener) {
  switch(event.type) {
    case EVENT_MOUSE_MOVED:
      s_state.mouse_position_x = event.mouse_pos_x;
      s_state.mouse_position_y = event.mouse_pos_y;

      s_state.mouse_offset_x = event.mouse_offset_x;
      s_state.mouse_offset_y = event.mouse_offset_y;
    return true;
    case EVENT_MOUSE_ENTER: 
      s_state.cursor_entered = true;
      return true;
    case EVENT_MOUSE_LEAVE: 
      s_state.cursor_entered = false;
      return true;
    case EVENT_MOUSE_BUTTON_PRESSED:
      s_state.current_mouse_state[event.mouse_button_pressed] = true;
      return true;
    case EVENT_MOUSE_BUTTON_RELEASED:
      s_state.current_mouse_state[event.mouse_button_released] = false;
      return true;
    case EVENT_MOUSE_SCROLL_WHEEL:
      s_state.scroll_value = event.mouse_scroll_value;
      return true;
    default:
      return false;
  }
}

static bool joystick_callback(const Event& event, const void* dispatcher, const void* listener) {
  switch(event.type) {
    case EVENT_JOYSTICK_CONNECTED: 
      s_state.connected_joysticks[event.joystick_id] = true;
      return true;
    case EVENT_JOYSTICK_DISCONNECTED: 
      s_state.connected_joysticks[event.joystick_id] = false;
      return true;
    default:
      return false;
  }
}
/// Callbacks

/// ---------------------------------------------------------------------
/// Input functions

void input_init() {
  // Listening to key events 
  event_listen(EVENT_KEY_PRESSED, key_callback);
  event_listen(EVENT_KEY_RELEASED, key_callback);

  // Listening to mouse events 
  event_listen(EVENT_MOUSE_MOVED, mouse_callback); 
  event_listen(EVENT_MOUSE_ENTER, mouse_callback); 
  event_listen(EVENT_MOUSE_LEAVE, mouse_callback); 
  event_listen(EVENT_MOUSE_BUTTON_PRESSED, mouse_callback); 
  event_listen(EVENT_MOUSE_BUTTON_RELEASED, mouse_callback); 
  event_listen(EVENT_MOUSE_SCROLL_WHEEL, mouse_callback); 
  
  // Listening to joystick events
  event_listen(EVENT_JOYSTICK_CONNECTED, joystick_callback); 
  event_listen(EVENT_JOYSTICK_DISCONNECTED, joystick_callback); 

  NIKOLA_LOG_INFO("Input system successfully initialized");
}

void input_update() {
  // Updating the input states 
  memory_copy(s_state.previous_key_state, s_state.current_key_state, sizeof(s_state.current_key_state)); 
  memory_copy(s_state.previous_mouse_state, s_state.current_mouse_state, sizeof(s_state.current_mouse_state)); 
  memory_copy(s_state.previous_gamepad_state, s_state.current_gamepad_state, sizeof(s_state.current_gamepad_state)); 
  
  // Checking for joystick input every frame to set the current state
  for(i32 i = 0; i < JOYSTICK_ID_LAST; i++) {
    // We don't care about non-gamepad joysticks
    if(!glfwJoystickIsGamepad(i)) {

    }

    // Check for every button press/release
    for(i32 j = 0; j < GAMEPAD_BUTTONS_MAX; j++) {
      GLFWgamepadstate gamepad_state;
      glfwGetGamepadState(i, &gamepad_state);
      
      s_state.current_gamepad_state[i][j] = gamepad_state.buttons[j] == GLFW_PRESS; 
    }
  }
} 

const bool input_key_pressed(const Key key) {
  return !s_state.previous_key_state[key] && s_state.current_key_state[key];
}

const bool input_key_released(const Key key) {
  return s_state.previous_key_state[key] && !s_state.current_key_state[key];
}

const bool input_key_down(const Key key) {
  return s_state.current_key_state[key];
}

const bool input_key_up(const Key key) {
  return !s_state.current_key_state[key];
}

const bool input_button_pressed(const MouseButton button) {
  return !s_state.previous_mouse_state[button] && s_state.current_mouse_state[button];
}

const bool input_button_released(const MouseButton button) {
  return s_state.previous_mouse_state[button] && !s_state.current_mouse_state[button];
}

const bool input_button_down(const MouseButton button) {
  return s_state.current_mouse_state[button];
}

const bool input_button_up(const MouseButton button) {
  return !s_state.current_mouse_state[button];
}

void input_mouse_position(f32* x, f32* y) {
  *x = s_state.mouse_position_x;
  *y = s_state.mouse_position_y;
}

void input_mouse_offset(f32* x, f32* y) {
  *x = s_state.mouse_offset_x;
  *y = s_state.mouse_offset_y;
}

const f32 input_mouse_scroll_value() {
  return s_state.scroll_value;
}

void input_cursor_show(const bool show) {
  event_dispatch(Event {
    .type         = EVENT_MOUSE_CURSOR_SHOWN, 
    .cursor_shown = show,
  });
}

const bool input_cursor_on_screen() {
  return s_state.cursor_entered;
}

const bool input_gamepad_connected(const JoystickID id) {
  return s_state.connected_joysticks[id];
}

void input_gamepad_axis_value(const JoystickID id, const GamepadAxis axis, f32* x, f32* y) {
  GLFWgamepadstate gamepad_state; 
  glfwGetGamepadState(id, &gamepad_state);
  
  *x = gamepad_state.axes[axis];
  *y = gamepad_state.axes[axis + 1];
}

const bool input_gamepad_button_pressed(const JoystickID id, const GamepadButton button) {
  return !s_state.previous_gamepad_state[id][button] && s_state.current_gamepad_state[id][button];
}

const bool input_gamepad_button_released(const JoystickID id, const GamepadButton button) {
  return s_state.previous_gamepad_state[id][button] && !s_state.current_gamepad_state[id][button];
}

const bool input_gamepad_button_down(const JoystickID id, const GamepadButton button) {
  return s_state.current_gamepad_state[id][button];
}

const bool input_gamepad_button_up(const JoystickID id, const GamepadButton button) {
  return !s_state.current_gamepad_state[id][button];
}

const i8* input_gamepad_get_name(const JoystickID id) {
  return glfwGetGamepadName(id);
}

/// Input functions
/// ---------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
