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
  nikola::ResourceID mesh_id, material_id, matrices_buffer_id;
  nikola::Transform transform;

  nikola::ResourceID skybox_id, skybox_material_id;
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
  app->storage = nikola::resource_storage_create("app_res", "assets");

  // Mesh init
  nikola::MeshLoader mesh_loader;
  nikola::mesh_loader_load(app->storage, &mesh_loader, nikola::MESH_TYPE_CUBE);
  app->mesh_id = nikola::resource_storage_push(app->storage, mesh_loader);

  // Diffuse texture init
  nikola::GfxTextureDesc diffuse_desc;
  nikola::texture_loader_load(&diffuse_desc, "assets/opengl.png");
  nikola::ResourceID diffuse_id = nikola::resource_storage_push(app->storage, diffuse_desc); 

  // Cubemap texture init
  nikola::GfxCubemapDesc cubemap_desc;
  nikola::cubemap_loader_load(&cubemap_desc, "assets/NightSky", 6);
  nikola::ResourceID cubemap_id = nikola::resource_storage_push(app->storage, cubemap_desc);

  // Skybox init
  nikola::SkyboxLoader sky_loader;
  nikola::skybox_loader_load(app->storage, &sky_loader, cubemap_id);
  app->skybox_id = nikola::resource_storage_push(app->storage, sky_loader);

  // Shader init
  nikola::ResourceID shader_id = nikola::resource_storage_push(app->storage, default3d_shader());
  nikola::ResourceID sky_shader_id = nikola::resource_storage_push(app->storage, cubemap_shader_glsl());

  // Matrices buffer init
  nikola::GfxBufferDesc matrices_desc = {
    .data  = nullptr, 
    .size  = sizeof(nikola::Mat4),
    .type  = nikola::GFX_BUFFER_UNIFORM, 
    .usage = nikola::GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  nikola::ResourceID matrices_id = nikola::resource_storage_push(app->storage, matrices_desc);

  // Material init
  nikola::MaterialLoader mat_loader;
  nikola::material_loader_load(app->storage, &mat_loader, diffuse_id, nikola::INVALID_RESOURCE, shader_id);
  nikola::material_loader_attach_uniform(app->storage, mat_loader, nikola::MATERIAL_MATRICES_BUFFER_INDEX, matrices_id);
  app->material_id = nikola::resource_storage_push(app->storage, mat_loader);

  // Skybox material init
  nikola::material_loader_load(app->storage, &mat_loader, diffuse_id, nikola::INVALID_RESOURCE, sky_shader_id);
  nikola::material_loader_attach_uniform(app->storage, mat_loader, nikola::MATERIAL_MATRICES_BUFFER_INDEX, matrices_id);
  app->skybox_material_id = nikola::resource_storage_push(app->storage, mat_loader);

  // Transform init
  nikola::transform_translate(app->transform, nikola::Vec3(10.0f, 0.0f, 10.0f));
  nikola::transform_scale(app->transform, nikola::Vec3(1.0f));

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
  rotation_angle += 0.1f;
  float x = 1.0f + nikola::sin(nikola::niclock_get_time()) * 2.0f; 
  float y = nikola::sin(nikola::niclock_get_time() / 2.0f) * 1.0f;

  // Render the cubes
  for(int i = 0; i < MESHES_MAX; i++) {
    for(int j = 0; j < MESHES_MAX; j++) {
      nikola::transform_translate(app->transform, nikola::Vec3(i * 2.0f, 0.0f, j * 2.0f));
      
      rnd_cmd.transform = app->transform;
      nikola::renderer_queue_command(rnd_cmd);
    }
  }

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
