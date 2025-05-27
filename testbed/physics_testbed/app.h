#pragma once

#include <nikola/nikola_base.h>
#include <nikola/nikola_app.h>

/// ----------------------------------------------------------------------
/// App functions 

nikola::App* app_init(const nikola::Args& args, nikola::Window* window);
void app_shutdown(nikola::App* app);

void app_update(nikola::App* app, const nikola::f64 delta_time);
void app_render(nikola::App* app);
void app_render_gui(nikola::App* app);

/// App functions 
/// ----------------------------------------------------------------------
