#include <nikola/nikola.h>

struct ComputeState {
  nikola::GfxContext* gfx = nullptr;

  nikola::GfxPipelineDesc pipe_desc = {};
  nikola::GfxPipeline* pipe         = nullptr;

  nikola::GfxTexture* texture       = nullptr;
  nikola::GfxShader* compute_shader = nullptr;
  nikola::GfxShader* default_shader = nullptr;

  nikola::GfxBuffer* storage_buffer = nullptr;
};

static ComputeState s_state{};

static void init_shaders() {
  // Compute shader init

  nikola::GfxShaderDesc compute_desc = {
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
  s_state.compute_shader = nikola::gfx_shader_create(s_state.gfx, compute_desc);

  // Default shader init

  nikola::GfxShaderDesc default_desc = {
    .vertex_source = R"(
      #version 460 core
      
      layout (location = 0) in vec2 aPos;
      layout (location = 1) in vec2 aTextureCoords;
      
      out VS_OUT {
        vec2 tex_coords;
      } vs_out;
      
      void main() {
        vs_out.tex_coords = aTextureCoords;
        gl_Position       = vec4(aPos, 0.0, 1.0);
      }
    )",

    .pixel_source = R"(
      #version 460 core
      
      layout (location = 0) out vec4 frag_color;
      
      in VS_OUT {
        vec2 tex_coords;
      } fs_in;
      
      layout(binding = 0) uniform sampler2D u_input;
      
      void main() {
        frag_color = texture(u_input, fs_in.tex_coords);
      }
    )",
  };
  s_state.default_shader = nikola::gfx_shader_create(s_state.gfx, default_desc);
}

static void init_texture() {
  nikola::GfxTextureDesc tex_desc = {
    .width  = 1280, 
    .height = 720,

    .type      = nikola::GFX_TEXTURE_2D,
    .format    = nikola::GFX_TEXTURE_FORMAT_RGBA32F, 
    .filter    = nikola::GFX_TEXTURE_FILTER_MIN_MAG_LINEAR, 
    .wrap_mode = nikola::GFX_TEXTURE_WRAP_MIRROR,
    .access    = nikola::GFX_TEXTURE_ACCESS_WRITE,

    .data = nullptr,
  };

  s_state.texture = nikola::gfx_texture_create(s_state.gfx, tex_desc);
}

