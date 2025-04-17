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

NIKOLA_API bool gui_begin_panel(const char* name);

NIKOLA_API void gui_end_panel();

NIKOLA_API void gui_debug_info();

NIKOLA_API void gui_renderer_info();

NIKOLA_API void gui_edit_transform(const char* name, Transform* transform);

NIKOLA_API void gui_edit_camera(Camera* camera);

NIKOLA_API void gui_edit_directional_light(const char* name, DirectionalLight* dir_light);

NIKOLA_API void gui_edit_point_light(const char* name, PointLight* point_light);

NIKOLA_API void gui_edit_material(const char* name, Material* material);

NIKOLA_API void gui_edit_texture(const char* name, GfxTexture* texture);

NIKOLA_API void gui_edit_cubemap(const char* name, GfxCubemap* cubemap);

NIKOLA_API void gui_edit_resource(const char* name, ResourceID& res_id);

/// GUI functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
