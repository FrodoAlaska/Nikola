#pragma once

#include "nikola/nikola_gfx.h"

inline nikola::GfxShaderDesc generate_screen_space_shader() {
  return nikola::GfxShaderDesc {
    .vertex_source = R"(
      #version 460 core
      
      layout (location = 0) in vec3 aPos;
      layout (location = 1) in vec2 aTextureCoords;
      
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
      
      layout (location = 0) out vec4 frag_color;
      
      in VS_OUT {
        vec2 tex_coords;
      } fs_in;
      
      layout(binding = 0) uniform sampler2D u_input;
      
      void main() {
        frag_color = texture(u_input, fs_in.tex_coords, 1.0);
      }
    )",
  };
}

