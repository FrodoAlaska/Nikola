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
  NIKOLA_ASSERT((diffuse_id != INVALID_RESOURCE), "Cannot load a material with an invalid diffuse texture ID");

  // Diffuse texture init
  mat->diffuse_map = resource_storage_get_texture(storage, diffuse_id);

  // Specular texture init
  if(specular_id != INVALID_RESOURCE) {
    mat->specular_map = resource_storage_get_texture(storage, specular_id);
  } 

  // Set default values for the material
  mat->ambient_color  = Vec3(1.0f); 
  mat->diffuse_color  = Vec3(1.0f); 
  mat->specular_color = Vec3(1.0f); 
  mat->model_matrix   = Mat4(1.0f);

  // The values below can only be set if the shader is active
  if(shader_id == INVALID_RESOURCE) {
    return;
  } 
    
  // Shader init 
  mat->shader = resource_storage_get_shader(storage, shader_id);
 
  // Set a default matrices buffer 
  // @TODO(Renderer/Reosurce): HAS to be a better way to do this...
  GfxBuffer* matrix_buffer = (GfxBuffer*)renderer_default_matrices_buffer();
  material_set_uniform_buffer(mat, MATERIAL_MATRICES_BUFFER_INDEX, matrix_buffer);
  
  // Reserve some space for the map
  mat->uniform_locations.reserve(MATERIAL_UNIFORMS_MAX);

  // All the current valid uniform names
  const i8* uniform_names[MATERIAL_UNIFORMS_MAX] = {
    MATERIAL_UNIFORM_AMBIENT_COLOR, 
    MATERIAL_UNIFORM_DIFFUSE_COLOR,
    MATERIAL_UNIFORM_SPECULAR_COLOR,
    MATERIAL_UNIFORM_MODEL_MATRIX,
  };
  
  // Adding only the valid uniforms in the shader
  for(sizei i = 0; i < MATERIAL_UNIFORMS_MAX; i++) {
    i32 location = gfx_glsl_get_uniform_location(mat->shader, uniform_names[i]);
    
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
