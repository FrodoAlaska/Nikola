#include "nikola/nikola_resources.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_gfx.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void check_and_send_uniform(Material* mat, const i8* name, GfxLayoutType type, const void* data) {
  GfxShader* shader = resources_get_shader(mat->shader); 
  NIKOLA_ASSERT(shader, "Invalid Material's shader");

  // Send the uniform (only if it is valid)
  if(mat->uniform_locations.find(name) != mat->uniform_locations.end()) {
    gfx_shader_upload_uniform(shader, mat->uniform_locations[name], type, data);
    return;
  }
 
  // Get the new uniform location
  i32 location = gfx_shader_uniform_lookup(shader, name);
  
  // The uniform just does not exist in the shader at all 
  if(location == -1) {
    NIKOLA_LOG_WARN("Could not find uniform \'%s\' in material", name);
    return;
  }
  
  // Uniform does not exist. Cache it instead.
  mat->uniform_locations[name] = location; 
  gfx_shader_upload_uniform(shader, location, type, data);
  NIKOLA_LOG_DEBUG("Cache uniform \'%s\' with location \'%i\' in material...", name, location);
}

static void send_preset_uniform(Material* mat, const i8* name, GfxLayoutType type, const void* data) {
  GfxShader* shader = resources_get_shader(mat->shader); 
  NIKOLA_ASSERT(shader, "Invalid Material's shader");

  // Send the preset uniform only if it exists in the cache 
  if(mat->uniform_locations.find(name) != mat->uniform_locations.end()) {
    gfx_shader_upload_uniform(shader, mat->uniform_locations[name], type, data);
    return;
  }
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Material functions

void material_set_uniform(ResourceID& mat_id, const i8* uniform_name, const i32 value) {
  Material* mat = resources_get_material(mat_id); 
  check_and_send_uniform(mat, uniform_name, GFX_LAYOUT_INT1, &value);
}

void material_set_uniform(ResourceID& mat_id, const i8* uniform_name, const f32 value) {
  Material* mat = resources_get_material(mat_id); 
  check_and_send_uniform(mat, uniform_name, GFX_LAYOUT_FLOAT1, &value);
}

void material_set_uniform(ResourceID& mat_id, const i8* uniform_name, const Vec2& value) {
  Material* mat = resources_get_material(mat_id); 
  check_and_send_uniform(mat, uniform_name, GFX_LAYOUT_FLOAT2, &value[0]);
}

void material_set_uniform(ResourceID& mat_id, const i8* uniform_name, const Vec3& value) {
  Material* mat = resources_get_material(mat_id); 
  check_and_send_uniform(mat, uniform_name, GFX_LAYOUT_FLOAT3, &value[0]);
}

void material_set_uniform(ResourceID& mat_id, const i8* uniform_name, const Vec4& value) {
  Material* mat = resources_get_material(mat_id); 
  check_and_send_uniform(mat, uniform_name, GFX_LAYOUT_FLOAT4, &value[0]);
}

void material_set_uniform(ResourceID& mat_id, const i8* uniform_name, const Mat4& value) {
  Material* mat = resources_get_material(mat_id); 
  check_and_send_uniform(mat, uniform_name, GFX_LAYOUT_MAT4, (void*)mat4_raw_data(value));
}

void material_set_uniform_buffer(ResourceID& mat_id, const sizei index, const ResourceID& buffer_id) {
  Material* mat     = resources_get_material(mat_id); 
  GfxShader* shader = resources_get_shader(mat->shader);
  GfxBuffer* buffer = resources_get_buffer(buffer_id);

  NIKOLA_ASSERT(shader, "Invalid Material's shader");
  NIKOLA_ASSERT(buffer, "Invalid buffer given to material_set_uniform_buffer");
  NIKOLA_ASSERT(((index >= MATERIAL_MATRICES_BUFFER_INDEX) && (index <= MATERIAL_LIGHTING_BUFFER_INDEX)), "Invalid index passed as uniform buffer index of a material");

  mat->uniform_buffers[index] = buffer_id;
  gfx_shader_attach_uniform(shader, GFX_SHADER_VERTEX, buffer, index);
}

void material_set_texture(ResourceID& mat_id, const MaterialTextureType type, const ResourceID& texture_id) {
  NIKOLA_ASSERT(RESOURCE_IS_VALID(mat_id), "Invalid Material passed");
  NIKOLA_ASSERT(RESOURCE_IS_VALID(texture_id), "Invalid texture ID passed to material");
  
  Material* mat = resources_get_material(mat_id); 

  switch(type) {
    case MATERIAL_TEXTURE_DIFFUSE:
      mat->diffuse_map = texture_id;
      break;
    case MATERIAL_TEXTURE_SPECULAR:
      mat->specular_map = texture_id;
      break;
  }
}

void material_set_shader(ResourceID& mat_id, const ResourceID& shader_id) {
  NIKOLA_ASSERT((mat_id.group != RESOURCE_GROUP_INVALID), "Invalid Material passed");
  NIKOLA_ASSERT((shader_id.group != RESOURCE_GROUP_INVALID), "Invalid shader ID passed to material");
  
  Material* mat = resources_get_material(mat_id); 
  mat->shader   = shader_id;
}

void material_use(ResourceID& mat_id) {
  Material* mat = resources_get_material(mat_id);

  // Use the shader 
  gfx_shader_use(resources_get_shader(mat->shader));

  // Send all of the available uniforms
  send_preset_uniform(mat, MATERIAL_UNIFORM_AMBIENT_COLOR, GFX_LAYOUT_FLOAT3, &mat->ambient_color[0]);
  send_preset_uniform(mat, MATERIAL_UNIFORM_DIFFUSE_COLOR, GFX_LAYOUT_FLOAT3, &mat->diffuse_color[0]);
  send_preset_uniform(mat, MATERIAL_UNIFORM_SPECULAR_COLOR, GFX_LAYOUT_FLOAT3, &mat->specular_color[0]);
  send_preset_uniform(mat, MATERIAL_UNIFORM_SHININESS, GFX_LAYOUT_FLOAT1, &mat->shininess);
  send_preset_uniform(mat, MATERIAL_UNIFORM_SCREEN_SIZE, GFX_LAYOUT_FLOAT2, &mat->screen_size[0]);
  send_preset_uniform(mat, MATERIAL_UNIFORM_MODEL_MATRIX, GFX_LAYOUT_MAT4, mat4_raw_data(mat->model_matrix));

  // Use the diffuse texture (if they are valid)
  if(RESOURCE_IS_VALID(mat->diffuse_map)) {
    GfxTexture* diffuse = resources_get_texture(mat->diffuse_map);
    gfx_texture_use(&diffuse, 1);
  }
 
  // @FIX (Material)
  // Use the specular texture (if they are valid)
  // if(RESOURCE_IS_VALID(mat->specular_map)) {
  //   GfxTexture* specular = resources_get_texture(mat->specular_map);
  //   gfx_texture_use(&specular, 1);
  // }
}

/// Material functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
