#include "app.hpp"

#include <nikola/nikola_core.hpp>
#include <nikola/nikola_engine.hpp>

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;
  nikola::Camera camera;
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
  nikola::camera_create(&app->camera, aspect_ratio, nikola::Vec3(10.0f, 0.0f, 10.0f), nikola::Vec3(-3.0f, 0.0f, 0.0f), nullptr);

  return app;
}

void app_shutdown(nikola::App* app) {
  nikola::memory_free(app);
}

void app_update(nikola::App* app) {
  if(nikola::input_key_down(nikola::KEY_ESCAPE)) {
    nikola::event_dispatch(nikola::Event{.type = nikola::EVENT_APP_QUIT});
    return;
  }

  nikola::camera_update(app->camera);
}

void app_render(nikola::App* app) {
  nikola::renderer_pre_pass(app->camera);
  nikola::renderer_begin_pass();
  
  nikola::renderer_end_pass();
  nikola::renderer_post_pass();
}

/// App functions 
/// ----------------------------------------------------------------------
