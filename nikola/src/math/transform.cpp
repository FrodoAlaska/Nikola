#include "nikola/nikola_base.h"
#include "nikola/nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Math transform ***

/// ----------------------------------------------------------------------
/// Transform functions

void transform_apply(Transform& trans) {
  trans.transform = mat4_translate(trans.position) *
                    quat_to_mat4(trans.rotation)   *
                    mat4_scale(trans.scale);
}

void transform_translate(Transform& trans, const Vec3& pos) {
  trans.position = pos; 
  transform_apply(trans);
}

void transform_rotate(Transform& trans, const Quat& rot) {
  trans.rotation = quat_normalize(rot); 
  transform_apply(trans);
}

void transform_rotate(Transform& trans, const Vec4& axis_angle) {
  transform_rotate(trans, quat_angle_axis(Vec3(axis_angle), axis_angle.w));
}

void transform_rotate(Transform& trans, const Vec3& axis, const f32 angle) {
  transform_rotate(trans, quat_angle_axis(axis, angle));
}

void transform_scale(Transform& trans, const Vec3& scale) {
  trans.scale = scale; 
  transform_apply(trans);
}

void transform_lerp(Transform& trans_a, const Transform& trans_b, const f32 delta) {
  trans_a.position = vec3_lerp(trans_a.position, trans_b.position, delta);
  trans_a.rotation = quat_slerp(trans_a.rotation, trans_b.rotation, delta);
  trans_a.scale    = vec3_lerp(trans_a.scale, trans_b.scale, delta);

  transform_apply(trans_a);
}

void transform_lerp(Transform& trans, 
                    const Vec3& position, 
                    const Quat& rotation, 
                    const Vec3& scale,
                    const f32 delta) {
  trans.position = vec3_lerp(trans.position, position, delta);
  trans.rotation = quat_normalize(quat_slerp(trans.rotation, rotation, delta));
  trans.scale    = vec3_lerp(trans.scale, scale, delta);

  transform_apply(trans);
}

void transform_lerp_position(Transform& trans, const Vec3& position, const f32 delta) {
  transform_translate(trans, vec3_lerp(trans.position, position, delta));
}

void transform_slerp_rotation(Transform& trans, const Quat& rotation, const f32 delta) {
  transform_rotate(trans, quat_slerp(trans.rotation, rotation, delta));
}

void transform_lerp_rotation(Transform& trans, const Vec3& axis, const f32 angle, const f32 delta) {
  transform_slerp_rotation(trans, quat_angle_axis(axis, angle), delta);
}

void transform_lerp_scale(Transform& trans, const Vec3& scale, const f32 delta) {
  transform_scale(trans, vec3_lerp(trans.scale, scale, delta));
}

/// Transform functions
/// ----------------------------------------------------------------------

/// *** Math ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
