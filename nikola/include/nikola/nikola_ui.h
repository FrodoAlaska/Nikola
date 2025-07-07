#pragma once

#include "nikola_resources.h"
#include "nikola_render.h"
#include "nikola_containers.h"
#include "nikola_audio.h"
#include "nikola_physics.h"
#include "nikola_timer.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// UIAnchor
enum UIAnchor {
  UI_ANCHOR_TOP_LEFT      = 21 << 0, 
  UI_ANCHOR_TOP_CENTER    = 21 << 1, 
  UI_ANCHOR_TOP_RIGHT     = 21 << 2, 
  
  UI_ANCHOR_CENTER_LEFT   = 21 << 3, 
  UI_ANCHOR_CENTER        = 21 << 4, 
  UI_ANCHOR_CENTER_RIGHT  = 21 << 5, 
  
  UI_ANCHOR_BOTTOM_LEFT   = 21 << 6, 
  UI_ANCHOR_BOTTOM_CENTER = 21 << 7, 
  UI_ANCHOR_BOTTOM_RIGHT  = 21 << 8, 
};
/// UIAnchor
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UITextAnimation 
enum UITextAnimation {
  UI_TEXT_ANIMATION_FADE_IN     = 22 << 0, 
  UI_TEXT_ANIMATION_FADE_OUT    = 22 << 1, 
  
  UI_TEXT_ANIMATION_BALLON_UP   = 22 << 2, 
  UI_TEXT_ANIMATION_BALLON_DOWN = 22 << 3, 
  
  UI_TEXT_ANIMATION_SLIDE_UP   = 22 << 4, 
  UI_TEXT_ANIMATION_SLIDE_DOWN = 22 << 5, 
};
/// UITextAnimation 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UIText
struct UIText {
  Vec2 position, offset, bounds;
  Window* window_ref;

  String string;
  Font* font;
  f32 font_size;

  UIAnchor anchor; 
  Vec4 color;
  bool is_active;
};
/// UIText
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UITextDesc
struct UITextDesc {
  String string;

  ResourceID font_id;
  f32 font_size;
  UIAnchor anchor;

  Vec2 offset = Vec2(0.0f);
  Vec4 color  = Vec4(1.0f);
};
/// UITextDesc
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UIButton
struct UIButton {
  UIText text; 
  u32 id;

  Vec2 padding, offset;
  Vec2 position, size;

  f32 outline_thickness;
  Vec4 color, outline_color;
  
  bool is_active, was_hovered;
};
/// UIButton
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UIButtonDesc
struct UIButtonDesc {
  String text; 
  
  ResourceID font_id; 
  f32 font_size;
  UIAnchor anchor; 
  
  u32 bind_id = 0;

  Vec2 offset  = Vec2(0.0f);
  Vec2 padding = Vec2(30.0f, 10.0f); 
  
  Vec4 color         = Vec4(1.0f);
  Vec4 outline_color = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
  Vec4 text_color    = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
  
  f32 outline_thickness = 7.0f;
};
/// UIButtonDesc
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UICheckbox
struct UICheckbox {
  UIAnchor anchor;
  Vec2 position, size, offset; 

  u32 id;
  Window* window_ref;

  Vec4 color, outline_color;
  bool is_active, is_checked, was_hovered;
};
/// UICheckbox
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UICheckboxDesc
struct UICheckboxDesc {
  f32 size;

  UIAnchor anchor;
  Vec2 offset = Vec2(0.0f);
  
  u32 bind_id = 0;

  Vec4 color         = Vec4(1.0f); 
  Vec4 outline_color = Vec4(0.0f, 0.0f, 0.0f, 1.0f); 

  bool initial_checked = true;
};
/// UICheckboxDesc
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UILayout
struct UILayout {
 ResourceID font_id; 
 Window* window_ref;
  
 DynamicArray<UIText> texts;
 DynamicArray<UIButton> buttons;

 UIAnchor current_anchor;
 Vec2 extra_offset;
 Vec2 current_offset;

 f32 buttons_outline_thickness;
 Vec2 buttons_padding;

 bool is_active;
};
/// UILayout
/// ----------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GUI functions

/// Initialize the GUI context, given the information in `window`.
NIKOLA_API bool gui_init(Window* window);

/// Shutdown the GUI context, reclaiming any allocated memory.
NIKOLA_API void gui_shutdown();

/// Begin a frame of the GUI context. 
///
/// @NOTE: This MUST be called BEFORE any GUI context calls.
NIKOLA_API void gui_begin();

