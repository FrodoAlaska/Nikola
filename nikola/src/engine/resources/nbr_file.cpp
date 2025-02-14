#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static bool check_nbr_validity(NBRFile& file) {
  // Check for the validity of the identifier
  if(file.identifier != NBR_VALID_IDENTIFIER) {
    NIKOLA_LOG_ERROR("Invalid identifier found in NBR file at \'%s\'", file.path.string().c_str());
    return false;
  }  

  // Check for the validity of the versions
  bool is_valid_version = ((file.major_version == NBR_VALID_MAJOR_VERSION) || (file.minor_version == NBR_VALID_MINOR_VERSION));
  if(!is_valid_version) {
    NIKOLA_LOG_ERROR("Invalid version found in NBR file at \'%s\'", file.path.string().c_str());
    return false;
  }

  return true;
}

static void load_texture(NBRFile& nbr) {
  // Allocate some space for the resource and assign it
  nbr.body_data       = memory_allocate(sizeof(NBRTexture));
  NBRTexture* texture = (NBRTexture*)nbr.body_data; 

  // Load the width and height 
  file_read_bytes(nbr.file_handle, &texture->width, sizeof(texture->width));  
  file_read_bytes(nbr.file_handle, &texture->height, sizeof(texture->height));  

  // Load the channels
  file_read_bytes(nbr.file_handle, &texture->channels, sizeof(texture->channels));  

  // Load the pixels
  sizei data_size = (texture->width * texture->height) * sizeof(u8);
  file_read_bytes(nbr.file_handle, &texture->pixels, data_size);
} 

static void load_cubemap(NBRFile& nbr) {
  // Allocate some space for the resource and assign it
  nbr.body_data       = memory_allocate(sizeof(NBRCubemap));
  NBRCubemap* cubemap = (NBRCubemap*)nbr.body_data; 

  // Load the width and height 
  file_read_bytes(nbr.file_handle, &cubemap->width, sizeof(cubemap->width));  
  file_read_bytes(nbr.file_handle, &cubemap->height, sizeof(cubemap->height));  

  // Load the channels
  file_read_bytes(nbr.file_handle, &cubemap->channels, sizeof(cubemap->channels));  

  // Load the faces count
  file_read_bytes(nbr.file_handle, &cubemap->faces_count, sizeof(cubemap->faces_count));  

  // Load the pixels
  sizei data_size = (cubemap->width * cubemap->height) * sizeof(u8);
  for(sizei i = 0; i < cubemap->faces_count; i++) {
    file_read_bytes(nbr.file_handle, &cubemap->pixels[i], data_size);
  }
}

static void load_shader(NBRFile& nbr) {
  // Load the length
  u32 src_length = 0;
  file_read_bytes(nbr.file_handle, &src_length, sizeof(src_length));

  // Load the src string
  i8* src_str = (i8*)memory_allocate(src_length); 
  file_read_bytes(nbr.file_handle, &src_str, src_length);

  // Allocate some space for the resource and assign it
  String str        = String(src_str, src_length);
  nbr.body_data     = memory_allocate(sizeof(str));
  NBRShader* shader = (NBRShader*)nbr.body_data; 

  *shader = str;
}

static void load_by_type(NBRFile& nbr) {
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
      break;
    case RESOURCE_TYPE_FONT:
      break;
    default:
      NIKOLA_LOG_ERROR("Cannot load specified resource type at NBR file \'%s\'", nbr.path.string().c_str());
      break;
  }
}

