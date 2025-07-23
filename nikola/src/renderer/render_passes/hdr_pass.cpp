#include "render_passes.h"

#include "../shaders/hdr.glsl.h"

#include "nikola/nikola_render.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// HDR pass functions

void hdr_pass_init(Window* window) {
  RenderPassDesc pass_desc = {}; 

  // Callbacks init

  pass_desc.prepare_func = hdr_pass_prepare;
  pass_desc.sumbit_func  = hdr_pass_sumbit;

  // Reosurce init

  pass_desc.res_group_id = RESOURCE_CACHE_ID;

  ResourceID hdr_shader       = resources_push_shader(RESOURCE_CACHE_ID, generate_hdr_shader());
  pass_desc.shader_context_id = resources_push_shader_context(RESOURCE_CACHE_ID, hdr_shader);

  // Other init

  i32 width, height; 
  window_get_size(window, &width, &height);

  pass_desc.frame_size  = IVec2(width, height);
  pass_desc.clear_flags = GFX_CLEAR_FLAGS_COLOR_BUFFER; 

  // Color attachment init

  GfxTextureDesc target_desc = {
    .width  = (u32)width,
    .height = (u32)height,

    .type   = GFX_TEXTURE_2D, 
    .format = GFX_TEXTURE_FORMAT_RGBA32F, 
  };
  pass_desc.targets.push_back(target_desc);

  // Render pass init
  
  RenderPass* hdr_pass = renderer_create_pass(pass_desc);
  renderer_append_pass(hdr_pass);
}

void hdr_pass_prepare(RenderPass* pass, const FrameData& data) {
  shader_context_set_uniform(pass->shader_context, "u_exposure", data.camera.exposure); 
}

void hdr_pass_sumbit(RenderPass* pass, const DynamicArray<GeometryPrimitive>& queue) {
  // Using resources

  GfxBindingDesc bind_desc = {
    .shader = pass->shader_context->shader, 
  
    .textures       = &pass->previous->outputs[0],
    .textures_count = 1,
  };
  gfx_context_use_bindings(pass->gfx, bind_desc);

  // Rendering the scene
 
  gfx_context_use_pipeline(pass->gfx, renderer_get_defaults().screen_quad);
  gfx_context_draw(pass->gfx, 0);
 
  // Setting outputs

  pass->outputs[0]    = pass->framebuffer_desc.color_attachments[0];
  pass->outputs_count = 1;
}

/// HDR pass functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
