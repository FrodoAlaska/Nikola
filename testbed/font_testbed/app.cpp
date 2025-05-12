#include "app.h"

#include <nikola/nikola.h>
#include <imgui/imgui.h>

/* @NOTE (9/5/2025, Mohamed):
 *
 * Thanks to the amazing artist over at https://sketchfab.com/Metazeon for 
 * providing the magnificent 3D model of the Langya Pavilion. It's huge, 
 * it's detailed, and it's damn pretty.
 *
 */

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;

  nikola::FrameData frame_data;
  nikola::RenderQueue render_queue;
  nikola::RenderCommand render_cmd;
  
  // @NOTE (Font tested): This is awful. Don't ACTUALLy do this! It's just for testing.
  nikola::Font* font;

  nikola::u16 res_group;
  nikola::ResourceID model_id;

  nikola::Transform transform;

  bool has_editor = false;
};

static float s_font_size       = 48.0f;
static nikola::Vec4 s_color    = nikola::Vec4(1.0f);
static nikola::String s_label  = "H.";
static nikola::Vec2 s_position = nikola::Vec2(20.0f, 50.0f);

/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

static void load_dialogue_text(const nikola::FilePath& path) {
  nikola::File file; 
  if(!nikola::file_open(&file, path, (nikola::i32)nikola::FILE_OPEN_READ)) {
    NIKOLA_LOG_FATAL("Could not open dialogue file \'%s\'", path.c_str());
    return;
  }

  nikola::file_read_string(file, &s_label);
  nikola::file_close(file);
}

static void init_resources(nikola::App* app) {
  // Resource group init
  nikola::FilePath res_path = nikola::filepath_append(nikola::filesystem_current_path(), "res");
  app->res_group            = nikola::resources_create_group("FontApp_res", res_path);

  // Cubemaps init
  nikola::ResourceID cubemap_id = nikola::resources_push_cubemap(app->res_group, "cubemaps/accurate_night.nbrcubemap");

  // Fonts init
  app->font = nikola::resources_get_font(nikola::resources_push_font(app->res_group, "fonts/IosevkaNerdFont-Bold.nbrfont"));

  // Models init
  app->model_id = nikola::resources_push_model(app->res_group, "models/langya.nbrmodel");

  // Skybox init
  app->frame_data.skybox_id = nikola::resources_push_skybox(app->res_group, cubemap_id);

  // Materials init
  app->render_cmd.material_id = nikola::resources_push_material(app->res_group);
}

static void init_lights(nikola::App* app) {
  // Directional light
  app->frame_data.dir_light.direction = nikola::Vec3(0.0f, 0.0f, 0.0f);
  app->frame_data.dir_light.color     = nikola::Vec3(0.0f, 0.0f, 0.0f);
 
  // Point lights
  app->frame_data.point_lights.push_back(nikola::PointLight{nikola::Vec3(-9.0f, -118.0f, 36.0f), nikola::Vec3(10.0f)});

  // Set the ambiance
  app->frame_data.ambient = nikola::Vec3(0.0f);
}

static void text_animation_typewriter(nikola::App* app, const nikola::String& text, const nikola::f32& duration) {
  static nikola::f32 anim_timer   = 0.0f; 
  static nikola::String anim_text = ""; 

  if(anim_text.size() != text.size()) {
    anim_timer += 0.1f;
  }

  if(anim_timer >= duration ) {
    anim_timer = 0.0f; 
    anim_text += text[anim_text.size()];
  }
  
  nikola::batch_render_text(app->font, anim_text, nikola::Vec2(20.0f, 50.0f), s_font_size, nikola::Vec4(1.0f));
}

