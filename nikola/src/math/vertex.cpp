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
      return "VERTEX_TYPE_PCUV";
    default:
      return "VERTEX_INVALID";
  }
}

void vertex_type_layout(const VertexType type, GfxVertexLayout* out_layout) {
  switch(type) {
    case VERTEX_TYPE_PCUV: 
      out_layout->attributes[0]    = GFX_LAYOUT_FLOAT3;
      out_layout->attributes[1]    = GFX_LAYOUT_FLOAT4;
      out_layout->attributes[2]    = GFX_LAYOUT_FLOAT2;
      out_layout->attributes_count = 3;
      break;
    case VERTEX_TYPE_PNUV: 
      out_layout->attributes[0]    = GFX_LAYOUT_FLOAT3;
      out_layout->attributes[1]    = GFX_LAYOUT_FLOAT3;
      out_layout->attributes[2]    = GFX_LAYOUT_FLOAT2;
      out_layout->attributes_count = 3;
      break;
    case VERTEX_TYPE_PNCUV: 
      out_layout->attributes[0]    = GFX_LAYOUT_FLOAT3;
      out_layout->attributes[1]    = GFX_LAYOUT_FLOAT3;
      out_layout->attributes[2]    = GFX_LAYOUT_FLOAT4;
      out_layout->attributes[3]    = GFX_LAYOUT_FLOAT2;
      out_layout->attributes_count = 4;
      break;
  }
}

/// Vertex functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Math ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
