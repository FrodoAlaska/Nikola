#include "nikola/nikola_ui.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_resources.h"
#include "nikola/nikola_render.h"
#include "nikola/nikola_math.h"
#include "nikola/nikola_audio.h"
#include "nikola/nikola_physics.h"
#include "nikola/nikola_input.h"
#include "nikola/nikola_event.h"
#include "nikola/nikola_entity.h"

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
  Window* window          = nullptr; 
  GLFWwindow* glfw_window = nullptr;

  ImGuiIO io_config;
  HashMap<String, Vec3> rotations;

  GUIWindowFlags window_flags;

  f32 big_step   = 0.01f; 
  f32 small_step = 0.001f; 

  bool is_active = false;
};

static GUIState s_gui;
/// GUIState
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Callbacks

static bool on_keyboard_action(const Event& event, const void* dispatcher, const void* listener) {
  if(!s_gui.is_active) {
    return true;
  }

  switch(event.type) {
    case EVENT_KEY_PRESSED:
      if(event.key_pressed == KEY_LEFT_SHIFT) {
        s_gui.big_step   = 0.1f;
        s_gui.small_step = 0.01f;
      }
      break;
    case EVENT_KEY_RELEASED:
      if(event.key_released == KEY_LEFT_SHIFT) {
        s_gui.big_step   = 0.01f;
        s_gui.small_step = 0.001f;
      }
      break;
  }

  return true; 
}

static bool on_mouse_action(const Event& event, const void* dispatcher, const void* listener) {
  if(!s_gui.is_active) {
    return true;
  }

  switch(event.type) {
    case EVENT_MOUSE_BUTTON_PRESSED:
      input_cursor_show(!(event.mouse_button_pressed == MOUSE_BUTTON_LEFT));
      break;
    case EVENT_MOUSE_BUTTON_RELEASED:
      input_cursor_show((event.mouse_button_released == MOUSE_BUTTON_LEFT));
      break;
  }

  return true; 
}

/// Callbacks
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GUI functions

