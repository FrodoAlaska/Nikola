#pragma once

#include "nikola_base.h"
#include "nikola_resources.h"
#include "nikola_render.h"
#include "nikola_containers.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Scene ***

///---------------------------------------------------------------------------------------------------------------------
/// EntityID
typedef u64 EntityID;
/// EntityID
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Scene callbacks

struct Scene;

/// A callback to create a `Scene` struct with the given `name`.
///
/// @NOTE: A new allocated `Scene` struct MUST be returned.
using SceneCreateFn  = Scene*(*)(const String& name);

/// A callback to be invoked every frame, using the data from `scene`.
using SceneUpdateFn  = void(*)(Scene* scene);

/// A callback to destroy the given `scene`.
using SceneDestroyFn = void(*)(Scene* scene);

/// Scene callbacks
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Scene
struct Scene {
  String debug_name;

  Camera camera;
  u16 resource_group; 
  RenderQueue render_queue;

  SceneCreateFn create_func;
  SceneUpdateFn update_func; 
  SceneDestroyFn destroy_func;

  HashMap<EntityID, ResourceID> renderables;
};
/// Scene
///---------------------------------------------------------------------------------------------------------------------

/// *** Scene ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
