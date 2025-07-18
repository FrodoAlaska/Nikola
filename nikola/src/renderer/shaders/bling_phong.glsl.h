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

      // Uniforms

      layout(std140, binding = 0) uniform Matrices {
        mat4 u_view;
        mat4 u_projection;
        vec3 u_camera_pos;
      };
 
      layout(std140, binding = 1) uniform InstanceBuffer {
        mat4 u_model[1024];
      };

      uniform mat4 u_light_space;
  
      // Outputs
      out VS_OUT {
        vec2 tex_coords;
        vec3 normal;
        
        vec3 camera_pos;
        vec3 pixel_pos;

        vec4 shadow_pos;
      } vs_out;
      
      void main() {
        vec4 model_space = u_model[gl_InstanceID] * vec4(aPos, 1.0);
      
        vs_out.tex_coords = aTexCoords;
        vs_out.normal     = mat3(transpose(inverse(u_model[gl_InstanceID]))) * aNormal;
        vs_out.camera_pos = u_camera_pos;
        vs_out.pixel_pos  = vec3(model_space);

        vs_out.shadow_pos = u_light_space * vec4(aPos, 1.0);

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

        vec4 shadow_pos;
      } fs_in;
   
      #define LIGHTS_MAX 16
  
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
      
      struct SpotLight {
        vec3 position;
        vec3 direction;
        vec3 color;

        float radius;
        float outer_radius;
      };

      uniform DirectionalLight u_dir_light; 
      uniform PointLight u_points[LIGHTS_MAX]; 
      uniform SpotLight u_spots[LIGHTS_MAX]; 
      
      uniform int u_points_count; 
      uniform int u_spots_count; 
      uniform vec3 u_ambient;
      
      uniform Material u_material;
      uniform sampler2D u_shadow;

      float calculate_shadow() {
        float shadow_depth = textureProj(u_shadow, fs_in.shadow_pos.xyw).r;

        return shadow_depth > fs_in.shadow_pos.z ? 0.0 : 0.8; 
        // return textureProj(u_shadow, fs_in.shadow_pos);
      }

      vec3 accumulate_point_lights_color(const vec3 diffuse_texel, const vec3 specular_texel, const int points_max) {
        vec3 norm     = normalize(fs_in.normal);
        vec3 view_dir = normalize(fs_in.camera_pos - fs_in.pixel_pos);

        vec3 point_lights_factor = vec3(0.0f);
        for(int i = 0; i < points_max; i++) {

         // Diffuse

         vec3 light_dir = normalize(u_points[i].position - fs_in.pixel_pos);
         float diff     = max(dot(norm, light_dir), 0.0);
         vec3 diffuse   = diff * diffuse_texel;

         // Specular

         vec3 halfway_dir = normalize(light_dir + view_dir);
         float spec       = pow(max(dot(norm, halfway_dir), 0.0), u_material.shininess);
         vec3 specular    = spec * specular_texel;

         // Apply attenuation

         float distance = length(light_dir);
        
         float dist_sq   = distance * distance;
         float radius_sq = u_points[i].radius * u_points[i].radius;
         float rd_sq     = dist_sq + radius_sq;

         // Thanks to Cem Yuksel for supplying this formula. It is _amazing_!
         float atten = (2 / radius_sq) * (1 - (dist_sq / sqrt(dist_sq + radius_sq)));

         point_lights_factor += ((diffuse + specular) * atten) * u_points[i].color;
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

      vec3 accumulate_spot_lights_color(const vec3 ambient, const vec3 diffuse_texel, const vec3 specular_texel, const int spots_max) {
        vec3 norm = normalize(fs_in.normal);
        
        vec3 result = vec3(0.0);
        for(int i = 0; i < spots_max; i++) {
          vec3 light_dir = normalize(u_spots[i].position - fs_in.pixel_pos);

          // Diffuse

          float diff   = max(dot(norm, light_dir), 0.0);
          vec3 diffuse = diff * diffuse_texel;

          // Specular

          vec3 view_dir    = normalize(fs_in.camera_pos - fs_in.pixel_pos);
          vec3 halfway_dir = normalize(light_dir + view_dir);
          float spec       = pow(max(dot(norm, halfway_dir), 0.0), u_material.shininess);
          vec3 specular    = spec * specular_texel;
          
          float theta     = dot(light_dir, normalize(-u_spots[i].direction));
          float epsilon   = u_spots[i].radius - u_spots[i].outer_radius;
          float intensity = (theta - u_spots[i].outer_radius) / epsilon;
          
          intensity = clamp(intensity, 0.0, 1.0);
          result += ((diffuse + specular) * intensity) * u_spots[i].color;
        }

        return result;
      }
 
      void main() {
        vec3 diffuse  = texture(u_material.diffuse_map, fs_in.tex_coords).rgb * u_material.color;
        // @TEMP
        vec3 specular = texture(u_material.specular_map, fs_in.tex_coords).rgb * 0.0;
        vec3 ambient  = u_ambient * diffuse;
  
        // Clamp the maximum number of lights
        // @TODO: This can be improved...
   
        int points_max = u_points_count;
        if(points_max > LIGHTS_MAX) {
          points_max = LIGHTS_MAX;
        }

        int spots_max = u_spots_count;
        if(spots_max > LIGHTS_MAX) {
          spots_max = LIGHTS_MAX;
        }
    
        // Gather the light factors from all the light sources
     
        vec3 point_lights_factor = accumulate_point_lights_color(diffuse, specular, points_max);
        vec3 spot_lights_factor  = accumulate_spot_lights_color(ambient, diffuse, specular, spots_max);
        vec3 dir_light_factor    = (ambient + (1.0 - calculate_shadow())) * accumulate_dir_light_color(diffuse, specular);

        vec3 final_factor = (point_lights_factor + spot_lights_factor + dir_light_factor);

        frag_color = vec4(final_factor, u_material.transparency);
      }
    )"
  };
};
