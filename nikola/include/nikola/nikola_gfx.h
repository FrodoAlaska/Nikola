#pragma once

#include "nikola_window.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ---------------------------------------------------------------------
/// *** Graphics ***

///---------------------------------------------------------------------------------------------------------------------
// Consts

/// The maximum amount of textures the GPU supports at a time. 
const sizei TEXTURES_MAX                     = 32;

/// The maximum amount of color attachments that can be attached to a framebuffer. 
const sizei FRAMEBUFFER_ATTACHMENTS_MAX      = 4;

/// The maximum amount of textures the GPU supports at a time. 
const sizei CUBEMAPS_MAX                     = 5;

/// The maximum amount of faces in a cubemap
const sizei CUBEMAP_FACES_MAX                = 6;

/// The maximum number of uniforms a shader is allowed to have.
const sizei UNIFORMS_MAX                     = 256;

/// The maximum amount of uniform buffers to be created in a shader type.
const sizei UNIFORM_BUFFERS_MAX              = 16;

/// The maximum number of characters a uniform name is allowed to have.
const sizei UNIFORM_NAME_LENGTH_MAX          = 64;

/// The maximum number of attributes a buffer's layout can have.
const sizei VERTEX_ATTRIBUTES_MAX            = 16;

/// The maximum number of vertex layouts a pipeline can have.
const sizei VERTEX_LAYOUTS_MAX               = 2;

/// The maximum number of characters a vertex attribute name is allowed to have.
const sizei VERTEX_ATTRIBUTE_NAME_LENGTH_MAX = 64;

/// The maximum number of render targets to be bound at once.
const sizei RENDER_TARGETS_MAX               = 8;

/// The maximum number of render targets to be bound at once.
const sizei MAX_COMPUTE_WORK_GROUPS_COUNT    = 65535;

// Consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxStates
enum GfxStates {
  /// Enable the depth testing pass.
  GFX_STATE_DEPTH   = 2 << 0, 
  
  /// Enable the stencil testing pass. 
  GFX_STATE_STENCIL = 2 << 1, 

  /// Enable blending.
  GFX_STATE_BLEND   = 2 << 2, 

  /// Enable multisampling. 
  GFX_STATE_MSAA    = 2 << 3, 

  /// Enable face culling. 
  GFX_STATE_CULL    = 2 << 4,

  /// Enable face culling. 
  GFX_STATE_SCISSOR = 2 << 5,
};
/// GfxStates
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxCompareFunc
enum GfxCompareFunc {
  /// Always passes the comparison.
  GFX_COMPARE_ALWAYS,

  /// Never passes the comparison.
  GFX_COMPARE_NEVER,
  
  /// Passes the comparison when `a == b`.
  GFX_COMPARE_EQUAL,

  /// Passes the comparison when the `a < b`.
  GFX_COMPARE_LESS, 

  /// Passes the comparison when the `a <= b`.
  GFX_COMPARE_LESS_EQUAL, 

  /// Passes the comparison when the `a > b`.
  GFX_COMPARE_GREATER, 

  /// Passes the comparison when the `a >= b`.
  GFX_COMPARE_GREATER_EQUAL, 

  /// Passes the comparison when the `a != b`.
  GFX_COMPARE_NOT_EQUAL,
};
/// GfxCompareFunc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxOperation 
enum GfxOperation {
  /// Always keep the value.
  GFX_OP_KEEP, 
  
  /// Always set the value to `0`.
  GFX_OP_ZERO, 

  /// Invert the value.
  GFX_OP_INVERT, 

  /// Replace the value `a` with the other value `b`.
  GFX_OP_REPLACE, 

  /// Increase the value by `1`.
  GFX_OP_INCR, 

  /// Decrease the value by `1`.
  GFX_OP_DECR, 

  /// Increase the value by `1` and wrap to the beginning when it reaches the maximum.
  GFX_OP_INCR_WRAP, 

  /// Decrease the value by `1` and wrap to the end when it reaches the minimum.
  GFX_OP_DECR_WRAP, 
};
/// GfxOperation 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBlendMode 
enum GfxBlendMode {
  /// Always keep the blended value at `0`.
  GFX_BLEND_ZERO,
  
  /// Always keep the blended value at `1`.
  GFX_BLEND_ONE, 
  
  /// Take the source's color as the blended value.
  GFX_BLEND_SRC_COLOR,
  
  /// Take the destination's color as the blended value.
  GFX_BLEND_DEST_COLOR, 
  
  /// Take the source's alpha value as the blended value.
  GFX_BLEND_SRC_ALPHA, 
  
  /// Take the destination's alpha value as the blended value.
  GFX_BLEND_DEST_ALPHA, 
  
  /// Take the inverse of the source's color as the blended value.
  GFX_BLEND_INV_SRC_COLOR, 
  
  /// Take the inverse of the destination's color as the blended value.
  GFX_BLEND_INV_DEST_COLOR, 
  
  /// Take the inverse of the source's alpha value as the blended value.
  GFX_BLEND_INV_SRC_ALPHA, 
  
  /// Take the inverse of the destination's alpha value as the blended value.
  GFX_BLEND_INV_DEST_ALPHA,
  
  /// Take the saturated alpha value of the source's color as the blended value.
  GFX_BLEND_SRC_ALPHA_SATURATE,
};
/// GfxBlendMode 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxCullMode
enum GfxCullMode {
  /// Only cull the front faces.
  GFX_CULL_FRONT,
  
  /// Only cull the back faces.
  GFX_CULL_BACK,
  
  /// Cull both the front and back faces.
  GFX_CULL_FRONT_AND_BACK,
};
/// GfxCullMode
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxCullOrder
enum GfxCullOrder {
  /// Clockwise vertices are the front faces.
  GFX_ORDER_CLOCKWISE, 
  
  /// Counter-clockwise vertices are the front faces.
  GFX_ORDER_COUNTER_CLOCKWISE,
};
/// GfxCullOrder
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
enum GfxClearFlags {
  /// No buffers will be cleared.
  GFX_CLEAR_FLAGS_NONE           = 3 << 0,
  
  /// Clear the color buffer of the current context. 
  GFX_CLEAR_FLAGS_COLOR_BUFFER   = 3 << 1,

  /// Clear the depth buffer of the current context. 
  ///
  /// @NOTE: This flag will be ignored if the depth state is disabled.
  GFX_CLEAR_FLAGS_DEPTH_BUFFER   = 3 << 2,
  
  /// Clear the stencil buffer of the current context. 
  ///
  /// @NOTE: This flag will be ignored if the stencil state is disabled.
  GFX_CLEAR_FLAGS_STENCIL_BUFFER = 3 << 3,
};
/// GfxContextFlags 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBufferType
enum GfxBufferType {
  /// A vertex buffer.
  GFX_BUFFER_VERTEX, 

  /// An index buffer.
  GFX_BUFFER_INDEX, 

  /// A uniform buffer.
  GFX_BUFFER_UNIFORM,

  /// A shader storage buffer.
  GFX_BUFFER_SHADER_STORAGE,
  
  /// A draw command indirect buffer.
  GFX_BUFFER_DRAW_INDIRECT,
};
/// GfxBufferType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBufferUsage
enum GfxBufferUsage {
  /// Set the buffer to be dynamically written to.
  /// This will be used for frequently writing to the buffer.
  GFX_BUFFER_USAGE_DYNAMIC_DRAW = 4 << 0,
  
