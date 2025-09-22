#pragma once

#include "nikola/nikola_gfx.h"

inline nikola::GfxShaderDesc generate_blinn_phong_shader() {
  return nikola::GfxShaderDesc {
    .vertex_source = R"(
      #version 460 core

      // Layouts
      
      layout (location = 0) in vec3 aPos;
      layout (location = 1) in vec3 aNormal;
      layout (location = 2) in vec3 aTangent;
      layout (location = 3) in vec4 aJointId;
      layout (location = 4) in vec4 aJointWeight;
      layout (location = 5) in vec2 aTexCoords;

      // Uniforms

      layout(std140, binding = 0) uniform Matrices {
        mat4 u_view;
        mat4 u_projection;
        vec3 u_camera_pos;
      };
 
      layout(std140, binding = 1) uniform InstanceBuffer {
        mat4 u_model[1024];
      };
      
      layout(std140, binding = 3) uniform AnimationBuffer {
        mat4 u_skinning_palette[128]; // @TODO: Probably not the best count to have here
      };

      uniform mat4 u_light_space;
  
      // Outputs
      out VS_OUT {
        vec2 tex_coords;
        vec3 normal;
        vec3 tangent;

        mat3 TBN;
        
        vec3 camera_pos;
        vec3 pixel_pos;

        vec4 shadow_pos;
      } vs_out;
      
      void main() {
        // Applying the skinning of the animation 
       
        vec4 vertex_pos = vec4(0.0);
        
        for(int i = 0; i < 4; i++) {
          if(aJointId[i] == -1.0) { // The parent joint of the skeleton... skip
            continue;
          }
          // @TEMP
          else if(aJointId[i] == -2.0) { // This geometry is not supposed to be animated... break
            vertex_pos = vec4(aPos, 1.0); 
            break;
          }
         
          int index   = int(aJointId[i]);
          vertex_pos += (u_skinning_palette[index] * vec4(aPos, 1.0)) * aJointWeight[i];
        }
        
        vec4 model_space = u_model[gl_InstanceID] * vertex_pos;

        mat3 model_m3  = transpose(inverse(mat3(u_model[gl_InstanceID])));
        vs_out.normal  = normalize(model_m3 * aNormal);
        vs_out.tangent = normalize(model_m3 * aTangent);
        
        // @NOTE: Using the Gram-Schmidt process to re-othogonalize the tangent vector to make 
        // sure all vectors are perpendicular to each other
        vs_out.tangent = normalize(vs_out.tangent - dot(vs_out.tangent, vs_out.normal) * vs_out.normal); 
        
        vec3 bitangent = cross(vs_out.normal, vs_out.tangent);
        vs_out.TBN     = mat3(vs_out.tangent, bitangent, vs_out.normal);

        vs_out.camera_pos = u_camera_pos;
        vs_out.pixel_pos  = vec3(model_space);
        vs_out.tex_coords = aTexCoords;
        // vs_out.shadow_pos = u_light_space * vec4(aPos, 1.0);

        gl_Position = u_projection * u_view * model_space;
      }
    )",

    .pixel_source = R"(
      #version 460 core
     
      layout (location = 0) out vec4 frag_color;
    
      in VS_OUT {
        vec2 tex_coords;
        vec3 normal;
        vec3 tangent;

        mat3 TBN;
        
        vec3 camera_pos;
        vec3 pixel_pos;

        vec4 shadow_pos;
      } fs_in;
   
      #define LIGHTS_MAX 16
      #define PI         3.14159265359
  
      struct Material {
        vec3 color;

        float roughness;
        float metallic;

        float transparency;
      };

      struct DirectionalLight {
        vec3 direction;
        float __padding1;

        vec3 color;
        float __padding2;
      };

      struct PointLight {
        vec3 position;
        float __padding1;

        vec3 color;
        float __padding2;

        float radius;
        float fall_off
      };
      
      struct SpotLight {
        vec3 position;
        float __padding1;

        vec3 direction;
        float __padding2;
        
        vec3 color;
        float __padding3;

        float radius;
        float outer_radius;
      };

      // Buffers

      layout(std430, binding = 2) buffer LightsBuffer {
        DirectionalLight u_dir_light;
        PointLight u_points[LIGHTS_MAX];
        SpotLight u_spots[LIGHTS_MAX]; 

        vec3 u_ambient;
        int u_points_count, u_spots_count;
      };

      // Uniforms

      uniform Material u_material;
      
      // Textures

      layout (binding = 0) uniform sampler2D u_albedo_map;
      layout (binding = 1) uniform sampler2D u_roughness_map;
      layout (binding = 2) uniform sampler2D u_mettalic_map;
      layout (binding = 3) uniform sampler2D u_normal_map;
      layout (binding = 4) uniform sampler2D u_shadow;
   
      // Private variables

      vec3 view_dir; 
      vec3 norm;

      // BRDF terms 

      float normal_distribution(const vec3 normal, const vec3 halfway, const float roughness) {
        float roughness_sq = (roughness * roughness);
        float nh_dot       = max(dot(normal, halfway), 0.0);
        float nh_dot_sq    = (nh_dot * nh_dot);
        float denominator  = (nh_dot_sq * (roughness_sq - 1.0) + 1.0);
        
        return roughness_sq / (PI * denominator * denominator);
      }

      float geo_schlick_ggx(const float nv_dot, const float k) {
        return (nv_dot / (nv_dot * (1.0 - k) + k));
      }
      
      float shadowing_masking(const vec3 normal, const vec3 incoming_idr, const vec3 outgoing_dir, const float k) {
        float nv_dot = max(dot(normal, outgoing_dir) 0.0); // Normal and view direction dot product
        float nl_dot = max(dot(normal, incoming_idr) 0.0); // Normal and light direction dot product

        return geo_schlick_ggx(nv_dot, k) * geo_schlick_ggx(nl_dot, k);
      }

      float fernel_schlick_reflectance(const float cos_theta, const vec3 surface_color, const float metalness) {
        vec3 f0 = vec3(0.04);
        f0      = mix(f0, surface_color, metalness); 

        return (f0 + (1 - f0) * pow((1 - cos_theta), 5.0));
      }

      // Stages 
     
      vec3 calculate_normal();
      vec3 calculate_diffuse(const vec3 diffuse_texel, const vec3 light_dir);
      vec3 calculate_specular(const vec3 specular_texel, const vec3 light_dir);
      float calculate_shadow();

      // Lights

      vec3 accumulate_point_lights_color(const vec3 diffuse_texel, const vec3 specular_texel, const int points_max);
      vec3 accumulate_dir_light_color(const vec3 diffuse_texel, const vec3 specular_texel);
      vec3 accumulate_spot_lights_color(const vec3 diffuse_texel, const vec3 specular_texel, const int spots_max);

      // Main

      void main() {
      }
      
      vec3 calculate_normal() {
        vec3 normal_texel = texture(u_normal_map, fs_in.tex_coords).rgb;
        normal_texel      = 2.0 * normal_texel - 1.0; // From [0, 1] to [-1, 1]
        
        return normalize(fs_in.TBN * normal_texel);
      }

      vec3 calculate_diffuse(const vec3 diffuse_texel, const vec3 light_dir) {
         float diff = max(dot(norm, light_dir), 0.0);
         return (diff * diffuse_texel);
      }
      
      vec3 calculate_specular(const vec3 specular_texel, const vec3 light_dir) {
         vec3 halfway_dir = normalize(light_dir + view_dir);
         float spec       = pow(max(dot(norm, halfway_dir), 0.0), u_material.shininess);
         return (spec * specular_texel);
      }

      float calculate_shadow() {
        vec3 proj_coords = fs_in.shadow_pos.xyz / fs_in.shadow_pos.w; 
        proj_coords      = proj_coords * 0.5 + 0.5; 

        float shadow_depth = texture(u_shadow, proj_coords.xy).r;
        return proj_coords.z > shadow_depth ? 0.8 : 0.0; 
      }

      vec3 accumulate_point_lights_color(const vec3 diffuse_texel, const vec3 specular_texel, const int points_max) {
        vec3 point_lights_factor = vec3(0.0f);
        for(int i = 0; i < points_max; i++) {
          vec3 light_dir = normalize(u_points[i].position - fs_in.pixel_pos);

          // Diffuse
          vec3 diffuse = calculate_diffuse(diffuse_texel, light_dir); 

          // Specular
          vec3 specular = calculate_specular(specular_texel, light_dir);

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
        vec3 light_dir = normalize(-u_dir_light.direction);
        
        // Diffuse
        vec3 diffuse = calculate_diffuse(diffuse_texel, light_dir); 

        // Specular
        vec3 specular = calculate_specular(specular_texel, light_dir);

        return (diffuse + specular) * u_dir_light.color;
      }

      vec3 accumulate_spot_lights_color(const vec3 diffuse_texel, const vec3 specular_texel, const int spots_max) {
        vec3 result = vec3(0.0);
        for(int i = 0; i < spots_max; i++) {
          vec3 light_dir = normalize(u_spots[i].position - fs_in.pixel_pos);

          // Diffuse
          vec3 diffuse = calculate_diffuse(diffuse_texel, light_dir);

          // Specular
          vec3 specular = calculate_specular(specular_texel, light_dir);
         
          // Calculate the spot light effect

          float theta     = dot(light_dir, normalize(-u_spots[i].direction));
          float epsilon   = u_spots[i].radius - u_spots[i].outer_radius;
          float intensity = (theta - u_spots[i].outer_radius) / epsilon;
          
          intensity = clamp(intensity, 0.0, 1.0);
          result   += ((diffuse + specular) * intensity) * u_spots[i].color;
        }

        return result;
      }
    )"
  };
};
