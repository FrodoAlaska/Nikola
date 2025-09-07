#include "nikola/nikola_resources.h"
#include "nikola/nikola_base.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Private functions  

static void generate_normals(Vertex3D* vertices, const u32 indices_count, const u32* indices) {
  for(u32 i = 0; i < indices_count; i += 3) {
    u32 idx0 = indices[i + 0];
    u32 idx1 = indices[i + 1];
    u32 idx2 = indices[i + 2];
    
    Vec3 edge1 = vertices[idx1].position - vertices[idx0].position;
    Vec3 edge2 = vertices[idx2].position - vertices[idx0].position;

    Vec3 normal = vec3_normalize(vec3_cross(edge2, edge1));

    vertices[idx0].normal = normal;
    vertices[idx1].normal = normal;
    vertices[idx2].normal = normal;
  }
}

static void generate_tangents(Vertex3D* vertices, const u32 indices_count, const u32* indices) {
  // Credits to Travis Vroman for this code. 
  // https://github.com/travisvroman/kohi/blob/main/kohi.core/src/math/geometry.c

  // In order to retrieve the tangents for each face we need: 
  //    1 - Calculate the differences between the points to get the edges. 
  //    2 - Calculate the differences between the texture coordinates which we call the "delta".
  //    3 - Using the edges and deltas, calculate a "d" value which will be used to calculate the tangents.
  //    4 - Use the "d" value, edges, and the "V" coordinate to calculate the tangents. 
  //    5 (Optional) - Use the "d" value, edges, and "U" coordinate to calculate the bi-tangents. However, we calculate the bi-tangents in the shader.

  for(u32 i = 0; i < indices_count; i += 3) {
    u32 idx0 = indices[i + 0];
    u32 idx1 = indices[i + 1];
    u32 idx2 = indices[i + 2];

    // Retrieve the edges
    
    Vec3 edge1 = vertices[idx1].position - vertices[idx0].position;
    Vec3 edge2 = vertices[idx2].position - vertices[idx0].position;

    // Retrieve the deltas 
    
    f32 deltaU1 = vertices[idx1].texture_coords.x - vertices[idx0].texture_coords.x; 
    f32 deltaV1 = vertices[idx1].texture_coords.y - vertices[idx0].texture_coords.y; 
  
    f32 deltaU2 = vertices[idx2].texture_coords.x - vertices[idx0].texture_coords.x; 
    f32 deltaV2 = vertices[idx2].texture_coords.y - vertices[idx0].texture_coords.y; 

    // Calculate the tangents
    
    f32 dividend = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);
  
    Vec3 tan; 
    tan.x = dividend * (deltaV2 * edge1.x - deltaV1 * edge2.x);
    tan.y = dividend * (deltaV2 * edge1.y - deltaV1 * edge2.y);
    tan.z = dividend * (deltaV2 * edge1.z - deltaV1 * edge2.z);

    tan = vec3_normalize(tan);

    vertices[idx0].tangent = tan;
    vertices[idx1].tangent = tan;
    vertices[idx2].tangent = tan;
  }
}

