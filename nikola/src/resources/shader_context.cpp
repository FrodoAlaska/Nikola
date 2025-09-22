#include "nikola/nikola_resources.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_gfx.h"
#include "nikola/nikola_render.h"

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
  
  // @TODO: Silent error??
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

void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const Material* value) {
  NIKOLA_ASSERT(ctx, "Invalid ShaderContext passed to shader_context_set_uniform_buffer");
  NIKOLA_ASSERT(ctx->shader, "Invalid shader in ShaderContext passed to shader_context_set_uniform_buffer");
  NIKOLA_ASSERT(value, "Invalid Material given to shader_context_set_uniform");

  shader_context_set_uniform(ctx, (uniform_name + ".color"), value->color);
  shader_context_set_uniform(ctx, (uniform_name + ".roughness"), value->roughness);
  shader_context_set_uniform(ctx, (uniform_name + ".metallic"), value->metallic);
  shader_context_set_uniform(ctx, (uniform_name + ".transparency"), value->transparency);
}

void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const PointLight& value) {
  NIKOLA_ASSERT(ctx, "Invalid ShaderContext passed to shader_context_set_uniform_buffer");
  NIKOLA_ASSERT(ctx->shader, "Invalid shader in ShaderContext passed to shader_context_set_uniform_buffer");

  shader_context_set_uniform(ctx, (uniform_name + ".position"), value.position);
  shader_context_set_uniform(ctx, (uniform_name + ".color"), value.color);
  shader_context_set_uniform(ctx, (uniform_name + ".radius"), value.radius);
  shader_context_set_uniform(ctx, (uniform_name + ".fall_off"), value.fall_off);
}

void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const DirectionalLight& value) {
  NIKOLA_ASSERT(ctx, "Invalid ShaderContext passed to shader_context_set_uniform_buffer");
  NIKOLA_ASSERT(ctx->shader, "Invalid shader in ShaderContext passed to shader_context_set_uniform_buffer");

  shader_context_set_uniform(ctx, (uniform_name + ".direction"), value.direction);
  shader_context_set_uniform(ctx, (uniform_name + ".color"), value.color);
}

void shader_context_set_uniform(ShaderContext* ctx, const String& uniform_name, const SpotLight& value) {
  NIKOLA_ASSERT(ctx, "Invalid ShaderContext passed to shader_context_set_uniform_buffer");
  NIKOLA_ASSERT(ctx->shader, "Invalid shader in ShaderContext passed to shader_context_set_uniform_buffer");

  shader_context_set_uniform(ctx, (uniform_name + ".position"), value.position);
  shader_context_set_uniform(ctx, (uniform_name + ".direction"), value.direction);
  shader_context_set_uniform(ctx, (uniform_name + ".color"), value.color);
  shader_context_set_uniform(ctx, (uniform_name + ".radius"), (f32)nikola::cos(value.radius));
  shader_context_set_uniform(ctx, (uniform_name + ".outer_radius"), (f32)nikola::cos(value.outer_radius));
}


void shader_context_set_uniform_buffer(ShaderContext* ctx, const sizei index, const GfxBuffer* buffer) {
  NIKOLA_ASSERT(ctx, "Invalid ShaderContext passed to shader_context_set_uniform_buffer");
  NIKOLA_ASSERT(ctx->shader, "Invalid shader in ShaderContext passed to shader_context_set_uniform_buffer");
  NIKOLA_ASSERT(buffer, "Invalid buffer given to shader_context_set_uniform_buffer");

  gfx_shader_attach_uniform(ctx->shader, GFX_SHADER_VERTEX, (GfxBuffer*)buffer, index);
}

/// ShaderContext functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
