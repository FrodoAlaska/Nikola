#include "nikola/nikola_resources.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_gfx.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void cache_uniform(ShaderContext* ctx, const String& name) {
  GfxShader* shader = resources_get_shader(ctx->shader); 
  
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
  GfxShader* shader = resources_get_shader(ctx->shader); 

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

void shader_context_cache_uniform(ResourceID& ctx_id, const String& uniform_name) {
  NIKOLA_ASSERT(RESOURCE_IS_VALID(ctx_id), "Invalid ShaderContext passed to shader_context_cache_uniform");
  
  ShaderContext* ctx = resources_get_shader_context(ctx_id);
  cache_uniform(ctx, uniform_name);
}

void shader_context_set_uniform(ResourceID& ctx_id, const String& uniform_name, const i32 value) {
  NIKOLA_ASSERT(RESOURCE_IS_VALID(ctx_id), "Invalid ShaderContext passed to shader_context_set_uniform");

  ShaderContext* ctx = resources_get_shader_context(ctx_id);
  check_and_send_uniform(ctx, uniform_name, GFX_LAYOUT_INT1, &value);
}

void shader_context_set_uniform(ResourceID& ctx_id, const String& uniform_name, const f32 value) {
  NIKOLA_ASSERT(RESOURCE_IS_VALID(ctx_id), "Invalid ShaderContext passed to shader_context_set_uniform");

  ShaderContext* ctx = resources_get_shader_context(ctx_id);
  check_and_send_uniform(ctx, uniform_name, GFX_LAYOUT_FLOAT1, &value);
}

void shader_context_set_uniform(ResourceID& ctx_id, const String& uniform_name, const Vec2& value) {
  NIKOLA_ASSERT(RESOURCE_IS_VALID(ctx_id), "Invalid ShaderContext passed to shader_context_set_uniform");

  ShaderContext* ctx = resources_get_shader_context(ctx_id);
  check_and_send_uniform(ctx, uniform_name, GFX_LAYOUT_FLOAT2, &value);
}

void shader_context_set_uniform(ResourceID& ctx_id, const String& uniform_name, const Vec3& value) {
  NIKOLA_ASSERT(RESOURCE_IS_VALID(ctx_id), "Invalid ShaderContext passed to shader_context_set_uniform");

  ShaderContext* ctx = resources_get_shader_context(ctx_id);
  check_and_send_uniform(ctx, uniform_name, GFX_LAYOUT_FLOAT3, &value);
}

void shader_context_set_uniform(ResourceID& ctx_id, const String& uniform_name, const Vec4& value) {
  NIKOLA_ASSERT(RESOURCE_IS_VALID(ctx_id), "Invalid ShaderContext passed to shader_context_set_uniform");

  ShaderContext* ctx = resources_get_shader_context(ctx_id);
  check_and_send_uniform(ctx, uniform_name, GFX_LAYOUT_FLOAT4, &value);
}

void shader_context_set_uniform(ResourceID& ctx_id, const String& uniform_name, const Mat4& value) {
  NIKOLA_ASSERT(RESOURCE_IS_VALID(ctx_id), "Invalid ShaderContext passed to shader_context_set_uniform");

  ShaderContext* ctx = resources_get_shader_context(ctx_id);
  check_and_send_uniform(ctx, uniform_name, GFX_LAYOUT_MAT4, &value);
}

void shader_context_set_uniform(ResourceID& ctx_id, const String& material_name, const ResourceID& mat_id) {
  NIKOLA_ASSERT(RESOURCE_IS_VALID(ctx_id), "Invalid ShaderContext passed to shader_context_set_uniform");
  NIKOLA_ASSERT(RESOURCE_IS_VALID(mat_id), "Invalid Material passed to shader_context_set_uniform");

  ShaderContext* ctx = resources_get_shader_context(ctx_id);
  Material* material = resources_get_material(mat_id);

  // Send all of the known material uniforms
  check_and_send_uniform(ctx, MATERIAL_UNIFORM_AMBIENT_COLOR, GFX_LAYOUT_FLOAT3, &material->ambient_color);
  check_and_send_uniform(ctx, MATERIAL_UNIFORM_DIFFUSE_COLOR, GFX_LAYOUT_FLOAT3, &material->diffuse_color);
  check_and_send_uniform(ctx, MATERIAL_UNIFORM_SPECULAR_COLOR, GFX_LAYOUT_FLOAT3, &material->diffuse_color);
  check_and_send_uniform(ctx, MATERIAL_UNIFORM_SHININESS, GFX_LAYOUT_FLOAT1, &material->shininess);
}

void shader_context_set_uniform_buffer(ResourceID& ctx_id, const sizei index, const ResourceID& buffer_id) {
  NIKOLA_ASSERT(RESOURCE_IS_VALID(ctx_id), "Invalid ShaderContext passed to shader_context_set_uniform_buffer");
  NIKOLA_ASSERT(RESOURCE_IS_VALID(buffer_id), "Invalid buffer given to shader_context_set_uniform_buffer");

  ShaderContext* ctx          = resources_get_shader_context(ctx_id);
  GfxBuffer* buffer           = resources_get_buffer(buffer_id);
  GfxShader* shader           = resources_get_shader(ctx->shader);
  ctx->uniform_buffers[index] = buffer_id;
  
  // Attach the uniform
  gfx_shader_attach_uniform(shader, GFX_SHADER_VERTEX, buffer, index);
}

void shader_context_set_shader(ResourceID& ctx_id, const ResourceID& shader_id) {
  NIKOLA_ASSERT(RESOURCE_IS_VALID(ctx_id), "Invalid ShaderContext passed to shader_context_set_shader");
  NIKOLA_ASSERT(RESOURCE_IS_VALID(shader_id), "Invalid shader given to shader_context_set_shader");

  ShaderContext* ctx = resources_get_shader_context(ctx_id);
  ctx->shader        = shader_id;
}

void shader_context_use(ResourceID& ctx_id) {
  NIKOLA_ASSERT(RESOURCE_IS_VALID(ctx_id), "Invalid ShaderContext passed to shader_context_use");
  
  ShaderContext* ctx = resources_get_shader_context(ctx_id);
  NIKOLA_ASSERT(RESOURCE_IS_VALID(ctx->shader), "Invalid shader in ShaderContext passed to shader_context_use");

  GfxShader* shader = resources_get_shader(ctx->shader);
  gfx_shader_use(shader);
}

/// ShaderContext functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
