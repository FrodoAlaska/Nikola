#include "app.h"

#include <nikola/nikola.h>
#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;
  nikola::FrameData frame_data;
  nikola::ResourceGroupID res_group_id;

  nikola::ResourceID mesh_id, material_id;

  nikola::ResourceID model;
  nikola::ResourceID animation;
  
  nikola::Animator animator;
  nikola::Transform transforms[2];
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
  app->frame_data.skybox_id = nikola::resources_push_skybox(app->res_group_id, "cubemaps/gloomy.nbr");

  // Meshes init
  app->mesh_id = nikola::resources_push_model(app->res_group_id, "models/medieval_bridge.nbr");

  // Models init
  app->model = nikola::resources_push_model(app->res_group_id, "models/dancing_knight.nbr");

  // Animations init
  app->animation = nikola::resources_push_animation(app->res_group_id, "animations/dancing_knight.nbr");

  // Materials init
  
  nikola::MaterialDesc mat_desc = {
    .diffuse_id = nikola::resources_push_texture(app->res_group_id, "textures/paviment.nbr"),
    .normal_id  = nikola::resources_push_texture(app->res_group_id, "textures/paviment_normal.nbr"),
  };
  app->material_id = nikola::resources_push_material(app->res_group_id, mat_desc);
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

  // Frame init
  
  nikola::CameraDesc cam_desc = {
    .position     = nikola::Vec3(-40.0f, 7.0f, 28.0f),
    .target       = nikola::Vec3(-3.0f, 7.0f, 0.0f),
    .up_axis      = nikola::Vec3(0.0f, 1.0f, 0.0f),
    .aspect_ratio = nikola::window_get_aspect_ratio(app->window),
    .move_func    = nikola::camera_free_move_func,
  };
  nikola::camera_create(&app->frame_data.camera, cam_desc);
  app->frame_data.camera.exposure = 1.0f;

  nikola::File file; 
  nikola::i32 file_flags = (nikola::i32)(nikola::FILE_OPEN_READ | nikola::FILE_OPEN_BINARY);
  if(!nikola::file_open(&file, "scene_config.nkcfg", file_flags)) {
    NIKOLA_LOG_ERROR("Failed to load file from path");
  }
    
  nikola::file_read_bytes(file, &app->frame_data);
  nikola::file_close(file);

  // Resoruces init
  init_resources(app);

  // Animators init
  nikola::animator_create(&app->animator, app->animation);

  // Transform init
  
  nikola::transform_translate(app->transforms[0], nikola::Vec3(10.0f, 0.1f, 65.0f));
  nikola::transform_rotate(app->transforms[0], nikola::Vec3(1.0f, 0.0f, 0.0f), -90.0f * nikola::DEG2RAD);
  nikola::transform_scale(app->transforms[0], nikola::Vec3(10.0f));
  
  nikola::transform_translate(app->transforms[1], nikola::Vec3(10.0f, 11.0f, -21.4f));
  nikola::transform_scale(app->transforms[1], nikola::Vec3(0.8f));

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

  // Animators update
  nikola::animator_animate(app->animator, (nikola::f32)delta_time);

  // Update the camera
  nikola::camera_update(app->frame_data.camera);
}

void app_render(nikola::App* app) {
  // Render 3D 
  nikola::renderer_begin(app->frame_data);

  // Render the objects
  
  nikola::renderer_queue_model(app->mesh_id, app->transforms[0], app->material_id);
  nikola::renderer_queue_animation(app->animator.animation_id, app->model, app->transforms[1]);

  nikola::renderer_end();
  
  // Render 2D 
  
  // nikola::batch_renderer_begin();
  // nikola::batch_renderer_end();
}

void app_render_gui(nikola::App* app) {
  if(!nikola::gui_is_active()) {
    return;
  }

  nikola::gui_begin();

  nikola::gui_debug_info();
  nikola::gui_window_info();
  
  nikola::gui_begin_panel("Scene");

  // Entities
  
  if(ImGui::CollapsingHeader("Entities")) {
    nikola::gui_edit_transform("Ground", &app->transforms[0]);

    nikola::gui_edit_transform("Model 1", &app->transforms[1]);
    nikola::gui_edit_animator("Animator 1", &app->animator);
  }

  // Frame
  nikola::gui_edit_frame("Main scene", &app->frame_data);

  if(ImGui::Button("Save frame")) {
    nikola::File file; 
    nikola::i32 flags = (nikola::i32)(nikola::FILE_OPEN_WRITE | nikola::FILE_OPEN_BINARY);

    if(!nikola::file_open(&file, "scene_config.nkcfg", flags)) {
      NIKOLA_LOG_ERROR("Failed to save file to path");
      return;
    }

    nikola::file_write_bytes(file, app->frame_data);
    nikola::file_close(file);
  }

  nikola::gui_end_panel();
  
  nikola::gui_end();
}


/// App functions 
/// ----------------------------------------------------------------------
