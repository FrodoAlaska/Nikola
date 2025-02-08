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
  nikola::ResourceID mesh_id, material_id;
  nikola::Transform transform;
};
/// App
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// App functions 

nikola::App* app_init(nikola::Window* window) {
  // App init
  nikola::App* app = (nikola::App*)nikola::memory_allocate(sizeof(nikola::App));
  nikola::memory_zero(app, sizeof(nikola::App));
  
  // Window init
  app->window = window;

  // Camera init
  float aspect_ratio = nikola::window_get_aspect_ratio(app->window);
  nikola::camera_create(&app->camera, aspect_ratio, nikola::Vec3(10.0f, 0.0f, 10.0f), nikola::Vec3(-3.0f, 0.0f, 0.0f));

  // Resource storage init
  app->storage = nikola::resource_storage_create("app_res", "assets");

  // Mesh init
  nikola::MeshLoader mesh_loader;
  nikola::mesh_loader_load(app->storage, &mesh_loader, nikola::MESH_TYPE_CUBE);
  app->mesh_id = nikola::resource_storage_push(app->storage, mesh_loader);

  // Diffuse texture init
  nikola::GfxTextureDesc diffuse_desc;
  nikola::texture_loader_load(&diffuse_desc, "assets/opengl.png");
  
  // Material init
  nikola::MaterialLoader mat_loader;
  nikola::material_loader_load(app->storage, &mat_loader, diffuse_desc, diffuse_desc, default3d_shader());
  app->material_id = nikola::resource_storage_push(app->storage, mat_loader);

  // Transform init
  nikola::transform_translate(app->transform, nikola::Vec3(10.0f, 0.0f, 10.0f));

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
  };

  constexpr int MESHES_MAX = 10;
  rotation_angle += 0.1f;
  float x = 1.0f + nikola::sin(nikola::niclock_get_time()) * 2.0f; 
  float y = nikola::sin(nikola::niclock_get_time() / 2.0f) * 1.0f;

  for(int i = 0; i < MESHES_MAX; i++) {
    for(int j = 0; j < MESHES_MAX; j++) {
      nikola::transform_translate(app->transform, nikola::Vec3(i * 2.0f, 0.0f, j * 2.0f));
    
      rnd_cmd.transform = app->transform;
      nikola::renderer_queue_command(app->storage, rnd_cmd);
    }
  }

  nikola::renderer_end_pass();
  nikola::renderer_post_pass();
}

/// App functions 
/// ----------------------------------------------------------------------
