#include "game_scene.h"

#include <nikola/nikola.h>
#include <cstdio>

/// ----------------------------------------------------------------------
/// Private functions

static void player_init(GameScene* scene, nikola::RenderCommand* cmd) {
  nikola::transform_translate(scene->transforms[0], nikola::Vec3(10.0f, 0.0f, 10.0f));
  nikola::transform_scale(scene->transforms[0], nikola::Vec3(1.0f));
  
  cmd->transform     = &scene->transforms[0]; 
  cmd->render_type   = nikola::RENDERABLE_TYPE_MODEL, 
  cmd->renderable_id = nikola::resources_get_id(scene->resource_group, "dice");
  nikola::render_queue_push(scene->render_queue, *cmd);
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

  // Shaders init
  nikola::resources_push_shader(res_group, "shaders/default3d.nbrshader");
  nikola::resources_push_shader(res_group, "shaders/cubemap.nbrshader");

  // Cubemaps init
  nikola::ResourceID gloomy_cm = nikola::resources_push_cubemap(res_group, "cubemaps/gloomy.nbrcubemap");

  // Models init
  nikola::resources_push_model(res_group, "models/dice.nbrmodel");

  // Shader contexts init
  nikola::ResourceID default_shader_context = nikola::resources_push_shader_context(res_group, nikola::resources_get_id(res_group, "default3d"));
  nikola::ResourceID skybox_shader_context  = nikola::resources_push_shader_context(res_group, nikola::resources_get_id(res_group, "cubemap"));

  // Materials init
  nikola::ResourceID default_material = nikola::resources_push_material(res_group);

  // Skyboxes init
  nikola::ResourceID skybox_id = nikola::resources_push_skybox(res_group, gloomy_cm);

  // Skybox render commands init
  nikola::RenderCommand skybox_render_cmd = {
    .render_type       = nikola::RENDERABLE_TYPE_SKYBOX, 
    .renderable_id     = skybox_id,
    .shader_context_id = skybox_shader_context,
  };
  nikola::render_queue_push(scene->render_queue, skybox_render_cmd);

  // Default render command init
  nikola::RenderCommand def_render_cmd = {
    .material_id       = default_material, 
    .shader_context_id = default_shader_context, 
  };

  // Transforms init
  scene->transforms.resize(6);
  player_init(scene, &def_render_cmd);
}

void game_scene_update(GameScene& scene) {
  if(nikola::input_key_pressed(nikola::KEY_E)) {
    scene.has_editor = !scene.has_editor;
    nikola::input_cursor_show(scene.has_editor);
  }

  if(scene.has_editor) {
    return;
  }

  nikola::camera_update(scene.camera);
}

void game_scene_render(GameScene& scene) {
  nikola::render_queue_flush(scene.render_queue);
}

void game_scene_gui_render(GameScene& scene) { 
  if(!scene.has_editor) {
    return;
  }
  
  nikola::gui_begin();
  
  nikola::gui_begin_panel("Debug");
  nikola::gui_settings_debug();
  nikola::gui_end_panel();

  nikola::gui_begin_panel("Transforms");
  nikola::gui_edit_transform("Dice", &scene.transforms[0]);
  nikola::gui_end_panel();
  
  nikola::gui_end();
}

void game_scene_shutdown(GameScene& scene) {
  nikola::resources_destroy_group(scene.resource_group);
}

/// GameScene functions 
/// ----------------------------------------------------------------------
