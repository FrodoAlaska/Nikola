#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

#include <filesystem>
#include <chrono>

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// FileWatchEntry
struct FileWatchEntry { 
  FilePath path;
  std::filesystem::file_time_type last_write_time; 

  FileWatchFunc callback;
  void* user_data;

  bool is_valid;
};
/// FileWatchEntry
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Globals
static DynamicArray<FileWatchEntry> s_entries;
/// Globals
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Callbacks

static void add_files_iterate(const FilePath& base, FilePath current_path, void* user_data) {
  FileWatchEntry* entry = (FileWatchEntry*)user_data;
 
  entry->path            = base;
  entry->path            = filepath_append(entry->path, current_path);
  entry->last_write_time = std::filesystem::last_write_time(entry->path),

  s_entries.push_back(*entry);
}

/// Callbacks
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Filewatcher functions 

void filewatcher_add_file(const FilePath& path, const FileWatchFunc& callback, const void* user_data) {
  s_entries.push_back(FileWatchEntry {
    .path            = path, 
    .last_write_time = std::filesystem::last_write_time(path),
    .callback        = callback, 
    .user_data       = (void*)user_data, 
    .is_valid        = true,
  });
}

void filewatcher_add_dir(const FilePath& dir, const FileWatchFunc& callback, const void* user_data, const bool recurse) {
  FileWatchEntry entry = {
    .callback        = callback, 
    .user_data       = (void*)user_data, 
    .is_valid        = true,
  };

  if(recurse) {
    filesystem_directory_recurse_iterate(dir, add_files_iterate, &entry);
  }
  else {
    filesystem_directory_iterate(dir, add_files_iterate, &entry);
  }
}

void filewatcher_update() {
  for(auto& entry : s_entries) {
    // We don't care about invalid entries 
    if(!entry.is_valid) {
      continue;
    }

    // If the file does not exist, it probably means 
    // that the file was deleted at some point. 
    if(!filesystem_exists(entry.path)) {
      entry.callback(FILE_STATUS_DELETED, entry.path, entry.user_data);
      entry.is_valid = false;

      continue;
    }

    // If a file has changed its last write time, it probably means  
    // it was modified at some point. 
    std::filesystem::file_time_type current_write_time = std::filesystem::last_write_time(entry.path); 
    if(entry.last_write_time != current_write_time) {
      entry.callback(FILE_STATUS_MODIFIED, entry.path, entry.user_data);
      entry.last_write_time = current_write_time;
    }
  }
}

void filewatcher_shutdown() {
  s_entries.clear();
}

/// Filewatcher functions 
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
