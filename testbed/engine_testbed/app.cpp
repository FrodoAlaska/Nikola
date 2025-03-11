#include "app.hpp"
#include "shaders.hpp"

#include <nikola/nikola_core.hpp>
#include <nikola/nikola_engine.hpp>
#include <nikola/nikola_ui.hpp>

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

  void render(nikola::ResourceID& material_id, const nikola::sizei count = 1) {
    for(nikola::sizei i = 0; i < count; i++) {
      nikola::RenderCommand rnd_cmd {
        .render_type     = renderable_type,
        
        .renderable_id   = renderable_id, 
        .material_id     = material_id, 
        
        .transform       = transform,
      };

      nikola::renderer_queue_command(rnd_cmd);
    }
  }

  void gui_render() {
    nikola::gui_settings_transform(debug_name.c_str(), &transform);
  }
};
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;
  nikola::Camera camera;

  bool has_editor;

  nikola::u16 res_group_id;

  nikola::ResourceID skybox_material_id, material_id;
  nikola::ResourceID skybox_id;
  nikola::DynamicArray<nikola::ResourceID> skyboxes;

  nikola::DynamicArray<Prop> props;
  Prop* current_prop = nullptr;
};
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

static nikola::f32 ease_in_out(const nikola::f32 x) {
  return -(nikola::cos(nikola::PI * x) - 1) / 2.0f;
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
  nikola::gui_end_panel();
  
  // nikola::gui_begin_panel("Resources");
  // nikola::gui_settings_material("Material", app->material_id);
  // nikola::gui_end_panel();

  nikola::gui_begin_panel("Props");
  app->current_prop->gui_render();
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

  // Resource storage init 
  nikola::FilePath current_path = nikola::filesystem_current_path();
  nikola::FilePath res_path = nikola::filepath_append(current_path, "res");
  app->res_group_id = nikola::resources_create_group("app_res", res_path);

  // Resoruces init
  nikola::resources_push_dir(app->res_group_id, "textures");
  nikola::resources_push_dir(app->res_group_id, "shaders");
  nikola::resources_push_dir(app->res_group_id, "cubemaps");
  nikola::resources_push_dir(app->res_group_id, "models");

  // Skybox init
  app->skyboxes.push_back(nikola::resources_push_skybox(app->res_group_id, nikola::resources_get_id(app->res_group_id, "gloomy")));
  app->skyboxes.push_back(nikola::resources_push_skybox(app->res_group_id, nikola::resources_get_id(app->res_group_id, "NightSky")));
  app->skyboxes.push_back(nikola::resources_push_skybox(app->res_group_id, nikola::resources_get_id(app->res_group_id, "desert_cubemap")));
  app->skybox_id = app->skyboxes[1];

  // Material init
  app->material_id = nikola::resources_push_material(app->res_group_id, nikola::resources_get_id(app->res_group_id, "default3d"));

  // Skybox material init
  app->skybox_material_id = nikola::resources_push_material(app->res_group_id, nikola::resources_get_id(app->res_group_id, "cubemap"));

  // Mesh init
  nikola::ResourceID mesh_id = nikola::resources_push_mesh(app->res_group_id, nikola::MESH_TYPE_CUBE);

  nikola::Vec3 default_pos(10.0f, 0.0f, 10.0f);
  
  // Porps init
  app->props.push_back(Prop("behelit", default_pos, nikola::Vec3(0.1f), nikola::RENDERABLE_TYPE_MODEL, nikola::resources_get_id(app->res_group_id, "behelit")));
  app->props.push_back(Prop("bridge", default_pos, nikola::Vec3(1.0f), nikola::RENDERABLE_TYPE_MODEL, nikola::resources_get_id(app->res_group_id, "bridge")));
  app->props.push_back(Prop("tempel", default_pos, nikola::Vec3(1.0f), nikola::RENDERABLE_TYPE_MODEL, nikola::resources_get_id(app->res_group_id, "tempel")));
  app->props.push_back(Prop("moon", default_pos, nikola::Vec3(1.0f), nikola::RENDERABLE_TYPE_MESH, mesh_id));

  app->current_prop = &app->props[0];

  return app;
}

