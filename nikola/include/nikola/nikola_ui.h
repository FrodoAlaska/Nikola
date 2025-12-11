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
  /// Anchor a UI element to the top-left of the screen.
  UI_ANCHOR_TOP_LEFT, 
  
  /// Anchor a UI element to the top-center of the screen.
  UI_ANCHOR_TOP_CENTER, 
  
  /// Anchor a UI element to the top-right of the screen.
  UI_ANCHOR_TOP_RIGHT, 
  
  /// Anchor a UI element to the center-left of the screen.
  UI_ANCHOR_CENTER_LEFT, 
  
  /// Anchor a UI element to the center of the screen.
  UI_ANCHOR_CENTER, 
  
  /// Anchor a UI element to the center-right of the screen.
  UI_ANCHOR_CENTER_RIGHT, 
  
  /// Anchor a UI element to the bottom-left of the screen.
  UI_ANCHOR_BOTTOM_LEFT, 
  
  /// Anchor a UI element to the bottom-center of the screen.
  UI_ANCHOR_BOTTOM_CENTER, 
  
  /// Anchor a UI element to the bottom-right of the screen.
  UI_ANCHOR_BOTTOM_RIGHT, 
};
/// UIAnchor
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UITextAnimation 
enum UITextAnimation {
  /// Apply a fade in animation on a text UI element.
  UI_TEXT_ANIMATION_FADE_IN, 
  
  /// Apply a fade out animation on a text UI element.
  UI_TEXT_ANIMATION_FADE_OUT, 
  
  /// Apply a ballon up (increase size) animation on a text UI element.
  UI_TEXT_ANIMATION_BALLON_UP, 
  
  /// Apply a ballon down (decrease size) animation on a text UI element.
  UI_TEXT_ANIMATION_BALLON_DOWN, 
  
  /// Apply a slide up animation on a text UI element.
  UI_TEXT_ANIMATION_SLIDE_UP, 
  
  /// Apply a slide down animation on a text UI element.
  UI_TEXT_ANIMATION_SLIDE_DOWN, 
};
/// UITextAnimation 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// GUIWindowFlags
enum GUIWindowFlags {
  /// No flags set.
  GUI_WINDOW_FLAGS_NONE                         = 0, 
  
  /// Windows will not have a title bar.
  GUI_WINDOW_FLAGS_NO_TITLE_BAR                 = 1 << 0, 
  
  /// Windows will not resize.
  GUI_WINDOW_FLAGS_NO_RESIZE                    = 1 << 1, 
  
  /// Windows will not move.
  GUI_WINDOW_FLAGS_NO_MOVE                      = 1 << 2, 
  
  /// Windows will not have a scroll bar.
  GUI_WINDOW_FLAGS_NO_SCROLL_BAR                = 1 << 3, 
  
  /// Windows will not scroll with the mouse.
  GUI_WINDOW_FLAGS_NO_SCROLL_WITH_MOUSE         = 1 << 4, 
  
  /// Windows will not collapse.
  GUI_WINDOW_FLAGS_NO_COLLAPSE                  = 1 << 5, 
  
  /// Windows will always resize on content update.
  GUI_WINDOW_FLAGS_AUTO_RESIZE                  = 1 << 6, 
  
  /// Windows will not have a background.
  GUI_WINDOW_FLAGS_NO_BACKGROUND                = 1 << 7, 
  
  /// Windows will not load settings from an `.ini` file.
  GUI_WINDOW_FLAGS_NO_SAVED_SETTINGS            = 1 << 8, 
  
  /// Windows will not catch mouse inputs.
  GUI_WINDOW_FLAGS_NO_MOUSE_INPUTS              = 1 << 9, 
  
  /// Windows will have a menu bar.
  GUI_WINDOW_FLAGS_MENU_BAR                     = 1 << 10, 
  
  /// Windows will have a horizontal scroll bar.
  GUI_WINDOW_FLAGS_HORIZONTAL_SCROLL_BAR        = 1 << 11, 
  
