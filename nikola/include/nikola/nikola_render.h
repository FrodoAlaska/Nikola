#pragma once

#include "nikola_gfx.h"
#include "nikola_resources.h"
#include "nikola_math.h"
#include "nikola_containers.h"
#include "nikola_physics.h"
#include "nikola_timer.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Renderer ***

///---------------------------------------------------------------------------------------------------------------------
/// Consts

/// The maximum amount of instances a renderer can dispatch.
const sizei RENDERER_MAX_INSTANCES        = 2048;

/// The maximum degrees the camera can achieve. 
const f32 CAMERA_MAX_DEGREES              = 89.0f;

/// The maximum amount of zoom the camera can achieve.
const f32 CAMERA_MAX_ZOOM                 = 180.0f;

/// The maximum amount of point lights a scene can have.
const sizei POINT_LIGHTS_MAX              = 16;

/// The maximum amount of particles tha can be emitted per emitter.
const sizei PARTICLES_MAX                 = 1024;

/// The maximum amount of corners a camera's frustum can have.
const sizei CAMERA_FRUSTUM_CORNERS_MAX    = 8;

/// The index of the matrices uniform buffer within all shaders.
const sizei SHADER_MATRICES_BUFFER_INDEX  = 0;

/// The index of the model matrices uniform buffer within all shaders.
const sizei SHADER_MODELS_BUFFER_INDEX    = 1;

/// The index of the material matrices uniform buffer within all shaders.
const sizei SHADER_MATERIALS_BUFFER_INDEX = 2;

/// The index of the light uniform buffer within all shaders.
const sizei SHADER_LIGHT_BUFFER_INDEX     = 3;

/// The index of the animation uniform buffer within all shaders.
const sizei SHADER_ANIMATION_BUFFER_INDEX = 4;

/// Consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderPassID
enum RenderPassID {
  /// The currently available render passes the 
  /// renderer goes through in execution order.

  RENDER_PASS_SHADOW = 0, 
  RENDER_PASS_LIGHT,
  RENDER_PASS_PARTICLE,
  RENDER_PASS_HDR,
  RENDER_PASS_DEBUG,
  
  RENDER_PASSES_MAX,
};
/// RenderPassID
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderQueueType
enum RenderQueueType {
  RENDER_QUEUE_OPAQUE = 0,
  RENDER_QUEUE_PARTICLE,
  RENDER_QUEUE_DEBUG,

  RENDER_QUEUES_MAX,
};
/// RenderQueueType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ParticleDistributionType
enum ParticleDistributionType {
  DISTRIBUTION_RANDOM = 0, 
  DISTRIBUTION_SQUARE,
  DISTRIBUTION_CUBE,
};
/// ParticleDistributionType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Callbacks

// Have to do this to fix underfined variable errors in the callback.

struct RenderPass;
struct RenderPassDesc;
struct FrameData;
struct RenderQueueEntry;

/// Render pass callbacks

using RenderPassPrepareFn   = void(*)(RenderPass* pass, const FrameData& data);
using RenderPassSumbitFn    = void(*)(RenderPass* pass, const RenderQueueEntry& queue);
using RenderPassOnResizeFn  = void(*)(RenderPass* pass, const IVec2& new_size);
using RenderPassOnDestroyFn = void(*)(RenderPass* pass);

/// Callbacks
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RendererDefaults 
struct RendererDefaults {
  /// Textures
  
  GfxTexture* albedo_texture    = nullptr;
  GfxTexture* roughness_texture = nullptr;
  GfxTexture* metallic_texture  = nullptr;
  GfxTexture* normal_texture    = nullptr;
  GfxTexture* emissive_texture  = nullptr;
 
  /// Buffers

  GfxBuffer* matrices_buffer  = nullptr;
  GfxBuffer* instance_buffer  = nullptr;
  GfxBuffer* lights_buffer    = nullptr;
  GfxBuffer* animation_buffer = nullptr;
 
  /// Materials

  Material* material       = nullptr;
  Material* debug_material = nullptr;
  
  /// Pipelines
  
  GfxPipeline* screen_quad_pipe = nullptr;
  GfxPipeline* skybox_pipe      = nullptr;
};
/// RendererDefaults 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// MaterialInterface
struct MaterialInterface {
  ///
  /// The shader representation of the material
  ///

