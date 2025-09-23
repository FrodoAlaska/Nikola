#include "nikola/nikola_base.h"
#include "nikola/nikola_file.h"
#include "nikola/nikola_math.h"
#include "nikola/nikola_render.h"
#include "nikola/nikola_audio.h"
#include "nikola/nikola_physics.h"
#include "nikola/nikola_resources.h"

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

static void read_nbr_header(File& file, u16* res_type) {
  u8 iden   = 0;
  i16 major = 0; 
  i16 minor = 0; 
  
  // Check for the validity of the identifier
  if(iden != NBR_VALID_IDENTIFIER) {
    NIKOLA_LOG_ERROR("Invalid identifier found in NBR file. Expected \'%i\' got \'%i\'", 
                      NBR_VALID_IDENTIFIER, iden);

    file_close(file);
    return;
  }  

  // Check for the validity of the versions
  if(((major != NBR_VALID_MAJOR_VERSION) || (minor != NBR_VALID_MINOR_VERSION))) {
    NIKOLA_LOG_ERROR("Invalid version found in NBR file!");

    file_close(file);
    return;
  }
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
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  file_write_bytes(file, str.c_str(), str.size());
}

void file_write_bytes(File& file, const NBRHeader& header) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");

  // Write the header information
  
  file_write_bytes(file, &header.identifier, sizeof(header.identifier));
  file_write_bytes(file, &header.major_version, sizeof(header.major_version));
  file_write_bytes(file, &header.minor_version, sizeof(header.major_version));
  file_write_bytes(file, &header.resource_type, sizeof(header.resource_type));
}

void file_write_bytes(File& file, const NBRTexture& texture) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");

  // Write the resource's information
  
  file_write_bytes(file, &texture.width, sizeof(texture.width));
  file_write_bytes(file, &texture.height, sizeof(texture.height));
  
  file_write_bytes(file, &texture.channels, sizeof(texture.channels));
  file_write_bytes(file, &texture.format, sizeof(texture.format));

  sizei pixel_size = 1; 
  if(texture.format == GFX_TEXTURE_FORMAT_RGBA16F) {
    pixel_size = 4;
  }

  sizei data_size = (texture.width * texture.height) * texture.channels * pixel_size;
  file_write_bytes(file, texture.pixels, data_size);
}

void file_write_bytes(File& file, const NBRCubemap& cubemap) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");

  // Write the resource's information
  
  file_write_bytes(file, &cubemap.width, sizeof(cubemap.width));
  file_write_bytes(file, &cubemap.height, sizeof(cubemap.height));

  file_write_bytes(file, &cubemap.channels, sizeof(cubemap.channels));
  file_write_bytes(file, &cubemap.format, sizeof(cubemap.format));

  file_write_bytes(file, &cubemap.faces_count, sizeof(cubemap.faces_count));
  
  sizei pixel_size = 1; 
  if(cubemap.format == GFX_TEXTURE_FORMAT_RGBA16F) {
    pixel_size = 4;
  }

  sizei data_size = (cubemap.width * cubemap.height) * cubemap.channels * pixel_size;
  for(sizei i = 0; i < cubemap.faces_count; i++) {
    file_write_bytes(file, cubemap.pixels[i], data_size);
  }
}

void file_write_bytes(File& file, const NBRShader& shader) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");

  // Write the compue shader

  file_write_bytes(file, &shader.compute_length, sizeof(u16));
  if(shader.compute_length > 0) {
    file_write_bytes(file, shader.compute_source, sizeof(i8) * shader.compute_length + 1);
    return;
  }

  // Write the vertex shader

  file_write_bytes(file, &shader.vertex_length, sizeof(u16));
  file_write_bytes(file, shader.vertex_source, sizeof(i8) * shader.vertex_length + 1);
  
  // Write the pixel shader
  
  file_write_bytes(file, &shader.pixel_length, sizeof(u16));
  file_write_bytes(file, shader.pixel_source, sizeof(i8) * shader.pixel_length + 1);
}

void file_write_bytes(File& file, const NBRMaterial& material) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  // Write the resource's information
  
  file_write_bytes(file, material.color, sizeof(f32) * 3); 
  file_write_bytes(file, &material.metallic, sizeof(f32)); 
  file_write_bytes(file, &material.roughness, sizeof(f32)); 
 
  file_write_bytes(file, &material.albedo_index, sizeof(i8)); 
  file_write_bytes(file, &material.metallic_index, sizeof(i8)); 
  file_write_bytes(file, &material.roughness_index, sizeof(i8)); 
  file_write_bytes(file, &material.normal_index, sizeof(i8)); 
}

