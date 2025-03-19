#include "nikola/nikola_base.h"
#include "nikola/nikola_file.h"

#include <filesystem>

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Filesystem functions

void filesystem_directory_iterate(const FilePath& dir, const FileIterateFunc& iter_func, const void* user_data) {
  for(auto& p : std::filesystem::directory_iterator(dir)) {
    iter_func(dir, nikola::FilePath(p.path().string()), (void*)user_data);
  }
}

void filesystem_directory_recurse_iterate(const FilePath& dir, const FileIterateFunc& iter_func, const void* user_data) {
  for(auto& p : std::filesystem::recursive_directory_iterator(dir)) {
    iter_func(dir, nikola::FilePath(p.path().string()), (void*)user_data);
  }
}

FilePath filesystem_current_path() {
  return std::filesystem::current_path().string();
}

bool filesystem_exists(const FilePath& path) {
  return std::filesystem::exists(path);
}

/// Filesystem functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
