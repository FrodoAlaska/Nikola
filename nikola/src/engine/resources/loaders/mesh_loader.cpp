#include "mesh_loader.hpp"

#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Private functions  

static void create_cube_mesh(const u16 group_id, Mesh* mesh) {
  DynamicArray<Vertex3D_PNUV> vertices = {
    // Position                 Normal                   UV coords
    
    // Back face
    {Vec3(-0.5f, -0.5f, -0.5f), Vec3(0.0f, 0.0f, -1.0f), Vec2(0.0f, 0.0f)},
    {Vec3( 0.5f, -0.5f, -0.5f), Vec3(0.0f, 0.0f, -1.0f), Vec2(1.0f, 0.0f)},
    {Vec3( 0.5f,  0.5f, -0.5f), Vec3(0.0f, 0.0f, -1.0f), Vec2(1.0f, 1.0f)},
    {Vec3(-0.5f,  0.5f, -0.5f), Vec3(0.0f, 0.0f, -1.0f), Vec2(0.0f, 1.0f)},

    // Front face
    {Vec3(-0.5f, -0.5f, 0.5f),  Vec3(0.0f, 0.0f, 1.0f),  Vec2(0.0f, 0.0f)},
    {Vec3( 0.5f, -0.5f, 0.5f),  Vec3(0.0f, 0.0f, 1.0f),  Vec2(1.0f, 0.0f)},
    {Vec3( 0.5f,  0.5f, 0.5f),  Vec3(0.0f, 0.0f, 1.0f),  Vec2(1.0f, 1.0f)},
    {Vec3(-0.5f,  0.5f, 0.5f),  Vec3(0.0f, 0.0f, 1.0f),  Vec2(0.0f, 1.0f)},

    // Left face
    {Vec3(-0.5f,  0.5f,  0.5f), Vec3(-1.0f, 0.0f, 0.0f), Vec2(1.0f, 0.0f)},
    {Vec3(-0.5f,  0.5f, -0.5f), Vec3(-1.0f, 0.0f, 0.0f), Vec2(1.0f, 1.0f)},
    {Vec3(-0.5f, -0.5f, -0.5f), Vec3(-1.0f, 0.0f, 0.0f), Vec2(0.0f, 1.0f)},
    {Vec3(-0.5f, -0.5f,  0.5f), Vec3(-1.0f, 0.0f, 0.0f), Vec2(0.0f, 0.0f)},

    // Right face
    {Vec3(0.5f,  0.5f,  0.5f),  Vec3(1.0f, 0.0f, 0.0f),  Vec2(1.0f, 0.0f)},
    {Vec3(0.5f,  0.5f, -0.5f),  Vec3(1.0f, 0.0f, 0.0f),  Vec2(1.0f, 1.0f)},
    {Vec3(0.5f, -0.5f, -0.5f),  Vec3(1.0f, 0.0f, 0.0f),  Vec2(0.0f, 1.0f)},
    {Vec3(0.5f, -0.5f,  0.5f),  Vec3(1.0f, 0.0f, 0.0f),  Vec2(0.0f, 0.0f)},
  
    // Top face
    {Vec3(-0.5f, -0.5f, -0.5f), Vec3(0.0f, -1.0f, 0.0f), Vec2(0.0f, 1.0f)},
    {Vec3( 0.5f, -0.5f, -0.5f), Vec3(0.0f, -1.0f, 0.0f), Vec2(1.0f, 1.0f)},
    {Vec3( 0.5f, -0.5f,  0.5f), Vec3(0.0f, -1.0f, 0.0f), Vec2(1.0f, 0.0f)},
    {Vec3(-0.5f, -0.5f,  0.5f), Vec3(0.0f, -1.0f, 0.0f), Vec2(0.0f, 0.0f)},

    // Bottom face
    {Vec3(-0.5f, 0.5f, -0.5f),  Vec3(0.0f, 1.0f, 0.0f),  Vec2(0.0f, 1.0f)},
    {Vec3( 0.5f, 0.5f, -0.5f),  Vec3(0.0f, 1.0f, 0.0f),  Vec2(1.0f, 1.0f)},
    {Vec3( 0.5f, 0.5f,  0.5f),  Vec3(0.0f, 1.0f, 0.0f),  Vec2(1.0f, 0.0f)},
    {Vec3(-0.5f, 0.5f,  0.5f),  Vec3(0.0f, 1.0f, 0.0f),  Vec2(0.0f, 0.0f)},
  }; 

  DynamicArray<u32> indices = {
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

  NBRMesh nbr_mesh = {
    .vertex_type = VERTEX_TYPE_PNUV, 
    
    .vertices_count = (u32)vertices.size(),
    .vertices       = (f32*)vertices.data(),
    
    .indices_count = (u32)indices.size(),
    .indices       = (u32*)indices.data(),
  };

  nbr_import_mesh(&nbr_mesh, group_id, mesh);
}

/// Private functions  
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Mesh loader functions

void mesh_loader_load(const u16 group_id, Mesh* mesh, const MeshType type) {
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
