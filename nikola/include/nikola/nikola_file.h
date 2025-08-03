#pragma once

#include "nikola_base.h"
#include "nikola_containers.h"
#include "nikola_math.h"
#include "nikola_audio.h"

#include "nikola_pch.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

// Forward declaration to help with compilation time.
struct Camera; 
struct DirectionalLight;
struct PointLight;
struct SpotLight;

struct PhysicsBody;
struct PhysicsBodyDesc;
struct Collider;
struct ColliderDesc;

struct NBRHeader;
struct NBRTexture;
struct NBRCubemap;
struct NBRShader;
struct NBRMaterial;
struct NBRMesh;
struct NBRModel;
struct NBRAnimation;
struct NBRGlyph;
struct NBRFont;
struct NBRAudio;

/// ----------------------------------------------------------------------
/// *** File system ***

///---------------------------------------------------------------------------------------------------------------------
/// FileOpenMode
enum FileOpenMode {
  /// Open a file in read-only mode.
  FILE_OPEN_READ       = 6 << 0,
 
  /// Open a file in write-only mode.
  FILE_OPEN_WRITE      = 6 << 1,
 
  /// Open a binary file.
  FILE_OPEN_BINARY     = 6 << 2,
 
  /// Open a file and append any extra data at the end of the file.
  FILE_OPEN_APPEND     = 6 << 3,
 
  /// Open a file and remove any existing data within.
  FILE_OPEN_TRUNCATE   = 6 << 4,
 
  /// Open a file and start at the end.
  FILE_OPEN_AT_END     = 6 << 5,

  /// Open a file in read and write mode.
  FILE_OPEN_READ_WRITE = 6 << 6
};
/// FileOpenMode
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FileStatus
enum FileStatus {
  /// Used when a file gets created.
  FILE_STATUS_CREATED, 
  
  /// Used when a file gets modified.
  FILE_STATUS_MODIFIED, 
  
  /// Used when a file gets deleted.
  FILE_STATUS_DELETED, 

  /// Used when a file gets renamed 
  FILE_STATUS_RENAMED, 
};
/// FileStatus
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FilePath
using FilePath = String;
/// FilePath
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// File
using File = std::fstream;
/// File
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FileIterateFunc callback
using FileIterateFunc = void(*)(const FilePath& base_dir, const FilePath& current_path, void* user_data);
/// FileIterateFunc callback
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FileWatchFunc
using FileWatchFunc = void(*)(const FileStatus status, const FilePath& path, void* user_data);
/// FileWatchFunc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Predefines to prevent circular dependencies

///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Filesystem functions

/// Iterate through the given `dir`, calling `iter_func` for each entry and passing `user_data`. 
NIKOLA_API void filesystem_directory_iterate(const FilePath& dir, const FileIterateFunc& iter_func, const void* user_data = nullptr);

/// Recursively iterate through the given `dir`, calling `iter_func` for each entry and passing `user_data`.
NIKOLA_API void filesystem_directory_recurse_iterate(const FilePath& dir, const FileIterateFunc& iter_func, const void* user_data = nullptr);

/// Get the current full path of the running process.
NIKOLA_API FilePath filesystem_current_path();

/// Check if the file at `path` exists.
NIKOLA_API bool filesystem_exists(const FilePath& path);

/// Return the size in bytes of the file found at `path`. 
NIKOLA_API sizei filesystem_get_size(const FilePath& path);

/// Returns `true` if the file at `path` is completely empty. Otherwise, the functions returns `false`.
NIKOLA_API bool filesystem_is_empty(const FilePath& path);

/// Create a directory with the name `dir_name`. 
/// Returns `true` if the directory was created. Otherwise, the function returns `false`.
NIKOLA_API bool filesystem_create_directory(const FilePath& dir_name);

/// Create all of the directories (if they don't exist) in the given `dir_path`. 
/// Returns `true` if the directories were created. Otherwise, the function returns `false`.
NIKOLA_API bool filesystem_create_directories(const FilePath& dir_path);

