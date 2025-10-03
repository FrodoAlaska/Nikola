#pragma once

#include "nikola/nikola_gfx.h"

inline nikola::GfxShaderDesc generate_debug_shader() {
  return nikola::GfxShaderDesc {
    .vertex_source = R"(
      #version 460 core

      // Layouts
      
      layout (location = 0) in vec3 aPos;
      layout (location = 1) in vec2 aTextureCoords;

      // Outputs
      out vec2 texture_coords;

      // Uniforms

      layout(std140, binding = 0) uniform Matrices {
        mat4 u_view;
        mat4 u_projection;
        vec3 u_camera_pos;
      };
 
      layout(std140, binding = 1) uniform InstanceBuffer {
        mat4 u_model[1024];
      };

      void main() {
        gl_Position    = u_projection * u_view * u_model[gl_InstanceID] * vec4(aPos, 1.0);
        texture_coords = aTextureCoords;
      }
    )",

    .pixel_source = R"(
      #version 460 core
     
      // Layouts
      layout (location = 0) out vec4 frag_color;
   
      // Inputs
      in vec2 texture_coords;

      // Uniforms
      
      struct Material {
        vec3 color;
        float transparency;
      };

      uniform Material u_material;
      
      layout (binding = 0) uniform sampler2D u_albedo;

      void main() {
        vec3 texel = texture(u_albedo, texture_coords).rgb * u_material.color;
        frag_color = vec4(texel, u_material.transparency);
      }
    )"
  };
}