  /// Textures
  
  u64 albedo_handle    = 0;  
  u64 metallic_handle  = 0;  
  u64 roughness_handle = 0;  
  u64 normal_handle    = 0;  
  u64 emissive_handle  = 0;  
  
  /// Variables
  
  f32 metallic     = 0.0f;
  f32 roughness    = 1.0f;
  f32 emissive     = 0.0f;
  f32 transparency = 1.0f;
  Vec2 __padding;

  Vec3 color = Vec3(1.0f);
};
/// MaterialInterface
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderQueueEntry 
struct RenderQueueEntry {
  /// Data to be transferred to the buffers

  DynamicArray<f32> vertices; 
  DynamicArray<u32> indices; 
  DynamicArray<Mat4> transforms; 
  DynamicArray<MaterialInterface> materials;
  DynamicArray<GfxDrawCommandIndirect> commands; 

  /// Pipeline

  GfxPipelineDesc pipe_desc = {};
  GfxPipeline* pipe         = nullptr;

  /// GPU buffers

  GfxBuffer* transform_buffer = nullptr; 
  GfxBuffer* material_buffer  = nullptr; 
  GfxBuffer* command_buffer   = nullptr;

  /// Misc.

  i32 vertex_flags = 0;
};
/// RenderQueueEntry 
///---------------------------------------------------------------------------------------------------------------------


///---------------------------------------------------------------------------------------------------------------------
/// RenderPassDesc
struct RenderPassDesc {
  /// Render pass functions to be called.

  RenderPassPrepareFn prepare_func   = nullptr; 
  RenderPassSumbitFn sumbit_func     = nullptr;
  RenderPassOnResizeFn resize_func   = nullptr;
  RenderPassOnDestroyFn destroy_func = nullptr; 

  /// Resources to be extrated to the render pass.

  ResourceGroupID res_group_id;
  ResourceID shader_context_id;

  /// The size of the framebuffer.
  IVec2 frame_size;

  /// The clear flags to be used every frame.
  u32 clear_flags;

  // An associated render queue to be given upon the 
  // call to `sumbit_func`. 
  //
  // @NOTE: This is set to `RENDER_QUEUE_OPAQUE` by default.
  RenderQueueType queue_type = RENDER_QUEUE_OPAQUE;

  /// The number of render targets of the render pass. 
  DynamicArray<GfxTextureDesc> targets;
};
/// RenderPassDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RenderPass
struct RenderPass {
  /// A reference to the main graphics context.
  GfxContext* gfx;

  /// Render pass functions to be called.

  RenderPassPrepareFn prepare_func   = nullptr; 
  RenderPassSumbitFn sumbit_func     = nullptr;
  RenderPassOnResizeFn resize_func   = nullptr;
  RenderPassOnDestroyFn destroy_func = nullptr; 

  RenderQueueType queue_type;

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
  String debug_name;
};
/// RenderPass
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
};
/// CameraDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Camera 
struct Camera {
  /// Camera's floats

  f32 yaw, pitch;
  f32 zoom, aspect_ratio;

  f32 near        = 0.1f; 
  f32 far         = 100.0f;
  f32 sensitivity = 0.1f;
  f32 exposure    = 1.0f; 

  /// The camera's vectors

  Vec3 position;
  Vec3 up, direction, front;

  /// The camera's matrices
  Mat4 view, projection, view_projection;

  /// Some state to keep

  Vec3 corners[CAMERA_FRUSTUM_CORNERS_MAX]; // The calculated corners of this camera's frustum.
  bool is_active;
};
/// Camera 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ParticleEmitterDesc
struct ParticleEmitterDesc {
  /// The starting position of the particles.
  Vec3 position; 

  /// The velocity of each particle that will be applied 
  /// in the update loop. 
  Vec3 velocity;

  /// The unit scale of each particle in the system. 
  ///
  /// @NOTE: The default scale is set to `Vec3(0.2f, 0.2f, 0.2f)`.
  Vec3 scale                            = Vec3(0.2f);

  /// The mesh of the particle to be used on rendering.
  ResourceID mesh_id;

  /// The material of the particle to be used on rendering.
  ResourceID material_id;

