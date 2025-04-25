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

  void serialize(nikola::File& file) {
    nikola::file_write_bytes(file, transform);
  }
  
  void deserialize(nikola::File& file) {
    nikola::file_read_bytes(file, &transform);
  }
};

static nikola::DynamicArray<Entity> s_entities;
/// Entity
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static void init_entities(GameScene* scene) {
  // Player
  nikola::ResourceID model = nikola::resources_get_id(scene->resource_group, "tempel");
  s_entities.emplace_back(nikola::Vec3(10.0f, 0.0f, 10.0f), model, nikola::RENDERABLE_TYPE_MODEL, "Tempel"); 
  
  // Cube
  nikola::ResourceID cube_mesh = nikola::resources_push_mesh(scene->resource_group, nikola::MESH_TYPE_CUBE);
  s_entities.emplace_back(nikola::Vec3(10.0f, 0.0f, 10.0f), cube_mesh, nikola::RENDERABLE_TYPE_MESH, "Cube"); 
}

static void init_lights(GameScene* scene) {
  // Directional light
  scene->frame_data.dir_light.direction = nikola::Vec3(0.0f, 0.0f, 0.0f);
  scene->frame_data.dir_light.ambient   = nikola::Vec3(0.0f, 0.0f, 0.0f);
 
  // Point lights
  scene->frame_data.point_lights.push_back(nikola::PointLight{nikola::Vec3(10.0f, 0.0f, 10.0f)});
}

static void game_scene_serialize(GameScene& scene) {
  const char* path = "game_scene.nscn";
  nikola::File file;

  if(!nikola::file_open(&file, path, (int)(nikola::FILE_OPEN_WRITE | nikola::FILE_OPEN_BINARY))) {
    NIKOLA_LOG_ERROR("Cannot open Scene file at \'%s\'", path);
    return;
  }

  // Save the camera
  nikola::file_write_bytes(file, scene.frame_data.camera);

  // Save the directional light
  nikola::file_write_bytes(file, scene.frame_data.dir_light);

  // Save the point lights
  for(auto& light : scene.frame_data.point_lights) {
    nikola::file_write_bytes(file, light);
  }

  // Save the entities 
  for(auto& entt : s_entities) {
    entt.serialize(file);
  }

  nikola::file_close(file); 
}

static void game_scene_deserialize(GameScene& scene) {
  nikola::FilePath path = nikola::filepath_append(nikola::filesystem_current_path(), "game_scene.nscn");
  nikola::File file;

  if(!nikola::file_open(&file, path, (int)(nikola::FILE_OPEN_READ | nikola::FILE_OPEN_BINARY))) {
    NIKOLA_LOG_ERROR("Cannot open Scene file at \'%s\'", path.c_str());
    return;
  }

  // Load the camera
  nikola::file_read_bytes(file, &scene.frame_data.camera);

  // Load the directional light
  nikola::file_read_bytes(file, &scene.frame_data.dir_light);

  // Load the point lights
  for(auto& light : scene.frame_data.point_lights) {
    nikola::file_read_bytes(file, &light);
  }

  // Load the entities 
  for(auto& entt : s_entities) {
    entt.deserialize(file);
  }

  nikola::file_close(file);
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
  nikola::camera_create(&scene->frame_data.camera, aspect_ratio, nikola::Vec3(10.0f, 0.0f, 10.0f), nikola::Vec3(-3.0f, 0.0f, 0.0f));

  // Resource group init 
  nikola::FilePath res_path = nikola::filepath_append(nikola::filesystem_current_path(), "res");
  scene->resource_group     = nikola::resources_create_group("game_res", res_path);
  nikola::u16 res_group     = scene->resource_group;

  // Cubemaps init
  nikola::ResourceID cubemap_id = nikola::resources_push_cubemap(res_group, "cubemaps/gloomy.nbrcubemap");

  // Models init
  nikola::resources_push_model(res_group, "models/tempel.nbrmodel");

  // Textures init
  nikola::ResourceID mesh_texture = nikola::resources_push_texture(res_group, "textures/opengl.nbrtexture");

  // Materials init
  scene->material_id = nikola::resources_push_material(res_group);
  nikola::material_set_texture(scene->material_id, nikola::MATERIAL_TEXTURE_DIFFUSE, mesh_texture);

  // Skyboxes init
  scene->skybox_id            = nikola::resources_push_skybox(res_group, cubemap_id);
  scene->frame_data.skybox_id = scene->skybox_id;

  // Entitites init
  init_entities(scene);

  // Lights init
  init_lights(scene);

  // Loading the binary scene
  game_scene_deserialize(*scene);
}

static nikola::f32 rotation = 0.0f;

void game_scene_update(GameScene& scene, nikola::f64 dt) {
  // Enable fullscreen
  if(nikola::input_key_pressed(nikola::KEY_F)) {
    nikola::window_set_fullscreen(scene.window, !nikola::window_is_fullscreen(scene.window));
  }
  
  // Enable editor
  if(nikola::input_key_pressed(nikola::KEY_E)) {
    scene.has_editor                  = !scene.has_editor;
    scene.frame_data.camera.is_active = !scene.has_editor;

    nikola::input_cursor_show(scene.has_editor);
  }

  nikola::camera_update(scene.frame_data.camera);
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
  
  ImGui::Combo("Render Effect", 
               &scene.render_effect, 
               "None\0Greyscale\0Inversion\0Sharpen\0Blur\0Emboss\0Edge Detection\0Pixelize\0");  
  nikola::gui_end_panel();
  
  nikola::gui_begin_panel("Resources");
  nikola::Material* material = nikola::resources_get_material(scene.material_id);
  nikola::gui_edit_material("Default Material", material);
  nikola::gui_end_panel();

  nikola::gui_begin_panel("Entities");
  for(auto& entt : s_entities) {
    entt.render_gui();
  }
  
  nikola::gui_edit_camera("Editor Camera", &scene.frame_data.camera); 

  if(ImGui::Button("Save Scene")) {
    game_scene_serialize(scene);
  }
  nikola::gui_end_panel();
  
  nikola::gui_debug_info();
}

void game_scene_shutdown(GameScene& scene) {
  nikola::resources_destroy_group(scene.resource_group);
}

/// GameScene functions 
/// ----------------------------------------------------------------------