  /// Windows will have no focus once created.
  GUI_WINDOW_FLAGS_NO_FOCUS_ON_APPEAR           = 1 << 12, 
  
  /// Windows will not be brought to the front on focus.
  GUI_WINDOW_FLAGS_NO_BRING_TO_FRONT            = 1 << 13, 
  
  /// Windows will always have vertical scroll bars.
  GUI_WINDOW_FLAGS_ALWAYS_VERTICAL_SCROLL_BAR   = 1 << 14, 
  
  /// Windows will always have horizontal scroll bars.
  GUI_WINDOW_FLAGS_ALWAYS_HORIZONTAL_SCROLL_BAR = 1 << 15, 
  
  /// Windows will not catch any navigation inputs.
  GUI_WINDOW_FLAGS_NO_NAV_INPUTS                = 1 << 16, 
  
  /// Windows will not be focused on any navigation inputs.
  GUI_WINDOW_FLAGS_NO_NAV_FOCUS                 = 1 << 17, 
  
  /// Windows will have a little dot indicating that they were changed.
  GUI_WINDOW_FLAGS_NO_UNSAVED_DOCUMENT          = 1 << 18, 
  
  /// Windows cannot be docked.
  GUI_WINDOW_FLAGS_NO_DOCKING                   = 1 << 19, 
};
/// GUIWindowFlags
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UIText
struct UIText {
  Vec2 position, offset, bounds;
  Vec2 canvas_bounds; 

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
/// UICheckbox
struct UICheckbox {
  UIAnchor anchor;
  Vec2 position, size, offset; 

  u32 id;

  Vec4 color, outline_color;
  bool is_active, is_checked, was_hovered;
};
/// UICheckbox
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UISlider
struct UISlider {
  UIAnchor anchor;
  Vec2 offset;

  Vec2 old_mouse_position;
  Vec2 position, notch_position;
  Vec2 size, notch_size; 
  
  u32 id;
  
  f32* value;
  f32 min, max, step;

  Vec4 color       = Vec4(1.0f); 
  Vec4 notch_color = Vec4(1.0f);

  bool is_active, is_hovering;
};
/// UISlider
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UIImage
struct UIImage {
  UIAnchor anchor; 
  Vec2 position, size, offset; 

  GfxTexture* texture = nullptr; 

  Vec4 tint = Vec4(1.0f); 
  bool is_active, was_hovered;
};
/// UIImage
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UIMenu
struct UIMenu {
  Vec2 bounds;
  ResourceID font_id;

  UIAnchor title_anchor, item_anchor; 
  Vec2 current_offset, title_offset, item_offset;
  Vec2 item_padding;

  UIText title; 
  DynamicArray<UIText> items; 

  sizei current_item  = 0;
  Vec4 selector_color = Vec4(1.0f); 

  bool is_active = true;
};
/// UIMenu
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UILayout
struct UILayout {
 ResourceID font_id; 
  
 DynamicArray<UIText> texts;
 DynamicArray<UIButton> buttons;
 DynamicArray<UICheckbox> checkboxes;
 DynamicArray<UISlider> sliders;
 DynamicArray<UIImage> images;

 UIAnchor current_anchor;
 Vec2 extra_offset, current_offset;
 Vec2 bounds;

 f32 buttons_outline_thickness;
 Vec2 buttons_padding;

 bool is_active;
};
/// UILayout
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UITextDesc
struct UITextDesc {
  /// The string to be set on the text UI element.
  String string;

  /// The id of a font resource to be used later.
  ResourceID font_id;
  
  /// The total size of the font of the text UI element.
  f32 font_size;

  /// The anchor point of the text UI element.
  UIAnchor anchor;

  /// The bounds of the canvas that the text will 
  /// be placed on. The UI element will calculate 
  /// its anchor point based on this.
  Vec2 canvas_bounds;

  /// The extra offset to be applied to the text UI element.
  ///
  /// @NOTE: This is set to `Vec2(0.0f, 0.0f)` by default.
  Vec2 offset = Vec2(0.0f);
  
