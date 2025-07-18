#pragma once

#include "nikola/nikola_gfx.h"

inline nikola::GfxShaderDesc generate_default_shader() {
  return nikola::GfxShaderDesc {
    .vertex_source = R"(
      #version 460 core
     
      layout (location = 0) in vec3 aPos;
      layout (location = 1) in vec3 aNormal;
      layout (location = 2) in vec2 aTextureCoords;
    
      out VS_OUT {
        vec2 tex_coords;
        vec3 normal;
      } vs_out;
   
      layout (std140, binding = 0) uniform Matrices {
        mat4 u_view;
        mat4 u_projection;
        vec3 u_camera_pos;
      };
    
      uniform mat4 u_model;
     
      void main() {
        vs_out.tex_coords = aTextureCoords;
        vs_out.normal     = aNormal;
     
        vec4 world_pos = u_model * vec4(aPos, 1.0);
        gl_Position    = u_projection * u_view * world_pos;
      }
    )",

    .pixel_source = R"(
      #version 460 core
   
      layout (location = 0) out vec4 frag_color;
    
      in VS_OUT {
        vec2 tex_coords;
        vec3 normal;
      } fs_in;
     
      struct Material {
        sampler2D diffuse_map;
        sampler2D specular_map;
      
        vec3 color;
        float shininess;
        float transparency;
      };"
      
      uniform Material u_material;
      
      void main() 
        frag_color = vec4(u_material.color, u_material.transparency);
      }
    )"
  };
}

