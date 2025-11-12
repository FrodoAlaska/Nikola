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

  nikola::UIMenu main_menu;

  bool has_editor = false;
};
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

static bool on_menu_event(const nikola::Event& event, const void* dispatcher, const void* listener) {
  switch(event.menu->current_item) {
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
  nikola::resources_push_texture(app->res_group_id, "textures/frodo.nbr");
}

/// Private functions 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App functions 

static float test_value = 0.0f;

nikola::App* app_init(const nikola::Args& args, nikola::Window* window) {
  // App init
  nikola::App* app = new nikola::App{};
  nikola::renderer_set_clear_color(nikola::Vec4(0.8f, 0.1f, 0.1f, 1.0f));

  // Window init
  
  app->window = window;
  
  nikola::i32 width, height;
  nikola::window_get_size(window, &width, &height);

  // Editor init
  nikola::gui_init(window);

  // Camera init
  nikola::CameraDesc cam_desc = {
    .position     = nikola::Vec3(10.0f, 0.0f, 10.0f),
    .target       = nikola::Vec3(-3.0f, 0.0f, 0.0f),
    .up_axis      = nikola::Vec3(0.0f, 1.0f, 0.0f),
    .aspect_ratio = nikola::window_get_aspect_ratio(app->window),
  };
  nikola::camera_create(&app->frame_data.camera, cam_desc);

  // Resoruces init
  init_resources(app);

  // Listen to events
  nikola::event_listen(nikola::EVENT_UI_MENU_ITEM_CLICKED, on_menu_event, app);

  // UI menu init

  nikola::UIMenuDesc menu_desc = {
    .bounds  = nikola::Vec2(width, height),
    .font_id = app->font_id,

    .title_anchor = nikola::UI_ANCHOR_TOP_CENTER, 
    .item_anchor  = nikola::UI_ANCHOR_CENTER,

    .item_offset  = nikola::Vec2(0.0f, 40.0f),
    .item_padding = nikola::Vec2(12.0f, 4.0f),
  };
  nikola::ui_menu_create(&app->main_menu, menu_desc);

  nikola::ui_menu_set_title(app->main_menu, "Menu Title", 50.0f, nikola::Vec4(1.0f));

  nikola::ui_menu_push_item(app->main_menu, "Play", 30.0f, nikola::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
  nikola::ui_menu_push_item(app->main_menu, "Settings", 30.0f, nikola::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
  nikola::ui_menu_push_item(app->main_menu, "Quit", 30.0f, nikola::Vec4(0.0f, 0.0f, 0.0f, 1.0f));

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

  nikola::ui_menu_process_input(app->main_menu);

  // Update the camera
  
  nikola::camera_free_move_func(app->frame_data.camera);
  nikola::camera_update(app->frame_data.camera);
} 

void app_render(nikola::App* app) {
  // Render 3D 
  
  nikola::renderer_begin(app->frame_data);
  nikola::renderer_end();
  
  // Render UI 
  
  nikola::ui_renderer_begin();
  nikola::ui_renderer_end();
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
