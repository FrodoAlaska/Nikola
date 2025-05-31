#pragma once

#include "nikola_base.h"
#include "nikola_resources.h"
#include "nikola_render.h"
#include "nikola_containers.h"
#include "nikola_audio.h"
#include "nikola_physics.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

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

} // End of nikola

//////////////////////////////////////////////////////////////////////////