static void text_animation_blink(nikola::App* app, const nikola::String& text, const nikola::f32& time) {
  static nikola::f32 direction = 1.0f;

  if(s_color.a > 1 || s_color.a < 0) {
    direction = -direction;
  } 
  
  s_color.a += direction * time; 
  nikola::batch_render_text(app->font, text, s_position, s_font_size, s_color);
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
  
  // Camera init
  float aspect_ratio = nikola::window_get_aspect_ratio(app->window);
  nikola::camera_create(&app->frame_data.camera, aspect_ratio, nikola::Vec3(10.0f, 0.0f, 10.0f), nikola::Vec3(-3.0f, 0.0f, 0.0f));
  app->frame_data.camera.exposure = 0.154f;

  // Resources init
  init_resources(app);

  // Transform init
  nikola::transform_translate(app->transform, nikola::Vec3(10.0f, -200.0f, 10.0f));
  nikola::transform_scale(app->transform, nikola::Vec3(1.0f));
  nikola::transform_rotate(app->transform, nikola::Vec4(1.0f, 0.0f, 0.0f, -90.0f * nikola::DEG2RAD));

  // Lights init
  init_lights(app);

  // Dialogue text init
  load_dialogue_text("dialogue.txt");

  return app;
}

void app_shutdown(nikola::App* app) {
  nikola::gui_shutdown();

  delete app;
}

static bool can_show = false;

void app_update(nikola::App* app, const nikola::f64 delta_time) {
  // Close the window when `ESCAPE` is pressed
  if(nikola::input_key_down(nikola::KEY_ESCAPE)) {
    nikola::event_dispatch(nikola::Event{.type = nikola::EVENT_APP_QUIT});
    return;
  }

  if(nikola::input_key_pressed(nikola::KEY_A)) {
    app->frame_data.camera.exposure -= 0.1f;
  }
  
  if(nikola::input_key_pressed(nikola::KEY_F1)) {
    app->has_editor                  = !app->has_editor;
    app->frame_data.camera.is_active = !app->has_editor;

    nikola::input_cursor_show(app->has_editor);
  }

  if(nikola::input_key_pressed(nikola::KEY_E)) {
    can_show = !can_show;
  }

  nikola::camera_update(app->frame_data.camera);
} 

void app_render(nikola::App* app) {
  // 3D renderer
  nikola::renderer_begin(app->frame_data);

  app->render_cmd.transform     = app->transform;
  app->render_cmd.render_type   = nikola::RENDERABLE_TYPE_MODEL;
  app->render_cmd.renderable_id = app->model_id;
  app->render_queue.push_back(app->render_cmd); 

  nikola::renderer_sumbit_queue(app->render_queue);
  nikola::renderer_end();
 
  // 2D renderer
  nikola::batch_renderer_begin();
  
  if(can_show) {
    text_animation_typewriter(app, s_label, 0.65f);
  }
  else {
    text_animation_blink(app, "Press [E] To Start", 0.01f);
  }

  nikola::batch_renderer_end();
}

void app_render_gui(nikola::App* app) {
  if(!app->has_editor) {
    return;
  }
 
  nikola::gui_begin(); 

  nikola::gui_begin_panel("Entities");
  
  // Models
  nikola::gui_edit_transform("Building", &app->transform); 
  
  // Camera
  nikola::gui_edit_camera("Editor Camera", &app->frame_data.camera); 
  ImGui::DragFloat3("Ambient", &app->frame_data.ambient[0], 0.1f, 0.0f, 1.0f);
  
  // Directional light
  nikola::gui_edit_directional_light("Directional", &app->frame_data.dir_light);
  
  // Point lights
  nikola::gui_edit_point_light("Light 1", &app->frame_data.point_lights[0]);
  
  nikola::gui_end_panel();

  // Text
  nikola::gui_begin_panel("Text");
 
  ImGui::DragFloat("Size", &s_font_size, 1.0f, 0.0f, nikola::FLOAT_MAX);
  ImGui::DragFloat2("Position", &s_position[0], 1.0f, 0.0f, nikola::FLOAT_MAX);
  nikola::gui_edit_color("Color", s_color);

  nikola::gui_edit_font("Font", app->font, &s_label);
  nikola::gui_end_panel();

  // Debug
  nikola::gui_debug_info();
  
  nikola::gui_end(); 
}

/// App functions 
/// ----------------------------------------------------------------------
