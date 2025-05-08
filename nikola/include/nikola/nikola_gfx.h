#pragma once

#include "nikola_base.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ---------------------------------------------------------------------
/// *** Graphics ***

///---------------------------------------------------------------------------------------------------------------------
// Consts

/// The maximum amount of textures the GPU supports at a time. 
const sizei TEXTURES_MAX                = 32;

/// The maximum amount of attachments that can be attached to a framebuffer. 
const sizei FRAMEBUFFER_ATTACHMENTS_MAX = 8;

/// The maximum amount of textures the GPU supports at a time. 
const sizei CUBEMAPS_MAX                = 5;

/// The maximum amount of faces in a cubemap
const sizei CUBEMAP_FACES_MAX           = 6;

/// The maximum amount of uniform buffers to be created in a shader type.
const sizei UNIFORM_BUFFERS_MAX         = 16;

/// The maximum number of elements a buffer's layout can have.
const sizei LAYOUT_ELEMENTS_MAX         = 32;

/// The maximum number of render targets to be bound at once.
const sizei RENDER_TARGETS_MAX          = 8;

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
};
/// GfxStates
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxCompareFunc
enum GfxCompareFunc {
  /// Always passes the comparison.
  GFX_COMPARE_ALWAYS        = 3 << 0,

  /// Never passes the comparison.
  GFX_COMPARE_NEVER         = 3 << 1,

  /// Passes the comparison when the `a < b`.
  GFX_COMPARE_LESS          = 3 << 2, 

  /// Passes the comparison when the `a <= b`.
  GFX_COMPARE_LESS_EQUAL    = 3 << 3, 

  /// Passes the comparison when the `a > b`.
  GFX_COMPARE_GREATER       = 3 << 4, 

  /// Passes the comparison when the `a >= b`.
  GFX_COMPARE_GREATER_EQUAL = 3 << 5, 

  /// Passes the comparison when the `a != b`.
  GFX_COMPARE_NOT_EQUAL     = 3 << 6,
};
/// GfxCompareFunc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxOperation 
enum GfxOperation {
  /// Always keep the value.
  GFX_OP_KEEP      = 4 << 0, 
  
  /// Always set the value to `0`.
  GFX_OP_ZERO      = 4 << 1, 

  /// Invert the value.
  GFX_OP_INVERT    = 4 << 2, 

  /// Replace the value `a` with the other value `b`.
  GFX_OP_REPLACE   = 4 << 3, 

  /// Increase the value by `1`.
  GFX_OP_INCR      = 4 << 4, 

  /// Decrease the value by `1`.
  GFX_OP_DECR      = 4 << 5, 

  /// Increase the value by `1` and wrap to the beginning when it reaches the maximum.
  GFX_OP_INCR_WRAP = 4 << 6, 

  /// Decrease the value by `1` and wrap to the end when it reaches the minimum.
  GFX_OP_DECR_WRAP = 4 << 7, 
};
/// GfxOperation 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBlendMode 
enum GfxBlendMode {
  /// Always keep the blended value at `0`.
  GFX_BLEND_ZERO               = 5 << 0,
  
  /// Always keep the blended value at `1`.
  GFX_BLEND_ONE                = 5 << 1, 
  
  /// Take the source's color as the blended value.
  GFX_BLEND_SRC_COLOR          = 5 << 2,
  
  /// Take the destination's color as the blended value.
  GFX_BLEND_DEST_COLOR         = 5 << 3, 
  
  /// Take the source's alpha value as the blended value.
  GFX_BLEND_SRC_ALPHA          = 5 << 4, 
  
  /// Take the destination's alpha value as the blended value.
  GFX_BLEND_DEST_ALPHA         = 5 << 5, 
  
  /// Take the inverse of the source's color as the blended value.
  GFX_BLEND_INV_SRC_COLOR      = 5 << 6, 
  
  /// Take the inverse of the destination's color as the blended value.
  GFX_BLEND_INV_DEST_COLOR     = 5 << 7, 
  
  /// Take the inverse of the source's alpha value as the blended value.
  GFX_BLEND_INV_SRC_ALPHA      = 5 << 8, 
  
