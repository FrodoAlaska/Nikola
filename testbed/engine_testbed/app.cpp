#include "app.hpp"
#include "shaders.hpp"

#include <nikola/nikola.h>

#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// @TEMP
struct Prop {
  nikola::Transform transform; 
  
  nikola::RenderableType renderable_type;
  nikola::ResourceID renderable_id; 
  
  nikola::String debug_name;

  Prop(const char* name, 
       const nikola::Vec3& pos, 
       const nikola::Vec3& scale, 
       const nikola::RenderableType type,
       const nikola::ResourceID& renderable) {
    nikola::transform_translate(transform, pos);
    nikola::transform_scale(transform, scale);

    renderable_type = type;
    renderable_id   = renderable;

    debug_name = name;
  }

  void render(nikola::RenderQueue& queue, nikola::ResourceID& shader_context_id, nikola::ResourceID& material_id, const nikola::sizei count = 1) {
    for(nikola::sizei i = 0; i < count; i++) {
      nikola::RenderCommand rnd_cmd {
        .transform       = transform,
        
        .render_type     = renderable_type,
        
        .renderable_id     = renderable_id, 
        .material_id       = material_id, 
        .shader_context_id = shader_context_id,
      };

      nikola::render_queue_push(queue, rnd_cmd);
    }
  }

  void gui_render() {
    nikola::gui_edit_transform(debug_name.c_str(), &transform);
  }
};
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;
  nikola::Camera camera;
  nikola::u16 res_group_id;

  nikola::RenderQueue render_queue;

  nikola::ResourceID material_id;
  nikola::ResourceID skybox_id;
  nikola::DynamicArray<nikola::ResourceID> skyboxes, shader_contexts;

  nikola::DynamicArray<Prop> props;
  Prop* current_prop = nullptr;

  nikola::i32 effect_index = 0;

  bool has_editor;
};
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

static nikola::f32 ease_in_out(const nikola::f32 x) {
  return -(nikola::cos(nikola::PI * x) - 1) / 2.0f;
}

static void init_resources(nikola::App* app) {
  // Resource storage init 
  nikola::FilePath current_path = nikola::filesystem_current_path();
  nikola::FilePath res_path = nikola::filepath_append(current_path, "res");
  app->res_group_id = nikola::resources_create_group("app_res", res_path);

  // Resoruces init
  nikola::resources_push_dir(app->res_group_id, "textures");
  nikola::resources_push_dir(app->res_group_id, "shaders");
  nikola::resources_push_dir(app->res_group_id, "cubemaps");
  // nikola::resources_push_dir(app->res_group_id, "models");
  nikola::resources_push_model(app->res_group_id, "models/behelit.nbrmodel");
  nikola::resources_push_model(app->res_group_id, "models/dice.nbrmodel");

  // Materials init
  app->material_id = nikola::resources_push_material(app->res_group_id);

  // Skybox init
  app->skyboxes.push_back(nikola::resources_push_skybox(app->res_group_id, nikola::resources_get_id(app->res_group_id, "gloomy")));
  app->skyboxes.push_back(nikola::resources_push_skybox(app->res_group_id, nikola::resources_get_id(app->res_group_id, "NightSky")));
  app->skyboxes.push_back(nikola::resources_push_skybox(app->res_group_id, nikola::resources_get_id(app->res_group_id, "desert_cubemap")));
  app->skybox_id = app->skyboxes[0];

  // Shader contexts init 
  app->shader_contexts.push_back(nikola::resources_push_shader_context(app->res_group_id, nikola::resources_get_id(app->res_group_id, "default3d")));
  app->shader_contexts.push_back(nikola::resources_push_shader_context(app->res_group_id, nikola::resources_get_id(app->res_group_id, "cubemap")));
  app->shader_contexts.push_back(nikola::resources_push_shader_context(app->res_group_id, nikola::resources_get_id(app->res_group_id, "geo_pass")));
  app->shader_contexts.push_back(nikola::resources_push_shader_context(app->res_group_id, nikola::resources_get_id(app->res_group_id, "post_process")));
}

static void init_props(nikola::App* app) {
  // Mesh init
  nikola::ResourceID mesh_id = nikola::resources_push_mesh(app->res_group_id, nikola::MESH_TYPE_CUBE);
 
  nikola::Vec3 default_pos(10.0f, 0.0f, 10.0f);
  
  // Porps init
  app->props.push_back(Prop("behelit", default_pos, nikola::Vec3(0.1f), nikola::RENDERABLE_TYPE_MODEL, nikola::resources_get_id(app->res_group_id, "behelit")));
  app->props.push_back(Prop("dice", default_pos + nikola::Vec3(20.0f, 0.0f, 20.0f), nikola::Vec3(1.0f), nikola::RENDERABLE_TYPE_MODEL, nikola::resources_get_id(app->res_group_id, "dice")));
  // app->props.push_back(Prop("bridge", default_pos, nikola::Vec3(1.0f), nikola::RENDERABLE_TYPE_MODEL, nikola::resources_get_id(app->res_group_id, "bridge")));
  // app->props.push_back(Prop("tempel", default_pos, nikola::Vec3(1.0f), nikola::RENDERABLE_TYPE_MODEL, nikola::resources_get_id(app->res_group_id, "tempel")));
  app->props.push_back(Prop("moon", default_pos + nikola::Vec3(-20.0f, 0.0f, 20.0f), nikola::Vec3(1.0f), nikola::RENDERABLE_TYPE_MESH, mesh_id));
  nikola::material_set_texture(app->material_id, nikola::MATERIAL_TEXTURE_DIFFUSE, nikola::resources_get_id(app->res_group_id, "moon"));

  app->current_prop = &app->props[0];
}

