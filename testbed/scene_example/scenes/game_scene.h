#include <nikola/nikola.h>

/// ----------------------------------------------------------------------
/// GameScene
struct GameScene {
  nikola::Window* window; 
  nikola::Camera camera;

  nikola::u16 resource_group;
  nikola::RenderQueue render_queue;

  nikola::DynamicArray<nikola::Transform> transforms;
  bool has_editor;
};
/// GameScene
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// GameScene functions 

void game_scene_init(GameScene* scene, nikola::Window* window);

void game_scene_update(GameScene& scene);

void game_scene_render(GameScene& scene);

void game_scene_gui_render(GameScene& scene);

void game_scene_shutdown(GameScene& scene);

/// GameScene functions 
/// ----------------------------------------------------------------------
