#include "nikola/nikola_base.h"
#include "nikola/nikola_math.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Math transform ***

/// ----------------------------------------------------------------------
/// Private functions
static void update_transform(Transform& trans) {
  trans.transform = mat4_translate(trans.position) *
                    quat_to_mat4(trans.rotation)   *
                    mat4_scale(trans.scale);
}
/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Transform functions

void transform_translate(Transform& trans, const Vec3& pos) {
  trans.position = pos; 
  update_transform(trans);
}

void transform_rotate(Transform& trans, const Quat& rot) {
  trans.rotation = rot; 
  update_transform(trans);
}

void transform_rotate(Transform& trans, const Vec4& axis_angle) {
  trans.rotation = quat_angle_axis(Vec3(axis_angle), axis_angle.w); 
  update_transform(trans);
}

void transform_rotate(Transform& trans, const Vec3& axis, const f32 angle) {
  trans.rotation = quat_angle_axis(axis, angle); 
  update_transform(trans);
}

void transform_scale(Transform& trans, const Vec3& scale) {
  trans.scale = scale; 
  update_transform(trans);
}

/// Transform functions
/// ----------------------------------------------------------------------

/// *** Math ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
