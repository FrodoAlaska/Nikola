#include "nikola/nikola_core.hpp"

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