  /// Take the inverse of the destination's alpha value as the blended value.
  GFX_BLEND_INV_DEST_ALPHA     = 5 << 9,
  
  /// Take the saturated alpha value of the source's color as the blended value.
  GFX_BLEND_SRC_ALPHA_SATURATE = 5 << 10,
};
/// GfxBlendMode 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxCullMode
enum GfxCullMode {
  /// Only cull the front faces.
  GFX_CULL_FRONT          = 6 << 0,
  
  /// Only cull the back faces.
  GFX_CULL_BACK           = 6 << 1,
  
  /// Cull both the front and back faces.
  GFX_CULL_FRONT_AND_BACK = 6 << 2,
};
/// GfxCullMode
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxCullOrder
enum GfxCullOrder {
  /// Clockwise vertices are the front faces.
  GFX_ORDER_CLOCKWISE         = 7 << 0, 
  
  /// Counter-clockwise vertices are the front faces.
  GFX_ORDER_COUNTER_CLOCKWISE = 7 << 1,
};
/// GfxCullOrder
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
enum GfxClearFlags {
  /// No buffers will be cleared.
  GFX_CLEAR_FLAGS_NONE                   = 3 << 0,
  
  /// Clear the color buffer of the current context. 
  GFX_CLEAR_FLAGS_COLOR_BUFFER     = 3 << 1,

  /// Clear the depth buffer of the current context. 
  ///
  /// @NOTE: This flag will be ignored if the depth state is disabled.
  GFX_CLEAR_FLAGS_DEPTH_BUFFER     = 3 << 2,
  
  /// Clear the stencil buffer of the current context. 
  ///
  /// @NOTE: This flag will be ignored if the stencil state is disabled.
  GFX_CLEAR_FLAGS_STENCIL_BUFFER   = 3 << 3,
};
/// GfxContextFlags 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBufferType
enum GfxBufferType {
  /// A vertex buffer.
  GFX_BUFFER_VERTEX  = 4 << 0, 

  /// An index buffer.
  GFX_BUFFER_INDEX   = 4 << 1, 

  /// A uniform buffer.
  GFX_BUFFER_UNIFORM = 4 << 2,
};
/// GfxBufferType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxBufferUsage
enum GfxBufferUsage {
  /// Set the buffer to be dynamically written to.
  /// This will be used for frequently writing to the buffer.
  GFX_BUFFER_USAGE_DYNAMIC_DRAW = 5 << 0,
  
  /// Set the buffer to be dynamically read from.
  /// This will be used for frequent reading from the buffer.
  GFX_BUFFER_USAGE_DYNAMIC_READ = 5 << 1,

  /// Set the buffer to be statically written to.
  /// This will be used for writing to the buffer once or rarely.
  GFX_BUFFER_USAGE_STATIC_DRAW  = 5 << 2,

  /// Set the buffer to be statically read from.
  /// This will be used for reading from the buffer once or rarely.
  GFX_BUFFER_USAGE_STATIC_READ  = 5 << 3,
};
/// GfxBufferUsage
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxDrawMode
enum GfxDrawMode {
  /// Will set up the pipeline to draw points.
  GFX_DRAW_MODE_POINT          = 6 << 0,

  /// Will set up the pipeline to draw triangles.
  GFX_DRAW_MODE_TRIANGLE       = 6 << 1,
  
  /// Will set up the pipeline to draw triangle strips.
  GFX_DRAW_MODE_TRIANGLE_STRIP = 6 << 2,
  
  /// Will set up the pipeline to draw lines.
  GFX_DRAW_MODE_LINE           = 6 << 3,
  
  /// Will set up the pipeline to draw line strips.
  GFX_DRAW_MODE_LINE_STRIP     = 6 << 4,
};
/// GfxDrawMode
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxLayoutType
enum GfxLayoutType {
  /// Equivalent to `float`.
  GFX_LAYOUT_FLOAT1 = 7 << 0,
  
  /// Equivalent to `fVector2`.
  GFX_LAYOUT_FLOAT2 = 7 << 1,
  
  /// Equivalent to `fVector3`.
  GFX_LAYOUT_FLOAT3 = 7 << 2,
  
