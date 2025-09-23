#include "nbr.h"

#include <nikola/nikola.h>

#include <stb/stb_image.h>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nbr

/// ----------------------------------------------------------------------
/// Private functions

static bool check_valid_extension(const nikola::FilePath& ext) {
  return ext == ".png"  ||  
         ext == ".jpg"  ||  
         ext == ".jpeg" ||  
         ext == ".bmp"  ||  
         ext == ".psd"  ||  
         ext == ".tga"  ||  
         ext == ".gif"  ||  
         ext == ".hdr"  ||  
         ext == ".pic"  ||  
         ext == ".ppm"  ||  
         ext == ".pgm";
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks 

static void directory_iterate_func(const nikola::FilePath& base_dir, const nikola::FilePath& current_path, void* user_data) {
  nikola::NBRCubemap* cube = (nikola::NBRCubemap*)user_data;

  // Check for the extension
  if(!check_valid_extension(nikola::filepath_extension(current_path))) {
    NIKOLA_LOG_WARN("Invalid image file found at \'%s\'. Ignoring it.", current_path.c_str());
    return;
  }

  // Load the face

  nikola::i32 width, height; 
  cube->faces_count++;
  
  if(stbi_is_hdr(current_path.c_str())) {
    cube->format                        = (nikola::u8)nikola::GFX_TEXTURE_FORMAT_RGBA16F;
    cube->pixels[cube->faces_count - 1] = stbi_loadf(current_path.c_str(), &width, &height, NULL, 4);
  }
  else {
    cube->format                        = (nikola::u8)nikola::GFX_TEXTURE_FORMAT_RGBA8;
    cube->pixels[cube->faces_count - 1] = stbi_load(current_path.c_str(), &width, &height, NULL, 4);
  }
  
  // Check for errors
  
  if(!cube->pixels[cube->faces_count - 1]) {
    NIKOLA_LOG_ERROR("Could not load cubemap face at %s", stbi_failure_reason());
    return;
  }

  cube->width  = width; 
  cube->height = height;
}

/// Callbacks 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Image loader functions

bool image_loader_load_texture(nikola::NBRTexture* texture, const nikola::FilePath& path) {
  if(!check_valid_extension(nikola::filepath_extension(path))) {
    NIKOLA_LOG_WARN("Invalid image file found at \'%s\'. Ignoring it.", path.c_str());
    return false;
  }

  nikola::i32 width, height; 
  if(stbi_is_hdr(path.c_str())) {
    texture->format = (nikola::u8)nikola::GFX_TEXTURE_FORMAT_RGBA16F;
    texture->pixels = stbi_loadf(path.c_str(), &width, &height, NULL, 4);
  }
  else {
    texture->format = (nikola::u8)nikola::GFX_TEXTURE_FORMAT_RGBA8;
    texture->pixels = stbi_load(path.c_str(), &width, &height, NULL, 4);
  }

  if(!texture->pixels) {
    NIKOLA_LOG_ERROR("Could not load texture at \'%s'\, %s", path.c_str(), stbi_failure_reason());
    return false;
  }

  texture->width    = width;
  texture->height   = height;
  texture->channels = 4; // Sadly, sometimes the loader depicts the texture with 3 components instead of 4, so we have to force it.

  return true;
}

bool image_loader_load_cubemap(nikola::NBRCubemap* cube, const nikola::FilePath& dir) {
  cube->faces_count = 0;

  // Go through each texture of the cubemap directory
  nikola::filesystem_directory_iterate(dir, directory_iterate_func, cube);

  // Sadly, sometimes the loader depicts the cubemap faces with 3 components instead of 4, so we have to force it.
  cube->channels = 4; 

  return true;
}

void image_loader_unload_texture(nikola::NBRTexture& texture) {
  if(!texture.pixels) {
    return;
  }
  
  stbi_image_free(texture.pixels);
}

void image_loader_unload_cubemap(nikola::NBRCubemap& cubemap) {
  for(nikola::sizei i = 0; i < cubemap.faces_count; i++) {
    if(!cubemap.pixels[i]) {
      continue;
    }

    stbi_image_free(cubemap.pixels[i]);
  }
}

/// Image loader functions
/// ----------------------------------------------------------------------

} // End of nbr

//////////////////////////////////////////////////////////////////////////
