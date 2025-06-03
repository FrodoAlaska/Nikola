#include "nikola/nikola_base.h"
#include "nikola/nikola_event.h"

#include <cstdio>
#include <cstdarg>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ---------------------------------------------------------------------
/// Logger functions

void logger_log_assert(const char* expr, const char* msg, const char* file, const u32 line_num) {
  fprintf(stderr, "[NIKOLA ASSERTION FAILED]: %s\n", msg); 
  fprintf(stderr, "[EXPR]: %s\n", expr); 
  fprintf(stderr, "[FILE]: %s\n", file); 
  fprintf(stderr, "[LINE]: %i\n", line_num); 
}

void logger_log(const LogLevel lvl, const char* msg, ...) {
  // Getting the correct log level text  
  const char* log_prefix[] = {"[NIKOLA-TRACE]: ", "[NIKOLA-DEBUG]: ", "[NIKOLA-INFO]: ", "[NIKOLA-WARN]: ", "[NIKOLA-ERROR]: ", "[NIKOLA-FATAL]: "};

  // Trying to unpack the veriadic arguments to add them to the string
  char out_msg[32000]; // @TODO: A limited size message will not suffice here for long
  va_list list; 

  // Some arg magic...
  va_start(list, msg);
  vsnprintf(out_msg, sizeof(out_msg), msg, list);
  va_end(list);

  // Printing the log message using different colors depending on the log level.
  // @NOTE: This currently only works on Linux. Windows implementation coming in the future.
  FILE* console = lvl == LOG_LEVEL_ERROR || lvl == LOG_LEVEL_FATAL ? stderr : stdout; 
  const char* colors[] = {"1;94", "1;96", "1;92", "1;93", "1;91", "1;2;31;40"};
  fprintf(console, "\033[%sm%s%s\033[0m\n", colors[lvl], log_prefix[lvl], out_msg);

  // Can't keep going with a log level of `FATAL`
  if(lvl == LOG_LEVEL_FATAL) {
    event_dispatch(Event{.type = EVENT_APP_QUIT});
  }
}

/// Logger functions
/// ---------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
