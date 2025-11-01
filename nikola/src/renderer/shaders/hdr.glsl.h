#pragma once

#include "nikola/nikola_gfx.h"

inline nikola::GfxShaderDesc generate_hdr_shader() {
  return nikola::GfxShaderDesc {
    .vertex_source = R"(
      #version 460 core
      
      layout (location = 0) in vec3 aPos;
      layout (location = 1) in vec3 aNormal; // Unsused
      layout (location = 2) in vec2 aTextureCoords;
      
      out VS_OUT {
        vec2 tex_coords;
      } vs_out;
      
      void main() {
        vs_out.tex_coords = aTextureCoords;
        gl_Position       = vec4(aPos, 1.0);
      }
    )",

    .pixel_source = R"(
      #version 460 core
      #extension GL_ARB_bindless_texture : require
      
      layout (location = 0) out vec4 frag_color;
      
      in VS_OUT {
        vec2 tex_coords;
      } fs_in;
    
      const float GAMMA = 2.0;

      layout (binding = 0) uniform sampler2D u_input;
      uniform float u_exposure;
      
      void main() {
        vec3 hdr_color = texture(u_input, fs_in.tex_coords).rgb;
        vec3 mapped    = vec3(1.0) - exp(-hdr_color * u_exposure);
     
        mapped     = pow(mapped, vec3(1.0 / GAMMA));
        frag_color = vec4(mapped, 1.0);
      }
    )"
  };
}