/// Filesystem functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Filewatcher functions 

/// Add the given file at `path` to the file watcher, with `callback` to be invoked later, passing in `user_data`.
NIKOLA_API void filewatcher_add_file(const FilePath& path, const FileWatchFunc& callback, const void* user_data);

/// Add all the files at `dir` to the file watcher, with `callback` to be invoked later, passing in `user_data`. 
NIKOLA_API void filewatcher_add_dir(const FilePath& dir, const FileWatchFunc& callback, const void* user_data);

/// Filewatcher functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FilePath functions

/// Append the given `other` to `base` and return the result.
///
/// @NOTE: This function will adhere to the specific operating systems's seperator.
NIKOLA_API FilePath filepath_append(const FilePath& base, const FilePath& other);

/// Retrieve the root name (for example, `C:` on Windows) of `path`.
/// 
/// @EXAMPLE: The root namt would be `C:` on Windows.
NIKOLA_API FilePath filepath_root_name(const FilePath& path);

/// Retrieve the root directory of `path`.
/// 
/// @EXAMPLE: The root directory would be `\` on Windows.
NIKOLA_API FilePath filepath_root_dir(const FilePath& path);

/// Retrieve the full root path of `path`.
/// 
/// @EXAMPLE: The root path would be `C:\` on Windows.
NIKOLA_API FilePath filepath_root_path(const FilePath& path);

/// Retrieve the full relative path of `path`. 
///
/// @EXAMPLE: The relative path of `C:\res\shaders\shader.nbr` would be `res\shaders\shader.nbr`.
NIKOLA_API FilePath filepath_relative_path(const FilePath& path);

/// Retrieve the full parent path of `path`. 
///
/// @EXAMPLE: The parent path of `res/shaders/shader.nbr` would be `res/shaders/`.
NIKOLA_API FilePath filepath_parent_path(const FilePath& path);

/// Retrieve the filename of `path`.
///
/// @EXAMPLE: The filename of `res/shaders/shader.nbr` would be `shader.nbr`.
NIKOLA_API FilePath filepath_filename(const FilePath& path);

/// Retrieve the stem of `path`.
///
/// @EXAMPLE: The stem of `res/shaders/shader.nbr` would be `shader`.
NIKOLA_API FilePath filepath_stem(const FilePath& path);

/// Retrieve the extension of `path`.
///
/// @EXAMPLE: The extension of `res/shaders/shader.nbr` would be `.nbr`.
NIKOLA_API FilePath filepath_extension(const FilePath& path);

/// Set the filename of `path` to the given `name`.
NIKOLA_API void filepath_set_filename(FilePath& path, const FilePath& name);

/// Set the extension of `path` to the given `ext`.
NIKOLA_API void filepath_set_extension(FilePath& path, const FilePath& ext);

/// Remove the filename of `path`. 
///
/// @EXAMPLE: The path `res/shaders/shader.nbr` will become `res/shaders/`
NIKOLA_API void filepath_remove_filename(FilePath& path);

/// Check if `path` has a root name.
NIKOLA_API bool filepath_has_root_name(const FilePath& path);

/// Check if `path` has a root directory.
NIKOLA_API bool filepath_has_root_dir(const FilePath& path);

/// Check if `path` has a root path.
NIKOLA_API bool filepath_has_root_path(const FilePath& path);

/// Check if `path` has a ralative path.
NIKOLA_API bool filepath_has_relative_path(const FilePath& path);

/// Check if `path` has a parent path.
NIKOLA_API bool filepath_has_parent_path(const FilePath& path);

/// Check if `path` has a filename.
NIKOLA_API bool filepath_has_filename(const FilePath& path);

/// Check if `path` has a stem.
NIKOLA_API bool filepath_has_stem(const FilePath& path);

/// Check if `path` has an extension.
NIKOLA_API bool filepath_has_extension(const FilePath& path);

