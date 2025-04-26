#pragma once 

#include "scene_manager.h"

#include <nikola/nikola.h>

/// ----------------------------------------------------------------------
/// GameScene functions 

void game_scene_init();

bool game_scene_create(Scene* scene);

void game_scene_destroy(Scene* scene);

void game_scene_update(Scene* scene, const nikola::f64 dt);

void game_scene_render(Scene* scene);

void game_scene_render_gui(Scene* scene);

void game_scene_save(Scene* scene, const nikola::FilePath& path);

void game_scene_load(Scene* scene, const nikola::FilePath& path);

/// GameScene functions 
/// ----------------------------------------------------------------------
