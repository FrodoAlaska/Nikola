#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Material functions

void material_set_color(Material* mat, const Vec4& color) {
  NIKOLA_ASSERT(mat, "Invalid Material passed");
  NIKOLA_ASSERT(mat->shader, "Invalid Material's shader");
  
  // @TODO: There has to be a better way
  i32 loc = gfx_glsl_get_uniform_location(mat->shader, "u_color");
  gfx_glsl_upload_uniform(mat->shader, loc, GFX_LAYOUT_FLOAT4, &color[0]);
}

void material_set_transform(Material* mat, const Transform& transform) {
  NIKOLA_ASSERT(mat, "Invalid Material passed");
  NIKOLA_ASSERT(mat->shader, "Invalid Material's shader");

  // @TODO: There has to be a better way
  i32 loc = gfx_glsl_get_uniform_location(mat->shader, "u_model");
  gfx_glsl_upload_uniform(mat->shader, loc, GFX_LAYOUT_MAT4, mat4_raw_data(transform.transform));
}

void material_set_matrices_buffer(Material* mat, const Mat4& view_projection) {
  NIKOLA_ASSERT(mat, "Invalid Material passed");
  NIKOLA_ASSERT(mat->shader, "Invalid Material's matrices buffer");
  
  // Update the uniform buffer
  GfxBuffer* mat_buff = mat->uniform_buffers[MATERIAL_MATRICES_BUFFER_INDEX];
  gfx_buffer_update(mat_buff, 0, sizeof(Mat4), mat4_raw_data(view_projection));
}

/// Material functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
