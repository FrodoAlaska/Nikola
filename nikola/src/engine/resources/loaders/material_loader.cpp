#include "material_loader.hpp"

#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Material loader functions

void material_loader_load(const u16 group_id,  
                          Material* mat, 
                          const ResourceID& shader_id) {
  NIKOLA_ASSERT(mat, "Invalid Material passed to material loader function");

  const RendererDefaults render_defaults = renderer_get_defaults();

  // Set default values for the material
  mat->ambient_color  = Vec3(1.0f); 
  mat->diffuse_color  = Vec3(1.0f); 
  mat->specular_color = Vec3(1.0f); 
  mat->model_matrix   = Mat4(1.0f);
  mat->shininess      = 1.0f;
  mat->screen_size    = Vec2(1366.0f, 720.0f); // @TODO (Material): Change this to be more configurable. 

  // Default textures init
  mat->diffuse_map  = render_defaults.texture;
  mat->specular_map = render_defaults.texture;
 
  // Cannot go one with an invalid shader
  if(shader_id.group == RESOURCE_GROUP_INVALID) {
    return;
  }

  // Shader init 
  mat->shader = resources_get_shader(shader_id);
 
  // Set a default matrices buffer 
  GfxBuffer* matrices_buffer                           = render_defaults.matrices_buffer;
  mat->uniform_buffers[MATERIAL_MATRICES_BUFFER_INDEX] = matrices_buffer;
  gfx_shader_attach_uniform(mat->shader, GFX_SHADER_VERTEX, matrices_buffer, MATERIAL_MATRICES_BUFFER_INDEX);
  
  // Reserve some space for the map
  mat->uniform_locations.reserve(MATERIAL_UNIFORMS_MAX);

  // All the current valid uniform names
  const i8* uniform_names[MATERIAL_UNIFORMS_MAX] = {
    MATERIAL_UNIFORM_AMBIENT_COLOR, 
    MATERIAL_UNIFORM_DIFFUSE_COLOR,
    MATERIAL_UNIFORM_SPECULAR_COLOR,
    MATERIAL_UNIFORM_SHININESS,
    MATERIAL_UNIFORM_SCREEN_SIZE,
    MATERIAL_UNIFORM_MODEL_MATRIX,
  };
  
  // Adding only the valid uniforms in the shader
  for(sizei i = 0; i < MATERIAL_UNIFORMS_MAX; i++) {
    i32 location = gfx_shader_uniform_lookup(mat->shader, uniform_names[i]);
    
    // The uniform does not exist
    if(location == -1) {
      continue;
    } 

    // Otherwise, add it to the cached list of locations
    mat->uniform_locations[uniform_names[i]] = location;
  }
}

/// Material loader functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