  /// Equivalent to `fVector4`.
  GFX_LAYOUT_FLOAT4 = 7 << 3,
  
  /// Equivalent to `int`.
  GFX_LAYOUT_INT1   = 7 << 4,
  
  /// Equivalent to `iVector2`.
  GFX_LAYOUT_INT2   = 7 << 5,
  
  /// Equivalent to `iVector3`.
  GFX_LAYOUT_INT3   = 7 << 6,
  
  /// Equivalent to `iVector4`.
  GFX_LAYOUT_INT4   = 7 << 7,
  
  /// Equivalent to `unsigned int`.
  GFX_LAYOUT_UINT1  = 7 << 8,
  
  /// Equivalent to `uVector2`.
  GFX_LAYOUT_UINT2  = 7 << 9,
  
  /// Equivalent to `uVector3`.
  GFX_LAYOUT_UINT3  = 7 << 10,
  
  /// Equivalent to `uVector4`.
  GFX_LAYOUT_UINT4  = 7 << 11,

  /// A 2x2 matrix (or 4 `float`s).
  GFX_LAYOUT_MAT2   = 7 << 12,
  
  /// A 3x3 matrix (or 9 `float`s).
  GFX_LAYOUT_MAT3   = 7 << 13,
  
  /// A 4x4 matrix (or 16 `float`s).
  GFX_LAYOUT_MAT4   = 7 << 14,
};
/// GfxLayoutType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTextureType
enum GfxTextureType {
  /// Creates a 1D texture.
  GFX_TEXTURE_1D                   = 8 << 0,
  
  /// Creates a 2D texture.
  GFX_TEXTURE_2D                   = 8 << 1,
  
  /// Creates a 3D texture.
  GFX_TEXTURE_3D                   = 8 << 2,
  
  /// Creates a texture to be used as a render target.
  GFX_TEXTURE_RENDER_TARGET        = 8 << 3,
  
  /// Creates a texture to be used as the depth target.
  GFX_TEXTURE_DEPTH_TARGET         = 8 << 4,
  
  /// Creates a texture to be used as the stencil target.
  GFX_TEXTURE_STENCIL_TARGET       = 8 << 5,
  
  /// Creates a texture to be used as both the depth and stencil target.
  GFX_TEXTURE_DEPTH_STENCIL_TARGET = 8 << 6,
};
/// GfxTextureType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTextureFormat
enum GfxTextureFormat {
  /// An `unsigned char` per pixel red channel texture format.
  GFX_TEXTURE_FORMAT_R8                 = 9 << 0,
  
  /// An `unsigned short` per pixel red channel texture format.
  GFX_TEXTURE_FORMAT_R16                = 9 << 1,
  
  /// A `half float` per pixel red channel texture format.
  GFX_TEXTURE_FORMAT_R16F               = 9 << 2,
  
  /// A `float` per pixel red channel texture format.
  GFX_TEXTURE_FORMAT_R32F               = 9 << 3,

  /// An `unsigned char` per pixel red and green channel texture format.
  GFX_TEXTURE_FORMAT_RG8                = 9 << 4,
  
  /// An `unsigned short` per pixel red and green channel texture format.
  GFX_TEXTURE_FORMAT_RG16               = 9 << 5,
  
  /// A `half float` per pixel red and green channel texture format.
  GFX_TEXTURE_FORMAT_RG16F              = 9 << 6,
  
  /// A `float` per pixel red and green channel texture format.
  GFX_TEXTURE_FORMAT_RG32F              = 9 << 7,
  
  /// An `unsigned char` per pixel red, green, blue, and alpha channel texture format.
  GFX_TEXTURE_FORMAT_RGBA8              = 9 << 8,
  
  /// An `unsigned short` bits per pixel red, green, blue, and alpha channel texture format.
  GFX_TEXTURE_FORMAT_RGBA16             = 9 << 9,
  
  /// A `half float` per pixel red, green, blue, and alpha channel texture format.
  GFX_TEXTURE_FORMAT_RGBA16F            = 9 << 10,
  
