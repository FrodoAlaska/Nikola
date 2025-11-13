#include "nikola/nikola_ui.h"
#include "nikola/nikola_resources.h"
#include "nikola/nikola_render.h"
#include "nikola/nikola_math.h"
#include "nikola/nikola_event.h"
#include "nikola/nikola_input.h"
#include "nikola/nikola_timer.h"

#include "shaders/ui_shaders.h"

#include <RmlUi/Core.h>
#include <RmlUi/Core/SystemInterface.h>
#include <RmlUi/Core/RenderInterface.h>
#include <RmlUi/Core/Input.h>
#include <RmlUi/Core/Types.h>
#include <RmlUi/Core/Log.h>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Consts

const sizei VERTEX_BUFFER_SIZE = KiB(256);
const sizei INDEX_BUFFER_SIZE  = KiB(256);

/// Consts
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ShaderID
enum ShaderID {
  SHADER_TEXTURE = 0, 
  SHADER_COLOR,
  SHADER_GRADIENT, 
  SHADER_CREATION, 
  SHADER_COLOR_MATRIX,
  SHADER_BLEND_MASK,

  SHADERS_MAX,
};
/// ShaderID
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Forward declarations

class NKSystemInterface;
class NKRenderInterface;

/// Forward declarations
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIBatch
struct UIBatch {
  DynamicArray<Rml::Vertex> vertices; 
  DynamicArray<i32> indices;
};
/// UIBatch
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIDrawCall
struct UIDrawCall {
  GfxTexture* texture; 
  ShaderContext* shader;

  Rml::Vector2f translation;
  UIBatch* batch;
};
/// UIDrawCall
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIRenderer
struct UIRenderer {
  NKSystemInterface* system_interface; 
  NKRenderInterface* render_interface;

  GfxContext* gfx;
  Window* window;
  GfxPipeline* pipeline; 

  ShaderContext* shaders[SHADERS_MAX]; // Pre-compiled shader contexts

  DynamicArray<UIBatch> batches;       // Compiled batches
  DynamicArray<GfxTexture*> textures;  // Textures in use
  DynamicArray<UIDrawCall> draw_calls; // Compiled draw calls

  Mat4 transform = Mat4(1.0f);
  Mat4 ortho     = Mat4(1.0f);
};

static UIRenderer s_renderer;
/// UIRenderer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NKSystemInterface
class NKSystemInterface : public Rml::SystemInterface {
public:
  NKSystemInterface() = default;

public:
  double GetElapsedTime() override {
    return niclock_get_time();
  }

