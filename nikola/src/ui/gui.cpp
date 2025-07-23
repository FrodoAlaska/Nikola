#include "nikola/nikola_ui.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_resources.h"
#include "nikola/nikola_render.h"
#include "nikola/nikola_math.h"
#include "nikola/nikola_audio.h"
#include "nikola/nikola_physics.h"
#include "nikola/nikola_input.h"

#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_opengl3_loader.h>

#include <glm/gtc/quaternion.hpp>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// GUIState
struct GUIState {
  Window* window = nullptr; 
  ImGuiIO io_config;
 
  Vec4 render_clear_color = Vec4(1.0f);

  f64 fps              = 0.0;
  IVec2 window_size    = IVec2(0); 
  Vec2 mouse_position  = Vec2(0.0f); 
  Vec2 mouse_offset    = Vec2(0.0f); 

  sizei allocations_count = 0; 
  sizei allocation_bytes  = 0;

  HashMap<const char*, Vec3> rotations;
};

static GUIState s_gui;
/// GUIState
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GUI functions

bool gui_init(Window* window) {
  // Setting default values for the GUI
  s_gui = GUIState{};
  s_gui.window = window;

  // Set up ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  s_gui.io_config = ImGui::GetIO();

  // Setting context flags
  s_gui.io_config.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  // Dark mode WOOOOOOOAH! 
  ImGui::StyleColorsDark();

  // Setting up the glfw backend
  if(!ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)window_get_handle(window), true)) {
    NIKOLA_LOG_ERROR("Failed to initialize GLFW for ImGui");
    return false;
  }
  
  // Setting up the opengl backend
  if(!ImGui_ImplOpenGL3_Init("#version 460 core")) {
    NIKOLA_LOG_ERROR("Failed to initialize OpenGL for ImGui");
    return false;
  }

  return true;
}

void gui_shutdown() {
  ImGui_ImplGlfw_Shutdown();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui::DestroyContext();
}

void gui_begin() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void gui_end() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool gui_begin_panel(const char* name) {
  return ImGui::Begin(name);
}

void gui_end_panel() {
  ImGui::End();
}

const bool gui_is_focused() {
  return ImGui::GetIO().WantCaptureMouse;
}

void gui_renderer_info() {
  if(!ImGui::Begin("Renderer Info")) {
    ImGui::End();
    return;
  }

  // Stats
  // -------------------------------------------------------------------
  ImGui::SeparatorText("Stats");
  // -------------------------------------------------------------------
 
  // Editables
  // -------------------------------------------------------------------
  ImGui::SeparatorText("##xx");
 
  // Clear color
  ImGui::ColorPicker4("Clear color", &s_gui.render_clear_color[0], ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
  renderer_set_clear_color(s_gui.render_clear_color);
  // -------------------------------------------------------------------

  ImGui::End();
}

void gui_debug_info() {
  if(!ImGui::Begin("Debug Info")) {
    ImGui::End();
    return;
  }

  // FPS 
  // -------------------------------
  if(ImGui::CollapsingHeader("Frames")) {
    s_gui.fps = niclock_get_fps();
    ImGui::Text("FPS: %f", s_gui.fps);
  } 
  // -------------------------------

  // Mouse
  // -------------------------------
  if(ImGui::CollapsingHeader("Mouse")) {
    input_mouse_position(&s_gui.mouse_position.x, &s_gui.mouse_position.y);
    input_mouse_offset(&s_gui.mouse_offset.x, &s_gui.mouse_offset.y);

    ImGui::Text("Position: %s", vec2_to_string(s_gui.mouse_position).c_str());
    ImGui::Text("Offset: %s", vec2_to_string(s_gui.mouse_offset).c_str());
  } 
  // -------------------------------

  // Window
  // -------------------------------
  if(ImGui::CollapsingHeader("Window")) {
    window_get_size(s_gui.window, &s_gui.window_size.x, &s_gui.window_size.y);

    ImGui::Text("Size: %s", vec2_to_string(s_gui.window_size).c_str());
  } 
  // -------------------------------

  // Memory
  // -------------------------------
  if(ImGui::CollapsingHeader("Memory")) {
    s_gui.allocations_count = memory_get_allocations_count();
    s_gui.allocation_bytes  = memory_get_allocation_bytes();

    ImGui::Text("Allocations: %zu", s_gui.allocations_count);
    ImGui::Text("Bytes allocated: %zu", s_gui.allocation_bytes);
  } 
  // -------------------------------

  ImGui::End();
}

void gui_edit_color(const char* name, Vec4& color) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 

  ImGui::ColorEdit4(name, &color[0]);

  ImGui::PopID();
}

