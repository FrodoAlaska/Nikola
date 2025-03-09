#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void check_and_send_uniform(Material* mat, const i8* name, GfxLayoutType type, const void* data) {
  // Send the uniform (only if it is valid)
  if(mat->uniform_locations.find(name) != mat->uniform_locations.end()) {
    gfx_shader_upload_uniform(mat->shader, mat->uniform_locations[name], type, data);
    return;
  }
 
  // Get the new uniform location
  i32 location = gfx_shader_uniform_lookup(mat->shader, name);
  
  // The uniform just does not exist in the shader at all 
  if(location == -1) {
    NIKOLA_LOG_WARN("Could not find uniform \'%s\' in material", name);
    return;
  }
  
  // Uniform does not exist. Cache it instead.
  mat->uniform_locations[name] = location; 
  gfx_shader_upload_uniform(mat->shader, location, type, data);
  NIKOLA_LOG_DEBUG("Cache uniform \'%s\' with location \'%i\' in material...", name, location);
}

static void send_preset_uniform(Material* mat, const i8* name, GfxLayoutType type, const void* data) {
  // Send the preset uniform only if it exists in the cache 
  if(mat->uniform_locations.find(name) != mat->uniform_locations.end()) {
    gfx_shader_upload_uniform(mat->shader, mat->uniform_locations[name], type, data);
    return;
  }
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Material functions

void material_set_uniform(ResourceID& mat_id, const i8* uniform_name, const i32 value) {
  Material* mat = resource_storage_get_material(mat_id); 
  NIKOLA_ASSERT(mat->shader, "Invalid Material's shader");
  
  check_and_send_uniform(mat, uniform_name, GFX_LAYOUT_INT1, &value);
}

void material_set_uniform(ResourceID& mat_id, const i8* uniform_name, const f32 value) {
  Material* mat = resource_storage_get_material(mat_id); 
  NIKOLA_ASSERT(mat->shader, "Invalid Material's shader");
  
  check_and_send_uniform(mat, uniform_name, GFX_LAYOUT_FLOAT1, &value);
}

void material_set_uniform(ResourceID& mat_id, const i8* uniform_name, const Vec2& value) {
  Material* mat = resource_storage_get_material(mat_id); 
  NIKOLA_ASSERT(mat->shader, "Invalid Material's shader");
  
  check_and_send_uniform(mat, uniform_name, GFX_LAYOUT_FLOAT2, &value[0]);
}

void material_set_uniform(ResourceID& mat_id, const i8* uniform_name, const Vec3& value) {
  Material* mat = resource_storage_get_material(mat_id); 
  NIKOLA_ASSERT(mat->shader, "Invalid Material's shader");
  
  check_and_send_uniform(mat, uniform_name, GFX_LAYOUT_FLOAT3, &value[0]);
}

void material_set_uniform(ResourceID& mat_id, const i8* uniform_name, const Vec4& value) {
  Material* mat = resource_storage_get_material(mat_id); 
  NIKOLA_ASSERT(mat->shader, "Invalid Material's shader");
  
  check_and_send_uniform(mat, uniform_name, GFX_LAYOUT_FLOAT4, &value[0]);
}

void material_set_uniform(ResourceID& mat_id, const i8* uniform_name, const Mat4& value) {
  Material* mat = resource_storage_get_material(mat_id); 
  NIKOLA_ASSERT(mat->shader, "Invalid Material's shader");
  
  check_and_send_uniform(mat, uniform_name, GFX_LAYOUT_MAT4, (void*)mat4_raw_data(value));
}

void material_set_uniform_buffer(ResourceID& mat_id, const sizei index, GfxBuffer* buffer) {
  Material* mat = resource_storage_get_material(mat_id); 
  NIKOLA_ASSERT(mat->shader, "Invalid Material's shader");
  NIKOLA_ASSERT(buffer, "Invalid Material's shader");
  NIKOLA_ASSERT(((index >= MATERIAL_MATRICES_BUFFER_INDEX) && (index <= MATERIAL_LIGHTING_BUFFER_INDEX)), "Invalid index passed as uniform buffer index of a material");

  mat->uniform_buffers[index] = (GfxBuffer*)buffer;
  gfx_shader_attach_uniform(mat->shader, GFX_SHADER_VERTEX, mat->uniform_buffers[index], index);
}

void material_use(ResourceID& mat_id) {
  Material* mat = resource_storage_get_material(mat_id); 
  NIKOLA_ASSERT(mat->shader, "Invalid Material's shader");

  // Send all of the available uniforms
  send_preset_uniform(mat, MATERIAL_UNIFORM_AMBIENT_COLOR, GFX_LAYOUT_FLOAT3, &mat->ambient_color[0]);
  send_preset_uniform(mat, MATERIAL_UNIFORM_DIFFUSE_COLOR, GFX_LAYOUT_FLOAT3, &mat->diffuse_color[0]);
  send_preset_uniform(mat, MATERIAL_UNIFORM_SPECULAR_COLOR, GFX_LAYOUT_FLOAT3, &mat->specular_color[0]);
  send_preset_uniform(mat, MATERIAL_UNIFORM_SHININESS, GFX_LAYOUT_FLOAT1, &mat->shininess);
  send_preset_uniform(mat, MATERIAL_UNIFORM_SCREEN_SIZE, GFX_LAYOUT_FLOAT2, &mat->screen_size[0]);
  send_preset_uniform(mat, MATERIAL_UNIFORM_MODEL_MATRIX, GFX_LAYOUT_MAT4, mat4_raw_data(mat->model_matrix));
}

/// Material functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
