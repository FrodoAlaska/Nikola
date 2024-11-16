#include "nikol_core.h"

#include <GLFW/glfw3.h>

//////////////////////////////////////////////////////////////////////////

namespace nikol { // Start of nikol

/// ClockState
struct ClockState {
  i64 frame_count = 0; 

  f64 last_frame_time, delta_time; 
  f64 fps, previous_time, current_time;
};

static ClockState s_state;
/// ClockState

/// ---------------------------------------------------------------------
/// Clock functions

void niclock_update() {
  // Calculating the delta time 
  s_state.delta_time      = glfwGetTime() - s_state.last_frame_time; // TODO: Maybe don't have a refrence to GLFW?
  s_state.last_frame_time = glfwGetTime();

  // Calculating the FPS 
  s_state.frame_count++;
  s_state.current_time = glfwGetTime();

  if((s_state.current_time - s_state.previous_time) >= 1.0f) {
    s_state.fps           = s_state.frame_count;
    s_state.previous_time = s_state.current_time;
    s_state.frame_count   = 0;
  }
}

const f64 niclock_get_time() {
  return glfwGetTime();
}

const f64 niclock_get_fps() {
  return s_state.fps;
}

const f64 niclock_get_delta_time() {
  return s_state.delta_time;
}

/// Clock functions
/// ---------------------------------------------------------------------

} // End of nikol

//////////////////////////////////////////////////////////////////////////
