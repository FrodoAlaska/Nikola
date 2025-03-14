#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

#include <filesystem>

//////////////////////////////////////////////////////////////////////////

namespace nikola {

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

bool filepath_is_dir(const FilePath& path) {
  return std::filesystem::is_directory(path);
}

/// FilePath functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
