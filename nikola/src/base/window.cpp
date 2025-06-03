#include "nikola/nikola_window.h"
#include "nikola/nikola_event.h"
#include "nikola/nikola_input.h"

#include <GLFW/glfw3.h>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Window
struct Window {
  GLFWwindow* handle  = nullptr;
  GLFWcursor* cursor  = nullptr;

  i32 width, height; 
  
  WindowFlags flags;

  f32 refresh_rate = 0.0f;

  bool is_fullscreen   = false;
  bool is_focused      = false;
  bool is_cursor_shown = true;

  i32 position_x = 0; 
  i32 position_y = 0;

  f64 mouse_position_x = 0.0f; 
  f64 mouse_position_y = 0.0f; 

  f32 last_mouse_position_x = 0.0f;
  f32 last_mouse_position_y = 0.0f;
  
  f32 mouse_offset_x = 0.0f; 
  f32 mouse_offset_y = 0.0f;
};
/// Window
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Callbacks
static void error_callback(int err_code, const char* desc) {
  NIKOLA_LOG_FATAL("%s", desc);
}

static void window_pos_callback(GLFWwindow* handle, int xpos, int ypos) {
  Window* window = (Window*)glfwGetWindowUserPointer(handle);

  window->position_x = xpos;
  window->position_y = ypos;
  
  event_dispatch(Event {
    .type = EVENT_WINDOW_MOVED, 
    .window_new_pos_x = window->position_x,
    .window_new_pos_y = window->position_y,
  });
}

static void window_maxmize_callback(GLFWwindow* window, int maximized) {
  EventType type; 

  if(maximized) {
    type = EVENT_WINDOW_MAXIMIZED;
  }
  else {
    type = EVENT_WINDOW_MINIMIZED;
  }

  event_dispatch(Event{.type = type});
}

static void window_focus_callback(GLFWwindow* handle, int focused) {
  Window* window = (Window*)glfwGetWindowUserPointer(handle);
  window->is_focused = focused;

  event_dispatch(Event {
    .type = EVENT_WINDOW_FOCUSED, 
    .window_has_focus = window->is_focused,
  });
}

static void window_framebuffer_resize_callback(GLFWwindow* handle, int width, int height) {
  Window* window = (Window*)glfwGetWindowUserPointer(handle);
  window->width  = width;
  window->height = height;
  
  event_dispatch(Event {
    .type = EVENT_WINDOW_FRAMEBUFFER_RESIZED, 
    .window_framebuffer_width  = width,
    .window_framebuffer_height = height,
  });
}

static void window_resize_callback(GLFWwindow* handle, int width, int height) {
  Window* window = (Window*)glfwGetWindowUserPointer(handle);
  window->width  = width;
  window->height = height;
  
  event_dispatch(Event {
    .type = EVENT_WINDOW_RESIZED, 
    .window_new_width  = width,
    .window_new_height = height,
  });
}

static void window_close_callback(GLFWwindow* window) {
  event_dispatch(Event{.type = EVENT_WINDOW_CLOSED});
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if(action == GLFW_PRESS) {
    event_dispatch(Event {
      .type = EVENT_KEY_PRESSED, 
      .key_pressed = key,
    });
  }
  else if(action == GLFW_RELEASE) {
    event_dispatch(Event {
      .type = EVENT_KEY_RELEASED, 
      .key_released = key,
    });
  }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  if(action == GLFW_PRESS) {
    event_dispatch(Event {
      .type = EVENT_MOUSE_BUTTON_PRESSED, 
      .mouse_button_pressed = button,
    });
  }
  else if(action == GLFW_RELEASE) {
    event_dispatch(Event {
      .type = EVENT_MOUSE_BUTTON_RELEASED, 
      .mouse_button_released = button,
    });
  }
}

void cursor_pos_callback(GLFWwindow* handle, double xpos, double ypos) {
  Window* window = (Window*)glfwGetWindowUserPointer(handle);

  window->mouse_position_x = xpos;
  window->mouse_position_y = ypos;
 
  f32 offset_x = xpos - window->last_mouse_position_x; 
  f32 offset_y = window->last_mouse_position_y - ypos; 
  
  window->last_mouse_position_x = xpos; 
  window->last_mouse_position_y = ypos; 

  window->mouse_offset_x += offset_x;
  window->mouse_offset_y += offset_y;

  event_dispatch(Event {
    .type = EVENT_MOUSE_MOVED, 
    .mouse_pos_x = (f32)window->mouse_position_x, 
    .mouse_pos_y = (f32)window->mouse_position_y, 
    
    .mouse_offset_x = window->mouse_offset_x,
    .mouse_offset_y = window->mouse_offset_y,
  });
}

void cursor_enter_callback(GLFWwindow* window, int entered) {
  event_dispatch(Event {
    .type = entered == GLFW_TRUE ? EVENT_MOUSE_ENTER : EVENT_MOUSE_LEAVE,
  });
}

