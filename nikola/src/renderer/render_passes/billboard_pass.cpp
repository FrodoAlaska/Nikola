#include "render_passes.h"

#include "../shaders/billboard.glsl.h"

#include "nikola/nikola_render.h"
#include "nikola/nikola_timer.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Billboard pass functions

void billboard_pass_init(Window* window) {
  RenderPassDesc pass_desc = {}; 

  // Callbacks init

  pass_desc.prepare_func = billboard_pass_prepare;
  pass_desc.sumbit_func  = billboard_pass_sumbit;

  // Reosurce init

  pass_desc.res_group_id = RESOURCE_CACHE_ID;

  ResourceID billboard_shader = resources_push_shader(RESOURCE_CACHE_ID, generate_billboard_shader());
  pass_desc.shader_context_id = resources_push_shader_context(RESOURCE_CACHE_ID, billboard_shader);

  // Attaching buffers

  ShaderContext* shader_context = resources_get_shader_context(pass_desc.shader_context_id);
  const RenderQueueEntry* queue = renderer_get_queue(RENDER_QUEUE_BILLBOARD); 

  shader_context_set_uniform_buffer(shader_context, SHADER_MODELS_BUFFER_INDEX, queue->transform_buffer);
  shader_context_set_uniform_buffer(shader_context, SHADER_MATERIALS_BUFFER_INDEX, queue->material_buffer);
  shader_context_set_uniform_buffer(shader_context, SHADER_TEXTURES_BUFFER_INDEX, queue->texture_buffer);
  
  // Frame size and flags init

  i32 width, height; 
  window_get_size(window, &width, &height);

  pass_desc.frame_size  = IVec2(width, height);
  pass_desc.clear_flags = (GFX_CLEAR_FLAGS_COLOR_BUFFER | GFX_CLEAR_FLAGS_DEPTH_BUFFER); 
  pass_desc.queue_type  = RENDER_QUEUE_BILLBOARD;

  // Color attachment init

  GfxTextureDesc target_desc = {
    .width  = (u32)width,
    .height = (u32)height,

    .type   = GFX_TEXTURE_2D, 
    .format = GFX_TEXTURE_FORMAT_RGBA8, 

    .is_bindless = false,
  };
  pass_desc.targets.push_back(target_desc);

  // Depth attachment init

  target_desc = {
    .width  = (u32)width,
    .height = (u32)height,

    .type   = GFX_TEXTURE_DEPTH_TARGET, 
    .format = GFX_TEXTURE_FORMAT_DEPTH16, 

    .is_bindless = false,
  };
  pass_desc.targets.push_back(target_desc);

  // Render pass init
  
  RenderPass* billboard_pass = renderer_create_pass(pass_desc);
  renderer_append_pass(billboard_pass);
}

void billboard_pass_prepare(RenderPass* pass, const FrameData& data) {
  NIKOLA_PROFILE_FUNCTION();

  // @TODO (Renderer): Absolutely not!!!!! 
  // NOOOOOOOOO!!!! DO NOT COPY THE FUCKING FRAMEBUFFERSSS!!!!!!

  IVec2 start_pos = IVec2(0);
  IVec2 src_size  = pass->previous->frame_size;
  IVec2 dest_size = pass->frame_size;

  gfx_framebuffer_copy(pass->previous->framebuffer, 
                       pass->framebuffer, 
                       start_pos.x, start_pos.y, 
                       src_size.x, src_size.y,
                       start_pos.x, start_pos.y,
                       dest_size.x, dest_size.y,
                       pass->framebuffer_desc.clear_flags);
}

void billboard_pass_sumbit(RenderPass* pass, const RenderQueueEntry& queue) {
  NIKOLA_PROFILE_FUNCTION();

  // Using resources

  GfxBindingDesc bind_desc = {
    .shader = pass->shader_context->shader, 
  };
  gfx_context_use_bindings(pass->gfx, bind_desc);

  // Render the scene
  
  gfx_context_use_pipeline(pass->gfx, queue.pipe);
  gfx_context_draw_multi_indirect(pass->gfx, 0, queue.commands.size());

  // Setting the output textures

  pass->outputs[0]    = pass->framebuffer_desc.color_attachments[0];
  pass->outputs_count = 1;
}

/// Billboard pass functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////

