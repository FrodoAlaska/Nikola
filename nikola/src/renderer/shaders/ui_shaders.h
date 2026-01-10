#pragma once

#include "nikola/nikola_gfx.h"

/// @NOTE:
///
/// Most of the shaders here are taken from RmlUI's repo. 
/// https://github.com/mikke89/RmlUi/blob/master/Backends/RmlUi_Renderer_GL3.cpp
///

inline const char* generate_vertex_main() {
  return R"(
      #version 460 core
      
      // Layouts 
      
      layout (location = 0) in vec2 aPos;
      layout (location = 1) in vec4 aColor;
      layout (location = 2) in vec2 aTextureCoords;
    
      // Outputs

      out VS_OUT {
        vec4 color;
        vec2 tex_coords;
      } vs_out;
   
      // Uniforms

      uniform mat4 u_transform, u_projection; 
      uniform vec2 u_translate;
      
      void main() {
        vs_out.color      = aColor;
        vs_out.tex_coords = aTextureCoords;
     
        gl_Position = u_projection * u_transform * vec4(vec2(u_translate + aPos), 0.0, 1.0);
      }
  )";
}

inline nikola::GfxShaderDesc generate_ui_texture_shader() {
  return nikola::GfxShaderDesc {
    .vertex_source = generate_vertex_main(),

    .pixel_source = R"(
      #version 460 core
      
      // Outputs
      
      layout (location = 0) out vec4 frag_color;
      
      in VS_OUT {
        vec4 color;
        vec2 tex_coords;
      } fs_in;
     
      // Uniforms
      
      layout (binding = 0) uniform sampler2D u_texture;
      
      void main() {
        frag_color = texture(u_texture, fs_in.tex_coords) * fs_in.color;
      }
    )"
  };
}

inline nikola::GfxShaderDesc generate_ui_color_shader() {
  return nikola::GfxShaderDesc {
    .vertex_source = generate_vertex_main(),

    .pixel_source = R"(
      #version 460 core
      
      // Outputs
      
      layout (location = 0) out vec4 frag_color;
      
      in VS_OUT {
        vec4 color;
        vec2 tex_coords;
      } fs_in;
      
      void main() {
        frag_color = fs_in.color;
      }
    )"
  };
}

inline nikola::GfxShaderDesc generate_ui_gradient_shader() {
  return nikola::GfxShaderDesc {
    .vertex_source = generate_vertex_main(),

    .pixel_source = R"(
      #version 460 core
     
      // Defines
     
      #define MAX_NUM_STOPS    16
      #define LINEAR           0
      #define RADIAL           1
      #define CONIC            2
      #define REPEATING_LINEAR 3
      #define REPEATING_RADIAL 4
      #define REPEATING_CONIC  5
      #define PI               3.14159265

      // Outputs
      
      layout (location = 0) out vec4 frag_color;
      
      in VS_OUT {
        vec4 color;
        vec2 tex_coords;
      } fs_in;

      // Uniforms
      
      uniform int u_func;                            // one of the above definitions
      uniform vec2 u_p;                              // linear: starting point,         radial: center,                        conic: center
      uniform vec2 u_v;                              // linear: vector to ending point, radial: 2d curvature (inverse radius), conic: angled unit vector
      uniform vec4 u_stop_colors[MAX_NUM_STOPS];
      uniform float u_stop_positions[MAX_NUM_STOPS]; // normalized, 0 -> starting point, 1 -> ending point
      uniform int u_num_stops;

      // Functions

      vec4 mix_stop_colors(float t) {
	      vec4 color = u_stop_colors[0];

	      for (int i = 1; i < u_num_stops; i++) {
		      color = mix(color, u_stop_colors[i], smoothstep(u_stop_positions[i - 1], u_stop_positions[i], t));
	        return color;
        }
      }

      void main() {
        float time = 0.0;
        
        if (u_func == LINEAR || u_func == REPEATING_LINEAR)
        {
        	float dist_square = dot(u_v, u_v);

        	vec2 V = fs_in.tex_coords - u_p;
        	time   = dot(u_v, V) / dist_square;
        }
        else if (u_func == RADIAL || u_func == REPEATING_RADIAL)
        {
        	vec2 V = fs_in.tex_coords - u_p;
        	time   = length(u_v * V);
        }
        else if (u_func == CONIC || u_func == REPEATING_CONIC)
        {
        	mat2 R = mat2(u_v.x, -u_v.y, u_v.y, u_v.x);
        	vec2 V = R * (fs_in.tex_coords - u_p);
        	time   = 0.5 + atan(-V.x, V.y) / (2.0 * PI);
        }
        
        if (u_func == REPEATING_LINEAR || u_func == REPEATING_RADIAL || u_func == REPEATING_CONIC)
        {
        	float t0 = u_stop_positions[0];
        	float t1 = u_stop_positions[u_num_stops - 1];

        	time = t0 + mod(time - t0, t1 - t0);
        }
        
        frag_color = fs_in.color * mix_stop_colors(time);
      }
    )"
  };
}

