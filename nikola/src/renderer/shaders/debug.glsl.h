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

        int index   = gl_BaseInstance + gl_InstanceID;
        gl_Position = u_projection * u_view * u_model[index] * vec4(aPos, 1.0);
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
        sampler2D albedo_handle;
        sampler2D metallic_handle;
        sampler2D roughness_handle;
        sampler2D normal_handle;
        sampler2D emissive_handle;

        float metallic;
        float roughness;
        float emissive;
        float transparency;
        vec2 __padding;

        vec3 color;
      };

      layout(std430, binding = 2) readonly buffer MaterialsBuffer {
        Material u_materials[4096];
      };

      void main() {
        Material material = u_materials[material_index];

        vec3 texel = vec3(texture(material.albedo_handle, texture_coords)) * material.color;
        frag_color = vec4(texel, material.transparency);
      }
    )"
  };
}
