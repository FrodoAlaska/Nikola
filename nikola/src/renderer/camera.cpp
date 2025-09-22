#include "nikola/nikola_render.h"
#include "nikola/nikola_gfx.h"
#include "nikola/nikola_input.h"
#include "nikola/nikola_event.h"
#include "nikola/nikola_physics.h"

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

void camera_follow(Camera& cam, const Vec3& target, const Vec3& offset) {
  cam.position = target + offset;
}

void camera_follow_lerp(Camera& cam, const Vec3& target, const Vec3& offset, const f32 delta) {
  cam.position = vec3_lerp(cam.position, target + offset, delta);
}

Vec2 camera_world_to_screen_space(const Camera& cam, const Vec3 position, const Window* window) {
  // Get the window size
  
  IVec2 window_size; 
  window_get_size(window, &window_size.x, &window_size.y);

  // Transform the given position to NDC

  Vec4 ndc_pos = Vec4(position, 1.0f);
  ndc_pos      = cam.view_projection * ndc_pos;

  // Transform the NDC position into screen space

  Vec2 screen_space = Vec2(ndc_pos.x / ndc_pos.w, -ndc_pos.y / ndc_pos.w);
  return (screen_space + 1.0f) / 2.0f * Vec2(window_size);
}

RayCastDesc camera_screen_to_world_space(const Camera& cam, const Vec2 position, const Window* window) {
  // Get the window size
  
  IVec2 window_size; 
  window_get_size(window, &window_size.x, &window_size.y);

  // Converting the given position to NDC coords
  
  Vec2 ndc_coords = Vec2((position.x / window_size.x - 0.5f) * 2.0f, 
                         (position.y / window_size.y - 0.5f) * 2.0f);

  // "Unprojecting" the NDC coords and bringing it back to view space 
  // to get the near and far planes.

  Mat4 inv_proj   = mat4_inverse(cam.view_projection);
  Vec4 near_point = inv_proj * Vec4(ndc_coords.x, ndc_coords.y, -1.0f, 1.0f);
  Vec4 far_point  = inv_proj * Vec4(ndc_coords.x, ndc_coords.y, 0.0f, 1.0f);

  near_point /= near_point.w;
  far_point  /= far_point.w;

  // Convert whatever is up there to a ray

  return RayCastDesc {
    .origin    = cam.position,
    .direction = vec3_normalize(Vec3(far_point - near_point)),
    .distance  = 1000000.0f,
  };
}

/// Camera functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
