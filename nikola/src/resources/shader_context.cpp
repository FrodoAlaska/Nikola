#include "nikola/nikola_resources.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_gfx.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void cache_uniform(ShaderContext* ctx, const String& name) {
  GfxShader* shader = ctx->shader; 
  
  // Get the new uniform location, first
  i32 location = gfx_shader_uniform_lookup(shader, name.c_str());
  
  // The uniform just does not exist in the shader at all 
  if(location == -1) {
    NIKOLA_LOG_WARN("Could not find uniform \'%s\' in ShaderContext", name.c_str());
    return;
  }
  
  ctx->uniforms_cache[name] = location; 
  NIKOLA_LOG_DEBUG("Cache uniform \'%s\' with location \'%i\' in ShaderContext...", name.c_str(), location);
}

static void check_and_send_uniform(ShaderContext* ctx, const String& name, GfxLayoutType type, const void* data) {
  GfxShader* shader = ctx->shader; 

  // Send the uniform (only if it is valid)
  if(ctx->uniforms_cache.find(name) != ctx->uniforms_cache.end()) {
    gfx_shader_upload_uniform(shader, ctx->uniforms_cache[name], type, data);
    return;
  }
  
  // Uniform exists but isn't cached. So, cache it.
  cache_uniform(ctx, name); 
  gfx_shader_upload_uniform(shader, ctx->uniforms_cache[name], type, data);
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ShaderContext functions

void shader_context_cache_uniform(ShaderContext* ctx, const String& uniform_name) {
  NIKOLA_ASSERT(ctx, "Invalid ShaderContext passed to shader_context_set_uniform");
  NIKOLA_ASSERT(ctx->shader, "Invalid shader in ShaderContext passed to shader_context_set_uniform");
  
  cache_uniform(ctx, uniform_name);
}

void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const i32 value) {
  NIKOLA_ASSERT(ctx, "Invalid ShaderContext passed to shader_context_set_uniform");
  NIKOLA_ASSERT(ctx->shader, "Invalid shader in ShaderContext passed to shader_context_set_uniform");

  check_and_send_uniform(ctx, uniform_name, GFX_LAYOUT_INT1, &value);
}

void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const f32 value) {
  NIKOLA_ASSERT(ctx, "Invalid ShaderContext passed to shader_context_set_uniform");
  NIKOLA_ASSERT(ctx->shader, "Invalid shader in ShaderContext passed to shader_context_set_uniform");

  check_and_send_uniform(ctx, uniform_name, GFX_LAYOUT_FLOAT1, &value);
}

void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const Vec2& value) {
  NIKOLA_ASSERT(ctx, "Invalid ShaderContext passed to shader_context_set_uniform");
  NIKOLA_ASSERT(ctx->shader, "Invalid shader in ShaderContext passed to shader_context_set_uniform");

  check_and_send_uniform(ctx, uniform_name, GFX_LAYOUT_FLOAT2, &value);
}

void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const Vec3& value) {
  NIKOLA_ASSERT(ctx, "Invalid ShaderContext passed to shader_context_set_uniform");
  NIKOLA_ASSERT(ctx->shader, "Invalid shader in ShaderContext passed to shader_context_set_uniform");

  check_and_send_uniform(ctx, uniform_name, GFX_LAYOUT_FLOAT3, &value);
}

void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const Vec4& value) {
  NIKOLA_ASSERT(ctx, "Invalid ShaderContext passed to shader_context_set_uniform");
  NIKOLA_ASSERT(ctx->shader, "Invalid shader in ShaderContext passed to shader_context_set_uniform");

  check_and_send_uniform(ctx, uniform_name, GFX_LAYOUT_FLOAT4, &value);
}

void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const Mat4& value) {
  NIKOLA_ASSERT(ctx, "Invalid ShaderContext passed to shader_context_set_uniform");
  NIKOLA_ASSERT(ctx->shader, "Invalid shader in ShaderContext passed to shader_context_set_uniform");

  check_and_send_uniform(ctx, uniform_name, GFX_LAYOUT_MAT4, &value);
}

void shader_context_set_uniform_buffer(ShaderContext* ctx, const sizei index, const GfxBuffer* buffer) {
  NIKOLA_ASSERT(ctx, "Invalid ShaderContext passed to shader_context_set_uniform_buffer");
  NIKOLA_ASSERT(ctx->shader, "Invalid shader in ShaderContext passed to shader_context_set_uniform_buffer");
  NIKOLA_ASSERT(buffer, "Invalid buffer given to shader_context_set_uniform_buffer");
  NIKOLA_ASSERT(((index < SHADER_UNIFORM_BUFFERS_MAX) && (index >= 0)), "Cannot exceed SHADER_UNIFORM_BUFFERS_MAX"); 

  // Attach the uniform
  ctx->uniform_buffers[index] = (GfxBuffer*)buffer;
  gfx_shader_attach_uniform(ctx->shader, GFX_SHADER_VERTEX, (GfxBuffer*)buffer, index);
}

void shader_context_use(ShaderContext* ctx) {
  NIKOLA_ASSERT(ctx, "Invalid ShaderContext passed to shader_context_use");
  NIKOLA_ASSERT(ctx->shader, "Invalid shader in ShaderContext passed to shader_context_use");

  gfx_shader_use(ctx->shader);
}

/// ShaderContext functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