  bool LogMessage(Rml::Log::Type type, const Rml::String& message) override {
    switch(type) {
      case Rml::Log::Type::LT_ERROR:
        NIKOLA_LOG_ERROR("RML-ERROR: %s", message.c_str());
        return false;
      case Rml::Log::Type::LT_ASSERT:
        NIKOLA_ASSERT(false, message.c_str());
        return false;
      case Rml::Log::Type::LT_WARNING:
        NIKOLA_LOG_WARN("RML-WARNING: %s", message.c_str());
        break;
      case Rml::Log::Type::LT_INFO:
        NIKOLA_LOG_INFO("RML-INFO:  %s", message.c_str());
        break;
    }

    return true;
  }
};
/// NKSystemInterface
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NKRenderInterface
class NKRenderInterface : public Rml::RenderInterface {
public:
  NKRenderInterface(UIRenderer& ui_renderer)
    :renderer(ui_renderer)
  {}

public:
  Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) override {
    UIBatch batch = {}; 
    batch.vertices.assign(vertices.data(), vertices.data() + vertices.size());
    batch.indices.assign(indices.data(), indices.data() + indices.size());

    renderer.batches.push_back(batch);
    return (Rml::CompiledGeometryHandle)renderer.batches.size();
  }
  
  void RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture) override {
    UIDrawCall call{};
    call.texture     = renderer.textures[(sizei)texture];
    call.shader      = (texture == 0) ? renderer.shaders[SHADER_COLOR] : renderer.shaders[SHADER_TEXTURE];
    call.translation = translation;
    call.batch       = &renderer.batches[(sizei)(geometry - 1)];

    renderer.draw_calls.push_back(call);
  }
  
  void ReleaseGeometry(Rml::CompiledGeometryHandle geometry) override {
    // @TODO (UI)
  }

  Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) override {
    // @TODO (UI): You gotta find a way to put the textures in the resource manager instead of loading 
    // them from here, and effectively letting the UI renderer handle the resources. 
    // I just don't want to put them in the resource cache since they are valid textures. 
    // Perhaps have a UI_CACHE? Just for fonts and UI textures? I don't know.
    //
    // It's good for now, but not for the future. 

    //
    // Load and check the NBR file
    //

    File file;
    if(!nbr_file_is_valid(file, FilePath(source), RESOURCE_TYPE_TEXTURE)) {
      return 0;
    }

    NBRTexture nbr_texture;
    file_read_bytes(file, &nbr_texture);

    //
    // Convert the NBR format to a valid texture
    //
    
    GfxTexture* texture = gfx_texture_create(renderer.gfx, GFX_TEXTURE_2D);

    GfxTextureDesc tex_desc; 
    tex_desc.width  = texture_dimensions.x; 
    tex_desc.height = texture_dimensions.y; 
    tex_desc.depth  = 0; 
    tex_desc.mips   = 1; 
    tex_desc.type   = GFX_TEXTURE_2D; 
    tex_desc.format = (GfxTextureFormat)nbr_texture.format; 
    tex_desc.data   = nbr_texture.pixels;

    if(!gfx_texture_load(texture, tex_desc)) {
      NIKOLA_LOG_ERROR("Failed to load texture at '\%s\'", FilePath(source).c_str());
      return 0;
    } 

    //
    // Freeing NBR data
    // 

    memory_free(nbr_texture.pixels);
    file_close(file); 

    // Done!
    
    renderer.textures.push_back(texture); 
    return (Rml::TextureHandle)(renderer.textures.size() - 1);
  }

  Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions) override {
    // @TODO (UI): Same story...

    //
    // Convert the NBR format to a valid texture
    //
    
    GfxTexture* texture = gfx_texture_create(renderer.gfx, GFX_TEXTURE_2D);

    GfxTextureDesc tex_desc; 
    tex_desc.width  = source_dimensions.x; 
    tex_desc.height = source_dimensions.y; 
    tex_desc.depth  = 0; 
    tex_desc.mips   = 1; 
    tex_desc.type   = GFX_TEXTURE_2D; 
    tex_desc.format = GFX_TEXTURE_FORMAT_RGBA8; 
    tex_desc.data   = (void*)source.data();

    if(!gfx_texture_load(texture, tex_desc)) {
      NIKOLA_LOG_ERROR("Failed to load source texture");
      return 0;
    } 

    // Done!
    
    renderer.textures.push_back(texture); 
    return (Rml::TextureHandle)(renderer.textures.size() - 1);
  }

  void ReleaseTexture(Rml::TextureHandle texture) override {
    // @NOTE:
    //
    // The first texture in the array is handled by the engine. If we free it here, the resource manager 
    // will try to free it again later. Double free. That's never good.
    //

    sizei index = (sizei)(texture - 1);
    if(index == 0) { 
      return; 
    }

    gfx_texture_destroy(renderer.textures[index]);
    renderer.textures[index] = nullptr;
  }

  void EnableScissorRegion(bool enable) override {
    // @TODO (UI)
  }

  void SetScissorRegion(Rml::Rectanglei region) {
    // @TODO (UI)
  }

  void SetTransform(const Rml::Matrix4f* transform) {
    if(!transform) {
      renderer.transform = Mat4(1.0f);
      return;
    }

    // @TODO (UI)
  }

  Rml::CompiledShaderHandle CompileShader(const Rml::String& name, const Rml::Dictionary& parameters) {
    // @TODO (UI)
    return 0;
  }
  
  void RenderShader(Rml::CompiledShaderHandle shader,
                    Rml::CompiledGeometryHandle geometry,
                    Rml::Vector2f translation,
                    Rml::TextureHandle texture) {
    // @TODO (UI)
  }

  void ReleaseShader(Rml::CompiledShaderHandle shader) {
    // @TODO (UI)
  }

public:
  UIRenderer& renderer;
};
/// NKRenderInterface
///---------------------------------------------------------------------------------------------------------------------


///---------------------------------------------------------------------------------------------------------------------
/// UI renderer functions

