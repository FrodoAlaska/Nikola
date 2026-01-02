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

  nikola::UIContext* context;
  nikola::UIDocument* document;

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
  
  app->font_id = nikola::resources_push_font(app->res_group_id, "fonts/HeavyDataNerdFont.nbr");
  nikola::resources_push_texture(app->res_group_id, "textures/frodo.nbr");

  nikola::ui_renderer_load_font(nikola::filepath_append(res_path, "fonts/HeavyDataNerdFont.ttf"));
}

static bool on_button_pressed(const nikola::Event& event, const void* dispatcher, const void* listener) {
  if(nikola::ui_element_get_id(event.element) == "quit") {
    nikola::event_dispatch(nikola::Event{.type = nikola::EVENT_APP_QUIT});
  }

  return true;
}

/// Private functions 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App functions 

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

  // UI init

  nikola::FilePath base_path = nikola::filepath_append(nikola::filesystem_current_path(), "res");
  
  app->context  = nikola::ui_context_create("Main", nikola::IVec2(width, height));
  app->document = nikola::ui_document_load(app->context, nikola::filepath_append(base_path, "ui/dialogue.rml"));
  
  nikola::ui_document_enable_events(app->document);
  nikola::ui_document_show(app->document);

  // Listen to events
  nikola::event_listen(nikola::EVENT_UI_ELEMENT_CLICKED, on_button_pressed);

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

  // Reload the CSS

  if(nikola::input_key_pressed(nikola::KEY_F5)) {
    nikola::ui_document_reload_stylesheet(app->document);
  }

  // Hide the document

  if(nikola::input_key_pressed(nikola::KEY_P)) {
    if(nikola::ui_document_is_shown(app->document)) {
      nikola::ui_document_hide(app->document);
    }
    else {
      nikola::ui_document_show(app->document);
    }
  }

  // UI elements update
  nikola::ui_context_update(app->context);

  // Update the camera
  
  nikola::camera_free_move_func(app->frame_data.camera);
  nikola::camera_update(app->frame_data.camera);
} 

void app_render(nikola::App* app) {
  // Render 3D 
  
  nikola::renderer_begin(app->frame_data);
  nikola::renderer_end();
  
  // Render 2D 
  
  nikola::batch_renderer_begin();
  nikola::batch_renderer_end();

  // Render UI 

  nikola::ui_renderer_begin();
  nikola::ui_context_render(app->context);
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
