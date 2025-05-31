#include "app.h"

#include <nikola/nikola.h>
#include <imgui/imgui.h>
#include <q3.h>

/// ----------------------------------------------------------------------
/// CubeEntity
struct CubeEntity {
  nikola::PhysicsBody* body; 
  nikola::Collider* collider; 
  nikola::ResourceID cube_mesh;
  nikola::Transform transform;

  CubeEntity(const nikola::Vec3& pos, const nikola::ResourceID& cube_id) {
    // Body init
    nikola::PhysicsBodyDesc body_desc = {
      .position = pos, 
      .type     = nikola::PHYSICS_BODY_DYNAMIC,
    };
    body = nikola::physics_body_create(body_desc);

    // Collider init
    nikola::ColliderDesc coll_desc = {
      .position = nikola::Vec3(0.0f),
      .extents  = nikola::Vec3(1.0f),
    };
    collider = nikola::physics_body_add_collider(body, coll_desc);

    // Cube mesh init
    cube_mesh = cube_id; 

    // Transform init
    transform = nikola::physics_body_get_transform(body);
    nikola::transform_scale(transform, nikola::Vec3(1.0f));
  }

  void render() {
    transform = nikola::physics_body_get_transform(body);
    nikola::renderer_queue_mesh(cube_mesh, transform);
  }

  void render_gui(const nikola::String& name) {
    nikola::gui_edit_physics_body((name + " body").c_str(), body);
    nikola::gui_edit_collider((name + " collider").c_str(), collider);
  }
};
/// CubeEntity
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;
  nikola::FrameData frame_data;

  nikola::u16 res_group_id;
  nikola::ResourceID mesh_id, material_id;

  nikola::PhysicsBody* plane_body;
  nikola::Collider* plane_collider;

  nikola::DynamicArray<CubeEntity> cubes;

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
  // Plane
  nikola::PhysicsBodyDesc body_desc = {
    .position = nikola::Vec3(10.0f, -5.0f, 10.0f), 
    .type     = nikola::PHYSICS_BODY_STATIC, 
  };
  app->plane_body = nikola::physics_body_create(body_desc);

  // Plane collider
  nikola::ColliderDesc coll_desc = {
    .position = nikola::Vec3(0.0f),
    .extents  = nikola::Vec3(64.0f, 1.0f, 64.0f), 
  };
  app->plane_collider = nikola::physics_body_add_collider(app->plane_body, coll_desc);
}

static void on_raycast_hit(const nikola::Ray& ray, const nikola::RayIntersection& info, const nikola::Collider* coll) {
  nikola::PhysicsBody* body = nikola::collider_get_attached_body(coll);

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
    app->has_editor = !app->has_editor;
    nikola::input_cursor_show(app->has_editor);
  }

  if(nikola::input_key_pressed(nikola::KEY_S)) {
    app->cubes.emplace_back(app->frame_data.camera.position, app->mesh_id);
  }

  // Raycast test
  nikola::Ray ray = {
    .position  = app->frame_data.camera.position, 
    .direction = app->frame_data.camera.front,
  };
  nikola::physics_world_check_raycast(ray, on_raycast_hit);

  // Update the camera
  nikola::camera_update(app->frame_data.camera);
}

void app_render(nikola::App* app) {
  // Render 3D 
  nikola::renderer_begin(app->frame_data);

  // Plane render
  nikola::Transform transform = nikola::physics_body_get_transform(app->plane_body);
  nikola::transform_scale(transform, nikola::collider_get_extents(app->plane_collider));
  nikola::renderer_queue_mesh(app->mesh_id, transform, app->material_id);

  // Cubes render
  for(auto& cube : app->cubes) {
    cube.render();
  }
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

  // These flags are edited based on if the GUI
  // currently has focus or not
  app->frame_data.camera.is_active = !nikola::gui_is_focused();

  // Begin GUI frame
  nikola::gui_begin();
  
  nikola::gui_begin_panel("Lights");
  nikola::gui_edit_directional_light("Directional light", &app->frame_data.dir_light);
  nikola::gui_end_panel();
  
  nikola::gui_begin_panel("Entities");
  nikola::gui_edit_physics_body("Plane body", app->plane_body);
  nikola::gui_edit_collider("Plane collider", app->plane_collider);

  for(nikola::sizei i = 0; i < app->cubes.size(); i++) {
    app->cubes[i].render_gui(("Cube " + std::to_string(i))); 
  }
  nikola::gui_end_panel();

  nikola::gui_debug_info();

  nikola::gui_end();
  // End GUI frame
}

/// App functions 
/// ----------------------------------------------------------------------
