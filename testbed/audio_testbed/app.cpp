#include "app.h"

#include <nikola/nikola.h>
#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;

  nikola::FrameData frame_data;
  nikola::ResourceGroupID res_group;

  nikola::AudioBufferID audio_buffer;
  nikola::AudioSourceID audio_source;

  bool has_editor = false;
};

/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

static void init_resources(nikola::App* app) {
  // Resource group init
  nikola::FilePath res_path = nikola::filepath_append(nikola::filesystem_current_path(), "res");
  app->res_group            = nikola::resources_create_group("AudioApp_res", res_path);

  // Skybox init
  app->frame_data.skybox_id = {};

  // Audio init
  nikola::ResourceID audio_id = nikola::resources_push_audio_buffer(app->res_group, "audio/alexandra.nbr");
  app->audio_buffer           = nikola::resources_get_audio_buffer(audio_id);

  // Audio sources init
  nikola::AudioSourceDesc source_desc;
  source_desc.buffers[0]    = app->audio_buffer; 
  source_desc.buffers_count = 1; 
  app->audio_source         = nikola::audio_source_create(source_desc);

  // Audio listener init
  nikola::audio_listener_init(nikola::AudioListenerDesc{});
}

/// Private functions 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App functions 

nikola::App* app_init(const nikola::Args& args, nikola::Window* window) {
  // App init
  nikola::App* app = new nikola::App{};
  nikola::renderer_set_clear_color(nikola::Vec4((16.0f / 255.0f), (14.0f / 255.0f), (28.0f / 255.0f), 1.0f));

  // Window init
  app->window = window;
  nikola::window_set_position(window, 100, 100);

  // Editor init
  nikola::gui_init(window);

  // Resources init
  init_resources(app);

  return app;
}

void app_shutdown(nikola::App* app) {
  nikola::audio_source_destroy(app->audio_source);
  nikola::gui_shutdown();
  nikola::resources_destroy_group(app->res_group);

  delete app;
}

static bool can_play_sound = false;

void app_update(nikola::App* app, const nikola::f64 delta_time) {
  // Close the window when `ESCAPE` is pressed
  if(nikola::input_key_down(nikola::KEY_ESCAPE)) {
    nikola::event_dispatch(nikola::Event{.type = nikola::EVENT_APP_QUIT});
    return;
  }

  if(nikola::input_key_pressed(nikola::KEY_F1)) {
    app->has_editor = !app->has_editor;
    nikola::input_cursor_show(app->has_editor);
  }
} 

void app_render(nikola::App* app) {
  nikola::renderer_begin(app->frame_data);
  nikola::renderer_end();
  
  // 2D renderer (even though it doesn't matter in this test, but still)
  nikola::batch_renderer_begin();
  nikola::batch_render_quad(nikola::Vec2(10.0f), nikola::Vec2(32.0f), nikola::Vec4(1.0f));  
  nikola::batch_renderer_end();
}

void app_render_gui(nikola::App* app) {
  if(!app->has_editor) {
    return;
  }
 
  nikola::gui_begin(); 

  // AudioSource
  nikola::gui_begin_panel("Audio");

  nikola::gui_edit_audio_source("Source", app->audio_source);
  nikola::gui_edit_audio_listener("Listener");

  nikola::gui_end_panel();

  // Debug
  nikola::gui_debug_info();
  
  nikola::gui_end(); 
}

/// App functions 
/// ----------------------------------------------------------------------
