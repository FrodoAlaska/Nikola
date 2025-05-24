#include "app.h"

#include <nikola/nikola.h>
#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// Entity 
struct Entity {
  nikola::PhysicsBody* body; 
  nikola::ResourceID renderable_id;
  nikola::String name;

  Entity(const char* debug_name, const nikola::Vec3& pos, const nikola::ResourceID& res_id, const bool dynamic = true) 
    :name(debug_name), renderable_id(res_id) {
    body = nikola::physics_body_create(nikola::PhysicsBodyDesc {
      .position   = pos,
      .is_dynamic = dynamic,
    });
    nikola::physics_world_add_body(body);
  }

  void add_collider(const nikola::Vec3& size) {
    nikola::transform_scale(body->transform, size);
    nikola::physics_body_add_collider(body, size, false);
  }

  void render() {
    nikola::renderer_queue_mesh(renderable_id, body->transform);
  }

  void render_gui() {
    nikola::gui_edit_transform(name.c_str(), &body->transform);
  }
};
/// Entity 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;

  nikola::FrameData frame_data;
  nikola::u16 res_group;

  nikola::DynamicArray<Entity> entities;

  bool has_editor = false;
};

/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

static void init_resources(nikola::App* app) {
  // Resource group init
  nikola::FilePath res_path = nikola::filepath_append(nikola::filesystem_current_path(), "res");
  app->res_group            = nikola::resources_create_group("AudioApp_res", res_path);

  // Skybox init
  app->frame_data.skybox_id = nikola::resources_push_skybox(app->res_group, "cubemaps/Skybox.nbrcubemap");
}

/// Private functions 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App functions 

nikola::App* app_init(const nikola::Args& args, nikola::Window* window) {
  // App init
  nikola::App* app = new nikola::App{};
  nikola::renderer_set_clear_color(nikola::Vec4((16.0f / 255.0f), (14.0f / 255.0f), (28.0f / 255.0f), 1.0f));

  // Window init
  app->window = window;
  nikola::window_set_position(window, 100, 100);

  // Editor init
  nikola::gui_init(window);

  // Camera init
  float aspect_ratio = nikola::window_get_aspect_ratio(app->window);
  nikola::camera_create(&app->frame_data.camera, aspect_ratio, nikola::Vec3(10.0f, 0.0f, 10.0f), nikola::Vec3(-3.0f, 0.0f, 0.0f));

  // Resources init
  init_resources(app);

  // Mesh init
  nikola::ResourceID mesh_id = nikola::resources_push_mesh(app->res_group, nikola::MESH_TYPE_CUBE);

  // Entities init
  app->entities.emplace_back("Cube", nikola::Vec3(10.0f, 0.0f, 10.0f), mesh_id, true);
  app->entities[0].add_collider(nikola::Vec3(1.0f));

  app->entities.emplace_back("Ground", nikola::Vec3(10.0f, -10.0f, 10.0f), mesh_id, false);
  app->entities[1].add_collider(nikola::Vec3(20.0f, 0.1f, 20.0f));

  return app;
}

void app_shutdown(nikola::App* app) {
  nikola::gui_shutdown();
  nikola::resources_destroy_group(app->res_group);

  delete app;
}

static bool can_play_sound = false;

void app_update(nikola::App* app, const nikola::f64 delta_time) {
  // Close the window when `ESCAPE` is pressed
  if(nikola::input_key_down(nikola::KEY_ESCAPE)) {
    nikola::event_dispatch(nikola::Event{.type = nikola::EVENT_APP_QUIT});
    return;
  }

  if(nikola::input_key_pressed(nikola::KEY_E)) {
    app->has_editor                  = !app->has_editor;
    app->frame_data.camera.is_active = !app->has_editor;

    nikola::input_cursor_show(app->has_editor);
  }

  nikola::camera_update(app->frame_data.camera);
} 

void app_render(nikola::App* app) {
  // 3D renderer
  nikola::renderer_begin(app->frame_data);
  
  for(auto& entt : app->entities) {
    entt.render();
  }

  nikola::renderer_end();
  
  // 2D renderer 
  nikola::batch_renderer_begin();
  nikola::batch_renderer_end();
}

void app_render_gui(nikola::App* app) {
  if(!app->has_editor) {
    return;
  }
 
  nikola::gui_begin(); 

  // Physics
  nikola::gui_begin_panel("Scene");
  
  if(ImGui::CollapsingHeader("Lights")) {
    nikola::gui_edit_directional_light("Directional light", &app->frame_data.dir_light);
  }
  
  if(ImGui::CollapsingHeader("Entities")) {
    for(auto& entt : app->entities) {
      entt.render_gui();
    }
  }

  nikola::gui_end_panel();
  
  nikola::gui_end(); 
}

/// App functions 
/// ----------------------------------------------------------------------