void gui_edit_transform(const char* name, Transform* transform) {
  if(s_gui.rotations.find(name) == s_gui.rotations.end()) {
    s_gui.rotations[name] = Vec3(0.0f);
  }

  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 
 
  // SRT translation
  // -------------------------------------------------------------------
  if(ImGui::DragFloat3("Position", &transform->position[0], 0.01f)) {
    transform_translate(*transform, transform->position);
  }

  if(ImGui::DragFloat3("Scale", &transform->scale[0], 0.01f)) {
    transform_scale(*transform, Vec3(transform->scale));
  }

  if(ImGui::DragFloat3("Rotation", &s_gui.rotations[name][0], 0.1f)) {
    Vec3 axis = vec3_normalize(s_gui.rotations[name]);
    transform_rotate(*transform, axis, s_gui.rotations[name].x);
  }
  // -------------------------------------------------------------------
  
  ImGui::PopID(); 
}

void gui_edit_camera(const char* name, Camera* camera) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 
  
  // Information
  // -------------------------------------------------------------------
  ImGui::Text("Yaw: %f", camera->yaw);
  ImGui::Text("Pitch: %f", camera->pitch);
  // -------------------------------------------------------------------

  // Editables
  // -------------------------------------------------------------------
  ImGui::DragFloat3("Postiion", &camera->position[0], 1.0f);  
  ImGui::SliderFloat("Zoom", &camera->zoom, CAMERA_MAX_ZOOM, 0.0f, "Zoom: %.3f");
  ImGui::SliderFloat("Near", &camera->near, 0.1f, 1000.0f, "Near: %.3f");
  ImGui::SliderFloat("Far", &camera->far, 0.1f, 1000.0f, "Far: %.3f");
  ImGui::SliderFloat("Sensitivity", &camera->sensitivity, 0.0f, 1.0f, "Sensitivity: %.3f");
  ImGui::SliderFloat("Exposure", &camera->exposure, 0.0f, 10.0f, "Exposure: %.3f");
  // -------------------------------------------------------------------
  
  ImGui::PopID(); 
}

void gui_edit_directional_light(const char* name, DirectionalLight* dir_light) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 
  
  ImGui::DragFloat3("Direction", &dir_light->direction[0], 0.01f, -1.0f, 1.0f);
  ImGui::DragFloat3("Color", &dir_light->color[0], 0.01f, 0.0f);

  ImGui::PopID(); 
}

void gui_edit_point_light(const char* name, PointLight* point_light) {
  ImGui::SeparatorText(name);
  ImGui::PushID(name); 

  ImGui::DragFloat3("Position", &point_light->position[0], 1.0f);
  ImGui::DragFloat3("Color", &point_light->color[0], 0.01f, 0.0f);
  ImGui::SliderFloat("Radius", &point_light->radius, 0.001f, 20.0f);

  ImGui::PopID(); 
}

void gui_edit_spot_light(const char* name, SpotLight* spot_light) {
  ImGui::SeparatorText(name);
  ImGui::PushID(name); 

  ImGui::DragFloat3("Position", &spot_light->position[0], 1.0f);
  ImGui::DragFloat3("Direction", &spot_light->direction[0], 0.01f, -1.0f, 1.0f);
  ImGui::DragFloat3("Color", &spot_light->color[0], 0.01f, 0.0f);
  ImGui::SliderFloat("Radius", &spot_light->radius, 0.0f, 1.0f);
  ImGui::SliderFloat("Outer radius", &spot_light->outer_radius, 0.0f, 1.0f);

  ImGui::PopID(); 
}

void gui_edit_material(const char* name, Material* material) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 

  ImGui::ColorEdit3("Color", &material->color[0]);
  ImGui::ColorEdit4("Blend factor", &material->blend_factor[0]);
  
  ImGui::DragFloat("Shininess", &material->shininess, 0.01f, 0.0f);
  ImGui::DragFloat("Transparency", &material->transparency, 0.01f, 0.0f, 1.0f);
  ImGui::DragInt("Stencil reference", &material->stencil_ref, 1.0f);
  ImGui::Checkbox("Depth Mask", &material->depth_mask);
  
  ImGui::PopID(); 
}

