#include "render_passes.h"

#include "../light_shaders.h"
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

  shader_context_set_uniform_buffer(resources_get_shader_context(pass_desc.shader_context_id), 
                                    SHADER_INSTANCE_BUFFER_INDEX, 
                                    (GfxBuffer*)renderer_get_defaults().instance_buffer);

  // Other init

  i32 width, height; 
  window_get_size(window, &width, &height);

  pass_desc.frame_size  = IVec2(width, height);
  pass_desc.clear_flags = (GFX_CLEAR_FLAGS_COLOR_BUFFER | GFX_CLEAR_FLAGS_DEPTH_BUFFER); 

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
    .format = GFX_TEXTURE_FORMAT_DEPTH24, 
  };
  pass_desc.targets.push_back(target_desc);

  // Render pass init
  
  RenderPass* light_pass = renderer_create_pass(pass_desc);
  renderer_append_pass(light_pass);
}

void light_pass_prepare(RenderPass* pass, const FrameData& data) {
  ShaderContext* ctx = pass->shader_context;

  // Set globals

  shader_context_set_uniform(ctx, "u_ambient", data.ambient); 
  shader_context_set_uniform(ctx, "u_point_lights_count", (i32)data.point_lights.size()); 
  shader_context_set_uniform(ctx, "u_dir_light.direction", data.dir_light.direction); 
  shader_context_set_uniform(ctx, "u_dir_light.color", data.dir_light.color); 

  // Set point lights

  i32 index = 0;

  for(auto& point : data.point_lights) {
    String point_index = "u_point_lights[" + std::to_string(index) + "].";

    shader_context_set_uniform(ctx, (point_index + "position"), point.position); 
    shader_context_set_uniform(ctx, (point_index + "color"), point.color); 

    shader_context_set_uniform(ctx, (point_index + "radius"), point.radius); 
  
    index++;
  }
}

void light_pass_sumbit(RenderPass* pass) {
  renderer_flush_queue_command(pass->shader_context); 

  // Setting the output textures

  pass->outputs[0]    = pass->framebuffer_desc.color_attachments[0];
  pass->outputs_count = 1;
}

/// Light pass functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////

