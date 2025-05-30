#include "nikola/nikola_resources.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_gfx.h"
#include "nikola/nikola_file.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static bool check_nbr_validity(NBRFile& file, const FilePath& path) {
  // Check for the validity of the identifier
  if(file.identifier != NBR_VALID_IDENTIFIER) {
    NIKOLA_LOG_ERROR("Invalid identifier found in NBR file at \'%s\'. Expected \'%i\' got \'%i\'", path.c_str(), NBR_VALID_IDENTIFIER, file.identifier);
    return false;
  }  

  // Check for the validity of the versions
  bool is_valid_version = ((file.major_version == NBR_VALID_MAJOR_VERSION) || (file.minor_version == NBR_VALID_MINOR_VERSION));
  if(!is_valid_version) {
    NIKOLA_LOG_ERROR("Invalid version found in NBR file at \'%s\'", path.c_str());
    return false;
  }

  return true;
}

static bool open_for_load(NBRFile& nbr, const FilePath& path) {
  if(!file_open(&nbr.file_handle, path, (i32)(FILE_OPEN_READ | FILE_OPEN_BINARY))) {
    NIKOLA_LOG_ERROR("Cannot load NBR file at \'%s\'", path.c_str());
    return false;
  }

  nbr.path = (FilePath)path;
  return true;
}

static bool open_for_save(NBRFile& nbr, const FilePath& path) {
  if(!file_open(&nbr.file_handle, path, (i32)(FILE_OPEN_WRITE | FILE_OPEN_BINARY))) {
    NIKOLA_LOG_ERROR("Cannot save NBR file at \'%s\'", path.c_str());
    return false;
  }
  
  nbr.path = (FilePath)path;
  return true;
}

static void write_texture(NBRFile& nbr, const NBRTexture& texture) {
  // Save width and height
  file_write_bytes(nbr.file_handle, &texture.width, sizeof(texture.width));
  file_write_bytes(nbr.file_handle, &texture.height, sizeof(texture.height));
  
  // Save the channels
  file_write_bytes(nbr.file_handle, &texture.channels, sizeof(texture.channels));
 
  // Save the pixels
  sizei data_size = (texture.width * texture.height) * texture.channels;
  file_write_bytes(nbr.file_handle, texture.pixels, data_size);
}

static void write_cubemap(NBRFile& nbr, const NBRCubemap& cubemap) {
  // Save width and height
  file_write_bytes(nbr.file_handle, &cubemap.width, sizeof(cubemap.width));
  file_write_bytes(nbr.file_handle, &cubemap.height, sizeof(cubemap.height));

  // Save the channels
  file_write_bytes(nbr.file_handle, &cubemap.channels, sizeof(cubemap.channels));

  // Save the faces count
  file_write_bytes(nbr.file_handle, &cubemap.faces_count, sizeof(cubemap.faces_count));

  // Save the pixels for each face
  sizei data_size = (cubemap.width * cubemap.height) * cubemap.channels;
  for(sizei i = 0; i < cubemap.faces_count; i++) {
    file_write_bytes(nbr.file_handle, cubemap.pixels[i], data_size);
  }
}

static void write_shader(NBRFile& nbr, const NBRShader& shader) {
  // Save the length of the vertex shader's code string 
  file_write_bytes(nbr.file_handle, &shader.vertex_length, sizeof(u16));

  // Save the vertex shader's code string
  file_write_bytes(nbr.file_handle, shader.vertex_source, sizeof(i8) * shader.vertex_length);
  
  // Save the length of the pixel shader's code string 
  file_write_bytes(nbr.file_handle, &shader.pixel_length, sizeof(u16));

  // Save the pixel shader's code string
  file_write_bytes(nbr.file_handle, shader.pixel_source, sizeof(i8) * shader.pixel_length);
}

