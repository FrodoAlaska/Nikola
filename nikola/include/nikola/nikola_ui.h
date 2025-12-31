#pragma once

#include "nikola_resources.h"
#include "nikola_render.h"
#include "nikola_containers.h"
#include "nikola_audio.h"
#include "nikola_physics.h"
#include "nikola_timer.h"
#include "nikola_entity.h"

/// ----------------------------------------------------------------------

// Some workaround so we don't have to include RmlUi here... probably not the best solution, but oh well.

namespace Rml {
  class Context;
  class ElementDocument;
  class Element;
  
  template<typename T>
  class Releaser;

  using ElementPtr = std::unique_ptr<Rml::Element, Rml::Releaser<Rml::Element>>;
}

/// ----------------------------------------------------------------------

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

NIKOLA_API UIContext* ui_context_create(const String& name, const IVec2 bounds);

NIKOLA_API void ui_context_destroy(UIContext* ui_ctx);

NIKOLA_API void ui_context_update(UIContext* ui_ctx);

NIKOLA_API void ui_context_render(UIContext* ui_ctx);

NIKOLA_API void ui_context_enable_mouse_cursor(UIContext* ui_ctx, const bool enable);

NIKOLA_API void ui_context_activate_theme(UIContext* ui_ctx, const String& theme_name, const bool active);

NIKOLA_API bool ui_context_is_theme_active(UIContext* ui_ctx, const String& theme_name);

NIKOLA_API UIElement* ui_context_get_hover_element(UIContext* ui_ctx);

NIKOLA_API UIElement* ui_context_get_focus_element(UIContext* ui_ctx);

NIKOLA_API UIElement* ui_context_get_root_element(UIContext* ui_ctx);

NIKOLA_API sizei ui_context_get_documents_count(UIContext* ui_ctx);

NIKOLA_API UIDocument* ui_context_get_document(UIContext* ui_ctx, const String& doc_id);

NIKOLA_API UIDocument* ui_context_get_document(UIContext* ui_ctx, const sizei index);

/// UIContext functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIDocument functions

NIKOLA_API UIDocument* ui_document_load(UIContext* ui_ctx, const FilePath& doc_path);

NIKOLA_API UIDocument* ui_document_load_from_memory(UIContext* ui_ctx, const String& doc_src);

NIKOLA_API UIDocument* ui_document_create(UIContext* ui_ctx, const String& maker_name = "body");

NIKOLA_API void ui_document_close(UIDocument* ui_doc);

NIKOLA_API void ui_document_show(UIDocument* ui_doc);

NIKOLA_API void ui_document_hide(UIDocument* ui_doc);

NIKOLA_API void ui_document_enable_events(UIDocument* ui_doc);

NIKOLA_API void ui_document_pull_to_front(UIDocument* ui_doc);

NIKOLA_API void ui_document_push_to_back(UIDocument* ui_doc);

NIKOLA_API void ui_document_set_title(UIDocument* ui_doc, const String& title);

NIKOLA_API const String& ui_document_get_title(const UIDocument* ui_doc);

NIKOLA_API const String& ui_document_get_source_url(const UIDocument* ui_doc);

NIKOLA_API UIContext* ui_document_get_context(UIDocument* ui_doc);

NIKOLA_API void ui_document_reload_stylesheet(UIDocument* ui_doc); 

/// UIDocument functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIElement functions

NIKOLA_API UIElement* ui_element_create(UIDocument* ui_doc, const String& name);

NIKOLA_API void ui_element_enable_events(UIElement* ui_element);

/// UIElement functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
