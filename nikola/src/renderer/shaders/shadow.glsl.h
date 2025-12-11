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

      // Uniform buffers
      
      layout(std430, binding = 1) readonly buffer ModelsBuffer {
        mat4 u_model[4096];
      };
      
      layout(std430, binding = 4) readonly buffer AnimationBuffer {
        mat4 u_skinning_palette[256][256]; // @TODO: Probably not the best count to have here
      };
      
      // Uniforms
      
      uniform int u_animation_remap_table[256]; 
      uniform mat4 u_light_space;

      // Utility functions 
     
      vec4 animate_vertex(const int index) {
        vec4 result = vec4(0.0, 0.0, 0.0, 1.0);

        result += u_skinning_palette[index][int(aJointId[0])] * vec4(aPos, 1.0) * aJointWeight[0];
        result += u_skinning_palette[index][int(aJointId[1])] * vec4(aPos, 1.0) * aJointWeight[1];
        result += u_skinning_palette[index][int(aJointId[2])] * vec4(aPos, 1.0) * aJointWeight[2];
        result += u_skinning_palette[index][int(aJointId[3])] * vec4(aPos, 1.0) * aJointWeight[3];

        return result;
      }

      void main() {
        int index       = gl_BaseInstance + gl_InstanceID;
        vec4 vertex_pos = vec4(aPos, 1.0);

        // Animate the vertex if that's possible

        if(aJointId[0] != -2.0) { // -2.0 is just a simple value to indicate a static mesh
          vertex_pos = animate_vertex(u_animation_remap_table[index]);
        }
        
        gl_Position = u_light_space * u_model[index] * vertex_pos;
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

