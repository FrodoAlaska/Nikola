#pragma once

#include "nikola_math.h" 
#include "nikola_physics.h"
#include "nikola_resources.h"
#include "nikola_timer.h"
#include "nikola_render.h"

//////////////////////////////////////////////////////////////////////////

/// ----------------------------------------------------------------------
/// EnTT namespace for abstraction

namespace entt {
  template<typename Entity, typename Allocator>
  class basic_registry;
}

/// EnTT namespace for abstraction
/// ----------------------------------------------------------------------

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
  ENTITY_RENDERABLE_ANIMATION,
  ENTITY_RENDERABLE_DEBUG_MESH,
};
/// EntityRenderableType
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// EntityID
using EntityID = u32;
/// EntityID
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// EntityWorld
using EntityWorld = entt::basic_registry<EntityID, std::allocator<EntityID>>;
/// EntityWorld
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

struct Entity; // Forward declaration... 

using OnCollisionEnterFn = std::function<void(const EntityWorld& world, const Entity& entt)>;

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Entity
struct Entity {
  Entity(const u32 id) 
    :_id(id) 
  {}

  public:
    OnCollisionEnterFn coll_func = nullptr; 

  public:
    EntityID get_id() const {return _id;}

  private:
    EntityID _id = ((u32)-1);
};
/// EntityID
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
/// EntityWorld functions

NIKOLA_API void entity_world_clear(EntityWorld& world);

NIKOLA_API Entity entity_world_create_entity(EntityWorld& world,
                                             const Vec3& position, 
                                             const Quat& rotation = Quat(1.0f, 0.0f, 0.0f, 0.0f), 
                                             const Vec3& scale    = Vec3(1.0f));

NIKOLA_API void entity_world_destroy_entity(EntityWorld& world, Entity& entt);

NIKOLA_API void entity_world_update(const EntityWorld& world, const f64 delta_time);

NIKOLA_API void entity_world_render(const EntityWorld& world);

/// EntityWorld functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Entity functions

NIKOLA_API void entity_add_tag(EntityWorld& world, Entity& entt, const String& tag);

NIKOLA_API void entity_add_physics_body(EntityWorld& world, 
                                        Entity& entt, 
                                        PhysicsBodyDesc& desc, 
                                        const OnCollisionEnterFn& coll_func = nullptr);

NIKOLA_API void entity_add_character_body(EntityWorld& world, 
                                          Entity& entt, 
                                          CharacterBodyDesc& desc, 
                                          const OnCollisionEnterFn& coll_func = nullptr);

NIKOLA_API void entity_add_audio_source(EntityWorld& world, 
                                        Entity& entt, 
                                        AudioSourceDesc& desc, 
                                        const ResourceID& audio_buffer_id);

NIKOLA_API void entity_add_timer(EntityWorld& world, 
                                 Entity& entt, 
                                 const f32 max_time, 
                                 const bool one_shot, 
                                 const bool active = true);

NIKOLA_API void entity_add_particle_emitter(EntityWorld& world, Entity& entt, ParticleEmitterDesc& desc);

NIKOLA_API void entity_add_animation_sampler(EntityWorld& world, 
                                             Entity& entt, 
                                             const ResourceID& skeleton_id, 
                                             const ResourceID& animation_id);

NIKOLA_API void entity_add_animation_sampler(EntityWorld& world, 
                                             Entity& entt, 
                                             const ResourceID& skeleton_id, 
                                             const ResourceID* animations, 
                                             const sizei animations_count);

NIKOLA_API void entity_add_animation_blender(EntityWorld& world, Entity& entt, const ResourceID& skeleton_id);

NIKOLA_API void entity_add_renderable(EntityWorld& world, 
                                      Entity& entt, 
                                      const EntityRenderableType renderable_type, 
                                      const ResourceID& renderable_id, 
                                      const ResourceID& material_id = {});

/// Entity functions
/// ----------------------------------------------------------------------

/// *** Entity ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
