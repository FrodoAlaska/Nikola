#include "nikola/nikola_entity.h"
#include "nikola/nikola_event.h"
#include "nikola/nikola_ui.h"

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

  if(world.any_of<UIContext*>(entt)) {
    UIContext* ui_ctx = world.get<UIContext*>(entt);
    ui_context_destroy(ui_ctx);
  }

  // Destroy the entity in the world
  world.destroy(entt); 
}

void entity_world_update(EntityWorld& world, const f64 delta_time) {
  NIKOLA_PROFILE_FUNCTION();

  // Physics bodies
  {
    NIKOLA_PROFILE_FUNCTION_NAMED("entity_world_update(PhysicsComponent)");

    auto physics_view = world.view<PhysicsComponent, Transform>();
    for(auto entt : physics_view) {
      PhysicsComponent& physics_comp = physics_view.get<PhysicsComponent>(entt); 
      PhysicsBodyType body_type      = physics_body_get_type(physics_comp.body);
      
      if(body_type == PHYSICS_BODY_STATIC) { // No need to update the transforms of static bodies.
        continue;
      }

      Transform& transform = physics_view.get<Transform>(entt); 

      transform.position = physics_body_get_position(physics_comp.body);
      transform.rotation = physics_body_get_rotation(physics_comp.body);
      transform_apply(transform);
    }
  }

  // Characters
  {
    NIKOLA_PROFILE_FUNCTION_NAMED("entity_world_update(CharacterComponent)");

    auto chars_view = world.view<CharacterComponent, Transform>();
    for(auto entt : chars_view) {
      Transform& transform  = chars_view.get<Transform>(entt); 
      CharacterComponent& char_comp = chars_view.get<CharacterComponent>(entt); 
      character_body_update(char_comp.character);

      transform.position = character_body_get_position(char_comp.character);
      transform.rotation = character_body_get_rotation(char_comp.character);
      transform_apply(transform);
    }
  }

  // Animation samplers
  {
    NIKOLA_PROFILE_FUNCTION_NAMED("entity_world_update(AnimationSampler)");

    auto anim_view = world.view<AnimationSampler*>();
    for(auto entt : anim_view) {
      AnimationSampler* sampler = anim_view.get<AnimationSampler*>(entt);
      animation_sampler_update(sampler, (f32)delta_time);
    }
  }

  // Animation blenders
  {
    NIKOLA_PROFILE_FUNCTION_NAMED("entity_world_update(AnimationBlender)");

    auto anim_view = world.view<AnimationBlender*>();
    for(auto entt : anim_view) {
      AnimationBlender* blender = anim_view.get<AnimationBlender*>(entt);
      animation_blender_update(blender, (f32)delta_time);
    }
  }

  // Timers
  {
    NIKOLA_PROFILE_FUNCTION_NAMED("entity_world_update(Timer)");

    auto timer_view = world.view<Timer>();
    for(auto entt : timer_view) {
      Timer& timer = timer_view.get<Timer>(entt);
      timer_update(timer, (f32)delta_time);
    }
  }

  // Particles 
  {
    NIKOLA_PROFILE_FUNCTION_NAMED("entity_world_update(ParticleEmitter)");

    auto view = world.view<ParticleEmitter>();
    for(auto entt : view) {
      ParticleEmitter& emitter = view.get<ParticleEmitter>(entt);
      particle_emitter_update(emitter, (f32)delta_time); 
    }
  }

}