  /// Set the buffer to be dynamically read from.
  /// This will be used for frequent reading from the buffer.
  GFX_BUFFER_USAGE_DYNAMIC_READ = 4 << 1,

  /// Set the buffer to be statically written to.
  /// This will be used for writing to the buffer once or rarely.
  GFX_BUFFER_USAGE_STATIC_DRAW  = 4 << 2,

  /// Set the buffer to be statically read from.
  /// This will be used for reading from the buffer once or rarely.
  GFX_BUFFER_USAGE_STATIC_READ  = 4 << 3,
};
/// GfxBufferUsage
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxDrawMode
enum GfxDrawMode {
  /// Will set up the pipeline to draw points.
  GFX_DRAW_MODE_POINT,

  /// Will set up the pipeline to draw triangles.
  GFX_DRAW_MODE_TRIANGLE,
  
  /// Will set up the pipeline to draw triangle strips.
  GFX_DRAW_MODE_TRIANGLE_STRIP,
  
  /// Will set up the pipeline to draw lines.
  GFX_DRAW_MODE_LINE,
  
  /// Will set up the pipeline to draw line strips.
  GFX_DRAW_MODE_LINE_STRIP,
};
/// GfxDrawMode
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxLayoutType
enum GfxLayoutType {
  /// Equivalent to `f32`.
  GFX_LAYOUT_FLOAT1,
  
  /// Equivalent to 2 `f32`s.
  GFX_LAYOUT_FLOAT2,
  
  /// Equivalent to 3 `f32`s.
  GFX_LAYOUT_FLOAT3,
  
  /// Equivalent to 4 `f32`s.
  GFX_LAYOUT_FLOAT4,
  
  /// Equivalent to `i8`.
  GFX_LAYOUT_BYTE1,
  
  /// Equivalent to 2 `i8`s.
  GFX_LAYOUT_BYTE2,
  
  /// Equivalent to 3 `i8`s.
  GFX_LAYOUT_BYTE3,
  
  /// Equivalent to 4 `i8`s.
  GFX_LAYOUT_BYTE4,
  
  /// Equivalent to `u8`.
  GFX_LAYOUT_UBYTE1,
  
  /// Equivalent to 2 `u8`s.
  GFX_LAYOUT_UBYTE2,
  
  /// Equivalent to 3 `u8`s.
  GFX_LAYOUT_UBYTE3,
  
  /// Equivalent to 4 `u8`s.
  GFX_LAYOUT_UBYTE4,
  
  /// Equivalent to `i16`.
  GFX_LAYOUT_SHORT1,
  
  /// Equivalent to 2 `i16`s.
  GFX_LAYOUT_SHORT2,
  
  /// Equivalent to 3 `i16`s.
  GFX_LAYOUT_SHORT3,
  
  /// Equivalent to 4 `i16`s.
  GFX_LAYOUT_SHORT4,
  
  /// Equivalent to `u16`.
  GFX_LAYOUT_USHORT1,
  
  /// Equivalent to 2 `u16`s.
  GFX_LAYOUT_USHORT2,
  
  /// Equivalent to 3 `u16`s.
  GFX_LAYOUT_USHORT3,
  
  /// Equivalent to 4 `u16`s.
  GFX_LAYOUT_USHORT4,
  
  /// Equivalent to `i32`.
  GFX_LAYOUT_INT1,
  
  /// Equivalent to 2 `i32`s.
  GFX_LAYOUT_INT2,
  
  /// Equivalent to 3 `i32`s.
  GFX_LAYOUT_INT3,
  
  /// Equivalent to 4 `i32`s.
  GFX_LAYOUT_INT4,
  
  /// Equivalent to `u32`.
  GFX_LAYOUT_UINT1,
  
  /// Equivalent to 2 `u32`s.
  GFX_LAYOUT_UINT2,
  
  /// Equivalent to 3 `u32`s.
  GFX_LAYOUT_UINT3,
  
  /// Equivalent to 4 `u32`s.
  GFX_LAYOUT_UINT4,
  
  /// Equivalent to `Mat2`.
  GFX_LAYOUT_MAT2,
  
  /// Equivalent to `Mat3`.
  GFX_LAYOUT_MAT3,
  
  /// Equivalent to `Mat4`.
  GFX_LAYOUT_MAT4,
};
/// GfxLayoutType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTextureType
enum GfxTextureType {
  /// Creates a 1D texture.
  GFX_TEXTURE_1D,
  
  /// Creates a 2D texture.
  GFX_TEXTURE_2D,
  
  /// Creates a 3D texture.
  GFX_TEXTURE_3D,
  
  /// Creates a 1D texture array.
  GFX_TEXTURE_1D_ARRAY,
  
  /// Creates a 2D texture array.
  GFX_TEXTURE_2D_ARRAY,
  
  /// Creates a 1D image.
  GFX_TEXTURE_IMAGE_1D,
  
  /// Creates a 2D image.
  GFX_TEXTURE_IMAGE_2D,
  
  /// Creates a 3D image.
  GFX_TEXTURE_IMAGE_3D,
  
  /// Creates a texture to be used as the depth target.
  GFX_TEXTURE_DEPTH_TARGET,
  
  /// Creates a texture to be used as the stencil target.
  GFX_TEXTURE_STENCIL_TARGET,
  
  /// Creates a texture to be used as both the depth and stencil target.
  GFX_TEXTURE_DEPTH_STENCIL_TARGET,
};
/// GfxTextureType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTextureFormat
enum GfxTextureFormat {
  /// An `unsigned char` per pixel red channel texture format.
  GFX_TEXTURE_FORMAT_R8,
  
  /// An `unsigned short` per pixel red channel texture format.
  GFX_TEXTURE_FORMAT_R16,
  
  /// A `half float` per pixel red channel texture format.
  GFX_TEXTURE_FORMAT_R16F,
  
  /// A `float` per pixel red channel texture format.
  GFX_TEXTURE_FORMAT_R32F,

  /// An `unsigned char` per pixel red and green channel texture format.
  GFX_TEXTURE_FORMAT_RG8,
  
  /// An `unsigned short` per pixel red and green channel texture format.
  GFX_TEXTURE_FORMAT_RG16,
  
  /// A `half float` per pixel red and green channel texture format.
  GFX_TEXTURE_FORMAT_RG16F,
  
  /// A `float` per pixel red and green channel texture format.
  GFX_TEXTURE_FORMAT_RG32F,
  
  /// An `unsigned char` per pixel red, green, blue, and alpha channel texture format.
  GFX_TEXTURE_FORMAT_RGBA8,
  
  /// An `unsigned short` bits per pixel red, green, blue, and alpha channel texture format.
  GFX_TEXTURE_FORMAT_RGBA16,
  
  /// A `half float` per pixel red, green, blue, and alpha channel texture format.
  GFX_TEXTURE_FORMAT_RGBA16F,
  
  /// A `float` per pixel red, green, blue, and alpha channel texture format.
  GFX_TEXTURE_FORMAT_RGBA32F,
  
  /// An `unsigned short` per depth value to be used for the depth buffer.
  GFX_TEXTURE_FORMAT_DEPTH16,

  /// A 24-bits per depth value to be used for the depth buffer.
  GFX_TEXTURE_FORMAT_DEPTH24,

  /// A `float` per depth value to be used for the depth buffer.
  GFX_TEXTURE_FORMAT_DEPTH32F,
 
