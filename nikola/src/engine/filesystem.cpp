#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

#include <sstream>
#include <filesystem>

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Private functions
static std::ios::openmode get_mode(const u32 mode) {
  std::ios::openmode cpp_mode;

  if(IS_BIT_SET(mode, FILE_OPEN_READ)) {
    cpp_mode = (std::ios::openmode)(cpp_mode | std::ios::in);
  }
  
  if(IS_BIT_SET(mode, FILE_OPEN_WRITE)) {
    cpp_mode = (std::ios::openmode)(cpp_mode | std::ios::out);
  }
  
  if(IS_BIT_SET(mode, FILE_OPEN_BINARY)) {
    cpp_mode = (std::ios::openmode)(cpp_mode | std::ios::binary);
  }
  
  if(IS_BIT_SET(mode, FILE_OPEN_APPEND)) {
    cpp_mode = (std::ios::openmode)(cpp_mode | std::ios::app);
  }
  
  if(IS_BIT_SET(mode, FILE_OPEN_TRUNCATE)) {
    cpp_mode = (std::ios::openmode)(cpp_mode | std::ios::trunc);
  }
  
  if(IS_BIT_SET(mode, FILE_OPEN_AT_END)) {
    cpp_mode = (std::ios::openmode)(cpp_mode | std::ios::ate);
  }
  
  if(IS_BIT_SET(mode, FILE_OPEN_READ_WRITE)) {
    cpp_mode = (std::ios::openmode)(cpp_mode | (std::ios::in | std::ios::out));
  }

  return cpp_mode;
}
/// Private functions
///---------------------------------------------------------------------------------------------------------------------

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

///---------------------------------------------------------------------------------------------------------------------
/// FilePath functions

FilePath filepath_append(FilePath& base, const FilePath& other) {
  return (std::filesystem::path(base) / other).string(); 
}

FilePath filepath_root_name(const FilePath& path) {
  return std::filesystem::path(path).root_name().string();
}

FilePath filepath_root_dir(const FilePath& path) {
  return std::filesystem::path(path).root_directory().string();
}

FilePath filepath_root_path(const FilePath& path) {
  return std::filesystem::path(path).root_path().string();
}

FilePath filepath_relative_path(const FilePath& path) {
  return std::filesystem::path(path).relative_path().string();
}

FilePath filepath_parent_path(const FilePath& path) {
  return std::filesystem::path(path).parent_path().string();
}

FilePath filepath_filename(const FilePath& path) {
  return std::filesystem::path(path).filename().string();
}

FilePath filepath_stem(const FilePath& path) {
  return std::filesystem::path(path).stem().string();
}

FilePath filepath_extension(const FilePath& path) {
  return std::filesystem::path(path).extension().string();
}

void filepath_set_filename(FilePath& path, const FilePath& name) {
  path = std::filesystem::path(path).replace_filename(name).string();
}

void filepath_set_extension(FilePath& path, const FilePath& ext) {
  path = std::filesystem::path(path).replace_extension(ext).string();
}

void filepath_remove_filename(FilePath& path) {
  path = std::filesystem::path(path).remove_filename().string();
}

bool filepath_has_root_name(const FilePath& path) {
  return std::filesystem::path(path).has_root_name();
}

bool filepath_has_root_dir(const FilePath& path) {
  return std::filesystem::path(path).has_root_directory();
}

bool filepath_has_root_path(const FilePath& path) {
  return std::filesystem::path(path).has_root_path();
}

bool filepath_has_relative_path(const FilePath& path) {
  return std::filesystem::path(path).has_relative_path();
}

bool filepath_has_parent_path(const FilePath& path) {
  return std::filesystem::path(path).has_parent_path();
}

bool filepath_has_filename(const FilePath& path) {
  return std::filesystem::path(path).has_filename();
}

bool filepath_has_stem(const FilePath& path) {
  return std::filesystem::path(path).has_stem();
}

bool filepath_has_extension(const FilePath& path) {
  return std::filesystem::path(path).has_extension();
}

bool filepath_is_empty(const FilePath& path) {
  return std::filesystem::path(path).empty();
}

bool filepath_is_relative(const FilePath& path) {
  return std::filesystem::path(path).is_relative();
}

bool filepath_is_absolute(const FilePath& path) {
  return std::filesystem::path(path).is_absolute();
}

/// FilePath functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// File functions

bool file_open(File* file, const char* path, const u32 mode) {
  NIKOLA_ASSERT(file, "Cannot open an invalid File handle");

  file->open(path, (std::ios::openmode)(mode));
  return file->is_open();
}

bool file_open(File* file, const FilePath& path, const u32 mode) {
  NIKOLA_ASSERT(file, "Cannot open an invalid File handle");

  file->open(path, (std::ios::openmode)(mode));
  return file->is_open();
}

void file_close(File& file) {
  file.close();
}

bool file_is_open(File& file) {
  return file.is_open();
}

void file_seek_write(File& file, const sizei pos) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  file.seekp(pos);
}

void file_seek_read(File& file, const sizei pos) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  file.seekg(pos);
}

const sizei file_tell_write(File& file) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  return file.tellp();
}

const sizei file_tell_read(File& file) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  return file.tellg();
}

const sizei file_get_size(File& file) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  file_seek_read(file, std::ios::end);
  sizei size = file_tell_read(file);
  file_seek_read(file, std::ios::beg);

  return size;
}

bool file_is_empty(File& file) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  return file_get_size(file) <= 0;
}

const sizei file_write_bytes(File& file, const void* buff, const sizei buff_size, const sizei offset) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  file.write((char*)buff, buff_size + offset);
  return (buff_size + offset);
}

const sizei file_read_bytes(File& file, void* out_buff, const sizei size, const sizei offset) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  file.read((char*)out_buff, size + offset);
  return (size + offset);
}

void file_write_string(File& file, const String& string) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  file << string;
}

String file_read_string(File& file) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");

  std::stringstream ss;
  ss << file.rdbuf();

  return ss.str();
}

/// File functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
