#include "app.hpp"
#include "shaders.hpp"

#include <nikola/nikola_core.hpp>
#include <nikola/nikola_engine.hpp>
#include <nikola/nikola_ui.hpp>

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;
  nikola::Camera camera;

  bool has_editor;
  nikola::Transform transform, light_transform;

  nikola::ResourceStorage* storage;
  nikola::HashMap<nikola::String, nikola::ResourceID> resources;

  nikola::ResourceID mesh_id, material_id;
  nikola::ResourceID skybox_id, skybox_material_id;
  nikola::ResourceID model_id;
};
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

static void render_app_ui(nikola::App* app) {
  if(!app->has_editor) {
    return;
  }

  nikola::gui_begin();

  // nikola::gui_begin_panel("Debug");
  // nikola::gui_settings_debug();
  // nikola::gui_end_panel();
  
  // nikola::gui_begin_panel("Camera");
  // nikola::gui_settings_camera(&app->camera);
  // nikola::gui_end_panel();
  
  nikola::gui_begin_panel("Renderer");
  nikola::gui_settings_renderer();
  nikola::gui_end_panel();
  
  // nikola::gui_begin_panel("Resources");
  // nikola::gui_settings_material("Material", app->material_id);
  // nikola::gui_end_panel();

  nikola::gui_begin_panel("Transforms");
  nikola::gui_settings_transform("Transform", &app->transform);
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

  // Transform init
  nikola::transform_translate(app->transform, nikola::Vec3(10.0f, 0.0f, 10.0f));
  nikola::transform_scale(app->transform, nikola::Vec3(0.1f));
 
  // Light transform init
  nikola::transform_translate(app->light_transform, nikola::Vec3(0.0f, 20.0f, 0.0f));

  // Resource storage init 
  nikola::FilePath current_path = nikola::filesystem_current_path();
  nikola::FilePath res_path = nikola::filepath_append(current_path, "res");
  app->storage = nikola::resource_storage_create("app_res", res_path);

  // Transform init
  nikola::transform_translate(app->transform, nikola::Vec3(10.0f, 0.0f, 10.0f));
  nikola::transform_scale(app->transform, nikola::Vec3(0.1f));

  // Mesh init
  app->mesh_id = nikola::resource_storage_push_mesh(app->storage, nikola::MESH_TYPE_CUBE);

  // Resoruces init
  nikola::resource_storage_push_dir(app->storage, &app->resources, "textures");
  nikola::resource_storage_push_dir(app->storage, &app->resources, "shaders");
  nikola::resource_storage_push_dir(app->storage, &app->resources, "cubemaps");

  // Skybox init
  app->skybox_id = nikola::resource_storage_push_skybox(app->storage, app->resources["NightSky"]);

  // Material init
  app->material_id = nikola::resource_storage_push_material(app->storage, app->resources["logo"], nikola::ResourceID{}, app->resources["default3d"]);

  // Skybox material init
  app->skybox_material_id = nikola::resource_storage_push_material(app->storage, app->resources["logo"], nikola::ResourceID{}, app->resources["cubemap"]);

  // Model init
  app->model_id = nikola::resource_storage_push_model(app->storage, "models\\behelit.nbrmodel");

  return app;
}

void app_shutdown(nikola::App* app) {
  nikola::resource_storage_destroy(app->storage);
  nikola::gui_shutdown();

  delete app;
}

void app_update(nikola::App* app) {
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

static float rotation_angle = 0.0f;

void app_render(nikola::App* app) {
  // Begin rendering objects
  nikola::RenderData render_dat = {
    .camera = app->camera,
  };
  nikola::renderer_begin_pass(render_dat);

  nikola::RenderCommand rnd_cmd;
  rnd_cmd.material_id = app->material_id;
  rnd_cmd.transform   = app->transform; 

  // Render the cubes
  // rnd_cmd.render_type   = nikola::RENDERABLE_TYPE_MESH, 
  // rnd_cmd.renderable_id = app->mesh_id,
  // constexpr int MESHES_MAX = 10;
  //
  // for(int i = 0; i < MESHES_MAX; i++) {
  //   for(int j = 0; j < MESHES_MAX; j++) {
  //     nikola::transform_translate(app->transform, nikola::Vec3(i * 2.0f, 0.0f, j * 2.0f));
  //     
  //     rnd_cmd.transform = app->transform;
  //     nikola::renderer_queue_command(rnd_cmd);
  //   }
  // }

  // nikola::transform_translate(app->light_transform, nikola::Vec3());

  // Render the model
  rnd_cmd.render_type   = nikola::RENDERABLE_TYPE_MODEL; 
  rnd_cmd.renderable_id = app->model_id; 
  nikola::renderer_queue_command(rnd_cmd);
  
  // Render the skybox 
  rnd_cmd.render_type   = nikola::RENDERABLE_TYPE_SKYBOX; 
  rnd_cmd.renderable_id = app->skybox_id; 
  rnd_cmd.material_id   = app->skybox_material_id; 
  nikola::renderer_queue_command(rnd_cmd);

  // Render the objects
  nikola::renderer_end_pass();
  
  // Render UI
  render_app_ui(app); 

  // Render 2D 
  nikola::batch_renderer_begin();

  // nikola::batch_render_quad(nikola::Vec2(100.0f), nikola::Vec2(128.0f), nikola::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
  // nikola::batch_render_texture(app->material->diffuse_map, nikola::Vec2(200.0f), nikola::Vec2(128.0f));

  nikola::batch_renderer_end();

  // End of the render loop...
  nikola::renderer_present();
}

/// App functions 
/// ----------------------------------------------------------------------
