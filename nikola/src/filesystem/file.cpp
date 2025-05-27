#include "nikola/nikola_base.h"
#include "nikola/nikola_file.h"
#include "nikola/nikola_math.h"
#include "nikola/nikola_render.h"
#include "nikola/nikola_audio.h"

#include <sstream>
#include <filesystem>

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Private functions
static std::ios::openmode get_mode(const i32 mode) {
  std::ios::openmode cpp_mode = (std::ios::openmode)0;

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

const sizei file_write_bytes(File& file, const void* buff, const sizei buff_size) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  file.write((char*)buff, buff_size);
  return buff_size;
}

void file_write_bytes(File& file, const String& str) {
  file_write_bytes(file, str.c_str(), str.size());
}

void file_write_bytes(File& file, const Transform& transform) {
  f32 raw_data[] = {
    transform.position.x,  
    transform.position.y,  
    transform.position.z,  
    
    transform.scale.x,  
    transform.scale.y,  
    transform.scale.z,  
    
    transform.rotation.x,  
    transform.rotation.y,  
    transform.rotation.z,  
    transform.rotation.w,  
  };

  file_write_bytes(file, raw_data, sizeof(raw_data));  
}

void file_write_bytes(File& file, const Camera& camera) {
  f32 data[] = {
    camera.yaw, 
    camera.pitch, 
    camera.zoom, 

    camera.near, 
    camera.far, 
    camera.sensitivity, 
    camera.exposure, 

    camera.position.x,
    camera.position.y,
    camera.position.z,
  };

  file_write_bytes(file, data, sizeof(data));
}

void file_write_bytes(File& file, const DirectionalLight& light) {
  f32 data[] = {
    light.direction.x,  
    light.direction.y,  
    light.direction.z,  
    
    light.color.r,  
    light.color.g,  
    light.color.b,  
  };

  file_write_bytes(file, data, sizeof(data));
}

void file_write_bytes(File& file, const PointLight& light) {
  f32 data[] = {
    light.position.x,  
    light.position.y,  
    light.position.z,  
    
    light.color.r,  
    light.color.g,  
    light.color.b,  

    light.linear, 
    light.quadratic,
  };

  file_write_bytes(file, data, sizeof(data));
}

void file_write_bytes(File& file, const AudioSourceID& source) {
  AudioSourceDesc desc = audio_source_get_desc(source);

  f32 data[] = {
    desc.volume, 
    desc.pitch, 

    desc.position.x, 
    desc.position.y, 
    desc.position.z,
    
    desc.velocity.x, 
    desc.velocity.y, 
    desc.velocity.z,
    
    desc.direction.x, 
    desc.direction.y, 
    desc.direction.z,

    desc.is_looping,
  };

  file_write_bytes(file, data, sizeof(data));
}

void file_write_bytes(File& file, const AudioListenerDesc& listener_desc) {
  f32 data[] = {
    listener_desc.volume, 

    listener_desc.position.x, 
    listener_desc.position.y, 
    listener_desc.position.z,
    
    listener_desc.velocity.x, 
    listener_desc.velocity.y, 
    listener_desc.velocity.z,
  };

  file_write_bytes(file, data, sizeof(data));
}

void file_write_string(File& file, const String& string) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");

  file << string;
}

const sizei file_read_bytes(File& file, void* out_buff, const sizei size) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  file.read((char*)out_buff, size);
  return size;
}

void file_read_bytes(File& file, String* str) {
  // @TODO (File): We should NOT assign arbitrary sizes to the read string
  char c_str[1024];

  file_read_bytes(file, c_str, sizeof(c_str));
  *str = String(c_str);
}

void file_read_bytes(File& file, Transform* transform) {
  f32 raw_data[10];
  file_read_bytes(file, raw_data, sizeof(raw_data));

  transform_translate(*transform, Vec3(raw_data[0], raw_data[1], raw_data[2]));
  transform_scale(*transform, Vec3(raw_data[3], raw_data[4], raw_data[5]));
  transform_rotate(*transform, Quat(raw_data[9], raw_data[6], raw_data[7], raw_data[8]));
}

void file_read_bytes(File& file, Camera* camera) {
  f32 raw_data[10]; 
  file_read_bytes(file, raw_data, sizeof(raw_data));

  camera->yaw         = raw_data[0]; 
  camera->pitch       = raw_data[1]; 
  camera->zoom        = raw_data[2]; 
  camera->near        = raw_data[3]; 
  camera->far         = raw_data[4]; 
  camera->sensitivity = raw_data[5]; 
  camera->exposure    = raw_data[6];
  camera->position    = Vec3(raw_data[7], raw_data[8], raw_data[9]);
}

void file_read_bytes(File& file, DirectionalLight* light) {
  f32 raw_data[6];
  file_read_bytes(file, raw_data, sizeof(raw_data));

  light->direction = Vec3(raw_data[0], raw_data[1], raw_data[2]);
  light->color     = Vec3(raw_data[3], raw_data[4], raw_data[5]);
}

void file_read_bytes(File& file, PointLight* light) {
  f32 raw_data[8];
  file_read_bytes(file, raw_data, sizeof(raw_data));

  light->position = Vec3(raw_data[0], raw_data[1], raw_data[2]);
  light->color    = Vec3(raw_data[3], raw_data[4], raw_data[5]);
  
  light->linear    = raw_data[6];
  light->quadratic = raw_data[7];
}

void file_read_bytes(File& file, AudioSourceID& source) {
  f32 raw_data[12];
  file_read_bytes(file, raw_data, sizeof(raw_data));

  AudioSourceDesc desc {
    .volume = raw_data[0],
    .pitch  = raw_data[1],

    .position  = Vec3(raw_data[2], raw_data[3], raw_data[4]),
    .velocity  = Vec3(raw_data[5], raw_data[6], raw_data[7]),
    .direction = Vec3(raw_data[8], raw_data[9], raw_data[10]),
    
    .is_looping = (bool)raw_data[11],
  };

  // Apply the new data;
  audio_source_set_volume(source, desc.volume);
  audio_source_set_pitch(source, desc.pitch);

  audio_source_set_position(source,desc.position);
  audio_source_set_velocity(source, desc.velocity);
  audio_source_set_direction(source, desc.direction);
  
  audio_source_set_looping(source, desc.is_looping);
}

void file_read_bytes(File& file, AudioListenerDesc* listener) {
  f32 raw_data[7];
  file_read_bytes(file, raw_data, sizeof(raw_data));

  listener->volume    = raw_data[0];
  listener->position  = Vec3(raw_data[1], raw_data[2], raw_data[3]),
  listener->velocity  = Vec3(raw_data[4], raw_data[5], raw_data[6]),
  
  audio_listener_set_volume(listener->volume);
  audio_listener_set_position(listener->position);
  audio_listener_set_velocity(listener->velocity);
}

void file_read_string(File& file, String* str) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");

  std::stringstream ss;
  ss << file.rdbuf();

  *str = ss.str();
}


/// File functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