static void save_header(NBRFile& nbr) {
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
  NIKOLA_ASSERT((nbr->path.extension().string() == "nbr"), "An NBR file with an invalid extension");

  if(!file_open(&nbr->file_handle, path, FILE_OPEN_READ | FILE_OPEN_BINARY)) {
    NIKOLA_LOG_ERROR("Cannot open NBR file at \'%s\'", path.string().c_str());
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
  if(!check_nbr_validity(*nbr)) {
    file_close(nbr->file_handle);
    return;
  }

  // Load the specified resource type and store it in `nbr.body_data`
  load_by_type(*nbr);

  // Close the file as it is not needed anymore 
  file_close(nbr->file_handle);
}

void nbr_file_unload(NBRFile& nbr) {
  file_close(nbr.file_handle);

  if(nbr.body_data) {
    memory_free(nbr.body_data);
  }
}

void nbr_file_save(NBRFile& nbr, GfxTexture* texture, const FilePath& path) {
  NIKOLA_ASSERT(texture, "Cannot save an invalid resource to an NBR file");

  // Must open the file
  if(!file_open(&nbr.file_handle, path, FILE_OPEN_WRITE | FILE_OPEN_BINARY)) {
    NIKOLA_LOG_ERROR("Could not open NBR file at \'%s\'", path.string().c_str());
    return;
  }

  // Save the header first
  nbr.resource_type = (i16)RESOURCE_TYPE_TEXTURE; 
  save_header(nbr);

  // Allocate a new texture 
  nbr.body_data            = (void*)memory_allocate(sizeof(NBRTexture));
  NBRTexture* nbr_texture  = (NBRTexture*)nbr.body_data;

  // Retrieve the description in order to convert it to an NBR format
  GfxTextureDesc& tex_desc = gfx_texture_get_desc(texture); 

  // Convert the given texture to an NBR format
  nbr_texture->width    = tex_desc.width;
  nbr_texture->height   = tex_desc.height;
  nbr_texture->channels = (i8)get_texture_channels(tex_desc.format);
  nbr_texture->pixels   = (u8*)tex_desc.data;

  // Save width and height
  file_write_bytes(nbr.file_handle, &nbr_texture->width, sizeof(nbr_texture->width));
  file_write_bytes(nbr.file_handle, &nbr_texture->height, sizeof(nbr_texture->height));

  // Save the channels
  file_write_bytes(nbr.file_handle, &nbr_texture->channels, sizeof(nbr_texture->channels));
 
  // Save the pixels
  sizei data_size = (nbr_texture->width * nbr_texture->height) * sizeof(u8);
  file_write_bytes(nbr.file_handle, &nbr_texture->pixels, data_size);
}

void nbr_file_save(NBRFile& nbr, GfxCubemap* cubemap, const FilePath& path) {
  NIKOLA_ASSERT(cubemap, "Cannot save an invalid resource to an NBR file");

  // Must open the file
  if(!file_open(&nbr.file_handle, path, FILE_OPEN_WRITE | FILE_OPEN_BINARY)) {
    NIKOLA_LOG_ERROR("Could not open NBR file at \'%s\'", path.string().c_str());
    return;
  }

  // Save the header first
  nbr.resource_type = (i16)RESOURCE_TYPE_CUBEMAP; 
  save_header(nbr);

  // Allocate a new texture 
  nbr.body_data            = (void*)memory_allocate(sizeof(NBRCubemap));
  NBRCubemap* nbr_cubemap  = (NBRCubemap*)nbr.body_data;

  // Retrieve the description in order to convert it to an NBR format
  GfxCubemapDesc& cube_desc = gfx_cubemap_get_desc(cubemap); 

  // Convert the given cubemap to an NBR format
  nbr_cubemap->width       = cube_desc.width;
  nbr_cubemap->height      = cube_desc.height;
  nbr_cubemap->channels    = (i8)get_texture_channels(cube_desc.format);
  nbr_cubemap->faces_count = (i8)cube_desc.faces_count;
  
  // Convert all of the pixesl
  for(sizei i = 0; i < nbr_cubemap->faces_count; i++) {
    nbr_cubemap->pixels[i] = (u8*)cube_desc.data[i];
  }

  // Save width and height
  file_write_bytes(nbr.file_handle, &nbr_cubemap->width, sizeof(nbr_cubemap->width));
  file_write_bytes(nbr.file_handle, &nbr_cubemap->height, sizeof(nbr_cubemap->height));

  // Save the channels
  file_write_bytes(nbr.file_handle, &nbr_cubemap->channels, sizeof(nbr_cubemap->channels));

  // Save the faces count
  file_write_bytes(nbr.file_handle, &nbr_cubemap->faces_count, sizeof(nbr_cubemap->faces_count));

  // Save the pixels for each face
  sizei data_size = (nbr_cubemap->width * nbr_cubemap->height) * sizeof(u8);
  for(sizei i = 0; i < nbr_cubemap->faces_count; i++) {
    file_write_bytes(nbr.file_handle, &nbr_cubemap->pixels[i], data_size);
  }
}

void nbr_file_save(NBRFile& nbr, GfxShader* shader, const FilePath& path) {
  NIKOLA_ASSERT(shader, "Cannot save an invalid resource to an NBR file");

  // Must open the file
  if(!file_open(&nbr.file_handle, path, FILE_OPEN_WRITE | FILE_OPEN_BINARY)) {
    NIKOLA_LOG_ERROR("Could not open NBR file at \'%s\'", path.string().c_str());
    return;
  }

  // Save the header first
  nbr.resource_type = (i16)RESOURCE_TYPE_SHADER; 
  save_header(nbr);

  // Get the shader's source string
  const i8* shader_src = gfx_shader_get_source(shader);

  // Allocate a new texture 
  nbr.body_data          = (void*)memory_allocate(String(shader_src).length());
  NBRShader* nbr_shader  = (NBRShader*)nbr.body_data;
 
  // Convert the given shader to an NBR format
  *nbr_shader    = shader_src;
  u32 src_length = (u32)nbr_shader->length();
  i8* src_str    = (i8*)nbr_shader->c_str();

  // Save the length of the shader's code string 
  file_write_bytes(nbr.file_handle, &src_length, sizeof(src_length));

  // Save the shader's code string
  file_write_bytes(nbr.file_handle, &src_str, src_length * sizeof(i8));
}

/// NBR (Nikola Binary Resource) functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
