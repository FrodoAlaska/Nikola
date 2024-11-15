#include "nikol_core.h"
#include "ishtar/ishtar.h"

//////////////////////////////////////////////////////////////////////////

namespace nikol { // Start of nikol

/// Event pool
/// EW!!!
static ishtar::DynamicArray<EventFireFn> s_event_pool[EVENTS_MAX];

/// ---------------------------------------------------------------------
/// Event functions

void event_init() {
  // Reserving some memory for each event pool to help performance
  for(sizei i = 0; i < EVENTS_MAX; i++) {
    s_event_pool[i].reserve(64);
  }
}

void event_shutdown() {
  for(sizei i = 0; i < EVENTS_MAX; i++) {
    s_event_pool[i].clear();
  } 
}

void event_listen(const EventType type, const EventFireFn& func) {
  s_event_pool[type].append(func);
}

const bool event_dispatch(const Event& event) {
  for(sizei i = 0; i < s_event_pool[event.type].size; i++) {
    // Calling all of the callbacks with the same `event.type` 
    EventFireFn func = s_event_pool[event.type][i];
    if(func(event)) {
      return true;
    }
  }

  return false;
}

/// Event functions
/// ---------------------------------------------------------------------

} // End of nikol

//////////////////////////////////////////////////////////////////////////