  /// The color of the text UI element.
  ///
  /// @NOTE: This is set to `Vec4(1.0f, 1.0f, 1.0f, 1.0f)` by default.
  Vec4 color  = Vec4(1.0f);
};
/// UITextDesc
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UIButtonDesc
struct UIButtonDesc {
  /// The text of the button.
  String text; 
  
  /// The id of a font resource to be used later.
  ResourceID font_id; 
  
  /// The total size of the font of the button's text UI element.
  f32 font_size;
  
  /// The anchor point of the button UI element.
  UIAnchor anchor; 

  /// The bounds of the canvas that the text will 
  /// be placed on. The UI element will calculate 
  /// its anchor point based on this.
  Vec2 canvas_bounds;
 
  /// The binded id that will be used to identify the button.
  ///
  /// @NOTE: This is set to `0` by default.
  u32 bind_id = 0;

  /// The extra offset to be applied to the button UI element.
  ///
  /// @NOTE: This is set to `Vec2(0.0f, 0.0f)` by default.
  Vec2 offset  = Vec2(0.0f);

  /// The text padding to be applied to the button UI element. 
  ///
  /// @NOTE: This is set to `Vec2(30.0f, 10.0f)` by default.
  Vec2 padding = Vec2(30.0f, 10.0f); 
  
  /// The color of the button UI element.
  ///
  /// @NOTE: This is set to `Vec4(1.0f, 1.0f, 1.0f, 1.0f)` by default.
  Vec4 color         = Vec4(1.0f);
  
  /// The outline color of the button UI element.
  ///
  /// @NOTE: This is set to `Vec4(0.0f, 0.0f, 0.0f, 1.0f)` by default.
  Vec4 outline_color = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
  
  /// The color of text of the button UI element.
  ///
  /// @NOTE: This is set to `Vec4(0.0f, 0.0f, 0.0f, 1.0f)` by default.
  Vec4 text_color    = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
 
  /// The thickness of the button's outline.
  ///
  /// @NOTE: This is set to `7.0f` by default.
  f32 outline_thickness = 7.0f;
};
/// UIButtonDesc
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UICheckboxDesc
struct UICheckboxDesc {
  /// The total size of the checkbox.
  f32 size;

  /// The anchor point of the checkbox.
  UIAnchor anchor;

  /// The bounds of the canvas that the text will 
  /// be placed on. The UI element will calculate 
  /// its anchor point based on this.
  Vec2 canvas_bounds;
  
  /// The extra offset to be applied to the checkbox.
  ///
  /// @NOTE: This is set to `Vec2(0.0f, 0.0f)` by default.
  Vec2 offset = Vec2(0.0f);
  
  /// The binded id that will be used to identify the checkbox.
  ///
  /// @NOTE: This is set to `0` by default.
  u32 bind_id = 0;

  /// The color of the checkbox UI element.
  ///
  /// @NOTE: This is set to `Vec4(1.0f, 1.0f, 1.0f, 1.0f)` by default.
  Vec4 color         = Vec4(1.0f);
  
  /// The outline color of the checkbox UI element.
  ///
  /// @NOTE: This is set to `Vec4(0.0f, 0.0f, 0.0f, 1.0f)` by default.
  Vec4 outline_color = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

  /// The initial checked state of the checkbox. 
  ///
  /// @NOTE: This is set to `true` by default.
  bool initial_checked = true;
};
/// UICheckboxDesc
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UISliderDesc
struct UISliderDesc {
  /// The anchor point of the slider.
  UIAnchor anchor; 

  /// The bounds of the canvas that the text will 
  /// be placed on. The UI element will calculate 
  /// its anchor point based on this.
  Vec2 canvas_bounds;
  
  /// The extra offset to be applied to the slider.
  ///
  /// @NOTE: This is set to `Vec2(0.0f, 0.0f)` by default.
  Vec2 offset = Vec2(0.0f);
 
  /// A pointer to the value to be manipulated by the slider. 
  f32* value = nullptr; 

