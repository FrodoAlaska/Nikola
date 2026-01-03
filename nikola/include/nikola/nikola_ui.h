#pragma once

#include "nikola_resources.h"
#include "nikola_render.h"
#include "nikola_containers.h"
#include "nikola_audio.h"
#include "nikola_physics.h"
#include "nikola_timer.h"
#include "nikola_entity.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
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
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIContext 
using UIContext = Rml::Context;
/// UIContext 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIDocument 
using UIDocument = Rml::ElementDocument;
/// UIDocument 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIElement 
using UIElement = Rml::Element;
/// UIElement 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIElementPtr 
using UIElementPtr = Rml::ElementPtr;
/// UIElementPtr 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIText 
using UIText = Rml::ElementText;
/// UIText 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIProgress 
using UIProgress = Rml::ElementProgress;
/// UIProgress 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIControlInput 
using UIControlInput = Rml::ElementFormControlInput;
/// UIControlInput 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIControlSelect 
using UIControlSelect = Rml::ElementFormControlSelect;
/// UIControlSelect 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UITabMenu 
using UITabMenu = Rml::ElementTabSet;
/// UITabMenu 
///---------------------------------------------------------------------------------------------------------------------

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

/// Add an entity section identified by `name` to edit the given `entt` using the `world` to retrieve components.
NIKOLA_API void gui_edit_entity(const char* name, EntityWorld& world, EntityID& entt);

/// GUI functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIContext functions

/// Allocate and return a `UIContext` object with a `name` and `bounds`.
NIKOLA_API UIContext* ui_context_create(const String& name, const IVec2 bounds);

/// Reclaim any memory allocated by `ui_ctx`.
NIKOLA_API void ui_context_destroy(UIContext* ui_ctx);

/// Update the given `ui_ctx`.
NIKOLA_API void ui_context_update(UIContext* ui_ctx);

/// Render the given `ui_ctx`.
///
/// @NOTE: This function _MUST_ be called _after_ `ui_renderer_begin`, and 
/// _before_ `ui_renderer_end`.
NIKOLA_API void ui_context_render(UIContext* ui_ctx);

/// If the given flag `enable` is `true`, the `ui_ctx` will be allowed to 
/// take mouse input.
NIKOLA_API void ui_context_enable_mouse_cursor(UIContext* ui_ctx, const bool enable);

/// Activate/deactivate the theme `theme_name` in `ui_ctx`. 
NIKOLA_API void ui_context_activate_theme(UIContext* ui_ctx, const String& theme_name, const bool active);

/// Check if the theme `theme_name` in `ui_ctx` is currently active or not.
NIKOLA_API bool ui_context_is_theme_active(UIContext* ui_ctx, const String& theme_name);

/// Retrieve the `UIElement` that is currently being hovered over in `ui_ctx`.
NIKOLA_API UIElement* ui_context_get_hover_element(UIContext* ui_ctx);

/// Retrieve the `UIElement` that is currently focused in `ui_ctx`.
NIKOLA_API UIElement* ui_context_get_focus_element(UIContext* ui_ctx);

/// Retrieve the root/parent `UIElement` in `ui_ctx`.
NIKOLA_API UIElement* ui_context_get_root_element(UIContext* ui_ctx);

/// Retrieve the number of documents in `ui_ctx`.
NIKOLA_API sizei ui_context_get_documents_count(UIContext* ui_ctx);

/// Retrieve the document with the name `doc_id` in `ui_ctx`.
NIKOLA_API UIDocument* ui_context_get_document(UIContext* ui_ctx, const String& doc_id);

/// Retrieve the document at `index` in `ui_ctx`.
NIKOLA_API UIDocument* ui_context_get_document(UIContext* ui_ctx, const sizei index);

/// UIContext functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UI debugger functions

/// Initialize the UI debugger, enabling debugging capabilities on `ui_ctx`.
NIKOLA_API void ui_debugger_init(UIContext* ui_ctx);

/// Shutdown the UI debugger system.
NIKOLA_API void ui_debugger_shutdown();

/// Set the debugger to target `ui_ctx`.
NIKOLA_API void ui_debugger_set_context(UIContext* ui_ctx);

