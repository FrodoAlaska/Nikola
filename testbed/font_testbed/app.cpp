#include "app.h"

#include <nikola/nikola.h>
#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;
  nikola::FrameData frame_data;
 
  nikola::u16 res_group;
  nikola::ResourceID post_shader_context_id;
  nikola::ResourceID texture;

  nikola::Vec2 screen_size  = nikola::Vec2(0.0f);
  nikola::i32 render_effect = 0;
  nikola::i32 pixel_rate    = 16;
  bool has_editor           = false;
};
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

static void post_process_pass(const nikola::RenderPass* prev, nikola::RenderPass* pass, void* user_data) {
  nikola::App* app = (nikola::App*)user_data;

  // Set the shader's uniform
  nikola::shader_context_set_uniform(pass->shader_context_id, "u_effect_index", app->render_effect);
  nikola::shader_context_set_uniform(pass->shader_context_id, "u_pixel_rate", app->pixel_rate);
  nikola::shader_context_set_uniform(pass->shader_context_id, "u_screen_size", app->screen_size);
  
  pass->frame_desc.attachments[0]    = prev->frame_desc.attachments[0];
  pass->frame_desc.attachments_count = 1;
}

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

static void init_passes(nikola::App* app) {
  // Post-process pass
  nikola::RenderPassDesc render_pass = {
    .frame_size        = app->screen_size, 
    .clear_color       = nikola::Vec4(1.0f),
    .clear_flags       = (nikola::GFX_CLEAR_FLAGS_COLOR_BUFFER),
    .shader_context_id = app->post_shader_context_id,
  };
  render_pass.targets.push_back(nikola::RenderTarget{});
  nikola::renderer_push_pass(render_pass, post_process_pass, app);
}

static void init_resources(nikola::App* app) {
  // Resource group init
  nikola::FilePath res_path = nikola::filepath_append(nikola::filesystem_current_path(), "res");
  app->res_group            = nikola::resources_create_group("FontApp_res", res_path);

  // Textures init
  app->texture = nikola::resources_push_texture(app->res_group, "textures/frodo.nbrtexture");
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

  nikola::i32 width, height; 
  nikola::window_get_size(app->window, &width, &height);
  app->screen_size = nikola::Vec2(width, height); 

  // Editor init
  nikola::gui_init(window);
  
  // Camera init
  float aspect_ratio = nikola::window_get_aspect_ratio(app->window);
  nikola::camera_create(&app->frame_data.camera, aspect_ratio, nikola::Vec3(10.0f, 0.0f, 10.0f), nikola::Vec3(-3.0f, 0.0f, 0.0f));

  // Shaders init
  nikola::resources_push_shader(nikola::RESOURCE_CACHE_ID, "shaders/post_process.nbrshader");

  // Shader contexts init
  app->post_shader_context_id = nikola::resources_push_shader_context(nikola::RESOURCE_CACHE_ID, nikola::resources_get_id(nikola::RESOURCE_CACHE_ID, "post_process"));

  // Skybox init
  app->frame_data.skybox_id = {};

  // Render passes init
  init_passes(app); 

  // Resources init
  init_resources(app);

  return app;
}

void app_shutdown(nikola::App* app) {
  nikola::gui_shutdown();

  delete app;
}

void app_update(nikola::App* app, const nikola::f64 delta_time) {
  // Close the window when `ESCAPE` is pressed
  if(nikola::input_key_down(nikola::KEY_ESCAPE)) {
    nikola::event_dispatch(nikola::Event{.type = nikola::EVENT_APP_QUIT});
    return;
  }
  
  if(nikola::input_key_pressed(nikola::KEY_E)) {
    app->has_editor                  = !app->has_editor;
    app->frame_data.camera.is_active = !app->has_editor;

    nikola::input_cursor_show(app->has_editor);
  }

  nikola::camera_update(app->frame_data.camera);
} 

static float radius = 128.0f;
static int sides    = 3;

void app_render(nikola::App* app) {
  // 3D renderer
  nikola::renderer_begin(app->frame_data);
  nikola::renderer_end();
 
  // 2D renderer
  nikola::batch_renderer_begin();

  nikola::batch_render_quad(nikola::Vec2(100.0f), nikola::Vec2(64.0f), nikola::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
  nikola::batch_render_texture(nikola::resources_get_texture(app->texture), nikola::Vec2(200.0f), nikola::Vec2(128.0f, 96.0f));
  
  nikola::batch_render_circle(nikola::Vec2(450.0f), radius, nikola::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
  
  nikola::batch_render_polygon(nikola::Vec2(450.0f, 550.0f), radius, sides, nikola::Vec4(1.0f, 0.0f, 0.0f, 1.0f));

  nikola::batch_renderer_end();
}

void app_render_gui(nikola::App* app) {
  nikola::gui_begin(); 
 
  if(!app->has_editor) {
    nikola::gui_end(); 
    return;
  }

  // Renderer
  nikola::gui_begin_panel("Renderer");
  ImGui::Combo("Render Effect", 
               &app->render_effect, 
               "None\0Greyscale\0Inversion\0Sharpen\0Blur\0Emboss\0Edge Detection\0Pixelize\0");  
  ImGui::DragInt("Pixel Rate", &app->pixel_rate, 1.0f, 0, 64);
  ImGui::DragFloat("Radius", &radius, 1.0f, 0.0f, nikola::FLOAT_MAX);
  ImGui::SliderInt("Sides", &sides, 0, 128);
  nikola::gui_end_panel();

  // Debug
  nikola::gui_debug_info();
  
  nikola::gui_end(); 
}

/// App functions 
/// ----------------------------------------------------------------------
