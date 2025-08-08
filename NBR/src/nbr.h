#pragma once

#include <nikola/nikola.h>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nbr

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

void shader_loader_unload(nikola::NBRShader& shader);

/// Shader loader functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Model loader functions

bool model_loader_load(nikola::NBRModel* model, const nikola::FilePath& path); 

void model_loader_unload(nikola::NBRModel& model); 

/// Model loader functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Animation loader functions

bool gltf_animation_loader_load(nikola::NBRAnimation* anim, const nikola::FilePath& path); 

bool animation_loader_load(nikola::NBRAnimation* anim, const nikola::FilePath& path); 

void animation_loader_unload(nikola::NBRAnimation& anim); 

/// Animation loader functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Font loader functions

bool font_loader_load(nikola::NBRFont* font, const nikola::FilePath& path);

void font_loader_unload(nikola::NBRFont& font);

/// Font loader functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Audio loader functions

bool audio_loader_load(nikola::NBRAudio* audio, const nikola::FilePath& path);

void audio_loader_unload(nikola::NBRAudio& audio);

/// Audio loader functions
/// ----------------------------------------------------------------------

/// *** Loaders ***
/// ---------------------------------------------------------------------------------------------------------

/// ---------------------------------------------------------------------------------------------------------
/// *** List *** 

/// ----------------------------------------------------------------------
/// ListSection
struct ListSection {
  nikola::ResourceType type;
  nikola::FilePath out_dir, local_dir;

  nikola::DynamicArray<nikola::FilePath> resources;
};
/// ListSection
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ListContext 
struct ListContext {
  nikola::DynamicArray<ListSection> sections; 

  nikola::FilePath parent_dir; 
  nikola::FilePath bin_dir;
};
/// ListContext 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// List context functions 

void list_context_create(const nikola::FilePath& path, ListContext* list);

void list_context_convert_by_type(ListContext* list, const nikola::ResourceType type);

void list_context_convert_all(ListContext* list);

/// List context functions 
/// ----------------------------------------------------------------------

/// *** List *** 
/// ---------------------------------------------------------------------------------------------------------

/// ---------------------------------------------------------------------------------------------------------
/// *** Lexer ***

/// ----------------------------------------------------------------------
/// ListTokenType
enum ListTokenType {
  LIST_TOKEN_SECTION, 
  LIST_TOKEN_PARAM,
  LIST_TOKEN_LOCAL,
  LIST_TOKEN_COMMENT,
  LIST_TOKEN_STRING_LITERAL,
  LIST_TOKEN_EOF,
};
/// ListTokenType
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ListToken 
struct ListToken {
  ListTokenType type; 
  nikola::String literal;
  nikola::i32 line;
};
/// ListToken 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// List lexer functions

bool list_lexer_init(const nikola::FilePath& list_path, nikola::DynamicArray<ListToken>* tokens);

/// List lexer functions
/// ----------------------------------------------------------------------

/// *** Lexer ***
/// ---------------------------------------------------------------------------------------------------------

/// ---------------------------------------------------------------------------------------------------------
/// *** Parser ***

/// ----------------------------------------------------------------------
/// List parser functions

bool list_parser_init(const nikola::DynamicArray<ListToken>& tokens, ListContext* list);

/// List parser functions
/// ----------------------------------------------------------------------

/// *** Parser ***
/// ---------------------------------------------------------------------------------------------------------

} // End of nbr

//////////////////////////////////////////////////////////////////////////