static void write_material(NBRFile& nbr, const NBRMaterial& material) {
  // Save the ambient color 
  file_write_bytes(nbr.file_handle, material.ambient, sizeof(f32) * 3); 

  // Save the diffuse color 
  file_write_bytes(nbr.file_handle, material.diffuse, sizeof(f32) * 3); 
  
  // Save the specular color 
  file_write_bytes(nbr.file_handle, material.specular, sizeof(f32) * 3); 
 
  // Save the texture indices
  file_write_bytes(nbr.file_handle, &material.diffuse_index, sizeof(i8)); 
  file_write_bytes(nbr.file_handle, &material.specular_index, sizeof(i8)); 
}

static void write_mesh(NBRFile& nbr, const NBRMesh& mesh) {
  // Save the vertex type
  file_write_bytes(nbr.file_handle, &mesh.vertex_type, sizeof(u8));

  // Save the vertices
  file_write_bytes(nbr.file_handle, &mesh.vertices_count, sizeof(u32));
  file_write_bytes(nbr.file_handle, mesh.vertices, sizeof(f32) * mesh.vertices_count);

  // Save the indices
  file_write_bytes(nbr.file_handle, &mesh.indices_count, sizeof(u32));
  file_write_bytes(nbr.file_handle, mesh.indices, sizeof(u32) * mesh.indices_count);

  // Save the material index
  file_write_bytes(nbr.file_handle, &mesh.material_index, sizeof(u8));
}

static void write_model(NBRFile& nbr, const NBRModel& model) {
  // Save the meshes
  file_write_bytes(nbr.file_handle, &model.meshes_count, sizeof(u16));
  for(sizei i = 0; i < model.meshes_count; i++) {
    write_mesh(nbr, model.meshes[i]);
  }

  // Save the materials
  file_write_bytes(nbr.file_handle, &model.materials_count, sizeof(u8));
  for(sizei i = 0; i < model.materials_count; i++) {
    write_material(nbr, model.materials[i]);
  }

  // Save the textures
  file_write_bytes(nbr.file_handle, &model.textures_count, sizeof(u8));
  for(sizei i = 0; i < model.textures_count; i++) {
    write_texture(nbr, model.textures[i]);
  }
}

static void write_font(NBRFile& nbr, const NBRFont& font) {
  // Save the glyphs 
  file_write_bytes(nbr.file_handle, &font.glyphs_count, sizeof(font.glyphs_count));
  for(u32 i = 0; i < font.glyphs_count; i++) {
    // Save the unicode
    file_write_bytes(nbr.file_handle, &font.glyphs[i].unicode, sizeof(i8));
  
    // Save the size
    file_write_bytes(nbr.file_handle, &font.glyphs[i].width, sizeof(u16));
    file_write_bytes(nbr.file_handle, &font.glyphs[i].height, sizeof(u16));

    // Save the bounds
    file_write_bytes(nbr.file_handle, &font.glyphs[i].left, sizeof(i16));
    file_write_bytes(nbr.file_handle, &font.glyphs[i].right, sizeof(i16));
    file_write_bytes(nbr.file_handle, &font.glyphs[i].top, sizeof(i16));
    file_write_bytes(nbr.file_handle, &font.glyphs[i].bottom, sizeof(i16));

    // Save the offsets
    file_write_bytes(nbr.file_handle, &font.glyphs[i].offset_x, sizeof(i16));
    file_write_bytes(nbr.file_handle, &font.glyphs[i].offset_y, sizeof(i16));
    
    // Save glyph information
    file_write_bytes(nbr.file_handle, &font.glyphs[i].advance_x, sizeof(i16));
    file_write_bytes(nbr.file_handle, &font.glyphs[i].kern, sizeof(i16));
    file_write_bytes(nbr.file_handle, &font.glyphs[i].left_bearing, sizeof(i16));
  
    // Save the pixels
    sizei pixels_size = font.glyphs[i].width * font.glyphs[i].height;
    file_write_bytes(nbr.file_handle, font.glyphs[i].pixels, pixels_size);
  }

  // Save font information
  file_write_bytes(nbr.file_handle, &font.ascent, sizeof(font.ascent));
  file_write_bytes(nbr.file_handle, &font.descent, sizeof(font.descent));
  file_write_bytes(nbr.file_handle, &font.line_gap, sizeof(font.line_gap));
}

