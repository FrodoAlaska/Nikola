#pragma once

#include "nikola_base.h"
#include "nikola_resources.h"
#include "nikola_render.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// GUI functions

NIKOLA_API bool gui_init(Window* window);

NIKOLA_API void gui_shutdown();

NIKOLA_API void gui_begin();

NIKOLA_API void gui_end();

NIKOLA_API void gui_begin_panel(const char* name);

NIKOLA_API void gui_end_panel();

NIKOLA_API void gui_settings_debug();

NIKOLA_API void gui_settings_camera(Camera* camera);

NIKOLA_API void gui_settings_resource(const u16 resource_group);

NIKOLA_API void gui_settings_renderer();

NIKOLA_API void gui_settings_material(const char* name, Material* material);

NIKOLA_API void gui_settings_transform(const char* name, Transform* transform);

NIKOLA_API void gui_settings_texture(const char* name, GfxTexture* texture);

/// GUI functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
