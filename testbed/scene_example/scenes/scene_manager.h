#pragma once

#include <nikola/nikola.h>

// Must have forward declarations here
struct Scene; 

///---------------------------------------------------------------------------------------------------------------------
/// Scene Callbacks

using SceneCreateFn    = bool(*)(Scene* scene);

using SceneDestroyFn   = void(*)(Scene* scene);

using SceneUpdateFn    = void(*)(Scene* scene, const nikola::f64 dt);

using SceneRenderFn    = void(*)(Scene* scene);

using SceneSerializeFn = void(*)(Scene* scene, const nikola::FilePath& path);

/// Scene Callbacks
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// SceneDesc
struct SceneDesc {
  SceneCreateFn create_func   = nullptr;
  SceneDestroyFn destroy_func = nullptr;
  SceneUpdateFn update_func   = nullptr; 

  SceneRenderFn render_func     = nullptr;
  SceneRenderFn render_gui_func = nullptr;

  SceneSerializeFn serialize_func   = nullptr;
  SceneSerializeFn deserialize_func = nullptr;

  void* user_data = nullptr;
};
/// SceneDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Scene
struct Scene {
  SceneDesc desc;

  const char* name;
  nikola::Window* window;

  nikola::u16 resource_group; 
  nikola::FrameData frame_data;

  bool has_editor = false;
};
/// Scene
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// SceneManager functions

void scenes_init(nikola::Window* window);

void scenes_shutdown();

void scenes_push_scene(const char* name, const SceneDesc& desc);

void scenes_set_current(const char* scene_name);

Scene* scenes_get_current();

Scene* scenes_get_scene(const char* scene_name);

void scenes_update(const nikola::f64 dt);

void scenes_render();

void scenes_render_gui();

/// SceneManager functions
///---------------------------------------------------------------------------------------------------------------------