static void write_audio(NBRFile& nbr, const NBRAudio& audio) {
  // Save the format
  file_write_bytes(nbr.file_handle, &audio.format, sizeof(audio.format));
  
  // Save the sample rate
  file_write_bytes(nbr.file_handle, &audio.sample_rate, sizeof(audio.sample_rate));
  
  // Save the channels
  file_write_bytes(nbr.file_handle, &audio.channels, sizeof(audio.channels));
  
  // Save the size of the samples
  file_write_bytes(nbr.file_handle, &audio.size, sizeof(audio.size));
  
  // Save the samples
  file_write_bytes(nbr.file_handle, audio.samples, audio.size);
}

static void read_texture(NBRFile& nbr, NBRTexture* texture) {
  // Load the width and height 
  file_read_bytes(nbr.file_handle, &texture->width, sizeof(texture->width));  
  file_read_bytes(nbr.file_handle, &texture->height, sizeof(texture->height));  
  
  // Load the channels
  file_read_bytes(nbr.file_handle, &texture->channels, sizeof(texture->channels));  

  // Load the pixels
  sizei data_size = (texture->width * texture->height) * texture->channels;
  texture->pixels = memory_allocate(data_size);
  file_read_bytes(nbr.file_handle, texture->pixels, data_size);
}

static void read_cubemap(NBRFile& nbr, NBRCubemap* cubemap) {
  // Load the width and height 
  file_read_bytes(nbr.file_handle, &cubemap->width, sizeof(cubemap->width));  
  file_read_bytes(nbr.file_handle, &cubemap->height, sizeof(cubemap->height));  

  // Load the channels
  file_read_bytes(nbr.file_handle, &cubemap->channels, sizeof(cubemap->channels));  

  // Load the faces count
  file_read_bytes(nbr.file_handle, &cubemap->faces_count, sizeof(cubemap->faces_count));  

  // Load the pixels
  sizei data_size = (cubemap->width * cubemap->height) * cubemap->channels;
  for(sizei i = 0; i < cubemap->faces_count; i++) {
    cubemap->pixels[i] = (u8*)memory_allocate(data_size);
    file_read_bytes(nbr.file_handle, cubemap->pixels[i], data_size);
  }
}

static void read_shader(NBRFile& nbr, NBRShader* shader) {
  // Load the vertex length
  file_read_bytes(nbr.file_handle, &shader->vertex_length, sizeof(u16));
  shader->vertex_length += 1;

  // Load the vertex source string
  shader->vertex_source = (i8*)memory_allocate(shader->vertex_length); 
  file_read_bytes(nbr.file_handle, shader->vertex_source, shader->vertex_length - 1);
  shader->vertex_source[shader->vertex_length - 1] = '\0';
 
  // Load the pixel length
  file_read_bytes(nbr.file_handle, &shader->pixel_length, sizeof(u16));
  shader->pixel_length += 1;

  // Load the pixel source string
  shader->pixel_source = (i8*)memory_allocate(shader->pixel_length); 
  file_read_bytes(nbr.file_handle, shader->pixel_source, shader->pixel_length - 1);
  shader->pixel_source[shader->pixel_length - 1] = '\0';
}

static void read_material(NBRFile& nbr, NBRMaterial* material) {
  // Load the ambient color 
  file_read_bytes(nbr.file_handle, material->ambient, sizeof(f32) * 3); 

  // Load the diffuse color 
  file_read_bytes(nbr.file_handle, material->diffuse, sizeof(f32) * 3); 
  
  // Load the specular color 
  file_read_bytes(nbr.file_handle, material->specular, sizeof(f32) * 3); 
 
  // Load the texture indices
  file_read_bytes(nbr.file_handle, &material->diffuse_index, sizeof(i8)); 
  file_read_bytes(nbr.file_handle, &material->specular_index, sizeof(i8)); 
}

