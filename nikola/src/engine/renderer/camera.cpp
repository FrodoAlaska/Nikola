#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Camera functions

void camera_create(Camera* cam, const f32 aspect_ratio, const Vec3& pos, const Vec3& target, const CameraMoveFn& move_fn) {
  cam->yaw   = -90.0f; 
  cam->pitch = 0.0f; 

  cam->zoom         = 90.0f;
  cam->aspect_ratio = aspect_ratio;

  Vec3 look_dir(pos - target);
  Vec3 up_axis(0.0f, 1.0f, 0.0f);
  Vec3 right_axis(vec3_cross(up_axis, look_dir));

  cam->position = pos;
  cam->up       = vec3_normalize(vec3_cross(look_dir, right_axis));

  cam->direction.x = nikola::cos((cam->yaw * nikola::DEG2RAD) * (cam->pitch * nikola::DEG2RAD));
  cam->direction.y = nikola::sin((cam->pitch * nikola::DEG2RAD));
  cam->direction.z = nikola::sin((cam->yaw * nikola::DEG2RAD)) * nikola::cos(cam->pitch * nikola::DEG2RAD);
  cam->front       = vec3_normalize(cam->direction);

  cam->view            = Mat4(1.0f);
  cam->projection      = Mat4(1.0f);
  cam->view_projection = Mat4(1.0f);

  cam->move_fn = move_fn;
}

void camera_update(Camera& cam) {
  cam.view            = mat4_look_at(cam.position, cam.position + cam.front, cam.up);
  cam.projection      = mat4_perspective((cam.zoom * nikola::DEG2RAD), cam.aspect_ratio, cam.near, cam.far);
  cam.view_projection = cam.projection * cam.view;

  Vec2 mouse_offset; 
  input_mouse_offset(&mouse_offset.x, &mouse_offset.y);

  if(cam.move_fn) {
    cam.move_fn(cam);
  }

  cam.direction.x = nikola::cos((cam.yaw * nikola::DEG2RAD) * (cam.pitch * nikola::DEG2RAD));
  cam.direction.y = nikola::sin((cam.pitch * nikola::DEG2RAD));
  cam.direction.z = nikola::sin((cam.yaw * nikola::DEG2RAD)) * nikola::cos(cam.pitch * nikola::DEG2RAD);
  cam.front       = vec3_normalize(cam.direction);
}

/// Camera functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