  /// The minimum value of `value`.
  ///
  /// @NOTE: This is set to `-1.0f` by default.
  f32 min    = -1.0f; 
  
  /// The maximum value of `value`.
  ///
  /// @NOTE: This is set to `1.0f` by default.
  f32 max    = 1.0f;

  /// The amount of increaments/decrements 
  /// the slider will change the `value` by.
  ///
  /// @NOTE: This is set to `0.01f` by default.
  f32 step   = 0.01f;

  /// The binded id that will be used to identify the slider.
  ///
  /// @NOTE: This is set to `0` by default.
  u32 bind_id = 0;

  /// The color of the slider UI element.
  ///
  /// @NOTE: This is set to `Vec4(1.0f, 1.0f, 1.0f, 1.0f)` by default.
  Vec4 color       = Vec4(1.0f);
  
  /// The color of the notch knob of the slider UI element.
  ///
  /// @NOTE: This is set to `Vec4(0.5f, 0.5f, 0.5f, 1.0f)` by default.
  Vec4 notch_color = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
};
/// UISliderDesc
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UIImageDesc
struct UIImageDesc {
  /// The resource ID of the texture to be used.
  ResourceID texture_id;

  /// The size of the image to be used when rendering 
  /// the texture.
  Vec2 size;

  /// The anchor point of the image.
  UIAnchor anchor; 
  
  /// The bounds of the canvas that the text will 
  /// be placed on. The UI element will calculate 
  /// its anchor point based on this.
  Vec2 canvas_bounds;
  
  /// The extra offset to be applied to the image.
  ///
  /// @NOTE: This is set to `Vec2(0.0f, 0.0f)` by default.
  Vec2 offset = Vec2(0.0f);

  /// The tint of the image UI element.
  ///
  /// @NOTE: This is set to `Vec4(1.0f, 1.0f, 1.0f, 1.0f)` by default.
  Vec4 tint   = Vec4(1.0f);
};
/// UIImageDesc
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// UIMenuDesc
struct UIMenuDesc {
  /// The bounds of the menu container, used to anchor 
  /// UI elements.
  Vec2 bounds;

  /// The ID of the font that will be used in the container.
  ResourceID font_id;

  /// The base anchor of the title element.
  UIAnchor title_anchor; 

  /// The base anchor of each item in the container.
  UIAnchor item_anchor; 
  
  /// The offset applied to the title. 
  ///
  /// @NOTE: This is set to `Vec2(0.0f, 0.0f)` by default.
  Vec2 title_offset   = Vec2(0.0f);

  /// The offset applied between items. 
  ///
  /// @NOTE: This is set to `Vec2(0.0f, 0.0f)` by default.
  Vec2 item_offset    = Vec2(0.0f);
  
  /// The padding applied inside the item selector. 
  ///
  /// @NOTE: This is set to `Vec2(4.0f, 4.0f)` by default.
  Vec2 item_padding   = Vec2(4.0f);

  /// The starting item the selector will initially land on. 
  ///
  /// @NOTE: This is set to `0` by default.
  sizei starting_item = 0;