static void read_mesh(NBRFile& nbr, NBRMesh* mesh) {
  // Load the vertex type
  file_read_bytes(nbr.file_handle, &mesh->vertex_type, sizeof(u8));

  // Load the vertices
  file_read_bytes(nbr.file_handle, &mesh->vertices_count, sizeof(u32));
  mesh->vertices = (f32*)memory_allocate(sizeof(f32) * mesh->vertices_count); 
  file_read_bytes(nbr.file_handle, mesh->vertices, sizeof(f32) * mesh->vertices_count);

  // Load the indices
  file_read_bytes(nbr.file_handle, &mesh->indices_count, sizeof(u32));
  mesh->indices = (u32*)memory_allocate(sizeof(u32) * mesh->indices_count); 
  file_read_bytes(nbr.file_handle, mesh->indices, sizeof(u32) * mesh->indices_count);

  // Load the material index
  file_read_bytes(nbr.file_handle, &mesh->material_index, sizeof(u8));
}

static void read_model(NBRFile& nbr, NBRModel* model) {
  // Load the meshes
  file_read_bytes(nbr.file_handle, &model->meshes_count, sizeof(u16));
  model->meshes = (NBRMesh*)memory_allocate(sizeof(NBRMesh) * model->meshes_count); 
  for(sizei i = 0; i < model->meshes_count; i++) {
    read_mesh(nbr, &model->meshes[i]);
  }

  // Load the materials 
  file_read_bytes(nbr.file_handle, &model->materials_count, sizeof(u8));
  model->materials = (NBRMaterial*)memory_allocate(sizeof(NBRMaterial) * model->materials_count); 
  for(sizei i = 0; i < model->materials_count; i++) {
    read_material(nbr, &model->materials[i]); 
  }

  // Load the textures 
  file_read_bytes(nbr.file_handle, &model->textures_count, sizeof(u8));
  model->textures = (NBRTexture*)memory_allocate(sizeof(NBRTexture) * model->textures_count); 
  for(sizei i = 0; i < model->textures_count; i++) {
    read_texture(nbr, &model->textures[i]);
  }
}

static void read_font(NBRFile& nbr, NBRFont* font) {
  // Load the glyphs 
  file_read_bytes(nbr.file_handle, &font->glyphs_count, sizeof(font->glyphs_count));
  font->glyphs = (NBRGlyph*)memory_allocate(sizeof(NBRGlyph) * font->glyphs_count);

  for(u32 i = 0; i < font->glyphs_count; i++) {
    // Load the unicode
    file_read_bytes(nbr.file_handle, &font->glyphs[i].unicode, sizeof(i8));
  
    // Load the size
    file_read_bytes(nbr.file_handle, &font->glyphs[i].width, sizeof(u16));
    file_read_bytes(nbr.file_handle, &font->glyphs[i].height, sizeof(u16));

    // Load the bounds
    file_read_bytes(nbr.file_handle, &font->glyphs[i].left, sizeof(u16));
    file_read_bytes(nbr.file_handle, &font->glyphs[i].right, sizeof(u16));
    file_read_bytes(nbr.file_handle, &font->glyphs[i].top, sizeof(u16));
    file_read_bytes(nbr.file_handle, &font->glyphs[i].bottom, sizeof(u16));

    // Load the offsets
    file_read_bytes(nbr.file_handle, &font->glyphs[i].offset_x, sizeof(i16));
    file_read_bytes(nbr.file_handle, &font->glyphs[i].offset_y, sizeof(i16));
    
    // Load glyph information
    file_read_bytes(nbr.file_handle, &font->glyphs[i].advance_x, sizeof(i16));
    file_read_bytes(nbr.file_handle, &font->glyphs[i].kern, sizeof(i16));
    file_read_bytes(nbr.file_handle, &font->glyphs[i].left_bearing, sizeof(i16));
  
    // Load the pixels
    sizei pixels_size      = font->glyphs[i].width * font->glyphs[i].height;
    font->glyphs[i].pixels = (u8*)memory_allocate(pixels_size); 

    file_read_bytes(nbr.file_handle, font->glyphs[i].pixels, pixels_size);
  }

  // Load font information
  file_read_bytes(nbr.file_handle, &font->ascent, sizeof(font->ascent));
  file_read_bytes(nbr.file_handle, &font->descent, sizeof(font->descent));
  file_read_bytes(nbr.file_handle, &font->line_gap, sizeof(font->line_gap));
}

