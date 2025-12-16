#include "nikola/nikola_entity.h"
#include "nikola/nikola_event.h"

#include <entt/entt.hpp>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Entity ***

/// ----------------------------------------------------------------------
/// EntityWorld functions

void entity_world_clear(EntityWorld& world) {
  world.clear();
}

Entity entity_world_create_entity(EntityWorld& world,
                                  const Vec3& position, 
                                  const Quat& rotation,
                                  const Vec3& scale) {
  // Create a new entity
  Entity entt(world.create());

  // Add a transform component  

  Transform transform; 
  transform.position = position; 
  transform.rotation = rotation; 
  transform.scale    = scale;
  transform_apply(transform);

  world.emplace<Transform>(entt.get_id(), transform);

  // Dispatch an event

  Event event = {
    .type    = EVENT_ENTITY_ADDED, 
    .entt_id = &entt,  // @TODO (Entity): NOOOOOOOO!!!!!
  };
  event_dispatch(event);

  // Done!
  return entt;
}

void entity_world_destroy_entity(EntityWorld& world, Entity& entt) {
  // Dispatch an event

  Event event = {
    .type    = EVENT_ENTITY_DESTROYED, 
    .entt_id = &entt,  // @TODO (Entity): NOOOOOOOO!!!!!
  };
  event_dispatch(event);

  // Destroy any components that require it 

  if(world.any_of<PhysicsBody*>(entt.get_id())) {
    PhysicsBody* body = world.get<PhysicsBody*>(entt.get_id());
    physics_world_remove_and_destroy_body(&body);
  }

  if(world.any_of<Character*>(entt.get_id())) {
    Character* character = world.get<Character*>(entt.get_id());
    
    physics_world_remove_character(character);
    character_body_destroy(&character);
  }

  if(world.any_of<AnimationSampler*>(entt.get_id())) {
    AnimationSampler* sampler = world.get<AnimationSampler*>(entt.get_id());
    animation_sampler_destroy(sampler);
  }

  if(world.any_of<AnimationBlender*>(entt.get_id())) {
    AnimationBlender* blender = world.get<AnimationBlender*>(entt.get_id());
    animation_blender_destroy(blender);
  }

  // Destroy the entity in the world
  world.destroy(entt.get_id()); 
}

void entity_world_update(const EntityWorld& world, const f64 delta_time) {
  NIKOLA_PROFILE_FUNCTION();

  // @TODO (Entity)
}

void entity_world_render(const EntityWorld& world) {
  NIKOLA_PROFILE_FUNCTION();
  
  // @TODO (Entity)
}

/// EntityWorld functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Entity functions

void entity_add_tag(EntityWorld& world, Entity& entt, const String& tag) {
  world.emplace<String>(entt.get_id(), tag);
}

void entity_add_physics_body(EntityWorld& world, 
                             Entity& entt, 
                             PhysicsBodyDesc& desc, 
                             const OnCollisionEnterFn& coll_func) {
  Transform& transform = world.get<Transform>(entt.get_id());

  entt.coll_func = coll_func;
  
  desc.position     = transform.position;
  desc.rotation     = transform.rotation;
  desc.user_data    = (u64)entt.get_id();
  PhysicsBody* body = physics_world_create_and_add_body(desc);

  world.emplace<PhysicsBody*>(entt.get_id(), body);
}

void entity_add_character_body(EntityWorld& world, 
                               Entity& entt, 
                               CharacterBodyDesc& desc, 
                               const OnCollisionEnterFn& coll_func) {
  Transform& transform = world.get<Transform>(entt.get_id());

  entt.coll_func = coll_func;
  
  desc.position  = transform.position;
  desc.rotation  = transform.rotation;
  desc.user_data = (u64)entt.get_id();

  Character* character = character_body_create(desc);
  physics_world_add_character(character);

  world.emplace<Character*>(entt.get_id(), character);
}

void entity_add_audio_source(EntityWorld& world, 
                             Entity& entt, 
                             AudioSourceDesc& desc, 
                             const ResourceID& audio_buffer_id) {
  Transform& transform = world.get<Transform>(entt.get_id());

  desc.position      = transform.position; 
  desc.buffers[0]    = resources_get_audio_buffer(audio_buffer_id);
  desc.buffers_count = 1;

  world.emplace<AudioSourceID>(entt.get_id(), audio_source_create(desc));
}

void entity_add_timer(EntityWorld& world, 
                      Entity& entt, 
                      const f32 max_time, 
                      const bool one_shot, 
                      const bool active) {
  Timer timer; 
  timer_create(&timer, max_time, one_shot, active);

  world.emplace<Timer>(entt.get_id(), timer);
}

void entity_add_particle_emitter(EntityWorld& world, Entity& entt, ParticleEmitterDesc& desc) {
  Transform& transform = world.get<Transform>(entt.get_id());
  desc.position        = transform.position;

  ParticleEmitter emitter; 
  particle_emitter_create(&emitter, desc);

  world.emplace<ParticleEmitter>(entt.get_id(), emitter);
}

void entity_add_animation_sampler(EntityWorld& world, 
                                  Entity& entt, 
                                  const ResourceID& skeleton_id, 
                                  const ResourceID& animation_id) {
  AnimationSampler* sampler = animation_sampler_create(skeleton_id, animation_id); 
  world.emplace<AnimationSampler*>(entt.get_id(), sampler);
}

void entity_add_animation_sampler(EntityWorld& world, 
                                  Entity& entt, 
                                  const ResourceID& skeleton_id, 
                                  const ResourceID* animations, 
                                  const sizei animations_count) {
  AnimationSampler* sampler = animation_sampler_create(skeleton_id, animations, animations_count); 
  world.emplace<AnimationSampler*>(entt.get_id(), sampler);
}

void entity_add_animation_blender(EntityWorld& world, Entity& entt, const ResourceID& skeleton_id) {
  AnimationBlender* blender = animation_blender_create(skeleton_id);
  world.emplace<AnimationBlender*>(entt.get_id(), blender);
}

void entity_add_renderable(EntityWorld& world, 
                           Entity& entt, 
                           const EntityRenderableType renderable_type, 
                           const ResourceID& renderable_id, 
                           const ResourceID& material_id) {
  world.emplace<RenderableComponent>(entt.get_id(), renderable_type, renderable_id, material_id);
}

/// Entity functions
/// ----------------------------------------------------------------------

/// *** Entity ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
