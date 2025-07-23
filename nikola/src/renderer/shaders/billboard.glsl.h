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
 
      layout(std140, binding = 1) uniform InstanceBuffer {
        mat4 u_model[1024];
      };
  
      // Outputs
      out VS_OUT {
        vec2 tex_coords;
        vec3 normal;

        vec3 camera_pos;
      } vs_out;
      
      void main() {
        vs_out.tex_coords = aTexCoords;
        vs_out.normal     = aNormal; 
        vs_out.camera_pos = u_camera_pos;
        
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

        gl_Position = u_projection * view * u_model[gl_InstanceID] * vec4(aPos, 1.0);
      }
    )",

    .pixel_source = R"(
      #version 460 core
   
      layout (location = 0) out vec4 frag_color;
    
      in VS_OUT {
        vec2 tex_coords;
        vec3 normal;

        vec3 camera_pos;
      } fs_in;

      struct Material {
        vec3 color;
        float transparency;
      };

      uniform Material u_material;
      uniform sampler2D u_texture;
      
      void main() {
        vec3 texel = vec3(texture(u_texture, fs_in.tex_coords)) * u_material.color;
        frag_color = vec4(texel, u_material.transparency);
      }
    )"
  };
}