  /// A `float` per pixel red, green, blue, and alpha channel texture format.
  GFX_TEXTURE_FORMAT_RGBA32F            = 9 << 11,

  /// A format to be used with the depth and stencil buffers where 
  /// the depth buffer gets 24 bits and the stencil buffer gets 8 bits.
  GFX_TEXTURE_FORMAT_DEPTH_STENCIL_24_8 = 9 << 12,
};
/// GfxTextureFromat
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTextureFilter
enum GfxTextureFilter {
  /// Uses linear filtering in both modes.
  GFX_TEXTURE_FILTER_MIN_MAG_LINEAR            = 10 << 0,
  
  /// Uses nearest filtering on both modes.
  GFX_TEXTURE_FILTER_MIN_MAG_NEAREST           = 10 << 1,
  
  /// Uses linear filtering on minification and nearest filtering magnification. 
  GFX_TEXTURE_FILTER_MIN_LINEAR_MAG_NEAREST    = 10 << 2,
  
  /// Uses nearest filtering on minification and linear filtering magnification. 
  GFX_TEXTURE_FILTER_MIN_NEAREST_MAG_LINEAR    = 10 << 3,
  
  /// Uses trilinear filtering (the weighted average of the two closest mipmaps)
  /// on minification and linear filtering on magnification.
  GFX_TEXTURE_FILTER_MIN_TRILINEAR_MAG_LINEAR  = 10 << 4,
  
  /// Uses trilinear filtering (the weighted average of the two closest mipmaps)
  /// on minification and nearest filtering on magnification.
  GFX_TEXTURE_FILTER_MIN_TRILINEAR_MAG_NEAREST = 10 << 5,
};
/// GfxTextureFilter
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxTextureWrap
enum GfxTextureWrap {
  /// Repeat the pixel when wrapped.
  GFX_TEXTURE_WRAP_REPEAT       = 11 << 0, 
  
  /// Mirror the pixel when wrapped.
  GFX_TEXTURE_WRAP_MIRROR       = 11 << 1, 
  
  /// Clamp the pixel when wrapped.
  GFX_TEXTURE_WRAP_CLAMP        = 11 << 2, 
  
  /// Use the border color when wrapped.
  GFX_TEXTURE_WRAP_BORDER_COLOR = 11 << 3,
};
/// GfxTextureWrap
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxShaderType
enum GfxShaderType {
  /// A vertex shader.
  GFX_SHADER_VERTEX   = 12 << 0, 

  /// A pixel/fragment shader.
  GFX_SHADER_PIXEL    = 12 << 1, 

  /// A geometry shader.
  GFX_SHADER_GEOMETRY = 12 << 2,
};
/// GfxShaderType
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

  /// An array of attachments up to `FRAMEBUFFER_ATTACHMENTS_MAX`. 
  ///
  /// @NOTE: Each texture in this array have one of the texture types `GFX_TEXTURE_*_TARGET`. 
  GfxTexture* attachments[FRAMEBUFFER_ATTACHMENTS_MAX];

  /// The amount of attachments in the `attachments` array.
  sizei attachments_count = 0;
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
  const i8* vertex_source = nullptr;

  /// The full source code for the pixel/fragment shader. 
  const i8* pixel_source  = nullptr;
};
/// GfxShaderDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxLayoutDesc
struct GfxLayoutDesc {
  /// The name of the layout attribute. 
  ///
  /// @NOTE: This can be left blank for OpenGL.
  const i8* name; 

  /// The type of the layout.
  GfxLayoutType type; 

  /// If this value is set to `0`, the layout will 
  /// be sent immediately to the shader. However, 
  /// if it is set to a value >= `1`, the layout 
  /// will be sent after the nth instance. 
  u32 instance_rate = 0;
};
/// GfxLayoutDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxUniformDesc
struct GfxUniformDesc {
  /// The type of the shader the uniform will be 
  /// uploaded to. 
  GfxShaderType shader_type;

  /// The index of the uniform buffer. 
  sizei index; 

  /// The data that will be sent to the shader.
  void* data;

