#pragma once

#include "nikola_gfx.h"
#include "nikola_resources.h"
#include "nikola_math.h"
#include "nikola_containers.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

// Forward declaration to help with compilation time.
struct Collider;

/// ----------------------------------------------------------------------
/// *** Renderer ***

///---------------------------------------------------------------------------------------------------------------------
/// RenderableType
enum RenderableType {
  RENDERABLE_MESH = 0, 
  RENDERABLE_MODEL, 
  RENDERABLE_DEBUG,

  // @TODO (Renderer): Add more renderable types...
};
/// RenderableType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Camera consts 

/// The maximum degrees the camera can achieve 
const f32 CAMERA_MAX_DEGREES = 89.0f;

/// The maximum amount of zoom the camera can achieve
const f32 CAMERA_MAX_ZOOM    = 180.0f;

/// Camera consts 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Callbacks

// Have to do this to fix underfined variable errors in the callback.
struct Camera;
struct RenderPass;

/// A function callback to move the camera every frame.
using CameraMoveFn = void(*)(Camera& camera);

/// A function to be called every render pass, passing in the `previous` and 
/// `current` render passes, and some `user_data`.
using RenderPassFn = void(*)(const RenderPass* previous, RenderPass* current, void* user_data);

/// Callbacks
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// CameraDesc
struct CameraDesc {
  /// The initial position of the camera
  Vec3 position    = Vec3(0.0f); 

  /// The forward looking target of the camera. 
  ///
  /// @NOTE: This is set to `Vec3(-3.0f, 0.0f, 0.0f)` by default.
  Vec3 target      = Vec3(-3.0f, 0.0f, 0.0f); 
  
  /// The up vector of the camera. 
  ///
  /// @NOTE: This is set to `Vec3(0.0f, 1.0f, 0.0f)` by default.
  Vec3 up_axis     = Vec3(0.0f, 1.0f, 0.0f);

  /// The aspect ratio of the created camera. 
  /// This can be a completely different value from the 
  /// default window aspect ratio if desired.
  ///
  /// @NOTE: This is set to `0.0f` by default.
  f32 aspect_ratio = 0.0f;

  /// The near distance of the camera. 
  ///
  /// @NOTE: This is set to `0.1f` by default.
  f32 near         = 0.1f; 

  /// The far distance of the camera. 
  ///
  /// @NOTE: This is set to `100.0f` by default.
  f32 far          = 100.0f;

  /// The function callback to move the camera 
  ///
  /// @NOTE: See `CameraMoveFn` for more details.
  CameraMoveFn move_func;
};
/// CameraDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Camera 
struct Camera {
  f32 yaw, pitch;
  f32 zoom, aspect_ratio;

  f32 near        = 0.1f; 
  f32 far         = 100.0f;
  f32 sensitivity = 0.1f;
  f32 exposure    = 1.0f; 

  Vec3 position;
  
  Vec3 up, direction, front;
  Mat4 view, projection, view_projection;

  CameraMoveFn move_fn;
  bool is_active;
};
/// Camera 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RendererDefaults 
struct RendererDefaults {
  GfxTexture* texture        = nullptr;
  GfxBuffer* matrices_buffer = nullptr;
  
  Material* material = nullptr;
  Mesh* cube_mesh    = nullptr;
};
/// RendererDefaults 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderCommand
struct RenderCommand {
  RenderableType type;
  Transform transform;

  ResourceID renderable_id = {};
  ResourceID material_id   = {};
};
/// RenderCommand
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderPassDesc
struct RenderPassDesc {
  Vec2 frame_size = Vec2(0.0f);
  u32 clear_flags = 0;
  
  ResourceID shader_context_id = {};
  DynamicArray<GfxTextureFormat> targets;

  void* user_data = nullptr;
};
/// RenderPassDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderPass
struct RenderPass {
  Vec2 frame_size = Vec2(0.0f);
  
  GfxFramebufferDesc frame_desc = {};
  GfxFramebuffer* frame         = nullptr;
  ShaderContext* shader_context = nullptr;

  bool is_active         = true;
  void* user_data        = nullptr;
  RenderPassFn pass_func = nullptr;
};
/// RenderPass
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// DirectionalLight 
struct DirectionalLight {
  Vec3 direction = Vec3(1.0f); 
  Vec3 color     = Vec3(1.0f);
};
/// DirectionalLight 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PointLight
struct PointLight {
  Vec3 position = Vec3(0.0f); 
  Vec3 color    = Vec3(1.0f);

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
  