void gui_edit_font(const char* name, Font* font, String* label) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 

  // Font info 
  // -------------------------------------------------------------------
  if(ImGui::CollapsingHeader("Font Information")) {
    ImGui::SliderFloat("Ascent", &font->ascent, -1000, 1000);
    ImGui::SliderFloat("Descent", &font->descent, -1000, 1000);
    ImGui::SliderFloat("Line Gap", &font->line_gap, -1000, 1000);
  } 
  // -------------------------------------------------------------------
 
  // Label info
  // -------------------------------------------------------------------
  if(ImGui::CollapsingHeader("Input Label")) {
    ImGui::InputTextMultiline("Label", label);
  } 
  // -------------------------------------------------------------------

  // Glyphs Info
  // -------------------------------------------------------------------
  if(ImGui::CollapsingHeader("Glyphs")) {
    for(auto& ch : *label) {
      Glyph* glyph = &font->glyphs[ch]; 
      
      String str_id = ("Char: " + ch);
      ImGui::PushID(str_id.c_str());
      
      ImGui::Text("Unicode: %c", glyph->unicode);
      
      ImGui::SliderFloat2("Size", &glyph->size[0], -1000, 1000);
      ImGui::SliderFloat2("Offset", &glyph->offset[0], -1000, 1000);
      
      ImGui::Text("Bounds  = {T: %i, L: %i, B: %i, R: %i}", glyph->top, glyph->left, glyph->bottom, glyph->right);
      
      ImGui::SliderInt("Advance", &glyph->advance_x, -1000, FLOAT_MAX);
      ImGui::SliderInt("Kern", &glyph->kern, -1000, FLOAT_MAX);
      ImGui::SliderInt("Left Side Bearing", &glyph->left_bearing, -1000, FLOAT_MAX);

      ImGui::Separator();
      ImGui::PopID();
    }
  }
  // -------------------------------------------------------------------
  
  ImGui::PopID(); 
}

void gui_edit_audio_source(const char* name, AudioSourceID& source) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 

  AudioSourceDesc source_desc = audio_source_get_desc(source);

  // Source Info
  // -------------------------------------------------------------------
  // Volume
  if(ImGui::SliderFloat("Volume", &source_desc.volume, 0.0f, 1.0f)) {
    audio_source_set_volume(source, source_desc.volume);
  }

  // Pitch
  if(ImGui::SliderFloat("Pitch", &source_desc.pitch, 0.0f, 1.0f)) {
    audio_source_set_pitch(source, source_desc.pitch);
  }

  // Position
  if(ImGui::DragFloat3("Position", &source_desc.position[0], 0.01f, -100.0f, 100.0f)) {
    audio_source_set_position(source, source_desc.position);
  }

  // Velocity
  if(ImGui::DragFloat3("Velocity", &source_desc.velocity[0], 0.01f, -1.0f, 1.0f)) {
    audio_source_set_velocity(source, source_desc.velocity);
  }
  
  // Direction
  if(ImGui::DragFloat3("Direction", &source_desc.direction[0], 0.01f, -1.0f, 1.0f)) {
    audio_source_set_direction(source, source_desc.direction);
  }
  
  // Looping
  if(ImGui::Checkbox("Looping", &source_desc.is_looping)) {
    audio_source_set_looping(source, source_desc.is_looping);
  }

  // Queued buffers 
  ImGui::Text("Current queued buffers %zu", source_desc.buffers_count);
  // -------------------------------------------------------------------

  // Command buttons
  // -------------------------------------------------------------------
  // Play button
  if(ImGui::Button("Play")) {
    audio_source_start(source);
  } 

  ImGui::SameLine();
  
  // Pause button
  if(ImGui::Button("Pause")) {
    audio_source_pause(source);
  } 
  
  ImGui::SameLine();
  
  // Stop button
  if(ImGui::Button("Stop")) {
    audio_source_stop(source);
  } 
  
  ImGui::SameLine();

  // Restart button
  if(ImGui::Button("Restart")) {
    audio_source_restart(source);
    audio_source_start(source);
  } 
  
  ImGui::SameLine();
  // -------------------------------------------------------------------

  ImGui::NewLine();
  ImGui::PopID(); 
}

