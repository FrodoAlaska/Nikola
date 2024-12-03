#include "../nikol_core.hpp"

#include <cstdio>
#include <cstdarg>

//////////////////////////////////////////////////////////////////////////

namespace nikol { // Start of nikol

/// ---------------------------------------------------------------------
/// Logger functions

void logger_log_assert(const i8* expr, const i8* msg, const i8* file, const u32 line_num) {
  fprintf(stderr, "[NIKOL ASSERTION FAILED]: %s\n", msg); 
  fprintf(stderr, "[EXPR]: %s\n", expr); 
  fprintf(stderr, "[FILE]: %s\n", file); 
  fprintf(stderr, "[LINE]: %i\n", line_num); 
}

void logger_log(const LogLevel lvl, const i8* msg, ...) {
  // Getting the correct log level text  
  const i8* log_prefix[] = {"[NIKOL-TRACE]: ", "[NIKOL-DEBUG]: ", "[NIKOL-INFO]: ", "[NIKOL-WARN]: ", "[NIKOL-ERROR]: ", "[NIKOL-FATAL]: "};

  // Trying to unpack the veriadic arguments to add them to the string
  i8 out_msg[32000]; // @TODO: A limited size message will not suffice here for long
  va_list list; 

  // Some arg magic...
  va_start(list, msg);
  vsnprintf(out_msg, sizeof(out_msg), msg, list);
  va_end(list);

  // Printing the log message using different colors depending on the log level.
  // @NOTE: This currently only works on Linux. Windows implementation coming in the future.
  FILE* console = lvl == LOG_LEVEL_ERROR || lvl == LOG_LEVEL_FATAL ? stderr : stdout; 
  const i8* colors[] = {"1;94", "1;96", "1;92", "1;93", "1;91", "1;2;31;40"};
  fprintf(console, "\033[%sm%s%s\033[0m\n", colors[lvl], log_prefix[lvl], out_msg);

  // Can't keep going with a log level of `FATAL`
  if(lvl == LOG_LEVEL_FATAL) {
    event_dispatch(Event{.type = EVENT_APP_QUIT});
  }
}

/// Logger functions
/// ---------------------------------------------------------------------

} // End of nikol

//////////////////////////////////////////////////////////////////////////
