#pragma once

#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

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

NIKOLA_API void gui_settings_resource(ResourceStorage* storage);

NIKOLA_API void gui_settings_renderer();

NIKOLA_API void gui_settings_material(Material* material);

/// GUI functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
