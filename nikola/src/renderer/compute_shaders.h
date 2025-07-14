#include <nikola/nikola.h> 

static inline nikola::GfxShaderDesc generate_particle_compute_shader() {
  return nikola::GfxShaderDesc {
    .compute_source = R"(
      #version 460 core 

      layout (rgba32f, binding = 0) uniform image2D u_output;
      layout (std430, binding = 0) buffer Time {
        float u_time;
      };
      layout (location = 0) uniform float u_delta;

      layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
      void main() {
        ivec2 coord    = ivec2(gl_GlobalInvocationID.xy);
        ivec2 img_size = imageSize(u_output);

        vec2 color = vec2(coord) / vec2(img_size);
        imageStore(u_output, coord, vec4(color.x, u_delta, 0.0, 1.0));
      }
    )",
  };
}