static void init_storage_buffer() {
  // Shader storage init

  float f_time = (float)nikola::niclock_get_time();

  nikola::GfxBufferDesc buff_desc = {
    .data  = &f_time, 
    .size  = sizeof(float), 
    .type  = nikola::GFX_BUFFER_SHADER_STORAGE, 
    .usage = nikola::GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  s_state.storage_buffer = nikola::gfx_buffer_create(s_state.gfx, buff_desc);

  // Bind the shader storage to the compute shader
  nikola::gfx_shader_attach_uniform(s_state.compute_shader, nikola::GFX_SHADER_COMPUTE, s_state.storage_buffer, 0);
}

static void init_pipeline() {
  // Vertex buffer init

  float vertices[] = {
     // Position        Texture coords
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f
  };

  nikola::GfxBufferDesc vert_desc = {
    .data  = vertices, 
    .size  = sizeof(vertices), 
    .type  = nikola::GFX_BUFFER_VERTEX, 
    .usage = nikola::GFX_BUFFER_USAGE_STATIC_DRAW,
  };

  s_state.pipe_desc.vertex_buffer  = nikola::gfx_buffer_create(s_state.gfx, vert_desc);
  s_state.pipe_desc.vertices_count = 4;
  
  // Index buffer init

  nikola::u32 indices[] = {
    0, 1, 2, 
    2, 3, 0,
  };

  nikola::GfxBufferDesc index_desc = {
    .data  = indices, 
    .size  = sizeof(indices), 
    .type  = nikola::GFX_BUFFER_INDEX, 
    .usage = nikola::GFX_BUFFER_USAGE_STATIC_DRAW,
  };

  s_state.pipe_desc.index_buffer  = nikola::gfx_buffer_create(s_state.gfx, index_desc);
  s_state.pipe_desc.indices_count = 6;

  // Vertex attributes init

  s_state.pipe_desc.layouts[0].attributes[0]    = nikola::GFX_LAYOUT_FLOAT3,
  s_state.pipe_desc.layouts[0].attributes[1]    = nikola::GFX_LAYOUT_FLOAT2,
  s_state.pipe_desc.layouts[0].attributes_count = 2;

  // Draw mode init
  s_state.pipe_desc.draw_mode = nikola::GFX_DRAW_MODE_TRIANGLE; 

  // Pipeline init
  s_state.pipe = nikola::gfx_pipeline_create(s_state.gfx, s_state.pipe_desc);
}

int main() {
  // Initialze the library
  if(!nikola::init()) {
    return -1;
  }

  // Openinig the window
  
  nikola::i32 win_flags  = nikola::WINDOW_FLAGS_FOCUS_ON_CREATE | 
                           nikola::WINDOW_FLAGS_CENTER_MOUSE    |
                           nikola::WINDOW_FLAGS_FULLSCREEN      | 
                           nikola::WINDOW_FLAGS_HIDE_CURSOR;
  nikola::Window* window = nikola::window_open("Compute Testbed", 1280, 720, win_flags);
  if(!window) {
    return -1;
  }

  // Creating a graphics context
  
  nikola::GfxContextDesc gfx_desc = {
    .window = window,
    .states = nikola::GFX_STATE_DEPTH | nikola::GFX_STATE_STENCIL | nikola::GFX_STATE_BLEND,
  };
  s_state.gfx = nikola::gfx_context_init(gfx_desc);
  if(!s_state.gfx) {
    return -1;
  }

  // Resources init
  
  init_shaders();
  init_texture();
  init_storage_buffer();
  init_pipeline();

  // Main loop
  
  while(nikola::window_is_open(window)) {
    if(nikola::input_key_pressed(nikola::KEY_ESCAPE)) {
      break;
    }
    
    // Clear the screen

    nikola::f32 r = 16.0f / 255.0f; 
    nikola::f32 g = 14.0f / 255.0f; 
    nikola::f32 b = 28.0f / 255.0f; 
    nikola::gfx_context_clear(s_state.gfx, r, g, b, 1.0f);
 
    // Dispatch call

    float f_time = nikola::sin(nikola::niclock_get_time()) * 2.0f - 1.0f; 
    nikola::gfx_shader_upload_uniform(s_state.compute_shader, 0, nikola::GFX_LAYOUT_FLOAT1, &f_time);

    nikola::GfxBindingDesc bind_desc = {
      .shader = s_state.compute_shader,

      .images       = &s_state.texture, 
      .images_count = 1,
    };
    nikola::gfx_context_use_bindings(s_state.gfx, bind_desc);
    
    nikola::gfx_context_dispatch(s_state.gfx, 1280, 720, 1);
    nikola::gfx_context_memory_barrier(s_state.gfx, (nikola::GFX_MEMORY_BARRIER_SHADER_IMAGE_ACCESS | nikola::GFX_MEMORY_BARRIER_SHADER_STORAGE_BUFFER));

    // Draw call
  
    nikola::gfx_context_set_state(s_state.gfx, nikola::GFX_STATE_DEPTH, false); 
   
    bind_desc.shader         = s_state.default_shader;
    bind_desc.textures       = &s_state.texture;
    bind_desc.textures_count = 1;
    nikola::gfx_context_use_bindings(s_state.gfx, bind_desc);
    
    nikola::gfx_context_use_pipeline(s_state.gfx, s_state.pipe);
    nikola::gfx_context_draw(s_state.gfx, 0);

    // Backbuffer switching

    nikola::gfx_context_present(s_state.gfx);
    nikola::window_poll_events(window);
  }

  // De-initialze
  nikola::gfx_context_shutdown(s_state.gfx);
  nikola::window_close(window);
  nikola::shutdown();
}
