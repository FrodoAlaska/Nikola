#include "nikola/nikola_render.h"
#include "nikola/nikola_timer.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola 

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void apply_normal_distribution(ParticleEmitter& emitter) {
  for(sizei i = 0; i < emitter.particles_count; i++) {
    Vec3 direction         = Vec3(random_f32(-emitter.distribution_radius, emitter.distribution_radius), 
                                  random_f32(-emitter.distribution_radius, emitter.distribution_radius),
                                  random_f32(-emitter.distribution_radius, emitter.distribution_radius));
    emitter.velocities[i] *= direction;
  }
}

static void apply_square_distribution(ParticleEmitter& emitter) {
  Vec3 min = (emitter.initial_position - (emitter.distribution_radius / 2.0f));
  Vec3 max = min + emitter.distribution_radius;

  min = vec3_normalize(min);
  max = vec3_normalize(max);

  for(sizei i = 0; i < emitter.particles_count; i++) {
    Vec3 direction        = Vec3(random_f32(min.x, max.x), 
                                 1.0f,
                                 random_f32(min.z, max.z));
    emitter.velocities[i] *= direction;
  }
}

static void apply_cube_distribution(ParticleEmitter& emitter) {
  // @TODO (Particles): Looks more like the random distribution

  Vec3 min = (emitter.initial_position - (emitter.distribution_radius / 2.0f));
  Vec3 max = min + emitter.distribution_radius;

  min = vec3_normalize(min);
  max = vec3_normalize(max);

  for(sizei i = 0; i < emitter.particles_count; i++) {
    Vec3 direction         = Vec3(random_f32(min.x, max.x), 
                                  random_f32(min.y, max.y),
                                  random_f32(min.z, max.z));
    emitter.velocities[i] *= direction;
  }
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ParticleEmitter functions

void particle_emitter_create(ParticleEmitter* out_emitter, const ParticleEmitterDesc& desc) {
  NIKOLA_ASSERT(out_emitter, "Invalid ParticleEmitter given to particle_emitter_create")

  // Setting default values 
  
  out_emitter->initial_position = desc.position;
  out_emitter->initial_velocity = desc.velocity;

  out_emitter->particles_count  = desc.count;
  out_emitter->gravity_factor   = desc.gravity_factor; 

  out_emitter->distribution_radius = desc.distribution_radius; 
  out_emitter->distribution        = desc.distribution;

  for(sizei i = 0; i < desc.count; i++) {
    Transform* transform = &out_emitter->transforms[i];

    transform->position = desc.position; 
    transform->scale    = desc.scale;
    transform_apply(*transform);
  }

  for(sizei i = 0; i < desc.count; i++) {
    out_emitter->forces[i] = Vec3(0.0f);
  }
  
  for(sizei i = 0; i < desc.count; i++) {
    out_emitter->velocities[i] = desc.velocity;
  }

  // Setting render variables 

  out_emitter->mesh_id     = desc.mesh_id; 
  out_emitter->material_id = desc.material_id;

  // Create the timer 
  timer_create(&out_emitter->lifetime, desc.lifetime, false);
}

void particle_emitter_update(ParticleEmitter& emitter, const f64 delta_time) {
  if(!emitter.is_active) {
    return;
  }

  // Apply the numarical integrator for each particle 

  for(sizei i = 0; i < emitter.particles_count; i++) {
    emitter.transforms[i].position += (emitter.velocities[i] + Vec3(0.0f, emitter.gravity_factor, 0.0f)) * (f32)delta_time; 
    transform_translate(emitter.transforms[i], emitter.transforms[i].position);
  }

  // Update the timer 

  timer_update(emitter.lifetime, (f32)delta_time);
  if(!emitter.lifetime.has_runout) {
    return;
  }

  // Bye bye, emitter. Goodnight
  emitter.is_active = false; 
}

void particle_emitter_emit(ParticleEmitter& emitter) {
  if(!emitter.is_active) {
    particle_emitter_reset(emitter);
    emitter.is_active = true;
  }

  // Applying the distribution

  switch(emitter.distribution) {
    case DISTRIBUTION_RANDOM: 
      apply_normal_distribution(emitter);
      break;
    case DISTRIBUTION_SQUARE: 
      apply_square_distribution(emitter);
      break;
    case DISTRIBUTION_CUBE: 
      apply_cube_distribution(emitter);
      break;
    default:
      break;
  }
}

void particle_emitter_reset(ParticleEmitter& emitter) {
  emitter.is_active = false;
  timer_reset(emitter.lifetime);
  
  for(sizei i = 0; i < emitter.particles_count; i++) {
    transform_translate(emitter.transforms[i], emitter.initial_position);
  }
  
  for(sizei i = 0; i < emitter.particles_count; i++) {
    emitter.forces[i] = Vec3(0.0f);
  }
  
  for(sizei i = 0; i < emitter.particles_count; i++) {
    emitter.velocities[i] = emitter.initial_velocity;
  }
}

/// ParticleEmitter functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
