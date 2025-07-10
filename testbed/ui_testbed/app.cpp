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

  nikola::UISlider ui_slider;
  nikola::UILayout ui_layout;

  bool has_editor = false;
};
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

static bool on_button_event(const nikola::Event& event, const void* dispatcher, const void* listener) {
  if(event.type != nikola::EVENT_UI_BUTTON_CLICKED) {
    return false;
  }

  switch(event.button->id) {
    case 0: // Play
      break;
    case 1: // Settings
      break;
    case 2: // Quit
      nikola::event_dispatch(nikola::Event{.type = nikola::EVENT_APP_QUIT});
      break;
    default:
      break;
  }

  return true;
}

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

static void init_resources(nikola::App* app) {
  // Resource storage init 
  nikola::FilePath res_path = nikola::filepath_append(nikola::filesystem_current_path(), "res");
  app->res_group_id = nikola::resources_create_group("app_res", res_path);

  // Resoruces init
  app->font_id = nikola::resources_push_font(app->res_group_id, "fonts/IosevkaNerdFont-Bold.nbr");
}

/// Private functions 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App functions 

static float test_value = 0.0f;

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

  // Listen to events
  nikola::event_listen(nikola::EVENT_UI_BUTTON_CLICKED, on_button_event, app);
  nikola::event_listen(nikola::EVENT_UI_BUTTON_ENTERED, on_button_event, app);
  nikola::event_listen(nikola::EVENT_UI_BUTTON_EXITED, on_button_event, app);

  // UI layout init
  nikola::ui_layout_create(&app->ui_layout, app->window, app->font_id);
  
  nikola::ui_layout_begin(app->ui_layout, nikola::UI_ANCHOR_TOP_CENTER);
  nikola::ui_layout_push_text(app->ui_layout, "Hello, Nikola", 80.0f, nikola::Vec4(1.0f));
  nikola::ui_layout_end(app->ui_layout);
 
  nikola::Vec4 color(1.0f);
  nikola::Vec4 outline_color(0.0f, 0.0f, 0.0f, 1.0f);
  nikola::Vec4 text_color(0.0f, 0.0f, 0.0f, 1.0f);

  nikola::ui_layout_begin(app->ui_layout, nikola::UI_ANCHOR_CENTER, nikola::Vec2(0.0f, 65.0f));
  
  nikola::ui_layout_push_button(app->ui_layout, "Play", 40.0f, color, outline_color, text_color);
  // nikola::ui_layout_push_button(app->ui_layout, "Settings", 40.0f, color, outline_color, text_color);
  // nikola::ui_layout_push_button(app->ui_layout, "Quit", 40.0f, color, outline_color, text_color);
  
  nikola::ui_layout_push_checkbox(app->ui_layout, 32.0f, color, outline_color);
  nikola::ui_layout_push_slider(app->ui_layout, 
                                &app->ui_layout.texts[0].color.a, 
                                0.0f, 1.0f, 0.01f, 
                                color);
  
  nikola::ui_layout_end(app->ui_layout);

  // UI slider init
  nikola::UISliderDesc slider_desc = {
    .anchor = nikola::UI_ANCHOR_BOTTOM_LEFT,

    .value = &test_value,
    .min   = 0.0f, 
    .max   = 1.0f,
  };
  nikola::ui_slider_create(&app->ui_slider, app->window, slider_desc);

  return app;
}

void app_shutdown(nikola::App* app) {
  nikola::resources_destroy_group(app->res_group_id);
  nikola::gui_shutdown();

  delete app;
}

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

  // Update the camera
  nikola::camera_update(app->frame_data.camera);
} 

void app_render(nikola::App* app) {
  // Render 3D 
  nikola::renderer_begin(app->frame_data);
  nikola::renderer_end();
  
  // Render 2D 
  nikola::batch_renderer_begin();

  nikola::ui_slider_render(app->ui_slider);

  // nikola::ui_text_apply_animation(app->ui_layout.texts[0], nikola::UI_TEXT_ANIMATION_FADE_IN, 0.4f);
  nikola::ui_layout_render(app->ui_layout);

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