static void create_cube_geo(const ResourceGroupID& group_id, GfxPipelineDesc* pipe_desc) {
  // Indices init

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

  // Vertices init
  
  Vertex3D vertices[24];
  
  // Position, Color

  // Back face
  
  vertices[0].position = Vec3(-1.0f, -1.0f, -1.0f);
  vertices[1].position = Vec3( 1.0f, -1.0f, -1.0f);
  vertices[2].position = Vec3( 1.0f,  1.0f, -1.0f);
  vertices[3].position = Vec3(-1.0f,  1.0f, -1.0f);
  
  vertices[0].texture_coords = Vec2(0.0f, 0.0f);
  vertices[1].texture_coords = Vec2(1.0f, 0.0f);
  vertices[2].texture_coords = Vec2(1.0f, 1.0f);
  vertices[3].texture_coords = Vec2(0.0f, 1.0f);

  // Front face
   
  vertices[4].position = Vec3(-1.0f, -1.0f,  1.0f);
  vertices[5].position = Vec3( 1.0f, -1.0f,  1.0f);
  vertices[6].position = Vec3( 1.0f,  1.0f,  1.0f);
  vertices[7].position = Vec3(-1.0f,  1.0f,  1.0f);
  
  vertices[4].texture_coords = Vec2(0.0f, 0.0f);
  vertices[5].texture_coords = Vec2(1.0f, 0.0f);
  vertices[6].texture_coords = Vec2(1.0f, 1.0f);
  vertices[7].texture_coords = Vec2(0.0f, 1.0f);
  
  // Left face
  
  vertices[8].position  = Vec3(-1.0f,  1.0f,  1.0f);
  vertices[9].position  = Vec3(-1.0f,  1.0f, -1.0f);
  vertices[10].position = Vec3(-1.0f, -1.0f, -1.0f);
  vertices[11].position = Vec3(-1.0f, -1.0f,  1.0f);
  
  vertices[8].texture_coords  = Vec2(1.0f, 0.0f);
  vertices[9].texture_coords  = Vec2(1.0f, 1.0f);
  vertices[10].texture_coords = Vec2(0.0f, 1.0f);
  vertices[11].texture_coords = Vec2(0.0f, 0.0f);
 
  // Right face
 
  vertices[12].position = Vec3(1.0f,  1.0f,  1.0f);
  vertices[13].position = Vec3(1.0f,  1.0f, -1.0f);
  vertices[14].position = Vec3(1.0f, -1.0f, -1.0f);
  vertices[15].position = Vec3(1.0f, -1.0f,  1.0f);
  
  vertices[12].texture_coords = Vec2(1.0f, 0.0f);
  vertices[13].texture_coords = Vec2(1.0f, 1.0f);
  vertices[14].texture_coords = Vec2(0.0f, 1.0f);
  vertices[15].texture_coords = Vec2(0.0f, 0.0f);

  // Top face
 
  vertices[16].position = Vec3(-1.0f, -1.0f, -1.0f);
  vertices[17].position = Vec3( 1.0f, -1.0f, -1.0f);
  vertices[18].position = Vec3( 1.0f, -1.0f,  1.0f);
  vertices[19].position = Vec3(-1.0f, -1.0f,  1.0f);
  
  vertices[16].texture_coords = Vec2(0.0f, 1.0f);
  vertices[17].texture_coords = Vec2(1.0f, 1.0f);
  vertices[18].texture_coords = Vec2(1.0f, 0.0f);
  vertices[19].texture_coords = Vec2(0.0f, 0.0f);
 
  // Bottom face
 
  vertices[20].position = Vec3(-1.0f,  1.0f, -1.0f);
  vertices[21].position = Vec3( 1.0f,  1.0f, -1.0f);
  vertices[22].position = Vec3( 1.0f,  1.0f,  1.0f);
  vertices[23].position = Vec3(-1.0f,  1.0f,  1.0f);
  
  vertices[20].texture_coords = Vec2(0.0f, 1.0f);
  vertices[21].texture_coords = Vec2(1.0f, 1.0f);
  vertices[22].texture_coords = Vec2(1.0f, 0.0f);
  vertices[23].texture_coords = Vec2(0.0f, 0.0f);

  // Normals, Tangents, UV Coords

  generate_normals(vertices, 36, indices);
  generate_tangents(vertices, 36, indices);

  // Vertex buffer init
  
  GfxBufferDesc buff_desc = {
    .data  = (void*)vertices,
    .size  = sizeof(vertices),
    .type  = GFX_BUFFER_VERTEX, 
    .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  pipe_desc->vertex_buffer  = resources_get_buffer(resources_push_buffer(group_id, buff_desc));
  pipe_desc->vertices_count = 24;  

  // Index buffer init
  
  buff_desc = {
    .data  = (void*)indices,
    .size  = sizeof(indices),
    .type  = GFX_BUFFER_INDEX, 
    .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  pipe_desc->index_buffer  = resources_get_buffer(resources_push_buffer(group_id, buff_desc));
  pipe_desc->indices_count = 36;  

  // Layout init
  
  pipe_desc->layouts[0].attributes[0]    = GFX_LAYOUT_FLOAT3;
  pipe_desc->layouts[0].attributes[1]    = GFX_LAYOUT_FLOAT3;
  pipe_desc->layouts[0].attributes[2]    = GFX_LAYOUT_FLOAT3;
  pipe_desc->layouts[0].attributes[3]    = GFX_LAYOUT_FLOAT4;
  pipe_desc->layouts[0].attributes[4]    = GFX_LAYOUT_FLOAT4;
  pipe_desc->layouts[0].attributes[5]    = GFX_LAYOUT_FLOAT2;
  pipe_desc->layouts[0].attributes_count = 6;

  // Draw mode init
  pipe_desc->draw_mode = GFX_DRAW_MODE_TRIANGLE;
}

static void create_skybox_geo(const ResourceGroupID& group_id, GfxPipelineDesc* pipe_desc) {
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
  pipe_desc->vertex_buffer  = resources_get_buffer(resources_push_buffer(group_id, vert_desc));
  pipe_desc->vertices_count = 36;

  // Layout init
  
  pipe_desc->layouts[0].attributes[0]    = GFX_LAYOUT_FLOAT3;
  pipe_desc->layouts[0].attributes_count = 1;

  // Draw mode init
  pipe_desc->draw_mode = GFX_DRAW_MODE_TRIANGLE;
}

static void create_billboard_geo(const ResourceGroupID& group_id, GfxPipelineDesc* pipe_desc) {
  // Vertex buffer init
  
  f32 vertices[] {
    // Position          Normal             Texture coords
    -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
     1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
  };
  
  GfxBufferDesc buff_desc = {
    .data  = (void*)vertices,
    .size  = sizeof(vertices),
    .type  = GFX_BUFFER_VERTEX, 
    .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  pipe_desc->vertex_buffer  = resources_get_buffer(resources_push_buffer(group_id, buff_desc));
  pipe_desc->vertices_count = 4;  

  // Index buffer init
 
  u32 indices[6] = {
    0, 1, 2, 
    2, 3, 0,
  };

  buff_desc = {
    .data  = (void*)indices,
    .size  = sizeof(indices),
    .type  = GFX_BUFFER_INDEX, 
    .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  pipe_desc->index_buffer  = resources_get_buffer(resources_push_buffer(group_id, buff_desc));
  pipe_desc->indices_count = 6;  

  // Layout init
  
  pipe_desc->layouts[0].attributes[0]    = GFX_LAYOUT_FLOAT3;
  pipe_desc->layouts[0].attributes[1]    = GFX_LAYOUT_FLOAT3;
  pipe_desc->layouts[0].attributes[2]    = GFX_LAYOUT_FLOAT2;
  pipe_desc->layouts[0].attributes_count = 3;

  // Draw mode init
  pipe_desc->draw_mode = GFX_DRAW_MODE_TRIANGLE;
}

static void create_debug_cube_geo(const ResourceGroupID& group_id, GfxPipelineDesc* pipe_desc) {
  // Vertices init
  
  Vec3 vertices[24] = {
    // Back face

    Vec3(-1.0f, -1.0f, -1.0f),
    Vec3( 1.0f, -1.0f, -1.0f),
    Vec3( 1.0f,  1.0f, -1.0f),
    Vec3(-1.0f,  1.0f, -1.0f),

    // Front face

    Vec3(-1.0f, -1.0f,  1.0f),
    Vec3( 1.0f, -1.0f,  1.0f),
    Vec3( 1.0f,  1.0f,  1.0f),
    Vec3(-1.0f,  1.0f,  1.0f),

    // Left face

    Vec3(-1.0f,  1.0f,  1.0f),
    Vec3(-1.0f,  1.0f, -1.0f),
    Vec3(-1.0f, -1.0f, -1.0f),
    Vec3(-1.0f, -1.0f,  1.0f),

    // Right face

    Vec3(1.0f,  1.0f,  1.0f),
    Vec3(1.0f,  1.0f, -1.0f),
    Vec3(1.0f, -1.0f, -1.0f),
    Vec3(1.0f, -1.0f,  1.0f),

    // Top face

    Vec3(-1.0f, -1.0f, -1.0f),
    Vec3( 1.0f, -1.0f, -1.0f),
    Vec3( 1.0f, -1.0f,  1.0f),
    Vec3(-1.0f, -1.0f,  1.0f),

    // Bottom face

    Vec3(-1.0f,  1.0f, -1.0f),
    Vec3( 1.0f,  1.0f, -1.0f),
    Vec3( 1.0f,  1.0f,  1.0f),
    Vec3(-1.0f,  1.0f,  1.0f),
  };

  // Indices init

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

  // Vertex buffer init
  
  GfxBufferDesc buff_desc = {
    .data  = (void*)vertices,
    .size  = sizeof(Vec3) * 24,
    .type  = GFX_BUFFER_VERTEX, 
    .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  pipe_desc->vertex_buffer  = resources_get_buffer(resources_push_buffer(group_id, buff_desc));
  pipe_desc->vertices_count = 24;  

  // Index buffer init
  
  buff_desc = {
    .data  = (void*)indices,
    .size  = sizeof(indices),
    .type  = GFX_BUFFER_INDEX, 
    .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  pipe_desc->index_buffer  = resources_get_buffer(resources_push_buffer(group_id, buff_desc));
  pipe_desc->indices_count = 36;  

  // Layout init
  
  pipe_desc->layouts[0].attributes[0]    = GFX_LAYOUT_FLOAT3;
  pipe_desc->layouts[0].attributes_count = 1;

  // Draw mode init
  pipe_desc->draw_mode = GFX_DRAW_MODE_TRIANGLE;
}

/// Private functions  
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Mesh loader functions

void geometry_loader_load(const ResourceGroupID& group_id, GfxPipelineDesc* pipe_desc, const GeometryType type) {
  switch(type) {
    case GEOMETRY_CUBE:
      create_cube_geo(group_id, pipe_desc);
      break;
    case GEOMETRY_SKYBOX:
      create_skybox_geo(group_id, pipe_desc);
      break;
    case GEOMETRY_BILLBOARD:
      create_billboard_geo(group_id, pipe_desc);
      break;
    case GEOMETRY_DEBUG_CUBE:
      create_debug_cube_geo(group_id, pipe_desc);
      break;
    default:
      NIKOLA_LOG_ERROR("Invalid geometry shape given");
      break;
  }
}

/// Mesh loader functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
