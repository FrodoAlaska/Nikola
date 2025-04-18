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

/// Retrieve the internal default values of the renderer.
NIKOLA_API const RendererDefaults& renderer_get_defaults();

/// Add an additional render pass using the information from `desc`. 
/// Internally, the renderer will call `func`, passing in `user_data`.
NIKOLA_API void renderer_push_pass(const RenderPassDesc& desc, const RenderPassFn& func, const void* user_data);

/// Sumbit the given `queue` to the renderer to be rendered once `renderer_end` is called.
NIKOLA_API void renderer_sumbit_queue(RenderQueue& queue);

/// Clear the screen to the given `clear_color`.
NIKOLA_API void renderer_clear(const Vec4& clear_color);

/// Setup the renderer for any upcoming render operations by 
/// the data given in `data`.
NIKOLA_API void renderer_begin(FrameData& data);

/// Start the render passes chain, flushing the given `RenderQueue` in the process. 
NIKOLA_API void renderer_end();

/// Present the final result to the screen. 
///
/// @NOTE: In OpenGL, this is equivalent to `glSwapBuffers`.
NIKOLA_API void renderer_present();

/// Renderer functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Batch renderer functions

/// Initialize the internal data of the batch renderer.
NIKOLA_API void batch_renderer_init();

/// Shutdown and destroy any resources created by the batch renderer.
NIKOLA_API void batch_renderer_shutdown();

/// Setup the batch renderer for any proceeding render operation.
NIKOLA_API void batch_renderer_begin();

/// Sumbit the results of the batch renderer to the screen.
NIKOLA_API void batch_renderer_end();

/// Render a quad at `position` with a size of `size` and tinted with `color`.
NIKOLA_API void batch_render_quad(const Vec2& position, const Vec2& size, const Vec4& color);

/// Render the given `texture` at `position` with size of `size` and tinted with `tint`.
///
/// @NOTE: By default, `tint` is set to `Vec4(1.0f)`.
NIKOLA_API void batch_render_texture(GfxTexture* texture, const Vec2& position, const Vec2& size, const Vec4& tint = Vec4(1.0f));

/// Batch renderer functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Renderer ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
