#include "app.hpp"
#include "scenes/scene_manager.h"
#include "scenes/game_scene.h"

#include <nikola/nikola.h>
#include <imgui/imgui.h>

/*
 * @NOTE (27/4/2025, Mohamed):
 * 
 * This demo uses a few resources that are (obviously) not mine. Below, 
 * are all of the names of the _amazing_ artists who made these fantastic pieces.
 *
 * Scene 1:
 *  - Tempel: https://sketchfab.com/www.noe-3d.at
 *  - Column Torch: https://sketchfab.com/Mavriarch
 * 
 * Scene 2: 
 *  - Bridge: https://sketchfab.com/xplanepilot 
 *
 * Scene 3: 
 *  - PS1: https://sketchfab.com/FionaGalloway
 * 
*/

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;

  nikola::ResourceID post_shader_context_id;
  nikola::Font* font;

  nikola::Vec2 screen_size  = nikola::Vec2(0.0f);
  nikola::i32 render_effect = 0;
  nikola::i32 pixel_rate    = 16;
};
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

static void post_process_pass(const nikola::RenderPass* prev, nikola::RenderPass* pass, void* user_data) {
  nikola::App* app           = (nikola::App*)user_data;
  nikola::ShaderContext* ctx = nikola::resources_get_shader_context(pass->shader_context_id);

  // Set the shader's uniform
  nikola::shader_context_set_uniform(ctx, "u_effect_index", app->render_effect);
  nikola::shader_context_set_uniform(ctx, "u_pixel_rate", app->pixel_rate);
  nikola::shader_context_set_uniform(ctx, "u_screen_size", app->screen_size);

  pass->frame_desc.attachments[0]    = prev->frame_desc.attachments[0];
  pass->frame_desc.attachments_count = 1;
}

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

static void init_scenes(nikola::App* app) {
  // Game scene init
  game_scene_init();
}

static void init_passes(nikola::App* app) {
  // Post-process pass
  nikola::RenderPassDesc render_pass = {
    .frame_size        = app->screen_size, 
    .clear_color       = nikola::Vec4(1.0f),
    .clear_flags       = (nikola::GFX_CLEAR_FLAGS_COLOR_BUFFER),
    .shader_context_id = app->post_shader_context_id,
  };
  render_pass.targets.push_back(nikola::RenderTarget{.format = nikola::GFX_TEXTURE_FORMAT_RGBA32F, .filter = nikola::GFX_TEXTURE_FILTER_MIN_MAG_LINEAR});
  // nikola::renderer_push_pass(render_pass, post_process_pass, app);
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
  nikola::window_set_position(window, 100, 100);
  
  nikola::i32 width, height; 
  nikola::window_get_size(app->window, &width, &height);
  app->screen_size = nikola::Vec2(width, height); 

  // Editor init
  nikola::gui_init(window);

  // Fonts init
  app->font = nikola::resources_get_font(nikola::resources_push_font(nikola::RESOURCE_CACHE_ID, "fonts/bit5x3.nbrfont"));

  // Shader contexts init
  app->post_shader_context_id = nikola::resources_push_shader_context(nikola::RESOURCE_CACHE_ID, "shaders/post_process.nbrshader");

  // Render passes init
  init_passes(app); 

  // Scene manager init
  scenes_init(window);

  // Scenes init
  init_scenes(app);

  return app;
}

void app_shutdown(nikola::App* app) {
  scenes_shutdown();
  nikola::gui_shutdown();

  delete app;
}

void app_update(nikola::App* app, const nikola::f64 delta_time) {
  // Close the window when `ESCAPE` is pressed
  if(nikola::input_key_down(nikola::KEY_ESCAPE)) {
    nikola::event_dispatch(nikola::Event{.type = nikola::EVENT_APP_QUIT});
    return;
  }

  scenes_update(delta_time);
} 

void app_render(nikola::App* app) {
  // 3D renderer
  nikola::renderer_begin(scenes_get_current()->frame_data);
  scenes_render();
  nikola::renderer_end();
  
  // 2D renderer
  nikola::batch_renderer_begin();
  nikola::batch_render_fps(app->font, nikola::Vec2(10.0f, 40.0f), 32.0f, nikola::Vec4(1.0f));
  nikola::batch_renderer_end();
}

void app_render_gui(nikola::App* app) {
  nikola::gui_begin(); 
 
  // Scene GUI
  scenes_render_gui();

  if(!scenes_get_current()->has_editor) {
    nikola::gui_end(); 
    return;
  }

  // Renderer
  nikola::gui_begin_panel("Renderer");
  ImGui::Combo("Render Effect", 
               &app->render_effect, 
               "None\0Greyscale\0Inversion\0Sharpen\0Blur\0Emboss\0Edge Detection\0Pixelize\0");  
  ImGui::DragInt("Pixel Rate", &app->pixel_rate, 1.0f, 0, 64);
  nikola::gui_end_panel();

  // Debug
  nikola::gui_debug_info();
  
  nikola::gui_end(); 
}

/// App functions 
/// ----------------------------------------------------------------------
