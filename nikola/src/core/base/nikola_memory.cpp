#include "nikola/nikola_core.hpp"

#include <cstdlib>
#include <cstring>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// MemoryState
struct MemoryState {
  sizei alloc_count = 0; 
  sizei free_count  = 0;

  sizei alloc_total_bytes = 0;
};

static MemoryState s_state;
/// MemoryState

/// ---------------------------------------------------------------------
/// Memory functions

void* memory_allocate(const sizei size) {
  void* ptr = malloc(size);
  NIKOLA_ASSERT(ptr, "Could not allocate any more memory!");

  s_state.alloc_count++;
  s_state.alloc_total_bytes += size;

  return ptr;
}

void* memory_reallocate(void* ptr, const sizei new_size) {
  void* temp_ptr = realloc(ptr, new_size);

  NIKOLA_ASSERT(ptr, "Could not allocate any more memory!");
  ptr = temp_ptr;
  
  s_state.alloc_count++;
  s_state.alloc_total_bytes += new_size;
  
  return ptr;
}

void* memory_set(void* ptr, const i32 value, const sizei ptr_size) {
  NIKOLA_ASSERT(ptr, "Cannot set values of invalid pointer");
  ptr = memset(ptr, value, ptr_size);

  return ptr;
}

void* memory_zero(void* ptr, const sizei ptr_size) {
  return memory_set(ptr, 0, ptr_size);
}

void* memory_blocks_allocate(const sizei count, const sizei block_size) {
  void* ptr = calloc(count, block_size);
  NIKOLA_ASSERT(ptr, "Could not allocate any more memory!");

  s_state.alloc_count++;
  s_state.alloc_total_bytes += (count * block_size);

  return ptr;
}

void* memory_copy(void* dest, const void* src, const sizei src_size) {
  NIKOLA_ASSERT(dest && src, "Cannot copy around invalid memory blocks!");
  
  void* ptr = memcpy(dest, src, src_size);
  NIKOLA_ASSERT(ptr, "Could not allocate any more memory!");

  return ptr;
}

void memory_free(void* ptr) {
  NIKOLA_ASSERT(ptr, "Cannot free an invalid pointer!");
  free(ptr);

  s_state.alloc_count--;
  s_state.free_count++;
}

const sizei memory_get_allocations_count() {
  return s_state.free_count + s_state.alloc_count;
}

const sizei memory_get_frees_count() {
  return s_state.free_count;
}

const sizei memory_get_allocation_bytes() {
  return s_state.alloc_total_bytes;
}

/// Memory functions
/// ---------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
