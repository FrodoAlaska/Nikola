#include "nikola/nikola_base.h"
#include "nikola/nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Math transform ***

/// ----------------------------------------------------------------------
/// Vertex functions

const sizei vertex_get_components_count(const i32 vertex_flags) {
  sizei count = 0; 

  // Vec3

  if(IS_BIT_SET(vertex_flags, VERTEX_COMPONENT_POSITION)) {
    count += 3;
  }

  if(IS_BIT_SET(vertex_flags, VERTEX_COMPONENT_NORMAL)) {
    count += 3;
  }

  if(IS_BIT_SET(vertex_flags, VERTEX_COMPONENT_TANGENT)) {
    count += 3;
  }

  // Vec2 

  if(IS_BIT_SET(vertex_flags, VERTEX_COMPONENT_TEXTURE_COORDS)) {
    count += 2;
  }

  // Vec4

  if(IS_BIT_SET(vertex_flags, VERTEX_COMPONENT_JOINT_ID)) {
    count += 4;
  }

  if(IS_BIT_SET(vertex_flags, VERTEX_COMPONENT_JOINT_WEIGHT)) {
    count += 4;
  }

  return count;
}

const sizei vertex_get_stride(const i32 vertex_flags) {
  sizei stride = 0;

  // Vec3

  if(IS_BIT_SET(vertex_flags, VERTEX_COMPONENT_POSITION)) {
    stride += sizeof(Vec3);
  }

  if(IS_BIT_SET(vertex_flags, VERTEX_COMPONENT_NORMAL)) {
    stride += sizeof(Vec3);
  }

  if(IS_BIT_SET(vertex_flags, VERTEX_COMPONENT_TANGENT)) {
    stride += sizeof(Vec3);
  }

  // Vec2 

  if(IS_BIT_SET(vertex_flags, VERTEX_COMPONENT_TEXTURE_COORDS)) {
    stride += sizeof(Vec2);
  }

  // Vec4

  if(IS_BIT_SET(vertex_flags, VERTEX_COMPONENT_JOINT_ID)) {
    stride += sizeof(Vec4);
  }

  if(IS_BIT_SET(vertex_flags, VERTEX_COMPONENT_JOINT_WEIGHT)) {
    stride += sizeof(Vec4);
  }

  return stride;
}

/// Vertex functions
/// ----------------------------------------------------------------------

/// *** Math ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
