#include "app.hpp"

#include <nikola/nikola.h>

#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;
  nikola::Camera camera;
  nikola::u16 res_group_id;

  nikola::RenderQueue render_queue;

  nikola::ResourceID material_id, shader_context_id;
  nikola::ResourceID skybox_id, model_id;

  nikola::Transform model_transform;

  bool has_editor;
};
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

static void init_resources(nikola::App* app) {
}

static void render_app_ui(nikola::App* app) {
  if(!app->has_editor) {
    return;
  }

  nikola::gui_begin();

  nikola::gui_begin_panel("Transform");
  nikola::gui_edit_transform("Model", &app->model_transform);
  nikola::gui_end_panel();

  nikola::gui_end();
}

/// Private functions 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App functions 

nikola::App* app_init(const nikola::Args& args, nikola::Window* window) {
  // App init
  nikola::App* app = new nikola::App{};

  // Window init
  app->window = window;

  // Editor init
  nikola::gui_init(window);
  app->has_editor = false;

  // Camera init
  float aspect_ratio = nikola::window_get_aspect_ratio(app->window);
  nikola::camera_create(&app->camera, aspect_ratio, nikola::Vec3(10.0f, 0.0f, 10.0f), nikola::Vec3(-3.0f, 0.0f, 0.0f));

  // Resource storage init 
  nikola::FilePath current_path = nikola::filesystem_current_path();
  nikola::FilePath res_path = nikola::filepath_append(current_path, "res");
  app->res_group_id = nikola::resources_create_group("app_res", res_path);

  // Resoruces init
  nikola::resources_push_dir(app->res_group_id, "models");
  nikola::resources_push_dir(app->res_group_id, "shaders");

  // Models init
  app->model_id = nikola::resources_get_id(app->res_group_id, "result200k");
  
  // Materials init
  app->material_id = nikola::resources_push_material(app->res_group_id);

  // Shader contexts init
  app->shader_context_id = nikola::resources_push_shader_context(app->res_group_id, nikola::resources_get_id(app->res_group_id, "default3d"));

  // Default transform init
  nikola::transform_translate(app->model_transform, nikola::Vec3(10.0f, 0.0f, 10.0f));
  nikola::transform_scale(app->model_transform, nikola::Vec3(1.0f));

  return app;
}

void app_shutdown(nikola::App* app) {
  nikola::resources_destroy_group(app->res_group_id);
  nikola::gui_shutdown();

  delete app;
}

void app_update(nikola::App* app, const nikola::f64 delta_time) {
  // Close the window when `ESCAPE` is pressed
  if(nikola::input_key_down(nikola::KEY_ESCAPE)) {
    nikola::event_dispatch(nikola::Event{.type = nikola::EVENT_APP_QUIT});
    return;
  }

  // Active/deactive the editor
  if(nikola::input_key_pressed(nikola::KEY_F1)) {
    app->has_editor = !app->has_editor;
    nikola::input_cursor_show(app->has_editor);
  }

  if(!app->has_editor) {
    nikola::camera_update(app->camera);
  }
} 

void app_render(nikola::App* app) {
  nikola::renderer_begin(app->camera);

  nikola::RenderCommand rnd_cmd;
  rnd_cmd.render_type       = nikola::RENDERABLE_TYPE_MODEL; 
  rnd_cmd.renderable_id     = app->model_id; 
  rnd_cmd.material_id       = app->material_id; 
  rnd_cmd.shader_context_id = app->shader_context_id; 
  rnd_cmd.transform         = app->model_transform; 
  nikola::render_queue_push(app->render_queue, rnd_cmd);
  nikola::render_queue_flush(app->render_queue);

  // Render UI
  render_app_ui(app); 

  nikola::renderer_end();
}

/// App functions 
/// ----------------------------------------------------------------------