static void read_audio(NBRFile& nbr, NBRAudio* audio) {
  // Load the format
  file_read_bytes(nbr.file_handle, &audio->format, sizeof(audio->format));
  
  // Load the sample rate
  file_read_bytes(nbr.file_handle, &audio->sample_rate, sizeof(audio->sample_rate));
  
  // Load the channels
  file_read_bytes(nbr.file_handle, &audio->channels, sizeof(audio->channels));
  
  // Load the size of the samples
  file_read_bytes(nbr.file_handle, &audio->size, sizeof(audio->size));
  
  // Load the samples
  audio->samples = (i16*)memory_allocate(audio->size); 
  file_read_bytes(nbr.file_handle, audio->samples, audio->size);
}

static void load_texture(NBRFile& nbr) {
  // Read the resource from the file 
  NBRTexture texture; 
  read_texture(nbr, &texture); 

  // Allocate some space for the resource and assign it
  nbr.body_data = memory_allocate(sizeof(texture));
  memory_copy(nbr.body_data, &texture, sizeof(texture)); 
}

static void load_cubemap(NBRFile& nbr) {
  // Read the resource from the file 
  NBRCubemap cubemap; 
  read_cubemap(nbr, &cubemap); 
  
  // Allocate some space for the resource and assign it
  nbr.body_data = memory_allocate(sizeof(cubemap));
  memory_copy(nbr.body_data, &cubemap, sizeof(cubemap)); 
}

static void load_shader(NBRFile& nbr) {
  // Read the resource from the file 
  NBRShader shader; 
  read_shader(nbr, &shader); 

  // Allocate some space for the resource and assign it
  nbr.body_data = memory_allocate(sizeof(NBRShader));
  memory_copy(nbr.body_data, &shader, sizeof(NBRShader));
}

static void load_model(NBRFile& nbr) {
  // Read the resource from the file 
  NBRModel model; 
  read_model(nbr, &model);

  // Allocate some space for the resource and assign it
  nbr.body_data = memory_allocate(sizeof(model));
  memory_copy(nbr.body_data, &model, sizeof(model)); 
}

static void load_font(NBRFile& nbr) {
  // Read the resource from the file 
  NBRFont font; 
  read_font(nbr, &font);

  // Allocate some space for the resource and assign it
  nbr.body_data = memory_allocate(sizeof(font));
  memory_copy(nbr.body_data, &font, sizeof(font)); 
}

static void load_audio(NBRFile& nbr) {
  // Read the resource from the file 
  NBRAudio audio; 
  read_audio(nbr, &audio);

  // Allocate some space for the resource and assign it
  nbr.body_data = memory_allocate(sizeof(audio));
  memory_copy(nbr.body_data, &audio, sizeof(audio)); 
}

static void unload_texture(NBRFile& nbr) {
  NBRTexture* tex = (NBRTexture*)nbr.body_data;
  memory_free(tex->pixels);
}

static void unload_cubemap(NBRFile& nbr) {
  NBRCubemap* cube = (NBRCubemap*)nbr.body_data;
  
  for(sizei i = 0; i < cube->faces_count; i++) {
    memory_free(cube->pixels[i]);
  }
}

static void unload_shader(NBRFile& nbr) {
  NBRShader* shader = (NBRShader*)nbr.body_data;

  memory_free(shader->vertex_source);
  memory_free(shader->pixel_source);
}

static void unload_model(NBRFile& nbr) {
  NBRModel* model = (NBRModel*)nbr.body_data;

  for(sizei i = 0; i < model->meshes_count; i++) {
    memory_free(model->meshes[i].vertices);
    memory_free(model->meshes[i].indices);
  }

  for(sizei i = 0; i < model->textures_count; i++) {
    memory_free(model->textures[i].pixels);
  }

  memory_free(model->meshes);
  memory_free(model->materials);
  memory_free(model->textures);
}

static void unload_font(NBRFile& nbr) {
  NBRFont* font = (NBRFont*)nbr.body_data;

  for(u32 i = 0; i < font->glyphs_count; i++) {
    memory_free(font->glyphs[i].pixels);
  }

  memory_free(font->glyphs);
}

