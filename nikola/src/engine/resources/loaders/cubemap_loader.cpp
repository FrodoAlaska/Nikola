#include "nikola/nikola_core.hpp"
#include "nikola/nikola_engine.hpp"

#include <stb/stb_image.h>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Private functions

static inline bool is_valid_extension(const String& ext) {
  return ext == "jpeg" || 
         ext == "png"  || 
         ext == "bmp"  || 
         ext == "psd"  || 
         ext == "tga"  || 
         ext == "gif"  || 
         ext == "hdr"  || 
         ext == "pic"  || 
         ext == "ppm"  || 
         ext == "pgm";
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Cubemap loader functions

void cubemap_loader_load(GfxCubemapDesc* desc, 
                         const FilePath path[CUBEMAP_FACES_MAX], 
                         const sizei faces_count,
                         const GfxTextureFormat format, 
                         const GfxTextureFilter filter, 
                         const GfxTextureWrap wrap) {
  NIKOLA_ASSERT((faces_count > CUBEMAP_FACES_MAX), "Cannot load more than 6 faces in a cubemap");

  i32 width, height, channels; 
  for(sizei i = 0; i < faces_count; i++) {
    desc->data[i] = stbi_load((const char*)path[i].c_str(), &width, &height, &channels, 4); 
    if(!desc->data[i]) {
      NIKOLA_LOG_ERROR("Could not load cubemap face at \'%s\'", path[i].c_str());
    }
  }

  desc->width       = width; 
  desc->height      = height; 
  desc->mips        = height; 
  desc->format      = format; 
  desc->filter      = filter; 
  desc->wrap_mode   = wrap; 
  desc->faces_count = faces_count; 
}

void cubemap_loader_load(GfxCubemapDesc* desc, 
                         const FilePath directory, 
                         const sizei faces_count,
                         const GfxTextureFormat format, 
                         const GfxTextureFilter filter, 
                         const GfxTextureWrap wrap) {
  DynamicArray<FilePath> paths(CUBEMAP_FACES_MAX);

  u32 count = 0;
  for(auto& p : std::filesystem::recursive_directory_iterator(directory)) {
    // We only care about indivisual files with the supported extensions
    if(!p.is_regular_file() && !is_valid_extension(p.path().extension().string())) {
      continue;
    }

    paths[count] = p.path(); 
    count++;
  }

  cubemap_loader_load(desc, paths.data(), count, format, filter, wrap);
}

void cubemap_loader_unload(GfxCubemapDesc& desc) {
  for(sizei i = 0; i < desc.faces_count; i++) {
    stbi_image_free(desc.data[i]);
  }
}

/// Cubemap loader functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