/// Check if `path` is an empty string.
NIKOLA_API bool filepath_is_empty(const FilePath& path);

/// Check if `path` is a relative path.
NIKOLA_API bool filepath_is_relative(const FilePath& path);

/// Check if `path` is an absolute path.
NIKOLA_API bool filepath_is_absolute(const FilePath& path);

/// Check if `path` is a directory.
NIKOLA_API bool filepath_is_dir(const FilePath& path);

/// FilePath functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// File functions

/// Open `file` with OR'd flags `mode` at C-string `path`, and return `true` if 
/// the operation was successfull and `false` otherwise.
///
/// `mode`:
///   - `FILE_OPEN_READ`       = Open `file` in read-only mode.
///   - `FILE_OPEN_WRITE`      = Open `file` in write-only mode.
///   - `FILE_OPEN_BINARY`     = Open `file` in binary mode.
///   - `FILE_OPEN_APPEND`     = Open `file` and append any new data at the end.
///   - `FILE_OPEN_TRUNCATE`   = Open `file` and delete any existing data.
///   - `FILE_OPEN_AT_END`     = Open `file` and start at the very end.
///   - `FILE_OPEN_READ_WRITE` = Open `file` in read and write mode.
NIKOLA_API bool file_open(File* file, const char* path, const i32 mode);

/// Open `file` with OR'd flags `mode` at `FilePath` `path`, and return `true` if 
/// the operation was successfull and `false` otherwise.
///
/// `mode`:
///   - `FILE_OPEN_READ`       = Open `file` in read-only mode.
///   - `FILE_OPEN_WRITE`      = Open `file` in write-only mode.
///   - `FILE_OPEN_BINARY`     = Open `file` in binary mode.
///   - `FILE_OPEN_APPEND`     = Open `file` and append any new data at the end.
///   - `FILE_OPEN_TRUNCATE`   = Open `file` and delete any existing data.
///   - `FILE_OPEN_AT_END`     = Open `file` and start at the very end.
///   - `FILE_OPEN_READ_WRITE` = Open `file` in read and write mode.
NIKOLA_API bool file_open(File* file, const FilePath& path, const i32 mode);

/// Close `file` if it is open.
NIKOLA_API void file_close(File& file);

/// Return `ture` if `file` is currently open and `false` otherwise.
NIKOLA_API bool file_is_open(File& file);

/// Seek the write pointer in `file` to `pos`.
NIKOLA_API void file_seek_write(File& file, const sizei pos);

/// Seek the read pointer in `file` to `pos`.
NIKOLA_API void file_seek_read(File& file, const sizei pos);

/// Return the current position of the write pointer in `file`.
NIKOLA_API const sizei file_tell_write(File& file);

/// Return the current position of the read pointer in `file`.
NIKOLA_API const sizei file_tell_read(File& file);

/// Write `buff` with `size` in bytes into `file` 
/// and return the amount of bytes written.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API const sizei file_write_bytes(File& file, const void* buff, const sizei buff_size);

/// Write the given `str` into `file` as a stream of bytes.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API void file_write_bytes(File& file, const String& str);

/// A list of functions to write in bytes the information of the given 
/// `NBR*` structures into `file`. 
///
/// @NOTE: These functions will raise an error if `file` is not opened.
NIKOLA_API void file_write_bytes(File& file, const NBRHeader& header);
NIKOLA_API void file_write_bytes(File& file, const NBRTexture& texture);
NIKOLA_API void file_write_bytes(File& file, const NBRCubemap& cubemap);
NIKOLA_API void file_write_bytes(File& file, const NBRShader& shader);
NIKOLA_API void file_write_bytes(File& file, const NBRMaterial& material);
NIKOLA_API void file_write_bytes(File& file, const NBRMesh& mesh);
NIKOLA_API void file_write_bytes(File& file, const NBRModel& model);
NIKOLA_API void file_write_bytes(File& file, const NBRAnimation& anim);
NIKOLA_API void file_write_bytes(File& file, const NBRFont& font);
NIKOLA_API void file_write_bytes(File& file, const NBRAudio& audio);

