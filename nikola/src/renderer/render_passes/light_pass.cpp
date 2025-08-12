#include "render_passes.h"

#include "../shaders/bling_phong.glsl.h"

#include "nikola/nikola_render.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Light pass functions

void light_pass_init(Window* window) {
  RenderPassDesc pass_desc = {}; 

  // Callbacks init

  pass_desc.prepare_func = light_pass_prepare;
  pass_desc.sumbit_func  = light_pass_sumbit;

  // Reosurce init

  pass_desc.res_group_id = RESOURCE_CACHE_ID;

  ResourceID blinn_phong_shader = resources_push_shader(RESOURCE_CACHE_ID, generate_blinn_phong_shader());
  pass_desc.shader_context_id   = resources_push_shader_context(RESOURCE_CACHE_ID, blinn_phong_shader);

  // Buffer injection

  shader_context_set_uniform_buffer(resources_get_shader_context(pass_desc.shader_context_id), 
                                    SHADER_INSTANCE_BUFFER_INDEX, 
                                    (GfxBuffer*)renderer_get_defaults().instance_buffer);
  
  shader_context_set_uniform_buffer(resources_get_shader_context(pass_desc.shader_context_id), 
                                    SHADER_ANIMATION_BUFFER_INDEX, 
                                    (GfxBuffer*)renderer_get_defaults().animation_buffer);

  // Frame size and flags init

  i32 width, height; 
  window_get_size(window, &width, &height);

  pass_desc.frame_size  = IVec2(width, height);
  pass_desc.clear_flags = (GFX_CLEAR_FLAGS_COLOR_BUFFER | GFX_CLEAR_FLAGS_DEPTH_BUFFER); 
  pass_desc.queue_type  = RENDER_QUEUE_OPAQUE;

  // Color attachment init

  GfxTextureDesc target_desc = {
    .width  = (u32)width,
    .height = (u32)height,

    .type   = GFX_TEXTURE_2D, 
    .format = GFX_TEXTURE_FORMAT_RGBA8, 
  };
  pass_desc.targets.push_back(target_desc);

  // Depth buffer init

  target_desc = {
    .width  = (u32)width,
    .height = (u32)height,

    .type   = GFX_TEXTURE_DEPTH_TARGET, 
    .format = GFX_TEXTURE_FORMAT_DEPTH16, 
    .filter = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST,
  };
  pass_desc.targets.push_back(target_desc);

  // Render pass init
  
  RenderPass* light_pass = renderer_create_pass(pass_desc);
  renderer_append_pass(light_pass);
}

void light_pass_prepare(RenderPass* pass, const FrameData& data) {
  ShaderContext* ctx = pass->shader_context;

  // @TODO (Renderer): We need to put all of per-scene data into a  buffer 
  // and send it over instead of setting each value indivisually

  // Turning the light space view into a texture coordinate
  
  Mat4 light_space = shadow_pass_get_light_space(pass->previous);
  
  Mat4 shadow_space = light_space * mat4_translate(Vec3(0.5f)) * mat4_scale(Vec3(0.5f));
  shader_context_set_uniform(pass->shader_context, "u_light_space", shadow_space);

  // Set the lighting uniforms
 
  shader_context_set_uniform(pass->shader_context, "u_dir_light", data.dir_light);
  shader_context_set_uniform(pass->shader_context, "u_ambient", data.ambient);
  
  shader_context_set_uniform(pass->shader_context, "u_points_count", (i32)data.point_lights.size());
  for(sizei i = 0; i < data.point_lights.size(); i++) {
    shader_context_set_uniform(pass->shader_context, ("u_points[" + std::to_string(i) + "]"), data.point_lights[i]);
  }
  
  shader_context_set_uniform(pass->shader_context, "u_spots_count", (i32)data.spot_lights.size());
  for(sizei i = 0; i < data.spot_lights.size(); i++) {
    shader_context_set_uniform(pass->shader_context, ("u_spots[" + std::to_string(i) + "]"), data.spot_lights[i]);
  }

  // Render the skybox
  // @TEMP

  renderer_draw_skybox(data.skybox_id);
}

void light_pass_sumbit(RenderPass* pass, const DynamicArray<GeometryPrimitive>& queue) {
  // Render everything 

  for(auto& geo : queue) {
    // Setup uniforms
    
    shader_context_set_uniform(pass->shader_context, "u_material", geo.material);

    // Use the required resources

    GfxTexture* textures[] = {
      geo.material->diffuse_map,
      geo.material->specular_map,
      geo.material->normal_map,
      
      pass->previous->outputs[0], // Should be the shadow pass's result
    };

    GfxBindingDesc bind_desc = {
      .shader = pass->shader_context->shader,

      .textures       = textures, 
      .textures_count = 4,
    };
    gfx_context_use_bindings(pass->gfx, bind_desc);

    renderer_draw_geometry_primitive(geo);
  }

  // Setting the output textures

  pass->outputs[0]    = pass->framebuffer_desc.color_attachments[0];
  pass->outputs[1]    = pass->previous->outputs[0],
  pass->outputs_count = 2;
}

/// Light pass functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////

