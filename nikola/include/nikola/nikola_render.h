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
/// RenderEffectType
enum RenderEffectType {
  RENDER_EFFECT_NONE        = 20 << 0, 
 
  RENDER_EFFECT_GREYSCALE   = 20 << 1, 
 
  RENDER_EFFECT_INVERSION   = 20 << 2, 
 
  RENDER_EFFECT_SHARPEN     = 20 << 3, 
 
  RENDER_EFFECT_BLUR        = 20 << 4, 
  
  RENDER_EFFECT_EMBOSS      = 20 << 5,

  RENDER_EFFECT_EDGE_DETECT = 20 << 6, 
  
  RENDER_EFFECT_PIXELIZE    = 20 << 7, 

  RENDER_EFFECTS_MAX        = 8,
};
/// RenderEffectType
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
  GfxTexture* texture        = nullptr;
  GfxBuffer* matrices_buffer = nullptr;
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
  ResourceID material_id, shader_context_id;
};
/// RenderCommand
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderQueue
using RenderQueue = DynamicArray<RenderCommand>;
/// RenderQueue
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderPass
struct RenderPass {
  Vec2 frame_size = Vec2(0.0f);

  GfxFramebufferDesc frame_desc = {};
  GfxFramebuffer* frame         = nullptr;

  ResourceID shader_context_id = {};
};
/// RenderPass
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
/// RenderQueue functions

NIKOLA_API void render_queue_flush(RenderQueue& queue);

NIKOLA_API void render_queue_push(RenderQueue& queue, const RenderCommand& cmd);

/// RenderQueue functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderPass functions

NIKOLA_API void render_pass_create(RenderPass* pass, const Vec2& size, u32 clear_flags, const DynamicArray<GfxTextureDesc>& targets);

NIKOLA_API void render_pass_destroy(RenderPass& pass);

NIKOLA_API void render_pass_begin(RenderPass& pass, const ResourceID& shader_context_id);

NIKOLA_API void render_pass_end(RenderPass& pass);

/// RenderPass functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Renderer functions

/// Initialize the global renderer using the given `window` for dimensions 
/// and `clear_color` as the default background color.
NIKOLA_API void renderer_init(Window* window, const Vec4& clear_color);

/// Free/reclaim any memory consumed by the global renderer
NIKOLA_API void renderer_shutdown();

/// Retrieve the internal `GfxContext` of the global renderer.
NIKOLA_API const GfxContext* renderer_get_context();

/// Set the background color of the global renderer to `clear_color`
NIKOLA_API void renderer_set_clear_color(const Vec4& clear_color);

/// Retrieve the internal default values of the renderer
NIKOLA_API const RendererDefaults& renderer_get_defaults();

NIKOLA_API void renderer_begin(Camera& camera);

NIKOLA_API void renderer_end();

NIKOLA_API void renderer_apply_pass(RenderPass& pass);

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
