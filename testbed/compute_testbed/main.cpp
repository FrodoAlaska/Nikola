#include <nikola/nikola.h>

struct ComputeState {
  nikola::GfxContext* gfx = nullptr;

  nikola::GfxPipelineDesc pipe_desc = {};
  nikola::GfxPipeline* pipe         = nullptr;

  nikola::GfxTexture* texture       = nullptr;
  nikola::GfxShader* compute_shader = nullptr;
  nikola::GfxShader* default_shader = nullptr;

  nikola::GfxBuffer* storage_buffer = nullptr;

  nikola::DynamicArray<nikola::Vec2> vertices;
};

static ComputeState s_state{};

static void init_shaders() {
  // Compute shader init

  nikola::GfxShaderDesc compute_desc = {
    .compute_source = R"(
      #version 460 core 
      
      layout (rgba32f, binding = 0) uniform image2D u_screen;

      layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
      void main() {
        vec4 color = vec4(0.062745, 0.054902, 0.109804, 1.0);
        vec2 coord = vec2(gl_GlobalInvocationID.xy);

        vec2 img_size = vec2(imageSize(u_screen));

        float x = -((coord.x * 2.0 - img_size.x) / img_size.x);
        float y = -((coord.y * 2.0 - img_size.y) / img_size.y);

        float fov = 90.0f;
        vec3 cam_o = vec3(0.0, 0.0, -tan(fov / 2.0));
        vec3 ray_o = vec3(x, y, 0.0);
        vec3 ray_d = normalize(ray_o - cam_o);

        vec3 sphere_c  = vec3(0.0, 0.0, -5.0);
        float sphere_r = 0.5;

        vec3 o_c = ray_o - sphere_c;

        float b = dot(ray_d, o_c);
        float c = dot(o_c, o_c) - (sphere_r * sphere_r);

        float intersect_state = (b * b) - c;
        vec3 intersection     = ray_o + ray_d * (-b + sqrt(intersect_state));

        if(intersect_state >= 0.0) {
          color = vec4(normalize(intersection - sphere_c) + 1.0 / 2.0, 1.0);
        }

        imageStore(u_screen, ivec2(gl_GlobalInvocationID), color);
      }
    )",
  };
  s_state.compute_shader = nikola::gfx_shader_create(s_state.gfx, compute_desc);
  NIKOLA_LOG_TRACE("COMPUTE loaded");

  // Default shader init

  nikola::GfxShaderDesc default_desc = {
    .vertex_source = R"(
      #version 460 core
      
      layout (location = 0) in vec2 aPos;
      layout (location = 1) in vec2 aTexCoord;

      out vec2 tex_coord;

      void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);

        tex_coord = aTexCoord;
      }
    )",

    .pixel_source = R"(
      #version 460 core
      
      layout (location = 0) out vec4 frag_color;
      layout (binding = 0) uniform sampler2D u_texture; 

      in vec2 tex_coord;

      void main() {
        frag_color = texture(u_texture, tex_coord);
      }
    )",
  };
  s_state.default_shader = nikola::gfx_shader_create(s_state.gfx, default_desc);
  NIKOLA_LOG_TRACE("DEFUALT loaded");
}

static void init_texture() {
  nikola::GfxTextureDesc tex_desc = {
    .width  = 1024, 
    .height = 1024,

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

  nikola::Vec2 vertices[128];
  for(int i = 0; i < 16; i++) {
    for(int j = 0; j < 8; j++) {
      int index = (i * 8) + j;

      vertices[index] = nikola::Vec2(0.005f * i, 0.001f * j);
    }
  }

  nikola::GfxBufferDesc buff_desc = {
    .data  = vertices, 
    .size  = sizeof(nikola::Vec2) * 128, 
    .type  = nikola::GFX_BUFFER_SHADER_STORAGE, 
    .usage = nikola::GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  s_state.storage_buffer = nikola::gfx_buffer_create(s_state.gfx, buff_desc);
}

static void init_pipeline() {
  // Vertex buffer init

  float vertices[] = {
    // Position    // Texture coords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,
  };

  nikola::GfxBufferDesc vert_desc = {
    .data  = vertices,
    .size  = sizeof(vertices), 
    .type  = nikola::GFX_BUFFER_VERTEX, 
    .usage = nikola::GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  s_state.pipe_desc.vertex_buffer  = nikola::gfx_buffer_create(s_state.gfx, vert_desc);
  s_state.pipe_desc.vertices_count = 6;
  
  // Vertex attributes init

  s_state.pipe_desc.layouts[0].attributes[0]    = nikola::GFX_LAYOUT_FLOAT2,
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
    .states = nikola::GFX_STATE_BLEND | nikola::GFX_STATE_MSAA,
  };
  s_state.gfx = nikola::gfx_context_init(gfx_desc);
  if(!s_state.gfx) {
    return -1;
  }

  // Resources init
  
  init_texture();
  init_shaders();
  init_pipeline();
  init_storage_buffer();

  // Main loop
  
  while(nikola::window_is_open(window)) {
    if(nikola::input_key_pressed(nikola::KEY_ESCAPE)) {
      break;
    }

    // Clear the screen

    nikola::gfx_context_clear(s_state.gfx, 0.062745f, 0.054902f, 0.109804f, 1.0f);

    // Dispatch call

    nikola::GfxBindingDesc bind_desc = {
      .shader = s_state.compute_shader,

      .images       = &s_state.texture, 
      .images_count = 1,
    };
    nikola::gfx_context_use_bindings(s_state.gfx, bind_desc);

    nikola::gfx_context_dispatch(s_state.gfx, 1024, 1024, 1);
    nikola::gfx_context_memory_barrier(s_state.gfx, nikola::GFX_MEMORY_BARRIER_SHADER_IMAGE_ACCESS);

    // Draw call

    bind_desc = {
      .shader = s_state.default_shader,

      .textures       = &s_state.texture, 
      .textures_count = 1,
    };
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