  /// An `unsigned char` per stencil value to be used for the stencil buffer.
  GFX_TEXTURE_FORMAT_STENCIL8,

  /// A format to be used with the depth and stencil buffers where 
  /// the depth buffer gets 24 bits and the stencil buffer gets 8 bits.
  GFX_TEXTURE_FORMAT_DEPTH_STENCIL_24_8,
};
/// GfxTextureFromat
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTextureFilter
enum GfxTextureFilter {
  /// Uses linear filtering in both modes.
  GFX_TEXTURE_FILTER_MIN_MAG_LINEAR,
  
  /// Uses nearest filtering on both modes.
  GFX_TEXTURE_FILTER_MIN_MAG_NEAREST,
  
  /// Uses linear filtering on minification and nearest filtering magnification. 
  GFX_TEXTURE_FILTER_MIN_LINEAR_MAG_NEAREST,
  
  /// Uses nearest filtering on minification and linear filtering magnification. 
  GFX_TEXTURE_FILTER_MIN_NEAREST_MAG_LINEAR,
  
  /// Uses trilinear filtering (the weighted average of the two closest mipmaps)
  /// on minification and linear filtering on magnification.
  GFX_TEXTURE_FILTER_MIN_TRILINEAR_MAG_LINEAR,
  
  /// Uses trilinear filtering (the weighted average of the two closest mipmaps)
  /// on minification and nearest filtering on magnification.
  GFX_TEXTURE_FILTER_MIN_TRILINEAR_MAG_NEAREST,
};
/// GfxTextureFilter
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTextureWrap
enum GfxTextureWrap {
  /// Repeat the pixel when wrapped.
  GFX_TEXTURE_WRAP_REPEAT, 
  
  /// Mirror the pixel when wrapped.
  GFX_TEXTURE_WRAP_MIRROR, 
  
  /// Clamp the pixel when wrapped.
  GFX_TEXTURE_WRAP_CLAMP, 
  
  /// Use the border color when wrapped.
  GFX_TEXTURE_WRAP_BORDER_COLOR,
};
/// GfxTextureWrap
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTextureAccess
enum GfxTextureAccess {
  /// Create a texture with read-only memory.
  GFX_TEXTURE_ACCESS_READ, 
  
  /// Create a texture with write-only memory.
  GFX_TEXTURE_ACCESS_WRITE, 
  
  /// Create a texture with read and write memory.
  GFX_TEXTURE_ACCESS_READ_WRITE, 
};
/// GfxTextureAccess
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxShaderType
enum GfxShaderType {
  /// A vertex shader.
  GFX_SHADER_VERTEX, 

  /// A pixel/fragment shader.
  GFX_SHADER_PIXEL, 

  /// A compute shader.
  GFX_SHADER_COMPUTE,
};
/// GfxShaderType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxUniformType
enum GfxUniformType {
  /// Float uniform types
  
  GFX_UNIFORM_FLOAT1 = 0, 
  GFX_UNIFORM_FLOAT2, 
  GFX_UNIFORM_FLOAT3, 
  GFX_UNIFORM_FLOAT4, 
  
  /// Double uniform types
  
  GFX_UNIFORM_DOUBLE1, 
  GFX_UNIFORM_DOUBLE2, 
  GFX_UNIFORM_DOUBLE3, 
  GFX_UNIFORM_DOUBLE4, 
  
  /// Signed int uniform types
  
  GFX_UNIFORM_INT1, 
  GFX_UNIFORM_INT2, 
  GFX_UNIFORM_INT3, 
  GFX_UNIFORM_INT4, 
  
  /// Unsigned int uniform types
  
  GFX_UNIFORM_UINT1, 
  GFX_UNIFORM_UINT2, 
  GFX_UNIFORM_UINT3, 
  GFX_UNIFORM_UINT4, 
  
  /// Bool uniform types
  
  GFX_UNIFORM_BOOL1, 
  GFX_UNIFORM_BOOL2, 
  GFX_UNIFORM_BOOL3, 
  GFX_UNIFORM_BOOL4, 
  
  /// Matrix uniform types
  
  GFX_UNIFORM_MAT2, 
  GFX_UNIFORM_MAT3, 
  GFX_UNIFORM_MAT4, 
  
  GFX_UNIFORM_MAT2X3, 
  GFX_UNIFORM_MAT2X4, 
  
  GFX_UNIFORM_MAT3X2, 
  GFX_UNIFORM_MAT3X4,

  GFX_UNIFORM_MAT4X2, 
  GFX_UNIFORM_MAT4X3, 
 
  /// Sampler uniform types
  
  GFX_UNIFORM_SAMPLER_1D, 
  GFX_UNIFORM_SAMPLER_2D, 
  GFX_UNIFORM_SAMPLER_3D, 
  GFX_UNIFORM_SAMPLER_CUBE, 
  
  GFX_UNIFORM_SAMPLER_1D_SHADOW, 
  GFX_UNIFORM_SAMPLER_2D_SHADOW, 
  GFX_UNIFORM_SAMPLER_CUBE_SHADOW, 
  
  GFX_UNIFORM_SAMPLER_1D_ARRAY, 
  GFX_UNIFORM_SAMPLER_2D_ARRAY, 
  GFX_UNIFORM_SAMPLER_1D_ARRAY_SHADOW, 
  GFX_UNIFORM_SAMPLER_2D_ARRAY_SHADOW, 

  /// Image uniform types 

  GFX_UNIFORM_IMAGE_1D,
  GFX_UNIFORM_IMAGE_2D,
  GFX_UNIFORM_IMAGE_3D,
  GFX_UNIFORM_IMAGE_CUBE,
  
  GFX_UNIFORM_IMAGE_1D_ARRAY,
  GFX_UNIFORM_IMAGE_2D_ARRAY,
};
/// GfxUniformType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxMemoryBarrierType
enum GfxMemoryBarrierType {
  /// Create a memory barrier for vertex attribute operations. 
  GFX_MEMORY_BARRIER_VERTEX_ATTRIBUTE      = 5 << 0,

  /// Create a memory barrier for index buffer operations.
  GFX_MEMORY_BARRIER_INDEX_BUFFER          = 5 << 1, 
  
  /// Create a memory barrier for uniform buffer operations.
  GFX_MEMORY_BARRIER_UNIFORM_BUFFER        = 5 << 2, 
  
  /// Create a memory barrier for shader storage buffer operations.
  GFX_MEMORY_BARRIER_SHADER_STORAGE_BUFFER = 5 << 3, 
  
  /// Create a memory barrier for framebuffer operations.
  GFX_MEMORY_BARRIER_FRAMEBUFFER           = 5 << 4, 
  
  /// Create a memory barrier for buffer update operations.
  GFX_MEMORY_BARRIER_BUFFER_UPDATE         = 5 << 5, 
  
  /// Create a memory barrier for texture fetch operations.
  GFX_MEMORY_BARRIER_TEXTURE_FETCH         = 5 << 6, 
  
  /// Create a memory barrier for texture update operations.
  GFX_MEMORY_BARRIER_TEXTURE_UPDATE        = 5 << 7, 
  
  /// Create a memory barrier for image access by shader operations.
  GFX_MEMORY_BARRIER_SHADER_IMAGE_ACCESS   = 5 << 8, 
  
  /// Create a memory barrier for atmoic counting operations.
  GFX_MEMORY_BARRIER_ATOMIC_COUNTER        = 5 << 9, 
  
