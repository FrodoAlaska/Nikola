#include "app.h"

#include <nikola/nikola.h>
#include <imgui/imgui.h>

/// ----------------------------------------------------------------------
/// App
struct nikola::App {
  nikola::Window* window;
  nikola::FrameData frame_data;

  nikola::ResourceGroupID res_group_id;
  nikola::ResourceID mesh_id, building_id;
  nikola::ResourceID font_id, material_id;

  nikola::Transform transforms[5];
  nikola::ParticleEmitter particle_emitter;

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

  // Skybox init
  app->frame_data.skybox_id = nikola::resources_push_skybox(app->res_group_id, "cubemaps/accurate_night.nbr");

  // Mesh init
  app->mesh_id = nikola::resources_push_mesh(app->res_group_id, nikola::GEOMETRY_CUBE);

  // Model init
  app->building_id = nikola::resources_push_model(app->res_group_id, "models/medieval_bridge.nbr");

  // Font init
  app->font_id = nikola::resources_push_font(app->res_group_id, "fonts/bit5x3.nbr");

  // Material init
  
  nikola::MaterialDesc mat_desc = {
    .albedo_id = nikola::resources_push_texture(app->res_group_id, "textures/paviment.nbr"),
  };
  app->material_id = nikola::resources_push_material(app->res_group_id, mat_desc);
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

  // Camera init
  
  nikola::CameraDesc cam_desc = {
    .position     = nikola::Vec3(-40.0f, 2.5f, -18.0f),
    .target       = nikola::Vec3(-3.0f, 2.5f, 0.0f),
    .up_axis      = nikola::Vec3(0.0f, 1.0f, 0.0f),
    .aspect_ratio = nikola::window_get_aspect_ratio(app->window),
  };

  app->frame_data.camera.exposure = 1.0f;
  nikola::camera_create(&app->frame_data.camera, cam_desc);

  // Resoruces init
  init_resources(app);

  // Transform init
  
  nikola::transform_translate(app->transforms[0], nikola::Vec3(5.0f, 0.05f, 5.0f));
  nikola::transform_scale(app->transforms[0], nikola::Vec3(64.0f, 0.1f, 64.0f));
  
  nikola::transform_translate(app->transforms[1], nikola::Vec3(5.0f, 0.2f, 70.0f));
  nikola::transform_scale(app->transforms[1], nikola::Vec3(10.0f));
  nikola::transform_rotate(app->transforms[1], nikola::Vec3(TO_RADIANS(-90.0f), 0.0f, 0.0f));

  // Lights init

  app->frame_data.dir_light.direction = nikola::Vec3(1.0f, -1.0f, 1.0f);
  app->frame_data.dir_light.color     = nikola::Vec3(1.0f);

  app->frame_data.ambient = nikola::Vec3(1.0f);
 
  // Particle emitter init

  nikola::MaterialDesc mat_desc = {
    .color = nikola::Vec3(1.0f, 0.0f, 0.0f), 
  };
   
  nikola::ParticleEmitterDesc emitter_desc = {
    .position = nikola::Vec3(-6.0f, 25.0f, 5.0f),
    .velocity = nikola::Vec3(5.0f),
    .scale    = nikola::Vec3(0.2f),

    .mesh_id     = nikola::resources_push_mesh(app->res_group_id, nikola::GEOMETRY_SIMPLE_CUBE),
    .material_id = nikola::resources_push_material(app->res_group_id, mat_desc),

    .lifetime       = 3.5f,
    .gravity_factor = -9.81f,

    .count = 16,
  };
  nikola::particle_emitter_create(&app->particle_emitter, emitter_desc);

  nikola::RenderPass* debug_pass = nikola::renderer_peek_pass(nikola::RENDER_PASS_DEBUG);
  nikola::renderer_insert_pass(debug_pass, nikola::RENDER_PASS_PARTICLE);

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
  
  // Emit particles

  if(nikola::input_key_pressed(nikola::KEY_SPACE)) {
    nikola::particle_emitter_emit(app->particle_emitter);
  }

  // Update the particle emitter 
  nikola::particle_emitter_update(app->particle_emitter, delta_time);

  // Update the camera
  
  nikola::camera_free_move_func(app->frame_data.camera);
  nikola::camera_update(app->frame_data.camera);
}

void app_render(nikola::App* app) {
  // Render 3D 
  nikola::renderer_begin(app->frame_data);

  // Render the objects
  
  nikola::renderer_queue_mesh(app->mesh_id, app->transforms[0], app->material_id);
  nikola::renderer_queue_model(app->building_id, app->transforms[1]);

  // @TEMP: Render the particles
  
  if(app->particle_emitter.is_active) {
    nikola::renderer_queue_particles(app->particle_emitter);
    // nikola::renderer_queue_mesh_instanced(app->particle_emitter.mesh_id, 
    //                                       app->particle_emitter.transforms, 
    //                                       app->particle_emitter.particles_count, 
    //                                       app->particle_emitter.material_id);
  }

  nikola::renderer_queue_debug_cube(app->transforms[0]);

  nikola::renderer_end();
  
  // Render 2D 
  
  nikola::batch_renderer_begin();
  nikola::batch_render_fps(nikola::resources_get_font(app->font_id), nikola::Vec2(10.0f, 32.0f), 32.0f, nikola::Vec4(1.0f)); 
  nikola::batch_renderer_end();
}

void app_render_gui(nikola::App* app) {
  if(!app->has_editor) {
    return;
  }

  nikola::gui_begin();
  nikola::gui_begin_panel("Scene");

  // Frame 
  nikola::gui_edit_frame("Frame", &app->frame_data);

  // Entities
  
  if(ImGui::CollapsingHeader("Entities")) {
    nikola::gui_edit_transform("Mesh", &app->transforms[0]);
    nikola::gui_edit_transform("Model", &app->transforms[1]);
    nikola::gui_edit_particle_emitter("Particles", &app->particle_emitter);
  }

  // Resources
  
  if(ImGui::CollapsingHeader("Resources")) {
    nikola::gui_edit_material("Material", nikola::resources_get_material(app->material_id));
  }

  nikola::gui_end_panel();
  nikola::gui_end();
}


/// App functions 
/// ----------------------------------------------------------------------
