#pragma once

#include "nikola/nikola_gfx.h"

inline nikola::GfxShaderDesc generate_ssao_shader() {
  return nikola::GfxShaderDesc {
    .vertex_source = R"(
      #version 460 core
      
      layout (location = 0) in vec2 aPos;
      layout (location = 1) in vec2 aTextureCoords;
      
      out VS_OUT {
        vec2 tex_coords;
      } vs_out;
      
      void main() {
        vs_out.tex_coords = aTextureCoords;
        gl_Position       = vec4(aPos, 0.0, 1.0);
      }
    )",

    .pixel_source = R"(
      #version 460 core
      
      layout (location = 0) out vec4 frag_color;
      
      in VS_OUT {
        vec2 tex_coords;
      } fs_in;

      uniform float u_near, u_far;
      
      uniform sampler2D u_color;
      uniform sampler2D u_depth;
     
      float linearize_depth(const float depth) {
        float z = depth * 2.0 - 1.0; 
        return (2.0 * u_near * u_far) / (u_far + u_near - z * (u_far - u_near));
      }

      void main() {
        float depth        = texture(u_depth, fs_in.tex_coords).r;
        float linear_depth = linearize_depth(depth); 

        frag_color = vec4(vec3(depth), 1.0);
      }
    )"
  };
}