void file_write_bytes(File& file, const NBRMesh& mesh) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  // Write the resource's information
  
  file_write_bytes(file, &mesh.vertex_component_bits, sizeof(u8));

  file_write_bytes(file, &mesh.vertices_count, sizeof(u32));
  file_write_bytes(file, mesh.vertices, sizeof(f32) * mesh.vertices_count);

  file_write_bytes(file, &mesh.indices_count, sizeof(u32));
  file_write_bytes(file, mesh.indices, sizeof(u32) * mesh.indices_count);

  file_write_bytes(file, &mesh.material_index, sizeof(u8));
}

void file_write_bytes(File& file, const NBRModel& model) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  // Write the resource's information
  
  // Save the meshes
  file_write_bytes(file, &model.meshes_count, sizeof(u16));
  for(sizei i = 0; i < model.meshes_count; i++) {
    file_write_bytes(file, model.meshes[i]);
  }

  // Save the materials
  file_write_bytes(file, &model.materials_count, sizeof(u8));
  for(sizei i = 0; i < model.materials_count; i++) {
    file_write_bytes(file, model.materials[i]);
  }

  // Save the textures
  file_write_bytes(file, &model.textures_count, sizeof(u8));
  for(sizei i = 0; i < model.textures_count; i++) {
    file_write_bytes(file, model.textures[i]);
  }
}

void file_write_bytes(File& file, const NBRAnimation& anim) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");

  // Write the resource's information

  // Write the joints

  file_write_bytes(file, &anim.joints_count, sizeof(anim.joints_count)); 
  for(sizei i = 0; i < anim.joints_count; i++) {
    NBRJoint* joint = &anim.joints[i];

    // Write the parent index 
    file_write_bytes(file, &joint->parent_index, sizeof(joint->parent_index)); 

    // Write the inverse bind matrix
    file_write_bytes(file, &joint->inverse_bind_pose, sizeof(joint->inverse_bind_pose)); 

    // Write the positions

    file_write_bytes(file, &joint->positions_count, sizeof(joint->positions_count)); 
    file_write_bytes(file, joint->position_samples, sizeof(f32) * joint->positions_count); 
    
    // Write the rotations

    file_write_bytes(file, &joint->rotations_count, sizeof(joint->rotations_count)); 
    file_write_bytes(file, joint->rotation_samples, sizeof(f32) * joint->rotations_count); 
    
    // Write the scales

    file_write_bytes(file, &joint->scales_count, sizeof(joint->scales_count)); 
    file_write_bytes(file, joint->scale_samples, sizeof(f32) * joint->scales_count); 
  } 

  // Write time info

  file_write_bytes(file, &anim.duration, sizeof(anim.duration)); 
  file_write_bytes(file, &anim.frame_rate, sizeof(anim.frame_rate)); 
}

void file_write_bytes(File& file, const NBRFont& font) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  // Write the resource's information
  
  // Write the glyphs 
  
  file_write_bytes(file, &font.glyphs_count, sizeof(font.glyphs_count));
  for(u32 i = 0; i < font.glyphs_count; i++) {
    file_write_bytes(file, &font.glyphs[i].unicode, sizeof(i8));
  
    file_write_bytes(file, &font.glyphs[i].width, sizeof(u16));
    file_write_bytes(file, &font.glyphs[i].height, sizeof(u16));

    file_write_bytes(file, &font.glyphs[i].left, sizeof(i16));
    file_write_bytes(file, &font.glyphs[i].right, sizeof(i16));
    file_write_bytes(file, &font.glyphs[i].top, sizeof(i16));
    file_write_bytes(file, &font.glyphs[i].bottom, sizeof(i16));

    file_write_bytes(file, &font.glyphs[i].offset_x, sizeof(i16));
    file_write_bytes(file, &font.glyphs[i].offset_y, sizeof(i16));
    
    file_write_bytes(file, &font.glyphs[i].advance_x, sizeof(i16));
    file_write_bytes(file, &font.glyphs[i].kern, sizeof(i16));
    file_write_bytes(file, &font.glyphs[i].left_bearing, sizeof(i16));
  
    sizei pixels_size = font.glyphs[i].width * font.glyphs[i].height;
    file_write_bytes(file, font.glyphs[i].pixels, pixels_size);
  }

  // Write font information
  
  file_write_bytes(file, &font.ascent, sizeof(font.ascent));
  file_write_bytes(file, &font.descent, sizeof(font.descent));
  file_write_bytes(file, &font.line_gap, sizeof(font.line_gap));
}

