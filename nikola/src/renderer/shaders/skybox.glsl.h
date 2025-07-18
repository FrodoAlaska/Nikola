#pragma once

#include "nikola/nikola_gfx.h"

inline nikola::GfxShaderDesc generate_skybox_shader() {
  return nikola::GfxShaderDesc {
    .vertex_source = R"(
      #version 460 core
     
      layout (location = 0) in vec3 aTextureCoords;
     
      out VS_OUT {
        vec3 tex_coords;
      } vs_out;
     
      layout (std140, binding = 0) uniform Matrices {
        mat4 u_view;
        mat4 u_projection;
        vec3 u_camera_pos;
      };
     
      void main() {
        vs_out.tex_coords = aTextureCoords;
     
        mat4 model  = u_projection * mat4(mat3(u_view));
        gl_Position = vec4(model * vec4(aTextureCoords, 1.0)).xyww;
      }
    )",

    .pixel_source = R"(
      #version 460 core
      
      layout (location = 0) out vec4 frag_color;
      
      in VS_OUT {
        vec3 tex_coords;
      } fs_in;
      
      layout (binding = 0) uniform samplerCube u_cubemap;
      
      void main() {
        frag_color = texture(u_cubemap, fs_in.tex_coords);
      }
    )"
  };
}