  /// Create a memory barrier for all operations above.
  GFX_MEMORY_BARRIER_ALL                   = 5 << 10, 
};
/// GfxMemoryBarrierType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxContext
struct GfxContext; 
/// GfxContext
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxFramebuffer
struct GfxFramebuffer;
/// GfxFramebuffer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBuffer
struct GfxBuffer;
/// GfxBuffer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxShader
struct GfxShader;
/// GfxShader
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTexture
struct GfxTexture;
/// GfxTexture
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxCubemap
struct GfxCubemap;
/// GfxCubemap
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxPipeline
struct GfxPipeline;
/// GfxPipeline
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxDepthDesc
struct GfxDepthDesc {
  /// The comparison funcion of the depth buffer.
  /// The default value is `GFX_COMPARE_LESS_EQUAL`.
  GfxCompareFunc compare_func = GFX_COMPARE_LESS_EQUAL;

  /// Enables/disables writing to the depth buffer.
  /// The default value is `true`.
  bool depth_write_enabled    = true;
};
/// GfxDepthDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxStencilDesc
struct GfxStencilDesc {
  /// Which of the polygon faces will the below operations
  /// affect on. 
  /// The default value is "GFX_FACE_FRONT_AND_BACK".
  GfxCullMode polygon_face     = GFX_CULL_FRONT_AND_BACK; 

  /// The comparison function of the stencil buffer.
  /// The default value is `GFX_COMPARE_ALWAYS`.
  GfxCompareFunc compare_func  = GFX_COMPARE_ALWAYS;

  /// The operation to carry when the stencil test fails. 
  /// The default value is `GFX_OP_KEEP`.
  GfxOperation stencil_fail_op = GFX_OP_KEEP;

  /// The operation to be carried out when the depth test succeeds. 
  /// The default value is `GFX_OP_KEEP`.
  GfxOperation depth_pass_op   = GFX_OP_KEEP;

  /// The operation to be carried out when the depth test fails. 
  /// The default value is `GFX_OP_KEEP`.
  GfxOperation depth_fail_op   = GFX_OP_KEEP;

  /// The reference value of the stencil test. 
  /// The default value is `1`.
  i32 ref                      = 1;

  /// The mask that will be bitwise `AND`ed with the `ref` and the stencil value 
  /// currently in the buffer. The two resulting `AND`ded values will then be compared 
  /// to determine the outcome of that pixel.
  ///
  /// The default value is `0xff`.
  u32 mask                     = 0xff;
};
/// GfxStencilDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBlendDesc
struct GfxBlendDesc {
  /// The blend mode of the RGB of the source's color. 
  /// The default value is `GFX_BLEND_SRC_ALPHA`.
  GfxBlendMode src_color_blend  = GFX_BLEND_SRC_ALPHA; 

  /// The blend mode of the RGB of the destination's color. 
  /// The default value is `GFX_BLEND_INV_SRC_ALPHA`.
  GfxBlendMode dest_color_blend = GFX_BLEND_INV_SRC_ALPHA; 

  /// The blend mode of the Alpha value of the source's color. 
  /// The default value is `GFX_BLEND_INV_SRC_ALPHA`.
  GfxBlendMode src_alpha_blend  = GFX_BLEND_ONE; 

  /// The blend mode of the Alpha value of the destination's color. 
  /// The default value is `GFX_BLEND_INV_SRC_ALPHA`.
  GfxBlendMode dest_alpha_blend = GFX_BLEND_ONE; 

  /// The default blend factor. 
  /// The default values are `R = 0, G = 0, B = 0, A = 0`.
  f32 blend_factor[4]           = {0, 0, 0, 0};
};
/// GfxBlendDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxCullDesc 
struct GfxCullDesc {
  /// The face to cull. 
  /// If this value is set to `GFX_CULL_FRONT_AND_BACK`, then 
  /// all the polygons will be culled. 
  /// 
  /// @NOTE: By default, this value is set to `GFX_CULL_FRONT`.
  GfxCullMode cull_mode  = GFX_CULL_FRONT;

  /// Determines the front-facing triangle. 
  /// If this value is set to `GFX_CULL_CLOCKWISE`, then
  /// the triangles with clockwise-ordering vertices are considered 
  /// front-facing and back-facing otherwise. The opposite is true 
  /// with `GFX_CULL_COUNTER_CLOCKWISE`.
  /// 
  /// @NOTE: The default value is `GFX_CULL_CLOCKWISE`.
  GfxCullOrder front_face = GFX_ORDER_CLOCKWISE; 
};
/// GfxCullDesc 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxContextDesc 
struct GfxContextDesc {
  /// A reference to the window.
  /// 
  /// @NOTE: This _must_ be set to a valid value.
  Window* window                = nullptr;

  /// A bitwise ORed value from `GfxStates` determining the 
  /// states to create/enable.
  /// 
  /// @NOTE: By default, no states are set. 
  u32 states                    = 0;

  /// When set to `true`, the context will enable vsync. 
  /// Otherwise, vsync will be turned off.
  ///
  /// @NOTE: By default, this value is set to `false`.
  bool has_vsync                = false;

  /// The subsamples of the MSAA buffer. 
  /// 
  /// @NOTE: By default, this is set to `1`.
  u32 msaa_samples              = 1;

  /// The description of the depth state. 
  /// 
  /// @NOTE: Check `GfxDepthDesc` to know the default values
  /// of each member.
  GfxDepthDesc depth_desc       = {}; 

  /// The description of the stencil state. 
  /// 
  /// @NOTE: Check `GfxStencilDesc` to know the default values
  /// of each member.
  GfxStencilDesc stencil_desc   = {};

  /// The description of the blend state. 
  /// 
  /// @NOTE: Check `GfxBlendDesc` to know the default values
  /// of each member.
  GfxBlendDesc blend_desc       = {};

  /// The description of the cull state. 
  /// 
  /// @NOTE: Check `GfxCullDesc` to know the default values
  /// of each member.
  GfxCullDesc cull_desc         = {};
};
/// GfxContextDesc 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxFramebufferDesc
struct GfxFramebufferDesc {
  /// A bitwise ORed value `GfxClearFlags` that determine 
  /// which buffers to clear every frame. 
  u32 clear_flags = 0;

  /// An array of color attachments up to `FRAMEBUFFER_ATTACHMENTS_MAX`. 
  GfxTexture* color_attachments[FRAMEBUFFER_ATTACHMENTS_MAX];

  /// The amount of color attachments in the `attachments` array.
  sizei attachments_count = 0;

  /// A pointer to the depth attachment texture. 
  ///
  /// @NOTE: If this texture was created using the `GFX_TEXTURE_DEPTH_STENCIL_TARGET` 
  /// texture type, then this texture will represent _both_ the depth and stencil attachments.
  GfxTexture* depth_attachment   = nullptr;
  
  /// A pointer to the stencil attachment texture. 
  GfxTexture* stencil_attachment = nullptr;
};
/// GfxFramebufferDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBufferDesc
struct GfxBufferDesc {
  /// The data that will be sent to the GPU.
  void* data = nullptr; 

  /// The size of `data` in bytes.
  sizei size;
  
  /// Notify the type of the buffer to the GPU.
  GfxBufferType type;  

  /// Set up how the buffer will be used depending on the usage. 
  GfxBufferUsage usage;
};
/// GfxBufferDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxShaderDesc
struct GfxShaderDesc {
  /// The full source code for the vertex shader. 
  const i8* vertex_source  = nullptr;