void scroll_wheel_callback(GLFWwindow* window, double xoffset, double yoffset) {
  event_dispatch(Event {
    .type = EVENT_MOUSE_SCROLL_WHEEL, 
    .mouse_scroll_value = (f32)yoffset,
  });
}

void joystick_callback(int jid, int event) {
  EventType type = event == GLFW_CONNECTED ? EVENT_JOYSTICK_CONNECTED : EVENT_JOYSTICK_DISCONNECTED; 

  event_dispatch(Event {
    .type = type, 
    .joystick_id = jid,
  });
}

static bool nikola_cursor_show_callback(const Event& event, const void* dispatcher, const void* listener) {
  if(event.type != EVENT_MOUSE_CURSOR_SHOWN) {
    return false;
  }

  Window* window = (Window*)listener;

  i32 cursor_mode = event.cursor_shown ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
  glfwSetInputMode(window->handle, GLFW_CURSOR, cursor_mode);

  return true;
}

static bool nikola_quit_app_callback(const Event& event, const void* dispatcher, const void* listener) {
  if(event.type != EVENT_APP_QUIT) {
    return false;
  }
  
  Window* window = (Window*)listener;
  glfwSetWindowShouldClose(window->handle, GLFW_TRUE);

  return true;
}

/// Callbacks
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void set_window_hints(Window* window) {
  // Setting the this for the MSAA down the line
  // @TODO (Window): This should probably be configurable
  glfwWindowHint(GLFW_SAMPLES, 4); 
 
  // Setting the OpenGL context configurations
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Error callback
  glfwSetErrorCallback(error_callback); 

  // Setting some defaults
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  // Setting the flags...

  if(IS_BIT_SET(window->flags, WINDOW_FLAGS_RESIZABLE)) {
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  }
  
  if(IS_BIT_SET(window->flags, WINDOW_FLAGS_FOCUS_ON_CREATE)) {
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    window->is_focused = true;
  }
  
  if(IS_BIT_SET(window->flags, WINDOW_FLAGS_FOCUS_ON_SHOW)) {
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
    window->is_focused = true;
  }
  
  if(IS_BIT_SET(window->flags, WINDOW_FLAGS_MINIMIZE)) {
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
  }
  
  if(IS_BIT_SET(window->flags, WINDOW_FLAGS_MAXMIZE)) {
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
  }
  
  if(IS_BIT_SET(window->flags, WINDOW_FLAGS_DISABLE_DECORATIONS)) {
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
  }
  
  if(IS_BIT_SET(window->flags, WINDOW_FLAGS_CENTER_MOUSE)) {
    glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);
  }
  
  if(IS_BIT_SET(window->flags, WINDOW_FLAGS_HIDE_CURSOR)) {
    window->is_cursor_shown = false;
  }
  
  if(IS_BIT_SET(window->flags, WINDOW_FLAGS_FULLSCREEN)) {
    window->is_fullscreen = true; 
  }
}

static void create_glfw_handle(Window* window, const char* title) {
  // Creating the window
  window->handle = glfwCreateWindow(window->width, window->height, title, nullptr, nullptr);

  // Setting the new refresh rate
  window->refresh_rate = glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate;
}

static void set_window_callbacks(Window* window) {
  // Window callbacks
  glfwSetWindowPosCallback(window->handle, window_pos_callback);
  glfwSetWindowMaximizeCallback(window->handle, window_maxmize_callback);
  glfwSetWindowFocusCallback(window->handle, window_focus_callback);
  glfwSetFramebufferSizeCallback(window->handle, window_framebuffer_resize_callback);
  glfwSetWindowSizeCallback(window->handle, window_resize_callback);
  glfwSetWindowCloseCallback(window->handle, window_close_callback);

  // Key callbacks 
  glfwSetKeyCallback(window->handle, key_callback);
  
  // Mouse callbacks 
  glfwSetMouseButtonCallback(window->handle, mouse_button_callback);
  glfwSetCursorPosCallback(window->handle, cursor_pos_callback);
  glfwSetCursorEnterCallback(window->handle, cursor_enter_callback);
  glfwSetScrollCallback(window->handle, scroll_wheel_callback);

  // Joystick callbacks 
  glfwSetJoystickCallback(joystick_callback); 

  // Nikola cursor show callback
  event_listen(EVENT_MOUSE_CURSOR_SHOWN, nikola_cursor_show_callback, window);
  event_listen(EVENT_APP_QUIT, nikola_quit_app_callback, window);
}
/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Window functions

