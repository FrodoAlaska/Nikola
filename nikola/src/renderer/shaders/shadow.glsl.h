#pragma once

#include "nikola/nikola_gfx.h"

inline nikola::GfxShaderDesc generate_shadow_shader() {
  return nikola::GfxShaderDesc {
    .vertex_source = R"(
      #version 460 core

      // Layouts
      
      layout (location = 0) in vec3 aPos;
      layout (location = 1) in vec3 aNormal;
      layout (location = 2) in vec3 aTangent;
      layout (location = 3) in vec4 aJointId;
      layout (location = 4) in vec4 aJointWeight;
      layout (location = 5) in vec2 aTexCoords;

      // Uniforms
      
      layout(std430, binding = 1) readonly buffer ModelsBuffer {
        mat4 u_model[4096];
      };
      
      uniform mat4 u_light_space;

      void main() {
        int index   = gl_BaseInstance + gl_InstanceID;
        gl_Position = u_light_space * u_model[index] * vec4(aPos, 1.0f);
      }
    )",
  
    .pixel_source = R"(
      #version 460 core
      #extension GL_ARB_bindless_texture : require

      void main() {
        // Nothing here...
      }
    )",
  };
}