bool gui_init(Window* window) {
  // Setting default values for the GUI
  
  s_gui        = GUIState{};
  s_gui.window = window;

  // Set up ImGui context
  
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();

  // Setting context flags
  
  io.ConfigFlags     = ImGuiConfigFlags_DockingEnable;
  s_gui.window_flags = GUI_WINDOW_FLAGS_NONE;

  // Dark mode WOOOOOOOAH! 
  ImGui::StyleColorsDark();

  // Setting up the glfw backend
 
  s_gui.glfw_window = (GLFWwindow*)window_get_handle(window);

  if(!ImGui_ImplGlfw_InitForOpenGL(s_gui.glfw_window, true)) {
    NIKOLA_LOG_ERROR("Failed to initialize GLFW for ImGui");
    return false;
  }
  
  // Setting up the opengl backend
  
  if(!ImGui_ImplOpenGL3_Init("#version 460 core")) {
    NIKOLA_LOG_ERROR("Failed to initialize OpenGL for ImGui");
    return false;
  }

  // Listen to events
  
  event_listen(EVENT_KEY_PRESSED, on_keyboard_action);
  event_listen(EVENT_KEY_RELEASED, on_keyboard_action);
  
  event_listen(EVENT_MOUSE_BUTTON_PRESSED, on_mouse_action);
  event_listen(EVENT_MOUSE_BUTTON_RELEASED, on_mouse_action);

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

void gui_set_window_flags(const i32 flags) {
  s_gui.window_flags = (GUIWindowFlags)flags;
}

bool gui_begin_panel(const char* name) {
  return ImGui::Begin(name, nullptr, (ImGuiWindowFlags)s_gui.window_flags);
}

void gui_end_panel() {
  ImGui::End();
}

void gui_toggle_active() {
  s_gui.is_active = !s_gui.is_active;
}

const bool gui_is_focused() {
  return ImGui::GetIO().WantCaptureMouse;
}

const bool gui_is_active() {
  return s_gui.is_active;
}

void gui_renderer_info() {
  if(!gui_begin_panel("Renderer Info")) {
    gui_end_panel();
    return;
  }
 
  // Clear color
 
  Vec4 clear_color = renderer_get_clear_color();
  bool is_picked   = ImGui::ColorPicker4("Clear color", &clear_color[0], ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);

  if(is_picked) {
    renderer_set_clear_color(clear_color);
  }

  gui_end_panel();
}

void gui_window_info() {
  if(!gui_begin_panel("Window")) {
    gui_end_panel();
    return;
  }
 
  // Title
 
  String window_title = window_get_title(s_gui.window);
  if(ImGui::InputText("Title", &window_title)) {
    window_set_title(s_gui.window, window_title.c_str());
  }
 
  // Size 
  
  IVec2 window_size;
  window_get_size(s_gui.window, &window_size.x, &window_size.y);

  if(ImGui::SliderInt2("Size", &window_size[0], 0, 1920)) {
    window_set_size(s_gui.window, window_size.x, window_size.y);
  }

  // Position 
  
  IVec2 window_pos;
  window_get_position(s_gui.window, &window_pos.x, &window_pos.y);

  if(ImGui::SliderInt2("Position", &window_pos[0], 0, 1920)) {
    window_set_position(s_gui.window, window_pos.x, window_pos.y);
  }

  // Fullscreen 

  bool is_fullscreen = window_is_fullscreen(s_gui.window);
  if(ImGui::Checkbox("Fullscreen", &is_fullscreen)) {
    window_set_fullscreen(s_gui.window, is_fullscreen);
  }

  gui_end_panel();
}

void gui_debug_info() {
  if(!gui_begin_panel("Debug Info")) {
    gui_end_panel();
    return;
  }

  // FPS 
  
  ImGui::Text("FPS: %.3lf", niclock_get_fps());
  ImGui::Separator();

  // Mouse
 
  if(ImGui::CollapsingHeader("Mouse")) {
    Vec2 mouse_pos;
    input_mouse_position(&mouse_pos.x, &mouse_pos.y);
    
    Vec2 mouse_offset;
    input_mouse_offset(&mouse_offset.x, &mouse_offset.y);

    ImGui::Text("Position: %s", vec2_to_string(mouse_pos).c_str());
    ImGui::Text("Offset: %s", vec2_to_string(mouse_offset).c_str());
  } 

  // Gamepad

  if(ImGui::CollapsingHeader("Gamepads")) {
    for(sizei i = JOYSTICK_ID_0; i < JOYSTICK_ID_LAST; i++) {
      JoystickID joy_id = (JoystickID)i;

      if(!input_gamepad_connected(joy_id)) {
        continue;
      }

      // Name 
      
      String name = ("Gamepad" + String(input_gamepad_get_name(joy_id)));
      ImGui::SeparatorText(name.c_str());

      // Axises values
    
      Vec2 left_axis; 
      input_gamepad_axis_value(joy_id, GAMEPAD_AXIS_LEFT, &left_axis.x, &left_axis.y);
      ImGui::Text("Left axis: X = %0.3f, Y = %0.3f", left_axis.x, left_axis.y);
   
      Vec2 right_axis; 
      input_gamepad_axis_value(joy_id, GAMEPAD_AXIS_RIGHT, &right_axis.x, &right_axis.y);
      ImGui::Text("Right axis: X = %0.3f, Y = %0.3f", right_axis.x, right_axis.y);
   
      Vec2 triggers; 
      input_gamepad_axis_value(joy_id, GAMEPAD_AXIS_TRIGGER, &triggers.x, &triggers.y);
      ImGui::Text("Trigger: Left = %0.3f, Right = %0.3f", triggers.x, triggers.y);
    }
  }

  // Memory
 
  if(ImGui::CollapsingHeader("Memory")) {
    ImGui::Text("Allocations: %zu", memory_get_allocations_count());
    ImGui::Text("Bytes allocated: %zuMiB", MiB(memory_get_allocation_bytes()));
  } 

  // Physics
 
  if(ImGui::CollapsingHeader("Physics")) {
    // Gravity 
    
    Vec3 gravity = physics_world_get_gravity();
    if(ImGui::DragFloat3("Gravity", &gravity[0], s_gui.big_step)) {
      physics_world_set_gravity(gravity);
    }

    // Paused
    
    bool paused = physics_world_is_paused();
    if(ImGui::Checkbox("Paused", &paused)) {
      physics_world_toggle_paused();
    }
  } 

  gui_end_panel();
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
  if(ImGui::DragFloat3("Position", &transform->position[0], s_gui.big_step)) {
    transform_translate(*transform, transform->position);
  }

  if(ImGui::DragFloat3("Scale", &transform->scale[0], s_gui.big_step)) {
    transform_scale(*transform, Vec3(transform->scale));
  }

  if(ImGui::DragFloat3("Rotation", &s_gui.rotations[name][0], s_gui.big_step)) {
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
  
  ImGui::Text("Yaw: %f", camera->yaw);
  ImGui::Text("Pitch: %f", camera->pitch);

  // Editables
 
  ImGui::DragFloat3("Position", &camera->position[0], s_gui.big_step);  
 
  ImGui::SliderFloat("Zoom", &camera->zoom, CAMERA_MAX_ZOOM, 0.0f, "Zoom: %.3f");
  ImGui::SliderFloat("Near", &camera->near, 0.0001f, 1000.0f, "Near: %.3f");
  ImGui::SliderFloat("Far", &camera->far, 0.0001f, 1000.0f, "Far: %.3f");
  
  ImGui::DragFloat("Sensitivity", &camera->sensitivity, s_gui.small_step);
  ImGui::SliderFloat("Exposure", &camera->exposure, 0.0f, 10.0f, "Exposure: %.3f");
  
  ImGui::PopID(); 
}

void gui_edit_directional_light(const char* name, DirectionalLight* dir_light) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 
  
  ImGui::DragFloat3("Direction", &dir_light->direction[0], s_gui.big_step);
  ImGui::DragFloat3("Color", &dir_light->color[0], s_gui.small_step, 0.0f);

  ImGui::PopID(); 
}

void gui_edit_point_light(const char* name, PointLight* point_light) {
  ImGui::SeparatorText(name);
  ImGui::PushID(name); 

  ImGui::DragFloat3("Position", &point_light->position[0], s_gui.big_step);
  ImGui::DragFloat3("Color", &point_light->color[0], s_gui.small_step, 0.0f);

  ImGui::SliderFloat("Radius", &point_light->radius, 0.0f, 32.0f);
  ImGui::SliderFloat("Fall off", &point_light->fall_off, 0.0f, 32.0f);

  ImGui::PopID(); 
}

void gui_edit_spot_light(const char* name, SpotLight* spot_light) {
  ImGui::SeparatorText(name);
  ImGui::PushID(name); 

  ImGui::DragFloat3("Position", &spot_light->position[0], s_gui.big_step);
  ImGui::DragFloat3("Direction", &spot_light->direction[0], s_gui.small_step, -1.0f, 1.0f);
  ImGui::DragFloat3("Color", &spot_light->color[0], s_gui.small_step, 0.0f);

  ImGui::SliderFloat("Radius", &spot_light->radius, 0.0f, 1.0f);
  ImGui::SliderFloat("Outer radius", &spot_light->outer_radius, 0.0f, 1.0f);

  ImGui::PopID(); 
}

void gui_edit_frame(const char* name, FrameData* frame) {
  ImGui::SeparatorText(name);
  ImGui::PushID(name); 

  // Camera
  
  if(ImGui::CollapsingHeader("Camera")) {
    nikola::gui_edit_camera("Editor Camera", &frame->camera); 
  }

  // Lights
  
  if(ImGui::CollapsingHeader("Lights")) {
    // Ambient

    ImGui::SeparatorText("Ambiance");
    ImGui::DragFloat3("Color", &frame->ambient[0], s_gui.small_step, 0.0f, 1.0f);
    
    // Directional light
    nikola::gui_edit_directional_light("Directional", &frame->dir_light);

    // Point light

    for(int i = 0; i < frame->point_lights.size(); i++) {
      nikola::PointLight* light = &frame->point_lights[i];
      nikola::String light_name = ("Point " + std::to_string(i));

      nikola::gui_edit_point_light(light_name.c_str(), light);
    }
   
    // Spot light

    for(int i = 0; i < frame->spot_lights.size(); i++) {
      nikola::SpotLight* light = &frame->spot_lights[i];
      nikola::String light_name = ("Spot " + std::to_string(i));

      nikola::gui_edit_spot_light(light_name.c_str(), light);
    }

    // Add buttons

    ImGui::Separator();
    if(ImGui::Button("Add PointLight")) {
      nikola::Vec3 point_pos = nikola::Vec3(10.0f, 5.0f, 10.0f);
      frame->point_lights.push_back(nikola::PointLight(point_pos));
    }
    
    if(ImGui::Button("Add SpotLight")) {
      nikola::Vec3 spot_pos = nikola::Vec3(10.0f, 5.0f, 10.0f);
      frame->spot_lights.push_back(nikola::SpotLight());
    }
  }

  ImGui::PopID(); 
}

void gui_edit_material(const char* name, Material* material) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 

  ImGui::ColorEdit3("Color", &material->color[0]);
  ImGui::ColorEdit4("Blend factor", &material->blend_factor[0]);
  
  ImGui::DragFloat("Roughness", &material->roughness, s_gui.small_step, 0.0f);
  ImGui::DragFloat("Metallic", &material->metallic, s_gui.small_step, 0.0f);
  ImGui::DragFloat("Emissive", &material->emissive, s_gui.small_step, 0.0f);

  ImGui::DragFloat("Transparency", &material->transparency, s_gui.small_step, 0.0f, 1.0f);
  ImGui::DragInt("Stencil reference", &material->stencil_ref, s_gui.big_step);
  ImGui::Checkbox("Depth Mask", &material->depth_mask);
  
  ImGui::PopID(); 
}