  /// The size of `data`.
  sizei size;
};
/// GfxUniformDesc
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
  u32 depth;

  /// The mipmap level of the texture. 
  ///
  /// @NOTE: Leave this as `1` if the mipmap levels are not important.
  u32 mips; 

  /// The type of the texture to be used.
  GfxTextureType type;

  /// The pixel format of the texture.
  GfxTextureFormat format;

  /// The filter to be used on the texture when magnified or minified.
  GfxTextureFilter filter;

  /// The addressing mode of the texture.
  GfxTextureWrap wrap_mode;
  
  /// The pixels that will be sent to the GPU.
  void* data = nullptr;
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
  GfxTextureFilter filter;

  /// The addressing mode of the cubemap.
  GfxTextureWrap wrap_mode;
  
  /// An array of pixels (up to `CUBEMAP_FACES_MAX`) of each face of the cubemap.
  void* data[CUBEMAP_FACES_MAX]; 

  /// The amount of faces of the cubemap to use in `data`.
  sizei faces_count = 0;
};
/// GfxCubemapDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GfxPipelineDesc
struct GfxPipelineDesc {
  /// The vertex buffer to be used in a `draw_vertex` command.
  ///
  /// @NOTE: This buffer _must_ be set. It cannot be left a `nullptr`.
  /// Even if `draw_index` is being used.
  GfxBuffer* vertex_buffer           = nullptr; 

  /// The amount of vertices in the `vertex_buffer` to be drawn. 
  sizei vertices_count               = 0;
  
  /// The index buffer to be used in a `draw_index` command.
  ///
  /// @NOTE: This buffer _must_ be set if a `draw_index` command is used.
  /// Otherwise, it can be left as `nullptr`.
  GfxBuffer* index_buffer            = nullptr;

  /// The amount of indices in the `index_buffer` to be drawn.
  sizei indices_count                = 0;
  
  /// Layout array up to `LAYOUT_ELEMENTS_MAX` describing each layout attribute.
  GfxLayoutDesc layout[LAYOUT_ELEMENTS_MAX];

  /// The amount of layouts to be set in `layout`.
  sizei layout_count                 = 0; 

  /// The draw mode of the entire pipeline.
  ///
  /// @NOTE: This can be changed at anytime before the draw command.
  GfxDrawMode draw_mode;
 
  /// A flag to indicate if the pipeline can 
  /// or cannot write to the depth buffer. 
  ///
  /// @NOTE: By default, this value is `true`.
  bool depth_mask                    = true;

  /// The stencil reference value of the pipeline. 
  ///
  /// @NOTE: This is `1` by default.
  u32 stencil_ref                    = 1;
  
  /// The blend factor to be used in the pipeline. 
  ///
  /// @NOTE: This is `{0, 0, 0, 0}` by default.
  f32 blend_factor[4]                = {0, 0, 0, 0};
};
/// GfxPipelineDesc
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

/// Set any `state` of the context `gfx` to `value`. 
/// i.e, this function can turn on or off the `state` in the given `gfx` context.
NIKOLA_API void gfx_context_set_state(GfxContext* gfx, const GfxStates state, const bool value);

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

/// Switch to the back buffer or, rather, present the back buffer to the screen. 
/// 
/// @NOTE: This function will be affected by vsync. 
NIKOLA_API void gfx_context_present(GfxContext* gfx);

/// Context functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Framebuffer functions

/// Allocate and return a `GfxFramebuffer` object, using the information in `desc`. 
NIKOLA_API GfxFramebuffer* gfx_framebuffer_create(GfxContext* gfx, const GfxFramebufferDesc& desc);

/// Free/reclaim any memory taken by `framebuffer`.
NIKOLA_API void gfx_framebuffer_destroy(GfxFramebuffer* framebuffer);

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

/// Allocate and return a `GfxBuffer` object, using the information in `desc`.
NIKOLA_API GfxBuffer* gfx_buffer_create(GfxContext* gfx, const GfxBufferDesc& desc);

/// Free/reclaim any memory taken by `buff`.
NIKOLA_API void gfx_buffer_destroy(GfxBuffer* buff);

/// Retrieve the internal `GfxBufferDesc` of `buffer`
NIKOLA_API GfxBufferDesc& gfx_buffer_get_desc(GfxBuffer* buffer);

