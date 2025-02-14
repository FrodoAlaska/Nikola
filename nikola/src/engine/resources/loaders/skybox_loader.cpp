#include "skybox_loader.hpp"

#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Private functions
static void construct_cube_skybox(ResourceStorage* storage, Skybox* sky) {
  // Vertices
  float vertices[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
  };
  
  // Vertex buffer init 
  GfxBufferDesc vert_desc = {
    .data  = vertices, 
    .size  = sizeof(vertices), 
    .type  = GFX_BUFFER_VERTEX, 
    .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
  };

  ResourceID buffer_id          = resource_storage_push(storage, vert_desc);
  sky->vertex_buffer            = resource_storage_get_buffer(storage, buffer_id);
  sky->pipe_desc.vertex_buffer  = sky->vertex_buffer;
  sky->pipe_desc.vertices_count = 36;
}
/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Skybox loader functions

void skybox_loader_load(ResourceStorage* storage, Skybox* sky, const ResourceID& cubemap_id) {
  NIKOLA_ASSERT(storage, "Cannot load with an invalid ResourceStorage");
  NIKOLA_ASSERT(sky, "Invalid Skybox passed into skybox loader function");
  NIKOLA_ASSERT((cubemap_id != INVALID_RESOURCE), "Cannot load a skybox with an invalid cubemap ID");
  
  // Default initialize the loader
  memory_zero(sky, sizeof(Skybox));
  sky->pipe_desc = {}; 

  // Vertex buffer init
  construct_cube_skybox(storage, sky);
 
  // Layout init
  sky->pipe_desc.layout[0]    = {"TEX", GFX_LAYOUT_FLOAT3, 0};
  sky->pipe_desc.layout_count = 1;
  
  // Draw mode init 
  sky->pipe_desc.draw_mode    = GFX_DRAW_MODE_TRIANGLE;

  // Cubemap init
  sky->cubemap                   = resource_storage_get_cubemap(storage, cubemap_id); 
  sky->pipe_desc.cubemaps[0]     = sky->cubemap; 
  sky->pipe_desc.cubemaps_count  = 1; 
}

/// Skybox loader functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
