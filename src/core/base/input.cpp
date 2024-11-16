#include "nikol_core.h"

#include <GLFW/glfw3.h>

//////////////////////////////////////////////////////////////////////////

namespace nikol { // Start of nikol

/// InputState
struct InputState {
  // Keyboard state
  bool current_key_state[KEYS_MAX];
  bool previous_key_state[KEYS_MAX];

  // Mouse state
  bool current_mouse_state[MOUSE_BUTTONS_MAX];
  bool previous_mouse_state[MOUSE_BUTTONS_MAX];

  Vec2 mouse_position, mouse_offset; 
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
      s_state.mouse_position = event.mouse_pos; 
      s_state.mouse_offset = event.mouse_offset;
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

  NIKOL_LOG_INFO("Input system successfully initialized");
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

const Vec2 input_mouse_position() {
  return s_state.mouse_position;
}

const Vec2 input_mouse_offset() {
  return s_state.mouse_offset;
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

const Vec2 input_gamepad_axis_value(const JoystickID id, const GamepadAxis axis) {
  GLFWgamepadstate gamepad_state; 
  glfwGetGamepadState(id, &gamepad_state);
  
  f32 x = gamepad_state.axes[axis];
  f32 y = gamepad_state.axes[axis + 1];

  return Vec2(x, y);
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

const String input_gamepad_get_name(const JoystickID id) {
  return glfwGetGamepadName(id);
}

/// Input functions
/// ---------------------------------------------------------------------

} // End of nikol

//////////////////////////////////////////////////////////////////////////
