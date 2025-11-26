#pragma once

#include "nikola_base.h"
#include "nikola_containers.h"

#include <tracy/Tracy.hpp>

#include <chrono>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Timers ***

///---------------------------------------------------------------------------------------------------------------------
/// PerfTimer 
struct PerfTimer {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  
  f32 to_milliseconds = 0.0f;
  f32 to_seconds      = 0.0f;
  f32 to_minutes      = 0.0f;
};
/// PerfTimer 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PerfTimer functions

/// Start the performance timer `timer`.
NIKOLA_API void perf_timer_start(PerfTimer& timer);

/// Stop the performance timer `timer`.
NIKOLA_API void perf_timer_stop(PerfTimer& timer);

/// Convert the contents of `timer` to a string format.
NIKOLA_API String perf_timer_to_string(PerfTimer& timer);

/// PerfTimer functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Macros

#if NIKOLA_BUILD_DEBUG == 1 
  #define NIKOLA_PERF_TIMER_BEGIN(timer)    nikola::perf_timer_start(timer);
  #define NIKOLA_PERF_TIMER_END(timer, tag) nikola::perf_timer_stop(timer); NIKOLA_LOG_TRACE("\'%s\' took %s to run", tag, nikola::perf_timer_to_string(timer).c_str());
#else
  #define NIKOLA_PERF_TIMER_BEGIN(timer)
  #define NIKOLA_PERF_TIMER_END(timer, tag)
#endif

#define NIKOLA_PROFILE_FUNCTION()           ZoneScoped
#define NIKOLA_PROFILE_FUNCTION_NAMED(name) ZoneScopedN(name)

/// Macros
///---------------------------------------------------------------------------------------------------------------------


///---------------------------------------------------------------------------------------------------------------------
/// Timer 
struct Timer {
  /// The internal counter of the timer.
  f32 counter = 0.0f; 

  /// The maximum limit of the timer.
  f32 limit   = 0.0f;

  /// Determines if the timer is one shot or not.
  bool is_one_shot = false;

  /// Determines if the timer is currently active or not.
  bool is_active   = true;

  /// Determines if the tiemr has run out or still going.
  bool has_runout  = false;
};
/// Timer 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Timer functions

/// Create a new `timer` with a maximum `limit`.
NIKOLA_API void timer_create(Timer* timer, const f32 limit, const bool one_shot, const bool initial_active = true);

/// Let the given `timer` tick, scaled by `delta_time.
NIKOLA_API void timer_update(Timer& timer, const f32 delta_time);

/// Reset the `timer` to its initial state.
NIKOLA_API void timer_reset(Timer& timer);

/// Timer functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Timers ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
