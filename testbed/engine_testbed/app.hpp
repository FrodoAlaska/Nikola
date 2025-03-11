#pragma once

#include <nikola/nikola_engine.hpp>

/// ----------------------------------------------------------------------
/// App functions 

nikola::App* app_init(const nikola::Args& args, nikola::Window* window);
void app_shutdown(nikola::App* app);

void app_update(nikola::App* app, const nikola::f64 delta_time);
void app_render(nikola::App* app);

/// App functions 
/// ----------------------------------------------------------------------
