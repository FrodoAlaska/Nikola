#include "material_loader.hpp"

#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Material loader functions

void material_loader_load(ResourceStorage* storage, 
                          Material* mat, 
                          const ResourceID& diffuse_id, 
                          const ResourceID& specular_id, 
                          const ResourceID& shader_id) {
  NIKOLA_ASSERT(storage, "Cannot load with an invalid ResourceStorage");
  NIKOLA_ASSERT(mat, "Invalid Material passed to material loader function");
  NIKOLA_ASSERT(diffuse_id.storage, "Cannot load a material with an invalid diffuse texture ID");

  const RendererDefaults render_defaults = renderer_get_defaults();

  // Set default values for the material
  mat->ambient_color  = Vec3(1.0f); 
  mat->diffuse_color  = Vec3(1.0f); 
  mat->specular_color = Vec3(1.0f); 
  mat->model_matrix   = Mat4(1.0f);
  mat->shininess      = 1.0f;
  mat->screen_size    = Vec2(1366.0f, 720.0f); // @TODO (Material): Change this to be more configurable. 
  
  // Diffuse texture init
  mat->diffuse_map = resource_storage_get_texture(diffuse_id);

  // Specular texture init (if it is available)
  mat->specular_map = render_defaults.texture; 
  if(specular_id.storage != nullptr) {
    mat->specular_map = resource_storage_get_texture(specular_id);
  } 

  // The values below can only be set if the shader is active
  if(shader_id.storage == nullptr) {
    return;
  } 
    
  // Shader init 
  mat->shader = resource_storage_get_shader(shader_id);
 
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
