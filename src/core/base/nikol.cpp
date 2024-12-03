#include "../nikol_core.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikol { // Start of nikol

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

} // End of nikol

//////////////////////////////////////////////////////////////////////////
