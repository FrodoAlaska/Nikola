#include "app.h"

#include <nikola/nikola.h>
#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;
  nikola::FrameData frame_data;

  nikola::u16 res_group_id;
  nikola::ResourceID mesh_id, material_id;

  nikola::Transform cube_transform, plane_transform;

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
  app->mesh_id = nikola::resources_push_mesh(app->res_group_id, nikola::MESH_TYPE_CUBE);
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

  // Editor init
  nikola::gui_init(window);

  // Camera init
  nikola::CameraDesc cam_desc = {
    .position     = nikola::Vec3(10.0f, 0.0f, 10.0f),
    .target       = nikola::Vec3(-3.0f, 0.0f, 0.0f),
    .up_axis      = nikola::Vec3(0.0f, 1.0f, 0.0f),
    .aspect_ratio = nikola::window_get_aspect_ratio(app->window),
    .move_func    = nikola::camera_fps_move_func,
  };
  nikola::camera_create(&app->frame_data.camera, cam_desc);

  // Resoruces init
  init_resources(app);

  // Lights init
  app->frame_data.dir_light.direction = nikola::Vec3(-1.0f, -1.0f, 1.0f);
  
  // Transforms init
  nikola::transform_translate(app->cube_transform, nikola::Vec3(10.0f, 0.0f, 10.0f));
  nikola::transform_scale(app->cube_transform, nikola::Vec3(1.0f));
  
  nikola::transform_translate(app->plane_transform, nikola::Vec3(10.0f, -5.0f, 10.0f));
  nikola::transform_scale(app->plane_transform, nikola::Vec3(128.0f, 0.1f, 128.0f));

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

  // Update the camera
  nikola::camera_update(app->frame_data.camera);
}

void app_render(nikola::App* app) {
  // Render 3D 
  nikola::renderer_begin(app->frame_data);
  nikola::renderer_queue_mesh(app->mesh_id, app->cube_transform);
  nikola::renderer_queue_mesh(app->mesh_id, app->plane_transform, app->material_id);
  nikola::renderer_end();
  
  // Render 2D 
  // nikola::batch_renderer_begin();
  // nikola::batch_render_texture(app->material->diffuse_map, nikola::Vec2(200.0f), nikola::Vec2(128.0f));
  // nikola::batch_renderer_end();
}

void app_render_gui(nikola::App* app) {
  if(!app->has_editor) {
    return;
  }

  nikola::gui_begin();
  
  nikola::gui_begin_panel("Entities");
  nikola::gui_edit_transform("Cube Transform", &app->cube_transform);
  nikola::gui_edit_material("Cube Material", nikola::resources_get_material(app->material_id));
  nikola::gui_end_panel();

  nikola::gui_debug_info();
  nikola::gui_end();
}

/// App functions 
/// ----------------------------------------------------------------------
