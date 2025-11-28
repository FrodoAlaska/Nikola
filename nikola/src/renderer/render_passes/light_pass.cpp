#include "render_passes.h"

#include "../shaders/pbr_shading.glsl.h"

#include "nikola/nikola_render.h"
#include "nikola/nikola_timer.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// LightPassState
struct LightPassState {
  ResourceID skybox_id = {};
};

static LightPassState s_state;
/// LightPassState
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Light pass functions

void light_pass_init(Window* window) {
  RenderPassDesc pass_desc = {}; 

  // Callbacks init

  pass_desc.prepare_func = light_pass_prepare;
  pass_desc.sumbit_func  = light_pass_sumbit;
  pass_desc.resize_func  = light_pass_on_resize;
  pass_desc.destroy_func = light_pass_destroy;

  // Reosurce init

  pass_desc.res_group_id = RESOURCE_CACHE_ID;

  ResourceID pbr_shader       = resources_push_shader(RESOURCE_CACHE_ID, generate_pbr_shader());
  pass_desc.shader_context_id = resources_push_shader_context(RESOURCE_CACHE_ID, pbr_shader);

  // Frame size and flags init

  i32 width, height; 
  window_get_size(window, &width, &height);

  pass_desc.frame_size  = IVec2(width, height);
  pass_desc.clear_flags = (GFX_CLEAR_FLAGS_COLOR_BUFFER | GFX_CLEAR_FLAGS_DEPTH_BUFFER); 
  pass_desc.queue_type  = RENDER_QUEUE_OPAQUE;

  // Attaching buffers
  gfx_buffer_bind_point(renderer_get_defaults().lights_buffer, SHADER_LIGHT_BUFFER_INDEX);

  // Color attachment init

  GfxTextureDesc target_desc = {
    .width  = (u32)width,
    .height = (u32)height,

    .type   = GFX_TEXTURE_2D, 
    .format = GFX_TEXTURE_FORMAT_RGBA8, 

    .is_bindless = false,
  };
  pass_desc.targets.push_back(target_desc);

  // Depth buffer init

  target_desc = {
    .width  = (u32)width,
    .height = (u32)height,

    .type   = GFX_TEXTURE_DEPTH_TARGET, 
    .format = GFX_TEXTURE_FORMAT_DEPTH16, 
    .filter = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST,

    .is_bindless = false,
  };
  pass_desc.targets.push_back(target_desc);

  // Render pass init
  
  RenderPass* light_pass = renderer_create_pass(pass_desc, "Light pass");
  renderer_append_pass(light_pass);
}

void light_pass_destroy(RenderPass* pass) {
  gfx_framebuffer_destroy(pass->framebuffer);
}

void light_pass_prepare(RenderPass* pass, const FrameData& data) {
  NIKOLA_PROFILE_FUNCTION();

  // Prepare the context for rendering 

  gfx_context_set_viewport(pass->gfx, 0, 0, pass->frame_size.x, pass->frame_size.y);

  Vec4 col = renderer_get_clear_color();
  gfx_context_clear(pass->gfx, col.r, col.g, col.b, col.a);

  // Turning the light space view into a texture coordinate
  
  ShaderContext* ctx = pass->shader_context;
  shader_context_set_uniform(pass->shader_context, "u_light_space", shadow_pass_get_light_space(pass->previous));

  // Set the light uniforms

  LightBuffer light_buffer = {
    .ambient_color = data.ambient,

    .point_lights_count = (i32)data.point_lights.size(),
    .spot_lights_count  = (i32)data.spot_lights.size(),
  };

  // Directional light

  light_buffer.dir_light = {
    .direction = data.dir_light.direction, 
    .color     = data.dir_light.color, 
  };

  // Point lights

  for(sizei i = 0; i < data.point_lights.size(); i++) {
    PointLightInterface light = {
      .position = data.point_lights[i].position, 
      .color    = data.point_lights[i].color,
      .radius   = data.point_lights[i].radius,
      .fall_off = data.point_lights[i].fall_off,
    };

    light_buffer.point_lights[i] = light;
  }

  // Spot lights

  for(sizei i = 0; i < data.spot_lights.size(); i++) {
    SpotLightInterface light = {
      .position  = data.spot_lights[i].position, 
      .direction = data.spot_lights[i].direction, 
      .color     = data.spot_lights[i].color,

      .radius       = data.spot_lights[i].radius,
      .outer_radius = data.spot_lights[i].outer_radius,
    };

    light_buffer.spot_lights[i] = light;
  }

  // Updating the buffer

  gfx_buffer_upload_data(renderer_get_defaults().lights_buffer, 
                         0, 
                         sizeof(LightBuffer), 
                         &light_buffer); 

  s_state.skybox_id = data.skybox_id;
}

void light_pass_sumbit(RenderPass* pass, const RenderQueueEntry& queue) {
  NIKOLA_PROFILE_FUNCTION();

  // Buffer bind points

  gfx_buffer_bind_point(queue.transform_buffer, SHADER_MODELS_BUFFER_INDEX);
  gfx_buffer_bind_point(queue.material_buffer, SHADER_MATERIALS_BUFFER_INDEX);
  gfx_buffer_bind_point(queue.animation_buffer, SHADER_ANIMATION_BUFFER_INDEX);

  // Render the skybox

  if(RESOURCE_IS_VALID(s_state.skybox_id)) {
    renderer_draw_skybox(s_state.skybox_id);
  }

  // Use the required resources

  GfxBuffer* command_buff  = queue.command_buffer;
  GfxBindingDesc bind_desc = {
    .shader = pass->shader_context->shader,

    .textures       = &pass->previous->outputs[0], 
    .textures_count = 1,

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

void light_pass_on_resize(RenderPass* pass, const IVec2& new_size) {
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

/// Light pass functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////