void gui_edit_font(const char* name, Font* font, String* label) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 

  // Font info 
  
  if(ImGui::CollapsingHeader("Font Information")) {
    ImGui::SliderFloat("Ascent", &font->ascent, -1000, 1000);
    ImGui::SliderFloat("Descent", &font->descent, -1000, 1000);
    ImGui::SliderFloat("Line Gap", &font->line_gap, -1000, 1000);
  } 
 
  // Label info
  
  if(ImGui::CollapsingHeader("Input Label")) {
    ImGui::InputTextMultiline("Label", label);
  } 

  // Glyphs Info
  
  if(ImGui::CollapsingHeader("Glyphs")) {
    for(auto& ch : *label) {
      Font::Glyph* glyph = &font->glyphs[ch]; 
      
      String str_id = ("Char: " + ch);
      ImGui::PushID(str_id.c_str());
      
      ImGui::Text("Codepoint: %c", glyph->codepoint);
      
      ImGui::SliderFloat2("Size", &glyph->size[0], -1000, 1000);
      ImGui::SliderFloat2("Offset", &glyph->offset[0], -1000, 1000);
      
      ImGui::Text("Bounds  = {T: %i, L: %i, B: %i, R: %i}", glyph->top, glyph->left, glyph->bottom, glyph->right);
      
      ImGui::SliderInt("Advance", &glyph->advance_x, -1000, FLOAT_MAX);
      ImGui::SliderInt("Left Side Bearing", &glyph->left_bearing, -1000, FLOAT_MAX);

      ImGui::Separator();
      ImGui::PopID();
    }
  }
  
  ImGui::PopID(); 
}

