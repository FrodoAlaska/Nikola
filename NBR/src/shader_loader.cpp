#include "nbr.hpp"

#include <nikola/nikola_core.hpp>
#include <nikola/nikola_engine.hpp>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nikola

/// ----------------------------------------------------------------------
/// Shader loader functions

bool shader_loader_load(nikola::NBRShader* shader, const nikola::FilePath& path) {
  nikola::File file;
  if(!nikola::file_open(&file, path, (nikola::i32)std::ios::in)) {
    return false;
  }

  *shader = nikola::file_read_string(file);
  return true;
}

/// Shader loader functions
/// ----------------------------------------------------------------------

} // End of nbr

//////////////////////////////////////////////////////////////////////////
