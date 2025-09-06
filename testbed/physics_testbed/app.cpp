#include "app.h"

#include <nikola/nikola.h>
#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;
  nikola::FrameData frame_data;

  nikola::ResourceGroupID res_group_id;
  nikola::ResourceID mesh_id;
  nikola::ResourceID materials[2];

  nikola::PhysicsBodyID floor_body; 
  nikola::ColliderID floor_collider; 

  nikola::PhysicsBodyID cube_body;
  nikola::ColliderID cube_collider;
};
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

static void init_resources(nikola::App* app) {
  // Resource storage init 
  nikola::FilePath res_path = nikola::filepath_append(nikola::filesystem_current_path(), "res");
  app->res_group_id = nikola::resources_create_group("app_res", res_path);

  // Skybox init
  app->frame_data.skybox_id = nikola::resources_push_skybox(app->res_group_id, "cubemaps/accurate_night.nbr");

  // Materials init
 
  nikola::MaterialDesc mat = {
    .diffuse_id =  nikola::resources_push_texture(app->res_group_id, "textures/grass.nbr"),
  };
  app->materials[0] = nikola::resources_push_material(app->res_group_id, mat);
 
  mat = {
    .color = nikola::Vec3(1.0f, 1.0f, 0.0f),
  };
  app->materials[1] = nikola::resources_push_material(app->res_group_id, mat);

  // Mesh init
  app->mesh_id = nikola::resources_push_mesh(app->res_group_id, nikola::GEOMETRY_CUBE);
}

static void init_bodies(nikola::App* app) {
  // Floor init

  nikola::BoxColliderDesc coll_desc = {
    .half_size = nikola::Vec3(32.0f, 1.0f, 32.0f) / 2.0f,
  };
  app->floor_collider = nikola::collider_create(coll_desc);

  nikola::PhysicsBodyDesc body_desc = {
    .position = nikola::Vec3(10.0f, -5.0f, 10.0f),
    .rotation = nikola::Quat(0.0f, 0.0f, 0.0f, 1.0f),

    .type   = nikola::PHYSICS_BODY_STATIC, 
    .layers = nikola::PHYSICS_OBJECT_LAYER_0,

    .collider_id = app->floor_collider,
    .user_data   = 0,
  };
  app->floor_body = nikola::physics_world_create_and_add_body(body_desc);

  // Cube init

  coll_desc = {
    .half_size = nikola::Vec3(3.0f) / 2.0f,
  };
  app->cube_collider = nikola::collider_create(coll_desc);

  body_desc = {
    .position = nikola::Vec3(10.0f, 5.0f, 10.0f),
    .rotation = nikola::Quat(0.0f, 0.0f, 0.0f, 1.0f),

    .type   = nikola::PHYSICS_BODY_DYNAMIC, 
    .layers = nikola::PHYSICS_OBJECT_LAYER_1,

    .collider_id = app->cube_collider,
    .user_data   = 1,
  };
  app->cube_body = nikola::physics_world_create_and_add_body(body_desc);
}

/// Private functions 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

static bool on_physics_event(const nikola::Event& event, const void* dispatcher, const void* listener) {
  nikola::App* app = (nikola::App*)listener;

  nikola::physics_world_set_safe_mode(false);

  nikola::u64 user_data = nikola::physics_body_get_user_data(event.collision_data.body1_id);
  nikola::Material* mat = nikola::resources_get_material(app->materials[user_data]);

  switch(event.type) {
    case nikola::EVENT_PHYSICS_CONTACT_ADDED:
      mat->color = nikola::Vec3(1.0f, 0.0f, 0.0f);
      break;
    case nikola::EVENT_PHYSICS_CONTACT_REMOVED:
      mat->color = nikola::Vec3(1.0f);
      break;
  }

  nikola::physics_world_set_safe_mode(true);
  return true;
}

static bool on_raycast_event(const nikola::Event& event, const void* dispatcher, const void* listener) {
  nikola::App* app = (nikola::App*)listener;

  nikola::physics_world_set_safe_mode(false);

  nikola::u64 user_data = nikola::physics_body_get_user_data(event.cast_result.body_id);
  nikola::Material* mat = nikola::resources_get_material(app->materials[user_data]);

  nikola::PhysicsBodyID body_id = event.cast_result.body_id;
  nikola::physics_body_apply_force(body_id, app->frame_data.camera.front * 100.0f);
  
  nikola::physics_world_set_safe_mode(true);
  return true;
}

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App functions 