void gui_edit_audio_source(const char* name, AudioSourceID& source) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 

  AudioSourceDesc source_desc = audio_source_get_desc(source);

  // Source Info
  
  // Volume
  if(ImGui::SliderFloat("Volume", &source_desc.volume, 0.0f, 1.0f)) {
    audio_source_set_volume(source, source_desc.volume);
  }

  // Pitch
  if(ImGui::SliderFloat("Pitch", &source_desc.pitch, 0.0f, 1.0f)) {
    audio_source_set_pitch(source, source_desc.pitch);
  }

  // Position
  if(ImGui::DragFloat3("Position", &source_desc.position[0], s_gui.small_step, -100.0f, 100.0f)) {
    audio_source_set_position(source, source_desc.position);
  }

  // Velocity
  if(ImGui::DragFloat3("Velocity", &source_desc.velocity[0], s_gui.small_step, -1.0f, 1.0f)) {
    audio_source_set_velocity(source, source_desc.velocity);
  }
  
  // Direction
  if(ImGui::DragFloat3("Direction", &source_desc.direction[0], s_gui.small_step, -1.0f, 1.0f)) {
    audio_source_set_direction(source, source_desc.direction);
  }
  
  // Looping
  if(ImGui::Checkbox("Looping", &source_desc.is_looping)) {
    audio_source_set_looping(source, source_desc.is_looping);
  }

  // Queued buffers 
  ImGui::Text("Current queued buffers %zu", source_desc.buffers_count);

  // Command buttons
  
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
  if(ImGui::DragFloat3("Position", &listener.position[0], s_gui.small_step, -1.0f, 1.0f)) {
    audio_listener_set_position(listener.position);
  }

  // Velocity
  if(ImGui::DragFloat3("Velocity", &listener.velocity[0], s_gui.small_step, -1.0f, 1.0f)) {
    audio_listener_set_velocity(listener.velocity);
  }

  ImGui::PopID(); 
}

