#include "nikola/nikola_ui.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_resources.h"
#include "nikola/nikola_render.h"

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
  ImGui::SeparatorText("##xx");
  ImGui::Text("Postiion: %s", vec3_to_string(camera->position).c_str());  
  ImGui::Text("Yaw: %f", camera->yaw);
  ImGui::Text("Pitch: %f", camera->pitch);
  // -------------------------------------------------------------------

  // Editables
  // -------------------------------------------------------------------
  ImGui::SeparatorText("##xx");
  ImGui::SliderFloat("Zoom", &camera->zoom, CAMERA_MAX_ZOOM, 0.0f, "Zoom: %.3f");
  ImGui::SliderFloat("Near", &camera->near, 0.1f, 1000.0f, "Near: %.3f");
  ImGui::SliderFloat("Far", &camera->far, 0.1f, 1000.0f, "Far: %.3f");
  ImGui::SliderFloat("Sensitivity", &camera->sensitivity, 0.0f, 1.0f, "Sensitivity: %.3f");
  // -------------------------------------------------------------------
}

void gui_settings_resource(const u16 resource_group) {
  // @TODO: ?
}

void gui_settings_renderer() {
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

  // Render effect
  static i32 current_effect = 0;
  RenderEffectType effects[] = {
    RENDER_EFFECT_NONE,
    RENDER_EFFECT_GREYSCALE,
    RENDER_EFFECT_INVERSION,
    RENDER_EFFECT_SHARPEN,
    RENDER_EFFECT_BLUR,
    RENDER_EFFECT_EMBOSS,
    RENDER_EFFECT_EDGE_DETECT,
    RENDER_EFFECT_PIXELIZE,
  };

  ImGui::Combo("Effect", &current_effect, "None\0Greyscale\0Inversion\0Sharpen\0Blur\0Emboss\0Edge Detect\0Pixelize\0", RENDER_EFFECTS_MAX);
  renderer_apply_effect(effects[current_effect]);
  // -------------------------------------------------------------------
}

void gui_settings_material(const char* name, Material* material) {
  ImGui::SeparatorText(name); 

  // Colors 
  // -------------------------------------------------------------------
  ImGui::PushID(name); 
  ImGui::SliderFloat3("Ambient", &material->ambient_color[0], 0.0f, 1.0f);
  ImGui::SliderFloat3("Diffuse", &material->diffuse_color[0], 0.0f, 1.0f);
  ImGui::SliderFloat3("Specular", &material->specular_color[0], 0.0f, 1.0f);
  ImGui::PopID(); 
  // -------------------------------------------------------------------
 
  // Lighting values
  // -------------------------------------------------------------------
  ImGui::SliderFloat("Shininess", &material->shininess, 0.0f, 100.0f);
  // -------------------------------------------------------------------
}


void gui_settings_transform(const char* name, Transform* transform) {
  f32 scale = transform->scale.x;
  static Vec4 rotation = Vec4(1.0f);
  
  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 
 
  // SRT translation
  // -------------------------------------------------------------------
  ImGui::DragFloat3("Position", &transform->position[0], 0.01f);
  ImGui::DragFloat("Scale", &scale, 0.01f);
  ImGui::DragFloat3("Rotation Axis", &rotation[0], 0.01f, 0.0f, 1.0f);
  ImGui::DragFloat("Rotation Angle", &rotation[3], 0.01f);
  // -------------------------------------------------------------------
  
  ImGui::PopID(); 

  // Applying the new values
  nikola::transform_translate(*transform, transform->position);
  nikola::transform_scale(*transform, Vec3(scale));
  nikola::transform_rotate(*transform, rotation);
}

void gui_settings_texture(const char* name, GfxTexture* texture) {
  GfxTextureDesc tex_desc = gfx_texture_get_desc(texture); 

  ImGui::SeparatorText(name); 
  ImGui::PushID(name); 

  // Size
  // -------------------------------------------------------------------
  Vec2 size = Vec2(tex_desc.width, tex_desc.height);
  ImGui::SliderFloat2("Size", &size[0], 0.1f, 5120.0f, "%.3f"); 

  tex_desc.width  = size.x;
  tex_desc.height = size.y;
  // -------------------------------------------------------------------
 
  // Mipmap levels
  // -------------------------------------------------------------------
  i32 mips = tex_desc.mips; 
  ImGui::SliderInt("Mipmaps", &mips, 0, 5);

  tex_desc.mips = mips;
  // -------------------------------------------------------------------
 
  // Format
  // -------------------------------------------------------------------
  i32 format = (i32)tex_desc.format;
  ImGui::Combo("Format", &format, "R8\0R16\0RG8\0RG16\0RGBA8\0RGBA16\0Depth24 Stencil8\0");  

  tex_desc.format = (GfxTextureFormat)format;
  // -------------------------------------------------------------------
  
  // Filter
  // -------------------------------------------------------------------
  i32 filter = (i32)tex_desc.filter;
  ImGui::Combo("Filter", &filter, "MinMagLinear\0MinMagNearest\0MinLinearMagNearest\0MinNearestMagLinear\0MinTrilinearMagLinear\0MinTrilinearMagNearest\0");  

  tex_desc.filter = (GfxTextureFilter)filter;
  // -------------------------------------------------------------------
  
  // Wrap
  // -------------------------------------------------------------------
  i32 wrap = (i32)tex_desc.wrap_mode;
  ImGui::Combo("Addressing Mode", &wrap, "Repeat\0Mirror\0Clamp\0Border Color\0");  

  tex_desc.wrap_mode = (GfxTextureWrap)wrap;
  // -------------------------------------------------------------------

  ImGui::PopID(); 

  // Applying the chnages
  gfx_texture_update(texture, tex_desc);
}

/// Editor functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
