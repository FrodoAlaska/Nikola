#include "nikol_core.h"

#include <ishtar/ishtar.h>
#include <GLFW/glfw3.h>

//////////////////////////////////////////////////////////////////////////

namespace nikol { // Start of nikol

/// Window
struct Window {
  GLFWwindow* handle = nullptr;
  GLFWcursor* cursor = nullptr;

  i32 width, height; 
  ishtar::String title;
  WindowFlags flags;

  f32 refresh_rate;

  bool is_fullscreen, is_focused, is_cursor_shown;

  Vec2 position; 
  Vec2 last_mouse_position, mouse_position, mouse_offset;
};
/// Window

/// Callbacks
static void error_callback(int err_code, const char* desc) {
  NIKOL_LOG_FATAL("%s", desc);
}

static void window_pos_callback(GLFWwindow* handle, int xpos, int ypos) {
  Window* window = (Window*)glfwGetWindowUserPointer(handle);

  window->position = Vec2(xpos, ypos);
  
  event_dispatch(Event {
    .type = EVENT_WINDOW_MOVED, 
    .window_new_pos = window->position,
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

static void window_framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
  event_dispatch(Event {
    .type = EVENT_WINDOW_FRAMEBUFFER_RESIZED, 
    .window_framebuffer_size = Vec2(width, height),
  });
}

static void window_resize_callback(GLFWwindow* window, int width, int height) {
  event_dispatch(Event {
    .type = EVENT_WINDOW_RESIZED, 
    .window_new_pos = Vec2(width, height),
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

  window->mouse_position = Vec2(xpos, ypos);
  
  Vec2 offset(xpos - window->last_mouse_position.x, ypos - window->last_mouse_position.y); 
  window->last_mouse_position = Vec2(xpos, ypos); 

  // @TODO: Applying sensitivity. This must be configurable
  offset *= 0.1f;
  window->mouse_offset += offset;

  event_dispatch(Event {
    .type = EVENT_MOUSE_MOVED, 
    .mouse_pos = window->mouse_position, 
    .mouse_offset = window->mouse_offset,
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

static bool nikol_cursor_show_callback(const Event& event, const void* dispatcher, const void* listener) {
  if(event.type != EVENT_MOUSE_CURSOR_SHOWN) {
    return false;
  }

  Window* window = (Window*)listener;

  i32 cursor_mode = event.cursor_shown ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
  glfwSetInputMode(window->handle, GLFW_CURSOR, cursor_mode);

  return true;
}
/// Callbacks

/// Private functions
static void set_window_hints(Window* window) {
  // Setting the this for the MSAA down the line
  // @TEMP: This should probably be configurable
  glfwWindowHint(GLFW_SAMPLES, 4); 

  // Error callback
  glfwSetErrorCallback(error_callback); 

  // Not resizable by default
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  
  /// @TODO: There must be a better way than this shit
  /// Seriously, what the fuck???

  if((window->flags & WINDOW_FLAGS_RESIZABLE) == WINDOW_FLAGS_RESIZABLE) {
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  }
  
  if((window->flags & WINDOW_FLAGS_FOCUS_ON_CREATE) == WINDOW_FLAGS_FOCUS_ON_CREATE) {
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
  }
  
  if((window->flags & WINDOW_FLAGS_FOCUS_ON_SHOW) == WINDOW_FLAGS_FOCUS_ON_SHOW) {
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
  }
  
  if((window->flags & WINDOW_FLAGS_MINIMIZE) == WINDOW_FLAGS_MINIMIZE) {
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
  }
  
  if((window->flags & WINDOW_FLAGS_MAXMIZE) == WINDOW_FLAGS_MAXMIZE) {
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
  }
  
  if((window->flags & WINDOW_FLAGS_DISABLE_DECORATIONS) == WINDOW_FLAGS_DISABLE_DECORATIONS) {
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
  }
  
  if((window->flags & WINDOW_FLAGS_CENTER_MOUSE) == WINDOW_FLAGS_CENTER_MOUSE) {
    glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);
  }
  
  if((window->flags & WINDOW_FLAGS_HIDE_CURSOR) == WINDOW_FLAGS_HIDE_CURSOR) {
    window->is_cursor_shown = false;
  }
  
  if((window->flags & WINDOW_FLAGS_FULLSCREEN) == WINDOW_FLAGS_FULLSCREEN) {
    window->is_fullscreen = true; 
  }
  
  if((window->flags & WINDOW_FLAGS_GFX_OPENGL) == WINDOW_FLAGS_GFX_OPENGL) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  }
  
  if((window->flags & WINDOW_FLAGS_GFX_D3D) == WINDOW_FLAGS_GFX_D3D) {
    // @TODO
  }
}

static void create_glfw_handle(Window* window) {
  // Creating the window
  window->handle = glfwCreateWindow(window->width, window->height, window->title.c_str(), nullptr, nullptr);

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

  // Nikol cursor show callback
  event_listen(EVENT_MOUSE_CURSOR_SHOWN, nikol_cursor_show_callback, window);
}
/// Private functions

/// ---------------------------------------------------------------------
/// Window functions

Window* window_open(const i8* title, const i32 width, const i32 height, i32 flags) {
  Window* window = (Window*)memory_allocate(sizeof(Window));

  window->handle = nullptr;
  window->cursor = nullptr;

  window->title  = title;
  window->width  = width; 
  window->height = height; 
  window->flags  = (WindowFlags)flags;

  window->is_focused      = true;
  window->is_fullscreen   = false;
  window->is_cursor_shown = true; 

  window->position            = Vec2(0.0f);
  window->last_mouse_position = Vec2(0.0f);
  window->mouse_position      = window->last_mouse_position;
  window->mouse_offset        = Vec2(0.0f);

  // GLFW init and setup 
  glfwInit();
  set_window_hints(window);
  create_glfw_handle(window);
  set_window_callbacks(window);
  
  // Something wrong...
  if(!window->handle) {
    memory_free(window);
    return nullptr;
  }

  // Setting our `window` as user data in the glfw window
  glfwSetWindowUserPointer(window->handle, window);

  // Set the current context 
  glfwMakeContextCurrent(window->handle);

  if(window->is_fullscreen) {
    window_set_fullscreen(window, true);
  }

  if(!window->is_cursor_shown) {
    glfwSetInputMode(window->handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
  
  NIKOL_LOG_INFO("Window: {t = \"%s\", w = %i, h = %i} was successfully opened", title, width, height);

  return window;
}

void window_close(Window* window) {
  if(window->cursor) {
    glfwDestroyCursor(window->cursor);
  }

  glfwDestroyWindow(window->handle);
  glfwTerminate();
  
  memory_free(window);

  NIKOL_LOG_INFO("Window was successfully closed");
}

void window_poll_events(Window* window) {
  // TODO: Maybe take these system updates somewhere else? 
  input_update();

  glfwPollEvents();
}

void window_swap_buffers(Window* window) {
  glfwSwapBuffers(window->handle);
}

const bool window_is_open(Window* window) {
  return !glfwWindowShouldClose(window->handle);
}

const bool window_is_fullscreen(Window* window) {
  return window->is_fullscreen;
}

const bool window_is_focused(Window* window) {
  return window->is_focused;
}

const bool window_is_shown(Window* window) {
  return glfwGetWindowAttrib(window->handle, GLFW_VISIBLE);
}

const Vec2 window_get_size(Window* window) {
  return Vec2(window->width, window->height);
}

const String window_get_title(Window* window) {
  return window->title.c_str();
}

const Vec2 window_get_monitor_size(Window* window) {
  const GLFWvidmode* video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  return Vec2(video_mode->width, video_mode->height);
}

const f32 window_get_aspect_ratio(Window* window) {
  return ((f32)window->width / (f32)window->height);
}

const f32 window_get_refresh_rate(Window* window) {
  return window->refresh_rate;
}

const WindowFlags window_get_flags(Window* window) {
  return window->flags;
}

const Vec2 window_get_position(Window* window) {
  return window->position;
}

void window_set_current_context(Window* window) {
  glfwMakeContextCurrent(window->handle);
}

void window_set_vsync(Window* window, const bool vsync) {
  glfwSwapInterval(vsync);
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
                         window->position.x, window->position.y, 
                         window->width, window->height, 
                         window->refresh_rate);
  }
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

void window_set_title(Window* window, const String& title) {
  window->title = title; 
  glfwSetWindowTitle(window->handle, window->title.c_str());
}

void window_set_position(Window* window, const i32 x_pos, const i32 y_pos) {
  window->position = Vec2(x_pos, y_pos); 
  glfwSetWindowPos(window->handle, window->position.x, window->position.y);
}

/// Window functions
/// ---------------------------------------------------------------------

} // End of nikol

//////////////////////////////////////////////////////////////////////////
