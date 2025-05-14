#pragma once

#include "nikola_base.h"
#include "nikola_containers.h"

#include <chrono>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Timers ***

///---------------------------------------------------------------------------------------------------------------------
/// PerfTimer 
struct PerfTimer {
  std::chrono::time_point<std::chrono::system_clock> start, end;
  
  f32 to_milliseconds = 0.0f;
  f32 to_seconds      = 0.0f;
  f32 to_minutes      = 0.0f;
};
/// PerfTimer 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PerfTimer functions

void perf_timer_start(PerfTimer& timer);

void perf_timer_stop(PerfTimer& timer);

String perf_timer_to_string(PerfTimer& timer);

/// PerfTimer functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Macros

#if NIKOLA_BUILD_DEBUG == 1 
  #define NIKOLA_PERF_TIMER_BEGIN(timer) nikola::perf_timer_start(timer);
  #define NIKOLA_PERF_TIMER_END(timer, tag) nikola::perf_timer_stop(timer); NIKOLA_LOG_TRACE("\'%s\' took %s to run", tag, nikola::perf_timer_to_string(timer).c_str());
#else
  #define NIKOLA_PERF_TIMER_BEGIN()
  #define NIKOLA_PERF_TIMER_END()
#endif

/// Macros
///---------------------------------------------------------------------------------------------------------------------


///---------------------------------------------------------------------------------------------------------------------
/// Timer 
struct Timer {
  f32 counter = 0.0f; 
  f32 limit   = 0.0f;

  bool is_one_shot = false;
  bool is_active   = true;
  bool has_runout  = false;
};
/// Timer 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Timer functions

void timer_create(Timer* timer, const f32 limit, const bool one_shot, const bool initial_active = true);

void timer_update(Timer& timer);

void timer_reset(Timer& timer);

/// Timer functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Timers ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