/// Update the contents of `buff` starting at `offset` with `data` of size `size`.
NIKOLA_API void gfx_buffer_update(GfxBuffer* buff, const sizei offset, const sizei size, const void* data);

/// Buffer functions 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Shader functions 

/// Allocate and return a `GfxShader`, using the information given in `desc`. 
NIKOLA_API GfxShader* gfx_shader_create(GfxContext* gfx, const GfxShaderDesc& desc);

/// Free/reclaim any memory consumed by `shader`.
NIKOLA_API void gfx_shader_destroy(GfxShader* shader);

/// Use/activate the given `shader` in any subsequent draw call.
///
/// @NOTE: If `shader` is a `nullptr`, the function will assert.
NIKOLA_API void gfx_shader_use(GfxShader* shader);

/// Retrieve the internal `GfxShaderDesc` of `shader`.
NIKOLA_API GfxShaderDesc& gfx_shader_get_source(GfxShader* shader);

/// Update the `shader`'s information from the given `desc`.
NIKOLA_API void gfx_shader_update(GfxShader* shader, const GfxShaderDesc& desc);

/// Attaches the uniform `buffer` to the `shader` of type `type` to point `bind_point`. 
/// Any updates to `buffer` will have an effect on the `shader`.
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

/// Allocate and return a `GfxTexture` from the information provided by `desc`. 
NIKOLA_API GfxTexture* gfx_texture_create(GfxContext* gfx, const GfxTextureDesc& desc);

/// Reclaim/free any memory allocated by `texture`.
NIKOLA_API void gfx_texture_destroy(GfxTexture* texture);

/// Use/activate the given `textures` array of `count` in any subsequent draw call. 
///
/// @NOTE: If any texture inside the `textures` array is a `nullptr`, the function will assert.
///
/// @NOTE: The given `count` CANNOT exceed `TEXTURES_MAX`.
NIKOLA_API void gfx_texture_use(GfxTexture** textures, const sizei count);

/// Retrieve the internal `GfxTextureDesc` of `texture`
NIKOLA_API GfxTextureDesc& gfx_texture_get_desc(GfxTexture* texture);

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

/// Allocate and return a `GfxCubemap` from the information provided by `desc`. 
NIKOLA_API GfxCubemap* gfx_cubemap_create(GfxContext* gfx, const GfxCubemapDesc& desc);

/// Reclaim/free any memory allocated by `texture`.
NIKOLA_API void gfx_cubemap_destroy(GfxCubemap* cubemap);

/// Use/activate the given `cubemaps` array of `count` in any subsequent draw call. 
///
/// @NOTE: If any cubemap inside the `cubemaps` array is a `nullptr`, the function will assert.
///
/// @NOTE: The given `count` CANNOT exceed `CUBEMAPS_MAX`.
NIKOLA_API void gfx_cubemap_use(GfxCubemap** cubemaps, const sizei count);

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

/// Allocate and return a `GfxPipeline` from the information provided by `desc`.
NIKOLA_API GfxPipeline* gfx_pipeline_create(GfxContext* gfx, const GfxPipelineDesc& desc);

/// Reclaim/free any memory allocated by `pipeline`.
NIKOLA_API void gfx_pipeline_destroy(GfxPipeline* pipeline);

/// Retrieve the internal `GfxPipelineDesc` of `pipeline`
NIKOLA_API GfxPipelineDesc& gfx_pipeline_get_desc(GfxPipeline* pipeline);

/// Update the `pipeline`'s information from the given `desc`.
NIKOLA_API void gfx_pipeline_update(GfxPipeline* pipeline, const GfxPipelineDesc& desc);

/// Draw the contents of the `vertex_buffer` in `pipeline`.
NIKOLA_API void gfx_pipeline_draw_vertex(GfxPipeline* pipeline);

/// Draw the contents of the `vertex_buffer` using the `index_buffer` in `pipeline`.
NIKOLA_API void gfx_pipeline_draw_index(GfxPipeline* pipeline);

/// Pipeline functions 
///---------------------------------------------------------------------------------------------------------------------

/// *** Graphics ***
/// ---------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
