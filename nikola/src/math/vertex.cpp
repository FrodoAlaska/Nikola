#include "nikola/nikola_base.h"
#include "nikola/nikola_math.h"
#include "nikola/nikola_gfx.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Vertex functions

const sizei vertex_type_size(const VertexType type) {
  switch(type) {
    case VERTEX_TYPE_PNUV:
      return sizeof(Vertex3D_PNUV);
    case VERTEX_TYPE_PCUV:
      return sizeof(Vertex3D_PCUV);
    case VERTEX_TYPE_PNCUV:
      return sizeof(Vertex3D_PNCUV);
    case VERTEX_TYPE_PNCTUV:
      return sizeof(Vertex3D_PNCTUV);
    case VERTEX_TYPE_PNTIWUV:
      return sizeof(Vertex3D_PNTIWUV);
    default:
      return 0;
  }
}

const u8 vertex_type_components(const VertexType type) {
  switch(type) {
    case VERTEX_TYPE_PNUV:
    case VERTEX_TYPE_PCUV:
      return 3;
    case VERTEX_TYPE_PNCUV:
      return 4;
    case VERTEX_TYPE_PNCTUV:
      return 5;
    case VERTEX_TYPE_PNTIWUV:
      return 6;
    default:
      return 0;
  }
}

const char* vertex_type_str(const VertexType type) {
  switch(type) {
    case VERTEX_TYPE_PNUV:
      return "VERTEX_TYPE_PNUV";
    case VERTEX_TYPE_PCUV:
      return "VERTEX_TYPE_PCUV";
    case VERTEX_TYPE_PNCUV:
      return "VERTEX_TYPE_PNCUV";
    case VERTEX_TYPE_PNCTUV:
      return "VERTEX_TYPE_PNCTUV";
    case VERTEX_TYPE_PNTIWUV:
      return "VERTEX_TYPE_PNTIWUV";
    default:
      return "VERTEX_INVALID";
  }
}

void vertex_type_layout(const VertexType type, GfxVertexLayout* out_layout) {
  switch(type) {
    case VERTEX_TYPE_PCUV: 
      out_layout->attributes[0]    = GFX_LAYOUT_FLOAT3; // Position 
      out_layout->attributes[1]    = GFX_LAYOUT_FLOAT4; // Color
      out_layout->attributes[2]    = GFX_LAYOUT_FLOAT2; // Texture coordinates
      out_layout->attributes_count = vertex_type_components(VERTEX_TYPE_PCUV);
      break;
    case VERTEX_TYPE_PNUV: 
      out_layout->attributes[0]    = GFX_LAYOUT_FLOAT3; // Position 
      out_layout->attributes[1]    = GFX_LAYOUT_FLOAT3; // Normal 
      out_layout->attributes[2]    = GFX_LAYOUT_FLOAT2; // Texture coordinates 
      out_layout->attributes_count = vertex_type_components(VERTEX_TYPE_PNUV);
      break;
    case VERTEX_TYPE_PNCUV: 
      out_layout->attributes[0]    = GFX_LAYOUT_FLOAT3; // Position 
      out_layout->attributes[1]    = GFX_LAYOUT_FLOAT3; // Normal 
      out_layout->attributes[2]    = GFX_LAYOUT_FLOAT4; // Color 
      out_layout->attributes[3]    = GFX_LAYOUT_FLOAT2; // Texture coordinates
      out_layout->attributes_count = vertex_type_components(VERTEX_TYPE_PNCUV);
      break;
    case VERTEX_TYPE_PNCTUV: 
      out_layout->attributes[0]    = GFX_LAYOUT_FLOAT3; // Position 
      out_layout->attributes[1]    = GFX_LAYOUT_FLOAT3; // Normal 
      out_layout->attributes[2]    = GFX_LAYOUT_FLOAT4; // Color 
      out_layout->attributes[3]    = GFX_LAYOUT_FLOAT3; // Tangent
      out_layout->attributes[4]    = GFX_LAYOUT_FLOAT2; // Texture coordinates
      out_layout->attributes_count = vertex_type_components(VERTEX_TYPE_PNCTUV);
      break;
    case VERTEX_TYPE_PNTIWUV: 
      out_layout->attributes[0]    = GFX_LAYOUT_FLOAT3; // Position 
      out_layout->attributes[1]    = GFX_LAYOUT_FLOAT3; // Normal 
      out_layout->attributes[2]    = GFX_LAYOUT_FLOAT3; // Tangent 
      out_layout->attributes[3]    = GFX_LAYOUT_FLOAT4; // Joint ID
      out_layout->attributes[4]    = GFX_LAYOUT_FLOAT4; // Joint weight
      out_layout->attributes[5]    = GFX_LAYOUT_FLOAT2; // Texture coordinates
      out_layout->attributes_count = vertex_type_components(VERTEX_TYPE_PNTIWUV);
      break;
  }
}

/// Vertex functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Math ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
