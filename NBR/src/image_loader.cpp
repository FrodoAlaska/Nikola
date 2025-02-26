#include "nbr.hpp"

#include <nikola/nikola_core.hpp>
#include <nikola/nikola_engine.hpp>

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
/// Image loader functions

bool image_loader_load_texture(nikola::NBRTexture* texture, const nikola::FilePath& path) {
  if(!check_valid_extension(path.extension())) {
    NIKOLA_LOG_ERROR("Invalid image file at \'%s\'", path.string().c_str());
    return false;
  }

  nikola::i32 width, height, channels; 
  texture->pixels = stbi_load(path.string().c_str(), &width, &height, &channels, 4);

  if(!texture->pixels) {
    NIKOLA_LOG_ERROR("Could not load texture at \'%s\', %s", path.string().c_str(), stbi_failure_reason());
    return false;
  }

  texture->width    = width;
  texture->height   = height;
  texture->channels = 4; // Sadly, sometimes the loader depicts the cubemap faces with 3 components instead of 4, so we have to force it.

  return true;
}

bool image_loader_load_cubemap(nikola::NBRCubemap* cube, const nikola::FilePath& dir) {
  nikola::u8 faces = 0; 
  nikola::i32 width, height, channels; 

  // Go through each texture of the cubemap directory
  for(auto& p : std::filesystem::recursive_directory_iterator(dir)) {
    if(!check_valid_extension(p.path().extension())) {
      NIKOLA_LOG_ERROR("Invalid image file at \'%s\'", p.path().string().c_str());
      return false;
    }
  
    cube->pixels[faces] = stbi_load(p.path().string().c_str(), &width, &height, &channels, 4);
  
    if(!cube->pixels[faces]) {
      NIKOLA_LOG_ERROR("Could not load cubemap face at \'%s\', %s", p.path().string().c_str(), stbi_failure_reason());
      return false;
    }

    faces++;
  }

  cube->width       = width;
  cube->height      = height;
  cube->channels    = 4; // Sadly, sometimes the loader depicts the cubemap faces with 3 components instead of 4, so we have to force it.
  cube->faces_count = faces;

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
