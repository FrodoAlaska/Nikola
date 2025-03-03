#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"
#include "nikola/nikola_ui.hpp"

#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_opengl3_loader.h>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// GUIState
struct GUIState {
  Window* window = nullptr; 
  ImGuiIO io_config;

  Vec4 render_clear_color = Vec4(0.1f, 0.1f, 0.1f, 1.0f);
  
  f64 fps              = 0.0;
  IVec2 window_size    = IVec2(0); 
  Vec2 mouse_position  = Vec2(0.0f); 
  Vec2 mouse_offset    = Vec2(0.0f); 

  sizei allocations_count = 0; 
  sizei allocation_bytes  = 0;
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

void gui_begin_panel(const char* name) {
  if(!ImGui::Begin(name)) {
    ImGui::End();
  } 
}

void gui_end_panel() {
  ImGui::End();
}

void gui_settings_debug() {
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
}

void gui_settings_camera(Camera* camera) {
  // Information
  // -------------------------------------------------------------------
  ImGui::SeparatorText("");
  ImGui::Text("Postiion: %s", vec3_to_string(camera->position).c_str());  
  ImGui::Text("Yaw: %f", camera->yaw);
  ImGui::Text("Pitch: %f", camera->pitch);
  // -------------------------------------------------------------------

  // Editables
  // -------------------------------------------------------------------
  ImGui::SeparatorText("");
  ImGui::SliderFloat("Zoom", &camera->zoom, CAMERA_MAX_ZOOM, 0.0f, "Zoom: %.3f");
  ImGui::SliderFloat("Near", &camera->near, 0.1f, 1000.0f, "Near: %.3f");
  ImGui::SliderFloat("Far", &camera->far, 0.1f, 1000.0f, "Far: %.3f");
  ImGui::SliderFloat("Sensitivity", &camera->sensitivity, 0.0f, 1.0f, "Sensitivity: %.3f");
  // -------------------------------------------------------------------
}

void gui_settings_resource(ResourceStorage* storage) {
}

void gui_settings_renderer() {
  // Stats
  // -------------------------------------------------------------------
  ImGui::SeparatorText("Stats");
  // -------------------------------------------------------------------
 
  // Editables
  // -------------------------------------------------------------------
  ImGui::SeparatorText("");
  ImGui::ColorPicker4("Clear color", &s_gui.render_clear_color[0], ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
  renderer_set_clear_color(s_gui.render_clear_color);
  // -------------------------------------------------------------------
}

void gui_settings_material(Material* material) {
  // Colors 
  // -------------------------------------------------------------------
  ImGui::SeparatorText("Colors"); 
  ImGui::SliderFloat3("Ambient", &material->ambient_color[0], 0.0f, 1.0f);
  ImGui::SliderFloat3("Diffuse", &material->diffuse_color[0], 0.0f, 1.0f);
  ImGui::SliderFloat3("Specular", &material->specular_color[0], 0.0f, 1.0f);
  // -------------------------------------------------------------------
}

/// Editor functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