static void unload_audio(NBRFile& nbr) {
  NBRAudio* audio = (NBRAudio*)nbr.body_data;
  memory_free(audio->samples);
}

static void load_by_type(NBRFile& nbr, const FilePath& path) {
  switch(nbr.resource_type) {
    case RESOURCE_TYPE_TEXTURE:
      load_texture(nbr);
      break;
    case RESOURCE_TYPE_CUBEMAP:
      load_cubemap(nbr);
      break;
    case RESOURCE_TYPE_SHADER:
      load_shader(nbr);
      break;
    case RESOURCE_TYPE_MODEL:
      load_model(nbr);
      break;
    case RESOURCE_TYPE_FONT:
      load_font(nbr);
      break;
    case RESOURCE_TYPE_AUDIO_BUFFER:
      load_audio(nbr);
      break;
    default:
      NIKOLA_LOG_ERROR("Cannot load specified resource type at NBR file \'%s\'", path.c_str());
      break;
  }
}

static void unload_by_type(NBRFile& nbr) {
  switch(nbr.resource_type) {
    case RESOURCE_TYPE_TEXTURE:
      unload_texture(nbr);
      break;
    case RESOURCE_TYPE_CUBEMAP:
      unload_cubemap(nbr);
      break;
    case RESOURCE_TYPE_SHADER:
      unload_shader(nbr);
      break;
    case RESOURCE_TYPE_MODEL:
      unload_model(nbr);
      break;
    case RESOURCE_TYPE_FONT:
      unload_font(nbr);
      break;
    case RESOURCE_TYPE_AUDIO_BUFFER:
      unload_audio(nbr);
      break;
    default:
      break;
  }
}

static void save_header(NBRFile& nbr) {
  nbr.identifier    = NBR_VALID_IDENTIFIER;
  nbr.major_version = NBR_VALID_MAJOR_VERSION;
  nbr.minor_version = NBR_VALID_MINOR_VERSION;

  // Save the identifier
  file_write_bytes(nbr.file_handle, &nbr.identifier, sizeof(nbr.identifier));

  // Save the major and minor versions
  file_write_bytes(nbr.file_handle, &nbr.major_version, sizeof(nbr.major_version));
  file_write_bytes(nbr.file_handle, &nbr.minor_version, sizeof(nbr.minor_version));

  // Save the resource type
  file_write_bytes(nbr.file_handle, &nbr.resource_type, sizeof(nbr.resource_type));
}

