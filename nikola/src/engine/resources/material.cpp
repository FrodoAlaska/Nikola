#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Material functions

void material_set_diffuse_map(Material* mat, const ResourceID& diffuse_map) {
  NIKOLA_ASSERT(mat, "Invalid Material passed");

  if(mat->diffuse_map == INVALID_RESOURCE) {
    NIKOLA_LOG_WARN("Cannot set diffuse map of an invalid resource in material");
    return;
  }
}

void material_set_specular_map(Material* mat, const ResourceID& specular_map) {
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

void material_set_model_matrix(Material* mat, const Mat4& model) {
  NIKOLA_ASSERT(mat, "Invalid Material passed");
  
  GfxShader* shader = resource_storage_get_shader(mat->resource_ref, mat->shader);

  // @TODO: There has to be a better way
  i32 loc = gfx_glsl_get_uniform_location(shader, "u_model");
  gfx_glsl_upload_uniform(shader, loc, GFX_LAYOUT_MAT4, mat4_raw_data(model));
}

/// Material functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
