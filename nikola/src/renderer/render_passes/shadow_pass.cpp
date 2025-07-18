#include "render_passes.h"

#include "../shaders/shadow.glsl.h"

#include "nikola/nikola_render.h"
#include "nikola/nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// ShadowPassState
struct ShadowPassState {
  GfxBuffer* light_matrix_buffer = nullptr;
  Mat4 light_view, light_projection;
};

static ShadowPassState s_state;
/// ShadowPassState
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Shadow pass functions

void shadow_pass_init(Window* window) {
  RenderPassDesc pass_desc = {}; 

  // Callbacks init

  pass_desc.prepare_func = shadow_pass_prepare;
  pass_desc.sumbit_func  = shadow_pass_sumbit;

  // Reosurce init

  pass_desc.res_group_id      = RESOURCE_CACHE_ID;
  pass_desc.shader_context_id = resources_push_shader_context(RESOURCE_CACHE_ID,  
                                                              resources_push_shader(RESOURCE_CACHE_ID, generate_shadow_shader()));

  GfxBufferDesc buff_desc = {
    .data  = nullptr, 
    .size  = sizeof(Mat4), 
    .type  = GFX_BUFFER_UNIFORM, 
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  s_state.light_matrix_buffer = resources_get_buffer(resources_push_buffer(RESOURCE_CACHE_ID, buff_desc));

  shader_context_set_uniform_buffer(resources_get_shader_context(pass_desc.shader_context_id), 
                                    SHADER_INSTANCE_BUFFER_INDEX, 
                                    (GfxBuffer*)renderer_get_defaults().instance_buffer);
  
  // Other init

  i32 width, height; 
  window_get_size(window, &width, &height);

  pass_desc.frame_size  = IVec2(1280, 1280);
  pass_desc.clear_flags = (GFX_CLEAR_FLAGS_DEPTH_BUFFER); 

  // Depth buffer init

  GfxTextureDesc target_desc = {
    .width  = (u32)pass_desc.frame_size.x,
    .height = (u32)pass_desc.frame_size.x,

    .type   = GFX_TEXTURE_DEPTH_TARGET, 
    .format = GFX_TEXTURE_FORMAT_DEPTH16, 
    .filter = GFX_TEXTURE_FILTER_MIN_MAG_LINEAR,

    .wrap_mode    = GFX_TEXTURE_WRAP_REPEAT,
    .compare_func = GFX_COMPARE_GREATER,
  };
  pass_desc.targets.push_back(target_desc);

  // Render pass init
  
  RenderPass* shadow_pass = renderer_create_pass(pass_desc);
  renderer_append_pass(shadow_pass);
}

void shadow_pass_prepare(RenderPass* pass, const FrameData& data) {
  // Setup the light projection matrix

  s_state.light_projection = mat4_ortho(-15, 15, -15, 15, 1.0f, 7.5f);
  s_state.light_view       = mat4_look_at(Vec3(-4.0f, 10.0f, -1.0f), Vec3(0.0f), Vec3(0.0f, 1.0f, 0.0f));

  // Set uniforms
 
  shader_context_set_uniform(pass->shader_context, "u_light_space", (s_state.light_projection * s_state.light_view));
}

void shadow_pass_sumbit(RenderPass* pass, const DynamicArray<GeometryPrimitive>& queue) {
  // Render everything 

  for(auto& geo : queue) {
    // Use the required resources

    GfxBindingDesc bind_desc = {
      .shader = pass->shader_context->shader,
    };
    gfx_context_use_bindings(pass->gfx, bind_desc);

    renderer_draw_geometry_primitive(geo);
  }

  // Setting the output textures

  pass->outputs[0]    = pass->framebuffer_desc.depth_attachment;
  pass->outputs_count = 1;
}

Mat4 shadow_pass_get_light_space(RenderPass* pass) {
  return (s_state.light_projection * s_state.light_view);
}

/// Shadow pass functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////