  Vec3 ambient = Vec3(0.125f);

  DirectionalLight dir_light; 
  DynamicArray<PointLight> point_lights;
};
/// FrameData
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Rect
struct Rect {
  Vec2 size; 
  Vec2 position;
};
/// Rect
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Camera functions

/// A function to mimick a free-form camera movement.
NIKOLA_API void camera_free_move_func(Camera& camera);

/// A function to mimick a first-person shooter camera movement.
NIKOLA_API void camera_fps_move_func(Camera& camera);

/// Fill the information in `cam` using the given `CameraDesc`.
NIKOLA_API void camera_create(Camera* cam, const CameraDesc& desc);

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

/// Set renderer's clear color to the given `clear_color`.
NIKOLA_API void renderer_set_clear_color(const Vec4& clear_color);

/// Return the renderer's current clear color.
NIKOLA_API Vec4& renderer_get_clear_color();

/// Add an additional render pass using the information from `desc`, returning back an
/// identifying ID for later use.
///
/// Internally, the renderer will call `func`, passing in `user_data`.
NIKOLA_API const u32 renderer_push_pass(const RenderPassDesc& desc, const RenderPassFn& func);

/// Queue a rendering command using the given `command` as a specifier. 
NIKOLA_API void renderer_queue_command(const RenderCommand& command);

/// Flush/render the internal command queue of the renderer, using the given 
/// `shader_context_id` as the main shader for rendering. 
///
/// @NOTE: Internally, the renderer will call this function inside many of 
/// its render passes. However, it can also be called explicitly by the client code 
/// for any specific reason. The internal queue will not be discarded until the end of the frame.
///
/// @NOTE: The `shader_context_id` can be left untouched to let the renderer 
/// handle the shading of the objects.
NIKOLA_API void renderer_flush_queue_command(const ResourceID& shader_context_id = {});

/// Setup the renderer for any upcoming render operations by 
/// the data given in `data`.
NIKOLA_API void renderer_begin(FrameData& data);

/// Start the render passes chain, flushing the given `RenderQueue` in the process. 
NIKOLA_API void renderer_end();

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

/// Source the given `texture` at `src` and render into `dest`, tinted with `tint`.
///
/// @NOTE: By default, `tint` is set to `Vec4(1.0f)`.
NIKOLA_API void batch_render_texture(GfxTexture* texture, const Rect& src, const Rect& dest, const Vec4& tint = Vec4(1.0f));

/// Render the given `texture` at `position` with size of `size` and tinted with `tint`.
///
/// @NOTE: By default, `tint` is set to `Vec4(1.0f)`.
NIKOLA_API void batch_render_texture(GfxTexture* texture, const Vec2& position, const Vec2& size, const Vec4& tint = Vec4(1.0f));

/// Render a quad at `position` with a size of `size` and tinted with `color`.
NIKOLA_API void batch_render_quad(const Vec2& position, const Vec2& size, const Vec4& color);

/// Render a 2D circle at `center` with a radius of `radius`, colored as `color`.
NIKOLA_API void batch_render_circle(const Vec2& center, const f32 radius, const Vec4& color);

/// Render a 2D polygon at `center` with `sides` amount of sides and with a radius of `radius`, colored as `color`.
NIKOLA_API void batch_render_polygon(const Vec2& center, const f32 radius, const u32 sides, const Vec4& color);

/// Using the given `font`, render `text` on the screen at `position` with `size` font size and colored as `color`.
NIKOLA_API void batch_render_text(Font* font, const String& text, const Vec2& position, const f32 size, const Vec4& color);

/// Using the given `font`, render `codepoint` on the screen at `position` with `size` font size and colored as `color`.
NIKOLA_API void batch_render_codepoint(Font* font, const char codepoint, const Vec2& position, const f32 size, const Vec4& color);

/// Using the given `font`, render a text representation of the FPS counter on the screen at `position` with `size` font size and colored as `color`.
NIKOLA_API void batch_render_fps(Font* font, const Vec2& position, const f32 size, const Vec4& color);

/// Batch renderer functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Renderer ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