void file_write_bytes(File& file, const NBRAudio& audio) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  // Write the resource's information
  
  file_write_bytes(file, &audio.format, sizeof(audio.format));
  file_write_bytes(file, &audio.sample_rate, sizeof(audio.sample_rate));
  file_write_bytes(file, &audio.channels, sizeof(audio.channels));
  file_write_bytes(file, &audio.size, sizeof(audio.size));
  file_write_bytes(file, audio.samples, audio.size);
}

void file_write_bytes(File& file, const Transform& transform) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
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
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
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
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
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
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  f32 data[] = {
    light.position.x,  
    light.position.y,  
    light.position.z,  
    
    light.color.r,  
    light.color.g,  
    light.color.b,  

    light.radius,
    light.fall_off,
  };

  file_write_bytes(file, data, sizeof(data));
}

void file_write_bytes(File& file, const SpotLight& light) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  f32 data[] = {
    light.position.x,  
    light.position.y,  
    light.position.z,  
    
    light.direction.x,  
    light.direction.y,  
    light.direction.z,  
    
    light.color.r,  
    light.color.g,  
    light.color.b,  

    light.radius,
    light.outer_radius,
  };

  file_write_bytes(file, data, sizeof(data));
}

void file_write_bytes(File& file, const FrameData& frame) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");

  // Camera
  file_write_bytes(file, frame.camera);

  // Ambient
  file_write_bytes(file, &frame.ambient[0], sizeof(Vec3)); 

  // Directional light
  file_write_bytes(file, frame.dir_light);

  // Point lights

  u32 points_count = (u32)frame.point_lights.size();
  file_write_bytes(file, &points_count, sizeof(u32));
  
  for(auto& light : frame.point_lights) {
    file_write_bytes(file, light);
  }

  // Spot lights

  u32 spots_count = (u32)frame.spot_lights.size();
  file_write_bytes(file, &spots_count, sizeof(u32));
  
  for(auto& light : frame.spot_lights) {
    file_write_bytes(file, light);
  }
}

void file_write_bytes(File& file, const AudioSourceID& source) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
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
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
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

void file_write_bytes(File& file, const PhysicsBodyID& body) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
 
  Vec3 position = physics_body_get_position(body);
  Quat rotation = physics_body_get_rotation(body);

  f32 restitution    = physics_body_get_restitution(body);
  f32 friction       = physics_body_get_friction(body);
  f32 gravity_factor = physics_body_get_gravity_factor(body);
  
  PhysicsBodyType type     = physics_body_get_type(body);
  PhysicsObjectLayer layer = physics_body_get_layer(body);

  f32 f_data[] = {
    position.x,   
    position.y,   
    position.z,   
    
    rotation.w,   
    rotation.x,   
    rotation.y,   
    rotation.z,   

    restitution, 
    friction, 
    gravity_factor,
  };

  u16 u_data[] = {
    (u16)type, 
    (u16)layer,
  };

  file_write_bytes(file, f_data, sizeof(f_data));
  file_write_bytes(file, u_data, sizeof(u_data));
}

void file_write_bytes(File& file, const ColliderID& collider) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  
  // @TODO (Physics)
  // Vec3 offset  = collider_get_local_transform(collider).position;
  // Vec3 extents = collider_get_extents(collider);
  //
  // f32 friction    = collider_get_friction(collider);
  // f32 restitution = collider_get_restitution(collider);
  // f32 density     = collider_get_density(collider);
  //
  // i32 sensor = (i32)collider_get_sensor(collider);
  //
  // f32 data[] = {
  //   offset.x,
  //   offset.y,
  //   offset.z,
  //   
  //   extents.x,
  //   extents.y,
  //   extents.z,
  //
  //   friction, 
  //   restitution, 
  //   density,
  // };
  // 
  // file_write_bytes(file, data, sizeof(data));
  // file_write_bytes(file, &sensor, sizeof(sensor));
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
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
 
  // @TODO (File): We should NOT assign arbitrary sizes to the read string
  char c_str[1024];

  file_read_bytes(file, c_str, sizeof(c_str));
  *str = String(c_str);
}

