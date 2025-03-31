#include "nikola/nikola_base.h"
#include "nikola/nikola_math.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

void vertex_type_layout(const VertexType type, GfxLayoutDesc* layout, sizei* count) {
  switch(type) {
    case VERTEX_TYPE_PCUV: 
      layout[0] = {"POSITION", GFX_LAYOUT_FLOAT3, 0};
      layout[1] = {"COLOR", GFX_LAYOUT_FLOAT4, 0};
      layout[2] = {"TEX", GFX_LAYOUT_FLOAT2, 0};
      *count    = 3;
      break;
    case VERTEX_TYPE_PNUV: 
      layout[0] = {"POSITION", GFX_LAYOUT_FLOAT3, 0};
      layout[1] = {"NORMAL", GFX_LAYOUT_FLOAT3, 0};
      layout[2] = {"TEX", GFX_LAYOUT_FLOAT2, 0};
      *count    = 3;
      break;
    case VERTEX_TYPE_PNCUV: 
      layout[0] = {"POSITION", GFX_LAYOUT_FLOAT3, 0};
      layout[1] = {"NORMAL", GFX_LAYOUT_FLOAT3, 0};
      layout[2] = {"COLOR", GFX_LAYOUT_FLOAT4, 0};
      layout[3] = {"TEX", GFX_LAYOUT_FLOAT2, 0};
      *count    = 3;
      break;
  }
}

/// Vertex functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Math ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
