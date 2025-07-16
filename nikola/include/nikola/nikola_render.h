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
/// Consts

/// The maximum amount of instances a renderer can dispatch.
const sizei RENDERER_MAX_INSTANCES = 2048;

/// The maximum degrees the camera can achieve. 
const f32 CAMERA_MAX_DEGREES       = 89.0f;

/// The maximum amount of zoom the camera can achieve.
const f32 CAMERA_MAX_ZOOM          = 180.0f;

/// Consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderableType
enum RenderableType {
  /// The currently supported renderable types to be used 
  /// in conjunction with a `RenderCommand`.

  RENDERABLE_MESH = 0, 
  RENDERABLE_MODEL, 
  RENDERABLE_DEBUG,

  // @TODO (Renderer): Add more renderable types...
};
/// RenderableType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderPassID
enum RenderPassID {
  /// The currently available render passes the 
  /// renderer goes through in execution order.

  RENDER_PASS_SHADOW = 0, 
  RENDER_PASS_LIGHT,
  RENDER_PASS_HDR,
  RENDER_PASS_BLOOM,
};
/// RenderPassID
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Callbacks

// Have to do this to fix underfined variable errors in the callback.

struct Camera;
struct RenderPass;
struct RenderPassDesc;
struct FrameData;

/// A function callback to move the camera every frame.
using CameraMoveFn = void(*)(Camera& camera);

/// Render pass callbacks

using RenderPassPrepareFn = void(*)(RenderPass* pass, const FrameData& data);
using RenderPassSumbitFn  = void(*)(RenderPass* pass);

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
  GfxBuffer* instance_buffer = nullptr;
  
  Material* material = nullptr;
  Mesh* cube_mesh    = nullptr;

  GfxPipeline* screen_quad = nullptr;
};
/// RendererDefaults 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderCommand
struct RenderCommand {
  /// The type of resource this command will render. 
  RenderableType type;

  /// The object's transform.
  Transform transform;

  /// The resource ID of the renderable. 
  ResourceID renderable_id = {};

  /// The material to be used. 
  ///
  /// @NOTE: If this material is left to its default, 
  /// the renderer will use the default material to render.
  ResourceID material_id   = {};
};
/// RenderCommand
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderInstanceCommand
struct RenderInstanceCommand {
  /// The type of resource this command will render. 
  RenderableType type; 
  
  /// An array of transforms for each of the instances. 
  ///
  /// @NOTE: The number of elements in this array will 
  /// be queried form `instance_count`.
  Transform* transforms;

  /// The resource ID of the renderable. 
  ResourceID renderable_id = {};
  
  /// The material to be used. 
  ///
  /// @NOTE: If this material is left to its default, 
  /// the renderer will use the default material to render.
  ResourceID material_id   = {}; 

  /// The number of instances to render. 
  /// By default, this is set to `1`.
  ///
  /// @NOTE: The instance count cannot exceed `RENDERER_MAX_INSTANCES`.
  sizei instance_count = 1;
};
/// RenderInstanceCommand
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderPassDesc
struct RenderPassDesc {
  /// Render pass functions to be called.

  RenderPassPrepareFn prepare_func; 
  RenderPassSumbitFn sumbit_func;

  /// Resources to be extrated to the render pass.

  ResourceGroupID res_group_id;
  ResourceID shader_context_id;

  /// The size of the framebuffer.
  IVec2 frame_size;

  /// The clear flags to be used every frame.
  u32 clear_flags;

  /// The number of render targets of the render pass. 
  DynamicArray<GfxTextureDesc> targets;

  /// Some user data to be sent every execution of the 
  /// `RenderPassFn` callback.
  void* user_data = nullptr;
};
/// RenderPassDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderPass
struct RenderPass {
  /// A reference to the main graphics context.
  GfxContext* gfx;

  /// Render pass functions to be called.

  RenderPassPrepareFn prepare_func; 
  RenderPassSumbitFn sumbit_func;

  /// Framebuffer information.

  IVec2 frame_size;
  GfxFramebufferDesc framebuffer_desc;
  GfxFramebuffer* framebuffer;
 
  /// The shader context that will be used 
  /// across the render pass.
  ShaderContext* shader_context = nullptr;

  /// The outputs textures which will be given to the next 
  /// render pass or to be rendered to the default framebuffer 
  /// at the end of the render pass chain.
  /// 
  /// @NOTE: The maximum number of inputs cannot exceed `RENDER_TARGETS_MAX` (found in `nikola_gfx.h`).

  GfxTexture* outputs[RENDER_TARGETS_MAX];
  sizei outputs_count = 0;

  /// References to the previous and next passes.
  ///
  /// @NOTE: Either or both of these pointers can be `nullptr`.
  /// Be sure to check them before using them.

  RenderPass* previous; 
  RenderPass* next;
  
  /// State handling

  bool is_active  = true;
  void* user_data = nullptr;
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

  f32 radius = 3.50f; 
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

/// Setup the renderer for any upcoming render operations by 
/// the data given in `data`.
NIKOLA_API void renderer_begin(FrameData& data);

/// Start the render passes chain, flushing the given `RenderQueue` in the process. 
NIKOLA_API void renderer_end();

/// Retrieve the internal `GfxContext` of the global renderer.
NIKOLA_API GfxContext* renderer_get_context();

/// Retrieve the internal default values of the renderer.
NIKOLA_API const RendererDefaults& renderer_get_defaults();

/// Set renderer's clear color to the given `clear_color`.
NIKOLA_API void renderer_set_clear_color(const Vec4& clear_color);

/// Return the renderer's current clear color.
NIKOLA_API Vec4& renderer_get_clear_color();

/// Create a new render pass using the information from `desc`, returning back a
/// pointer to the newly added render pass. 
NIKOLA_API RenderPass* renderer_create_pass(const RenderPassDesc& desc);

/// Append the given `pass` to the renderer's pass chain to be 
/// initiated at the end of the chain.
NIKOLA_API void renderer_append_pass(RenderPass* pass);

/// Prepend the given `pass` to the renderer's pass chain to be 
/// initiated at the beginning of the chain. 
NIKOLA_API void renderer_prepend_pass(RenderPass* pass);

/// Insert the given `pass` at the given `index` in the renderer's pass chain to be 
/// initiated at the certain given `index`.
///
/// @NOTE: There is no maximum value the `index` can check up against. 
/// However, if the end of the pass chain is reached before the given `index`, 
/// the renderer will simply `append` the new pass at the end of the chain.
NIKOLA_API void renderer_insert_pass(RenderPass* pass, const sizei index);

/// Return a const reference to the render pass at `index`.
///
/// @NOTE: If the given `index` is greater than the number of currently 
/// active render passes, this function will just return the 
/// last added render pass.
NIKOLA_API RenderPass* renderer_peek_pass(const sizei index);

/// Queue a rendering command using the given `command` as a specifier. 
NIKOLA_API void renderer_queue_command(const RenderCommand& command);

/// Queue an instanced rendering command using the given `command` as a specifier. 
NIKOLA_API void renderer_queue_command(const RenderInstanceCommand& command);

/// Flush/render the internal command queue of the renderer, using the given 
/// `shader_context` as the main shader for rendering. 
///
/// @NOTE: Internally, the renderer will call this function inside many of 
/// its render passes. However, it can also be called explicitly by the client code 
/// for any specific reason. The internal queue will not be discarded until the end of the frame.
NIKOLA_API void renderer_flush_queue_command(ShaderContext* shader_context);

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
