#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static bool check_nbr_validity(NBRFile& file, const FilePath& path) {
  // Check for the validity of the identifier
  if(file.identifier != NBR_VALID_IDENTIFIER) {
    NIKOLA_LOG_ERROR("Invalid identifier found in NBR file at \'%s\'. Expected \'%i\' got \'%i\'", path.string().c_str(), NBR_VALID_IDENTIFIER, file.identifier);
    return false;
  }  

  // Check for the validity of the versions
  bool is_valid_version = ((file.major_version == NBR_VALID_MAJOR_VERSION) || (file.minor_version == NBR_VALID_MINOR_VERSION));
  if(!is_valid_version) {
    NIKOLA_LOG_ERROR("Invalid version found in NBR file at \'%s\'", path.string().c_str());
    return false;
  }

  return true;
}

static void load_texture(NBRFile& nbr) {
  NBRTexture texture; 

  // Load the width and height 
  file_read_bytes(nbr.file_handle, &texture.width, sizeof(texture.width));  
  file_read_bytes(nbr.file_handle, &texture.height, sizeof(texture.height));  
  
  // Load the channels
  file_read_bytes(nbr.file_handle, &texture.channels, sizeof(texture.channels));  

  // Load the pixels
  sizei data_size = (texture.width * texture.height) * texture.channels;
  texture.pixels  = memory_allocate(data_size);
  file_read_bytes(nbr.file_handle, texture.pixels, data_size);
  
  // Allocate some space for the resource and assign it
  nbr.body_data = memory_allocate(sizeof(texture));
  memory_copy(nbr.body_data, &texture, sizeof(texture)); 
}

static void load_cubemap(NBRFile& nbr) {
  NBRCubemap cubemap; 

  // Load the width and height 
  file_read_bytes(nbr.file_handle, &cubemap.width, sizeof(cubemap.width));  
  file_read_bytes(nbr.file_handle, &cubemap.height, sizeof(cubemap.height));  

  // Load the channels
  file_read_bytes(nbr.file_handle, &cubemap.channels, sizeof(cubemap.channels));  

  // Load the faces count
  file_read_bytes(nbr.file_handle, &cubemap.faces_count, sizeof(cubemap.faces_count));  

  // Load the pixels
  sizei data_size = (cubemap.width * cubemap.height) * cubemap.channels;
  for(sizei i = 0; i < cubemap.faces_count; i++) {
    cubemap.pixels[i] = (u8*)memory_allocate(data_size);
    file_read_bytes(nbr.file_handle, cubemap.pixels[i], data_size);
  }
  
  // Allocate some space for the resource and assign it
  nbr.body_data = memory_allocate(sizeof(cubemap));
  memory_copy(nbr.body_data, &cubemap, sizeof(cubemap)); 
}

static void load_shader(NBRFile& nbr) {
  NBRShader shader; 
  
  // Load the length
  u32 src_length = 0;
  file_read_bytes(nbr.file_handle, &src_length, sizeof(src_length));
  src_length += 1; 

  // Load the src string
  i8* src_str = (i8*)memory_allocate(src_length); 
  file_read_bytes(nbr.file_handle, src_str, src_length - 1);
  src_str[src_length - 1] = '\0';
 
  shader = String(src_str, src_length);

  // Allocate some space for the resource and assign it
  nbr.body_data = memory_allocate(src_length);
  memory_copy(nbr.body_data, src_str, src_length);

  // Goodbye, extra string
  memory_free(src_str);
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
      break;
    case RESOURCE_TYPE_FONT:
      break;
    default:
      NIKOLA_LOG_ERROR("Cannot load specified resource type at NBR file \'%s\'", path.string().c_str());
      break;
  }
}

static void unload_by_type(NBRFile& nbr) {
  switch(nbr.resource_type) {
    case RESOURCE_TYPE_TEXTURE: {
      NBRTexture* tex = (NBRTexture*)nbr.body_data;
      memory_free(tex->pixels);
    } break;
    case RESOURCE_TYPE_CUBEMAP: {
      NBRCubemap* cube = (NBRCubemap*)nbr.body_data;
      for(sizei i = 0; i < cube->faces_count; i++) {
        memory_free(cube->pixels[i]);
      }
    }
      break;
    case RESOURCE_TYPE_SHADER:
      break;
    case RESOURCE_TYPE_MODEL:
      break;
    case RESOURCE_TYPE_FONT:
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
  NIKOLA_ASSERT((path.extension().string() == ".nbr"), "An NBR file with an invalid extension");

  // Open the NBR file
  if(!file_open(&nbr->file_handle, path, (i32)(std::ios::in | std::ios::binary))) {
    NIKOLA_LOG_ERROR("Cannot load NBR file at \'%s\'", path.string().c_str());
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

void nbr_file_save(NBRFile& nbr, const NBRTexture& texture, const FilePath& path) {
  // Must open the file
  if(!file_open(&nbr.file_handle, path, (i32)(std::ios::out | std::ios::binary))) {
    NIKOLA_LOG_ERROR("Cannot save NBR file at \'%s\'", path.string().c_str());
    return;
  }

  // Save the header first
  nbr.resource_type = (i16)RESOURCE_TYPE_TEXTURE; 
  save_header(nbr);

  // Save width and height
  file_write_bytes(nbr.file_handle, &texture.width, sizeof(texture.width));
  file_write_bytes(nbr.file_handle, &texture.height, sizeof(texture.height));
  
  // Save the channels
  file_write_bytes(nbr.file_handle, &texture.channels, sizeof(texture.channels));
 
  // Save the pixels
  sizei data_size = (texture.width * texture.height) * texture.channels;
  file_write_bytes(nbr.file_handle, texture.pixels, data_size);
  
  // Always remember to close the file
  file_close(nbr.file_handle);
}

void nbr_file_save(NBRFile& nbr, const NBRCubemap& cubemap, const FilePath& path) {
  // Must open the file
  if(!file_open(&nbr.file_handle, path, (i32)(std::ios::out | std::ios::binary))) {
    NIKOLA_LOG_ERROR("Cannot save NBR file at \'%s\'", path.string().c_str());
    return;
  }

  // Save the header first
  nbr.resource_type = (i16)RESOURCE_TYPE_CUBEMAP; 
  save_header(nbr);

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

  // Always remember to close the file
  file_close(nbr.file_handle);
}

void nbr_file_save(NBRFile& nbr, const NBRShader& shader, const FilePath& path) {
  // Must open the file
  if(!file_open(&nbr.file_handle, path, (i32)(std::ios::out | std::ios::binary))) {
    NIKOLA_LOG_ERROR("Cannot save NBR file at \'%s\'", path.string().c_str());
    return;
  }

  // Save the header first
  nbr.resource_type = (i16)RESOURCE_TYPE_SHADER; 
  save_header(nbr);

  // Convert the given shader to an NBR format
  u32 src_length = (u32)shader.length();
  i8* src_str    = (i8*)shader.c_str();

  // Save the length of the shader's code string 
  file_write_bytes(nbr.file_handle, &src_length, sizeof(src_length));

  // Save the shader's code string
  file_write_bytes(nbr.file_handle, src_str, src_length);

  // Always remember to close the file
  file_close(nbr.file_handle);
}

/// NBR (Nikola Binary Resource) functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
