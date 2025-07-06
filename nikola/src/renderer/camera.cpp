#include "nikola/nikola_render.h"
#include "nikola/nikola_gfx.h"
#include "nikola/nikola_input.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Camera consts
const f32 CAMERA_SPEED = 20.0f;
/// Camera consts
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Camera functions

void camera_free_move_func(Camera& camera) {
  float speed = CAMERA_SPEED * niclock_get_delta_time();

  Vec2 mouse_offset; 
  input_mouse_offset(&mouse_offset.x, &mouse_offset.y);

  camera.yaw   = mouse_offset.x * camera.sensitivity;
  camera.pitch = mouse_offset.y * camera.sensitivity;

  camera.pitch = clamp_float(camera.pitch, -CAMERA_MAX_DEGREES, CAMERA_MAX_DEGREES);
  camera.zoom  = clamp_float(camera.zoom, 1.0f, CAMERA_MAX_ZOOM);

  // Move forward
  if(input_key_down(KEY_UP)) {
    camera.position += camera.front * speed;
  }
  // Move backwards
  else if(input_key_down(KEY_DOWN)) {
    camera.position -= camera.front * speed;
  }
 
  // Move right
  if(input_key_down(KEY_RIGHT)) {
    camera.position += vec3_normalize(vec3_cross(camera.front, camera.up)) * speed;
  }
  // Move left
  else if(input_key_down(KEY_LEFT)) {
    camera.position -= vec3_normalize(vec3_cross(camera.front, camera.up)) * speed;
  }
}

void camera_fps_move_func(Camera& camera) {
  float speed = CAMERA_SPEED * niclock_get_delta_time();

  Vec2 mouse_offset; 
  input_mouse_offset(&mouse_offset.x, &mouse_offset.y);

  camera.yaw   = mouse_offset.x * camera.sensitivity;
  camera.pitch = mouse_offset.y * camera.sensitivity;

  camera.pitch = clamp_float(camera.pitch, -CAMERA_MAX_DEGREES, CAMERA_MAX_DEGREES);
  camera.zoom  = clamp_float(camera.zoom, 1.0f, CAMERA_MAX_ZOOM);

  // Move forward
  if(input_key_down(KEY_W)) {
    camera.position += Vec3(camera.front.x, 0.0f, camera.front.z) * speed;
  }
  // Move backwards
  else if(input_key_down(KEY_S)) {
    camera.position -= Vec3(camera.front.x, 0.0f, camera.front.z) * speed;
  }
 
  // Move right
  if(input_key_down(KEY_A)) {
    camera.position -= vec3_normalize(vec3_cross(camera.front, camera.up)) * speed;
  }
  // Move left
  else if(input_key_down(KEY_D)) {
    camera.position += vec3_normalize(vec3_cross(camera.front, camera.up)) * speed;
  }
}

void camera_create(Camera* cam, const CameraDesc& desc) {
  NIKOLA_ASSERT(cam, "Invalid Camera struct given to camera_create");
  memory_zero(cam, sizeof(Camera));

  cam->yaw   = -90.0f; 
  cam->pitch = 0.0f; 

  cam->zoom         = 45.0f;
  cam->aspect_ratio = desc.aspect_ratio;

  cam->near = desc.near;
  cam->far  = desc.far;

  cam->sensitivity = 0.1f;
  cam->exposure    = 1.0f;

  Vec3 look_dir(desc.position - desc.target);
  Vec3 right_axis(vec3_cross(desc.up_axis, look_dir));

  cam->position = desc.position;
  cam->up       = vec3_normalize(vec3_cross(look_dir, right_axis));
  
  cam->direction.x = nikola::cos(cam->yaw    * nikola::DEG2RAD)  * nikola::cos(cam->pitch * nikola::DEG2RAD);
  cam->direction.y = nikola::sin((cam->pitch * nikola::DEG2RAD));
  cam->direction.z = nikola::sin((cam->yaw   * nikola::DEG2RAD)) * nikola::cos(cam->pitch * nikola::DEG2RAD);
  cam->front       = vec3_normalize(cam->direction);

  cam->view            = Mat4(1.0f);
  cam->projection      = Mat4(1.0f);
  cam->view_projection = Mat4(1.0f);

  cam->move_fn   = desc.move_func;
  cam->is_active = true;
}

void camera_update(Camera& cam) {
  cam.view            = mat4_look_at(cam.position, cam.position + cam.front, cam.up);
  cam.projection      = mat4_perspective((cam.zoom * nikola::DEG2RAD), cam.aspect_ratio, cam.near, cam.far);
  cam.view_projection = (cam.projection * cam.view);

  if(cam.move_fn && cam.is_active) {
    cam.move_fn(cam);
  }

  cam.direction.x = nikola::cos(cam.yaw   * DEG2RAD) * nikola::cos(cam.pitch * DEG2RAD);
  cam.direction.y = nikola::sin(cam.pitch * DEG2RAD);
  cam.direction.z = nikola::sin(cam.yaw   * DEG2RAD) * nikola::cos(cam.pitch * DEG2RAD);
  cam.front       = vec3_normalize(cam.direction);
}

/// Camera functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
