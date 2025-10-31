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

    Vec3 normal = vec3_normalize(vec3_cross(edge1, edge2));

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

static void create_cube_geo(DynamicArray<f32>& vertices, DynamicArray<u32>& indices) {
  // Indices init

  indices = {
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
  
  Vertex3D full_vertices[24];
  
  // Position, Color

  // Back face
  
  full_vertices[0].position = Vec3(-1.0f, -1.0f, -1.0f);
  full_vertices[1].position = Vec3( 1.0f, -1.0f, -1.0f);
  full_vertices[2].position = Vec3( 1.0f,  1.0f, -1.0f);
  full_vertices[3].position = Vec3(-1.0f,  1.0f, -1.0f);
  
  full_vertices[0].texture_coords = Vec2(0.0f, 0.0f);
  full_vertices[1].texture_coords = Vec2(1.0f, 0.0f);
  full_vertices[2].texture_coords = Vec2(1.0f, 1.0f);
  full_vertices[3].texture_coords = Vec2(0.0f, 1.0f);

  // Front face
   
  full_vertices[4].position = Vec3(-1.0f, -1.0f,  1.0f);
  full_vertices[5].position = Vec3( 1.0f, -1.0f,  1.0f);
  full_vertices[6].position = Vec3( 1.0f,  1.0f,  1.0f);
  full_vertices[7].position = Vec3(-1.0f,  1.0f,  1.0f);
  
  full_vertices[4].texture_coords = Vec2(0.0f, 0.0f);
  full_vertices[5].texture_coords = Vec2(1.0f, 0.0f);
  full_vertices[6].texture_coords = Vec2(1.0f, 1.0f);
  full_vertices[7].texture_coords = Vec2(0.0f, 1.0f);
  
  // Left face
  
  full_vertices[8].position  = Vec3(-1.0f,  1.0f,  1.0f);
  full_vertices[9].position  = Vec3(-1.0f,  1.0f, -1.0f);
  full_vertices[10].position = Vec3(-1.0f, -1.0f, -1.0f);
  full_vertices[11].position = Vec3(-1.0f, -1.0f,  1.0f);
  
  full_vertices[8].texture_coords  = Vec2(1.0f, 0.0f);
  full_vertices[9].texture_coords  = Vec2(1.0f, 1.0f);
  full_vertices[10].texture_coords = Vec2(0.0f, 1.0f);
  full_vertices[11].texture_coords = Vec2(0.0f, 0.0f);
 
  // Right face
 
  full_vertices[12].position = Vec3(1.0f,  1.0f,  1.0f);
  full_vertices[13].position = Vec3(1.0f,  1.0f, -1.0f);
  full_vertices[14].position = Vec3(1.0f, -1.0f, -1.0f);
  full_vertices[15].position = Vec3(1.0f, -1.0f,  1.0f);
  
  full_vertices[12].texture_coords = Vec2(1.0f, 0.0f);
  full_vertices[13].texture_coords = Vec2(1.0f, 1.0f);
  full_vertices[14].texture_coords = Vec2(0.0f, 1.0f);
  full_vertices[15].texture_coords = Vec2(0.0f, 0.0f);

  // Top face
 
  full_vertices[16].position = Vec3(-1.0f, -1.0f, -1.0f);
  full_vertices[17].position = Vec3( 1.0f, -1.0f, -1.0f);
  full_vertices[18].position = Vec3( 1.0f, -1.0f,  1.0f);
  full_vertices[19].position = Vec3(-1.0f, -1.0f,  1.0f);
  
  full_vertices[16].texture_coords = Vec2(0.0f, 1.0f);
  full_vertices[17].texture_coords = Vec2(1.0f, 1.0f);
  full_vertices[18].texture_coords = Vec2(1.0f, 0.0f);
  full_vertices[19].texture_coords = Vec2(0.0f, 0.0f);
 
  // Bottom face
 
  full_vertices[20].position = Vec3(-1.0f,  1.0f, -1.0f);
  full_vertices[21].position = Vec3( 1.0f,  1.0f, -1.0f);
  full_vertices[22].position = Vec3( 1.0f,  1.0f,  1.0f);
  full_vertices[23].position = Vec3(-1.0f,  1.0f,  1.0f);
  
  full_vertices[20].texture_coords = Vec2(0.0f, 1.0f);
  full_vertices[21].texture_coords = Vec2(1.0f, 1.0f);
  full_vertices[22].texture_coords = Vec2(1.0f, 0.0f);
  full_vertices[23].texture_coords = Vec2(0.0f, 0.0f);

  // Normals and Tangents

  generate_normals(full_vertices, indices.size(), indices.data());
  generate_tangents(full_vertices, indices.size(), indices.data());

  // Carrying over the full vertices into just floats
  // @NOTE: Woah!!!!! What the hell??? How dare you??!!

  vertices.reserve(24 * sizeof(Vertex3D));
  for(auto& vertex : full_vertices) {
    // Position

    vertices.push_back(vertex.position.x);
    vertices.push_back(vertex.position.y);
    vertices.push_back(vertex.position.z);

    // Normal

    vertices.push_back(vertex.normal.x);
    vertices.push_back(vertex.normal.y);
    vertices.push_back(vertex.normal.z);

    // Tangents
    
    vertices.push_back(vertex.tangent.x);
    vertices.push_back(vertex.tangent.y);
    vertices.push_back(vertex.tangent.z);

    // Joint IDs
    
    vertices.push_back(vertex.joint_ids.x);
    vertices.push_back(vertex.joint_ids.y);
    vertices.push_back(vertex.joint_ids.z);
    vertices.push_back(vertex.joint_ids.w);

    // Joint weight
    
    vertices.push_back(vertex.joint_weights.x);
    vertices.push_back(vertex.joint_weights.y);
    vertices.push_back(vertex.joint_weights.z);
    vertices.push_back(vertex.joint_weights.w);

    // Texture coords

    vertices.push_back(vertex.texture_coords.x);
    vertices.push_back(vertex.texture_coords.y);
  }
}

static void create_sphere_geo(DynamicArray<f32>& vertices, DynamicArray<u32>& indices) {
  ///
  /// @NOTE (9/7/2025, Mohamed):
  /// No, I'm not smart enough to figure this whole thing out. Like any other great programmer, 
  /// I've stolen this solution from a much capable person. Please go visit their website because 
  /// it's amazing. 
  ///
  /// Website: https://www.songho.ca/
  /// Specific sphere example: https://www.songho.ca/opengl/gl_sphere.html
  ///

  u32 sector_count = 32;
  u32 stack_count  = 32;

  f32 sector_step = 2 * PI / sector_count;
  f32 stack_step  = PI / stack_count;
  f32 radius      = 1.0f;
  f32 inv_length  = 1.0f / radius;

  // Vertices init
  
  DynamicArray<Vertex3D> full_vertices; // @NOTE: I know this is bad, but oh well...
  full_vertices.reserve(24);

  for(u32 i = 0; i <= stack_count; i++) {
    f32 stack_angle = ((PI / 2.0f) - (i * stack_step));

    f32 xy = radius * nikola::cos(stack_angle);
    f32 z  = radius * nikola::sin(stack_angle);

    for(u32 j = 0; j <= sector_count; j++) {
      f32 sector_angle = j * sector_step;
      Vertex3D vertex  = {};

      // Positions
      vertex.position = Vec3(xy * nikola::cos(sector_angle),
                             xy * nikola::sin(sector_angle), 
                             z);

      // Normals
      vertex.normal = -(vertex.position * inv_length);

      // Texture coords
      vertex.texture_coords = Vec2((f32)j / sector_count, (f32)i / stack_count); 

      // New vertex!
      full_vertices.push_back(vertex); 
    }
  }

  // Indices init

  u32 k1 = 0; 
  u32 k2 = 0;

  indices.reserve(36);

  for(u32 i = 0; i < stack_count; i++) {
    k1 = i * (sector_count + 1);
    k2 = k1 + sector_count + 1;

    for(u32 j = 0; j < sector_count; j++, k1++, k2++) {
      if(i != 0) {
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k1 + 1);
      }

      if(i != (stack_count - 1)) {
        indices.push_back(k1 + 1);
        indices.push_back(k2);
        indices.push_back(k2 + 1);
      }
    }
  }

  // Tangents?
  generate_tangents(&full_vertices[0], 36, &indices[0]);

  // Carrying over the full vertices into just floats
  // @NOTE: Still bad. Don't worry about it, though. Calm down, dude.

  vertices.reserve(24 * sizeof(Vertex3D));
  for(auto& vertex : full_vertices) {
    // Position

    vertices.push_back(vertex.position.x);
    vertices.push_back(vertex.position.y);
    vertices.push_back(vertex.position.z);

    // Normal

    vertices.push_back(vertex.normal.x);
    vertices.push_back(vertex.normal.y);
    vertices.push_back(vertex.normal.z);

    // Tangents
    
    vertices.push_back(vertex.tangent.x);
    vertices.push_back(vertex.tangent.y);
    vertices.push_back(vertex.tangent.z);

    // Joint IDs
    
    vertices.push_back(vertex.joint_ids.x);
    vertices.push_back(vertex.joint_ids.y);
    vertices.push_back(vertex.joint_ids.z);
    vertices.push_back(vertex.joint_ids.w);

    // Joint weight
    
    vertices.push_back(vertex.joint_weights.x);
    vertices.push_back(vertex.joint_weights.y);
    vertices.push_back(vertex.joint_weights.z);
    vertices.push_back(vertex.joint_weights.w);

    // Texture coords

    vertices.push_back(vertex.texture_coords.x);
    vertices.push_back(vertex.texture_coords.y);
  }
}

