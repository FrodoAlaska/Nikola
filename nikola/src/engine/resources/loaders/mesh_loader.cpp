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

static void setup_mesh_pipe_desc(ResourceStorage* storage, MeshLoader* loader, VertexType type, void* vertices, sizei vertices_count, void* indices, sizei indices_count) {
  // Vertex buffer init 
  GfxBufferDesc vert_buff_desc = {
    .size  = get_vertex_type_size(type) * vertices_count,
    .data  = vertices,
    .type  = GFX_BUFFER_VERTEX, 
    .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  loader->vertex_buffer            = resource_storage_push(storage, vert_buff_desc);
  loader->pipe_desc.vertex_buffer  = resource_storage_get_buffer(storage, loader->vertex_buffer);
  loader->pipe_desc.vertices_count = vertices_count;  
  
  // Index buffer init
  GfxBufferDesc idx_buff_desc = {
    .size  = sizeof(u32) * indices_count,
    .data  = indices,
    .type  = GFX_BUFFER_INDEX, 
    .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  loader->index_buffer            = resource_storage_push(storage, idx_buff_desc);
  loader->pipe_desc.index_buffer  = resource_storage_get_buffer(storage, loader->index_buffer);
  loader->pipe_desc.indices_count = indices_count;  

  // Layout init
  get_layout_from_vertex_type(type, loader->pipe_desc);
  
  // Draw mode init
  loader->pipe_desc.draw_mode = GFX_DRAW_MODE_TRIANGLE;
}

static void create_cube_mesh(ResourceStorage* storage, MeshLoader* loader) {
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

  setup_mesh_pipe_desc(
    storage, 
    loader,
    VERTEX_TYPE_PNUV, 
    (void*)vertices.data(), 
    vertices.size(), 
    (void*)indices.data(),
    indices.size()
  );
}

/// Private functions  
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Mesh loader functions

void mesh_loader_load(ResourceStorage* storage, MeshLoader* loader, const DynamicArray<Vertex3D_PCUV>& vertices, const DynamicArray<u32>& indices) {
  setup_mesh_pipe_desc(
    storage, 
    loader, 
    VERTEX_TYPE_PCUV, 
    (void*)vertices.data(), 
    vertices.size(),
    (void*)indices.data(),
    indices.size()
  );
}

void mesh_loader_load(ResourceStorage* storage, MeshLoader* loader, const DynamicArray<Vertex3D_PNUV>& vertices, const DynamicArray<u32>& indices) {
  setup_mesh_pipe_desc(
    storage, 
    loader, 
    VERTEX_TYPE_PNUV, 
    (void*)vertices.data(), 
    vertices.size(),
    (void*)indices.data(),
    indices.size()
  );
}

void mesh_loader_load(ResourceStorage* storage, MeshLoader* loader, const DynamicArray<Vertex3D_PNCUV>& vertices, const DynamicArray<u32>& indices) {
  setup_mesh_pipe_desc(
    storage, 
    loader, 
    VERTEX_TYPE_PNCUV, 
    (void*)vertices.data(), 
    vertices.size(),
    (void*)indices.data(),
    indices.size()
  );
}

void mesh_loader_load(ResourceStorage* storage, MeshLoader* loader, const MeshType type) {
  switch(type) {
    case MESH_TYPE_CUBE:
      create_cube_mesh(storage, loader);
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
