#pragma once

#include "nikola/nikola_gfx.h"

inline nikola::GfxShaderDesc generate_blinn_phong_shader() {
  return nikola::GfxShaderDesc {
    .vertex_source = R"(
      #version 460 core

      // Layouts
      layout (location = 0) in vec3 aPos;
      layout (location = 1) in vec3 aNormal;
      layout (location = 2) in vec2 aTexCoords;

      // Uniform block

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
        vec3 pixel_pos;
      } vs_out;
      
      void main() {
        vec4 model_space = u_model[gl_InstanceID] * vec4(aPos, 1.0f);
      
        vs_out.tex_coords = aTexCoords;
        vs_out.normal     = mat3(transpose(inverse(u_model[gl_InstanceID]))) * aNormal;
        vs_out.camera_pos = u_camera_pos;
        vs_out.pixel_pos  = vec3(model_space);

        gl_Position = u_projection * u_view * model_space;
      }
    )",

    .pixel_source = R"(
      #version 460 core
     
      layout (location = 0) out vec4 frag_color;
    
      in VS_OUT {
        vec2 tex_coords;
        vec3 normal;
        
        vec3 camera_pos;
        vec3 pixel_pos;
      } fs_in;
   
      #define POINT_LIGHTS_MAX 16
  
      struct Material {
        sampler2D diffuse_map;
        sampler2D specular_map;
 
        vec3 color;
        float shininess;
        float transparency;
      };

      struct DirectionalLight {
        vec3 direction;
        vec3 color;
      };

      struct PointLight {
        vec3 position;
        vec3 color;

        float radius;
      };

      uniform Material u_material;

      uniform DirectionalLight u_dir_light;
      uniform PointLight u_point_lights[POINT_LIGHTS_MAX];
      uniform int u_point_lights_count;

      uniform vec3 u_ambient;

      vec3 accumulate_point_lights_color(const vec3 diffuse_texel, const vec3 specular_texel, const int points_max) {
        vec3 norm     = normalize(fs_in.normal);
        vec3 view_dir = normalize(fs_in.camera_pos - fs_in.pixel_pos);

        vec3 point_lights_factor = vec3(0.0f);
        for(int i = 0; i < points_max; i++) {

         // Diffuse

         vec3 light_dir = normalize(u_point_lights[i].position - fs_in.pixel_pos);
         float diff     = max(dot(norm, light_dir), 0.0);
         vec3 diffuse   = diff * diffuse_texel;

         // Specular

         vec3 halfway_dir = normalize(light_dir + view_dir);
         float spec       = pow(max(dot(norm, halfway_dir), 0.0), u_material.shininess);
         vec3 specular    = spec * specular_texel;

         // Apply attenuation

         float distance = length(light_dir);
        
         float dist_sq   = distance * distance;
         float radius_sq = u_point_lights[i].radius * u_point_lights[i].radius;
         float rd_sq     = dist_sq + radius_sq;

         // Thanks to Cem Yuksel for supplying this formula. It is _amazing_!
         float atten = 2 / (rd_sq + distance * sqrt(rd_sq));

         point_lights_factor += ((diffuse + specular) * atten) * u_point_lights[i].color;
       }

         return point_lights_factor;
      }

      vec3 accumulate_dir_light_color(const vec3 diffuse_texel, const vec3 specular_texel) {
        // Diffuse

        vec3 light_dir = normalize(-u_dir_light.direction);
        vec3 norm      = normalize(fs_in.normal);
        float diff     = max(dot(norm, light_dir), 0.0);
        vec3 diffuse   = diff * diffuse_texel;

        // Specular

        vec3 view_dir    = normalize(fs_in.camera_pos - fs_in.pixel_pos);
        vec3 halfway_dir = normalize(light_dir + view_dir);
        float spec       = pow(max(dot(norm, halfway_dir), 0.0), u_material.shininess);
        vec3 specular    = spec * specular_texel;

        return (diffuse + specular) * u_dir_light.color;
      }
 
      void main() {
        vec3 diffuse  = texture(u_material.diffuse_map, fs_in.tex_coords).rgb * u_material.color;
        vec3 specular = texture(u_material.specular_map, fs_in.tex_coords).rgb * u_material.color;
        vec3 ambient  = u_ambient * diffuse;
  
        // Clamp the max point lights
   
        int points_max = u_point_lights_count;
        if(points_max > POINT_LIGHTS_MAX) {
          points_max = POINT_LIGHTS_MAX;
        }
    
        // Gather the light factors from all the light sources
     
        vec3 point_lights_factor = accumulate_point_lights_color(diffuse, specular, points_max);
        vec3 dir_light_factor    = accumulate_dir_light_color(diffuse, specular);

        vec3 final_factor = ambient + point_lights_factor + dir_light_factor;

        frag_color = vec4(final_factor, u_material.transparency);
      }
    )"
  };
};