static void create_skybox_geo(DynamicArray<f32>& vertices) {
  // Vertices init
  
  vertices = {
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
}

static void create_billboard_geo(DynamicArray<f32>& vertices, DynamicArray<u32>& indices) {
  // Vertices init
  
  vertices = {
    // Position          Normal             Texture coords
    -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
     1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
  };

  // Indices init
 
  indices = {
    0, 1, 2, 
    2, 3, 0,
  };
}

static void create_debug_cube_geo(DynamicArray<f32>& vertices, DynamicArray<u32>& indices) {
  // Vertices init
 
  vertices = {
    // Back face

    -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
    -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,

    // Front face

    -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
    -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,

    // Left face

    -1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,

    // Right face

    1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
    1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
    1.0f, -1.0f,  1.0f,  0.0f, 0.0f,

    // Top face

    -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
     1.0f, -1.0f, -1.0f,  1.0f, 1.0f,
     1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,

    // Bottom face

    -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
     1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
     1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
  };

  indices = {
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
}

static void create_debug_sphere_geo(DynamicArray<f32>& vertices, DynamicArray<u32>& indices) {
  u32 sector_count = 32;
  u32 stack_count  = 32;

  f32 sector_step = 2 * PI / sector_count;
  f32 stack_step  = PI / stack_count;
  f32 radius      = 1.0f;

  // Vertices init
  
  vertices.reserve(120);

  for(u32 i = 0; i <= stack_count; i++) {
    f32 stack_angle = ((PI / 2.0f) - (i * stack_step));

    f32 xy = radius * nikola::cos(stack_angle);
    f32 z  = radius * nikola::sin(stack_angle);

    for(u32 j = 0; j <= sector_count; j++) {
      f32 sector_angle = j * sector_step;
    
      // Positions
      
      vertices.push_back(xy * nikola::cos(sector_angle));
      vertices.push_back(xy * nikola::sin(sector_angle));
      vertices.push_back(z);

      // Texture coords
      
      vertices.push_back((f32)j / sector_count);
      vertices.push_back((f32)i / stack_count); 
    }
  }

  // Indices init

  u32 k1 = 0; 
  u32 k2 = 0;

  indices.reserve(32);

  for(u32 i = 0; i < stack_count; i++) {
    k1 = i * (sector_count + 1);
    k2 = k1 + sector_count + 1;

    for(u32 j = 0; j < sector_count; j++, k1++, k2++) {
      if(i != 0) {
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k1 + 1);
      }

      if(i != (stack_count - 1)) {
        indices.push_back(k1 + 1);
        indices.push_back(k2);
        indices.push_back(k2 + 1);
      }
    }
  }
}

/// Private functions  
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Geometry loader functions

void geometry_loader_load(DynamicArray<f32>& vertices, DynamicArray<u32>& indices, const GeometryType type) {
  switch(type) {
    case GEOMETRY_CUBE:
      create_cube_geo(vertices, indices);
      break;
    case GEOMETRY_SPHERE:
      create_sphere_geo(vertices, indices);
      break;
    case GEOMETRY_SKYBOX:
      create_skybox_geo(vertices);
      break;
    case GEOMETRY_BILLBOARD:
      create_billboard_geo(vertices, indices);
      break;
    case GEOMETRY_DEBUG_CUBE:
      create_debug_cube_geo(vertices, indices);
      break;
    case GEOMETRY_DEBUG_SPHERE:
      create_debug_sphere_geo(vertices, indices);
      break;
    default:
      NIKOLA_LOG_ERROR("Invalid geometry shape given");
      break;
  }
}

void geometry_loader_set_vertex_layout(GfxVertexLayout& layout, const GeometryType type) {
  // @TODO (Geometry): This is not the best. Find a better place to do 
  // this. Maybe bring back the `VertexType` enum?

  switch(type) {
    case GEOMETRY_CUBE:
    case GEOMETRY_SPHERE:
      layout.attributes[0]    = GFX_LAYOUT_FLOAT3; // Position
      layout.attributes[1]    = GFX_LAYOUT_FLOAT3; // Normal
      layout.attributes[2]    = GFX_LAYOUT_FLOAT3; // Tangent
      layout.attributes[3]    = GFX_LAYOUT_FLOAT4; // Joint ID
      layout.attributes[4]    = GFX_LAYOUT_FLOAT4; // Joint weight
      layout.attributes[5]    = GFX_LAYOUT_FLOAT2; // Texture coords
      layout.attributes_count = 6;
      break;
    case GEOMETRY_SKYBOX:
      layout.attributes[0]    = GFX_LAYOUT_FLOAT3; // Position
      layout.attributes_count = 1;
      break;
    case GEOMETRY_BILLBOARD:
      layout.attributes[0]    = GFX_LAYOUT_FLOAT3; // Position
      layout.attributes[1]    = GFX_LAYOUT_FLOAT3; // Normal
      layout.attributes[2]    = GFX_LAYOUT_FLOAT2; // Texture coords
      layout.attributes_count = 3;
      break;
    case GEOMETRY_DEBUG_CUBE:
    case GEOMETRY_DEBUG_SPHERE:
      layout.attributes[0]    = GFX_LAYOUT_FLOAT3; // Position
      layout.attributes[1]    = GFX_LAYOUT_FLOAT2; // Texture coords
      layout.attributes_count = 2;
      break;
    default:
      NIKOLA_LOG_ERROR("Invalid geometry shape given");
      break;
  }
}

/// Geometry loader functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
