#include "nikola/nikola_thread.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

using namespace std::chrono_literals; // I FUCKING HATE YOU, C++!!!!!

/// ----------------------------------------------------------------------
/// JobManager
struct JobManager {
  DynamicArray<std::thread> thread_pool; 
  std::mutex queue_lock, execute_lock;

  Queue<JobDesc> queues; 

  sizei threads_max = 0;
  sizei jobs_count  = 0;

  bool is_running;
};

static JobManager s_jobs;
/// JobManager
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static const bool dequeue_job(JobDesc* out_desc) {
  std::unique_lock<std::mutex> lock(s_jobs.execute_lock);
  if(s_jobs.queues.empty()) {
    return false;
    lock.unlock();
  }
  
  *out_desc = s_jobs.queues.front();
  s_jobs.queues.pop();

  lock.unlock();
  return true;
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

static void job_callback() {
  while(true) {
    // Sleep for a little while if there are 
    // no jobs to execute...

    if(!s_jobs.is_running) {
      break;
    }

    JobDesc job;
    if(!dequeue_job(&job)) {
      std::this_thread::sleep_for(10ms); // @TODO (Thread): Yeah, no fucking way...
      continue;
    }

    // Execute the job and remove it from the queue
    if(job.entry_func) {
      job.entry_func(job.params, job.params_size);
    }
  }
}

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Job manager functions

void job_manager_init(const sizei jobs_count) {
  s_jobs.jobs_count = jobs_count;
  s_jobs.is_running = true;

  // @NOTE: We subtract `2` from the number of current threads because we don't 
  // want to starve the system. On some systems, it might even shut us down.
  s_jobs.threads_max = (sizei)(std::thread::hardware_concurrency() - 2);

  // Adding all the requested threads into the pool with 
  // our "blocking" callback.

  s_jobs.thread_pool.resize(s_jobs.threads_max);
  for(nikola::sizei i = 0; i < s_jobs.threads_max; i++) {
    s_jobs.thread_pool[i] = std::thread(job_callback);
  }

  NIKOLA_LOG_INFO("Job manager successfully initialized with \'%zu\' threads", s_jobs.threads_max);
}

void job_manager_shutdown() {
  while(!s_jobs.queues.empty()); // @TEMP (Threads): Do better...
  
  s_jobs.is_running = false;

  for(auto& th : s_jobs.thread_pool) {
    th.join();
  }
  s_jobs.thread_pool.clear();
  
  NIKOLA_LOG_INFO("Job manager was successfully shutdown");
}

void job_manager_enqueue_job(const JobDesc& desc) {
  std::lock_guard<std::mutex> lock(s_jobs.queue_lock);
  s_jobs.queues.push(desc); 
}

/// Job manager functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