  /// The full source code for the pixel/fragment shader. 
  const i8* pixel_source   = nullptr;

  /// The full source code for the compute shader.
  ///
  /// @NOTE: If this is a valid string, the other 
  /// two strings will not be evaluated. The shader 
  /// will be known as a compute shader moving on, 
  /// and can be used with the `gfx_context_dispatch` function.
  const i8* compute_source = nullptr;
};
/// GfxShaderDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxVertexLayout
struct GfxVertexLayout {
  /// An attributes array up to `VERTEX_ATTRIBUTES_MAX`, indicating 
  /// the type of each attribute in the layout.
  GfxLayoutType attributes[VERTEX_ATTRIBUTES_MAX]; 

  /// The first attribute the buffer should process.
  /// 
  /// @NOTE: This value is set to `0` by default.
  sizei start_index      = 0;

  /// The amount of active attributes in the `attributes` array.
  ///
  /// @NOTE: The buffer will process attributes from starting from `start_index` 
  /// all the till `start_index + attributes_count`.
  sizei attributes_count = 0;

  /// If this value is set to `0`, the layout will 
  /// be sent immediately to the shader. However, 
  /// if it is set to a value >= `1`, the layout 
  /// will be sent after the nth instance. 
  u32 instance_rate      = 0;
};
/// GfxVertexLayout
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxUniformDesc
struct GfxUniformDesc {
  char name[UNIFORM_NAME_LENGTH_MAX];
  GfxUniformType type;

  i32 location        = 0;
  u32 component_count = 0;
};
/// GfxUniformDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxShaderQueryDesc
struct GfxShaderQueryDesc {
  // Active attributes

  GfxUniformType active_attributes[VERTEX_ATTRIBUTES_MAX];
  i32 attributes_count = 0;

  // Active uniforms
 
  GfxUniformDesc active_uniforms[UNIFORMS_MAX];
  i32 uniforms_count = 0;

  // Active uniform blocks

  i32 active_uniform_blocks[UNIFORM_BUFFERS_MAX];
  i32 uniform_blocks_count = 0;

  // Active work group size

  i32 work_group_x, work_group_y, work_group_z;
};
/// GfxShaderQueryDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTextureDesc
struct GfxTextureDesc {
  /// The overall size of the texture.
  u32 width, height; 
  
  /// The depth on the Z-axis of the texture. 
  ///
  /// If the texture `type` is anything other than `GFX_TEXTURE_3D`, 
  /// the `depth` member will be ignored.
  u32 depth = 0;

  /// The mipmap level of the texture. 
  ///
  /// @NOTE: Leave this as `1` if the mipmap levels are not important.
  u32 mips  = 1; 

  /// The type of the texture to be used.
  GfxTextureType type;

  /// The pixel format of the texture.
  GfxTextureFormat format;

  /// The filter to be used on the texture when magnified or minified.
  ///
  /// @NOTE: The default filter is set to `GFX_TEXTURE_FILTER_MIN_MAG_LINEAR`.
  GfxTextureFilter filter     = GFX_TEXTURE_FILTER_MIN_MAG_LINEAR;

  /// The addressing mode of the texture.
  ///
  /// @NOTE: The default wrap mode is set to `GFX_TEXTURE_WRAP_REPEAT`.
  GfxTextureWrap wrap_mode    = GFX_TEXTURE_WRAP_REPEAT;

  /// The access mode of the texture.
  /// By default, this value is set to `GFX_TEXTURE_ACCESS_WRITE`.
  ///
  /// @NOTE: This flag will only be taken into account 
  /// with the `gfx_texture_use_as_image` function.
  GfxTextureAccess access     = GFX_TEXTURE_ACCESS_WRITE;

  /// The comparison function to carry out 
  /// if the given `format` is any of the `GFX_TEXTURE_FORMAT_DEPTH_*` variants. 
  ///
  /// @NOTE: By default, this is set to `GFX_COMPARE_ALWAYS`.
  GfxCompareFunc compare_func = GFX_COMPARE_ALWAYS;
 
  /// If the `wrap_mode` member is set to `GFX_TEXTURE_WRAP_BORDER_COLOR`, 
  /// this member will decide said border color. 
  ///
  /// @NOTE: By default, this value is set to `{0.0f, 0.0f, 0.0f, 0.0f}`. 
  f32 border_color[4] = {0.0f, 0.0f, 0.0f, 0.0f};

  /// When this flag is on (which it is by default), 
  /// this texture can be used in bindless operations. 
  ///
  /// If the texture was created in a non-bindless way, 
  /// you _must_ destroy it and re-create it with this 
  /// flag on, in order to enable bindless capabilities.
  bool is_bindless    = true;

  /// The pixels that will be sent to the GPU.
  void* data          = nullptr;
}; 
/// GfxTextureDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxCubemapDesc
struct GfxCubemapDesc {
  /// The overall size of the cubemap.
  u32 width, height; 

  /// The mipmap level of the cubemap. 
  ///
  /// @NOTE: Leave this as `0` if the depth is not important.
  u32 mips; 

  /// The pixel format of the cubemap.
  GfxTextureFormat format;

  /// The filter to be used on the cubemap when magnified or minified.
  ///
  /// @NOTE: The default filter is set to `GFX_TEXTURE_FILTER_MIN_MAG_LINEAR`.
  GfxTextureFilter filter  = GFX_TEXTURE_FILTER_MIN_MAG_LINEAR;

  /// The addressing mode of the cubemap.
  ///
  /// @NOTE: The default wrap mode is set to `GFX_TEXTURE_WRAP_REPEAT`.
  GfxTextureWrap wrap_mode = GFX_TEXTURE_WRAP_REPEAT;
  
  /// An array of pixels (up to `CUBEMAP_FACES_MAX`) of each face of the cubemap.
  void* data[CUBEMAP_FACES_MAX]; 

  /// The amount of faces of the cubemap to use in `data`.
  sizei faces_count = 0;
};
/// GfxCubemapDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBindingDesc
struct GfxBindingDesc {
  /// The shader to bind during the binding operation.
  GfxShader* shader = nullptr; 

  /// An array of textures of `textures_count`.
  ///
  /// @NOTE: The `textures_count` member MUST be < `TEXTURES_MAX`.
  
  GfxTexture** textures = nullptr;
  sizei textures_count  = 0;

  /// An array of images of `images_count`.
  ///
  /// @NOTE: The `images_count` member MUST be < `TEXTURES_MAX`.
  
  GfxTexture** images = nullptr;
  sizei images_count  = 0;
  
  /// An array of buffers of `buffers_count`.
  
  GfxBuffer** buffers = nullptr;
  sizei buffers_count = 0;

  /// An array of cubemaps of `cubemaps_count`.
  ///
  /// @NOTE: The `cubemaps_count` member MUST be < `CUBEMAPS_MAX`.
  
  GfxCubemap** cubemaps = nullptr;
  sizei cubemaps_count  = 0;
};
/// GfxBindingDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxPipelineDesc
struct GfxPipelineDesc {
  /// The vertex buffer to be used in a `gfx_context_draw` command.
  ///
  /// @NOTE: This buffer _must_ be set. It cannot be left a `nullptr`.
  /// Even if `gfx_context_draw` is being used.
  GfxBuffer* vertex_buffer   = nullptr; 

  /// The amount of vertices in the `vertex_buffer` to be drawn. 
  sizei vertices_count       = 0;
  