  /// The color of the item selector.
  ///
  /// @NOTE: This is set to `Vec4(1.0f, 1.0f, 1.0f, 1.0f)` by default.
  Vec4 selector_color = Vec4(1.0f);
};
/// UIMenuDesc
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

/// Set the window flags to `flags` of the current GUI context.
///
/// @NOTE: Check `GUIWindowFlags` for more information about specific flags.
/// Also, this function _must_ be called before any calls to `gui_begin_panel` 
/// to see the effects.
NIKOLA_API void gui_set_window_flags(const i32 flags);

/// Start a panel with the name `name`.
NIKOLA_API bool gui_begin_panel(const char* name);

/// End the panel with the latest `gui_begin_panel` call.
NIKOLA_API void gui_end_panel();

/// Toggle between an inactive and active GUI. 
NIKOLA_API void gui_toggle_active();

/// Returns `true` if the GUI is currently active. 
NIKOLA_API const bool gui_is_active();

/// Returns `true` if the GUI is currently focused or capturing the mouse. 
NIKOLA_API const bool gui_is_focused();

/// Draw a preset panel with all of the renderer information.
NIKOLA_API void gui_renderer_info();

/// Draw a preset panel with all of the window information.
NIKOLA_API void gui_window_info();

/// Draw a preset panel with all of the debug information.
NIKOLA_API void gui_debug_info();

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

/// Add a spot light section identified by `name` to edit the given `spot_light`.
NIKOLA_API void gui_edit_spot_light(const char* name, SpotLight* spot_light);

/// A convenience function that adds a frame section identified by `name` to edit the given `frame`.
NIKOLA_API void gui_edit_frame(const char* name, FrameData* frame);

/// Add a material section identified by `name` to edit the given `material`.
NIKOLA_API void gui_edit_material(const char* name, Material* material);

/// Add a font section identified by `name` to edit the given `font` applied onto the given `label` text.
NIKOLA_API void gui_edit_font(const char* name, Font* font, String* label);

/// Add an audio source section identified by `name` to edit the given `source`.
NIKOLA_API void gui_edit_audio_source(const char* name, AudioSourceID& source);

/// Add an audio listener section identified by `name` to edit the global audio listener.
NIKOLA_API void gui_edit_audio_listener(const char* name);

/// Add a timer section identified by `name` to edit the given `timer`.
NIKOLA_API void gui_edit_timer(const char* name, Timer* timer);

/// Add a physics body section identified by `name` to edit the given `body`.
NIKOLA_API void gui_edit_physics_body(const char* name, PhysicsBody* body);

/// Add a character body section identified by `name` to edit the given `character`.
NIKOLA_API void gui_edit_character_body(const char* name, Character* character);

/// Add a particles section identified by `name` to edit the given `emitter`.
NIKOLA_API void gui_edit_particle_emitter(const char* name, ParticleEmitter* emitter);

/// Add an animation sampler section identified by `name` to edit the given `sampler`.
NIKOLA_API void gui_edit_animation_sampler(const char* name, AnimationSampler* sampler);

/// Add an animation blender section identified by `name` to edit the given `blender`.
NIKOLA_API void gui_edit_animation_blender(const char* name, AnimationBlender* blender);

/// GUI functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UI renderer functions

/// Initialize the UI renderer.
NIKOLA_API bool ui_renderer_init(GfxContext* gfx);

/// Shutdown and reclaim all the memory allocated by the UI renderer.
NIKOLA_API void ui_renderer_shutdown();

/// Begin the rendering process of the UI renderer.
NIKOLA_API void ui_renderer_begin();

/// End the rendering process of the UI renderer. 
NIKOLA_API void ui_renderer_end();

/// Load the font face found at the given `path`.
/// Returns `true` if the font was loaded successfully, or `false` otherwise.
///
/// @NOTE: This is only a temporary function until I find a way 
/// to integrate NBR fonts into the UI renderer.
NIKOLA_API bool ui_renderer_load_font(const FilePath& path);

/// UI renderer functions
///---------------------------------------------------------------------------------------------------------------------

/// @ATTENTION @ATTENTION @ATTENTION @ATTENTION @ATTENTION @ATTENTION
///
///
/// @NOTE: 
/// All the functions below are going to potentially be deprecated. 
/// Use the RmlUi stuff instead. It's way better, anyway.
///
///
/// @ATTENTION @ATTENTION @ATTENTION @ATTENTION @ATTENTION @ATTENTION

///---------------------------------------------------------------------------------------------------------------------
/// UIText functions

/// Initialize the UI `text` element, using the information in `desc`.
NIKOLA_API void ui_text_create(UIText* text, const UITextDesc& desc);

/// Set the position of `text` based on the anchor point `anchor`, inside `bounds`.
NIKOLA_API void ui_text_set_anchor(UIText& text, const UIAnchor& anchor, const Vec2& bounds);

/// Set the internal string `text` to `new_string` and re-position accordingly.
NIKOLA_API void ui_text_set_string(UIText& text, const String& new_string);

/// Apply the animation `anim_type` to the given `text` UI element with the duration of `duration`.
NIKOLA_API void ui_text_apply_animation(UIText& text, const UITextAnimation anim_type, const f32 duration);

/// Render the given `text` UI element.
NIKOLA_API void ui_text_render(UIText& text);

/// UIText functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIButton functions

/// Initialize the UI `button` element, using the information in `desc`.
NIKOLA_API void ui_button_create(UIButton* button, const UIButtonDesc& desc);

/// Set the position of `button` based on the anchor point `anchor`, inside `bounds`.
NIKOLA_API void ui_button_set_anchor(UIButton& button, const UIAnchor& anchor, const Vec2& bounds);

/// Set the internal string of `button` to `new_string` and re-position accordingly.
NIKOLA_API void ui_button_set_string(UIButton& button, const String& new_string);

/// Render the given `button` UI element.
NIKOLA_API void ui_button_render(UIButton& button);

/// UIButton functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UICheckbox functions

/// Initialize the UI `checkbox` element, using the information in `desc`.
NIKOLA_API void ui_checkbox_create(UICheckbox* checkbox, const UICheckboxDesc& desc);

/// Set the position of `checkbox` based on the anchor point `anchor`, inside `bounds`.
NIKOLA_API void ui_checkbox_set_anchor(UICheckbox& checkbox, const UIAnchor& anchor, const Vec2& bounds);

/// Render the given `checkbox` UI element.
NIKOLA_API void ui_checkbox_render(UICheckbox& checkbox);

/// UICheckbox functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UISlider functions

/// Initialize the UI `slider` element, using the information in `desc`.
NIKOLA_API void ui_slider_create(UISlider* slider, const UISliderDesc& desc);

/// Set the position of `slider` based on the anchor point `anchor`, inside `bounds`.
NIKOLA_API void ui_slider_set_anchor(UISlider& slider, const UIAnchor& anchor, const Vec2& bounds);

/// Render the given `slider` UI element.
NIKOLA_API void ui_slider_render(UISlider& slider);

/// UISlider functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIImage functions

/// Initialize the UI `image` element, using the information in `desc`.
NIKOLA_API void ui_image_create(UIImage* image, const UIImageDesc& desc);

/// Set the position of `image` based on the anchor point `anchor`, inside `bounds`.
NIKOLA_API void ui_image_set_anchor(UIImage& image, const UIAnchor& anchor, const Vec2& bounds);

/// Set the texture of `image` based to the given `texture_id`.
NIKOLA_API void ui_image_set_texture(UIImage& image, const ResourceID& texture_id);

/// Render the given `image` UI element.
NIKOLA_API void ui_image_render(UIImage& image);

/// UIImage functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIMenu functions

/// Initialize the UI `menu` container, using the information in `desc`.
NIKOLA_API void ui_menu_create(UIMenu* menu, const UIMenuDesc& desc);

/// Set the title element of the given `menu` container, using the given `str`,
/// `size`, `color`, and `padding` for extra padding inside the container.
NIKOLA_API void ui_menu_set_title(UIMenu& menu, 
                                  const String& str,
                                  const f32 size, 
                                  const Vec4& color, 
                                  const Vec2& padding = Vec2(0.0f));

/// Push a new the item into the given `menu` container, using the given `str`,
/// `size`, `color`, and `padding` for extra padding inside the container.
NIKOLA_API UIText& ui_menu_push_item(UIMenu& menu, 
                                     const String& str,
                                     const f32 size, 
                                     const Vec4& color, 
                                     const Vec2& padding = Vec2(0.0f));

/// Process the navigation of the given `menu`. 
///
/// @NOTE: This _must_ be called every frame to enable menu navigation.
NIKOLA_API void ui_menu_process_input(UIMenu& menu);

/// Render all of the active UI elements in the given `menu`.
NIKOLA_API void ui_menu_render(UIMenu& menu);

/// UIMenu functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UILayout functions

/// Initialize the UI `layout` element, using the given `bounds` and the `font_id` to be used 
/// for any text in this specific layout.
NIKOLA_API void ui_layout_create(UILayout* layout, const Vec2& bounds, const ResourceID& font_id);

/// Begin a new layout segment of `layout` at anchor point `anchor`, increasing by `offset` every 
/// new entry using the `ui_layout_push_*` functions.
NIKOLA_API void ui_layout_begin(UILayout& layout, const UIAnchor anchor, const Vec2& offset = Vec2(0.0f));

/// End the previous layout segment in `layout` and reset everything back to normal.
NIKOLA_API void ui_layout_end(UILayout& layout);

/// Push a new text entry to `layout`, using `str`, `size`, and `color`.
/// Use `layout_padding` for extra padding between this element and the next.
NIKOLA_API UIText& ui_layout_push_text(UILayout& layout, 
                                       const String& str, 
                                       const f32 size, 
                                       const Vec4& color, 
                                       const Vec2& layout_padding = Vec2(0.0f));

/// Push a new button entry to `layout`, using `str`, `size`, `color`, `outline_color`, `text_color`, and `bind_id`.
/// Use `layout_padding` for extra padding between this element and the next.
///
/// @NOTE: When the `bind_id` is left as the default value (which is `-1`), the layout will assign 
/// it to the element's index in the layout's array. Otherwise, the UI element will have the given 
/// `bind_id`.
///
/// @NOTE: The outline thickness as well as the internal button padding can be set 
/// using the internal `buttons_outline_thickness` and `buttons_padding` in `UILayout`.
NIKOLA_API UIButton& ui_layout_push_button(UILayout& layout, 
                                            const String& str, 
                                            const f32 size, 
                                            const Vec4& color, 
                                            const Vec4& outline_color, 
                                            const Vec4& text_color, 
                                            const i32 bind_id          = -1,
                                            const Vec2& layout_padding = Vec2(0.0f));

/// Push a new checkbox entry to `layout`, using `size`, `color`, `outline_color`, and `bind_id`.
/// Use `layout_padding` for extra padding between this element and the next.
///
/// @NOTE: When the `bind_id` is left as the default value (which is `-1`), the layout will assign 
/// it to the element's index in the layout's array. Otherwise, the UI element will have the given 
/// `bind_id`.
NIKOLA_API UICheckbox& ui_layout_push_checkbox(UILayout& layout, 
                                               const f32 size, 
                                               const Vec4& color, 
                                               const Vec4& outline_color, 
                                               const i32 bind_id          = -1,
                                               const Vec2& layout_padding = Vec2(0.0f));

/// Push a new slider entry to `layout`, using `value`, `min`, `max`, `step`, `color`, `notch_color`, and `bind_id`.
/// Use `layout_padding` for extra padding between this element and the next.
///
/// @NOTE: When the `bind_id` is left as the default value (which is `-1`), the layout will assign 
/// it to the element's index in the layout's array. Otherwise, the UI element will have the given 
/// `bind_id`.
NIKOLA_API UISlider& ui_layout_push_slider(UILayout& layout, 
                                           f32* value,
                                           const f32 min, 
                                           const f32 max,
                                           const f32 step,
                                           const Vec4& color, 
                                           const Vec4& notch_color    = Vec4(0.5f, 0.5f, 0.5f, 1.0f), 
                                           const i32 bind_id          = -1,
                                           const Vec2& layout_padding = Vec2(0.0f));

/// Push a new image entry to `layout`, using the given `texture_id`, `size`, and `tint`.
/// Use `layout_padding` for extra padding between this element and the next.
NIKOLA_API UIImage& ui_layout_push_image(UILayout& layout, 
                                         const ResourceID& texture_id, 
                                         const Vec2& size, 
                                         const Vec4& tint           = Vec4(1.0f), 
                                         const Vec2& layout_padding = Vec2(0.0f));

/// Render all of the active UI elements in the given `layout`.
NIKOLA_API void ui_layout_render(UILayout& layout);

/// UILayout functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
