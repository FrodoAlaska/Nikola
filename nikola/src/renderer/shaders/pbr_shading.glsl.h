#pragma once

#include "nikola/nikola_gfx.h"

inline nikola::GfxShaderDesc generate_pbr_shader() {
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
        vs_out.tangent = normalize(model_m3 * aTangent);
        vs_out.normal  = normalize(model_m3 * aNormal);
        
        // @NOTE: Using the Gram-Schmidt process to re-othogonalize the tangent vector to make 
        // sure all vectors are perpendicular to each other
        vs_out.tangent = normalize(vs_out.tangent - dot(vs_out.tangent, vs_out.normal) * vs_out.normal); 
        
        vec3 bitangent = normalize(cross(vs_out.normal, vs_out.tangent));
        vs_out.TBN     = transpose(mat3(vs_out.tangent, bitangent, vs_out.normal));

        vs_out.camera_pos = u_camera_pos;
        vs_out.pixel_pos  = vec3(model_space);
        vs_out.tex_coords = aTexCoords;
        vs_out.shadow_pos = u_light_space * vec4(vs_out.pixel_pos, 1.0);

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
        float emissive;

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
        float fall_off;
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

      struct BRDFDesc {
        vec3 f0;

        vec3 normal; 
        vec3 view_dir; 
        vec3 light_dir; 

        float roughness; 
        float metallic;

        vec3 albedo_texel;
        vec3 radiance;
      };

      struct BRDFResult {
        vec3 specular; 
        vec3 diffuse;

        vec3 radiance_color;
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
      layout (binding = 4) uniform sampler2D u_emissive_map;
      layout (binding = 5) uniform sampler2D u_shadow_map;
   
      // BRDF terms 

      float normal_distribution(const vec3 normal, const vec3 halfway, const float roughness) {
        float roughness_sq  = (roughness * roughness);
        float roughness2_sq = (roughness_sq * roughness_sq);
        
        float nh_dot       = max(dot(normal, halfway), 0.0);
        float nh_dot_sq    = (nh_dot * nh_dot);
        float denominator  = (nh_dot_sq * (roughness2_sq - 1.0) + 1.0);
        
        return roughness2_sq / (PI * denominator * denominator);
      }

      float geo_schlick_ggx(const float nx_dot, const float roughness) {
        float k = roughness / 2.0;
        return (nx_dot / (nx_dot * (1.0 - k) + k));
      }
      
      float shadowing_masking(const vec3 normal, const vec3 view_dir, const vec3 light_dir, const float roughness) {
        float nv_dot = max(dot(normal, view_dir), 0.0);
        float nl_dot = max(dot(normal, light_dir), 0.0);

        return geo_schlick_ggx(nv_dot, roughness) * geo_schlick_ggx(nl_dot, roughness);
      }
      
      vec3 fersnel_schlick_reflectance(const float cos_theta, const vec3 f0) {
        return f0 + (1 - f0) * pow(clamp((1.0 - cos_theta), 0.0, 1.0), 5.0);
      }

      // Stages 

      BRDFResult calculate_brdf(const BRDFDesc desc) {
        vec3 halfway_dir = normalize(desc.light_dir + desc.view_dir);
        float hv_dot     = max(dot(halfway_dir, desc.view_dir), 0.0);
        float nv_dot     = max(dot(desc.normal, desc.view_dir), 0.0);
        float nl_dot     = max(dot(desc.normal, desc.light_dir), 0.0);

        float ndf = normal_distribution(desc.normal, halfway_dir, desc.roughness);
        float g   = shadowing_masking(desc.normal, desc.view_dir, desc.light_dir, desc.roughness);
        vec3 f    = fersnel_schlick_reflectance(hv_dot, desc.f0); 

        vec3 numerator    = ndf * g * f;
        float denominator = 4 / (nv_dot * nl_dot) + 0.0001;

        BRDFResult res; 
        res.specular       = (numerator / denominator);
        res.diffuse        = (vec3(1.0) - f) * (1.0 - desc.metallic);
        res.radiance_color = (res.diffuse * desc.albedo_texel / PI + res.specular) * desc.radiance * nl_dot;

        return res;
      }

      vec3 calculate_normal(const vec3 normal_texel) {
        vec3 mapped_normal = 2.0 * normal_texel - 1.0; // From [0, 1] to [-1, 1]

        vec3 q1 = dFdx(fs_in.pixel_pos);
        vec3 q2 = dFdy(fs_in.pixel_pos);
        vec2 st1 = dFdx(fs_in.tex_coords);
        vec2 st2 = dFdy(fs_in.tex_coords);

        vec3 normal    = normalize(fs_in.normal);
        vec3 tangent   = normalize(q1 * st2.t - q2 * st1.t); 
        vec3 bitangent = -normalize(cross(normal, tangent)); 

        mat3 TBN = mat3(tangent, bitangent, normal);
        return normalize(TBN * mapped_normal);
      }

      float calculate_shadow() {
        // Converting the shadow space position to texture coordinates
        
        vec3 proj_coords = (fs_in.shadow_pos.xyz / fs_in.shadow_pos.w) * 0.5f + 0.5f;
        if(proj_coords.z > 1.0) { // Early out for objects too far away
          return 0.0;
        }

        // @TEMP (Shadows): Have a better bias setup here...
        float bias = 0.005;

        // Applying a simple PCF (Percentage-closer filtering)
        // @TEMP (Shadows): Have a better PCF algo here...

        float shadow_factor = 0.0; 
        vec2 texture_size   = 1.0 / textureSize(u_shadow_map, 0);

        for(int x = -1; x < 1; x++) {
          for(int y = -1; y < 1; y++) {
            float pcf_depth = texture(u_shadow_map, proj_coords.xy + vec2(x, y) * texture_size).r;
            shadow_factor  += (proj_coords.z - bias) > pcf_depth ? 1.0 : 0.0;
          }
        }

        // Final shadow value
        return (shadow_factor /= 9.0);
      }  

      // Lights
      
      float attenuate(const float distance, const float radius, const float max_intensity, const float fall_off) {
        /// @NOTE: 
        /// This attenuation function was taken from:
        /// https://lisyarus.github.io/blog/posts/point-light-attenuation.html
        /// 

        float s = distance / radius;
        if(s >= 1.0) {
          return 0.0;
        }

        float s2_sq  = (s * s);
        float inv_s2 = (1 - s2_sq);

        return max_intensity * (inv_s2 * inv_s2) / (1 + fall_off * s);
      }

      vec3 evaluate_directional_light(const DirectionalLight light, BRDFDesc brdf) {
        brdf.light_dir = normalize(light.direction);
        brdf.radiance  = light.color;

        BRDFResult res = calculate_brdf(brdf);
        return res.radiance_color;
      }

      vec3 evaluate_point_light(const PointLight light, BRDFDesc brdf) {
        brdf.light_dir = normalize(light.position - fs_in.pixel_pos);
        brdf.radiance  = light.color * attenuate(length(brdf.light_dir), light.radius, light.color.r, light.fall_off);

        BRDFResult res = calculate_brdf(brdf);
        return res.radiance_color;
      }

      vec3 evaluate_spot_light(const SpotLight light, BRDFDesc brdf) {
        vec3 light_dir = normalize(light.position - fs_in.pixel_pos);
         
        // Calculate the spot light effect

        float theta     = dot(light_dir, normalize(-light.direction));
        float epsilon   = light.radius - light.outer_radius;
        float intensity = (theta - light.outer_radius) / epsilon;
        intensity       = clamp(intensity, 0.0, 1.0);

        // Set the final parameters of the BRDF

        brdf.light_dir = light_dir;
        brdf.radiance  = light.color * intensity; 

        // Done!
        
        BRDFResult res = calculate_brdf(brdf);
        return res.radiance_color;
      }

      // Main

      void main() {
        // Sampling the many textures 

        vec3 albedo_texel     = texture(u_albedo_map, fs_in.tex_coords).rgb * u_material.color;
        float roughness_texel = texture(u_roughness_map, fs_in.tex_coords).g * u_material.roughness;
        float metallic_texel  = texture(u_mettalic_map, fs_in.tex_coords).b * u_material.metallic;
        vec3 normal_texel     = texture(u_normal_map, fs_in.tex_coords).rgb;
        vec3 emissive_texel   = texture(u_emissive_map, fs_in.tex_coords).rgb * u_material.emissive;
        
        // Preparing the BRDF stage
       
        vec3 f0 = vec3(0.04); // @TODO (PBR): Change this to be more dynamic and/or user controlled.
        f0      = mix(f0, albedo_texel, metallic_texel); 

        BRDFDesc brdf; 
        brdf.f0           = f0; 
        brdf.normal       = calculate_normal(normal_texel);
        brdf.view_dir     = normalize(fs_in.camera_pos - fs_in.pixel_pos); 
        brdf.roughness    = roughness_texel;
        brdf.metallic     = metallic_texel;
        brdf.albedo_texel = albedo_texel;

        // Accumulate the many different lights 
      
        vec3 dir_light_factor = evaluate_directional_light(u_dir_light, brdf);

        vec3 point_lights_factor = vec3(0.0);
        for(int i = 0; i < u_points_count; i++) {
          point_lights_factor += evaluate_point_light(u_points[i], brdf);
        }

        vec3 spot_lights_factor = vec3(0.0);
        for(int i = 0; i < u_spots_count; i++) {
          spot_lights_factor += evaluate_spot_light(u_spots[i], brdf);
        }

        // Add it all together...
        
        vec3 final_color = (emissive_texel + (dir_light_factor + point_lights_factor + spot_lights_factor)) * u_ambient;
        frag_color       = vec4((1 - calculate_shadow()) * final_color, u_material.transparency);
      }
    )"
  };
};
