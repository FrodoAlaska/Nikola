#include "app.h"

#include <nikola/nikola.h>
#include <imgui/imgui.h>
#include <q3.h>

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;
  nikola::FrameData frame_data;

  nikola::u16 res_group_id;
  nikola::ResourceID mesh_id, material_id;

  nikola::PhysicsBodyID cube_body, plane_body;
  nikola::ColliderID cube_collider, plane_collider;

  bool has_editor = false;
};
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

static void init_resources(nikola::App* app) {
  // Resource storage init 
  nikola::FilePath res_path = nikola::filepath_append(nikola::filesystem_current_path(), "res");
  app->res_group_id = nikola::resources_create_group("app_res", res_path);

  // Resoruces init
  nikola::resources_push_dir(app->res_group_id, "textures");

  // Skybox init
  app->frame_data.skybox_id = nikola::resources_push_skybox(app->res_group_id, "cubemaps/accurate_night.nbrcubemap");

  // Materials init
  app->material_id = nikola::resources_push_material(app->res_group_id, nikola::resources_get_id(app->res_group_id, "grass"));

  // Mesh init
  app->mesh_id = nikola::resources_push_mesh(app->res_group_id, nikola::GEOMATRY_CUBE);
}

static void init_bodies(nikola::App* app) {
  // Cube 
  nikola::PhysicsBodyDesc body_desc = {
    .position = nikola::Vec3(10.0f, 0.0f, 10.0f), 
    .type     = nikola::PHYSICS_BODY_DYNAMIC, 
  };
  app->cube_body = nikola::physics_body_create(body_desc);

  // Cube collider 
  nikola::ColliderDesc coll_desc = {
    .position = nikola::Vec3(0.0f),
    .extents  = nikola::Vec3(1.0f), 
  };
  app->cube_collider = nikola::physics_body_add_collider(app->cube_body, coll_desc);

  // Plane
  body_desc = {
    .position = nikola::Vec3(10.0f, -5.0f, 10.0f), 
    .type     = nikola::PHYSICS_BODY_STATIC, 
  };
  app->plane_body = nikola::physics_body_create(body_desc);

  // Plane collider
  coll_desc = {
    .position = nikola::Vec3(0.0f),
    .extents  = nikola::Vec3(64.0f, 1.0f, 64.0f), 
  };
  app->plane_collider = nikola::physics_body_add_collider(app->plane_body, coll_desc);
}

static void on_raycast_hit(const nikola::Ray& ray, const nikola::RayIntersection& info, const nikola::ColliderID& coll) {
  nikola::PhysicsBodyID body = nikola::collider_get_attached_body(coll);

  if(info.has_intersected && nikola::input_button_pressed(nikola::MOUSE_BUTTON_LEFT)) {
    nikola::physics_body_apply_force_at(body, ray.direction * 200.0f, info.point);
  }
}

/// Private functions 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App functions 

nikola::App* app_init(const nikola::Args& args, nikola::Window* window) {
  // App init
  nikola::App* app = new nikola::App{};
  nikola::renderer_set_clear_color(nikola::Vec4(0.1f, 0.1f, 0.1f, 1.0f));

  // Window init
  app->window = window;
  nikola::window_set_position(window, 100, 100);

  // Editor init
  nikola::gui_init(window);

  // Camera init
  nikola::CameraDesc cam_desc = {
    .position     = nikola::Vec3(10.0f, 0.0f, 10.0f),
    .target       = nikola::Vec3(-3.0f, 0.0f, 0.0f),
    .up_axis      = nikola::Vec3(0.0f, 1.0f, 0.0f),
    .aspect_ratio = nikola::window_get_aspect_ratio(app->window),
    .move_func    = nikola::camera_free_move_func,
  };
  nikola::camera_create(&app->frame_data.camera, cam_desc);

  // Resoruces init
  init_resources(app);

  // Lights init
  app->frame_data.dir_light.direction = nikola::Vec3(-1.0f, -1.0f, 1.0f);
  
  // Physics bodies init
  init_bodies(app);

  return app;
}

void app_shutdown(nikola::App* app) {
  nikola::resources_destroy_group(app->res_group_id);
  nikola::gui_shutdown();

  delete app;
}

void app_update(nikola::App* app, const nikola::f64 delta_time) {
  // Quit the application when the specified exit key is pressed
  if(nikola::input_key_pressed(nikola::KEY_ESCAPE)) {
    nikola::event_dispatch(nikola::Event{.type = nikola::EVENT_APP_QUIT});
    return;
  }

  // Disable/enable the GUI
  if(nikola::input_key_pressed(nikola::KEY_F1)) {
    app->has_editor                  = !app->has_editor;
    app->frame_data.camera.is_active = !app->has_editor;

    nikola::input_cursor_show(app->has_editor);
  }

  if(nikola::input_key_pressed(nikola::KEY_SPACE)) {
    nikola::physics_body_apply_force(app->cube_body, nikola::Vec3(0.0f, 300.0f, 0.0f));
  }

  // Raycast test
  nikola::Ray ray = {
    .position  = app->frame_data.camera.position, 
    .direction = app->frame_data.camera.front,
  };
  nikola::RayIntersection intersect = nikola::collider_check_raycast(app->cube_collider, ray); 
  nikola::physics_world_check_raycast(ray, on_raycast_hit);

  // Update the camera
  nikola::camera_update(app->frame_data.camera);
}

void app_render(nikola::App* app) {
  // Render 3D 
  nikola::renderer_begin(app->frame_data);

  nikola::Transform transform;

  transform = nikola::physics_body_get_transform(app->cube_body);
  nikola::transform_scale(transform, nikola::collider_get_extents(app->cube_collider));
  nikola::renderer_queue_mesh(app->mesh_id, transform);
  
  transform = nikola::physics_body_get_transform(app->plane_body);
  nikola::transform_scale(transform, nikola::collider_get_extents(app->plane_collider));
  nikola::renderer_queue_mesh(app->mesh_id, transform, app->material_id);

  // Debug stuff
  // nikola::renderer_debug_collider(app->cube_collider, nikola::Vec3(1.0f, 0.1f, 0.1f));
  // nikola::renderer_debug_collider(app->plane_collider, nikola::Vec3(0.1f, 1.0f, 0.1f));
 
  nikola::renderer_end();
  
  // Render 2D 
  nikola::batch_renderer_begin();
    
  nikola::i32 width, height; 
  nikola::window_get_size(app->window, &width, &height);

  nikola::batch_render_quad(nikola::Vec2(width, height) / 2.0f, nikola::Vec2(10.0f), nikola::Vec4(1.0f));
  nikola::batch_renderer_end();
}

void app_render_gui(nikola::App* app) {
  if(!app->has_editor) {
    return;
  }

  nikola::gui_begin();
  
  nikola::gui_begin_panel("Entities");

  nikola::gui_edit_material("Cube Material", nikola::resources_get_material(app->material_id));
  nikola::gui_edit_directional_light("Directional light", &app->frame_data.dir_light);

  nikola::gui_end_panel();

  nikola::gui_debug_info();
  nikola::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------