/// Depending on the given `visible` flag, enable or disable the debugger.
NIKOLA_API void ui_debugger_set_visible(const bool visible);

/// Check whether the debugger is currently active or not.
NIKOLA_API bool ui_debugger_is_visible();

/// UI debugger functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIDocument functions

/// Allocate and load a new `UIDocument`, using the given `doc_path` to retrieve the RML file.
NIKOLA_API UIDocument* ui_document_load(UIContext* ui_ctx, const FilePath& doc_path);

/// Allocate and load a new `UIDocument`, using the given `doc_src` as the source of the RML content.
NIKOLA_API UIDocument* ui_document_load_from_memory(UIContext* ui_ctx, const String& doc_src);

/// Allocate and new (empty) `UIDocument`, using the given `maker_name` as the root element.
NIKOLA_API UIDocument* ui_document_create(UIContext* ui_ctx, const String& maker_name = "body");

/// Unload the given `ui_doc`.
NIKOLA_API void ui_document_unload(UIDocument* ui_doc);

/// Close the given `ui_doc`, disabling it completely.
NIKOLA_API void ui_document_close(UIDocument* ui_doc);

/// Show the `ui_doc` to enable it to render.
NIKOLA_API void ui_document_show(UIDocument* ui_doc);

/// Hide the `ui_doc` to disable it.
NIKOLA_API void ui_document_hide(UIDocument* ui_doc);

/// Check whether the given `ui_doc` is currently shown or not.
NIKOLA_API bool ui_document_is_shown(UIDocument* ui_doc);

/// Enable the given `ui_doc` to push events to the engine.
NIKOLA_API void ui_document_enable_events(UIDocument* ui_doc);

/// Disable the ability of the given `ui_doc` to push events to the engine.
NIKOLA_API void ui_document_disable_events(UIDocument* ui_doc);

/// Pull the given `ui_doc` to the front, making it render on top of all other documents currently active.
NIKOLA_API void ui_document_pull_to_front(UIDocument* ui_doc);

/// Push the given `ui_doc` to the back, making it render under all other documents currently active.
NIKOLA_API void ui_document_push_to_back(UIDocument* ui_doc);

/// Reload the stylesheet of the given `ui_doc`, using the original path given when it was loaded.
NIKOLA_API void ui_document_reload_stylesheet(UIDocument* ui_doc); 

/// Append the given `element` as a child to `ui_doc`.
NIKOLA_API void ui_document_append_child(UIDocument* ui_doc, UIElementPtr element); 

/// Insert the given `element` before `adjacent_element` in the given `ui_doc`.
NIKOLA_API void ui_document_insert_before(UIDocument* ui_doc, UIElementPtr element, UIElement* adjacent_element); 

/// Replace the given `other_element` with `element` in the given `ui_doc`.
NIKOLA_API void ui_document_replace_child(UIDocument* ui_doc, UIElementPtr element, UIElement* other_element); 

/// Remove the given `element` from the given `ui_doc`.
NIKOLA_API void ui_document_remove_child(UIDocument* ui_doc, UIElement* element); 

/// Set the title of the given `ui_doc` to `title`.
NIKOLA_API void ui_document_set_title(UIDocument* ui_doc, const String& title);

/// Retrieve the current title of `ui_doc`.
NIKOLA_API const String& ui_document_get_title(const UIDocument* ui_doc);

/// Retrieve the source URL of `ui_doc`.
NIKOLA_API const String& ui_document_get_source_url(const UIDocument* ui_doc);

/// Retrieve the `UIContext` that was given to `ui_doc` on creation.
NIKOLA_API UIContext* ui_document_get_context(UIDocument* ui_doc);

/// Retrieve a child `UIElement` with `id` ID in the given `ui_doc`.
///
/// @NOTE: If no `UIElement` with `id` was found in `ui_doc`, this function will return a `nullptr`.
NIKOLA_API UIElement* ui_document_get_element_by_id(UIDocument* ui_doc, const String& id);

/// Retrieve a list of `UIElement`s that share the same `tag_name` in `ui_doc` and save the results to `out_elements`.
NIKOLA_API void ui_document_get_elements_by_tag(UIDocument* ui_doc, const String& tag_name, DynamicArray<UIElement*>& out_elements);

