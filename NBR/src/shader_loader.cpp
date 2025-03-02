#include "nbr.hpp"

#include <nikola/nikola_core.hpp>
#include <nikola/nikola_engine.hpp>

#include <cstdio>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nikola

/// ----------------------------------------------------------------------
/// Shader loader functions

bool shader_loader_load(nikola::NBRShader* shader, const nikola::FilePath& path) {
  nikola::File file;
  if(!nikola::file_open(&file, path, (nikola::i32)std::ios::in)) {
    return false;
  }

  // Read the string from the file
  nikola::String shader_src = nikola::file_read_string(file);
  nikola::file_close(file);

  // Trying to seperate the shader into two
  // @FIX: All of this is temporary until we can make our 
  // own shader language to avoid this bullshit.

  // Identifying each shader by the `#version` 
  nikola::sizei vert_iden_pos = shader_src.find_first_of('#');
  nikola::sizei frag_iden_pos = shader_src.find_last_of('#');

  // Make sure the identifiers actually exist
  if(vert_iden_pos == shader_src.npos) {
    NIKOLA_LOG_ERROR("NBR: Could not find Vertex identifier in shader at \'%s\'", path.c_str());
  }

  if(frag_iden_pos == shader_src.npos) {
    NIKOLA_LOG_ERROR("NBR: Could not find Pixel identifier in shader at \'%s\'", path.c_str());
  }

  // Actually seperate the string
  nikola::String vert_src = shader_src.substr(vert_iden_pos, frag_iden_pos - 1);
  nikola::String frag_src = shader_src.substr(frag_iden_pos);

  // Setting the lengths
  shader->vertex_length = (nikola::u16)vert_src.size();
  shader->pixel_length  = (nikola::u16)frag_src.size();

  // Setting the vertex source strings
  shader->vertex_source = (nikola::i8*)nikola::memory_allocate(shader->vertex_length); 
  nikola::memory_copy(shader->vertex_source, vert_src.c_str(), shader->vertex_length);

  // Setting the pixel source strings
  shader->pixel_source = (nikola::i8*)nikola::memory_allocate(shader->pixel_length); 
  nikola::memory_copy(shader->pixel_source, frag_src.c_str(), shader->pixel_length); // Copy the string

  return true;
}

void shader_loader_unload(nikola::NBRShader& shader) {
  nikola::memory_free(shader.vertex_source);
  nikola::memory_free(shader.pixel_source);
}

/// Shader loader functions
/// ----------------------------------------------------------------------

} // End of nbr

//////////////////////////////////////////////////////////////////////////