void gui_edit_timer(const char* name, Timer* timer) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 

  // Limit 
  {
    ImGui::DragFloat("Limit", &timer->limit, s_gui.big_step, 0.0f);
  }

  // Booleans
  {
    ImGui::Checkbox("One shot", &timer->is_one_shot);
    ImGui::Checkbox("Active", &timer->is_active);
  }
  
  ImGui::PopID(); 
}

void gui_edit_physics_body(const char* name, PhysicsBody* body) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 
 
  PhysicsBodyType body_type = physics_body_get_type(body);

  if(body_type != PHYSICS_BODY_STATIC) {
    // Position
    {
      Vec3 position = physics_body_get_position(body);
      if(ImGui::DragFloat3("Position", &position[0], s_gui.big_step)) {
        physics_body_set_position(body, position);
      }
    }

    // Linear velocity
    {
      Vec3 linear = physics_body_get_linear_velocity(body);
      if(ImGui::DragFloat3("Linear velocity", &linear[0], s_gui.big_step)) {
        physics_body_set_linear_velocity(body, linear);
      }
    }

    // Angular velocity
    {
      Vec3 angular = physics_body_get_angular_velocity(body);
      if(ImGui::DragFloat3("Angular velocity", &angular[0], s_gui.big_step)) {
        physics_body_set_angular_velocity(body, angular);
      }
    }
  }

  // Active
  {
    bool active = physics_body_is_active(body);
    if(ImGui::Checkbox("Active", &active)) {
      physics_body_set_active(body, active);
    }
  }

  // Layer
  {
    i32 current_layer   = (i32)physics_body_get_layer(body);
    const char* options = "Layer 0\0Layer 1\0Layer 2\0Layer 3\0Layer 4\0Layer 5\0Layer 6\0Layer 7\0 Layer 8\0Layer 9\0\0";

    if(ImGui::Combo("Layer", &current_layer, options)) {
      physics_body_set_layer(body, (PhysicsObjectLayer)current_layer);
    }
  }

  // Body type
  {
    i32 current_type    = (i32)body_type;
    const char* options = "Static\0Dynamic\0Kinematic\0\0";

    if(ImGui::Combo("Type", &current_type, options)) {
      physics_body_set_type(body, (PhysicsBodyType)current_type);
    }
  }

  // Restitution
  {
    f32 restitution = physics_body_get_restitution(body);
    if(ImGui::DragFloat("Restitution", &restitution, s_gui.small_step, 0.0f, 1.0f)) {
      physics_body_set_restitution(body, restitution);
    }
  }
  
  // Friction
  {
    f32 friction = physics_body_get_friction(body);
    if(ImGui::DragFloat("Friction", &friction, s_gui.small_step, 0.0f, 1.0f)) {
      physics_body_set_friction(body, friction);
    }
  }
  
  // Gravity factor
  {
    f32 factor = physics_body_get_gravity_factor(body);
    if(ImGui::DragFloat("Gravity factor", &factor, s_gui.big_step)) {
      physics_body_set_gravity_factor(body, factor);
    }
  }

  ImGui::PopID(); 
}

