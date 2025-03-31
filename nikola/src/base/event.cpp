#include "nikola/nikola_base.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// EventEntry
struct EventEntry {
  EventFireFn func; 
  void* listener;
};
/// EventEntry

/// EventPool
/// A dynamic array to hold event entries of a specific type
struct EventPool {
  EventEntry* entries;
  sizei size;
  sizei capacity;
};
/// EventPool

/// EventState
struct EventState {
  EventPool event_pool[EVENTS_MAX];
  sizei events_count = 0;
};

static EventState s_state;
/// EventState

/// ---------------------------------------------------------------------
/// Private functions 

static void create_pool(EventType type, const sizei capacity) {
  EventPool* pool = &s_state.event_pool[type]; 

  pool->size     = 0; 
  pool->capacity = capacity;  
  pool->entries  = (EventEntry*)memory_allocate(sizeof(EventEntry) * capacity);
}

static void append_event(const EventType type, const EventEntry& entry) {
  EventPool* pool = &s_state.event_pool[type];

  pool->size++;
  if(pool->size >= pool->capacity) {
    pool->capacity = pool->size + (pool->size / 2);
    pool->entries  = (EventEntry*)memory_allocate(sizeof(EventEntry) * pool->capacity);
  }

  pool->entries[pool->size - 1] = entry;
}

/// Private functions 
/// ---------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// Event functions

void event_init() {
  for(sizei i = 0; i < EVENTS_MAX; i++) {
    create_pool((EventType)i, 64);
  }

  NIKOLA_LOG_INFO("Event system was successfully initialized");
}

void event_shutdown() {
  for(sizei i = 0; i < EVENTS_MAX; i++) {
    memory_free(s_state.event_pool[i].entries);
  }

  NIKOLA_LOG_INFO("Event system was successfully shutdown");
}

void event_listen(const EventType type, const EventFireFn& func, const void* listener) {
  append_event(type, EventEntry{func, (void*)listener});
}

const bool event_dispatch(const Event& event, const void* dispatcher) {
  for(sizei i = 0; i < s_state.event_pool[event.type].size; i++) {
    // Calling all of the callbacks with the same `event.type` 
    EventEntry entry = s_state.event_pool[event.type].entries[i];
    if(entry.func(event, dispatcher, entry.listener)) {
      return true;
    }
  }

  return false;
}

/// Event functions
/// ---------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
