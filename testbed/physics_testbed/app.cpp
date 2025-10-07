#include "app.h"

#include <nikola/nikola.h>
#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;
  nikola::FrameData frame_data;

  nikola::ResourceGroupID res_group_id;
  nikola::ResourceID mesh_id, cube_id;
  nikola::ResourceID materials[2];

  nikola::PhysicsBody* floor_body; 
  nikola::Character* cube_body;

  nikola::Vec3 velocity          = nikola::Vec3(0.0f);
  nikola::RenderPass* debug_pass = nullptr;
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
    .albedo_id =  nikola::resources_push_texture(app->res_group_id, "textures/grass.nbr"),
  };
  app->materials[0] = nikola::resources_push_material(app->res_group_id, mat);
 
  mat = {
    .color = nikola::Vec3(1.0f, 1.0f, 0.0f),
  };
  app->materials[1] = nikola::resources_push_material(app->res_group_id, mat);

  // Meshes init
  
  app->mesh_id = nikola::resources_push_mesh(app->res_group_id, nikola::GEOMETRY_SPHERE);
  app->cube_id = nikola::resources_push_mesh(app->res_group_id, nikola::GEOMETRY_CUBE);
}

static void init_bodies(nikola::App* app) {
  // Floor init

  nikola::BoxColliderDesc coll_desc = {
    .half_size = nikola::Vec3(32.0f, 0.1f, 32.0f),
  };

  nikola::PhysicsBodyDesc body_desc = {
    .position = nikola::Vec3(10.0f, -5.0f, 10.0f),
    .rotation = nikola::Quat(0.0f, 0.0f, 0.0f, 1.0f),

    .type   = nikola::PHYSICS_BODY_STATIC, 
    .layers = nikola::PHYSICS_OBJECT_LAYER_0,

    .collider = nikola::collider_create(coll_desc),
  };
  app->floor_body = nikola::physics_world_create_and_add_body(body_desc);

  // Cube init

  nikola::SphereColliderDesc sphere_coll_desc = {
    .radius = 1.0f,
  };

  nikola::CharacterBodyDesc char_desc = {
    .position = nikola::Vec3(10.0f, 5.0f, 10.0f),
    .rotation = nikola::Quat(0.0f, 0.0f, 0.0f, 1.0f),

    .layer = nikola::PHYSICS_OBJECT_LAYER_0,
    
    .collider  = nikola::collider_create(sphere_coll_desc),
  };

  app->cube_body = nikola::character_body_create(char_desc);
  nikola::physics_world_add_character(app->cube_body);
}

/// Private functions 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

static bool on_physics_event(const nikola::Event& event, const void* dispatcher, const void* listener) {
  nikola::App* app = (nikola::App*)listener;

  nikola::physics_world_set_safe_mode(false);

  // nikola::u64 user_data = nikola::physics_body_get_user_data(event.collision_data.body1_id);
  // nikola::Material* mat = nikola::resources_get_material(app->materials[user_data]);
  // mat->color = nikola::Vec3(1.0f, 0.0f, 0.0f);

  nikola::physics_world_set_safe_mode(true);
  return true;
}

static bool on_raycast_event(const nikola::Event& event, const void* dispatcher, const void* listener) {
  nikola::App* app = (nikola::App*)listener;

  nikola::physics_world_set_safe_mode(false);

  nikola::PhysicsBody* body  = event.cast_result.body;
  nikola::Vec3 body_position = nikola::physics_body_get_position(body);
  nikola::Vec3 hit_position  = event.cast_result.point;

  nikola::physics_body_apply_force(body, event.cast_result.ray_direction * 1500.0f);

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
  
  app->frame_data.dir_light.direction = nikola::Vec3(-5.0f, 1.0f, 1.0f);
  app->frame_data.dir_light.color     = nikola::Vec3(2.0f);
  app->frame_data.ambient             = nikola::Vec3(1.0f); 

  // Listen to events
  
  nikola::event_listen(nikola::EVENT_PHYSICS_CONTACT_ADDED, on_physics_event, app);
  nikola::event_listen(nikola::EVENT_PHYSICS_RAYCAST_HIT, on_raycast_event, app);

  app->debug_pass = nikola::renderer_peek_pass(nikola::RENDER_PASS_DEBUG);

  return app;
}

void app_shutdown(nikola::App* app) {
  nikola::physics_world_remove_and_destroy_body(&app->floor_body);

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

  static bool is_debug = false;
  if(nikola::input_key_pressed(nikola::KEY_F4)) {
    is_debug = !is_debug;

    if(is_debug) {
      nikola::renderer_insert_pass(app->debug_pass, nikola::RENDER_PASS_BILLBOARD);
    }
    else {
      nikola::renderer_remove_pass(nikola::RENDER_PASS_DEBUG);
    }
  }

  if(nikola::input_key_pressed(nikola::KEY_K)) {
    nikola::physics_world_remove_character(app->cube_body);
    nikola::character_body_destroy(&app->cube_body);
  }

  // Update the camera
  nikola::camera_update(app->frame_data.camera);

  // Handle input

  if(!nikola::character_body_is_valid(app->cube_body)) {
    return;
  }

  nikola::Vec3 current_velocity = nikola::character_body_get_linear_velocity(app->cube_body);
  app->velocity                 = nikola::Vec3(0.0f, current_velocity.y, 0.0f);

  if(nikola::input_key_down(nikola::KEY_W)) {
    app->velocity.x = 10.0f;
  }
  else if(nikola::input_key_down(nikola::KEY_S)) {
    app->velocity.x = -10.0f;
  }
  
  if(nikola::input_key_down(nikola::KEY_D)) {
    app->velocity.z = 10.0f;
  }
  else if(nikola::input_key_down(nikola::KEY_A)) {
    app->velocity.z = -10.0f;
  }

  if(nikola::input_key_pressed(nikola::KEY_SPACE)) {
    app->velocity.y = 5.0f;
  }

  if(nikola::input_key_pressed(nikola::KEY_P)) {
    nikola::RayCastDesc ray = {
      .origin    = app->frame_data.camera.position, 
      .direction = app->frame_data.camera.front, 
      .distance  = 1000000.0f, 
    };
    nikola::physics_world_cast_ray(ray);
  }

  // Update character
  
  nikola::character_body_set_linear_velocity(app->cube_body, app->velocity);
  nikola::character_body_update(app->cube_body);
}

void app_render(nikola::App* app) {
  // Render 3D 
  
  nikola::renderer_begin(app->frame_data);

  nikola::Transform transform;

  transform = nikola::physics_body_get_transform(app->floor_body);
  nikola::transform_scale(transform, nikola::Vec3(32.0f, 0.1f, 32.0f));
  nikola::renderer_queue_mesh(app->cube_id, transform, app->materials[0]);
 
  if(app->cube_body) {
    transform = nikola::character_body_get_transform(app->cube_body);
    nikola::transform_scale(transform, nikola::Vec3(1.0f));
    nikola::renderer_queue_mesh(app->cube_id, transform, app->materials[1]);
  }

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

    if(app->cube_body) {
      nikola::gui_edit_character_body("Cube", app->cube_body);
    }
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