void gui_edit_character_body(const char* name, Character* character) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 
  
  // Position
  {
    Vec3 position = character_body_get_position(character);
    if(ImGui::DragFloat3("Position", &position[0], s_gui.big_step)) {
      character_body_set_position(character, position);
    }
  }

  // Linear velocity
  {
    Vec3 linear = character_body_get_linear_velocity(character);
    if(ImGui::DragFloat3("Linear velocity", &linear[0], s_gui.big_step)) {
      character_body_set_linear_velocity(character, linear);
    }
  }

  // Layer
  {
    i32 current_layer   = (i32)character_body_get_layer(character);
    const char* options = "Layer 0\0Layer 1\0Layer 2\0Layer 3\0Layer 4\0Layer 5\0Layer 6\0Layer 7\0 Layer 8\0Layer 9\0\0";

    if(ImGui::Combo("Layer", &current_layer, options)) {
      character_body_set_layer(character, (PhysicsObjectLayer)current_layer);
    }
  }

  // Slop angle
  {
    f32 slope_angle = character_body_get_slope_angle(character) * RAD2DEG;
    if(ImGui::DragFloat("Slope angle (Degrees)", &slope_angle, s_gui.big_step)) {
      character_body_set_slope_angle(character, slope_angle * DEG2RAD);
    }
  }

  // Ground state
  {
    String state_string      = "On ground";
    GroundState ground_state = character_body_query_ground_state(character);
 
    switch(ground_state) {
      case GROUND_STATE_ON_GROUND:
        state_string = "On ground";
        break;
      case GROUND_STATE_ON_STEEP_GROUND:
        state_string = "On steep ground";
        break;
      case GROUND_STATE_NOT_SUPPORTED:
        state_string = "Not supported";
        break;
      case GROUND_STATE_IN_AIR:
        state_string = "In air";
        break;
    }

    ImGui::Text("Ground state: %s", state_string.c_str());
  }

  ImGui::PopID(); 
}

void gui_edit_particle_emitter(const char* name, ParticleEmitter* emitter) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 
 
  ImGui::DragFloat3("Position", &emitter->initial_position[0], s_gui.big_step);
  ImGui::DragFloat3("Velocity", &emitter->initial_velocity[0], s_gui.big_step);
  
  ImGui::DragFloat("Lifetime", &emitter->lifetime.limit, s_gui.big_step, 0.0f, 512.0f);
  ImGui::DragFloat("Gravity", &emitter->gravity_factor, s_gui.big_step);
  
  ImGui::DragFloat("Distribution radius", &emitter->distribution_radius, s_gui.big_step);
  
  i32 current_dist = emitter->distribution;
  if(ImGui::Combo("Distributions", &current_dist, "Random\0Square\0Cube\0\0")) {
    emitter->distribution = (ParticleDistributionType)current_dist;
  }

  i32 count = (i32)emitter->particles_count;
  if(ImGui::SliderInt("Count", &count, 1, (i32)(PARTICLES_MAX - 1))) {
    emitter->particles_count = (sizei)count;
  }
  
  ImGui::PopID(); 
}