/// Retrieve a list of `UIElement`s that share the same `class_name` in `ui_doc` and save the results to `out_elements`.
NIKOLA_API void ui_document_get_elements_by_class(UIDocument* ui_doc, const String& class_name, DynamicArray<UIElement*>& out_elements);

/// Retrieve a `UIElement` with the same RCSS selector query `selector_name` in `ui_doc` .
NIKOLA_API UIElement* ui_document_query_selector(UIDocument* ui_doc, const String& selector_name);

/// Retrieve a list of `UIElement`s that share the same RCSS selector query `selector_name` 
/// in `ui_doc` and save the results to `out_elements`.
NIKOLA_API void ui_document_query_selector_all(UIDocument* ui_doc, const String& selector_name, DynamicArray<UIElement*>& out_elements);

/// UIDocument functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIElement functions

/// Allocate and return a new (empty) `UIElementPtr` in `ui_doc` with `name`.
NIKOLA_API UIElementPtr ui_element_create(UIDocument* ui_doc, const String& name);

/// Enable the given `ui_element` to push events to the engine.
NIKOLA_API void ui_element_enable_events(UIElement* ui_element);

/// Disable the ability of the given `ui_element` to push events to the engine.
NIKOLA_API void ui_element_disable_events(UIElement* ui_element);

/// Clone the contents of `ui_element` and return a new `UIElementPtr`.
NIKOLA_API UIElementPtr ui_element_clone(UIElement* ui_element);

/// Give focus to the given `ui_element`.
NIKOLA_API void ui_element_focus(UIElement* ui_element);

/// Blur, or rather, remove focus from `ui_element`.
NIKOLA_API void ui_element_blur(UIElement* ui_element);

/// Append the given `element` as a child to `ui_element`.
NIKOLA_API void ui_element_append_child(UIElement* ui_element, UIElementPtr element); 

/// Insert the given `element` before `adjacent_element` in the given `ui_element`.
NIKOLA_API void ui_element_insert_before(UIElement* ui_element, UIElementPtr element, UIElement* adjacent_element); 

/// Replace the given `other_element` with `element` in the given `ui_element`.
NIKOLA_API void ui_element_replace_child(UIElement* ui_element, UIElementPtr element, UIElement* other_element); 

/// Remove the given `element` from the given `ui_element`.
NIKOLA_API void ui_element_remove_child(UIElement* ui_element, UIElement* element); 

/// Set the ID of `ui_element` to the given `id`.
NIKOLA_API void ui_element_set_id(UIElement* ui_element, const String& id);

/// Set the inner HTML contents of `ui_element` to the given `html`.
NIKOLA_API void ui_element_set_inner_html(UIElement* ui_element, const String& html);

/// Retrieve a child `UIElement` with `id` ID in the given `ui_element`.
///
/// @NOTE: If no `UIElement` with `id` was found in `ui_element`, this function will return a `nullptr`.
NIKOLA_API UIElement* ui_element_get_element_by_id(UIElement* ui_element, const String& id);

/// Retrieve a list of `UIElement`s that share the same `tag_name` in `ui_element` and save the results to `out_elements`.
NIKOLA_API void ui_element_get_elements_by_tag(UIElement* ui_element, const String& tag_name, DynamicArray<UIElement*>& out_elements);

/// Retrieve a list of `UIElement`s that share the same `class_name` in `ui_element` and save the results to `out_elements`.
NIKOLA_API void ui_element_get_elements_by_class(UIElement* ui_element, const String& class_name, DynamicArray<UIElement*>& out_elements);

/// Retrieve a `UIElement` with the same RCSS selector query `selector_name` in `ui_element` .
NIKOLA_API UIElement* ui_element_query_selector(UIElement* ui_element, const String& selector_name);

/// Retrieve a list of `UIElement`s that share the same RCSS selector query `selector_name` 
/// in `ui_element` and save the results to `out_elements`.
NIKOLA_API void ui_element_query_selector_all(UIElement* ui_element, const String& selector_name, DynamicArray<UIElement*>& out_elements);