void gui_edit_audio_listener(const char* name) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 

  AudioListenerDesc listener = audio_listener_get_desc();

  // Volume
  if(ImGui::SliderFloat("Volume", &listener.volume, 0.0f, 1.0f)) {
    audio_listener_set_volume(listener.volume);
  }

  // Position
  if(ImGui::DragFloat3("Position", &listener.position[0], 0.01f, -1.0f, 1.0f)) {
    audio_listener_set_position(listener.position);
  }

  // Velocity
  if(ImGui::DragFloat3("Velocity", &listener.velocity[0], 0.01f, -1.0f, 1.0f)) {
    audio_listener_set_velocity(listener.velocity);
  }

  ImGui::PopID(); 
}

void gui_edit_physics_body(const char* name, PhysicsBody* body) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 
  
  // Position
  Vec3 position = physics_body_get_position(body);
  if(ImGui::DragFloat3("Position", &position[0])) {
    physics_body_set_position(body, position);
  }

  // Linear velocity
  Vec3 linear = physics_body_get_linear_velocity(body);
  if(ImGui::DragFloat3("Linear velocity", &linear[0], 0.1f)) {
    physics_body_set_linear_velocity(body, linear);
  }
  
  // Angular velocity
  Vec3 angular = physics_body_get_angular_velocity(body);
  if(ImGui::DragFloat3("Angular velocity", &angular[0], 0.1f)) {
    physics_body_set_angular_velocity(body, angular);
  }
  
  // Awake
  bool awake = physics_body_is_awake(body);
  if(ImGui::Checkbox("Awake", &awake)) {
    physics_body_set_awake(body, awake);
  }

  ImGui::PopID(); 
}

void gui_edit_collider(const char* name, Collider* collider) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 
  
  // Extents
  Vec3 extents = collider_get_extents(collider);
  if(ImGui::DragFloat3("Extents", &extents[0])) {
    collider_set_extents(collider, extents);
  }
  
  // Local position
  Vec3 local_pos = collider_get_local_transform(collider).position;
  if(ImGui::DragFloat3("Local position", &local_pos[0])) {
    collider_set_local_position(collider, local_pos);
  }
  
  // Friction
  f32 friction = collider_get_friction(collider);
  if(ImGui::DragFloat("Friction", &friction)) {
    collider_set_friction(collider, friction);
  }
  
  // Restitution
  f32 restitution = collider_get_restitution(collider);
  if(ImGui::DragFloat("Restitution", &restitution)) {
    collider_set_restitution(collider, restitution);
  }
  
  // Density
  f32 density = collider_get_density(collider);
  if(ImGui::DragFloat("Density", &density)) {
    collider_set_density(collider, density);
  }
  
  ImGui::PopID(); 
}

void gui_edit_particle_emitter(const char* name, ParticleEmitterDesc* emitter_desc) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 
 
  ImGui::DragFloat3("Position", &emitter_desc->position[0], 0.1f);
  ImGui::DragFloat3("Velocity", &emitter_desc->velocity[0], 1.0f);

  ImGui::DragFloat3("Scale", &emitter_desc->scale[0], 0.1f, 0.0f, 256.0f);
  ImGui::DragFloat4("Color", &emitter_desc->color[0], 0.1f, 0.0f, 12.0f);
  
  ImGui::DragFloat("Lifetime", &emitter_desc->lifetime, 0.1f, 0.0f, 512.0f);
  ImGui::DragFloat("Gravity", &emitter_desc->gravity_factor, 0.1f);
  
  ImGui::DragFloat("Distribution radius", &emitter_desc->distribution_radius, 0.1f);
  
  i32 current_dist = emitter_desc->distribution;
  if(ImGui::Combo("Distributions", &current_dist, "Random\0Square\0Cube\0\0")) {
    emitter_desc->distribution = (ParticleDistributionType)current_dist;
  }

  i32 count = (i32)emitter_desc->count;
  if(ImGui::SliderInt("Count", &count, 1, (i32)(PARTICLES_MAX - 1))) {
    emitter_desc->count = (sizei)count;
  }
  
  ImGui::PopID(); 
}

/// Editor functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