  /// The maximum amount of time a particle can 
  /// live for after being activated.
  ///
  /// @NOTE: The default lifetime is set to `2.5f`.
  f32 lifetime                          = 2.5f;

  /// The gravity contributor of each particle in the system.
  ///
  /// @NOTE: The default gravity is set to `-9.81f`.
  f32 gravity_factor                    = -9.81f;

  /// Defines how the particles will be distributed 
  /// when emitted. 
  ///
  /// @NOTE: The default distribution is set to `DISTRIBUTION_RANDOM`.
  ParticleDistributionType distribution = DISTRIBUTION_RANDOM;

  /// The area or radius of the distribution being applied. 
  /// The radius will act differently depending on the specific distribution. 
  ///
  /// For example, for the `DISTRIBUTION_RANDOM` type, the radius will 
  /// be the maxium value of the random function. While the negation of 
  /// the radius will be the minimum value of the random function.
  ///
  /// @NOTE: The default distribution radius is set to `1.0f`.
  f32 distribution_radius               = 1.0f;

  /// The amount of particles to emit. 
  ///
  /// @NOTE: This variable CANNOT exceed `PARTICLES_CPU_MAX`.
  sizei count                           = 0; 
};
/// ParticleEmitterDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ParticleEmitter 
struct ParticleEmitter {
  Vec3 initial_position = Vec3(0.0f);
  Vec3 initial_velocity = Vec3(0.0f);

  Transform transforms[PARTICLES_MAX];
  Vec3 forces[PARTICLES_MAX];
  Vec3 velocities[PARTICLES_MAX];

  sizei particles_count = 0;
  Timer lifetime; 

  ResourceID mesh_id; 
  ResourceID material_id;
  
  f32 distribution_radius               = 1.0f;
  ParticleDistributionType distribution = DISTRIBUTION_RANDOM;

  f32 gravity_factor = 0.0f; 
  bool is_active     = false;
};
/// ParticleEmitter 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Animator
struct Animator {
  /// The animation associated with this animator.
  ResourceID animation_id; 

  /// The current ticking time of the animation.
  f32 current_time  = 0.0f;
 
  /// Indicates the start point in frames 
  /// of the animation.
  f32 start_point   = 0.0f;

  /// Indicates the end point in frames 
  /// of the animation.
  ///
  /// @NOTE: This is usually set to whatever the `duration` 
  /// member of the `animation_id` is.
  f32 end_point     = 0.0f;

  /// Determines whether the animation should loop or not.
  bool is_looping   = true;

  /// When this flag is set to `true`, the animation will 
  /// go through its samples and play. Otherwise, the animation 
  /// will be paused.
  bool is_animating = true;

  /// Determines whether the animation should be 
  /// played in reverse or not.
  bool is_reversed  = false;
};
/// Animator
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// DirectionalLight 
struct DirectionalLight {
  /// The direction vector of the directional light.
  Vec3 direction = Vec3(1.0f); 

  /// The color of the directional light.
  Vec3 color     = Vec3(1.0f);
  
  DirectionalLight() : 
    direction(Vec3(1.0f)), color(Vec3(1.0f))
    {}

  DirectionalLight(const Vec3& dir, const Vec3& col) : 
    direction(dir), color(col)
    {}
};
/// DirectionalLight 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PointLight
struct PointLight {
  /// The position vector of the point light in world space.
  Vec3 position = Vec3(0.0f); 

  /// The color of the point light.
  Vec3 color    = Vec3(1.0f);

  /// The radius of a point light in radians.
  float radius = 2.5f;

  /// The distance of the fall off
  float fall_off = 1.0f; 

  PointLight() : 
    position(Vec3(0.0f)), color(Vec3(1.0f)), radius(2.5f)
    {}
  
  PointLight(const Vec3& pos) : 
    position(pos), color(Vec3(1.0f)), radius(2.5f)
    {}

  PointLight(const Vec3& pos, const Vec3& col, const float radius) : 
    position(pos), color(col), radius(radius)
    {}
};
/// PointLight
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// SpotLight
struct SpotLight {
  /// The position of the spot light in world space.
  Vec3 position; 

  /// The direction of the spot light.
  Vec3 direction;

  /// The color of the spot light
  Vec3 color;

  /// The radius of the spot light in radians, 
  /// or the "umbra" of the light.
  float radius;