void app_shutdown(nikola::App* app) {
  nikola::resources_destroy_group(app->res_group_id);
  nikola::gui_shutdown();

  delete app;
}

static float rotation_angle = 0.0f;

void app_update(nikola::App* app, const nikola::f64 delta_time) {
  // Close the window when `ESCAPE` is pressed
  if(nikola::input_key_down(nikola::KEY_ESCAPE)) {
    nikola::event_dispatch(nikola::Event{.type = nikola::EVENT_APP_QUIT});
    return;
  }

  rotation_angle += (nikola::f64)nikola::niclock_get_delta_time();
  nikola::transform_rotate(app->props[3].transform, nikola::Vec3(0.0f, 1.0f, 0.0f), rotation_angle);

  // Active/deactive the editor
  if(nikola::input_key_pressed(nikola::KEY_F1)) {
    app->has_editor = !app->has_editor;
    nikola::input_cursor_show(app->has_editor);
  }

  if(!app->has_editor) {
    nikola::camera_update(app->camera);
  }

  if(nikola::input_key_pressed(nikola::KEY_Q)) {
    nikola::renderer_apply_effect(nikola::RENDER_EFFECT_PIXELIZE);
  }
  else if(nikola::input_key_pressed(nikola::KEY_E)) {
    nikola::renderer_apply_effect(nikola::RENDER_EFFECT_NONE);
  }

  if(nikola::input_key_pressed(nikola::KEY_1)) {
    app->current_prop  = &app->props[0];
    app->skybox_id     = app->skyboxes[0]; 
  }
  else if(nikola::input_key_pressed(nikola::KEY_2)) {
    app->current_prop  = &app->props[1];
    app->skybox_id     = app->skyboxes[1]; 
  }
  else if(nikola::input_key_pressed(nikola::KEY_3)) {
    app->current_prop  = &app->props[2];
    app->skybox_id     = app->skyboxes[2]; 
  }
} 

void app_render(nikola::App* app) {
  // Begin rendering objects
  nikola::RenderData render_dat = {
    .camera = app->camera,
  };
  nikola::renderer_begin_pass(render_dat);

  // Render the cubes
  // rnd_cmd.render_type   = nikola::RENDERABLE_TYPE_MESH, 
  // rnd_cmd.renderable_id = app->mesh_id,
  // constexpr int MESHES_MAX = 10;
  //
  // for(int i = 0; i < MESHES_MAX; i++) {
  //   for(int j = 0; j < MESHES_MAX; j++) {
  //     nikola::transform_translate(app->transform, nikola::Vec3(i * 2.0f, 0.0f, j * 2.0f));
  //     
  //     rnd_cmd.transform = app->transform;
  //     nikola::renderer_queue_command(rnd_cmd);
  //   }
  // }

  // Render the prop
  nikola::material_set_texture(app->material_id, nikola::MATERIAL_TEXTURE_DIFFUSE, nikola::resources_get_id(app->res_group_id, "opengl")); 
  app->current_prop->render(app->material_id);

  // Render the skybox 
  nikola::RenderCommand rnd_cmd;
  rnd_cmd.render_type   = nikola::RENDERABLE_TYPE_SKYBOX; 
  rnd_cmd.renderable_id = app->skybox_id; 
  rnd_cmd.material_id   = app->skybox_material_id; 
  rnd_cmd.transform     = app->current_prop->transform; 
  nikola::renderer_queue_command(rnd_cmd);

  // Render the objects
  nikola::renderer_end_pass();
  
  // Render UI
  render_app_ui(app); 

  // Render 2D 
  // nikola::batch_renderer_begin();

  // nikola::batch_render_quad(nikola::Vec2(100.0f), nikola::Vec2(128.0f), nikola::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
  // nikola::batch_render_texture(app->material->diffuse_map, nikola::Vec2(200.0f), nikola::Vec2(128.0f));

  // nikola::batch_renderer_end();

  // End of the render loop...
  nikola::renderer_present();
}

/// App functions 
/// ----------------------------------------------------------------------
