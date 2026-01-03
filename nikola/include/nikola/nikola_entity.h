#pragma once

#include "nikola_math.h" 
#include "nikola_physics.h"
#include "nikola_resources.h"
#include "nikola_timer.h"
#include "nikola_render.h"
#include "nikola_pch.h"

/// ----------------------------------------------------------------------
/// Forward declarations

namespace Rml {
  class Context;
};

/// Forward declarations
/// ----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Forward declarations

using UIContext = Rml::Context;

/// Forward declarations
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Entity ***

/// ----------------------------------------------------------------------
/// EntityRenderableType
enum EntityRenderableType {
  /// Used to render an entity with a mesh.
  ENTITY_RENDERABLE_MESH,
  
  /// Used to render an entity with a model.
  ENTITY_RENDERABLE_MODEL,
  
  /// Used to render an entity with a debug cube.
  ENTITY_RENDERABLE_DEBUG_CUBE,
  
  /// Used to render an entity with a debug sphere.
  ENTITY_RENDERABLE_DEBUG_SPHERE,
};
/// EntityRenderableType
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// EntityID
using EntityID = entt::entity;
/// EntityID
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// EntityWorld
using EntityWorld = entt::registry;
/// EntityWorld
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Consts

/// Used to indicate an invalid entity ID.
const EntityID ENTITY_NULL = entt::null;

/// Consts
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

/// Called when the physics body of `entt` is collided with `other` in the physics world.
using OnCollisionEnterFn = std::function<void(EntityWorld& world, EntityID& entt, EntityID& other)>;

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// PhysicsComponent
struct PhysicsComponent {
  /// The internal handle of the physics body, 
  /// to be used with `physics_body_*` functions.
  PhysicsBody* body; 

  /// A collision callback to be initiated once 
  /// the body is collided.
  OnCollisionEnterFn coll_func;
};
/// PhysicsComponent
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// CharacterComponent
struct CharacterComponent {
  /// The internal handle of the character body, 
  /// to be used with `character_body_*` functions.
  Character* character; 

  /// A collision callback to be initiated once 
  /// the body is collided.
  OnCollisionEnterFn coll_func;
};
/// CharacterComponent
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// RenderableComponent
struct RenderableComponent {
  /// The type of the render command to be initiated.
  EntityRenderableType type;

  /// The ID of the resource to be rendered. 
  /// Must be either a model or mesh.
  ResourceID renderable_id; 
  
  /// The ID of the material to be used 
  /// in the render process.
  ResourceID material_id;
};
/// RenderableComponent
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// InstancedRenderableComponent
struct InstancedRenderableComponent {
  /// The type of the render command to be initiated.
  EntityRenderableType type; 

  /// The ID of the resource to be rendered. 
  /// Must be either a model or mesh.
  ResourceID renderable_id; 
  
  /// The ID of the material to be used 
  /// in the render process.
  ResourceID material_id;

  /// An array of transforms to be submitted 
  /// to the render instanced command.
  DynamicArray<Transform> transforms;
};
/// InstancedRenderableComponent
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// EntityWorld functions

/// Clear (and thereby destroy) the given `world` of any 
/// entities and their components.
NIKOLA_API void entity_world_clear(EntityWorld& world);

/// Create an return a new entity in the given `world`, with `position`, 
/// `rotation`, and `scale` as its transform properties.
NIKOLA_API EntityID entity_world_create_entity(EntityWorld& world,
                                               const Vec3& position, 
                                               const Quat& rotation = Quat(1.0f, 0.0f, 0.0f, 0.0f), 
                                               const Vec3& scale    = Vec3(1.0f));

/// Destroy the given `entt` and remove it and its components from `world`.
NIKOLA_API void entity_world_destroy_entity(EntityWorld& world, EntityID& entt);

/// Update all the components of `world` in a data-oriented manner, using 
/// `delta_time` as the time scale. 
///
/// @NOTE: This function _MUST_ be called only once per frame. 
NIKOLA_API void entity_world_update(EntityWorld& world, const f64 delta_time);

/// Render all the components of `world` in a data-oriented manner.
///
/// @NOTE: This function _MUST_ be called only once per frame after calling 
/// `renderer_begin` and _BEFORE_ `renderer_end`.
NIKOLA_API void entity_world_render(const EntityWorld& world);

/// Render all the UI components of `world` in a data-oriented manner.
///
/// @NOTE: This function _MUST_ be called only once per frame after calling 
/// `ui_renderer_begin` and _BEFORE_ `ui_renderer_end`.
NIKOLA_API void entity_world_render_ui(const EntityWorld& world);

/// EntityWorld functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// EntityID functions

/// Add a generic component `Comp` with `Args` initialization arguments 
/// to the given `entt` in the respective `world`.
template<typename Comp, typename... Args>
NIKOLA_API void entity_add_component(EntityWorld& world, EntityID& entt, Args&&... args) {
  world.emplace<Comp>(entt, std::forward<Args>(args)...);
}

/// A helper function to add a physics body to `entt`, using the information 
/// in `desc`, and `coll_func` to call later on collision events. 
///
/// @NOTE: The position, rotation, and user data of the given `desc` will be
/// set inside the function using the transform of `entt` and its ID respectively.
/// However, the rest of the memebers of `desc` must be filled by the caller.
NIKOLA_API void entity_add_physics_body(EntityWorld& world, 
                                        EntityID& entt, 
                                        PhysicsBodyDesc& desc, 
                                        const OnCollisionEnterFn& coll_func = nullptr);

