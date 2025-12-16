#include "nikola/nikola_entity.h"
#include "nikola/nikola_event.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Entity ***

/// ----------------------------------------------------------------------
/// EntityWorld functions

void entity_world_clear(EntityWorld& world) {
  world.clear();
}

EntityID entity_world_create_entity(EntityWorld& world,
                                  const Vec3& position, 
                                  const Quat& rotation,
                                  const Vec3& scale) {
  // Create a new entity
  EntityID entt = world.create();

  // Add a transform component  

  Transform transform; 
  transform.position = position; 
  transform.rotation = rotation; 
  transform.scale    = scale;
  transform_apply(transform);

  world.emplace<Transform>(entt, transform);

  // Dispatch an event

  Event event = {
    .type    = EVENT_ENTITY_ADDED, 
    .entt_id = (u32)entt,
  };
  event_dispatch(event);

  // Done!
  return entt;
}

void entity_world_destroy_entity(EntityWorld& world, EntityID& entt) {
  // Dispatch an event

  Event event = {
    .type    = EVENT_ENTITY_DESTROYED, 
    .entt_id = (u32)entt,
  };
  event_dispatch(event);

  // Destroy any components that require it 

  if(world.any_of<PhysicsComponent>(entt)) {
    PhysicsComponent& comp = world.get<PhysicsComponent>(entt);
    physics_world_remove_and_destroy_body(&comp.body);
  }

  if(world.any_of<CharacterComponent>(entt)) {
    CharacterComponent& comp = world.get<CharacterComponent>(entt);
    
    physics_world_remove_character(comp.character);
    character_body_destroy(&comp.character);
  }

  if(world.any_of<AnimationSampler*>(entt)) {
    AnimationSampler* sampler = world.get<AnimationSampler*>(entt);
    animation_sampler_destroy(sampler);
  }

  if(world.any_of<AnimationBlender*>(entt)) {
    AnimationBlender* blender = world.get<AnimationBlender*>(entt);
    animation_blender_destroy(blender);
  }

  // Destroy the entity in the world
  world.destroy(entt); 
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
/// EntityID functions

void entity_add_physics_body(EntityWorld& world, 
                             EntityID& entt, 
                             PhysicsBodyDesc& desc, 
                             const OnCollisionEnterFn& coll_func) {
  Transform& transform = world.get<Transform>(entt);

  desc.position     = transform.position;
  desc.rotation     = transform.rotation;
  desc.user_data    = (u64)entt;
  PhysicsBody* body = physics_world_create_and_add_body(desc);

  world.emplace<PhysicsComponent>(entt, body, coll_func);
}

void entity_add_character_body(EntityWorld& world, 
                               EntityID& entt, 
                               CharacterBodyDesc& desc, 
                               const OnCollisionEnterFn& coll_func) {
  Transform& transform = world.get<Transform>(entt);

  desc.position  = transform.position;
  desc.rotation  = transform.rotation;
  desc.user_data = (u64)entt;

  Character* character = character_body_create(desc);
  physics_world_add_character(character);

  world.emplace<CharacterComponent>(entt, character, coll_func);
}

void entity_add_audio_source(EntityWorld& world, 
                             EntityID& entt, 
                             AudioSourceDesc& desc, 
                             const ResourceID& audio_buffer_id) {
  Transform& transform = world.get<Transform>(entt);

  desc.position      = transform.position; 
  desc.buffers[0]    = resources_get_audio_buffer(audio_buffer_id);
  desc.buffers_count = 1;

  world.emplace<AudioSourceID>(entt, audio_source_create(desc));
}

void entity_add_timer(EntityWorld& world, 
                      EntityID& entt, 
                      const f32 max_time, 
                      const bool one_shot, 
                      const bool active) {
  Timer timer; 
  timer_create(&timer, max_time, one_shot, active);

  world.emplace<Timer>(entt, timer);
}

void entity_add_particle_emitter(EntityWorld& world, EntityID& entt, ParticleEmitterDesc& desc) {
  Transform& transform = world.get<Transform>(entt);
  desc.position        = transform.position;

  ParticleEmitter emitter; 
  particle_emitter_create(&emitter, desc);

  world.emplace<ParticleEmitter>(entt, emitter);
}

void entity_add_animation_sampler(EntityWorld& world, 
                                  EntityID& entt, 
                                  const ResourceID& skeleton_id, 
                                  const ResourceID& animation_id) {
  AnimationSampler* sampler = animation_sampler_create(skeleton_id, animation_id); 
  world.emplace<AnimationSampler*>(entt, sampler);
}

void entity_add_animation_sampler(EntityWorld& world, 
                                  EntityID& entt, 
                                  const ResourceID& skeleton_id, 
                                  const ResourceID* animations, 
                                  const sizei animations_count) {
  AnimationSampler* sampler = animation_sampler_create(skeleton_id, animations, animations_count); 
  world.emplace<AnimationSampler*>(entt, sampler);
}

void entity_add_animation_blender(EntityWorld& world, EntityID& entt, const ResourceID& skeleton_id) {
  AnimationBlender* blender = animation_blender_create(skeleton_id);
  world.emplace<AnimationBlender*>(entt, blender);
}

void entity_add_renderable(EntityWorld& world, 
                           EntityID& entt, 
                           const EntityRenderableType renderable_type, 
                           const ResourceID& renderable_id, 
                           const ResourceID& material_id) {
  world.emplace<RenderableComponent>(entt, renderable_type, renderable_id, material_id);
}

/// EntityID functions
/// ----------------------------------------------------------------------

/// *** Entity ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