bool ui_renderer_init(GfxContext* gfx) {
  NIKOLA_ASSERT(gfx, "Invalid GfxContext given to ui_renderer_init");
  NIKOLA_PROFILE_FUNCTION();

  s_renderer.gfx    = gfx;
  s_renderer.window = gfx_context_get_desc(gfx).window;

  //
  // Pipeline init
  //

  GfxPipelineDesc pipe_desc{};

  // Vertex buffer init

  GfxBufferDesc buff_desc = {
    .data  = nullptr, 
    .size  = VERTEX_BUFFER_SIZE,
    .type  = GFX_BUFFER_VERTEX,
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  pipe_desc.vertex_buffer = resources_get_buffer(resources_push_buffer(RESOURCE_CACHE_ID, buff_desc));

  // Index buffer init

  buff_desc = {
    .data  = nullptr, 
    .size  = INDEX_BUFFER_SIZE,
    .type  = GFX_BUFFER_INDEX,
    .usage = GFX_BUFFER_USAGE_DYNAMIC_DRAW,
  };
  pipe_desc.index_buffer = resources_get_buffer(resources_push_buffer(RESOURCE_CACHE_ID, buff_desc));

  // Layouts init

  pipe_desc.layouts[0].attributes[0]    = GFX_LAYOUT_FLOAT2; // Position
  pipe_desc.layouts[0].attributes[1]    = GFX_LAYOUT_UBYTE4; // Color
  pipe_desc.layouts[0].attributes[2]    = GFX_LAYOUT_FLOAT2; // Texture coords
  pipe_desc.layouts[0].attributes_count = 3;
  
  // Create the pipeline 

  pipe_desc.draw_mode = GFX_DRAW_MODE_TRIANGLE; 
  s_renderer.pipeline = gfx_pipeline_create(gfx, pipe_desc);

  //
  // Shaders init
  //

  ResourceID shader_ids[SHADERS_MAX] = {
    resources_push_shader(RESOURCE_CACHE_ID, generate_ui_texture_shader()), 
    resources_push_shader(RESOURCE_CACHE_ID, generate_ui_color_shader()), 
    resources_push_shader(RESOURCE_CACHE_ID, generate_ui_gradient_shader()), 
    resources_push_shader(RESOURCE_CACHE_ID, generate_ui_creation_shader()),  
    resources_push_shader(RESOURCE_CACHE_ID, generate_ui_color_matrix()),  
    resources_push_shader(RESOURCE_CACHE_ID, generate_ui_blend_mask()),  
  };

  for(sizei i = 0; i < SHADERS_MAX; i++) {
    s_renderer.shaders[i] = resources_get_shader_context(resources_push_shader_context(RESOURCE_CACHE_ID, shader_ids[i]));
  }

  //
  // Default texture init
  //

  s_renderer.textures.push_back(renderer_get_defaults().albedo_texture);

  //
  // Interfaces init
  //

  s_renderer.system_interface = new NKSystemInterface();
  s_renderer.render_interface = new NKRenderInterface(s_renderer);

  Rml::SetSystemInterface(s_renderer.system_interface);
  Rml::SetRenderInterface(s_renderer.render_interface);
  
  Rml::Initialise();

  // Done!
  
  NIKOLA_LOG_INFO("Successfully initialized the ui renderer");
  return true;
}

void ui_renderer_shutdown() {
  NIKOLA_PROFILE_FUNCTION();

  // Interfaces deinit
  
  Rml::Shutdown();

  delete s_renderer.render_interface;
  delete s_renderer.system_interface;

  // Clearing all the resources

  for(auto& batch : s_renderer.batches) {
    batch.vertices.clear();
    batch.indices.clear();
  }

  s_renderer.textures.clear();
  s_renderer.batches.clear();
  s_renderer.draw_calls.clear();

  // Done!
  NIKOLA_LOG_INFO("Successfully shutdown the ui renderer");
}

void ui_renderer_begin() {
  NIKOLA_PROFILE_FUNCTION();

  // Calculating the orthographic matrix

  IVec2 framebuffer_size;
  window_get_size(s_renderer.window, &framebuffer_size.x, &framebuffer_size.y);

  s_renderer.ortho = mat4_ortho(0.0f, (f32)framebuffer_size.x, (f32)framebuffer_size.y, 0.0f) * s_renderer.transform;

  // Clear calls from previous frames
  s_renderer.draw_calls.clear();
}

void ui_renderer_end() {
  NIKOLA_PROFILE_FUNCTION();

  // Initiating the draw calls

  for(auto& call : s_renderer.draw_calls) {
    // Setting uniforms 

    shader_context_set_uniform(call.shader, "u_translate", Vec2(call.translation.x, call.translation.y));
    shader_context_set_uniform(call.shader, "u_transform", s_renderer.ortho);

    // Use the resources

    GfxBindingDesc bind_desc = {
      .shader = call.shader->shader,

      .textures       = &call.texture, 
      .textures_count = 1,
    };
    gfx_context_use_bindings(s_renderer.gfx, bind_desc);

    // Update the required resources
    
    GfxPipelineDesc& pipe_desc = gfx_pipeline_get_desc(s_renderer.pipeline);

    gfx_buffer_upload_data(pipe_desc.vertex_buffer, 
                           0, 
                           sizeof(Rml::Vertex) * call.batch->vertices.size(), 
                           call.batch->vertices.data());

    gfx_buffer_upload_data(pipe_desc.index_buffer, 
                           0, 
                           sizeof(i32) * call.batch->indices.size(), 
                           call.batch->indices.data());

    pipe_desc.vertices_count = call.batch->vertices.size();
    pipe_desc.indices_count  = call.batch->indices.size();
    gfx_pipeline_update(s_renderer.pipeline, pipe_desc);
    
    // Render the batch

    gfx_context_use_pipeline(s_renderer.gfx, s_renderer.pipeline); 
    gfx_context_draw(s_renderer.gfx, 0);
  }
}

/// UI renderer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
