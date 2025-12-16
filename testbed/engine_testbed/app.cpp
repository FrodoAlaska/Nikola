#include "app.h"

#include <nikola/nikola.h>
#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// Consts 

const nikola::sizei MAX_OBJECTS = 8;

/// Consts 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;
  nikola::FrameData frame_data;

  nikola::ResourceGroupID res_group_id;
  nikola::ResourceID mesh_id, model_id;
  nikola::ResourceID ground_material;

  nikola::Font* font;

  nikola::EntityWorld entt_world;
  nikola::EntityID entity, ground_entity;
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
  app->frame_data.skybox_id = nikola::resources_push_skybox(app->res_group_id, "cubemaps/Skybox.nbr");

  // Meshes init
  app->mesh_id = nikola::resources_push_mesh(app->res_group_id, nikola::GEOMETRY_CUBE);

  // Models init
  app->model_id = nikola::resources_push_model(app->res_group_id, "models/behelit.nbr"); 

  // Materials init
  
  nikola::MaterialDesc mat_desc = {
    .albedo_id = nikola::resources_push_texture(app->res_group_id, "textures/paviment.nbr"),
  };
  app->ground_material = nikola::resources_push_material(app->res_group_id, mat_desc);

  // Fonts init
  app->font = nikola::resources_get_font(nikola::resources_push_font(app->res_group_id, "fonts/bit5x3.nbr"));
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
    .position     = nikola::Vec3(10.0f, 5.0f, 10.0f),
    .target       = nikola::Vec3(-3.0f, 5.0f, 0.0f),
    .up_axis      = nikola::Vec3(0.0f, 1.0f, 0.0f),
    .aspect_ratio = nikola::window_get_aspect_ratio(app->window),
  };
  nikola::camera_create(&app->frame_data.camera, cam_desc);
  app->frame_data.camera.exposure = 1.0f;

  // Resoruces init
  init_resources(app);

  // Entities init

  app->ground_entity = nikola::entity_world_create_entity(app->entt_world, 
                                                          nikola::Vec3(10.0f, 0.5f, 10.0f), 
                                                          nikola::Quat(1.0f, 0.0f, 0.0f, 0.0f), 
                                                          nikola::Vec3(32.0f, 0.1f, 32.0f));
  nikola::entity_add_renderable(app->entt_world, app->ground_entity, nikola::ENTITY_RENDERABLE_MESH, app->mesh_id, app->ground_material);
 
  nikola::DynamicArray<nikola::Transform> transforms; 
  for(nikola::sizei i = 0; i < 16; i++) {
    nikola::Transform transform;
    transform.position = nikola::Vec3((i / 4) * 15.0f, 1.5f, (i % 4) * 15.0f);
    transform.scale    = nikola::Vec3(0.2f);
    nikola::transform_apply(transform);

    transforms.push_back(transform);
  }

  app->entity = nikola::entity_world_create_entity(app->entt_world, 
                                                   nikola::Vec3(15.0f, 1.5f, 15.0f), 
                                                   nikola::quat_identity(),
                                                   nikola::Vec3(0.2f));
  nikola::entity_add_instanced_renderable(app->entt_world, app->entity, nikola::ENTITY_RENDERABLE_MODEL, transforms, app->model_id);

  // Lights init

  app->frame_data.dir_light.direction = nikola::Vec3(1.0f, 1.0f, 2.5f);
  app->frame_data.dir_light.color     = nikola::Vec3(1.0f);

  app->frame_data.ambient = nikola::Vec3(1.0f);

  return app;
}

void app_shutdown(nikola::App* app) {
  nikola::entity_world_clear(app->entt_world);
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

  // Update the entity world
  nikola::entity_world_update(app->entt_world, delta_time);

  // Update the camera
  
  nikola::camera_free_move_func(app->frame_data.camera);
  nikola::camera_update(app->frame_data.camera);
}

void app_render(nikola::App* app) {
  nikola::renderer_begin(app->frame_data);

  // Render the entity world
  nikola::entity_world_render(app->entt_world);

  nikola::renderer_end();
  
  nikola::batch_renderer_begin();
  nikola::batch_render_fps(app->font, nikola::Vec2(30.0f, 40.0f), 32.0f, nikola::Vec4(1.0f, 0.0f, 0.0f, 1.0f)); 
  nikola::batch_renderer_end();
}

void app_render_gui(nikola::App* app) {
  if(!nikola::gui_is_active()) {
    return;
  }

  nikola::gui_begin();
  
  nikola::gui_set_window_flags(nikola::GUI_WINDOW_FLAGS_NO_COLLAPSE | nikola::GUI_WINDOW_FLAGS_NO_MOVE);

  ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f));
  nikola::gui_begin_panel("Scene");
  
  // Frame
  nikola::gui_edit_frame("Frame", &app->frame_data);

  // Resources
  
  if(ImGui::CollapsingHeader("Resources")) {
    nikola::gui_edit_material("Material", nikola::resources_get_material(app->ground_material));
  }

  nikola::gui_end_panel();
  nikola::gui_end();
}


/// App functions 
/// ----------------------------------------------------------------------
