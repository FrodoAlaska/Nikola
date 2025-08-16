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

  nikola::ResourceID model1, model2;
  nikola::ResourceID animation1, animation2;
  
  nikola::Animator animator1, animator2;
  nikola::Transform transforms[3];

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

  // Skybox init
  app->frame_data.skybox_id = nikola::resources_push_skybox(app->res_group_id, "cubemaps/accurate_night.nbr");

  // Meshes init
  app->mesh_id = nikola::resources_push_mesh(app->res_group_id, nikola::GEOMETRY_CUBE);

  // Models init
  app->model1 = nikola::resources_push_model(app->res_group_id, "models/zombie_idle.nbr");

  // Animations init

  app->animation1 = nikola::resources_push_animation(app->res_group_id, "animations/zombie_idle.nbr");
  app->animation2 = nikola::resources_push_animation(app->res_group_id, "animations/zombie_walk.nbr");

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

  // Camera init
  nikola::CameraDesc cam_desc = {
    .position     = nikola::Vec3(-40.0f, 7.0f, 28.0f),
    .target       = nikola::Vec3(-3.0f, 7.0f, 0.0f),
    .up_axis      = nikola::Vec3(0.0f, 1.0f, 0.0f),
    .aspect_ratio = nikola::window_get_aspect_ratio(app->window),
    .move_func    = nikola::camera_free_move_func,
  };
  nikola::camera_create(&app->frame_data.camera, cam_desc);
  app->frame_data.camera.exposure = 1.0f;

  // Resoruces init
  init_resources(app);

  // Animators init

  nikola::animator_create(&app->animator1, app->animation1);
  nikola::animator_create(&app->animator2, app->animation2);

  // Transform init
  
  nikola::transform_translate(app->transforms[0], nikola::Vec3(5.0f, 0.05f, 5.0f));
  nikola::transform_scale(app->transforms[0], nikola::Vec3(16.0f, 1.0f, 16.0f));
  
  nikola::transform_translate(app->transforms[1], nikola::Vec3(10.0f, 1.0f, 10.0f));
  nikola::transform_rotate(app->transforms[1], nikola::Vec3(1.0f, 0.0f, 0.0f), 90.0f * nikola::DEG2RAD);
  nikola::transform_scale(app->transforms[1], nikola::Vec3(0.3f));
  
  nikola::transform_translate(app->transforms[2], nikola::Vec3(20.0f, 1.0f, 10.0f));
  nikola::transform_rotate(app->transforms[2], nikola::Vec3(1.0f, 0.0f, 0.0f), 90.0f * nikola::DEG2RAD);
  nikola::transform_scale(app->transforms[2], nikola::Vec3(0.3f));

  // Lights init

  app->frame_data.dir_light.direction = nikola::Vec3(0.0f);
  app->frame_data.dir_light.color     = nikola::Vec3(0.0f);

  nikola::SpotLight spot_light(app->frame_data.camera.position, 
                               app->frame_data.camera.front, 
                               nikola::Vec3(1.0f), 
                               0.0f, 
                               1.0f);
  app->frame_data.spot_lights.push_back(spot_light);

  app->frame_data.ambient = nikola::Vec3(0.0f);

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

  // Animators update

  nikola::animator_animate(app->animator1, (nikola::f32)delta_time);

  // Update the camera
  
  nikola::SpotLight* spot_light = &app->frame_data.spot_lights[0]; 

  spot_light->position  = app->frame_data.camera.position;
  spot_light->direction = app->frame_data.camera.front;
  
  nikola::camera_update(app->frame_data.camera);
}

void app_render(nikola::App* app) {
  // Render 3D 
  nikola::renderer_begin(app->frame_data);

  // Render the objects
  
  nikola::renderer_queue_mesh(app->mesh_id, app->transforms[0], app->material_id);
  nikola::renderer_queue_animation(app->animator1.animation_id, app->model1, app->transforms[1]);

  nikola::renderer_end();
  
  // Render 2D 
  
  // nikola::batch_renderer_begin();
  // nikola::batch_renderer_end();
}

void app_render_gui(nikola::App* app) {
  if(!app->has_editor) {
    return;
  }

  nikola::gui_begin();
  
  nikola::gui_debug_info();
  
  nikola::gui_begin_panel("Scene");
 
  // Entities
  if(ImGui::CollapsingHeader("Entities")) {
    nikola::gui_edit_transform("Ground", &app->transforms[0]);
    nikola::gui_edit_transform("Model 1", &app->transforms[1]);
    nikola::gui_edit_transform("Model 2", &app->transforms[2]);
  }
  
  // Lights
  if(ImGui::CollapsingHeader("Lights")) {
    nikola::gui_edit_directional_light("Directional", &app->frame_data.dir_light);

    for(int i = 0; i < app->frame_data.point_lights.size(); i++) {
      nikola::PointLight* light = &app->frame_data.point_lights[i];
      nikola::String light_name = ("Point " + std::to_string(i));

      nikola::gui_edit_point_light(light_name.c_str(), light);
    }
    
    for(int i = 0; i < app->frame_data.spot_lights.size(); i++) {
      nikola::SpotLight* light = &app->frame_data.spot_lights[i];
      nikola::String light_name = ("Spot " + std::to_string(i));

      nikola::gui_edit_spot_light(light_name.c_str(), light);
    }
 
    ImGui::Separator();
    if(ImGui::Button("Add PointLight")) {
      nikola::Vec3 point_pos = nikola::Vec3(10.0f, 5.0f, 10.0f);
      app->frame_data.point_lights.push_back(nikola::PointLight(point_pos));
    }
    
    if(ImGui::Button("Add SpotLight")) {
      nikola::Vec3 spot_pos = nikola::Vec3(10.0f, 5.0f, 10.0f);
      app->frame_data.spot_lights.push_back(nikola::SpotLight());
    }
  }

  // Camera
  if(ImGui::CollapsingHeader("Camera")) {
    nikola::gui_edit_camera("Editor Camera", &app->frame_data.camera); 
    ImGui::DragFloat3("Ambient", &app->frame_data.ambient[0], 0.1f, 0.0f, 1.0f);
  }

  nikola::gui_end_panel();
  
  nikola::gui_end();
}


/// App functions 
/// ----------------------------------------------------------------------
