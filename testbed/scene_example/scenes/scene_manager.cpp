#include "scene_manager.h"

#include <nikola/nikola.h>

///---------------------------------------------------------------------------------------------------------------------
/// Macros

#define SCENE_CALLBACK(func, scene, ...) if(scene->desc.func) scene->desc.func(scene, ##__VA_ARGS__);

/// Macros
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// SceneManager 
struct SceneManager {
  nikola::Window* window; 

  nikola::HashMap<const char*, Scene*> scenes;
  Scene* current_scene;
};

static SceneManager s_manager;
/// SceneManager 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// SceneManager functions

void scenes_init(nikola::Window* window) {
  s_manager.window        = window;
  s_manager.current_scene = nullptr;
  
  NIKOLA_LOG_INFO("Successfully initialized the scene manager");
}

void scenes_shutdown() {
  for(auto& [key, value] : s_manager.scenes) {
    SCENE_CALLBACK(destroy_func, value);
    delete value;
  }
  s_manager.scenes.clear();
  
  NIKOLA_LOG_INFO("Successfully shutdown the scene manager");
}

void scenes_push_scene(const char* name, const SceneDesc& desc) {
  Scene* scene = new Scene{};
  
  // Default values init
  scene->desc   = desc; 
  scene->name   = name;
  scene->window = s_manager.window;

  // Resource group init 
  nikola::FilePath res_path = nikola::filepath_append(nikola::filesystem_current_path(), "res");
  scene->resource_group     = nikola::resources_create_group(name, res_path);

  // Create and add a new scene 
  SCENE_CALLBACK(create_func, scene);
  s_manager.scenes[name] = scene;
  NIKOLA_LOG_INFO("Successfully created \'%s\' scene", name);

  // Set the current scene to the latest pushed scene
  scenes_set_current(name);
}

void scenes_set_current(const char* scene_name) {
  NIKOLA_LOG_TRACE("Current scene set to \'%s\'", scene_name);

  s_manager.current_scene = s_manager.scenes[scene_name];
}

Scene* scenes_get_current() {
  return s_manager.current_scene;
}

Scene* scenes_get_scene(const char* scene_name) {
  return s_manager.scenes[scene_name];
}

void scenes_update(const nikola::f64 dt) {
  SCENE_CALLBACK(update_func, s_manager.current_scene, dt);
}

void scenes_render() {
  SCENE_CALLBACK(render_func, s_manager.current_scene);
}

void scenes_render_gui() {
  SCENE_CALLBACK(render_gui_func, s_manager.current_scene);
}

/// SceneManager functions
///---------------------------------------------------------------------------------------------------------------------
