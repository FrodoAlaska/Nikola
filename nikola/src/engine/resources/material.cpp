#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Material functions

void material_set_diffuse(Material* mat, const ResourceID& diffuse_map) {
  NIKOLA_ASSERT(mat, "Invalid Material passed");

  if(mat->diffuse_map == INVALID_RESOURCE) {
    NIKOLA_LOG_WARN("Cannot set diffuse map of an invalid resource in material");
    return;
  }
}

void material_set_specualr(Material* mat, const ResourceID& specular_map) {
  NIKOLA_ASSERT(mat, "Invalid Material passed");

  if(mat->specular_map == INVALID_RESOURCE) {
    NIKOLA_LOG_WARN("Cannot set specular map of an invalid resource in material");
    return;
  }
}

void material_set_color(Material* mat, const Vec4& color) {
  NIKOLA_ASSERT(mat, "Invalid Material passed");
  
  GfxShader* shader = resource_storage_get_shader(mat->resource_ref, mat->shader);

  // @TODO: There has to be a better way
  i32 loc = gfx_glsl_get_uniform_location(shader, "u_color");
  gfx_glsl_upload_uniform(shader, loc, GFX_LAYOUT_FLOAT4, &color[0]);
}

void material_set_transform(Material* mat, const Transform& transform) {
  NIKOLA_ASSERT(mat, "Invalid Material passed");
  
  GfxShader* shader = resource_storage_get_shader(mat->resource_ref, mat->shader);

  // @TODO: There has to be a better way
  i32 loc = gfx_glsl_get_uniform_location(shader, "u_model");
  gfx_glsl_upload_uniform(shader, loc, GFX_LAYOUT_MAT4, mat4_raw_data(transform.transform));
}

void material_set_matrcies_buffer(Material* mat, const Mat4& view_projection) {
  NIKOLA_ASSERT(mat, "Invalid Material passed");

  // Make sure the id is valid otherwise that is a huge problem
  ResourceID mat_buff_id = mat->uniform_buffers[MATERIAL_MATRICES_BUFFER_INDEX];
  NIKOLA_ASSERT((mat_buff_id != INVALID_RESOURCE), "Invalid matrices uniform buffer");
  
  // Update the uniform buffer
  GfxBuffer* mat_buff = resource_storage_get_buffer(mat->resource_ref, mat_buff_id);
  gfx_buffer_update(mat_buff, 0, sizeof(Mat4), mat4_raw_data(view_projection));
}

/// Material functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
