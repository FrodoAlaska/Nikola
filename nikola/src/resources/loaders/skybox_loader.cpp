#include "skybox_loader.hpp"

#include "nikola/nikola_base.h"
#include "nikola/nikola_resources.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Private functions
static void construct_cube_skybox(const u16 group_id, Skybox* sky) {
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

  sky->pipe_desc.vertex_buffer  = resources_get_buffer(resources_push_buffer(group_id, vert_desc));
  sky->pipe_desc.vertices_count = 36;
}
/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Skybox loader functions

void skybox_loader_load(const u16 group_id, Skybox* sky) {
  NIKOLA_ASSERT(sky, "Invalid Skybox passed into skybox loader function");
  
  // Default initialize the loader
  memory_zero(sky, sizeof(Skybox));
  sky->pipe_desc = {}; 

  // Vertex buffer init
  construct_cube_skybox(group_id, sky);
 
  // Layout init
  sky->pipe_desc.layout[0]    = {"TEX", GFX_LAYOUT_FLOAT3, 0};
  sky->pipe_desc.layout_count = 1;
  
  // Draw mode init 
  sky->pipe_desc.draw_mode = GFX_DRAW_MODE_TRIANGLE;
}

/// Skybox loader functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