static void geometry_pass(const nikola::RenderPass* prev, nikola::RenderPass* pass, void* user_data) {
  nikola::App* app = (nikola::App*)user_data;

  // Render the objects to the framebuffer
  nikola::render_queue_flush(app->render_queue);
}

static void post_process_pass(const nikola::RenderPass* prev, nikola::RenderPass* pass, void* user_data) {
  nikola::App* app = (nikola::App*)user_data;

  // Set the shader
  nikola::shader_context_set_uniform(pass->shader_context_id, "u_effect_index", app->effect_index);

  pass->frame_desc.attachments[0]    = prev->frame_desc.attachments[0];
  pass->frame_desc.attachments_count = 1;
}

static void init_passes(nikola::App* app) {
  nikola::i32 width, height; 
  nikola::window_get_size(app->window, &width, &height);

  // Geometry pass
  nikola::RenderPassDesc render_pass = {
    .frame_size        = nikola::Vec2(width, height), 
    .clear_color       = nikola::Vec4(0.1f, 0.1f, 0.1f, 1.0f),
    .clear_flags       = (nikola::GFX_CLEAR_FLAGS_COLOR_BUFFER | nikola::GFX_CLEAR_FLAGS_DEPTH_BUFFER),
    .shader_context_id = app->shader_contexts[2],
  };
  render_pass.targets.push_back(nikola::GFX_TEXTURE_FORMAT_RGBA8);
  nikola::renderer_push_pass(render_pass, geometry_pass, app);

  // Post-process pass
  render_pass.clear_flags       = nikola::GFX_CLEAR_FLAGS_COLOR_BUFFER; 
  render_pass.shader_context_id = app->shader_contexts[3];
  nikola::renderer_push_pass(render_pass, post_process_pass, app);
}

static void render_app_ui(nikola::App* app) {
  if(!app->has_editor) {
    return;
  }

  nikola::gui_begin();

  nikola::gui_begin_panel("Debug");
  nikola::gui_settings_debug();
  nikola::gui_end_panel();
  
  // nikola::gui_begin_panel("Camera");
  // nikola::gui_settings_camera(&app->camera);
  // nikola::gui_end_panel();
  
  nikola::gui_begin_panel("Renderer");
  nikola::gui_settings_renderer();
  ImGui::SliderInt("Render Effect", &app->effect_index, 0, 7);
  nikola::gui_end_panel();
  
  // nikola::gui_begin_panel("Resources");
  // nikola::gui_edit_material("Material", app->material_id);
  // nikola::gui_end_panel();

  nikola::gui_begin_panel("Props");
  for(auto& prop : app->props) {
    prop.gui_render();
  }
  nikola::gui_end_panel();

  nikola::gui_end();
}

/// Private functions 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App functions 

nikola::App* app_init(const nikola::Args& args, nikola::Window* window) {
  // App init
  nikola::App* app = new nikola::App{};

  // Window init
  app->window = window;

  // Editor init
  nikola::gui_init(window);
  app->has_editor = false;

  // Camera init
  float aspect_ratio = nikola::window_get_aspect_ratio(app->window);
  nikola::camera_create(&app->camera, aspect_ratio, nikola::Vec3(10.0f, 0.0f, 10.0f), nikola::Vec3(-3.0f, 0.0f, 0.0f));

  // Resoruces init
  init_resources(app);

  // Props init
  init_props(app);

  // Render passes init
  init_passes(app);

  return app;
}

void app_shutdown(nikola::App* app) {
  nikola::resources_destroy_group(app->res_group_id);
  nikola::gui_shutdown();

  delete app;
}

void app_update(nikola::App* app, const nikola::f64 delta_time) {
  // Close the window when `ESCAPE` is pressed
  if(nikola::input_key_down(nikola::KEY_ESCAPE)) {
    nikola::event_dispatch(nikola::Event{.type = nikola::EVENT_APP_QUIT});
    return;
  }

  // Active/deactive the editor
  if(nikola::input_key_pressed(nikola::KEY_F1)) {
    app->has_editor = !app->has_editor;
    nikola::input_cursor_show(app->has_editor);
  }

  if(!app->has_editor) {
    nikola::camera_update(app->camera);
  }
} 

void app_render(nikola::App* app) {
  nikola::renderer_begin(app->camera);
  
  // Render the props 
  for(auto& prop : app->props) {
    prop.render(app->render_queue, app->shader_contexts[0], app->material_id);
  }
  
  // Render the skybox
  nikola::RenderCommand rnd_cmd;
  rnd_cmd.render_type       = nikola::RENDERABLE_TYPE_SKYBOX; 
  rnd_cmd.renderable_id     = app->skybox_id; 
  rnd_cmd.material_id       = app->material_id; 
  rnd_cmd.shader_context_id = app->shader_contexts[1]; 
  rnd_cmd.transform         = app->current_prop->transform; 
  nikola::render_queue_push(app->render_queue, rnd_cmd);

  // Go through all the passes and render them
  nikola::renderer_apply_passes(); 

  // Render UI
  render_app_ui(app); 

  // Render 2D 
  // nikola::batch_renderer_begin();

  // nikola::batch_render_quad(nikola::Vec2(100.0f), nikola::Vec2(128.0f), nikola::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
  // nikola::batch_render_texture(app->material->diffuse_map, nikola::Vec2(200.0f), nikola::Vec2(128.0f));

  // nikola::batch_renderer_end();

  nikola::renderer_end();
}

/// App functions 
/// ----------------------------------------------------------------------
