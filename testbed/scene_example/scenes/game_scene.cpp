#include "game_scene.h"

#include <nikola/nikola.h>
#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// DirectionalLight 
struct DirectionalLight {
  nikola::Vec3 direction; 

  nikola::Vec3 ambient;
  nikola::Vec3 diffuse;
  nikola::Vec3 specular;

  DirectionalLight(const nikola::Vec3& dir) {
    direction = dir; 

    ambient  = nikola::Vec3(1.0f);
    diffuse  = nikola::Vec3(1.0f);
    specular = nikola::Vec3(1.0f);
  }

  void use(nikola::RenderCommand& cmd) {
    // nikola::shader_context_set_uniform(cmd.shader_context_id, "u_dir_light.direction", direction); 
    // nikola::shader_context_set_uniform(cmd.shader_context_id, "u_dir_light.ambient", ambient); 
    // nikola::shader_context_set_uniform(cmd.shader_context_id, "u_dir_light.diffuse", diffuse); 
    // nikola::shader_context_set_uniform(cmd.shader_context_id, "u_light.specular", specular); 
  }

  void render_gui(const char* name) {
    ImGui::SeparatorText(name); 
    ImGui::PushID(name); 

    ImGui::DragFloat3("Direction", &direction[0], 0.01f, -1.0f, 1.0f);
    ImGui::DragFloat3("Ambient", &ambient[0], 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat3("Diffuse", &diffuse[0], 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat3("Specular", &specular[0], 0.01f, 0.0f, 1.0f);

    ImGui::PopID(); 
  }
};

static nikola::DynamicArray<DirectionalLight> s_dir_lights;
/// DirectionalLight 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// PointLight 
struct PointLight {
  nikola::Vec3 position; 

  nikola::Vec3 ambient;
  nikola::Vec3 diffuse;
  nikola::Vec3 specular;

  nikola::f32 linear, quadratic;

  PointLight(const nikola::Vec3& pos) {
    position = pos; 

    ambient  = nikola::Vec3(1.0f);
    diffuse  = nikola::Vec3(1.0f);
    specular = nikola::Vec3(1.0f);

    linear    = 0.09f;
    quadratic = 0.032f;
  }

  void use(nikola::RenderCommand& cmd) {
    // nikola::shader_context_set_uniform(cmd.shader_context_id, "u_point_light.position", position); 

    // nikola::shader_context_set_uniform(cmd.shader_context_id, "u_point_light.ambient", ambient); 
    // nikola::shader_context_set_uniform(cmd.shader_context_id, "u_point_light.diffuse", diffuse); 
    // nikola::shader_context_set_uniform(cmd.shader_context_id, "u_light.specular", specular); 
   
    // nikola::shader_context_set_uniform(cmd.shader_context_id, "u_point_light.linear", linear); 
    // nikola::shader_context_set_uniform(cmd.shader_context_id, "u_point_light.quadratic", quadratic); 
  }

  void render_gui(const char* name) {
    ImGui::SeparatorText(name); 
    ImGui::PushID(name); 

    ImGui::DragFloat3("Position", &position[0], 1.0f);
    ImGui::DragFloat3("Ambient", &ambient[0], 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat3("Diffuse", &diffuse[0], 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat3("Specular", &specular[0], 0.01f, 0.0f, 1.0f);
    
    ImGui::DragFloat("Linear", &linear, 0.01f);
    ImGui::DragFloat("Quadratic", &quadratic, 0.01f);

    ImGui::PopID(); 
  }
};

static nikola::DynamicArray<PointLight> s_point_lights;
/// PointLight 
/// ----------------------------------------------------------------------

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
  s_dir_lights.emplace_back(nikola::Vec3(0.0f, 0.0f, -1.0f));
 
  // Point lights
  s_point_lights.emplace_back(nikola::Vec3(-1.0f, 0.0f, 10.0f));
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
  scene->skybox_id = nikola::resources_push_skybox(res_group, cubemap_id);

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
  
  // nikola::gui_begin_panel("Lights");
  // for(auto& light : s_dir_lights) {
  //   light.render_gui("Directional Light");
  // }
  // 
  // for(auto& light : s_point_lights) {
  //   light.render_gui("Point Light");
  // }
  // nikola::gui_end_panel();

  // nikola::gui_begin_panel("Entities");
  // for(auto& entt : s_entities) {
  //   entt.render_gui();
  // }
  // nikola::gui_end_panel();
  
  nikola::gui_begin_panel("Debug");
  nikola::gui_settings_debug();
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
