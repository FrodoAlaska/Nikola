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
 
using JobEntryFunc = bool(*)(void* params, const sizei params_size);

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

NIKOLA_API void job_manager_init(const sizei jobs_count);

NIKOLA_API void job_manager_shutdown();

NIKOLA_API void job_manager_enqueue_job(const JobDesc& desc);

/// Job manager functions
/// ----------------------------------------------------------------------

/// *** Threads ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