/// Write the contents of the given `transform` into `file`.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API void file_write_bytes(File& file, const Transform& transform);

/// Write the contents of the given `camera` into `file`.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API void file_write_bytes(File& file, const Camera& camera);

/// Write the contents of different light types, `file`, into `file`.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API void file_write_bytes(File& file, const DirectionalLight& light);
NIKOLA_API void file_write_bytes(File& file, const PointLight& light);
NIKOLA_API void file_write_bytes(File& file, const SpotLight& light);

/// Write the contents of the given `source` into `file`.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API void file_write_bytes(File& file, const AudioSourceID& source);

/// Write the contents of the given `listener_desc` into `file`.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API void file_write_bytes(File& file, const AudioListenerDesc& listener_desc);

/// Write the contents of the given `body` into `file`.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API void file_write_bytes(File& file, const PhysicsBody* body);

/// Write the contents of the given `collider` into `file`.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API void file_write_bytes(File& file, const Collider* collider);

/// Write the given `string` into `file` as a string representation.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API void file_write_string(File& file, const String& string);

/// Read data into `out_buff` with `size` in bytes from `file` 
/// and return the amount of bytes read.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API const sizei file_read_bytes(File& file, void* out_buff, const sizei size);

/// Return a `String` from `file` and it into `str`.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API void file_read_bytes(File& file, String* str);

/// A list of functions to read the contents of `file` into the given 
/// `NBR*` structures.
///
/// @NOTE: These functions will allocated memory for the internal 
/// data of the `NBR*` structures. You will need to free the allocated 
/// memory after calling these functions.
///
/// @NOTE: These functions will raise an error if `file` is not opened.

NIKOLA_API void file_read_bytes(File& file, NBRHeader* out_header);
NIKOLA_API void file_read_bytes(File& file, NBRTexture* out_texture);
NIKOLA_API void file_read_bytes(File& file, NBRCubemap* out_cubemap);
NIKOLA_API void file_read_bytes(File& file, NBRShader* out_shader);
NIKOLA_API void file_read_bytes(File& file, NBRMaterial* out_material);
NIKOLA_API void file_read_bytes(File& file, NBRMesh* out_mesh);
NIKOLA_API void file_read_bytes(File& file, NBRModel* out_model);
NIKOLA_API void file_read_bytes(File& file, NBRAnimation* out_anim);
NIKOLA_API void file_read_bytes(File& file, NBRFont* out_font);
NIKOLA_API void file_read_bytes(File& file, NBRAudio* out_audio);

/// Read a `Transform` from `file` and save it into `transform`.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API void file_read_bytes(File& file, Transform* transform);

/// Read a `Camera` from `file` and save it into `camera`.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API void file_read_bytes(File& file, Camera* camera);

/// Read different light types from `file` and save it into `light`.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API void file_read_bytes(File& file, DirectionalLight* light);
NIKOLA_API void file_read_bytes(File& file, PointLight* light);
NIKOLA_API void file_read_bytes(File& file, SpotLight* light);

/// Read an `AudioSourceID` from `file` and save it into `source`.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API void file_read_bytes(File& file, AudioSourceID& source);

/// Read an `AudioListenerDesc` from `file` and save it into `listener`.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API void file_read_bytes(File& file, AudioListenerDesc* listener);

/// Read a `PhysicsBodyDesc` from `file` and save it into `body_desc`.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API void file_read_bytes(File& file, PhysicsBodyDesc* body_desc);

/// Read a `ColliderDesc` from `file` and save it into `collider_desc`.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API void file_read_bytes(File& file, ColliderDesc* collider_desc);

/// Read the WHOLE `file` as a `String` and save it into `str`.
///
/// @NOTE: This function will raise an error if `file` is not opened.
NIKOLA_API void file_read_string(File& file, String* str);

/// File functions
///---------------------------------------------------------------------------------------------------------------------

/// *** File system ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
