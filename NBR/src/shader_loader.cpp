#include "nbr.h"

#include <nikola/nikola.h>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nbr

/// ----------------------------------------------------------------------
/// Private functions

static bool compile_compute_shader(const nikola::sizei start, const nikola::String& src, nikola::NBRShader* shader) {
  nikola::sizei begin        = src.find_first_of('#', start); 
  nikola::String compute_src = src.substr(begin);
  
  shader->compute_length = (nikola::u16)compute_src.size();
  shader->compute_source = (nikola::i8*)nikola::memory_allocate(shader->compute_length + 1); 

  nikola::memory_copy(shader->compute_source, compute_src.c_str(), shader->compute_length + 1);
  shader->compute_source[shader->compute_length] = '\0';

  return true;
}

static const nikola::sizei compile_vertex_shader(const nikola::sizei start, const nikola::String& src, nikola::NBRShader* shader) {
  nikola::sizei begin     = src.find_first_of('#', start); 
  nikola::String vert_src = "";
  for(nikola::sizei i = begin; i < src.size(); i++) { // @NOTE: Apparently the combination of `substr` and `find_first_of` does not work here
    if(src[i] == '@') {
      break;
    }

    vert_src += src[i];
  }
  
  shader->vertex_length = (nikola::u16)vert_src.size();
  shader->vertex_source = (nikola::i8*)nikola::memory_allocate(shader->vertex_length + 1); 

  nikola::memory_copy(shader->vertex_source, vert_src.c_str(), shader->vertex_length + 1);
  shader->vertex_source[shader->vertex_length] = '\0';
  
  return vert_src.size();
}

static const nikola::sizei compile_pixel_shader(const nikola::sizei start, const nikola::String& src, nikola::NBRShader* shader) {
  nikola::sizei begin     = src.find_first_of('#', start); 
  nikola::String frag_src = src.substr(begin);
  
  shader->pixel_length = (nikola::u16)frag_src.size();
  shader->pixel_source = (nikola::i8*)nikola::memory_allocate(shader->pixel_length + 1);

  nikola::memory_copy(shader->pixel_source, frag_src.c_str(), shader->pixel_length + 1);
  shader->pixel_source[shader->pixel_length] = '\0';

  return frag_src.size();
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Shader loader functions

bool shader_loader_load(nikola::NBRShader* shader, const nikola::FilePath& path) {
  nikola::File file;
  if(!nikola::file_open(&file, path, (nikola::i32)nikola::FILE_OPEN_READ)) {
    return false;
  }
  
  shader->vertex_length  = 0;
  shader->pixel_length   = 0;
  shader->compute_length = 0;

  // Read the string from the file
  nikola::String shader_src; 
  nikola::file_read_string(file, &shader_src);
  nikola::file_close(file);

  // Trying to seperate the shader into two
  // @FIX: All of this is temporary until we can make our 
  // own shader language to avoid this bullshit.

  nikola::sizei iden_pos    = shader_src.find_first_of('@');
  nikola::sizei current_pos = 0;

  switch(shader_src[iden_pos + 1]) {
    case 'c':
      return compile_compute_shader(0, shader_src, shader);
    case 'v':
      current_pos = compile_vertex_shader(0, shader_src, shader);
      compile_pixel_shader(current_pos, shader_src, shader);
      break;
    default: 
      NIKOLA_LOG_ERROR("NBR: Could not find shader identifiers in shader at \'%s\'", path.c_str());
      break;
  }

  return true;
}

void shader_loader_unload(nikola::NBRShader& shader) {
  if(shader.compute_length > 0) {
    nikola::memory_free(shader.compute_source);
    return;
  }

  nikola::memory_free(shader.vertex_source);
  nikola::memory_free(shader.pixel_source);
}

/// Shader loader functions
/// ----------------------------------------------------------------------

} // End of nbr

//////////////////////////////////////////////////////////////////////////
