#pragma once

#include "nikola/nikola_gfx.h"

inline nikola::GfxShaderDesc generate_batch_quad_shader() {
  return nikola::GfxShaderDesc {
    .vertex_source = R"(
      #version 460 core
     
      // Layouts
      
      layout (location = 0) in vec2 aPos;
      layout (location = 1) in vec2 aTextureCoords;
      layout (location = 2) in vec4 aColor;
      layout (location = 3) in float aMaterialIndex;
    
      // Outputs
      
      out VS_OUT {
        vec2 tex_coords;
        vec2 pixel_pos;
        vec4 out_color;
   
        flat int material_index;
      } vs_out;

      // Uniforms 
      uniform mat4 u_ortho;
  
      void main() {
        vs_out.tex_coords     = aTextureCoords;
        vs_out.pixel_pos      = aPos;
        vs_out.out_color      = aColor;
        vs_out.material_index = int(aMaterialIndex);

        gl_Position = u_ortho * vec4(aPos, 0.0f, 1.0f);
      }
    )",
 
    .pixel_source = R"(
      #version 460 core
     
      // Outputs
      layout (location = 0) out vec4 frag_color;
  
      // Defines
      
      #define PI     3.14159265359
      #define TWO_PI 6.28318530718
    
      // Inputs
      
      in VS_OUT {
        vec2 tex_coords;
        vec2 pixel_pos;
        vec4 out_color;
   
        flat int material_index;
      } fs_in;

      // Material2D

      struct Material2D {
        vec2 size; 
        vec2 __padding0;

        float radius;
        float sides_count; 
        float shape_type;

        float __padding1;
      };

      // Uniforms
      uniform sampler2D u_texture;

      // Buffers 

      layout(std430, binding = 5) readonly buffer Material2DBuffer {
        Material2D u_materials[1024];
      };

      // Functions

      vec4 quad_shape(Material2D material) {
        // This helped a lot: https://www.shadertoy.com/view/Nlc3zf

        vec2 position  = (fs_in.tex_coords - 0.5) * material.size;
        vec2 half_size = material.size * 0.5f;

        vec2 abs_pos = abs(position) - half_size + material.radius;
        float sdf    = min(max(abs_pos.x, abs_pos.y), 0.0) + length(max(abs_pos, 0.0)) - material.radius;

        float dist = 1.0 - smoothstep(0.0, fwidth(sdf), sdf);
        if(dist <= 0.0) {
          discard;
        }

        return texture(u_texture, fs_in.tex_coords) * fs_in.out_color;
      }

      vec4 circle_shape(Material2D material) {
        vec2 uv = fs_in.tex_coords.xy * 2.0 - 1.0;

        float dist = 1.0 - length(uv);
        if(dist < 0.0) {
          discard;
        }

        return texture(u_texture, fs_in.tex_coords) * fs_in.out_color;
      }

      vec4 polygon_shape(Material2D material) {
        vec2 uv = fs_in.tex_coords.xy * 2.0 - 1.0;

        float angle  = atan(uv.x, uv.y);
        float radius = TWO_PI / material.sides_count;

        float d   = cos(floor(0.5 + angle / material.radius) * material.radius - angle) * length(uv);
        float val = 1.0 - smoothstep(0.4, 0.41, d);

        if(val <= 0.0) {
          discard;
        }

        return texture(u_texture, fs_in.tex_coords) * fs_in.out_color;
      }

      vec4 text_shape(Material2D material) {
        vec4 color = vec4(1.0, 1.0, 1.0, texture(u_texture, fs_in.tex_coords).r);
        return fs_in.out_color * color;
      }

      // Main

      void main() {
        Material2D material = u_materials[fs_in.material_index];
        vec4 color          = vec4(0.0, 0.0, 1.0, 1.0);

        if(material.shape_type == 0.0) {
          color = quad_shape(material);
        }
        else if(material.shape_type == 1.0) {
          color = circle_shape(material);
        }
        else if(material.shape_type == 2.0) {
          color = polygon_shape(material);
        }
        else if(material.shape_type == 3.0) {
          color = text_shape(material);
        }
      
        frag_color = color;
      }
    )"
  };
}
