#include "app.h"

#include <nikola/nikola.h>
#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;
  nikola::FrameData frame_data;
  
  nikola::ResourceGroupID res_group_id;
  nikola::ResourceID font_id;

  nikola::UIText ui_text;

  bool has_editor = false;
};
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

static void init_resources(nikola::App* app) {
  // Resource storage init 
  nikola::FilePath res_path = nikola::filepath_append(nikola::filesystem_current_path(), "res");
  app->res_group_id = nikola::resources_create_group("app_res", res_path);

  // Resoruces init
  app->font_id = nikola::resources_push_font(app->res_group_id, "fonts/IosevkaNerdFont-Bold.nbrfont");
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

  // Editor init
  nikola::gui_init(window);

  // Camera init
  nikola::CameraDesc cam_desc = {
    .position     = nikola::Vec3(10.0f, 0.0f, 10.0f),
    .target       = nikola::Vec3(-3.0f, 0.0f, 0.0f),
    .up_axis      = nikola::Vec3(0.0f, 1.0f, 0.0f),
    .aspect_ratio = nikola::window_get_aspect_ratio(app->window),
    .move_func    = nikola::camera_free_move_func,
  };
  nikola::camera_create(&app->frame_data.camera, cam_desc);

  // Resoruces init
  init_resources(app);

  // UI init
  nikola::UITextDesc text_desc = {
    .string = "Hello, Nikola", 

    .font_id   = app->font_id, 
    .font_size = 50.0f,

    .anchor = nikola::UI_ANCHOR_CENTER,
  };
  nikola::ui_text_create(&app->ui_text, app->window, text_desc);

  return app;
}

void app_shutdown(nikola::App* app) {
  nikola::resources_destroy_group(app->res_group_id);
  nikola::gui_shutdown();

  delete app;
}

static int current_anchor = 0;

void app_update(nikola::App* app, const nikola::f64 delta_time) {
  // Quit the application when the specified exit key is pressed
  if(nikola::input_key_pressed(nikola::KEY_ESCAPE)) {
    nikola::event_dispatch(nikola::Event{.type = nikola::EVENT_APP_QUIT});
    return;
  }

  // Disable/enable the GUI
  if(nikola::input_key_pressed(nikola::KEY_F1)) {
    app->has_editor                  = !app->has_editor;
    app->frame_data.camera.is_active = !app->has_editor;

    nikola::input_cursor_show(app->has_editor);
  }

  if(nikola::input_action_pressed("ui-click")) {
    nikola::ui_text_set_anchor(app->ui_text, (nikola::UIAnchor)(21 << current_anchor));
    current_anchor++;
  }
 
  if(current_anchor > 8) {
    current_anchor = 0;
  }

  // Update the camera
  nikola::camera_update(app->frame_data.camera);
} 

void app_render(nikola::App* app) {
  // Render 3D 
  nikola::renderer_begin(app->frame_data);
  nikola::renderer_end();
  
  // Render 2D 
  nikola::batch_renderer_begin();
  
  // nikola::ui_text_apply_animation(app->ui_text, nikola::UI_TEXT_ANIMATION_FADE_OUT, 0.3f);
  // nikola::ui_text_apply_animation(app->ui_text, nikola::UI_TEXT_ANIMATION_BALLON_DOWN, 1.5f);
  nikola::ui_text_render(app->ui_text);
  
  nikola::batch_renderer_end();
}

void app_render_gui(nikola::App* app) {
  if(!app->has_editor) {
    return;
  }

  nikola::gui_begin();
  nikola::gui_end();
}


/// App functions 
/// ----------------------------------------------------------------------