  /// The index buffer to be used in a `gfx_context_draw` command.
  GfxBuffer* index_buffer    = nullptr;

  /// The amount of indices in the `index_buffer` to be drawn.
  sizei indices_count        = 0;
  
  /// The instance buffer to be used in a `gfx_context_draw_instanced` command.
  ///
  /// @NOTE: This buffer _must_ be set if `gfx_context_draw_instanced` 
  /// is to be called.
  GfxBuffer* instance_buffer = nullptr; 

  /// The amount of vertices in the `vertex_buffer` to be drawn. 
  sizei instance_count       = 0;
 
  /// The type of each index in the `index_buffer`. 
  /// 
  /// @NOTE: The default is set to `GFX_LAYOUT_UINT1`.
  GfxLayoutType indices_type = GFX_LAYOUT_UINT1;

  /// Layout array up to `VERTEX_LAYOUTS_MAX` describing each layout attribute.
  ///
  /// @NOTE: The layout types `GFX_LAYOUT_MAT2`, `GFX_LAYOUT_MAT3`, and `GFX_LAYOUT_MAT4` 
  /// are not supported currently.
  GfxVertexLayout layouts[VERTEX_LAYOUTS_MAX];

  /// The draw mode of the entire pipeline.
  ///
  /// @NOTE: This can be changed at anytime before the draw command.
  GfxDrawMode draw_mode;
 
  /// A flag to indicate if the pipeline can 
  /// or cannot write to the depth buffer. 
  ///
  /// @NOTE: By default, this value is `true`.
  bool depth_mask    = true;

  /// The stencil reference value of the pipeline. 
  ///
  /// @NOTE: This is `1` by default.
  u32 stencil_ref    = 1;
  
  /// The blend factor to be used in the pipeline. 
  ///
  /// @NOTE: This is `{0, 0, 0, 0}` by default.
  f32 blend_factor[4] = {0, 0, 0, 0};
};
/// GfxPipelineDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxDrawCommandIndirect
struct GfxDrawCommandIndirect {
  /// The number of primitives (vertices or indices) to be drawn.
  u32 elements_count = 0; 

  /// The number of instances to draw. 
  ///
  /// @NOTE: This is usually set to `1` to render only 
  /// _one_ instance of a mesh.
  u32 instance_count = 1;

  /// The first element to be considered when drawing. 
  ///
  /// @NOTE: This is usually set to `0`.
  u32 first_element  = 0; 

  /// The base vertex to start drawing from. 
  ///
  /// @NOTE: This is used to offset into a large 
  /// vertex/index array.
  i32 base_vertex    = 0;

  /// The base index of the vertex attribute to 
  /// be used when using this command for an instanced 
  /// draw call. 
  ///
  /// @NOTE: For non-instanced draw calls, this should 
  /// be left as `0`.
  u32 base_instance  = 0;
};
/// GfxDrawCommandIndirect
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Context functions 

/// Initialize the `GfxContext` struct and return it. 
/// This function will handle the initialization of the underlying graphics API.
/// Setting up any states, creating the graphics devices, setting the pixel format of the swapchain, and so on.
///  
/// Any information describing the many states will be taken from `desc`.
/// 
/// If any errors occur during initialization, this function will return a `nullptr`.
/// 
/// Check out the `GfxContextDesc` struct for more details on its members 
/// that will be passed into this function. All of the members have default values. 
/// However, two members _have_ to be set. Mainly, `window` and `states`.
/// 
/// The `window` is a pointer to the previously created `Window` struct.
/// The `states` flags of type `GfxStates` can be `OR`ed together which will 
/// indicate the initial active states of the context.
/// 
/// This function will also save a reference internally to `desc`.
/// 
/// @NOTE: Later on, with any function, if an instance of `GfxContext` 
/// is passed as a `nullptr`, the function will assert. 
NIKOLA_API GfxContext* gfx_context_init(const GfxContextDesc& desc);

/// Free/reclaim any memory `gfx` has consumed. 
/// This function will do any required de-initialization of the graphics API.
NIKOLA_API void gfx_context_shutdown(GfxContext* gfx);

/// Retrieve the internal `GfxContextDesc` of `gfx`
NIKOLA_API GfxContextDesc& gfx_context_get_desc(GfxContext* gfx);

/// Check whether the given `ext` extension is supported in the current enviornment.
NIKOLA_API bool gfx_context_has_extension(GfxContext* gfx, const char* ext); 

/// Set any `state` of the context `gfx` to `value`. 
/// i.e, this function can turn on or off the `state` in the given `gfx` context.
NIKOLA_API void gfx_context_set_state(GfxContext* gfx, const GfxStates state, const bool value);

/// Set the information of the Depth state of the pipeline to the new `depth_desc`.
/// 
/// @NOTE: The new changes will only take effect if the depth state is enabled.
NIKOLA_API void gfx_context_set_depth_state(GfxContext* gfx, const GfxDepthDesc& depth_desc);

/// Set the information of the Stencil state of the pipeline to the new `stencil_desc`.
/// 
/// @NOTE: The new changes will only take effect if the stencil state is enabled.
NIKOLA_API void gfx_context_set_stencil_state(GfxContext* gfx, const GfxStencilDesc& stencil_desc);

/// Set the information of the Cull state of the pipeline to the new `cull_desc`.
/// 
/// @NOTE: The new changes will only take effect if the cull state is enabled.
NIKOLA_API void gfx_context_set_cull_state(GfxContext* gfx, const GfxCullDesc& cull_desc);

/// Set the information of the Blend state of the pipeline to the new `blend_desc`.
/// 
/// @NOTE: The new changes will only take effect if the blend state is enabled.
NIKOLA_API void gfx_context_set_blend_state(GfxContext* gfx, const GfxBlendDesc& blend_desc);

/// Set the scissor rectangle of `gfx` to the given `x`, `y`, `width`, `height`
/// 
/// @NOTE: The new changes will only take effect if the scissor state is enabled.
NIKOLA_API void gfx_context_set_scissor_rect(GfxContext* gfx, const i32 x, const i32 y, const i32 width, const i32 height);

/// Set the viewport of `gfx` to the given `x`, `y`, `width`, `height`
NIKOLA_API void gfx_context_set_viewport(GfxContext* gfx, const i32 x, const i32 y, const i32 width, const i32 height);

/// Set the context's current render target to `framebuffer`. The clear flags 
/// as well as any attachments will be sampled from `framebuffer`. However, 
/// if `framebuffer` is set to `nullptr`, the render target will be the default framebuffer instead.
NIKOLA_API void gfx_context_set_target(GfxContext* gfx, GfxFramebuffer* framebuffer);

/// Clear buffers of `gfx` as well as the color to `r`, `g`, `b`, `a`.
///
/// @NOTE: The clear flags for the default render target will depend on the current 
/// active states of `gfx`. Otherwise, `gfx` will inherit the clear flags of the currently 
/// active render target (i.e framebuffer).
NIKOLA_API void gfx_context_clear(GfxContext* gfx, const f32 r, const f32 g, const f32 b, const f32 a);

/// Use/activate all of the resources in `binding_desc` for the next draw call in the given `gfx` context.
NIKOLA_API void gfx_context_use_bindings(GfxContext* gfx, const GfxBindingDesc& binding_desc);

/// Use/activate the given `pipeline` for the next draw call in the given `gfx` context.
NIKOLA_API void gfx_context_use_pipeline(GfxContext* gfx, GfxPipeline* pipeline);

