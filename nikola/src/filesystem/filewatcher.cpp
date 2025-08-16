#include "nikola/nikola_base.h"
#include "nikola/nikola_file.h"

#include <filewatch/FileWatch.hpp> 

//////////////////////////////////////////////////////////////////////////

namespace nikola {

// @TODO (Filesystem): This doesn't work, because fuck me...

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static FileStatus get_nikola_status(const filewatch::Event event) {
  switch(event) {
    case filewatch::Event::added:
      return FILE_STATUS_CREATED;
	  case filewatch::Event::removed:
      return FILE_STATUS_DELETED;
	  case filewatch::Event::modified:
      return FILE_STATUS_MODIFIED;
	  case filewatch::Event::renamed_old:
    case filewatch::Event::renamed_new:
      return FILE_STATUS_RENAMED;
  }
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Filewatcher functions 

void filewatcher_add_file(const FilePath& path, const FileWatchFunc& callback, const void* user_data) {
  filewatch::FileWatch<FilePath> watcher(
    path, 
    [=](const FilePath& watched_path, const filewatch::Event event) {
      callback(get_nikola_status(event), watched_path, (void*)user_data);
    }
  ); 
}

void filewatcher_add_dir(const FilePath& dir, const FileWatchFunc& callback, const void* user_data) {
  filewatch::FileWatch<FilePath> watcher(
    dir, 
    [=](const FilePath& watched_path, const filewatch::Event event) {
      callback(get_nikola_status(event), watched_path, (void*)user_data);
    }
  ); 
}

/// Filewatcher functions 
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