void gui_edit_animation_sampler(const char* name, AnimationSampler* sampler) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 

  AnimationSamplerInfo& info = animation_sampler_get_info(sampler);

  ImGui::SliderFloat("Playback speed", &info.play_speed, -1.0f, 1.0f);

  ImGui::Checkbox("Looping", &info.is_looping);
  ImGui::Checkbox("Playing", &info.is_animating);

  if(ImGui::Button("Reset")) {
    info.current_time = 0.0f;
  }
  
  ImGui::PopID(); 
}

void gui_edit_animation_blender(const char* name, AnimationBlender* blender) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 

  AnimationBlenderInfo& info = animation_blender_get_info(blender);

  ImGui::SliderFloat("Blending threshold", &info.blending_threshold, 0.01f, 1.0f);
  ImGui::SliderFloat("Blending ratio", &info.blending_ratio, 0.0f, 1.0f);

  ImGui::Checkbox("Looping", &info.is_looping);
  ImGui::Checkbox("Playing", &info.is_animating);
  
  ImGui::PopID(); 
}

void gui_edit_entity(const char* name, EntityWorld& world, EntityID& entt) {
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 
 
  // Transform
 
  if(ImGui::TreeNode("Transform")) {
    Transform& transform = entity_get_component<Transform>(world, entt);
    gui_edit_transform("", &transform);

    ImGui::TreePop();
  }

  // Physics body

  if(entity_has_component<PhysicsComponent>(world, entt)) {
    if(ImGui::TreeNode("Physics body")) {
      PhysicsComponent& comp = entity_get_component<PhysicsComponent>(world, entt);
      gui_edit_physics_body("", comp.body);

      ImGui::TreePop();
    }
  }

  // Character

  if(entity_has_component<CharacterComponent>(world, entt)) {
    if(ImGui::TreeNode("Character body")) {
      CharacterComponent& comp = entity_get_component<CharacterComponent>(world, entt);
      gui_edit_character_body("", comp.character);

      ImGui::TreePop();
    }
  }

  // Audio 

  if(entity_has_component<AudioSourceID>(world, entt)) {
    if(ImGui::TreeNode("Audio source")) {
      AudioSourceID& source = entity_get_component<AudioSourceID>(world, entt);
      gui_edit_audio_source("", source);

      ImGui::TreePop();
    }
  }
  
  // Timer 

  if(entity_has_component<Timer>(world, entt)) {
    if(ImGui::TreeNode("Timer")) {
      Timer& timer = entity_get_component<Timer>(world, entt);
      gui_edit_timer("", &timer);

      ImGui::TreePop();
    }
  }
  
  // Particles 

  if(entity_has_component<ParticleEmitter>(world, entt)) {
    if(ImGui::TreeNode("Particle emitter")) {
      ParticleEmitter& emitter = entity_get_component<ParticleEmitter>(world, entt);
      gui_edit_particle_emitter("", &emitter);

      ImGui::TreePop();
    }
  }
  
  // Animation sampler 

  if(entity_has_component<AnimationSampler*>(world, entt)) {
    if(ImGui::TreeNode("Animation sampler")) {
      AnimationSampler* sampler = entity_get_component<AnimationSampler*>(world, entt);
      gui_edit_animation_sampler("", sampler);

      ImGui::TreePop();
    }
  }
  
  // Animation blender 

  if(entity_has_component<AnimationBlender*>(world, entt)) {
    if(ImGui::TreeNode("Animation blender")) {
      AnimationBlender* blender = entity_get_component<AnimationBlender*>(world, entt);
      gui_edit_animation_blender("", blender);

      ImGui::TreePop();
    }
  }

  ImGui::PopID(); 
}

/// Editor functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