Window* window_open(const char* title, const i32 width, const i32 height, i32 flags) {
  Window* window = (Window*)memory_allocate(sizeof(Window));

  window->width  = width; 
  window->height = height; 
  window->flags  = (WindowFlags)flags;

  // GLFW init and setup 
  glfwInit();
  set_window_hints(window);
  create_glfw_handle(window, title);
  set_window_callbacks(window);
  
  // Something wrong...
  if(!window->handle) {
    memory_free(window);
    return nullptr;
  }

  // Setting our `window` as user data in the glfw window
  glfwSetWindowUserPointer(window->handle, window);
  
  // Querying data from the GLFW window
  glfwGetWindowPos(window->handle, &window->position_x, &window->position_y); 
  glfwGetCursorPos(window->handle, &window->mouse_position_x, &window->mouse_position_y);

  window->last_mouse_position_x = window->mouse_position_x;
  window->last_mouse_position_y = window->mouse_position_y;
  
  // Setting the correct initial mouse offset
  window->mouse_offset_x = window->last_mouse_position_x - window->mouse_position_x;
  window->mouse_offset_y = window->last_mouse_position_y - window->mouse_position_y;

  // Set the current context 
  glfwMakeContextCurrent(window->handle);

  if(window->is_fullscreen) {
    window_set_fullscreen(window, true);
  }

  if(!window->is_cursor_shown) {
    glfwSetInputMode(window->handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
  
  NIKOLA_LOG_INFO("Window: {t = \"%s\", w = %i, h = %i} was successfully opened", title, width, height);
  return window;
}

void window_close(Window* window) {
  if(window->cursor) {
    glfwDestroyCursor(window->cursor);
  }

  glfwDestroyWindow(window->handle);
  glfwTerminate();
  
  memory_free(window);

  NIKOLA_LOG_INFO("Window was successfully closed");
}

void window_poll_events(Window* window) {
  // Update the internal systems
  input_update();
  niclock_update();

  // Poll for events
  glfwPollEvents();
}

void window_swap_buffers(Window* window, const i32 interval) {
  glfwSwapInterval(interval);
  glfwSwapBuffers(window->handle);
}

const bool window_is_open(const Window* window) {
  return !glfwWindowShouldClose(window->handle);
}

const bool window_is_fullscreen(const Window* window) {
  return window->is_fullscreen;
}

const bool window_is_focused(const Window* window) {
  return window->is_focused;
}

const bool window_is_shown(const Window* window) {
  return glfwGetWindowAttrib(window->handle, GLFW_VISIBLE);
}

void window_get_size(const Window* window, i32* width, i32* height) {
  *width  = window->width;
  *height = window->height;
}

void* window_get_handle(const Window* window) {
  return window->handle;
}

const char* window_get_title(const Window* window) {
  return glfwGetWindowTitle(window->handle);
}

void window_get_monitor_size(const Window* window, i32* width, i32* height) {
  const GLFWvidmode* video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  
  *width  = video_mode->width;
  *height = video_mode->height;
}

const f32 window_get_aspect_ratio(const Window* window) {
  return ((f32)window->width / (f32)window->height);
}

const f32 window_get_refresh_rate(const Window* window) {
  return window->refresh_rate;
}

const WindowFlags window_get_flags(const Window* window) {
  return window->flags;
}

void window_get_position(const Window* window, i32* x, i32* y) {
  *x = window->position_x;
  *y = window->position_y;
}

void window_set_current_context(Window* window) {
  glfwMakeContextCurrent(window->handle);
}

void window_set_fullscreen(Window* window, const bool fullscreen) {
  window->is_fullscreen = fullscreen; 
  const GLFWvidmode* video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

  if(fullscreen) {
    glfwSetWindowMonitor(window->handle, 
                         glfwGetPrimaryMonitor(), 
                         0, 0, 
                         video_mode->width, video_mode->height, 
                         window->refresh_rate);
  }
  else {
    glfwSetWindowMonitor(window->handle, 
                         nullptr, 
                         window->position_x, window->position_y, 
                         window->width, window->height, 
                         window->refresh_rate);
  }
  
  // Firing an event for the internal systems
  event_dispatch(Event {
    .type                 = EVENT_WINDOW_FULLSCREEN, 
    .window_is_fullscreen = fullscreen,
  }, window);
}

void window_set_show(Window* window, const bool show) {
  if(show) {
    glfwShowWindow(window->handle);
  }
  else {
    glfwHideWindow(window->handle);
  }
}

void window_set_size(Window* window, const i32 width, const i32 height) {
  window->width = width; 
  window->height = height;

  glfwSetWindowSize(window->handle, width, height);
}

void window_set_title(Window* window, const char* title) {
  glfwSetWindowTitle(window->handle, title);
}

void window_set_position(Window* window, const i32 x_pos, const i32 y_pos) {
  window->position_x = x_pos; 
  window->position_y = y_pos; 
  
  glfwSetWindowPos(window->handle, window->position_x, window->position_y);
}

/// Window functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
