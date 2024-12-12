#include <nikol_core.hpp>

#include <windows.h>

int main() {
  // Initialze the library
  if(!nikol::init()) {
    return -1;
  } 

  // Openinig the window
  nikol::i32 win_flags = nikol::WINDOW_FLAGS_FOCUS_ON_CREATE | nikol::WINDOW_FLAGS_GFX_HARDWARE;
  nikol::Window* window = nikol::window_open("Hello, Nikol", 1366, 768, win_flags);
  if(!window) {
    return -1;
  }

  // Creating a graphics context
  nikol::i32 flags = nikol::GFX_FLAGS_BLEND | 
                     nikol::GFX_FLAGS_DEPTH | 
                     nikol::GFX_FLAGS_STENCIL | 
                     nikol::GFX_FLAGS_ENABLE_VSYNC | 
                     nikol::GFX_FLAGS_CULL_CCW;
  nikol::GfxContext* gfx = gfx_context_init(window, flags);
  if(!gfx) {
    return -1;
  }

  // Creating a shader and adding it to the draw call
  const char* src =
  "struct vs_in {\n"
  "   float3 vertex_position : POSITION;\n"
  "   float4 color : COLOR;\n"
  "};\n"
  "\n"
  "struct vs_out {\n"
  "   float4 position : SV_POSITION;\n"
  "   float4 color : COLOR;\n"
  "};\n"
  "\n"
  "vs_out vs_main(vs_in input) {\n"
  "   vs_out output = (vs_out)0;\n"
  "\n"
  "   output.position = float4(input.vertex_position, 1.0);\n"
  "   output.color    = input.color;\n"
  "\n"
  "   return output;\n"
  "}\n"
  "\n"
  "cbuffer OutColor {\n"
  "   float4 color;\n"
  "};\n"
  "\n"
  "float4 ps_main(vs_out input) : SV_TARGET {\n"
  "   return color;\n"
  "}\n";
  nikol::GfxShader* shader = nikol::gfx_shader_create(gfx, src);
  nikol::sizei uniform_index = nikol::gfx_shader_create_uniform(gfx, shader, nikol::GFX_SHADER_PIXEL, sizeof(float) * 4);

  // Creating a vertex buffer and adding it to the draw call
  nikol::f32 vertices[] = {
    -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
  };
  nikol::GfxBufferDesc vert_buff = {
    .data  = vertices, 
    .size  = sizeof(vertices), 
    .type  = nikol::GFX_BUFFER_VERTEX, 
    .usage = nikol::GFX_BUFFER_USAGE_STATIC_DRAW,
  };

  // Creating an index buffer and adding it to the draw call
  nikol::u32 indices[] = {
    0, 1, 2, 
    2, 3, 0,
  };
  nikol::GfxBufferDesc index_buff = {
    .data  = indices, 
    .size  = sizeof(indices), 
    .type  = nikol::GFX_BUFFER_INDEX, 
    .usage = nikol::GFX_BUFFER_USAGE_STATIC_DRAW,
  };

  // Finally creating the pipeline from the desc
  nikol::GfxPipelineDesc pipe_desc = {
    .vertex_buffer  = &vert_buff, 
    .vertices_count = 4, 

    .index_buffer  = &index_buff, 
    .indices_count = 6,

    .shader = shader, 

    .layout       = {{"POSITION", nikol::GFX_LAYOUT_FLOAT3, 0}, {"COLOR", nikol::GFX_LAYOUT_FLOAT4, 0}},
    .layout_count = 2,

    .draw_mode = nikol::GFX_DRAW_MODE_TRIANGLE, 
  };
  nikol::GfxPipeline* pipeline = nikol::gfx_pipeline_create(gfx, pipe_desc);

  // Setting up the constant buffer to be uploaded to the shader later
  float color[4] = {1.0f, 0.2f, 0.4f, 1.0f};
  nikol::GfxUniformDesc uniform_desc = {
    .shader_type = nikol::GFX_SHADER_PIXEL, 
    .index       = uniform_index, 
    .data        = color, 
    .size        = sizeof(color),
  };
  nikol::gfx_shader_queue_uniform(gfx, shader, uniform_desc);

  // Main loop
  while(nikol::window_is_open(window)) {
    // Will stop the application when ESCAPE is pressed
    if(nikol::input_key_pressed(nikol::KEY_ESCAPE)) {
      break;
    }
    
    // Clear the screen to gray
    nikol::gfx_context_clear(gfx, 0.3f, 0.3f, 0.3f, 1.0f);
    
    // Sumbit the draw call and render it to the screen
    nikol::gfx_pipeline_draw_index(gfx, pipeline, pipe_desc);

    // Swap the internal window buffer
    nikol::gfx_context_present(gfx); 
    
    // Poll the window events
    nikol::window_poll_events(window);
  }

  // De-initialze
  nikol::gfx_pipeline_destroy(pipeline);
  nikol::gfx_context_shutdown(gfx);
  nikol::window_close(window);
  nikol::shutdown();
}