// "Creation" by Danilo Guanabara, based on: https://www.shadertoy.com/view/XsXXDn
inline nikola::GfxShaderDesc generate_ui_creation_shader() {
  return nikola::GfxShaderDesc {
    .vertex_source = generate_vertex_main(),

    .pixel_source = R"(
      #version 460 core
      
      // Outputs
      
      layout (location = 0) out vec4 frag_color;
      
      in VS_OUT {
        vec4 color;
        vec2 tex_coords;
      } fs_in;
     
      // Uniforms
      
      layout (binding = 0) uniform sampler2D u_texture;
     
      uniform float u_value;
      uniform vec2 u_dimensions;

      void main() {
        float time = u_value;

        vec3 color;
        float len;

        for (int i = 0; i < 3; i++) {
        	vec2 p  = fs_in.tex_coords;
        	vec2 uv = p;

        	p   -= 0.5;
        	p.x *= (u_dimensions.x / u_dimensions.y);

        	float z = time + float(i) * 0.07;
        	len     = length(p);

        	uv      += p / len * (sin(z) + 1.0) * abs(sin(len * 9.0 - z - z));
        	color[i] = 0.01 / length(mod(uv, 1.0) - 0.5);
        }

        frag_color = vec4(color / len, fs_in.color.a);
      }
    )"
  };
}

inline nikola::GfxShaderDesc generate_ui_color_matrix() {
  return nikola::GfxShaderDesc {
    .vertex_source = generate_vertex_main(),

    .pixel_source = R"(
      #version 460 core
      
      // Outputs
      
      layout (location = 0) out vec4 frag_color;
      
      in VS_OUT {
        vec4 color;
        vec2 tex_coords;
      } fs_in;
     
      // Uniforms
      
      layout (binding = 0) uniform sampler2D u_texture;
     
      uniform mat4 u_color_matrix;

      void main() {
        // @NOTE:
        //
        // The general case uses a 4x5 color matrix for full rgba transformation, plus a constant term with the last column.
        // However, we only consider the case of rgb transformations. Thus, we could in principle use a 3x4 matrix, but we
        // keep the alpha row for simplicity.
        // In the general case we should do the matrix transformation in non-premultiplied space. However, without alpha
        // transformations, we can do it directly in premultiplied space to avoid the extra division and multiplication
        // steps. In this space, the constant term needs to be multiplied by the alpha value, instead of unity.
        //

        vec4 texel_color       = texture(u_texture, fs_in.tex_coords);
        vec3 transformed_color = vec3(u_color_matrix * texel_color);

        frag_color = vec4(transformed_color, texel_color.a);
      }
    )"
  };
}

inline nikola::GfxShaderDesc generate_ui_blend_mask() {
  return nikola::GfxShaderDesc {
    .vertex_source = generate_vertex_main(),

    .pixel_source = R"(
      #version 460 core
      
      // Outputs
      
      layout (location = 0) out vec4 frag_color;
      
      in VS_OUT {
        vec4 color;
        vec2 tex_coords;
      } fs_in;
     
      // Uniforms
      
      layout (binding = 0) uniform sampler2D u_texture;
      layout (binding = 1) uniform sampler2D u_texture_mask;
     
      void main() {
        vec4 texel_color = texture(u_texture, fs_in.tex_coords);
        float mask_alpha = texture(u_texture_mask, fs_in.tex_coords).a;

        frag_color = texel_color * mask_alpha;
      }
    )"
  };
}
