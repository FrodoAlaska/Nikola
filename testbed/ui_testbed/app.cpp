#include "app.h"

#include <nikola/nikola.h>
#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// DialogueInfo
struct DialogueInfo {
  nikola::String text;
  nikola::String speaker; 

  nikola::sizei current_line = 0;
  nikola::DynamicArray<nikola::String> lines;
  
  nikola::UIElement* speaker_element;
  nikola::UIElement* text_element;
  nikola::UIElement* next_button;
};
/// DialogueInfo
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;
  nikola::FrameData frame_data;
  
  nikola::ResourceGroupID res_group_id;
  nikola::ResourceID font_id;

  nikola::UIContext* context;
  nikola::UIDocument* document;

  DialogueInfo dialogue_info;

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

static bool dialogue_load(DialogueInfo* info, const nikola::FilePath& txt_path, nikola::UIDocument* ui_doc) {
  // Open the dialogue file

  nikola::File file;
  nikola::i32 file_flags = (nikola::i32)(nikola::FILE_OPEN_READ);

  if(!nikola::file_open(&file, txt_path, file_flags)) {
    NIKOLA_LOG_ERROR("Failed to read dialogue file at \'%s\'", txt_path.c_str());
    return false;
  }

  // Read the whole text first
  nikola::file_read_string(file, &info->text);

  // Read the speaker's name

  nikola::sizei start = 0;
  for(nikola::sizei i = 0; i < info->text.size(); i++) {
    char ch = info->text[i];

    if(ch != ':') {
      info->speaker += ch;
      continue;
    }

    info->speaker += ':';
    start          = i + 1;

    break;
  }

  // Read the lines

  nikola::String line;
  for(nikola::sizei i = start; i < info->text.size(); i++) {
    char ch = info->text[i];
    
    if(ch != '#') {
      line += ch;
      continue;
    }

    info->lines.emplace_back(line);
    line.clear();
  }

  // Set the initial dialogue values

  info->speaker_element = nikola::ui_document_get_element_by_id(ui_doc, "speaker");
  info->text_element    = nikola::ui_document_get_element_by_id(ui_doc, "dialogue");
  info->next_button     = nikola::ui_document_get_element_by_id(ui_doc, "next-arrow");

  nikola::ui_element_set_inner_html(info->speaker_element, info->speaker);
  nikola::ui_element_set_inner_html(info->text_element, info->lines[0]);

  // Enabling events for the next button
  nikola::ui_element_enable_events(info->next_button);

  // Done!
  return true;
}

static void dialogue_advance(DialogueInfo& info) {
  info.current_line++;
  if(info.current_line >= info.lines.size()) { // Out of bounds...
    return;
  }

  nikola::ui_element_set_inner_html(info.text_element, info.lines[info.current_line]);
}

/// Private functions 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

static bool on_button_pressed(const nikola::Event& event, const void* dispatcher, const void* listener) {
  const nikola::String& tag = nikola::ui_element_get_id(event.element);

  if(tag == "quit") {
    nikola::event_dispatch(nikola::Event{.type = nikola::EVENT_APP_QUIT});
  }

  return true;
}

/// Callbacks
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
  app->document = nikola::ui_document_load(app->context, nikola::filepath_append(base_path, "ui/main_menu.rml"));
  
  nikola::ui_document_enable_events(app->document);
  nikola::ui_document_show(app->document);

  // Enable events for all the menu buttons

  nikola::DynamicArray<nikola::UIElement*> elements;
  nikola::ui_document_query_selector_all(app->document, ".menu-button", elements);

  for(auto& element : elements) {
    nikola::ui_element_enable_events(element);
  }

  // Read the dialogue file

  // if(!dialogue_load(&app->dialogue_info, nikola::filepath_append(base_path, "dialogue/hera_intro.txt"), app->document)) {
  //   return app;
  // }

  // Listen to events
  nikola::event_listen(nikola::EVENT_UI_ELEMENT_CLICKED, on_button_pressed);

  return app;
}

void app_shutdown(nikola::App* app) {
  nikola::ui_context_destroy(app->context);
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
