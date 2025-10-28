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
      out flat int material_index;

      // Uniforms

      layout(std140, binding = 0) uniform Matrices {
        mat4 u_view;
        mat4 u_projection;
        vec3 u_camera_pos;
      };
 
      layout(std140, binding = 1) uniform ModelsBuffer {
        mat4 u_model[4096];
      };

      void main() {
        texture_coords = aTextureCoords;
        material_index = gl_DrawID; 

        gl_Position = u_projection * u_view * u_model[gl_DrawID] * vec4(aPos, 1.0);
      }
    )",

    .pixel_source = R"(
      #version 460 core
      #extension GL_ARB_bindless_texture : require

      // Layouts
      layout (location = 0) out vec4 frag_color;
   
      // Inputs
      
      in vec2 texture_coords;
      in flat int material_index;

      // Uniforms
      
      struct Material {
        int albedo_index;
        int metallic_index;
        int roughness_index;
        int normal_index;

        int emissive_index;
        float metallic;
        float roughness;
        float emissive;

        vec3 color;
        float transparency;
      };

      layout(binding = 2, std430) readonly buffer MaterialsBuffer {
        Material u_materials[];
      };
      
      layout(binding = 3, std140) uniform TexturesBuffer {
        sampler2D u_textures[4096];
      };

      void main() {
        Material material = u_materials[material_index];

        vec3 texel = vec3(texture(u_textures[material.albedo_index], texture_coords)) * material.color;
        frag_color = vec4(texel, material.transparency);
      }
    )"
  };
}