void file_read_bytes(File& file, NBRHeader* out_header) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  NIKOLA_ASSERT(out_header, "Invalid NBRHeader type given to file_read_bytes");
  
  file_read_bytes(file, &out_header->identifier, sizeof(out_header->identifier));

  file_read_bytes(file, &out_header->major_version, sizeof(out_header->major_version));
  file_read_bytes(file, &out_header->minor_version, sizeof(out_header->minor_version));

  file_read_bytes(file, &out_header->resource_type, sizeof(out_header->resource_type));
}

void file_read_bytes(File& file, NBRTexture* out_texture) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  NIKOLA_ASSERT(out_texture, "Invalid NBRTexture type given to file_read_bytes");
  
  file_read_bytes(file, &out_texture->width, sizeof(out_texture->width));  
  file_read_bytes(file, &out_texture->height, sizeof(out_texture->height));  
  
  file_read_bytes(file, &out_texture->channels, sizeof(out_texture->channels));  
  file_read_bytes(file, &out_texture->format, sizeof(out_texture->format));  
  
  sizei pixel_size = 1; 
  if(out_texture->format == GFX_TEXTURE_FORMAT_RGBA16F) {
    pixel_size = 4;
  }

  sizei data_size = (out_texture->width * out_texture->height) * out_texture->channels * pixel_size;
  out_texture->pixels = memory_allocate(data_size);
  file_read_bytes(file, out_texture->pixels, data_size);
}

void file_read_bytes(File& file, NBRCubemap* out_cubemap) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  NIKOLA_ASSERT(out_cubemap, "Invalid NBRCubemap type given to file_read_bytes");
  
  file_read_bytes(file, &out_cubemap->width, sizeof(out_cubemap->width));  
  file_read_bytes(file, &out_cubemap->height, sizeof(out_cubemap->height));  

  file_read_bytes(file, &out_cubemap->channels, sizeof(out_cubemap->channels));  
  file_read_bytes(file, &out_cubemap->format, sizeof(out_cubemap->format));  

  file_read_bytes(file, &out_cubemap->faces_count, sizeof(out_cubemap->faces_count));  
  
  sizei pixel_size = 1; 
  if(out_cubemap->format == GFX_TEXTURE_FORMAT_RGBA16F) {
    pixel_size = 4;
  }

  sizei data_size = (out_cubemap->width * out_cubemap->height) * out_cubemap->channels * pixel_size;
  for(sizei i = 0; i < out_cubemap->faces_count; i++) {
    out_cubemap->pixels[i] = (u8*)memory_allocate(data_size);
    file_read_bytes(file, out_cubemap->pixels[i], data_size);
  }
}

void file_read_bytes(File& file, NBRShader* out_shader) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  NIKOLA_ASSERT(out_shader, "Invalid NBRShader type given to file_read_bytes");

  // Read the compute shader 

  file_read_bytes(file, &out_shader->compute_length, sizeof(u16));
  if(out_shader->compute_length > 0) {
    out_shader->compute_source = (i8*)memory_allocate(out_shader->compute_length + 1); 
    file_read_bytes(file, out_shader->compute_source, out_shader->compute_length + 1);
    
    out_shader->compute_source[out_shader->compute_length] = '\0';

    out_shader->vertex_source = nullptr;
    out_shader->pixel_source  = nullptr;

    return;
  }
 
  // Read the vertex shader

  file_read_bytes(file, &out_shader->vertex_length, sizeof(u16));
  out_shader->vertex_source = (i8*)memory_allocate(out_shader->vertex_length + 1); 

  file_read_bytes(file, out_shader->vertex_source, out_shader->vertex_length + 1);
  out_shader->vertex_source[out_shader->vertex_length + 1] = 0;

  // Read the pixel shader

  file_read_bytes(file, &out_shader->pixel_length, sizeof(u16));
  out_shader->pixel_source = (i8*)memory_allocate(out_shader->pixel_length + 1); 

  file_read_bytes(file, out_shader->pixel_source, out_shader->pixel_length + 1);
  out_shader->pixel_source[out_shader->pixel_length + 1] = 0;
}

