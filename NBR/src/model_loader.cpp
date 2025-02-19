#include "nbr.hpp"

#include <nikola/nikola_core.hpp>
#include <nikola/nikola_engine.hpp>

#include <cgltf/cgltf.h>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nbr

/// ----------------------------------------------------------------------
/// Private functions


/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Model loader functions

bool model_loader_load(nikola::NBRModel* model, const nikola::FilePath& path) {
  cgltf_options opts = {};
  cgltf_data* data   = nullptr;
  cgltf_result res   = cgltf_parse_file(&opts, path.string().c_str(), &data);
  
  // Check if everything is okay
  if(result != cgltf_result_success) {
    NIKOLA_LOG_ERROR("Invalid Model file at \'%s\'", path.string().c_str());
    cgltf_free(data);

    return false;
  }

  return true;
}

/// Model loader functions
/// ----------------------------------------------------------------------


} // End of nbr

//////////////////////////////////////////////////////////////////////////
