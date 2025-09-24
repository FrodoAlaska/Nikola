#include "render_passes.h"

#include "../shaders/pbr_shading.glsl.h"

#include "nikola/nikola_render.h"

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

  // Reosurce init

  pass_desc.res_group_id = RESOURCE_CACHE_ID;

  ResourceID pbr_shader       = resources_push_shader(RESOURCE_CACHE_ID, generate_pbr_shader());
  pass_desc.shader_context_id = resources_push_shader_context(RESOURCE_CACHE_ID, pbr_shader);

  // Buffer injection

  shader_context_set_uniform_buffer(resources_get_shader_context(pass_desc.shader_context_id), 
                                    SHADER_INSTANCE_BUFFER_INDEX, 
                                    (GfxBuffer*)renderer_get_defaults().instance_buffer);
  
  shader_context_set_uniform_buffer(resources_get_shader_context(pass_desc.shader_context_id), 
                                    SHADER_LIGHT_BUFFER_INDEX, 
                                    (GfxBuffer*)renderer_get_defaults().lights_buffer);
  
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

  //
  // @TODO (Renderer): Turning the light space view into a texture coordinate
  // Mat4 light_space = shadow_pass_get_light_space(pass->previous);
  // 
  // Mat4 shadow_space = light_space * mat4_translate(Vec3(0.5f)) * mat4_scale(Vec3(0.5f));
  // shader_context_set_uniform(pass->shader_context, "u_light_space", shadow_space);
  //

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

void light_pass_sumbit(RenderPass* pass, const DynamicArray<GeometryPrimitive>& queue) {
  // Render the skybox

  if(RESOURCE_IS_VALID(s_state.skybox_id)) {
    renderer_draw_skybox(s_state.skybox_id);
  }

  // Render everything 

  for(auto& geo : queue) {
    // Setup uniforms
    
    shader_context_set_uniform(pass->shader_context, "u_material", geo.material);

    // Use the required resources

    GfxTexture* textures[] = {
      geo.material->albedo_map,
      geo.material->roughness_map,
      geo.material->metallic_map,
      geo.material->normal_map,
      geo.material->emissive_map,
    };

    GfxBindingDesc bind_desc = {
      .shader = pass->shader_context->shader,

      .textures       = textures, 
      .textures_count = 5,
    };
    gfx_context_use_bindings(pass->gfx, bind_desc);

    renderer_draw_geometry_primitive(geo);
  }

  // Setting the output textures

  pass->outputs[0]    = pass->framebuffer_desc.color_attachments[0];
  pass->outputs_count = 1;
}

/// Light pass functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////