  /// The outer radius of the spot light in radians.
  /// or the "penumbra" of the light.
  float outer_radius; 

  SpotLight() : 
    position(Vec3(0.0f)), direction(Vec3(1.0f)), color(Vec3(1.0f)), radius(0.3f), outer_radius(0.5f)
    {}

  SpotLight(const Vec3& pos, const Vec3& dir, const Vec3& col, const float radius, const float outer_radius) : 
    position(pos), direction(dir), color(col), radius(radius), outer_radius(outer_radius)
    {}
};
/// SpotLight
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// FrameData
struct FrameData {
  Camera camera; 
  ResourceID skybox_id;
  
  Vec3 ambient = Vec4(0.125f);

  DirectionalLight dir_light; 
  DynamicArray<PointLight> point_lights;
  DynamicArray<SpotLight> spot_lights;
};
/// FrameData
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

/// Set renderer's clear color to the given `clear_color`.
NIKOLA_API void renderer_set_clear_color(const Vec4& clear_color);

/// Retrieve the internal `GfxContext` of the global renderer.
NIKOLA_API GfxContext* renderer_get_context();

/// Retrieve the internal default values of the renderer.
NIKOLA_API const RendererDefaults& renderer_get_defaults();

/// Retrieve the renderer's current viewport size
NIKOLA_API IVec2 renderer_get_viewport_size();

/// Retrieve the renderer's current clear color.
NIKOLA_API Vec4 renderer_get_clear_color();

/// Retrieve the render queue of `type`.
NIKOLA_API const RenderQueueEntry* renderer_get_queue(const RenderQueueType type);

/// Create a new render pass using the information from `desc` identified with `debug_name`,
/// returning back a pointer to the newly added render pass. 
///
/// If the given `parent` is set to be valid, the `GfxFramebuffer` of the render pass,
/// `GfxFramebufferDesc`, and the `frame_size` will all be inherited from `parent`. Otherwise, 
/// the pass will create its own `GfxFramebuffer`, using the information provided in `desc`.
///
/// @NOTE: By default, `parent` is set to `nullptr`.
NIKOLA_API RenderPass* renderer_create_pass(const RenderPassDesc& desc, const String& debug_name, const RenderPass* parent = nullptr);

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

/// Remove the pass at the given `index` from the render pass chain.
NIKOLA_API void renderer_remove_pass(const sizei index);

/// Return a const reference to the render pass at `index`.
///
/// @NOTE: If the given `index` is greater than the number of currently 
/// active render passes, this function will just return the 
/// last added render pass.
NIKOLA_API RenderPass* renderer_peek_pass(const sizei index);

/// A series of functions to queue `count` instanced rendering commands
/// using the given `res_id` of type `type` at `transforms` in world space, using `mat_id`.
///
/// @NOTE: The given `count` cannot exceed `RENDERER_MAX_INSTANCES`.
/// 
/// @NOTE: If `mat_id` is left untouched, the renderer will use the default material.

NIKOLA_API void renderer_queue_mesh_instanced(const ResourceID& res_id, 
                                              const Transform* transforms, 
                                              const sizei count, 
                                              const ResourceID& mat_id = {});

NIKOLA_API void renderer_queue_model_instanced(const ResourceID& res_id, 
                                               const Transform* transforms, 
                                               const sizei count, 
                                               const ResourceID& mat_id = {});

NIKOLA_API void renderer_queue_animation_instanced(const ResourceID& res_id, 
                                                   const ResourceID& model_id,
                                                   const Transform* transforms, 
                                                   const sizei count, 
                                                   const ResourceID& mat_id = {});

/// A series of functions that queue a rendering command, using the given `res_id`
/// at `transform` in world space, using `mat_id`.
///
/// @NOTE: If `mat_id` is left untouched, the renderer will use the default material.

NIKOLA_API void renderer_queue_mesh(const ResourceID& res_id, 
                                    const Transform& transform, 
                                    const ResourceID& mat_id = {});

NIKOLA_API void renderer_queue_model(const ResourceID& res_id, 
                                     const Transform& transform, 
                                     const ResourceID& mat_id = {});

NIKOLA_API void renderer_queue_animation(const ResourceID& res_id, 
                                         const ResourceID& model_id,
                                         const Transform& transform, 
                                         const ResourceID& mat_id = {});