static i32 get_texture_channels(GfxTextureFormat format) {
  switch(format) {
    case GFX_TEXTURE_FORMAT_R8:
    case GFX_TEXTURE_FORMAT_R16:
      return 1;
    case GFX_TEXTURE_FORMAT_RG8:
    case GFX_TEXTURE_FORMAT_RG16:
      return 2;
    case GFX_TEXTURE_FORMAT_RGBA8:
    case GFX_TEXTURE_FORMAT_RGBA16:
      return 4;
    case GFX_TEXTURE_FORMAT_DEPTH_STENCIL_24_8:
      return 2;
  } 
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NBR (Nikola Binary Resource) functions

void nbr_file_load(NBRFile* nbr, const FilePath& path) {
  NIKOLA_ASSERT(nbr, "Cannot load an invalid NBR file");

  // Open the NBR file
  if(!open_for_load(*nbr, path)) {
    return;
  }

  // Read the identifier
  file_read_bytes(nbr->file_handle, &nbr->identifier, sizeof(nbr->identifier));

  // Read the major and minor versions
  file_read_bytes(nbr->file_handle, &nbr->major_version, sizeof(nbr->major_version));
  file_read_bytes(nbr->file_handle, &nbr->minor_version, sizeof(nbr->minor_version));

  // Read the resource type
  file_read_bytes(nbr->file_handle, &nbr->resource_type, sizeof(nbr->resource_type));

  // Make sure everything is looking good
  if(!check_nbr_validity(*nbr, path)) {
    file_close(nbr->file_handle);
    return;
  }

  // Load the specified resource type and store it in `nbr.body_data`
  load_by_type(*nbr, path);

  // Close the file as it is not needed anymore 
  file_close(nbr->file_handle);
}

void nbr_file_unload(NBRFile& nbr) {
  file_close(nbr.file_handle);

  if(nbr.body_data) {
    unload_by_type(nbr);
    memory_free(nbr.body_data);
  }
}

const bool nbr_file_valid_extension(const FilePath& nbr_path) {
  String ext = filepath_extension(nbr_path);

  return ext == ".nbrtexture" || 
         ext == ".nbrcubemap" || 
         ext == ".nbrshader"  || 
         ext == ".nbrmodel";
}

void nbr_file_save(NBRFile& nbr, const NBRTexture& texture, const FilePath& path) {
  // Make sure to set the correct extension
  FilePath nbr_path = path;
  filepath_set_extension(nbr_path, "nbrtexture");

  // Must open the file
  if(!open_for_save(nbr, nbr_path)) {
    return;
  }

  // Save the header first
  nbr.resource_type = (u16)RESOURCE_TYPE_TEXTURE; 
  save_header(nbr);

  // Write the texture 
  write_texture(nbr, texture); 

  // Always remember to close the file
  file_close(nbr.file_handle);
}

void nbr_file_save(NBRFile& nbr, const NBRCubemap& cubemap, const FilePath& path) {
  // Make sure to set the correct extension
  FilePath nbr_path = path;
  filepath_set_extension(nbr_path, "nbrcubemap");

  // Must open the file
  if(!open_for_save(nbr, nbr_path)) {
    return;
  }

  // Save the header first
  nbr.resource_type = (u16)RESOURCE_TYPE_CUBEMAP; 
  save_header(nbr);

  // Write the cubemap
  write_cubemap(nbr, cubemap);

  // Always remember to close the file
  file_close(nbr.file_handle);
}

void nbr_file_save(NBRFile& nbr, const NBRShader& shader, const FilePath& path) {
  // Make sure to set the correct extension
  FilePath nbr_path = path;
  filepath_set_extension(nbr_path, "nbrshader");

  // Must open the file
  if(!open_for_save(nbr, nbr_path)) {
    return;
  }

  // Save the header first
  nbr.resource_type = (u16)RESOURCE_TYPE_SHADER; 
  save_header(nbr);

  // Write the shader 
  write_shader(nbr, shader);

  // Always remember to close the file
  file_close(nbr.file_handle);
}

void nbr_file_save(NBRFile& nbr, const NBRModel& model, const FilePath& path) {
  // Make sure to set the correct extension
  FilePath nbr_path = path;
  filepath_set_extension(nbr_path, "nbrmodel");

  // Must open the file
  if(!open_for_save(nbr, nbr_path)) {
    return;
  }

  // Save the header first
  nbr.resource_type = (u16)RESOURCE_TYPE_MODEL; 
  save_header(nbr);

  // Write the model 
  write_model(nbr, model);

  // Always remember to close the file
  file_close(nbr.file_handle);
}

void nbr_file_save(NBRFile& nbr, const NBRFont& font, const FilePath& path) {
  // Make sure to set the correct extension
  FilePath nbr_path = path;
  filepath_set_extension(nbr_path, "nbrfont");

  // Must open the file
  if(!open_for_save(nbr, nbr_path)) {
    return;
  }

  // Save the header first
  nbr.resource_type = (u16)RESOURCE_TYPE_FONT; 
  save_header(nbr);

  // Write the font 
  write_font(nbr, font);

  // Always remember to close the file
  file_close(nbr.file_handle);
}

void nbr_file_save(NBRFile& nbr, const NBRAudio& audio, const FilePath& path) {
  // Make sure to set the correct extension
  FilePath nbr_path = path;
  filepath_set_extension(nbr_path, "nbraudio");

  // Must open the file
  if(!open_for_save(nbr, nbr_path)) {
    return;
  }

  // Save the header first
  nbr.resource_type = (u16)RESOURCE_TYPE_AUDIO_BUFFER; 
  save_header(nbr);

  // Write the audio 
  write_audio(nbr, audio);

  // Always remember to close the file
  file_close(nbr.file_handle);
}

/// NBR (Nikola Binary Resource) functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
