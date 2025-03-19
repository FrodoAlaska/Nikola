#include "nikola/nikola_base.h"
#include "nikola/nikola_file.h"

#include <sstream>
#include <filesystem>

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Private functions
static std::ios::openmode get_mode(const i32 mode) {
  std::ios::openmode cpp_mode = 0;

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
/// File functions

bool file_open(File* file, const char* path, const i32 mode) {
  NIKOLA_ASSERT(file, "Cannot open an invalid File handle");

  file->open(path, get_mode(mode));
  return file->is_open();
}

bool file_open(File* file, const FilePath& path, const i32 mode) {
  NIKOLA_ASSERT(file, "Cannot open an invalid File handle");

  file->open(path, get_mode(mode));
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
