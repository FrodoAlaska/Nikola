#pragma once

#include "nikola/nikola_gfx.h"

inline nikola::GfxShaderDesc generate_batch_quad_shader() {
  return nikola::GfxShaderDesc {
    .vertex_source = R"(
      #version 460 core
     
      // Layouts
      layout (location = 0) in vec2 aPos;
      layout (location = 1) in vec4 aColor;
      layout (location = 2) in vec2 aTextureCoords;
      layout (location = 3) in vec2 aShapeSide;
    
      // Outputs
      out VS_OUT {
        vec4 out_color;
        vec2 tex_coords;
   
        float shape_type;
        float sides_count;
      } vs_out;
  
      void main() {
        vs_out.out_color  = aColor;
        vs_out.tex_coords = aTextureCoords;
 
        vs_out.shape_type  = aShapeSide.x;
        vs_out.sides_count = aShapeSide.y;

        gl_Position = vec4(aPos, 0.0f, 1.0f);
      }
    )",
 
    .pixel_source = R"(
      #version 460 core
     
      // Outputs
      layout (location = 0) out vec4 frag_color;
    
      // Inputs
      in VS_OUT {
        vec4 out_color;
        vec2 tex_coords;
   
        float shape_type;
        float sides_count;
      } fs_in;
  
      // Defines
      #define PI     3.14159265359
      #define TWO_PI 6.28318530718
 
      // Uniforms
      uniform sampler2D u_texture;

      vec4 quad_shape() {
        return texture(u_texture, fs_in.tex_coords) * fs_in.out_color;
      }

      vec4 circle_shape() {
        vec2 uv = fs_in.tex_coords.xy * 2.0 - 1.0;

        float dist = 1.0 - length(uv);
        if(dist < 0.0) {
          discard;
        }

        return fs_in.out_color;
      }

      vec4 polygon_shape() {
        vec2 uv = fs_in.tex_coords.xy * 2.0 - 1.0;

        float angle  = atan(uv.x, uv.y);
        float radius = TWO_PI / fs_in.sides_count;

        float d   = cos(floor(0.5 + angle / radius) * radius - angle) * length(uv);
        float val = 1.0 - smoothstep(0.4, 0.41, d);
        if(val <= 0.0) {
          discard;
        }

        return fs_in.out_color;
      }

      vec4 text_shape() {
        vec4 color = vec4(1.0, 1.0, 1.0, texture(u_texture, fs_in.tex_coords).r);
        return fs_in.out_color * color;
      }

      void main() {
        vec4 color = vec4(1.0, 0.0, 1.0, 1.0);

        if(fs_in.shape_type == 0.0) {
          color = quad_shape();
        }
        else if(fs_in.shape_type == 1.0) {
          color = circle_shape();
        }
        else if(fs_in.shape_type == 2.0) {
          color = polygon_shape();
        }
        else if(fs_in.shape_type == 3.0) {
          color = text_shape();
        }
      
        frag_color = color;
      }
    )"
  };
}