void entity_world_render(const EntityWorld& world) {
  NIKOLA_PROFILE_FUNCTION();

  // Renderables
  {
    NIKOLA_PROFILE_FUNCTION_NAMED("entity_world_render(RenderableComponent)");

    auto view = world.view<RenderableComponent, Transform>();
    for(auto entt : view) {
      const Transform& transform            = view.get<Transform>(entt);
      const RenderableComponent& renderable = view.get<RenderableComponent>(entt);

      switch(renderable.type) {
        case ENTITY_RENDERABLE_MESH:
          renderer_queue_mesh(renderable.renderable_id, transform, renderable.material_id);
          break;
        case ENTITY_RENDERABLE_MODEL:
          renderer_queue_model(renderable.renderable_id, transform, renderable.material_id);
          break;
        case ENTITY_RENDERABLE_DEBUG_CUBE:
          renderer_queue_debug_cube(transform, renderable.material_id);
          break;
        case ENTITY_RENDERABLE_DEBUG_SPHERE:
          renderer_queue_debug_sphere(transform, renderable.material_id);
          break;
      }
    }
  }
  
  // Instanced renderables
  {
    NIKOLA_PROFILE_FUNCTION_NAMED("entity_world_render(InstancedRenderableComponent)");

    auto view = world.view<InstancedRenderableComponent, Transform>();
    for(auto entt : view) {
      const Transform& transform                     = view.get<Transform>(entt);
      const InstancedRenderableComponent& renderable = view.get<InstancedRenderableComponent>(entt);

      switch(renderable.type) {
        case ENTITY_RENDERABLE_MESH:
          renderer_queue_mesh_instanced(renderable.renderable_id, renderable.transforms.data(), renderable.transforms.size(), renderable.material_id);
          break;
        case ENTITY_RENDERABLE_MODEL:
          renderer_queue_model_instanced(renderable.renderable_id, renderable.transforms.data(), renderable.transforms.size(), renderable.material_id);
          break;
        case ENTITY_RENDERABLE_DEBUG_CUBE:
          renderer_queue_debug_cube_instanced(renderable.transforms.data(), renderable.transforms.size(), renderable.material_id);
          break;
        case ENTITY_RENDERABLE_DEBUG_SPHERE:
          renderer_queue_debug_sphere_instanced(renderable.transforms.data(), renderable.transforms.size(), renderable.material_id);
          break;
      }
    }
  }

  // Animation samplers
  {
    NIKOLA_PROFILE_FUNCTION_NAMED("entity_world_render(AnimationSampler)");

    auto view = world.view<AnimationSampler*, Transform>();
    for(auto entt : view) {
      const Transform& transform            = view.get<Transform>(entt);
      const AnimationSampler* sampler       = view.get<AnimationSampler*>(entt);
      const RenderableComponent& renderable = world.get<RenderableComponent>(entt);

      renderer_queue_animation(renderable.renderable_id, transform, sampler, renderable.material_id);
    }
  }

  // Animation blenders
  {
    NIKOLA_PROFILE_FUNCTION_NAMED("entity_world_render(AnimationBlender)");

    auto view = world.view<AnimationBlender*, Transform>();
    for(auto entt : view) {
      const Transform& transform            = view.get<Transform>(entt);
      const AnimationBlender* blender       = view.get<AnimationBlender*>(entt);
      const RenderableComponent& renderable = world.get<RenderableComponent>(entt);

      renderer_queue_animation(renderable.renderable_id, transform, blender, renderable.material_id);
    }
  }
  
  // Particles
  {
    NIKOLA_PROFILE_FUNCTION_NAMED("entity_world_render(ParticleEmitter)");

    auto view = world.view<ParticleEmitter, Transform>();
    for(auto entt : view) {
      const Transform& transform     = view.get<Transform>(entt);
      const ParticleEmitter& emitter = view.get<ParticleEmitter>(entt);

      renderer_queue_particles(emitter);
    }
  }
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

void entity_add_ui_context(EntityWorld& world, EntityID& entt, const String& name, const IVec2& bounds) {
  world.emplace<UIContext*>(entt, ui_context_create(name, bounds));
}

void entity_add_renderable(EntityWorld& world, 
                           EntityID& entt, 
                           const EntityRenderableType renderable_type, 
                           const ResourceID& renderable_id, 
                           const ResourceID& material_id) {
  world.emplace<RenderableComponent>(entt, renderable_type, renderable_id, material_id);
}

void entity_add_instanced_renderable(EntityWorld& world, 
                                     EntityID& entt, 
                                     const EntityRenderableType renderable_type, 
                                     const DynamicArray<Transform>& transforms,
                                     const ResourceID& renderable_id, 
                                     const ResourceID& material_id) {
  world.emplace<InstancedRenderableComponent>(entt, renderable_type, renderable_id, material_id, transforms);
}

/// EntityID functions
/// ----------------------------------------------------------------------

/// *** Entity ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