/// Retrieve the owner `UIContext` of `ui_element`.
NIKOLA_API UIContext* ui_element_get_context(UIElement* ui_element);

/// Retrieve the owner `UIDocument` of `ui_element`.
NIKOLA_API UIDocument* ui_element_get_document(UIElement* ui_element);

/// Retrieve the parent element of `ui_element`.
NIKOLA_API UIElement* ui_element_get_parent(UIElement* ui_element);

/// Retrieve the child element immediately following `ui_element`.
NIKOLA_API UIElement* ui_element_get_next_sibling(UIElement* ui_element);

/// Retrieve the child element immediately preceding `ui_element`.
NIKOLA_API UIElement* ui_element_get_previous_sibling(UIElement* ui_element);

/// Retrieve the first child element of `ui_element`.
NIKOLA_API UIElement* ui_element_get_first_child(UIElement* ui_element);

/// Retrieve the last child element of `ui_element`.
NIKOLA_API UIElement* ui_element_get_last_child(UIElement* ui_element);

/// Retrieve the child element at `index` of `ui_element`.
NIKOLA_API UIElement* ui_element_get_child(UIElement* ui_element, const sizei index);

/// Retrieve the number of children `ui_element` currently has.
NIKOLA_API const sizei ui_element_get_children_count(UIElement* ui_element);

/// Retrieve the string tag of `ui_element`. 
NIKOLA_API const String& ui_element_get_tag(UIElement* ui_element);

/// Retrieve the string ID of `ui_element`. 
NIKOLA_API const String& ui_element_get_id(UIElement* ui_element);

/// Retrieve the inner HTML contents of `ui_element`. 
NIKOLA_API String ui_element_get_inner_html(UIElement* ui_element);

/// Check whether the given `ui_element` is currently visible (active).
NIKOLA_API bool ui_element_is_visible(UIElement* ui_element);

/// Check whether the given `ui_element` has any direct children.
NIKOLA_API bool ui_element_has_children(UIElement* ui_element);

/// Check whether the RCSS query selector of the given `ui_element` matches `selector`.
NIKOLA_API bool ui_element_matches(UIElement* ui_element, const String& selector);

/// UIElement functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIText functions

/// Allocate and return a new (empty) `UIText` in `ui_doc` with `text` contents.
NIKOLA_API UIText* ui_text_create(UIDocument* ui_doc, const String& text);

/// Add a new line in `ui_text` with `line_offset` from the previous line, and `line_text` contents.
NIKOLA_API void ui_text_add_line(UIText* ui_text, const Vec2& line_offset, const String& line_text);

/// Clear all lines of `ui_text`.
NIKOLA_API void ui_text_clear_lines(UIText* ui_text);

/// Set the contents of `ui_text` to `str`.
NIKOLA_API void ui_text_set_string(UIText* ui_text, const String& str);

/// Retrieve the string contents of `ui_text`.
NIKOLA_API const String& ui_text_get_string(UIText* ui_text);

/// Retrieve the width of the string contents of `ui_text`.
NIKOLA_API const sizei ui_text_get_width(UIText* ui_text);

/// UIText functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIProgress functions

/// Set the value of `ui_progress` to `value`.
NIKOLA_API void ui_progress_set_value(UIProgress* ui_progress, const f32 value);

/// Set the maximum of `ui_progress` to `max`.
NIKOLA_API void ui_progress_set_max(UIProgress* ui_progress, const f32 max);

/// Retrieve the current value of `ui_progress`.
NIKOLA_API f32 ui_progress_get_value(UIProgress* ui_progress);

/// Retrieve the current maximum of `ui_progress`.
NIKOLA_API f32 ui_progress_get_max(UIProgress* ui_progress);

/// UIProgress functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIControlInput functions

/// Set the string value of `ui_input` to `value`.
NIKOLA_API void ui_control_input_set_value(UIControlInput* ui_input, const String& value);

/// Set the selection range from `selection_start` to `selection_end` of `ui_input`.
NIKOLA_API void ui_control_input_set_selection_range(UIControlInput* ui_input, i32 selection_start, i32 selection_end);

/// Retrieve the string value of the given `ui_input`.
NIKOLA_API String ui_control_input_get_value(UIControlInput* ui_input);

