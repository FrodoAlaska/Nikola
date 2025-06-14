#include "nikola/nikola_timer.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_containers.h"

#include <chrono>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Timers ***

///---------------------------------------------------------------------------------------------------------------------
/// PerfTimer functions

void perf_timer_start(PerfTimer& timer) {
  timer.start = std::chrono::high_resolution_clock::now();

  timer.to_milliseconds = 0.0f;
  timer.to_seconds      = 0.0f;
  timer.to_minutes      = 0.0f;
}

void perf_timer_stop(PerfTimer& timer) {
  timer.end = std::chrono::high_resolution_clock::now();
  
  std::chrono::duration<f32, std::milli> dur_ms = timer.end - timer.start; 

  timer.to_milliseconds = dur_ms.count(); 
  timer.to_seconds      = timer.to_milliseconds / 1000;
  timer.to_minutes      = timer.to_seconds / 60;
}

String perf_timer_to_string(PerfTimer& timer) {
  return (std::to_string(timer.to_milliseconds) + "ms, " + 
          std::to_string(timer.to_seconds) + "sec, " + 
          std::to_string(timer.to_minutes) + "min");
}

/// PerfTimer functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Timer functions

void timer_create(Timer* timer, const f32 limit, const bool one_shot, const bool initial_active) {
  memory_zero(timer, sizeof(Timer));

  timer->limit = limit; 
  
  timer->is_one_shot = one_shot; 
  timer->is_active   = initial_active; 
}

void timer_update(Timer& timer) {
  // An inactive timer shouldn't run logic
  if(!timer.is_active) {
    return;
  }
  
  // Keep counting
  timer.counter++;

  // Check if the timer has reached its limit
  if(timer.counter > timer.limit) {
    timer.counter    = 0.0f;
    timer.has_runout = true;

    // There's no reason to deactivate the timer if it needs 
    // to run again. Otherwise, the timer is just a one shot 
    // timer and should be deactivated.
    timer.is_active  = !timer.is_one_shot ? true : false;
  }
  else {
    timer.has_runout = false;
  }
}

void timer_reset(Timer& timer) {
  timer.is_active  = true; 
  timer.counter    = 0.0f;
  timer.has_runout = false;
}

/// Timer functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Timers ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
