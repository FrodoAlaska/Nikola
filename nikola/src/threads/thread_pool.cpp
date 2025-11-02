#include "nikola/nikola_thread.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Callbacks

static void worker_callback(ThreadPool* pool, const sizei worker_index) {
  while(true) {
    if(!pool->is_active) { // Not working anymore! Go back home...
      break;
    }

    // Dequeue a task from the queue

    ThreadTaskFn func;
    bool found_task = pool->tasks.try_dequeue(func);

    if(found_task) { // Found one! Have at it...
      func();
    }
  }

  // Worker done...
}

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ThreadPool functions

void thread_pool_create(ThreadPool* pool, const String& name, const sizei worker_count) {
  NIKOLA_ASSERT(pool, "Invalid ThreadPool given to thread_pool_create");

  pool->name      = name; 
  pool->is_active = true;

  pool->workers.reserve(worker_count);
  for(sizei i = 0; i < worker_count; i++) {
    pool->workers.push_back(new std::thread(worker_callback, pool, i));
  }
}

void thread_pool_destroy(ThreadPool& pool) {
  // Make sure that all of the tasks are done
  while(pool.tasks.size_approx() > 0);

  pool.is_active = false;
  for(auto& worker : pool.workers) {
    worker->join(); 
    delete worker;
  }
}

void thread_pool_push_task(ThreadPool& pool, const ThreadTaskFn& task) {
  pool.tasks.enqueue(task);
}

const sizei thread_pool_get_approx_size(const ThreadPool& pool) {
  return pool.tasks.size_approx();
}

/// ThreadPool functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