void file_read_bytes(File& file, NBRMaterial* out_material) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  NIKOLA_ASSERT(out_material, "Invalid NBRMaterial type given to file_read_bytes");
  
  file_read_bytes(file, out_material->color, sizeof(f32) * 3); 
  file_write_bytes(file, &out_material->metallic, sizeof(f32)); 
  file_write_bytes(file, &out_material->roughness, sizeof(f32)); 
 
  file_read_bytes(file, &out_material->albedo_index, sizeof(i8)); 
  file_read_bytes(file, &out_material->metallic_index, sizeof(i8)); 
  file_read_bytes(file, &out_material->roughness_index, sizeof(i8)); 
  file_read_bytes(file, &out_material->normal_index, sizeof(i8)); 
}

void file_read_bytes(File& file, NBRMesh* out_mesh) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  NIKOLA_ASSERT(out_mesh, "Invalid NBRMesh type given to file_read_bytes");
  
  file_read_bytes(file, &out_mesh->vertex_component_bits, sizeof(u8));

  file_read_bytes(file, &out_mesh->vertices_count, sizeof(u32));
  out_mesh->vertices = (f32*)memory_allocate(sizeof(f32) * out_mesh->vertices_count); 
  file_read_bytes(file, out_mesh->vertices, sizeof(f32) * out_mesh->vertices_count);

  file_read_bytes(file, &out_mesh->indices_count, sizeof(u32));
  out_mesh->indices = (u32*)memory_allocate(sizeof(u32) * out_mesh->indices_count); 
  file_read_bytes(file, out_mesh->indices, sizeof(u32) * out_mesh->indices_count);

  file_read_bytes(file, &out_mesh->material_index, sizeof(u8));
}

void file_read_bytes(File& file, NBRModel* out_model) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  NIKOLA_ASSERT(out_model, "Invalid NBRModel type given to file_read_bytes");
  
  // Load the meshes
  
  file_read_bytes(file, &out_model->meshes_count, sizeof(u16));
  out_model->meshes = (NBRMesh*)memory_allocate(sizeof(NBRMesh) * out_model->meshes_count); 
  for(sizei i = 0; i < out_model->meshes_count; i++) {
    file_read_bytes(file, &out_model->meshes[i]);
  }

  // Load the materials 
  
  file_read_bytes(file, &out_model->materials_count, sizeof(u8));
  out_model->materials = (NBRMaterial*)memory_allocate(sizeof(NBRMaterial) * out_model->materials_count); 
  for(sizei i = 0; i < out_model->materials_count; i++) {
    file_read_bytes(file, &out_model->materials[i]); 
  }

  // Load the textures 
  
  file_read_bytes(file, &out_model->textures_count, sizeof(u8));
  out_model->textures = (NBRTexture*)memory_allocate(sizeof(NBRTexture) * out_model->textures_count); 
  for(sizei i = 0; i < out_model->textures_count; i++) {
    file_read_bytes(file, &out_model->textures[i]);
  }
}

void file_read_bytes(File& file, NBRAnimation* out_anim) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  NIKOLA_ASSERT(out_anim, "Invalid NBRAnimation type given to file_read_bytes");

  // Read the joints

  file_read_bytes(file, &out_anim->joints_count, sizeof(out_anim->joints_count)); 
  out_anim->joints = (NBRJoint*)memory_allocate(sizeof(NBRJoint) * out_anim->joints_count);

  for(sizei i = 0; i < out_anim->joints_count; i++) {
    NBRJoint* joint = &out_anim->joints[i];

    // Read the parent index 
    file_read_bytes(file, &joint->parent_index, sizeof(joint->parent_index)); 

    // Read the inverse bind matrix
    file_read_bytes(file, &joint->inverse_bind_pose, sizeof(joint->inverse_bind_pose)); 

    // Read the positions

    file_read_bytes(file, &joint->positions_count, sizeof(joint->positions_count)); 
    joint->position_samples = (f32*)memory_allocate(sizeof(f32) * joint->positions_count); 
    
    file_read_bytes(file, joint->position_samples, sizeof(f32) * joint->positions_count); 
    
    // Read the rotations

    file_read_bytes(file, &joint->rotations_count, sizeof(joint->rotations_count)); 
    joint->rotation_samples = (f32*)memory_allocate(sizeof(f32) * joint->rotations_count); 
    
    file_read_bytes(file, joint->rotation_samples, sizeof(f32) * joint->rotations_count); 
    
    // Read the scales

    file_read_bytes(file, &joint->scales_count, sizeof(joint->scales_count)); 
    joint->scale_samples = (f32*)memory_allocate(sizeof(f32) * joint->scales_count); 
    
    file_read_bytes(file, joint->scale_samples, sizeof(f32) * joint->scales_count); 
  } 

  // Read time info

  file_read_bytes(file, &out_anim->duration, sizeof(out_anim->duration)); 
  file_read_bytes(file, &out_anim->frame_rate, sizeof(out_anim->frame_rate)); 
}

