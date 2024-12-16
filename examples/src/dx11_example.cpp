#include <nikol_core.hpp>
#include <stb/stb_image.h>

static nikol::GfxTextureDesc load_texture_from_file(const char* path) {
  nikol::GfxTextureDesc desc; 

  int width, height, channels;

  stbi_set_flip_vertically_on_load(true);
  nikol::u8* pixels = stbi_load(path, &width, &height, &channels, 4);

  desc.width     = width; 
  desc.height    = height;
  desc.depth     = 1; 
  desc.format    = nikol::GFX_TEXTURE_FORMAT_RGBA8; 
  desc.filter    = nikol::GFX_TEXTURE_FILTER_MIN_MAG_LINEAR;
  desc.wrap_mode = nikol::GFX_TEXTURE_WRAP_REPEAT;
  desc.data      = pixels;

  return desc;
}

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
  "   float3 vertex_position : POS;\n"
  "   float2 tex_coords : TEX;\n"
  "};\n"
  "\n"
  "struct vs_out {\n"
  "   float4 position : SV_POSITION;\n"
  "   float2 tex_coords : TEXCOORD0;\n"
  "};\n"
  "\n"
  "Texture2D text : register(t0);\n"
  "SamplerState samp : register(s0);\n"
  "\n"
  "vs_out vs_main(vs_in input) {\n"
  "   vs_out output;\n"
  "\n"
  "   output.position   = float4(input.vertex_position, 1.0);\n"
  "   output.tex_coords = input.tex_coords;\n"
  "\n"
  "   return output;\n"
  "}\n"
  "\n"
  "float4 ps_main(vs_out input) : SV_TARGET {\n"
  "   float4 color;\n"
  "   color = text.Sample(samp, input.tex_coords);\n"
  "   return color;\n"
  "}\n";
  nikol::GfxShader* shader = nikol::gfx_shader_create(gfx, src);
  // // Setting up the constant buffer to be uploaded to the shader later
  // float color[4] = {1.0f, 0.2f, 0.4f, 1.0f};
  // nikol::GfxUniformDesc uniform_desc = {
  //   .shader_type = nikol::GFX_SHADER_PIXEL, 
  //   .index       = nikol::gfx_shader_create_uniform(gfx, shader, nikol::GFX_SHADER_PIXEL, sizeof(color)), 
  //   .data        = color, 
  //   .size        = sizeof(color),
  // };
  // nikol::gfx_shader_queue_uniform(gfx, shader, uniform_desc);

  // Creating a vertex buffer and adding it to the draw call
  nikol::f32 vertices[] = {
    -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
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

  // Creating a texture 
  nikol::GfxTextureDesc texture_desc = load_texture_from_file("container.png");
  nikol::GfxTexture* texture = nikol::gfx_texture_create(gfx, texture_desc);

  // Finally creating the pipeline from the desc
  nikol::GfxPipelineDesc pipe_desc = {
    .vertex_buffer  = &vert_buff, 
    .vertices_count = 4, 

    .index_buffer  = &index_buff, 
    .indices_count = 6,

    .shader = shader, 

    .layout       = {{"POS", nikol::GFX_LAYOUT_FLOAT3, 0}, {"TEX", nikol::GFX_LAYOUT_FLOAT2, 0}},
    .layout_count = 2,

    .draw_mode = nikol::GFX_DRAW_MODE_TRIANGLE, 

    .textures = {texture}, 
    .texture_count = 1,
  };
  nikol::GfxPipeline* pipeline = nikol::gfx_pipeline_create(gfx, pipe_desc);

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
