#pragma once

#include "nikola_base.h"
#include "nikola_containers.h"
#include "nikola_pch.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Threads ***

/// ----------------------------------------------------------------------
/// JobEntryFunc

/// The function callback to be invoked when a worker thread 
/// is done with its work, passing in `params`, and `params_size`.
using JobEntryFunc = std::function<bool(void* params, const sizei params_size)>;

/// JobEntryFunc
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// JobDesc
struct JobDesc {
  JobEntryFunc entry_func; 

  void* params;
  sizei params_size;
};
/// JobDesc
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Job manager functions

/// Initialize the job manager system with `jobs_count` jobs.
NIKOLA_API void job_manager_init(const sizei jobs_count);

/// Shutdown the job manager.
NIKOLA_API void job_manager_shutdown();

/// Push the given `desc` job to the job manager's queue.
NIKOLA_API void job_manager_enqueue_job(const JobDesc& desc);

/// Job manager functions
/// ----------------------------------------------------------------------

/// *** Threads ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
