#include "render_passes.h"

#include "../shaders/particle.glsl.h"

#include "nikola/nikola_render.h"
#include "nikola/nikola_timer.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Particle pass functions

void particle_pass_init(Window* window) {
  RenderPassDesc pass_desc = {}; 

  // Callbacks init

  pass_desc.sumbit_func = particle_pass_sumbit;
  pass_desc.resize_func = particle_pass_on_resize;

  // Reosurce init

  pass_desc.res_group_id = RESOURCE_CACHE_ID;

  ResourceID particle_shader  = resources_push_shader(RESOURCE_CACHE_ID, generate_particle_shader());
  pass_desc.shader_context_id = resources_push_shader_context(RESOURCE_CACHE_ID, particle_shader);
  
  // Other variables init
  pass_desc.queue_type  = RENDER_QUEUE_PARTICLE;

  // Render pass init
  
  RenderPass* particle_pass = renderer_create_pass(pass_desc, "Particle pass", renderer_peek_pass(RENDER_PASS_LIGHT));
  renderer_append_pass(particle_pass);
}

void particle_pass_sumbit(RenderPass* pass, const RenderQueueEntry& queue) {
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

void particle_pass_on_resize(RenderPass* pass, const IVec2& new_size) {
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

/// Particle pass functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////

