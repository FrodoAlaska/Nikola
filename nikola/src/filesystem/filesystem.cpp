#include "nikola/nikola_base.h"
#include "nikola/nikola_file.h"

#include <filesystem>

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Filesystem functions

void filesystem_directory_iterate(const FilePath& dir, const FileIterateFunc& iter_func, const void* user_data) {
  if(!filesystem_exists(dir)) {
    NIKOLA_LOG_ERROR("Cannot iterate through a directory (\'%s\') that does not exist", dir.c_str());
    return;
  }

  for(auto& p : std::filesystem::directory_iterator(dir)) {
    iter_func(dir, p.path().string(), (void*)user_data);
  }
}

void filesystem_directory_recurse_iterate(const FilePath& dir, const FileIterateFunc& iter_func, const void* user_data) {
  if(!filesystem_exists(dir)) {
    NIKOLA_LOG_ERROR("Cannot iterate through a directory (\'%s\') that does not exist", dir.c_str());
    return;
  }

  for(auto& p : std::filesystem::recursive_directory_iterator(dir)) {
    iter_func(dir, p.path().string(), (void*)user_data);
  }
}

FilePath filesystem_current_path() {
  return std::filesystem::current_path().string();
}

bool filesystem_exists(const FilePath& path) {
  return std::filesystem::exists(path);
}

sizei filesystem_get_size(const FilePath& path) {
  return std::filesystem::file_size(path);
}

bool filesystem_is_empty(const FilePath& path) {
  return filesystem_get_size(path) <= 0;
}

bool filesystem_create_directory(const FilePath& dir_name) {
  return std::filesystem::create_directory(dir_name);
}

bool filesystem_create_directories(const FilePath& dir_path) {
  return std::filesystem::create_directories(dir_path);
}

/// Filesystem functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
