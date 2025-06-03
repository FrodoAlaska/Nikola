#include "nikola/nikola_base.h"
#include "nikola/nikola_event.h"
#include "nikola/nikola_input.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ---------------------------------------------------------------------
/// Nikol init functions

const bool init() {
  event_init();
  input_init();

  return true;
}

void shutdown() {
  event_shutdown();
}

/// Nikol init functions
/// ---------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
