#include <nikola/nikola.h> 

static inline nikola::GfxShaderDesc generate_particle_compute_shader() {
  return nikola::GfxShaderDesc {
    .compute_source = R"(
      #version 460 core 

      layout (rgba32f, binding = 0) uniform restrict readonly image2D u_input;
      layout (rgba32f, binding = 1) uniform restrict writeonly image2D u_output;

      layout (location = 0) uniform ivec2 u_dir;

      const int M = 16; 
      const int N = 2 * M + 1;
      
      // Pre-computed coefficients
      const float coeffs[N] = float[N](
        0.012318109844189502,
        0.014381474814203989,
        0.016623532195728208,
        0.019024086115486723,
        0.02155484948872149,
        0.02417948052890078,
        0.02685404941667096,
        0.0295279624870386,
        0.03214534135442581,
        0.03464682117793548,
        0.0369716985390341,
        0.039060328279673276,
        0.040856643282313365,
        0.04231065439216247,
        0.043380781642569775,
        0.044035873841196206,
        0.04425662519949865,
        0.044035873841196206,
        0.043380781642569775,
        0.04231065439216247,
        0.040856643282313365,
        0.039060328279673276,
        0.0369716985390341,
        0.03464682117793548,
        0.03214534135442581,
        0.0295279624870386,
        0.02685404941667096,
        0.02417948052890078,
        0.02155484948872149,
        0.019024086115486723,
        0.016623532195728208,
        0.014381474814203989,
        0.012318109844189502
      );

      layout (local_size_x = 16, local_size_y = 16) in;
      void main() {
        ivec2 img_size = imageSize(u_input);
        ivec2 coord    = ivec2(gl_GlobalInvocationID.xy);

        // Some useful bounds checking 
        if(coord.x > img_size.x || coord.y > img_size.y) {
          return;
        }
          
        vec2 color = vec2(0.0);

        for(int i = 0; i < N; i++) {
          ivec2 pixel_coord = coord + u_dir * (i - M);

          // Clamp from 0 to image size
          pixel_coord.x = clamp(pixel_coord.x, 0, img_size.x - 1);
          pixel_coord.y = clamp(pixel_coord.y, 0, img_size.y - 1);

          color += vec2(coeffs[i] * imageLoad(u_input, pixel_coord));
        }

        imageStore(u_output, coord, vec4(color, 0.0, 1.0));
      }
    )",
  };
}
