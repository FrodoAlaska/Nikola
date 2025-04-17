#include <nikola/nikola.h>

/// ----------------------------------------------------------------------
/// GameScene
struct GameScene {
  nikola::Window* window; 
  nikola::Camera camera;

  nikola::u16 resource_group;
  nikola::ResourceID skybox_id, material_id;
  
  nikola::FrameData frame_data;
  nikola::RenderQueue render_queue;
  
  bool has_editor;
  nikola::i32 render_effect = 0;
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
