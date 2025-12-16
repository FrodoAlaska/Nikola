#pragma once

#include "nikola_math.h" 
#include "nikola_physics.h"
#include "nikola_resources.h"
#include "nikola_timer.h"
#include "nikola_render.h"
#include "nikola_pch.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Entity ***

/// ----------------------------------------------------------------------
/// Consts

const u32 ENITTY_INVALID = ((u32)-1);

/// Consts
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// EntityRenderableType
enum EntityRenderableType {
  ENTITY_RENDERABLE_MESH,
  ENTITY_RENDERABLE_MODEL,
  ENTITY_RENDERABLE_DEBUG_CUBE,
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
/// Callbacks

using OnCollisionEnterFn = std::function<void(const EntityWorld& world, const EntityID& entt)>;

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// PhysicsComponent
struct PhysicsComponent {
  PhysicsBody* body; 
  OnCollisionEnterFn coll_func;
};
/// PhysicsComponent
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// CharacterComponent
struct CharacterComponent {
  Character* character; 
  OnCollisionEnterFn coll_func;
};
/// CharacterComponent
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// RenderableComponent
struct RenderableComponent {
  EntityRenderableType type;

  ResourceID renderable_id; 
  ResourceID material_id;
};
/// RenderableComponent
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// InstancedRenderableComponent
struct InstancedRenderableComponent {
  EntityRenderableType type; 

  ResourceID renderable_id; 
  ResourceID material_id;

  DynamicArray<Transform> transforms;
};
/// InstancedRenderableComponent
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// EntityWorld functions

NIKOLA_API void entity_world_clear(EntityWorld& world);

NIKOLA_API EntityID entity_world_create_entity(EntityWorld& world,
                                               const Vec3& position, 
                                               const Quat& rotation = Quat(1.0f, 0.0f, 0.0f, 0.0f), 
                                               const Vec3& scale    = Vec3(1.0f));

NIKOLA_API void entity_world_destroy_entity(EntityWorld& world, EntityID& entt);

NIKOLA_API void entity_world_update(EntityWorld& world, const f64 delta_time);

NIKOLA_API void entity_world_render(const EntityWorld& world);

/// EntityWorld functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// EntityID functions

template<typename Comp, typename... Args>
NIKOLA_API void entity_add_component(EntityWorld& world, EntityID& entt, Args&&... args) {
  world.emplace<Comp>(entt, std::forward<Args>(args)...);
}

NIKOLA_API void entity_add_physics_body(EntityWorld& world, 
                                        EntityID& entt, 
                                        PhysicsBodyDesc& desc, 
                                        const OnCollisionEnterFn& coll_func = nullptr);

NIKOLA_API void entity_add_character_body(EntityWorld& world, 
                                          EntityID& entt, 
                                          CharacterBodyDesc& desc, 
                                          const OnCollisionEnterFn& coll_func = nullptr);

NIKOLA_API void entity_add_audio_source(EntityWorld& world, 
                                        EntityID& entt, 
                                        AudioSourceDesc& desc, 
                                        const ResourceID& audio_buffer_id);

NIKOLA_API void entity_add_timer(EntityWorld& world, 
                                 EntityID& entt, 
                                 const f32 max_time, 
                                 const bool one_shot, 
                                 const bool active = true);

NIKOLA_API void entity_add_particle_emitter(EntityWorld& world, EntityID& entt, ParticleEmitterDesc& desc);

NIKOLA_API void entity_add_animation_sampler(EntityWorld& world, 
                                             EntityID& entt, 
                                             const ResourceID& skeleton_id, 
                                             const ResourceID& animation_id);

NIKOLA_API void entity_add_animation_sampler(EntityWorld& world, 
                                             EntityID& entt, 
                                             const ResourceID& skeleton_id, 
                                             const ResourceID* animations, 
                                             const sizei animations_count);

NIKOLA_API void entity_add_animation_blender(EntityWorld& world, EntityID& entt, const ResourceID& skeleton_id);

NIKOLA_API void entity_add_renderable(EntityWorld& world, 
                                      EntityID& entt, 
                                      const EntityRenderableType renderable_type, 
                                      const ResourceID& renderable_id, 
                                      const ResourceID& material_id = {});

NIKOLA_API void entity_add_instanced_renderable(EntityWorld& world, 
                                                EntityID& entt, 
                                                const EntityRenderableType renderable_type, 
                                                const DynamicArray<Transform>& transforms,
                                                const ResourceID& renderable_id, 
                                                const ResourceID& material_id = {});

template<typename Comp>
NIKOLA_API Comp& entity_get_component(EntityWorld& world, EntityID& entt) {
  return world.get<Comp>(entt);
}

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
