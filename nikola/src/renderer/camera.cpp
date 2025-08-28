#include "nikola/nikola_render.h"
#include "nikola/nikola_gfx.h"
#include "nikola/nikola_input.h"
#include "nikola/nikola_event.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Camera consts

const f32 CAMERA_SPEED = 20.0f;

/// Camera consts
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

static bool window_resize_callback(const Event& event, const void* dispatcher, const void* listener) {
  Vec2 new_size = Vec2(0.0f);

  switch(event.type) {
    case EVENT_WINDOW_FRAMEBUFFER_RESIZED:
      new_size = Vec2(event.window_framebuffer_width, event.window_framebuffer_width);
      break;
    case EVENT_WINDOW_RESIZED:
    case EVENT_WINDOW_FULLSCREEN:
      new_size = Vec2(event.window_new_width, event.window_new_height);
      break;
    default:
      return true;
  }

  Camera* cam       = (Camera*)listener;
  cam->aspect_ratio = (f32)(new_size.x / new_size.y);

  return true;
}

/// Callbacks
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
  
  cam->direction.x = nikola::cos(cam->yaw    * DEG2RAD)  * nikola::cos(cam->pitch * DEG2RAD);
  cam->direction.y = nikola::sin((cam->pitch * DEG2RAD));
  cam->direction.z = nikola::sin((cam->yaw   * DEG2RAD)) * nikola::cos(cam->pitch * DEG2RAD);
  cam->front       = vec3_normalize(cam->direction);

  cam->view            = Mat4(1.0f);
  cam->projection      = Mat4(1.0f);
  cam->view_projection = Mat4(1.0f);

  cam->move_fn   = desc.move_func;
  cam->is_active = true;

  event_listen(EVENT_WINDOW_RESIZED, window_resize_callback, cam);
  event_listen(EVENT_WINDOW_FULLSCREEN, window_resize_callback, cam);
  event_listen(EVENT_WINDOW_FRAMEBUFFER_RESIZED, window_resize_callback, cam);
}

void camera_update(Camera& cam) {
  cam.view            = mat4_look_at(cam.position, cam.position + cam.front, cam.up);
  cam.projection      = mat4_perspective((cam.zoom * DEG2RAD), cam.aspect_ratio, cam.near, cam.far);
  cam.view_projection = (cam.projection * cam.view);

  if(cam.move_fn && cam.is_active) {
    cam.move_fn(cam);
  }

  cam.direction.x = nikola::cos(cam.yaw   * DEG2RAD) * nikola::cos(cam.pitch * DEG2RAD);
  cam.direction.y = nikola::sin(cam.pitch * DEG2RAD);
  cam.direction.z = nikola::sin(cam.yaw   * DEG2RAD) * nikola::cos(cam.pitch * DEG2RAD);
  cam.front       = vec3_normalize(cam.direction);
}

void camera_calculate_frustrum_corners(const Camera& cam, Vec3* out_corners) {
  // Mat4 inv = mat4_inverse(cam.view_projection);
  //
  // for(sizei x = 0; x < 2; x++) {
  //   for(sizei y = 0; y < 2; y++) {
  //     for(sizei z = 0; z < 2; z++) {
  //       Vec4 point = Vec4(2.0f * x - 1.0f, 
  //                         2.0f * y - 1.0f, 
  //                         2.0f * z - 1.0f, 
  //                         1.0f);
  //      
  //       Vec4 corner = inv * point;
  //
  //       sizei flat_index        = x + 2 * (y + 2 * z);
  //       out_corners[flat_index] = Vec3(corner / corner.w);
  //     }
  //   }
  // }

  
  f32 tan_fov    = nikola::tan(cam.zoom * DEG2RAD);
  Vec3 cam_right = vec3_cross(cam.front, cam.up);

  // Calculating the bounds of the frustrum

  f32 far_width  = tan_fov * cam.far;
  f32 far_height = far_width / cam.aspect_ratio;

  f32 near_width  = tan_fov * cam.near;
  f32 near_height = near_width / cam.aspect_ratio;

  // Calculating the center of both the near and far planes

  Vec3 far_center  = cam.position + (cam.front * cam.far);
  Vec3 near_center = cam.position + (cam.front * cam.near);

  // Calculating each far point in both the near and far planes

  Vec3 far_top   = far_center + cam.up    * far_height; 
  Vec3 far_right = far_center + cam_right * far_width;

  Vec3 near_top   = near_center + cam.up    * near_height; 
  Vec3 near_right = near_center + cam_right * near_width;

  // Filling the given `corners` array with the new calculated values
  
  // Far plane

  out_corners[0] = far_center + far_top - far_right; // Bottom left
  out_corners[1] = far_center + far_top - far_right; // Top left
  out_corners[2] = far_center + far_top + far_right; // Top right
  out_corners[3] = far_center - far_top + far_right; // Bottom right

  // Near plane

  out_corners[4] = near_center - near_top - near_right; // Bottom left
  out_corners[5] = near_center + near_top - near_right; // Top left
  out_corners[6] = near_center + near_top + near_right; // Top right
  out_corners[7] = near_center - near_top + near_right; // Bottom right
 
}

/// Camera functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
