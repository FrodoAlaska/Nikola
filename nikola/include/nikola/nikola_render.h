#pragma once

#include "nikola_base.h"
#include "nikola_gfx.h"
#include "nikola_resources.h"
#include "nikola_math.h"
#include "nikola_containers.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Renderer ***

///---------------------------------------------------------------------------------------------------------------------
/// Camera consts 

/// The maximum degrees the camera can achieve 
const f32 CAMERA_MAX_DEGREES = 89.0f;

/// The maximum amount of zoom the camera can achieve
const f32 CAMERA_MAX_ZOOM    = 180.0f;

/// Camera consts 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderableType 
enum RenderableType {
  /// Will commence a mesh rendering operation
  RENDERABLE_TYPE_MESH   = 19 << 0,
  
  /// Will commence a model rendering operation
  RENDERABLE_TYPE_MODEL  = 19 << 1,
  
  /// Will commence a skybox rendering operation
  RENDERABLE_TYPE_SKYBOX = 19 << 2,
};
/// RenderableType 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Camera function pointers

// Have to do this to fix underfined variable errors in the callback.
struct Camera;

/// A function callback to move the camera every frame.
using CameraMoveFn = void(*)(Camera& camera);

/// Camera function pointers
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Camera 
struct Camera {
  f32 yaw, pitch;
  f32 zoom, aspect_ratio;

  f32 near        = 0.1f; 
  f32 far         = 100.0f;
  f32 sensitivity = 0.1f;

  Vec3 position, up, direction, front;
  Mat4 view, projection, view_projection;

  CameraMoveFn move_fn;
};
/// Camera 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RendererDefaults 
struct RendererDefaults {
  ResourceID texture         = {};
  ResourceID matrices_buffer = {};
};
/// RendererDefaults 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderCommand
struct RenderCommand {
  Transform transform;
  
  RenderableType render_type;
  sizei instance_count = 0;

  ResourceID renderable_id;
  ResourceID material_id;
};
/// RenderCommand
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderQueue
using RenderQueue = DynamicArray<RenderCommand>;
/// RenderQueue
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderTarget 
struct RenderTarget {
  GfxTextureType type      = GFX_TEXTURE_RENDER_TARGET;
  GfxTextureFormat format  = GFX_TEXTURE_FORMAT_RGBA8;
  GfxTextureFilter filter  = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST;
  GfxTextureWrap wrap_mode = GFX_TEXTURE_WRAP_MIRROR;
};
/// RenderTarget 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderPassDesc
struct RenderPassDesc {
  Vec2 frame_size  = Vec2(0.0f);
  Vec4 clear_color = Vec4(1.0f);
  u32 clear_flags  = 0;
  
  ResourceID shader_context_id = {};
  DynamicArray<RenderTarget> targets;
};
/// RenderPassDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderPass
struct RenderPass {
  Vec2 frame_size  = Vec2(0.0f);
  Vec4 clear_color = Vec4(1.0f);
  
  GfxFramebufferDesc frame_desc = {};
  GfxFramebuffer* frame         = nullptr;
  ResourceID shader_context_id  = {};
};
/// RenderPass
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// DirectionalLight 
struct DirectionalLight {
  Vec3 direction = Vec3(1.0f); 

  Vec3 ambient  = Vec3(1.0f);
  Vec3 diffuse  = Vec3(1.0f);
  Vec3 specular = Vec3(1.0f);
};
/// DirectionalLight 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PointLight
struct PointLight {
  Vec3 position = Vec3(0.0f); 

  Vec3 ambient  = Vec3(1.0f);
  Vec3 diffuse  = Vec3(1.0f);
  Vec3 specular = Vec3(1.0f);

  f32 linear    = 0.09f; 
  f32 quadratic = 0.032f;
};
/// PointLight
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FrameData
struct FrameData {
  Camera camera; 
  ResourceID skybox_id;

  DirectionalLight dir_light; 
  DynamicArray<PointLight> point_lights;
};
/// FrameData
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderPassFn 
using RenderPassFn = void(*)(const RenderPass* previous, RenderPass* current, void* user_data);
/// RenderPassFn 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Camera functions

/// The default function callback to use in order to move `camera`.
NIKOLA_API void camera_default_move_func(Camera& camera);

/// Fill the information in `cam` using the given values.
NIKOLA_API void camera_create(Camera* cam, const f32 aspect_ratio, const Vec3& pos, const Vec3& target, const CameraMoveFn& move_fn = camera_default_move_func);

/// Update the internal matrices of `cam` and call the associated `CameraMoveFn`. 
NIKOLA_API void camera_update(Camera& cam);

/// Camera functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Renderer functions

/// Initialize the global renderer using the given `window` for dimensions.
NIKOLA_API void renderer_init(Window* window);

/// Free/reclaim any memory consumed by the global renderer
NIKOLA_API void renderer_shutdown();

/// Retrieve the internal `GfxContext` of the global renderer.
NIKOLA_API GfxContext* renderer_get_context();

/// Retrieve the internal default values of the renderer
NIKOLA_API const RendererDefaults& renderer_get_defaults();

NIKOLA_API void renderer_push_pass(const RenderPassDesc& desc, const RenderPassFn& func, const void* user_data);

NIKOLA_API void renderer_sumbit_queue(RenderQueue& queue);

NIKOLA_API void renderer_clear(const Vec4& clear_color);

NIKOLA_API void renderer_begin(FrameData& data);

NIKOLA_API void renderer_end();

NIKOLA_API void renderer_present();

/// Renderer functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Batch renderer functions

NIKOLA_API void batch_renderer_init();

NIKOLA_API void batch_renderer_shutdown();

NIKOLA_API void batch_renderer_begin();

NIKOLA_API void batch_renderer_end();

NIKOLA_API void batch_render_quad(const Vec2& position, const Vec2& size, const Vec4& color);

NIKOLA_API void batch_render_texture(GfxTexture* texture, const Vec2& position, const Vec2& size, const Vec4& tint = Vec4(1.0f));

/// Batch renderer functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Renderer ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