/// End a frame of the GUI context. 
///
/// @NOTE: This MUST be called BEFORE the end of the GUI frame.
NIKOLA_API void gui_end();

/// Start a panel with the name `name`. 
NIKOLA_API bool gui_begin_panel(const char* name);

/// End the panel with the latest `gui_begin_panel` call.
NIKOLA_API void gui_end_panel();

/// Returns `true` if the GUI is currently focused or capturing the mouse. 
NIKOLA_API const bool gui_is_focused();

/// Draw a preset panel with all of the debug information.
NIKOLA_API void gui_debug_info();

/// Draw a preset panel with all of the renderer information.
NIKOLA_API void gui_renderer_info();

/// Add a color wheel identified by `name` to edit the given `color`.
NIKOLA_API void gui_edit_color(const char* name, Vec4& color);

/// Add a transform section identified by `name` to edit the given `transform`.
NIKOLA_API void gui_edit_transform(const char* name, Transform* transform);

/// Add a camera section identified by `name` to edit the given `camera`.
NIKOLA_API void gui_edit_camera(const char* name, Camera* camera);

/// Add a directional light section identified by `name` to edit the given `dir_light`.
NIKOLA_API void gui_edit_directional_light(const char* name, DirectionalLight* dir_light);

/// Add a point light section identified by `name` to edit the given `point_light`.
NIKOLA_API void gui_edit_point_light(const char* name, PointLight* point_light);

/// Add a material section identified by `name` to edit the given `material`.
NIKOLA_API void gui_edit_material(const char* name, Material* material);

/// Add a font section identified by `name` to edit the given `font` applied onto the given `label` text.
NIKOLA_API void gui_edit_font(const char* name, Font* font, String* label);

/// Add an audio source section identified by `name` to edit the given `source`.
NIKOLA_API void gui_edit_audio_source(const char* name, AudioSourceID& source);

/// Add an audio listener section identified by `name` to edit the global audio listener.
NIKOLA_API void gui_edit_audio_listener(const char* name);

/// Add a physics body section identified by `name` to edit the given `body`.
NIKOLA_API void gui_edit_physics_body(const char* name, PhysicsBody* body);

/// Add a collider section identified by `name` to edit the given `collider`.
NIKOLA_API void gui_edit_collider(const char* name, Collider* collider);

/// GUI functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIText functions

NIKOLA_API void ui_text_create(UIText* text, Window* window, const UITextDesc& desc);

NIKOLA_API void ui_text_set_anchor(UIText& text, const UIAnchor& anchor);

NIKOLA_API void ui_text_set_string(UIText& text, const String& new_string);

NIKOLA_API void ui_text_apply_animation(UIText& text, const UITextAnimation anim_type, const f32 duration);

NIKOLA_API void ui_text_render(const UIText& text);

/// UIText functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIButton functions

NIKOLA_API void ui_button_create(UIButton* button, Window* window, const UIButtonDesc& desc);

NIKOLA_API void ui_button_set_anchor(UIButton& button, const UIAnchor& anchor);

NIKOLA_API void ui_button_set_string(UIButton& button, const String& new_string);

NIKOLA_API void ui_button_render(UIButton& button);

/// UIButton functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UICheckbox

NIKOLA_API void ui_checkbox_create(UICheckbox* checkbox, Window* window, const UICheckboxDesc& desc);

NIKOLA_API void ui_checkbox_set_anchor(UICheckbox& checkbox, const UIAnchor& anchor);

NIKOLA_API void ui_checkbox_render(UICheckbox& checkbox);

/// UICheckbox
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UILayout functions

NIKOLA_API void ui_layout_create(UILayout* layout, Window* window, const ResourceID& font_id);

NIKOLA_API void ui_layout_begin(UILayout& layout, const UIAnchor anchor, const Vec2& offset = Vec2(0.0f));

NIKOLA_API void ui_layout_end(UILayout& layout);

NIKOLA_API void ui_layout_push_text(UILayout& layout, 
                                    const String& str, 
                                    const f32 size, 
                                    const Vec4& color, 
                                    const Vec2& layout_padding = Vec2(0.0f));

NIKOLA_API void ui_layout_push_button(UILayout& layout, 
                                      const String& str, 
                                      const f32 size, 
                                      const Vec4& color, 
                                      const Vec4& outline_color, 
                                      const Vec4& text_color, 
                                      const Vec2& layout_padding = Vec2(0.0f));

NIKOLA_API void ui_layout_render(const UILayout& layout);

/// UILayout functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
