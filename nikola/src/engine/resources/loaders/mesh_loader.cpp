#include "mesh_loader.hpp"

#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Private functions  

static void get_layout_from_vertex_type(VertexType type, GfxPipelineDesc& desc) {
  switch(type) {
    case VERTEX_TYPE_PCUV: 
      desc.layout[0]    = {"POSITION", GFX_LAYOUT_FLOAT3, 0};
      desc.layout[1]    = {"COLOR", GFX_LAYOUT_FLOAT4, 0};
      desc.layout[2]    = {"TEX", GFX_LAYOUT_FLOAT2, 0};
      desc.layout_count = 3;
      break;
    case VERTEX_TYPE_PNUV: 
      desc.layout[0]    = {"POSITION", GFX_LAYOUT_FLOAT3, 0};
      desc.layout[1]    = {"NORMAL", GFX_LAYOUT_FLOAT3, 0};
      desc.layout[2]    = {"TEX", GFX_LAYOUT_FLOAT2, 0};
      desc.layout_count = 3;
      break;
    case VERTEX_TYPE_PNCUV: 
      desc.layout[0]    = {"POSITION", GFX_LAYOUT_FLOAT3, 0};
      desc.layout[1]    = {"NORMAL", GFX_LAYOUT_FLOAT3, 0};
      desc.layout[2]    = {"COLOR", GFX_LAYOUT_FLOAT4, 0};
      desc.layout[3]    = {"TEX", GFX_LAYOUT_FLOAT2, 0};
      desc.layout_count = 4;
      break;
  }
}

static sizei get_vertex_type_size(VertexType type) {
  switch(type) {
    case VERTEX_TYPE_PCUV: 
      return sizeof(Vertex3D_PCUV);
      break;
    case VERTEX_TYPE_PNUV: 
      return sizeof(Vertex3D_PNUV);
      break;
    case VERTEX_TYPE_PNCUV: 
      return sizeof(Vertex3D_PNCUV);
      break;
  }
}

static void create_cube_mesh(ResourceStorage* storage, Mesh* mesh) {
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

  GfxBufferDesc vert_buff = {
    .data  = (void*)vertices.data(),
    .size  = sizeof(Vertex3D_PNUV) * vertices.size(),
    .type  = GFX_BUFFER_VERTEX,
    .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  ResourceID vert_id = resource_storage_push_buffer(storage, vert_buff);
  
  GfxBufferDesc index_buff = {
    .data  = (void*)indices.data(),
    .size  = sizeof(u32) * indices.size(),
    .type  = GFX_BUFFER_INDEX,
    .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  ResourceID index_id = resource_storage_push_buffer(storage, index_buff);

  mesh_loader_load(storage, mesh, vert_id, VERTEX_TYPE_PNUV, index_id, indices.size());
}

/// Private functions  
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Mesh loader functions

void mesh_loader_load(ResourceStorage* storage, 
                      Mesh* mesh, 
                      const ResourceID& vertex_buffer_id, 
                      const VertexType vertex_type, 
                      const ResourceID& index_buffer_id, 
                      const sizei indices_count) {
  NIKOLA_ASSERT(storage, "Cannot load with an invalid ResourceStorage");
  NIKOLA_ASSERT(mesh, "Invalid Mesh passed to mesh loader function");
  NIKOLA_ASSERT((vertex_buffer_id != INVALID_RESOURCE), "Cannot load a mesh with an invalid vertex buffer ID");
  
  // Default initialize the loader
  memory_zero(mesh, sizeof(Mesh));
  mesh->pipe_desc = {}; 

  // Vertex buffer init 
  mesh->vertex_buffer            = resource_storage_get_buffer(storage, vertex_buffer_id);
  mesh->pipe_desc.vertex_buffer  = mesh->vertex_buffer;

  // Calculate the number of vertices in the vertex buffer
  sizei vert_buff_size           = gfx_buffer_get_desc(mesh->pipe_desc.vertex_buffer).size;
  mesh->pipe_desc.vertices_count = (get_vertex_type_size(vertex_type) / vert_buff_size);  
  
  // Index buffer init (only if available)
  if(index_buffer_id != INVALID_RESOURCE) {
    mesh->index_buffer            = resource_storage_get_buffer(storage, index_buffer_id);
    mesh->pipe_desc.index_buffer  = mesh->index_buffer;
    mesh->pipe_desc.indices_count = indices_count;  
  }

  // Layout init
  get_layout_from_vertex_type(vertex_type, mesh->pipe_desc);
  
  // Draw mode init
  mesh->pipe_desc.draw_mode = GFX_DRAW_MODE_TRIANGLE;
}

void mesh_loader_load(ResourceStorage* storage, Mesh* mesh, const MeshType type) {
  switch(type) {
    case MESH_TYPE_CUBE:
      create_cube_mesh(storage, mesh);
      break;
    case MESH_TYPE_CIRCLE:
      // @TODO
      break;
    case MESH_TYPE_CYLINDER:
      // @TODO
      break;
  }
}

/// Mesh loader functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
