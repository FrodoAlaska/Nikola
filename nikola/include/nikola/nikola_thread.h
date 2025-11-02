#pragma once

#include "nikola_base.h"
#include "nikola_containers.h"
#include "nikola_pch.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Threads ***

/// ----------------------------------------------------------------------
/// ThreadTaskFn

/// The function callback to be invoked when a worker thread 
/// is done with its work.
using ThreadTaskFn = std::function<void()>;

/// ThreadTaskFn
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ThreadPool 
struct ThreadPool {
  String name; 
  bool is_active;

  DynamicArray<std::thread*> workers; 
  moodycamel::ConcurrentQueue<ThreadTaskFn> tasks;
};
/// ThreadPool 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ThreadPool functions

/// Create a thread pool with a given `name`, with `worker_count` amount 
/// of worker threads the `pool` is allowed to have.
NIKOLA_API void thread_pool_create(ThreadPool* pool, const String& name, const sizei worker_count);

/// Destroy the given `pool`, joining all of its worker threads.
NIKOLA_API void thread_pool_destroy(ThreadPool& pool);

/// Push the given `task` job to the `pool`'s tasks.
NIKOLA_API void thread_pool_push_task(ThreadPool& pool, const ThreadTaskFn& task);

/// Retrieve the approximate amount of tasks left.
NIKOLA_API const sizei thread_pool_get_approx_size(const ThreadPool& pool);

/// ThreadPool functions
/// ----------------------------------------------------------------------

/// *** Threads ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