/// A helper function to add a character body to `entt`, using the information 
/// in `desc`, and `coll_func` to call later on collision events. 
///
/// @NOTE: The position, rotation, and user data of the given `desc` will be
/// set inside the function using the transform of `entt` and its ID respectively.
/// However, the rest of the memebers of `desc` must be filled by the caller.
NIKOLA_API void entity_add_character_body(EntityWorld& world, 
                                          EntityID& entt, 
                                          CharacterBodyDesc& desc, 
                                          const OnCollisionEnterFn& coll_func = nullptr);

/// A helper function to add an audio source to `entt`, using the information 
/// in `desc`, and the `audio_buffer_id` as the main buffer.
///
/// @NOTE: The position of the given `desc` will be set inside the function
/// using the current position of `entt`.
/// However, the rest of the memebers of `desc` must be filled by the caller.
NIKOLA_API void entity_add_audio_source(EntityWorld& world, 
                                        EntityID& entt, 
                                        AudioSourceDesc& desc, 
                                        const ResourceID& audio_buffer_id);

/// A helper function to add a timer component to `entt`, using the given 
/// `max_time`, `one_shot`, and `active` parameters, mirroring the 
/// `timer_create` function.
NIKOLA_API void entity_add_timer(EntityWorld& world, 
                                 EntityID& entt, 
                                 const f32 max_time, 
                                 const bool one_shot, 
                                 const bool active = true);

/// A helper function to add a particle emitter to `entt`, using the information 
/// in `desc`.
///
/// @NOTE: The position of the given `desc` will be set inside the function
/// using the current position of `entt`.
/// However, the rest of the memebers of `desc` must be filled by the caller.
NIKOLA_API void entity_add_particle_emitter(EntityWorld& world, EntityID& entt, ParticleEmitterDesc& desc);

/// A helper function to add an animation sampler component to `entt`, using the 
/// given `skeleton_id` and `animation_id`, mirroring the `animation_sampler_create` function.
NIKOLA_API void entity_add_animation_sampler(EntityWorld& world, 
                                             EntityID& entt, 
                                             const ResourceID& skeleton_id, 
                                             const ResourceID& animation_id);

/// A helper function to add an animation sampler component to `entt`, using the 
/// given `skeleton_id`, `animations`, and `animations_count`, mirroring the `animation_sampler_create` function.
NIKOLA_API void entity_add_animation_sampler(EntityWorld& world, 
                                             EntityID& entt, 
                                             const ResourceID& skeleton_id, 
                                             const ResourceID* animations, 
                                             const sizei animations_count);

/// A helper function to add a UI context component to `entt`, using the given `name` and `bounds`.
NIKOLA_API void entity_add_ui_context(EntityWorld& world, 
                                      EntityID& entt, 
                                      const String& name, 
                                      const IVec2& bounds);

/// A helper function to add an animation blender component to `entt`, using the 
/// given `skeleton_id`, mirroring the `animation_blender_create` function.
NIKOLA_API void entity_add_animation_blender(EntityWorld& world, EntityID& entt, const ResourceID& skeleton_id);

/// A helper function to add a renderable component to `entt`, using the given
/// `renderable_type` to distinguish the render command, and `renderable_id` and `material_id`
/// to give to the render command.
NIKOLA_API void entity_add_renderable(EntityWorld& world, 
                                      EntityID& entt, 
                                      const EntityRenderableType renderable_type, 
                                      const ResourceID& renderable_id, 
                                      const ResourceID& material_id = {});

/// A helper function to add an instanced renderable component to `entt`, using the given
/// `renderable_type` to distinguish the render command, and a `transforms` array of all 
/// the possible instances of the render command. 
/// Of course, the given `renderable_id` and `material_id` will be given to the render command as well.
NIKOLA_API void entity_add_instanced_renderable(EntityWorld& world, 
                                                EntityID& entt, 
                                                const EntityRenderableType renderable_type, 
                                                const DynamicArray<Transform>& transforms,
                                                const ResourceID& renderable_id, 
                                                const ResourceID& material_id = {});

/// Retrieve a reference to a generic component `Comp` from `entt` that lives in the given `world`.
///
/// @NOTE: It is often advised to first use `entity_has_component` to first check 
/// if the given `entt` has that component in the first place.
template<typename Comp>
NIKOLA_API Comp& entity_get_component(EntityWorld& world, EntityID& entt) {
  return world.get<Comp>(entt);
}

/// Retrieve a const reference to a generic component `Comp` from `entt` that lives in the given `world`.
///
/// @NOTE: It is often advised to first use `entity_has_component` to first check 
/// if the given `entt` has that component in the first place.
template<typename Comp>
NIKOLA_API const Comp& entity_get_component_const(const EntityWorld& world, const EntityID& entt) {
  return world.get<Comp>(entt);
}

/// Return true if the given `Comp` component type currently exists in 
/// the given `entt` that lives in `world`.
template<typename Comp>
NIKOLA_API bool entity_has_component(EntityWorld& world, EntityID& entt) {
  return world.any_of<Comp>(entt);
}

/// EntityID functions
/// ----------------------------------------------------------------------

/// *** Entity ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
