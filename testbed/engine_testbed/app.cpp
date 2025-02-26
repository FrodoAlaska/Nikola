#include "app.hpp"
#include "shaders.hpp"

#include <nikola/nikola_core.hpp>
#include <nikola/nikola_engine.hpp>

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;
  nikola::Camera camera;

  nikola::ResourceStorage* storage;
  nikola::Transform transform;

  nikola::ResourceID mesh_id, material_id;
  nikola::ResourceID skybox_id, skybox_material_id;
  nikola::ResourceID model_id;
};
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App functions 

nikola::App* app_init(const nikola::Args& args, nikola::Window* window) {
  // App init
  nikola::App* app = (nikola::App*)nikola::memory_allocate(sizeof(nikola::App));
  nikola::memory_zero(app, sizeof(nikola::App));

  // Window init
  app->window = window;

  // Camera init
  float aspect_ratio = nikola::window_get_aspect_ratio(app->window);
  nikola::camera_create(&app->camera, aspect_ratio, nikola::Vec3(10.0f, 0.0f, 10.0f), nikola::Vec3(-3.0f, 0.0f, 0.0f));

  // Resource storage init 
  app->storage = nikola::resource_storage_create("app_res", std::filesystem::current_path() / "res" / "nbr");

  // Transform init
  nikola::transform_translate(app->transform, nikola::Vec3(10.0f, 0.0f, 10.0f));
  nikola::transform_scale(app->transform, nikola::Vec3(1.0f));

  // Mesh init
  app->mesh_id = nikola::resource_storage_push_mesh(app->storage, nikola::MESH_TYPE_CUBE);

  // Diffuse texture init
  nikola::ResourceID diffuse_id = nikola::resource_storage_push_texture(app->storage, nikola::FilePath("dx11.nbr")); 

  // Cubemap texture init
  nikola::ResourceID cubemap_id = nikola::resource_storage_push_cubemap(app->storage, nikola::FilePath("NightSky.nbr"));

  // Skybox init
  app->skybox_id = nikola::resource_storage_push_skybox(app->storage, cubemap_id);

  // Shader init
  nikola::ResourceID shader_id     = nikola::resource_storage_push_shader(app->storage, nikola::FilePath("default3d.nbr"));
  nikola::ResourceID sky_shader_id = nikola::resource_storage_push_shader(app->storage, nikola::FilePath("cubemap.nbr"));

  // Material init
  app->material_id = nikola::resource_storage_push_material(app->storage, diffuse_id, nikola::INVALID_RESOURCE, shader_id);

  // Skybox material init
  app->skybox_material_id = nikola::resource_storage_push_material(app->storage, diffuse_id, nikola::INVALID_RESOURCE, sky_shader_id);

  // Model init
  app->model_id = nikola::resource_storage_push_model(app->storage, "Karner_C.nbr");

  return app;
}

void app_shutdown(nikola::App* app) {
  nikola::resource_storage_destroy(app->storage);
  nikola::memory_free(app);
}

void app_update(nikola::App* app) {
  if(nikola::input_key_down(nikola::KEY_ESCAPE)) {
    nikola::event_dispatch(nikola::Event{.type = nikola::EVENT_APP_QUIT});
    return;
  }

  nikola::camera_update(app->camera);
}

static float rotation_angle = 0.0f;

void app_render(nikola::App* app) {
  nikola::renderer_pre_pass(app->camera);
  nikola::renderer_begin_pass();
 
  nikola::RenderCommand rnd_cmd = {
    .render_type   = nikola::RENDERABLE_TYPE_MESH, 
    .renderable_id = app->mesh_id,
    .material_id   = app->material_id,
    .transform     = app->transform, 
    .storage       = app->storage,
  };

  constexpr int MESHES_MAX = 10;
  rotation_angle += 0.01f;
  float x = 1.0f + nikola::sin(nikola::niclock_get_time()) * 2.0f; 
  float y = nikola::sin(nikola::niclock_get_time() / 2.0f) * 1.0f;

  // Render the cubes
  // for(int i = 0; i < MESHES_MAX; i++) {
  //   for(int j = 0; j < MESHES_MAX; j++) {
  //     nikola::transform_translate(app->transform, nikola::Vec3(i * 2.0f, 0.0f, j * 2.0f));
  //     
  //     rnd_cmd.transform = app->transform;
  //     nikola::renderer_queue_command(rnd_cmd);
  //   }
  // }

  nikola::transform_translate(rnd_cmd.transform, nikola::Vec3(10.0f, 0.0f, 10.0f));
  nikola::transform_scale(rnd_cmd.transform, nikola::Vec3(1.0f));
  nikola::transform_rotate(rnd_cmd.transform, nikola::Vec3(1.0f, 0.0f, 0.0f), -90.0f * nikola::DEG2RAD);

  // Render the model
  rnd_cmd.render_type   = nikola::RENDERABLE_TYPE_MODEL; 
  rnd_cmd.renderable_id = app->model_id; 
  rnd_cmd.material_id   = app->material_id; 
  nikola::renderer_queue_command(rnd_cmd);
  
  // Render the skybox 
  rnd_cmd.render_type   = nikola::RENDERABLE_TYPE_SKYBOX; 
  rnd_cmd.renderable_id = app->skybox_id; 
  rnd_cmd.material_id   = app->skybox_material_id; 
  nikola::renderer_queue_command(rnd_cmd);

  nikola::renderer_end_pass();
  nikola::renderer_post_pass();
}

/// App functions 
/// ----------------------------------------------------------------------