NIKOLA_API void renderer_queue_particles(const ParticleEmitter& emitter);

/// A series of functions to queue `count` instanced debug rendering commands
/// using the given the `transforms` and `mat_id`.
/// These are only used for debugging purposes and will be drawn in the 
/// debug render pass. 
///
/// @NOTE: The given `count` cannot exceed `RENDERER_MAX_INSTANCES`.
/// 
/// @NOTE: If `mat_id` is left untouched, the renderer will use the default debug material.

NIKOLA_API void renderer_queue_debug_cube_instanced(const Transform* transforms, 
                                                    const sizei count, 
                                                    const ResourceID& mat_id = {});

NIKOLA_API void renderer_queue_debug_sphere_instanced(const Transform* transforms, 
                                                      const sizei count, 
                                                      const ResourceID& mat_id = {});

/// A series of functions to queue a debug rendering commands
/// using the given the `transform` and `mat_id`.
/// These are only used for debugging purposes and will be drawn in the 
/// debug render pass. 
/// 
/// @NOTE: If `mat_id` is left untouched, the renderer will use the default debug material.

NIKOLA_API void renderer_queue_debug_cube(const Transform& transform, const ResourceID& mat_id = {});
NIKOLA_API void renderer_queue_debug_sphere(const Transform& transform, const ResourceID& mat_id = {});

/// Draw the given `skybox_id` skybox into the scene. 
///
/// @NOTE: This call should only be made inside render passes. 
/// Any calls to this function outside render passes is not 
/// guaranteed to work.
///
/// Yes, I am a bad developer. Thank you.
NIKOLA_API void renderer_draw_skybox(const ResourceID& skybox_id);

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
NIKOLA_API void batch_render_texture(GfxTexture* texture, const Rect2D& src, const Rect2D& dest, const Vec4& tint = Vec4(1.0f));

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

/// Have the given `cam` follow the `target`, taking into account the given `offset`.
NIKOLA_API void camera_follow(Camera& cam, const Vec3& target, const Vec3& offset);

/// Using linear interpolation, have the given `cam` follow `target` by `delta`, taking
/// into account the given `offset`.
NIKOLA_API void camera_follow_lerp(Camera& cam, const Vec3& target, const Vec3& offset, const f32 delta);

/// Using the given `cam`, convert the world space `position` to screen space coordinates 
/// using the width and height of the given `window`.
NIKOLA_API Vec2 camera_world_to_screen_space(const Camera& cam, const Vec3 position, const Window* window);

/// Using the given `cam`, convert the screen space `position`,
/// using the width and height of the given `window`, returning back a ray with the direction 
/// and the origin in world space coordinates. 
NIKOLA_API RayCastDesc camera_screen_to_world_space(const Camera& cam, const Vec2 position, const Window* window);

/// Check if the given `transform` is currently intersecting the `cam`'s frustum.
NIKOLA_API const bool camera_check_intersection(const Camera& cam, const Transform& transform);

/// Camera functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ParticleEmitter functions

/// Create a particle emitter `out_emitter`, using the information in `desc`.
NIKOLA_API void particle_emitter_create(ParticleEmitter* out_emitter, const ParticleEmitterDesc& desc);

/// A physics update of each particle in the given `emitter` using the scale of `delta_time`. 
NIKOLA_API void particle_emitter_update(ParticleEmitter& emitter, const f64 delta_time); 

/// Emit the particles of `emitter`.
NIKOLA_API void particle_emitter_emit(ParticleEmitter& emitter);

/// Reset the given `emitter` to its initial state.
NIKOLA_API void particle_emitter_reset(ParticleEmitter& emitter);

/// ParticleEmitter functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Animator functions

/// Create an animator component using the given `animation`.
NIKOLA_API void animator_create(Animator* animator, const ResourceID& animation);

/// Start the animation process of the given `animator`, using the given `dt` as 
/// a delta time for progressing through the animation.
NIKOLA_API void animator_animate(Animator& animator, const f32& dt);

/// Set the current animation of `animator` to the given `animation`.
NIKOLA_API void animator_set_animation(Animator& animator, const ResourceID& animation);

/// Animator functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Renderer ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
