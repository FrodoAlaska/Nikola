#include "app.h"

#include <nikola/nikola.h>
#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;
  nikola::FrameData frame_data;

  nikola::ResourceGroupID res_group_id;
  nikola::ResourceID mesh_id, font_id, material_id;

  nikola::Transform transform;

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
  app->frame_data.skybox_id = nikola::resources_push_skybox(app->res_group_id, "cubemaps/gloomy.nbr");

  // Mesh init
  app->mesh_id = nikola::resources_push_mesh(app->res_group_id, nikola::GEOMETRY_CUBE);

  // Font init
  app->font_id = nikola::resources_push_font(app->res_group_id, "fonts/bit5x3.nbr");

  // Material init
  nikola::MaterialDesc mat_desc = {
    .diffuse_id  = nikola::resources_push_texture(app->res_group_id, "textures/container_diffuse.nbr"),
    .specular_id = nikola::resources_push_texture(app->res_group_id, "textures/container_specular.nbr"),
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
    .position     = nikola::Vec3(10.0f, 0.0f, 10.0f),
    .target       = nikola::Vec3(-3.0f, 0.0f, 0.0f),
    .up_axis      = nikola::Vec3(0.0f, 1.0f, 0.0f),
    .aspect_ratio = nikola::window_get_aspect_ratio(app->window),
    .move_func    = nikola::camera_free_move_func,
  };
  nikola::camera_create(&app->frame_data.camera, cam_desc);

  // Resoruces init
  init_resources(app);

  // Transform init
  nikola::transform_translate(app->transform, nikola::Vec3(5.0f, 0.0f, 5.0f));
  nikola::transform_scale(app->transform, nikola::Vec3(1.0f));

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

  constexpr int MAX_CUBES = 100;

  nikola::Transform transforms[MAX_CUBES];
  for(int i = 0; i < 10; i++) {
    for(int j = 0; j < 10; j++) {
      int index = (i * 10) + j;

      nikola::transform_translate(transforms[index], nikola::Vec3(j * 2.0f, 0.0f, i * 2.0f));
      nikola::transform_scale(transforms[index], app->transform.scale);
    }
  }

  nikola::RenderInstanceCommand cmd = {
    .type       = nikola::RENDERABLE_MESH, 
    .transforms = transforms, 

    .renderable_id = app->mesh_id, 
    .material_id   = app->material_id,

    .instance_count = MAX_CUBES,
  };
  nikola::renderer_queue_command(cmd);
  
  nikola::renderer_end();
  
  // Render 2D 
  nikola::batch_renderer_begin();
  nikola::batch_render_fps(nikola::resources_get_font(app->font_id), nikola::Vec2(10.0f, 32.0f), 32.0f, nikola::Vec4(1.0f)); 
  nikola::batch_renderer_end();
}

void app_render_gui(nikola::App* app) {
  if(!app->has_editor) {
    return;
  }

  nikola::gui_begin();
  
  nikola::gui_debug_info();
  
  nikola::gui_begin_panel("Scene");
  
  nikola::gui_edit_transform("Mesh Transform", &app->transform);
  nikola::gui_edit_material("Material", nikola::resources_get_material(app->material_id));
  
  // Lights
  if(ImGui::CollapsingHeader("Lights")) {
    nikola::gui_edit_directional_light("Directional", &app->frame_data.dir_light);

    for(int i = 0; i < app->frame_data.point_lights.size(); i++) {
      nikola::PointLight* light = &app->frame_data.point_lights[i];
      nikola::String light_name = ("Point " + std::to_string(i));

      nikola::gui_edit_point_light(light_name.c_str(), light);
    }
  
    if(ImGui::Button("Add PointLight")) {
      app->frame_data.point_lights.push_back(nikola::PointLight{nikola::Vec3(10.0f, 0.0f, 10.0f)});
    }
  }

  // Camera
  if(ImGui::CollapsingHeader("Camera")) {
    nikola::gui_edit_camera("Editor Camera", &app->frame_data.camera); 
    ImGui::DragFloat3("Ambient", &app->frame_data.ambient[0], 0.1f, 0.0f, 1.0f);
  }

  // Renderer
  if(ImGui::CollapsingHeader("Renderer")) {
    static bool state_active = true;
    if(ImGui::Checkbox("HDR active", &state_active)) {
      nikola::renderer_pass_set_active(nikola::RENDER_PASS_HDR, state_active);
    }
  }

  nikola::gui_end_panel();
  
  nikola::gui_end();
}


/// App functions 
/// ----------------------------------------------------------------------
