#include "nikola/nikola_render.h"
#include "nikola/nikola_timer.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola 

///---------------------------------------------------------------------------------------------------------------------
/// Consts

const sizei PARTICLE_EMITTERS_MAX = 16;

/// Consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ParticleEmitter 
struct ParticleEmitter {
  Transform transforms[PARTICLES_MAX];
  Vec3 forces[PARTICLES_MAX];
  Vec3 velocities[PARTICLES_MAX];

  Timer lifetime;
  sizei particles_count = 0;
  float gravity_factor  = 0.0f;

  bool is_active = false;
};
/// ParticleEmitter 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ParticleManager
struct ParticleManager {
  ParticleEmitter emitters[PARTICLE_EMITTERS_MAX];
  sizei active_emitters = 0; 

  ResourceID shape_id, material_id;
  Material* material;
};

static ParticleManager s_manager;
/// ParticleManager
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void apply_normal_distribution(ParticleEmitter* emitter, const ParticleEmitterDesc& desc) {
  for(sizei i = 0; i < emitter->particles_count; i++) {
    Vec3 direction     = Vec3(random_f32(-desc.distribution_radius, desc.distribution_radius), 
                              random_f32(-desc.distribution_radius, desc.distribution_radius),
                              random_f32(-desc.distribution_radius, desc.distribution_radius));
    emitter->forces[i] = desc.velocity * direction;

    transform_translate(emitter->transforms[i], desc.position);
    transform_scale(emitter->transforms[i], desc.scale);
  }
}

static void apply_square_distribution(ParticleEmitter* emitter, const ParticleEmitterDesc& desc) {
  // Getting the top-left corner of the square
  Vec3 min = (desc.position - (desc.distribution_radius / 2.0f));
  Vec3 max = min + desc.distribution_radius;

  for(sizei i = 0; i < emitter->particles_count; i++) {
    Vec3 direction     = Vec3(random_f32(min.x, max.x), 
                              1.0f,
                              random_f32(min.z, max.z));
    emitter->forces[i] = desc.velocity * direction;

    transform_translate(emitter->transforms[i], desc.position);
    transform_scale(emitter->transforms[i], desc.scale);
  }
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Particles functions

void particles_init() {
  // Particles resources init
  
  s_manager.shape_id    = resources_push_mesh(RESOURCE_CACHE_ID, GEOMETRY_BILLBOARD);
  s_manager.material_id = resources_push_material(RESOURCE_CACHE_ID, MaterialDesc{});
  
  s_manager.material = resources_get_material(s_manager.material_id);

  // Default values for each emitter

  for(sizei i = 0; i < PARTICLE_EMITTERS_MAX; i++) {
    ParticleEmitter* emitter = &s_manager.emitters[i];

    for(sizei j = 0; j < PARTICLES_MAX; j++) {
      transform_translate(emitter->transforms[j], Vec3(-1000.0f));
    }

    for(sizei j = 0; j < PARTICLES_MAX; j++) {
      emitter->forces[j] = Vec3(0.0f);
    }

    for(sizei j = 0; j < PARTICLES_MAX; j++) {
      emitter->velocities[j] = Vec3(0.0f);
    }
     
    timer_create(&emitter->lifetime, 2.5f, false);
  }
}

void particles_update(const f64 delta_time) {
  for(sizei i = 0; i < PARTICLE_EMITTERS_MAX; i++) {
    ParticleEmitter* emitter = &s_manager.emitters[i];
    if(!emitter->is_active) {
      continue;
    }

    // Apply the numarical integrator 
  
    for(sizei j = 0; j < emitter->particles_count; j++) {
      Vec3 acceleration = emitter->forces[j] * -1.0f; // -1.0f = inverse mass... for now
      acceleration.y   += emitter->gravity_factor;

      emitter->velocities[j]          += acceleration * (f32)delta_time;
      emitter->transforms[j].position += emitter->velocities[j] * (f32)delta_time;

      transform_translate(emitter->transforms[j], emitter->transforms[j].position);

      emitter->forces[j] = Vec3(0.0f);
    }

    // Send out a render command for the emitter 
    // @TEMP (Particles)

    renderer_queue_command_instanced(RENDERABLE_BILLBOARD,  
                                     s_manager.shape_id, 
                                     emitter->transforms, 
                                     emitter->particles_count, 
                                     s_manager.material_id);

    // Manage lifetimes
  
    timer_update(emitter->lifetime);
    if(!emitter->lifetime.has_runout) {
      continue;
    }

    // Emitter has runout. Deactivated.
     
    emitter->is_active = false;
    s_manager.active_emitters--;

    for(sizei j = 0; j < emitter->particles_count; j++) {
      emitter->velocities[j]          = Vec3(0.0f);
      emitter->transforms[j].position = Vec3(0.0f);
    }
  }

  // Just some safety procedures 
  s_manager.active_emitters = clamp_int(s_manager.active_emitters, 0, PARTICLE_EMITTERS_MAX - 1);
}

void particles_emit(const ParticleEmitterDesc& desc) {
  NIKOLA_ASSERT((desc.count >= 0 && desc.count < PARTICLES_MAX), "Out-of-bounds particles count");

  // Restrict the particles from over... uh... particling?
  // Just wait for the next frame.
  if((s_manager.active_emitters + 1) >= (PARTICLE_EMITTERS_MAX - 1)) {
    return;
  }

  // Retrieve any inactive emitter from the array

  ParticleEmitter* emitter = nullptr;
  for(sizei i = 0; i < PARTICLE_EMITTERS_MAX; i++) {
    if(s_manager.emitters[i].is_active) {
      continue;
    }

    emitter            = &s_manager.emitters[i];
    emitter->is_active = true;
    s_manager.active_emitters++;

    break;
  }

  // Applying the settings to the selected emitter

  s_manager.material->color        = Vec3(desc.color);
  s_manager.material->transparency = desc.color.a;

  emitter->particles_count = desc.count;
  emitter->lifetime.limit  = desc.lifetime;
  emitter->gravity_factor  = desc.gravity_factor;

  switch(desc.distribution) {
    case DISTRIBUTION_RANDOM: 
      apply_normal_distribution(emitter, desc);
      break;
    case DISTRIBUTION_SQUARE: 
      apply_square_distribution(emitter, desc);
      break;
    default:
      break;
  }
}

/// Particles functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
