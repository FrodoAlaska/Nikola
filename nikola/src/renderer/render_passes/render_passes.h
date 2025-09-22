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

void shadow_pass_prepare(RenderPass* pass, const FrameData& data);

void shadow_pass_sumbit(RenderPass* pass, const DynamicArray<GeometryPrimitive>& queue);

Mat4 shadow_pass_get_light_space(RenderPass* pass);

/// Shadow pass functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Light pass functions

void light_pass_init(Window* window);

void light_pass_prepare(RenderPass* pass, const FrameData& data);

void light_pass_sumbit(RenderPass* pass, const DynamicArray<GeometryPrimitive>& queue);

/// Light pass functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Billboard pass functions

void billboard_pass_init(Window* window);

void billboard_pass_prepare(RenderPass* pass, const FrameData& data);

void billboard_pass_sumbit(RenderPass* pass, const DynamicArray<GeometryPrimitive>& queue);

/// Billboard pass functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// HDR pass functions

void hdr_pass_init(Window* window);

void hdr_pass_prepare(RenderPass* pass, const FrameData& data);

void hdr_pass_sumbit(RenderPass* pass, const DynamicArray<GeometryPrimitive>& queue);

/// HDR pass functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Debug pass functions

void debug_pass_init(Window* window);

void debug_pass_prepare(RenderPass* pass, const FrameData& data);

void debug_pass_sumbit(RenderPass* pass, const DynamicArray<GeometryPrimitive>& queue);

/// Debug pass functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////

