#pragma once

#include <nikola/nikola_engine.hpp>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nbr

/// ---------------------------------------------------------------------------------------------------------
/// *** Lexer ***

/// ----------------------------------------------------------------------
/// ArgTokenType
enum ArgTokenType {
  ARG_TOKEN_RESOURCE_TYPE = 0, 
  ARG_TOKEN_DIRECTORY,
  ARG_TOKEN_RECURSE, 
  ARG_TOKEN_HELP, 
  ARG_TOKEN_PARAM,
  ARG_TOKEN_EOF,
};
/// ArgTokenType
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ArgToken
struct ArgToken {
  ArgTokenType type; 
  nikola::String arg, alt_arg;
};
/// ArgToken
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Lexer functions

bool lexer_init(nikola::DynamicArray<ArgToken>* tokens, int argc, char** argv);

/// Lexer functions
/// ----------------------------------------------------------------------

/// *** Lexer ***
/// ---------------------------------------------------------------------------------------------------------

/// ---------------------------------------------------------------------------------------------------------
/// *** Parser ***

/// ----------------------------------------------------------------------
/// Parser functions

bool parser_init(const nikola::DynamicArray<ArgToken>& tokens);

/// Parser functions
/// ----------------------------------------------------------------------

/// *** Parser ***
/// ---------------------------------------------------------------------------------------------------------

/// ---------------------------------------------------------------------------------------------------------
/// *** Loaders ***

/// ----------------------------------------------------------------------
/// Image loader functions

bool image_loader_load_texture(nikola::NBRTexture* texture, const nikola::FilePath& path);

bool image_loader_load_cubemap(nikola::NBRCubemap* cube, const nikola::FilePath& dir);

void image_loader_unload_texture(nikola::NBRTexture& texture);

void image_loader_unload_cubemap(nikola::NBRCubemap& cubemap);

/// Image loader functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Shader loader functions

bool shader_loader_load(nikola::NBRShader* shader, const nikola::FilePath& path);

/// Shader loader functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Model loader functions

bool model_loader_load(nikola::NBRModel* model, const nikola::FilePath& path); 

void model_loader_unload(nikola::NBRModel& model); 

/// Model loader functions
/// ----------------------------------------------------------------------

/// *** Loaders ***
/// ---------------------------------------------------------------------------------------------------------

} // End of nbr

//////////////////////////////////////////////////////////////////////////
