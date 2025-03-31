#include "mesh_loader.h"

#include "nikola/nikola_base.h"
#include "nikola/nikola_resources.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Private functions  

static void create_cube_mesh(const u16 group_id, NBRMesh* mesh) {
  f32 vertices[] = {
     // Position          Normal              UV coords
    
     // Back face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
     
     // Front face
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
     
     // Left face
    -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
    
     // Right face
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
     
     // Top face
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
    
     // Bottom face
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
  }; 

  u32 indices[] = {
    // Back face 
    0, 1, 2, 
    2, 3, 0, 

    // Front face 
    4, 5, 6, 
    6, 7, 4, 

    // Left face 
    10, 9, 8, 
    8, 11, 10, 

    // Right face 
    14, 13, 12, 
    12, 15, 14,

    // Top face 
    16, 17, 18, 
    18, 19, 16, 

    // Bottom face 
    20, 21, 22, 
    22, 23, 20, 
  };

  mesh->vertex_type = VERTEX_TYPE_PNUV; 
  
  mesh->vertices_count = sizeof(vertices) / sizeof(f32);
  mesh->vertices       = (f32*)memory_allocate(sizeof(vertices));
  memory_copy(mesh->vertices, vertices, sizeof(vertices));
  
  mesh->indices_count = sizeof(indices) / sizeof(u32);
  mesh->indices       = (u32*)memory_allocate(sizeof(indices));
  memory_copy(mesh->indices, indices, sizeof(indices));
}

/// Private functions  
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Mesh loader functions

void mesh_loader_load(const u16 group_id, NBRMesh* mesh, const MeshType type) {
  switch(type) {
    case MESH_TYPE_CUBE:
      create_cube_mesh(group_id, mesh);
      break;
    case MESH_TYPE_CIRCLE:
      // @TODO: Make a circle mesh
      break;
    case MESH_TYPE_CYLINDER:
      // @TODO: Make a cylinder mesh
      break;
  }
}

/// Mesh loader functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
