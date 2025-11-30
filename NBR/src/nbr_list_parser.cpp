#include "nbr.h"

#include <nikola/nikola.h>

#include <cstdlib>
#include <cstdio>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nbr

/// ---------------------------------------------------------------------------------------------------------
/// *** Parser ***

/// ----------------------------------------------------------------------
/// Parser
struct Parser {
  nikola::DynamicArray<ListToken> tokens;
  nikola::i32 current;

  ListContext* list;
};

static Parser s_parser;
/// Parser
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static bool is_eof() {
  return s_parser.tokens[s_parser.current].type == LIST_TOKEN_EOF;
}

static ListToken& token_consume() {
  if(is_eof()) {
    return s_parser.tokens[s_parser.current];
  }

  s_parser.current++;
  return s_parser.tokens[s_parser.current - 1];
}

static ListToken& token_peek_prev() {
  if(s_parser.current == 0) {
    return s_parser.tokens[0];
  }

  return s_parser.tokens[s_parser.current - 2];
}

static ListToken& token_peek_next() {
  if(is_eof()) {
    return s_parser.tokens[s_parser.current];
  }

  return s_parser.tokens[s_parser.current];
}

static nikola::ResourceType get_type_from_section(const nikola::String& section) {
  if(section == "TEXTURE" || section == "texture") {
    return nikola::RESOURCE_TYPE_TEXTURE;
  }
  else if(section == "CUBEMAP" || section == "cubemap") {
    return nikola::RESOURCE_TYPE_CUBEMAP;
  }
  else if(section == "SHADER" || section == "shader") {
    return nikola::RESOURCE_TYPE_SHADER;
  }
  else if(section == "MODEL" || section == "model") {
    return nikola::RESOURCE_TYPE_MODEL;
  }
  else if(section == "SKELETON" || section == "skeleton") {
    return nikola::RESOURCE_TYPE_SKELETON;
  }
  else if(section == "ANIMATION" || section == "animation") {
    return nikola::RESOURCE_TYPE_ANIMATION;
  }
  else if(section == "FONT" || section == "font") {
    return nikola::RESOURCE_TYPE_FONT;
  }
  else if(section == "AUDIO" || section == "audio") {
    return nikola::RESOURCE_TYPE_AUDIO_BUFFER;
  }

  NIKOLA_LOG_ERROR("Invalid resource type \'%s\'", section.c_str());
  return (nikola::ResourceType)-1;
}

static void assign_section(ListContext* list, ListSection* section) {
  // Safety check
  if(!section) {
    NIKOLA_LOG_ERROR("Unassigned section found");
    return;
  }

  if(token_peek_next().type != LIST_TOKEN_STRING_LITERAL) {
    NIKOLA_LOG_ERROR("Section declared without a name");
    return;
  }

  // Initialize a new section
  section->out_dir   = list->bin_dir; 
  section->local_dir = list->parent_dir;

  // Assign the type of the new section
  section->type = get_type_from_section(token_consume().literal);
}

static void assign_param(ListSection* section) {
  // Safety check
  if(!section) {
    NIKOLA_LOG_ERROR("Unassigned section found");
    return;
  }

  // Check for the literal
  if(token_peek_next().type != LIST_TOKEN_STRING_LITERAL) {
    NIKOLA_LOG_ERROR("Parametar declared without an identifier");
    return;
  } 

  // Consume and use the out parametar
  section->out_dir = nikola::filepath_append(section->out_dir, token_consume().literal); 
}

static void assign_local(ListSection* section) {
  // Safety check
  if(!section) {
    NIKOLA_LOG_ERROR("Unassigned section found");
    return;
  }

  // Check for the literal
  if(token_peek_next().type != LIST_TOKEN_STRING_LITERAL) {
    NIKOLA_LOG_ERROR("Local variable declared without an identifier");
    return;
  } 
  
  // Consume and use the local variable
  section->local_dir = nikola::filepath_append(section->local_dir, token_consume().literal); 
}

static void assign_path(ListSection* section, const ListToken& current_token) {
  // Safety check
  if(!section) {
    NIKOLA_LOG_ERROR("Unassigned section found");
    return;
  }
   
  section->resources.push_back(nikola::filepath_append(section->local_dir, current_token.literal)); 
}

static bool parser_start(ListContext* list) {
  ListSection* section = nullptr;

  while(!is_eof()) {
    ListToken token = token_consume(); 

    switch(token.type) {
      case LIST_TOKEN_SECTION: 
        // Add a new section to the context
        list->sections.push_back(ListSection{});
        section = &list->sections[list->sections.size() - 1];
       
        assign_section(list, section);
        break;
      case LIST_TOKEN_PARAM:
        assign_param(section);
        break;
      case LIST_TOKEN_LOCAL:
        assign_local(section);
        break;
      case LIST_TOKEN_STRING_LITERAL:
        assign_path(section, token);
        break;
      case LIST_TOKEN_COMMENT:
      case LIST_TOKEN_EOF:
        break;
      default:
        NIKOLA_LOG_ERROR("Invalid token found");
        return false;
    }
  }

  return true;
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// List parser functions

bool list_parser_init(const nikola::DynamicArray<ListToken>& tokens, ListContext* list) {
  NIKOLA_PROFILE_FUNCTION();

  if(tokens.empty()) {
    NIKOLA_LOG_ERROR("Empty tokens array given to parser!");
    return false;
  }

  s_parser         = {};
  s_parser.current = 0; 
  s_parser.tokens  = tokens;
  s_parser.list    = list;

  return parser_start(list);
}

/// List parser functions
/// ----------------------------------------------------------------------

/// *** Parser ***
/// ---------------------------------------------------------------------------------------------------------

} // End of nbr

//////////////////////////////////////////////////////////////////////////
