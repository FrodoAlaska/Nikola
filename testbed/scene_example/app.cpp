#include "app.hpp"
#include "scenes/game_scene.h"

#include <nikola/nikola.h>

#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;
  nikola::ResourceID post_shader_context_id;

  GameScene game_scene;
};
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

static void post_process_pass(const nikola::RenderPass* prev, nikola::RenderPass* pass, void* user_data) {
  nikola::App* app = (nikola::App*)user_data;

  // Set the shader
  nikola::shader_context_set_uniform(pass->shader_context_id, "u_effect_index", app->game_scene.render_effect);

  pass->frame_desc.attachments[0]    = prev->frame_desc.attachments[0];
  pass->frame_desc.attachments_count = 1;
}

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

static void init_scenes(nikola::App* app) {
  // Game scene init
  game_scene_init(&app->game_scene, app->window);
}

static void init_passes(nikola::App* app) {
  nikola::i32 width, height; 
  nikola::window_get_size(app->window, &width, &height);

  // Post-process pass
  nikola::RenderPassDesc render_pass = {
    .frame_size        = nikola::Vec2(width, height), 
    .clear_color       = nikola::Vec4(1.0f),
    .clear_flags       = (nikola::GFX_CLEAR_FLAGS_COLOR_BUFFER),
    .shader_context_id = app->post_shader_context_id,
  };
  render_pass.targets.push_back(nikola::RenderTarget{});
  nikola::renderer_push_pass(render_pass, post_process_pass, app);
}

/// Private functions 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App functions 

nikola::App* app_init(const nikola::Args& args, nikola::Window* window) {
  // App init
  nikola::App* app = new nikola::App{};

  // Window init
  app->window = window;

  // Editor init
  nikola::gui_init(window);

  // Shaders init
  nikola::resources_push_shader(nikola::RESOURCE_CACHE_ID, "shaders/post_process.nbrshader");

  // Shader contexts init
  app->post_shader_context_id = nikola::resources_push_shader_context(nikola::RESOURCE_CACHE_ID, nikola::resources_get_id(nikola::RESOURCE_CACHE_ID, "post_process"));

  // Render passes init
  init_passes(app); 

  // Scenes init
  init_scenes(app);

  return app;
}

void app_shutdown(nikola::App* app) {
  game_scene_shutdown(app->game_scene);
  nikola::gui_shutdown();

  delete app;
}

void app_update(nikola::App* app, const nikola::f64 delta_time) {
  // Close the window when `ESCAPE` is pressed
  if(nikola::input_key_down(nikola::KEY_ESCAPE)) {
    nikola::event_dispatch(nikola::Event{.type = nikola::EVENT_APP_QUIT});
    return;
  }

  game_scene_update(app->game_scene);
} 

void app_render(nikola::App* app) {
  nikola::renderer_clear(nikola::Vec4(0.14f, 0.16f, 0.28f, 1.0f));
  
  // Render the scene
  nikola::renderer_begin(app->game_scene.frame_data);
  game_scene_render(app->game_scene);
  nikola::renderer_end();
  
  // Render GUI
  nikola::gui_begin(); 
  game_scene_gui_render(app->game_scene);
  nikola::gui_end(); 
  
  nikola::renderer_present();
}

/// App functions 
/// ----------------------------------------------------------------------
