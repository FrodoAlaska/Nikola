#include "game_scene.h"

#include <nikola/nikola.h>
#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// Entity
struct Entity {
  nikola::Transform transform;
  nikola::ResourceID renderable;
  nikola::RenderableType type;
  nikola::String name;

  Entity(const nikola::Vec3& pos, const nikola::ResourceID& res_id, const nikola::RenderableType& render_type, const char* debug_name) {
    nikola::transform_translate(transform, pos);
    nikola::transform_scale(transform, nikola::Vec3(1.0f));

    renderable = res_id;
    type       = render_type;
    name       = debug_name;
  }

  void render(nikola::RenderQueue* queue, nikola::RenderCommand* cmd) {
    cmd->transform     = transform; 
    cmd->render_type   = type; 
    cmd->renderable_id = renderable;

    queue->push_back(*cmd);
  }

  void render_gui() {
    nikola::gui_edit_transform(name.c_str(), &transform); 
  }
};

static nikola::DynamicArray<Entity> s_entities;
/// Entity
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static void init_entities(GameScene* scene) {
  // Tempel
  nikola::ResourceID tempel_model = nikola::resources_get_id(scene->resource_group, "cottage_obj");
  s_entities.emplace_back(nikola::Vec3(10.0f, 0.0f, 10.0f), tempel_model, nikola::RENDERABLE_TYPE_MODEL, "Tempel"); 
  // nikola::transform_rotate(transform, nikola::Vec3(1.0f, 0.0f, 0.0f), -90.0f * nikola::DEG2RAD);
  
  // Ground
  // nikola::ResourceID ground_mesh = nikola::resources_push_mesh(scene->resource_group, nikola::MESH_TYPE_CUBE);
  // s_entities.emplace_back(nikola::Vec3(10.0f, 0.0f, 10.0f), ground_mesh, nikola::RENDERABLE_TYPE_MESH, "Ground"); 
}

static void init_lights(GameScene* scene) {
  // Directional light
  scene->frame_data.dir_light.direction = nikola::Vec3(0.0f, 0.0f, 0.0f);
 
  // Point lights
  scene->frame_data.point_lights.push_back(nikola::PointLight{nikola::Vec3(-1.0f, 0.0f, 10.0f)});
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// GameScene functions 

void game_scene_init(GameScene* scene, nikola::Window* window) {
  scene->window     = window;
  scene->has_editor = false;

  // Camera init
  float aspect_ratio = nikola::window_get_aspect_ratio(scene->window);
  nikola::camera_create(&scene->camera, aspect_ratio, nikola::Vec3(10.0f, 0.0f, 10.0f), nikola::Vec3(-3.0f, 0.0f, 0.0f));
  scene->frame_data.camera = scene->camera;

  // Resource group init 
  nikola::FilePath res_path = nikola::filepath_append(nikola::filesystem_current_path(), "res");
  scene->resource_group     = nikola::resources_create_group("game_res", res_path);
  nikola::u16 res_group     = scene->resource_group;

  // Cubemaps init
  nikola::ResourceID cubemap_id = nikola::resources_push_cubemap(res_group, "cubemaps/NightSky.nbrcubemap");

  // Models init
  nikola::resources_push_model(res_group, "models/cottage_obj.nbrmodel");

  // Materials init
  scene->material_id = nikola::resources_push_material(res_group);

  // Skyboxes init
  scene->skybox_id            = nikola::resources_push_skybox(res_group, cubemap_id);
  scene->frame_data.skybox_id = scene->skybox_id;

  // Entitites init
  init_entities(scene);

  // Lights init
  init_lights(scene);
}

void game_scene_update(GameScene& scene) {
  // Enable editor
  if(nikola::input_key_pressed(nikola::KEY_E)) {
    scene.has_editor = !scene.has_editor;
    nikola::input_cursor_show(scene.has_editor);
  }

  // Enable fullscreen
  if(nikola::input_key_pressed(nikola::KEY_F)) {
    nikola::window_set_fullscreen(scene.window, !nikola::window_is_fullscreen(scene.window));
  }

  if(scene.has_editor) {
    return;
  }

  nikola::camera_update(scene.camera);
  scene.frame_data.camera = scene.camera;
}

void game_scene_render(GameScene& scene) {
  nikola::RenderCommand render_cmd{};
  render_cmd.material_id = scene.material_id;

  // Render entities 
  for(auto& entt : s_entities) {
    entt.render(&scene.render_queue, &render_cmd);
  }
  nikola::renderer_sumbit_queue(scene.render_queue);
}

void game_scene_gui_render(GameScene& scene) { 
  if(!scene.has_editor) {
    return;
  }
  
  nikola::gui_begin_panel("Lights");
  nikola::gui_edit_directional_light("Directional", &scene.frame_data.dir_light);
  for(auto& light : scene.frame_data.point_lights) {
    nikola::gui_edit_point_light("Point", &light);
  }
  nikola::gui_end_panel();

  // nikola::gui_begin_panel("Entities");
  // for(auto& entt : s_entities) {
  //   entt.render_gui();
  // }
  // nikola::gui_end_panel();
  
  nikola::gui_begin_panel("Debug");
  ImGui::Combo("Render Effect", 
               &scene.render_effect, 
               "None\0Greyscale\0Inversion\0Sharpen\0Blur\0Emboss\0Edge Detection\0Pixelize\0");  
  // -------------------------------------------------------------------
  nikola::gui_end_panel();
}

void game_scene_shutdown(GameScene& scene) {
  nikola::resources_destroy_group(scene.resource_group);
}

/// GameScene functions 
/// ----------------------------------------------------------------------