void file_read_bytes(File& file, NBRFont* out_font) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  NIKOLA_ASSERT(out_font, "Invalid NBRFont type given to file_read_bytes");
  
  // Load the glyphs 
  
  file_read_bytes(file, &out_font->glyphs_count, sizeof(out_font->glyphs_count));
  out_font->glyphs = (NBRGlyph*)memory_allocate(sizeof(NBRGlyph) * out_font->glyphs_count);

  for(u32 i = 0; i < out_font->glyphs_count; i++) {
    file_read_bytes(file, &out_font->glyphs[i].unicode, sizeof(i8));
  
    file_read_bytes(file, &out_font->glyphs[i].width, sizeof(u16));
    file_read_bytes(file, &out_font->glyphs[i].height, sizeof(u16));

    file_read_bytes(file, &out_font->glyphs[i].left, sizeof(u16));
    file_read_bytes(file, &out_font->glyphs[i].right, sizeof(u16));
    file_read_bytes(file, &out_font->glyphs[i].top, sizeof(u16));
    file_read_bytes(file, &out_font->glyphs[i].bottom, sizeof(u16));

    file_read_bytes(file, &out_font->glyphs[i].offset_x, sizeof(i16));
    file_read_bytes(file, &out_font->glyphs[i].offset_y, sizeof(i16));
    
    file_read_bytes(file, &out_font->glyphs[i].advance_x, sizeof(i16));
    file_read_bytes(file, &out_font->glyphs[i].kern, sizeof(i16));
    file_read_bytes(file, &out_font->glyphs[i].left_bearing, sizeof(i16));
  
    sizei pixels_size      = out_font->glyphs[i].width * out_font->glyphs[i].height;
    out_font->glyphs[i].pixels = (u8*)memory_allocate(pixels_size); 

    file_read_bytes(file, out_font->glyphs[i].pixels, pixels_size);
  }

  // Load font information
  
  file_read_bytes(file, &out_font->ascent, sizeof(out_font->ascent));
  file_read_bytes(file, &out_font->descent, sizeof(out_font->descent));
  file_read_bytes(file, &out_font->line_gap, sizeof(out_font->line_gap));
}

void file_read_bytes(File& file, NBRAudio* out_audio) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
  NIKOLA_ASSERT(out_audio, "Invalid NBRAudio type given to file_read_bytes");
  
  file_read_bytes(file, &out_audio->format, sizeof(out_audio->format));
  file_read_bytes(file, &out_audio->sample_rate, sizeof(out_audio->sample_rate));
  file_read_bytes(file, &out_audio->channels, sizeof(out_audio->channels));
  file_read_bytes(file, &out_audio->size, sizeof(out_audio->size));
  
  out_audio->samples = (i16*)memory_allocate(out_audio->size); 
  file_read_bytes(file, out_audio->samples, out_audio->size);
}

void file_read_bytes(File& file, Transform* transform) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
 
  f32 raw_data[10];
  file_read_bytes(file, raw_data, sizeof(raw_data));

  transform_translate(*transform, Vec3(raw_data[0], raw_data[1], raw_data[2]));
  transform_scale(*transform, Vec3(raw_data[3], raw_data[4], raw_data[5]));
  transform_rotate(*transform, Quat(raw_data[9], raw_data[6], raw_data[7], raw_data[8]));
}

void file_read_bytes(File& file, Camera* camera) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
 
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
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
 
  f32 raw_data[6];
  file_read_bytes(file, raw_data, sizeof(raw_data));

  light->direction = Vec3(raw_data[0], raw_data[1], raw_data[2]);
  light->color     = Vec3(raw_data[3], raw_data[4], raw_data[5]);
}