/// Draw the currently bound `GfxPipeline` object of `gfx`, starting at `start_element`, drawing 
/// either `GfxPipeline.vertices_count` or `GfxPipeline.indices_count` amount of elements and/or vertices, 
/// depending on which buffer is active.
///
/// @NOTE: If the currently bound `GfxPipeline` object only has one valid `vertex_buffer`, then 
/// this draw call will use the vertex buffer. Otherwise, the index buffer will be used. 
NIKOLA_API void gfx_context_draw(GfxContext* gfx, const u32 start_element);

/// Equivalent to `gfx_context_draw` but uses instancing, using the `GfxPipeline.instance_count` memeber.
///
/// @NOTE: If the given `instance_count` is == 1, the function will STILL use instancing.
///
/// @NOTE: This function will assert if `GfxPipeline.instance_buffer` is set to `nullptr`.
/// An instance buffer _must_ be created before this function.
NIKOLA_API void gfx_context_draw_instanced(GfxContext* gfx, const u32 start_element);

/// Draw the currently bound `GfxPipeline` object of `gfx` as an indirect call, using a buffer with a 
/// type of `GFX_BUFFER_DRAW_INDIRECT` populated with `GfxDrawCommandIndirect` objects. 
///
/// The given `offset` will be used to offset into the vertex buffer or the index buffer. 
/// The given `count` signifies the number of `GfxDrawCommandIndirect` objects in the buffer. 
/// The given `stride` represents the distance between each `GfxDrawCommandIndirect` object in the buffer. 
/// By default, the `stride` parametar is set to `0` to provide a packed buffer of draw commands.
///
/// @NOTE: If there is no buffer of type `GFX_BUFFER_DRAW_INDIRECT` created, you will get an OpenGL error. 
NIKOLA_API void gfx_context_draw_multi_indirect(GfxContext* gfx, const u32 offset, const sizei count, const sizei stride = 0);

/// Dispatch the currently active compute shader in `work_group_x`, `work_group_y`, and `work_group_z`. 
/// Before calling this function, a previous call to `gfx_shader_use` MUST be made with the appropriate 
/// compute shader to be used.
///
/// @NOTE: The `work_group_*` parametars should NEVER be < 1 or > `MAX_COMPUTE_WORK_GROUPS_COUNT`.
NIKOLA_API void gfx_context_dispatch(GfxContext* gfx, const u32 work_group_x, const u32 work_group_y, const u32 work_group_z); 

/// Apply a memory barrier to the given `gfx` context, using the bitwise fields in `barrier_bits`.
/// Use the `GfxMemoryBarrierType` to set `barrier_bits`.
NIKOLA_API void gfx_context_memory_barrier(GfxContext* gfx, const i32 barrier_bits); 

/// Switch to the back buffer or, rather, present the back buffer to the screen. 
/// 
/// @NOTE: This function will be affected by vsync. 
NIKOLA_API void gfx_context_present(GfxContext* gfx);

/// Context functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Framebuffer functions

/// Allocate using the `alloc_fn` callback and return a `GfxFramebuffer` object, using the information in `desc`. 
///
/// @NOTE: The `alloc_fn` uses the default memory allocater.
NIKOLA_API GfxFramebuffer* gfx_framebuffer_create(GfxContext* gfx, const GfxFramebufferDesc& desc, const AllocateMemoryFn& alloc_fn = memory_allocate);

/// Free/reclaim any memory taken by `framebuffer` using the `free_fn` callback.
///
/// @NOTE: The `free_fn` uses the default memory allocater.
NIKOLA_API void gfx_framebuffer_destroy(GfxFramebuffer* framebuffer, const FreeMemoryFn& free_fn = memory_free);

/// Copy the contents of the buffer associated with `buffer_mask` (which is an ORable flag from the `GfxClearFlags` enum) 
/// of the `src_frame` (confined by `src_x`, `src_y`, `src_width`, and `src_height`) framebuffer into the `dest_frame` 
/// (confined by `dest_x`, `dest_y`, `dest_width`, and `dest_height`) framebuffer. 
///
/// @NOTE: If either `src_frame` or `dest_frame` is a `nullptr`, the contents are going to be copied 
/// from or to the default framebuffer. 
///
/// @NOTE: If BOTH `src_frame` and `dest_frame` are set to `nullptr`
NIKOLA_API void gfx_framebuffer_copy(const GfxFramebuffer* src_frame, 
                                     GfxFramebuffer* dest_frame, 
                                     i32 src_x, i32 src_y, 
                                     i32 src_width, i32 src_height, 
                                     i32 dest_x, i32 dest_y, 
                                     i32 dest_width, i32 dest_height, 
                                     i32 buffer_mask);

/// Retrieve the internal `GfxFramebufferDesc` of `framebuffer`
NIKOLA_API GfxFramebufferDesc& gfx_framebuffer_get_desc(GfxFramebuffer* framebuffer);

/// Update the information of `framebuffer` by resetting the clear flags as well as reattaching the 
/// render targets in `desc`.
NIKOLA_API void gfx_framebuffer_update(GfxFramebuffer* framebuffer, const GfxFramebufferDesc& desc);

/// Framebuffer functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Buffer functions 

/// Allocate using the `alloc_fn` callback and return a `GfxBuffer` object.
///
/// @NOTE: The `alloc_fn` uses the default memory allocater.
NIKOLA_API GfxBuffer* gfx_buffer_create(GfxContext* gfx, const AllocateMemoryFn& alloc_fn = memory_allocate);

/// Load data into the given `buffer` object, usin the information provided in `desc`.
///
/// If the function fails for whatever reason, `false` will be returned. Otherwise, 
/// the function will return `true`.
NIKOLA_API const bool gfx_buffer_load(GfxBuffer* buffer, const GfxBufferDesc& desc);

/// Free/reclaim any memory taken by `buff` using the `free_fn` callback.
///
/// @NOTE: The `free_fn` uses the default memory allocater.
NIKOLA_API void gfx_buffer_destroy(GfxBuffer* buff, const FreeMemoryFn& free_fn = memory_free);

/// Retrieve the internal `GfxBufferDesc` of `buffer`
NIKOLA_API GfxBufferDesc& gfx_buffer_get_desc(GfxBuffer* buffer);

/// Update the internal `GfxBufferDesc` of `buff` to the given `desc`.
NIKOLA_API void gfx_buffer_update(GfxBuffer* buff, const GfxBufferDesc& desc);

/// Update the contents of `buff` starting at `offset` with `data` of size `size`.
/// 
/// @NOTE: If the `offset + size` is > `GfxBuffer.size`, this function will assert.
NIKOLA_API void gfx_buffer_upload_data(GfxBuffer* buff, const sizei offset, const sizei size, const void* data);

/// Buffer functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Shader functions 

/// Allocate using the `alloc_fn` callback and return a `GfxShader` object.
///
/// @NOTE: The `alloc_fn` uses the default memory allocater.
NIKOLA_API GfxShader* gfx_shader_create(GfxContext* gfx, const AllocateMemoryFn& alloc_fn = memory_allocate);

/// Load data into the given `shader` object, usin the information provided in `desc`.
///
/// If the function fails for whatever reason, `false` will be returned. Otherwise, 
/// the function will return `true`.
NIKOLA_API const bool gfx_shader_load(GfxShader* shader, const GfxShaderDesc& desc);