/// Retrieve the current selection range of `ui_input`, writing the results in to `selection_start`,
/// `selection_end`, and `selected_text`.
NIKOLA_API void ui_control_input_get_selection_range(UIControlInput* ui_input, i32* selection_start, i32* selection_end, String* selected_text);

/// Check whether the contents of `ui_input` have been submitted or not.
NIKOLA_API bool ui_control_input_is_submitted(UIControlInput* ui_input);

/// UIControlInput functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIControlSelect functions

/// Add a new option to `ui_select` with `html` contents and `value` string value. 
/// The given `before` indicates the index of the element to insert this new option before. Leave as `-1` to append 
/// this new option to the end. 
/// The given `selectable` indicates whether this option is currently selectable or not.
NIKOLA_API void ui_control_select_add(UIControlSelect* ui_select, const String& html, const String& value, i32 before = -1, bool selectable = true);

/// Remove the option at `index` of `ui_select`.
NIKOLA_API void ui_control_select_remove(UIControlSelect* ui_select, const i32 index);

/// Remove all options of the given `ui_select`.
NIKOLA_API void ui_control_select_remove_all(UIControlSelect* ui_select);

/// Show the selection box of `ui_select`.
NIKOLA_API void ui_control_select_show_box(UIControlSelect* ui_select);

/// Hide the selection box of `ui_select`.
NIKOLA_API void ui_control_select_hide_box(UIControlSelect* ui_select);

/// Set the string value of `ui_select` to `value`.
NIKOLA_API void ui_control_select_set_value(UIControlSelect* ui_select, const String& value);

/// Set the index of the selection of `ui_select` to `selection`. 
///
/// @NOTE: If the given `selection` is out of bounds, the value will be clamped.
NIKOLA_API void ui_control_select_set_selection(UIControlSelect* ui_select, const i32 selection);

/// Check whether the selection box of the given `ui_select` is shown or not.
NIKOLA_API bool ui_control_select_is_box_shown(UIControlSelect* ui_select);

/// Retrieve the `UIElement` option of `ui_select` at `index`.
NIKOLA_API UIElement* ui_control_select_get_option(UIControlSelect* ui_select, const i32 index);

/// Retrieve the string value of `ui_select`.
NIKOLA_API String ui_control_select_get_value(UIControlSelect* ui_select);

/// Retrieve the current selection index of `ui_select`.
NIKOLA_API i32 ui_control_select_get_selection(UIControlSelect* ui_select);

/// Retrieve the number of options in `ui_select`.
NIKOLA_API sizei ui_control_select_get_options_count(UIControlSelect* ui_select);

/// UIControlSelect functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UITabMenu functions

/// Set the contents of the tab of `ui_tab` at `index` to `html`.
NIKOLA_API void ui_tab_menu_set_tab(UITabMenu* ui_tab, const i32 index, const String& html);

/// Set the contents of the panel of `ui_tab` at `index` to `html`.
NIKOLA_API void ui_tab_menu_set_panel(UITabMenu* ui_tab, const i32 index, const String& html);

/// Set the tab of `ui_tab` at `index` to `element`.
NIKOLA_API void ui_tab_menu_set_tab(UITabMenu* ui_tab, const i32 index, UIElementPtr element);

/// Set the panel of `ui_tab` at `index` to `element`.
NIKOLA_API void ui_tab_menu_set_panel(UITabMenu* ui_tab, const i32 index, UIElementPtr element);

/// Set the currently active tab of `ui_tab` to `index`.
NIKOLA_API void ui_tab_menu_set_active_tab(UITabMenu* ui_tab, const i32 index);

/// Remove the tab of `ui_tab` at `index`.
NIKOLA_API void ui_tab_menu_remove_tab(UITabMenu* ui_tab, const i32 index);

/// Retrieve the index of the currently active tab of `ui_tab`.
NIKOLA_API i32 ui_tab_menu_get_active_tab(UITabMenu* ui_tab);

/// Retrieve the number of tabs in `ui_tab`.
NIKOLA_API sizei ui_tab_menu_get_tabs_count(UITabMenu* ui_tab);

/// UITabMenu functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
