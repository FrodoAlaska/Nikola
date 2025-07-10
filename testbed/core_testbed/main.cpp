#include "default_shader.h"

#include <nikola/nikola.h>

#include <sokol/sokol_gfx.h>
#include <sokol/sokol_glue.h>

struct Renderer {
  sg_pipeline pipeline;
  sg_pass_action pass_action; 
  sg_bindings bindings;
};

static Renderer s_renderer;

static void init_sokol() {
  // Sokol state init
  sg_desc desc = {
    .environment = sglue_enviornment()
  };
  sg_setup();

  // Vertex buffer init
  f32 vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
  };
  sg_buffer_desc buff_desc = {
    .data = SG_RANGE(vertices),
  };
  s_renderer.bindings.vertex_buffer[0] = sg_make_buffer(buff_desc);

  // Pipeline init
  sg_pipeline_desc pipe_desc = {
    .shader = sg_make_shader(default_shader_desc(sg_query_backend()));

    .layout = {
      .attrs = {
        [ATTR_default_position].format  = SG_VERTEXFORMAT_FLOAT3,
        [ATTR_default_texcoord0].format = SG_VERTEXFORMAT_FLOAT2,
        [ATTR_default_color0].format    = SG_VERTEXFORMAT_FLOAT4,
      },
    },
  };
  s_renderer.pipeline = sg_make_pipeline(pipe_desc);

  // Pass action init
  s_renderer.pass_action.colors[0] = {
    .load_action = SG_LOADACTION_CLEAR, 
    .clear_value = {0.1f, 0.1f, 0.1f, 1.0f},
  };
}

static void begin_pass() {
  sg_pass pass = {
    .action    = s_renderer.pass_action,
    .swapchain = sglue_swapchain(),
  };
  sg_begin_pass(pass);
}

static void end_pass() {
  sg_end_pass();
  sg_commit();
}

int main() {
  // Initialze the library
  if(!nikola::init()) {
    return -1;
  }

  // Openinig the window
  nikola::i32 win_flags = nikola::WINDOW_FLAGS_FOCUS_ON_CREATE | nikola::WINDOW_FLAGS_RESIZABLE;
  nikola::Window* window = nikola::window_open("Hello, Nikola", 1366, 768, win_flags);
  if(!window) {
    return -1;
  }

  // Main loop
  while(nikola::window_is_open(window)) {
    // Will stop the application when F1 is pressed
    if(nikola::input_key_pressed(nikola::KEY_F1)) {
      break;
    }

    begin_pass();
    
    sg_apply_pipeline(s_renderer.pipeline);
    sg_apply_bindings(s_renderer.bindings);
    sg_draw(0, 6, 1);

    end_pass();
  }

  // De-initialze
  sg_shutdown(); 
  nikola::window_close(window);
  nikola::shutdown();
}
