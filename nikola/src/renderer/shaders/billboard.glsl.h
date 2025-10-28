#pragma once

#include "nikola/nikola_gfx.h"

inline nikola::GfxShaderDesc generate_billboard_shader() {
  return nikola::GfxShaderDesc {
    .vertex_source = R"(
      #version 460 core

      // Layouts
      
      layout (location = 0) in vec3 aPos;
      layout (location = 1) in vec3 aNormal;
      layout (location = 2) in vec2 aTexCoords;

      // Uniforms

      layout(std140, binding = 0) uniform Matrices {
        mat4 u_view;
        mat4 u_projection;
        vec3 u_camera_pos;
      };
 
      layout(std430, binding = 1) readonly buffer ModelsBuffer {
        mat4 u_model[4096];
      };
  
      // Outputs
      out VS_OUT {
        vec2 tex_coords;
        vec3 normal;

        vec3 camera_pos;
        flat int material_index;
      } vs_out;
      
      void main() {
        vs_out.tex_coords     = aTexCoords;
        vs_out.normal         = aNormal; 
        vs_out.camera_pos     = u_camera_pos;
        vs_out.material_index = gl_DrawID;
        
        // @NOTE (23/7/2025, Mohamed): 
        //
        // This is kind of cheating to achieve the billboard look. 
        // We are effectively "canceling" the rotation of the object
        // to make it always point towards the camera. 
        //
        // We can probably use a spherical billboard to have a better-looking 
        // billboard. But, for now, this will do.
        //
        
        mat4 view = u_view; // We can't edit the uniforms directly. GLSL, man.

        view[0][0] = 1;
        view[0][1] = 0;
        view[0][2] = 0;
        
        view[1][0] = 0;
        view[1][1] = 1;
        view[1][2] = 0;
        
        view[2][0] = 0;
        view[2][1] = 0;
        view[2][2] = 1;

        gl_Position = u_projection * view * u_model[gl_DrawID] * vec4(aPos, 1.0);
      }
    )",

    .pixel_source = R"(
      #version 460 core
      #extension GL_ARB_bindless_texture : require
   
      layout (location = 0) out vec4 frag_color;
    
      in VS_OUT {
        vec2 tex_coords;
        vec3 normal;

        vec3 camera_pos;
        flat int material_index;
      } fs_in;

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
        Material material = u_materials[fs_in.material_index];

        vec3 texel = vec3(texture(u_textures[material.albedo_index], fs_in.tex_coords)) * material.color;
        frag_color = vec4(texel, material.transparency);
      }
    )"
  };
}