nikola::App* app_init(const nikola::Args& args, nikola::Window* window) {
  // App init
  nikola::App* app = new nikola::App{};

  // Window init
  
  app->window = window;
  nikola::window_set_position(window, 100, 100);

  // Editor init
  nikola::gui_init(window);

  // Camera init
  
  nikola::CameraDesc cam_desc = {
    .position     = nikola::Vec3(-32.0f, 0.0f, 10.0f),
    .target       = nikola::Vec3(-3.0f, 0.0f, 0.0f),
    .up_axis      = nikola::Vec3(0.0f, 1.0f, 0.0f),
    .aspect_ratio = nikola::window_get_aspect_ratio(app->window),
    .move_func    = nikola::camera_free_move_func,
  };
  nikola::camera_create(&app->frame_data.camera, cam_desc);

  // Resoruces init
  init_resources(app);

  // Bodies init
  init_bodies(app);

  // Lights init
  
  app->frame_data.dir_light.direction = nikola::Vec3(-5.0f, -1.0f, 1.0f);
  app->frame_data.dir_light.color     = nikola::Vec3(2.0f);
  app->frame_data.ambient             = nikola::Vec3(1.0f); 

  // Listen to events
  
  nikola::event_listen(nikola::EVENT_PHYSICS_CONTACT_ADDED, on_physics_event, app);
  nikola::event_listen(nikola::EVENT_PHYSICS_CONTACT_REMOVED, on_physics_event, app);
  nikola::event_listen(nikola::EVENT_PHYSICS_RAYCAST_HIT, on_raycast_event, app);

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
    nikola::gui_toggle_active();
    app->frame_data.camera.is_active = !nikola::gui_is_active();
  }

  if(nikola::input_key_pressed(nikola::KEY_SPACE)) {
    // nikola::RayCastDesc ray = {
    //   .origin    = app->frame_data.camera.position, 
    //   .direction = app->frame_data.camera.front,
    //   .distance  = 100000.0f, 
    //
    //   .object_layer = nikola::PHYSICS_OBJECT_LAYER_1,
    // };
    // nikola::physics_world_cast_ray(ray);
  
    nikola::physics_body_apply_force(app->cube_body, nikola::Vec3(0.0f, 1.0f, 0.0f) * 100.0f);
  }

  // Update the camera
  nikola::camera_update(app->frame_data.camera);
}

void app_render(nikola::App* app) {
  // Render 3D 
  
  nikola::renderer_begin(app->frame_data);

  nikola::Transform transform;

  transform = nikola::physics_body_get_transform(app->floor_body);
  nikola::transform_scale(transform, nikola::Vec3(32.0f, 1.0f, 32.0f));
  nikola::renderer_queue_mesh(app->mesh_id, transform, app->materials[0]);
  
  transform = nikola::physics_body_get_transform(app->cube_body);
  nikola::transform_scale(transform, nikola::Vec3(1.0f));
  nikola::renderer_queue_mesh(app->mesh_id, transform, app->materials[1]);

  nikola::renderer_end();
  
  // Render 2D 
  
  nikola::batch_renderer_begin();
  
  nikola::i32 width, height; 
  nikola::window_get_size(app->window, &width, &height);

  nikola::batch_render_quad(nikola::Vec2(width, height) / 2.0f, nikola::Vec2(8.0f), nikola::Vec4(1.0f));
  nikola::batch_renderer_end();
}

void app_render_gui(nikola::App* app) {
  if(!nikola::gui_is_active()) {
    return;
  }

  nikola::gui_begin();
  nikola::gui_begin_panel("Scene");

  // Bodies
  if(ImGui::CollapsingHeader("Bodies")) {
    nikola::gui_edit_physics_body("Floor", app->floor_body);
    nikola::gui_edit_physics_body("Cube", app->cube_body);
  }

  // Frame 
  nikola::gui_edit_frame("Frame", &app->frame_data);

  // Debug
  nikola::gui_debug_info();
  
  nikola::gui_end_panel();
  nikola::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------
