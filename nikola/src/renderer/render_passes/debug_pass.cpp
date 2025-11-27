#include "render_passes.h"

#include "../shaders/debug.glsl.h"

#include "nikola/nikola_render.h"
#include "nikola/nikola_timer.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Debug pass functions

void debug_pass_init(Window* window) {
  RenderPassDesc pass_desc = {}; 

  // Callbacks init

  pass_desc.sumbit_func = debug_pass_sumbit;
  pass_desc.resize_func = debug_pass_on_resize;

  // Reosurces init

  pass_desc.res_group_id = RESOURCE_CACHE_ID;

  ResourceID debug_shader     = resources_push_shader(RESOURCE_CACHE_ID, generate_debug_shader());
  pass_desc.shader_context_id = resources_push_shader_context(RESOURCE_CACHE_ID, debug_shader);

  // Other variables init
  pass_desc.queue_type  = RENDER_QUEUE_DEBUG;

  // Render pass init
  RenderPass* debug_pass = renderer_create_pass(pass_desc, "Debug pass", renderer_peek_pass(RENDER_PASS_LIGHT));
}

void debug_pass_sumbit(RenderPass* pass, const RenderQueueEntry& queue) {
  NIKOLA_PROFILE_FUNCTION();

  // Early out to save on CPU time

  if(queue.commands.empty()) {
    // Saving the output of the last render pass, 
    // since this one won't be used.

    pass->outputs[0]    = pass->previous->framebuffer_desc.color_attachments[0];
    pass->outputs_count = 1;
    
    return;
  }

  // Buffer bind points

  gfx_buffer_bind_point(queue.transform_buffer, SHADER_MODELS_BUFFER_INDEX);
  gfx_buffer_bind_point(queue.material_buffer, SHADER_MATERIALS_BUFFER_INDEX);

  // Using resources

  GfxBuffer* command_buff  = queue.command_buffer;
  GfxBindingDesc bind_desc = {
    .shader = pass->shader_context->shader, 

    .buffers       = &command_buff, 
    .buffers_count = 1
  };
  gfx_context_use_bindings(pass->gfx, bind_desc);

  // Render the scene
  
  gfx_context_use_pipeline(pass->gfx, queue.pipe);
  gfx_context_draw_multi_indirect(pass->gfx, 0, queue.commands.size());
  
  // Setting the output textures

  pass->outputs[0]    = pass->framebuffer_desc.color_attachments[0];
  pass->outputs_count = 1;
}

void debug_pass_on_resize(RenderPass* pass, const IVec2& new_size) {
  pass->frame_size = new_size;

  // Updating the color attachment

  GfxTextureDesc& color_desc = gfx_texture_get_desc(pass->framebuffer_desc.color_attachments[0]);

  color_desc.width  = (u32)new_size.x;
  color_desc.height = (u32)new_size.y;
  gfx_texture_reload(pass->framebuffer_desc.color_attachments[0], color_desc);

  // Updating the depth attachment

  GfxTextureDesc& depth_desc = gfx_texture_get_desc(pass->framebuffer_desc.depth_attachment);

  depth_desc.width  = (u32)new_size.x;
  depth_desc.height = (u32)new_size.y;
  gfx_texture_reload(pass->framebuffer_desc.depth_attachment, depth_desc);

  // Update the framebuffer
  gfx_framebuffer_update(pass->framebuffer, pass->framebuffer_desc);
}

/// Debug pass functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
