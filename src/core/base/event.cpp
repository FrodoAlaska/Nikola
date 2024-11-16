#include "nikol_core.h"
#include "ishtar/ishtar.h"

//////////////////////////////////////////////////////////////////////////

namespace nikol { // Start of nikol

/// EventEntry
struct EventEntry {
  EventFireFn func; 
  void* listener;
};
/// EventEntry

/// Event pool
/// EW!!!
static ishtar::DynamicArray<EventEntry> s_event_pool[EVENTS_MAX];

/// ---------------------------------------------------------------------
/// Event functions

void event_init() {
  // Reserving some memory for each event pool to help performance
  for(sizei i = 0; i < EVENTS_MAX; i++) {
    s_event_pool[i].reserve(64);
  }

  NIKOL_LOG_INFO("Event system was successfully initialized");
}

void event_shutdown() {
  for(sizei i = 0; i < EVENTS_MAX; i++) {
    s_event_pool[i].clear();
  } 
  
  NIKOL_LOG_INFO("Event system was successfully shutdown");
}

void event_listen(const EventType type, const EventFireFn& func, const void* listener) {
  s_event_pool[type].append(EventEntry{func, (void*)listener});
}

const bool event_dispatch(const Event& event, const void* dispatcher) {
  for(sizei i = 0; i < s_event_pool[event.type].size; i++) {
    // Calling all of the callbacks with the same `event.type` 
    EventEntry entry = s_event_pool[event.type][i];
    if(entry.func(event, dispatcher, entry.listener)) {
      return true;
    }
  }

  return false;
}

/// Event functions
/// ---------------------------------------------------------------------

} // End of nikol

//////////////////////////////////////////////////////////////////////////
