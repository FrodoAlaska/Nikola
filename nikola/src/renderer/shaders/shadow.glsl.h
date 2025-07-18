#pragma once

#include "nikola/nikola_gfx.h"

inline nikola::GfxShaderDesc generate_shadow_shader() {
  return nikola::GfxShaderDesc {
    .vertex_source = R"(
      #version 460 core

      // Layouts
      layout (location = 0) in vec3 aPos;
      layout (location = 1) in vec3 aNormal;
      layout (location = 2) in vec2 aTexCoords;

      // Uniforms
      
      layout(std140, binding = 1) uniform InstanceBuffer {
        mat4 u_model[1024];
      };
      
      uniform mat4 u_light_space;

      void main() {
        gl_Position = u_light_space * u_model[gl_InstanceID] * vec4(aPos, 1.0f);
      }
    )",
  
    .pixel_source = R"(
      #version 460 core
      
      void main() {
        // Does nothing. Just for the depth buffer...
      }
    )",
  };
}

