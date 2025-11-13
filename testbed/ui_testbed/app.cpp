#include "app.h"

#include <nikola/nikola.h>
#include <imgui/imgui.h>

#include <RmlUi/Core.h>

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;
  nikola::FrameData frame_data;
  
  nikola::ResourceGroupID res_group_id;
  nikola::ResourceID font_id;

  nikola::UIMenu main_menu;

  bool has_editor = false;

  Rml::Context* context; 
  Rml::ElementDocument* document;
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
  
  // app->font_id = nikola::resources_push_font(app->res_group_id, "fonts/IosevkaNerdFont-Bold.nbr");
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

  // UI init

  nikola::FilePath res_path  = nikola::filepath_append(nikola::filesystem_current_path(), "res");
  nikola::FilePath doc_path  = nikola::filepath_append(res_path, "ui/first.rml");
  nikola::FilePath font_path = nikola::filepath_append(res_path, "fonts/HeavyDataNerdFont.ttf");

  Rml::LoadFontFace(font_path);

  app->context  = Rml::CreateContext("main", Rml::Vector2i(width, height)); 
  app->document = app->context->LoadDocument(doc_path); 
  app->document->Show();

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

  app->context->Update();

  // Update the camera
  
  // nikola::camera_free_move_func(app->frame_data.camera);
  nikola::camera_update(app->frame_data.camera);
} 

void app_render(nikola::App* app) {
  // Render 3D 
  
  nikola::renderer_begin(app->frame_data);
  nikola::renderer_end();
  
  // Render UI 
  
  nikola::ui_renderer_begin();
  app->context->Render();
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