void file_read_bytes(File& file, PointLight* light) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
 
  f32 raw_data[8];
  file_read_bytes(file, raw_data, sizeof(raw_data));

  light->position = Vec3(raw_data[0], raw_data[1], raw_data[2]);
  light->color    = Vec3(raw_data[3], raw_data[4], raw_data[5]);
  light->radius   = raw_data[6];
  light->fall_off = raw_data[7];
}

void file_read_bytes(File& file, SpotLight* light) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
 
  f32 raw_data[11];
  file_read_bytes(file, raw_data, sizeof(raw_data));
  
  light->position.x = raw_data[0];  
  light->position.y = raw_data[1];  
  light->position.z = raw_data[2];  

  light->direction.x = raw_data[3];  
  light->direction.y = raw_data[4];  
  light->direction.z = raw_data[5];  

  light->color.r = raw_data[6];  
  light->color.g = raw_data[7];  
  light->color.b = raw_data[8];  

  light->radius       = raw_data[9];
  light->outer_radius = raw_data[10];
}

void file_read_bytes(File& file, FrameData* frame) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");

  // Camera
  file_read_bytes(file, &frame->camera);

  // Ambient
  file_read_bytes(file, &frame->ambient[0], sizeof(Vec3)); 

  // Directional light
  file_read_bytes(file, &frame->dir_light);

  // Point lights

  u32 points_count = 0;
  file_read_bytes(file, &points_count, sizeof(u32));
  frame->point_lights.resize(points_count); 

  for(auto& light : frame->point_lights) {
    file_read_bytes(file, &light);
  }

  // Spot lights

  u32 spots_count = 0;
  file_read_bytes(file, &spots_count, sizeof(u32));
  frame->spot_lights.resize(spots_count); 

  for(auto& light : frame->spot_lights) {
    file_read_bytes(file, &light);
  }
}

void file_read_bytes(File& file, AudioSourceID& source) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
 
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
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");
 
  f32 raw_data[7];
  file_read_bytes(file, raw_data, sizeof(raw_data));

  listener->volume    = raw_data[0];
  listener->position  = Vec3(raw_data[1], raw_data[2], raw_data[3]),
  listener->velocity  = Vec3(raw_data[4], raw_data[5], raw_data[6]),
  
  audio_listener_set_volume(listener->volume);
  audio_listener_set_position(listener->position);
  audio_listener_set_velocity(listener->velocity);
}

void file_read_bytes(File& file, PhysicsBodyDesc* body_desc) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");

  // Read the data from the file

  f32 f_data[10];
  file_read_bytes(file, f_data, sizeof(f_data));

  u16 u_data[2];
  file_read_bytes(file, u_data, sizeof(u_data));

  // Make sense of the data
  
  Vec3 position = Vec3(f_data[0], f_data[1], f_data[2]);
  Quat rotation = Quat(f_data[3], f_data[4], f_data[5], f_data[6]);

  f32 restitution    = f_data[7];
  f32 friction       = f_data[8];
  f32 gravity_factor = f_data[9];

  PhysicsBodyType type     = (PhysicsBodyType)u_data[0];
  PhysicsObjectLayer layer = (PhysicsObjectLayer)u_data[1];

  // Fill in the body desc
  
  body_desc->position = position;
  body_desc->rotation = rotation;
  
  body_desc->restitution    = restitution;
  body_desc->friction       = friction;
  body_desc->gravity_factor = gravity_factor;

  body_desc->type   = type;
  body_desc->layers = layer;
}

void file_read_bytes(File& file, ColliderDesc* collider_desc) {
  NIKOLA_ASSERT(file.is_open(), "Cannot perform an operation on an unopened file");

  // @TODO (Physics)
  // f32 data[9];
  // file_read_bytes(file, data, sizeof(data));
  //
  // i32 sensor; 
  // file_read_bytes(file, &sensor, sizeof(sensor));
  //
  // collider_desc->position    = Vec3(data[0], data[1], data[2]);
  // collider_desc->extents     = Vec3(data[3], data[4], data[5]);
  // collider_desc->friction    = data[6];
  // collider_desc->restitution = data[7];
  // collider_desc->density     = data[8];
  // collider_desc->is_sensor   = (bool)sensor;
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
