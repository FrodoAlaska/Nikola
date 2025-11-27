#pragma once 

#include "nikola/nikola_render.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// DirectionalLightInterface
struct DirectionalLightInterface {
  Vec3 direction;
  f32 __padding1;

  Vec3 color;
  f32 __padding2;
};
/// DirectionalLightInterface
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PointLightInterface
struct PointLightInterface {
  Vec3 position;
  f32 __padding1;

  Vec3 color;
  f32 __padding2;

  f32 radius;
  f32 fall_off;
  Vec2 __padding3;
};
/// PointLightInterface
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// SpotLightInterface
struct SpotLightInterface {
  Vec3 position;
  f32 __padding1;

  Vec3 direction;
  f32 __padding2;

  Vec3 color;
  f32 __padding3;

  f32 radius;
  f32 outer_radius;
  Vec2 _padding4;
};
/// SpotLightInterface
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// LightBuffer
struct LightBuffer {
  DirectionalLightInterface dir_light; 
  PointLightInterface point_lights[POINT_LIGHTS_MAX]; 
  SpotLightInterface spot_lights[POINT_LIGHTS_MAX];

  Vec3 ambient_color;
  int point_lights_count, spot_lights_count;
};
/// LightBuffer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Shadow pass functions

void shadow_pass_init(Window* window);

void shadow_pass_destroy(RenderPass* pass);

void shadow_pass_prepare(RenderPass* pass, const FrameData& data);

void shadow_pass_sumbit(RenderPass* pass, const RenderQueueEntry& queue);

Mat4 shadow_pass_get_light_space(RenderPass* pass);

/// Shadow pass functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Light pass functions

void light_pass_init(Window* window);

void light_pass_destroy(RenderPass* pass);

void light_pass_prepare(RenderPass* pass, const FrameData& data);

void light_pass_sumbit(RenderPass* pass, const RenderQueueEntry& queue);

void light_pass_on_resize(RenderPass* pass, const IVec2& new_size);

/// Light pass functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Particle pass functions

void particle_pass_init(Window* window);

void particle_pass_sumbit(RenderPass* pass, const RenderQueueEntry& queue);

void particle_pass_on_resize(RenderPass* pass, const IVec2& new_size);

/// Particle pass functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// HDR pass functions

void hdr_pass_init(Window* window);

void hdr_pass_destroy(RenderPass* pass);

void hdr_pass_prepare(RenderPass* pass, const FrameData& data);

void hdr_pass_sumbit(RenderPass* pass, const RenderQueueEntry& queue);

void hdr_pass_on_resize(RenderPass* pass, const IVec2& new_size);

/// HDR pass functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Debug pass functions

void debug_pass_init(Window* window);

void debug_pass_sumbit(RenderPass* pass, const RenderQueueEntry& queue);

void debug_pass_on_resize(RenderPass* pass, const IVec2& new_size);

/// Debug pass functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////