/// Free/reclaim any memory taken by `shader` using the `free_fn` callback.
///
/// @NOTE: The `free_fn` uses the default memory allocater.
NIKOLA_API void gfx_shader_destroy(GfxShader* shader, const FreeMemoryFn& free_fn = memory_free);

/// Retrieve the internal `GfxShaderDesc` of `shader`.
NIKOLA_API GfxShaderDesc& gfx_shader_get_source(GfxShader* shader);

/// Update the `shader`'s information from the given `desc`.
NIKOLA_API void gfx_shader_update(GfxShader* shader, const GfxShaderDesc& desc);

/// Query the shader for its information and store it in the given `out_desc` structure.
///
/// @NOTE: If the given `shader` has not been correctly linked, 
/// this function will raise an error.
NIKOLA_API void gfx_shader_query(GfxShader* shader, GfxShaderQueryDesc* out_desc);

/// Attaches the uniform `buffer` to the `shader` of type `type` to point `bind_point`. 
/// Any updates to `buffer` will have an effect on the `shader`. Besides that, the 
/// given `buffer` MUST be of type `GFX_BUFFER_UNIFORM`. Otherwise, this function 
/// will assert.
/// 
/// @NOTE: For GLSL (OpenGL), you _need_ to specify the binding point of the uniform buffer in the shader itself. For example, 
/// do something like, `layout (std140, binding = 0)`. Now the uniform buffer will be bound to the point `0` and the shader 
/// can easily find it. 
NIKOLA_API void gfx_shader_attach_uniform(GfxShader* shader, const GfxShaderType type, GfxBuffer* buffer, const u32 bind_point);

/// Lookup the `uniform_name` in `shader` and retrieve its location. 
///
/// @NOTE: If `uniform_name` does NOT exist in `shader`, the returned value will be `-1`.
NIKOLA_API i32 gfx_shader_uniform_lookup(GfxShader* shader, const i8* uniform_name);

/// Upload a uniform array with `count` elements of type `type` with `data` at `location` to `shader`. 
NIKOLA_API void gfx_shader_upload_uniform_array(GfxShader* shader, const i32 location, const sizei count, const GfxLayoutType type, const void* data);

/// Upload a uniform of type `type` with `data` at `location` to `shader`. 
NIKOLA_API void gfx_shader_upload_uniform(GfxShader* shader, const i32 location, const GfxLayoutType type, const void* data);

/// Shader functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Texture functions 

/// Allocate using the `alloc_fn` callback and return a `GfxTexture` object of type `tex_type`.
///
/// @NOTE: The `alloc_fn` uses the default memory allocater.
NIKOLA_API GfxTexture* gfx_texture_create(GfxContext* gfx, const GfxTextureType tex_type, const AllocateMemoryFn& alloc_fn = memory_allocate);

/// Load data into the given `texture` object, usin the information provided in `desc`.
///
/// If the function fails for whatever reason, `false` will be returned. Otherwise, 
/// the function will return `true`.
NIKOLA_API const bool gfx_texture_load(GfxTexture* texture, const GfxTextureDesc& desc);

/// Free/reclaim any memory taken by `texture` using the `free_fn` callback.
///
/// @NOTE: The `free_fn` uses the default memory allocater.
NIKOLA_API void gfx_texture_destroy(GfxTexture* texture, const FreeMemoryFn& free_fn = memory_free);

/// Retrieve the internal `GfxTextureDesc` of `texture`
NIKOLA_API GfxTextureDesc& gfx_texture_get_desc(GfxTexture* texture);

/// Retrieve the internal bindless ID of `texture`.
NIKOLA_API const u64 gfx_texture_get_bindless_id(GfxTexture* texture);

/// Update the `texture`'s information from the given `desc`.
///
/// @NOTE: This functions will NOT resend the pixels in `desc`.
NIKOLA_API void gfx_texture_update(GfxTexture* texture, const GfxTextureDesc& desc);

/// Reupload the data of `texture`, using the given `width`, `height`, `depth`, and `data`.
/// The `depth` parametar can be left as `0` for any non-3D textures.
///
/// @NOTE: The internal `GfxTextureDesc` of `texture` will be used to supply information 
/// about the `data`.
NIKOLA_API void gfx_texture_upload_data(GfxTexture* texture, 
                                        const i32 width, const i32 height, const i32 depth, 
                                        const void* data);

/// Texture functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Cubemap functions 

/// Allocate using the `alloc_fn` callback and return a `GfxCubemap` object.
///
/// @NOTE: The `alloc_fn` uses the default memory allocater.
NIKOLA_API GfxCubemap* gfx_cubemap_create(GfxContext* gfx, const AllocateMemoryFn& alloc_fn = memory_allocate);

/// Load data into the given `cubemap` object, usin the information provided in `desc`.
///
/// If the function fails for whatever reason, `false` will be returned. Otherwise, 
/// the function will return `true`.
NIKOLA_API const bool gfx_cubemap_load(GfxCubemap* cubemap, const GfxCubemapDesc& desc);

/// Free/reclaim any memory taken by `cubemap` using the `free_fn` callback.
///
/// @NOTE: The `free_fn` uses the default memory allocater.
NIKOLA_API void gfx_cubemap_destroy(GfxCubemap* cubemap, const FreeMemoryFn& free_fn = memory_free);

/// Retrieve the internal `GfxCubemapDesc` of `cubemap`
NIKOLA_API GfxCubemapDesc& gfx_cubemap_get_desc(GfxCubemap* cubemap);

/// Update the `cubemap`'s information from the given `desc`.
///
/// @NOTE: This functions will NOT resend the pixels in `desc`.
NIKOLA_API void gfx_cubemap_update(GfxCubemap* cubemap, const GfxCubemapDesc& desc);

/// Reupload `count` amount of of `cubemap`, using the given `width`, `height`, and `faces`.
///
/// @NOTE: The internal `GfxCubemapDesc` of `cubemap` will be used to supply information 
/// about the `data`.
///
/// @NOTE: The given `count` CANNOT exceed `CUBEMAPS_MAX`.
NIKOLA_API void gfx_cubemap_upload_data(GfxCubemap* cubemap, 
                                        const i32 width, const i32 height,
                                        const void** faces, const sizei count);

/// Cubemap functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Pipeline functions 

/// Allocate using the `alloc_fn` callback and return a `GfxPipeline` object, using the information in `desc`. 
///
/// @NOTE: The `alloc_fn` uses the default memory allocater.
NIKOLA_API GfxPipeline* gfx_pipeline_create(GfxContext* gfx, const GfxPipelineDesc& desc, const AllocateMemoryFn& alloc_fn = memory_allocate);

/// Free/reclaim any memory taken by `pipeline` using the `free_fn` callback.
///
/// @NOTE: The `free_fn` uses the default memory allocater.
NIKOLA_API void gfx_pipeline_destroy(GfxPipeline* pipeline, const FreeMemoryFn& free_fn = memory_free);

/// Update the `pipeline`'s information from the given `desc`.
NIKOLA_API void gfx_pipeline_update(GfxPipeline* pipeline, const GfxPipelineDesc& desc);

/// Retrieve the internal `GfxPipelineDesc` of `pipeline`
NIKOLA_API GfxPipelineDesc& gfx_pipeline_get_desc(GfxPipeline* pipeline);

/// Pipeline functions 
///---------------------------------------------------------------------------------------------------------------------

/// *** Graphics ***
/// ---------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
