#include "nbr.h"

#include <nikola/nikola.h>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nbr

/// ----------------------------------------------------------------------
/// Lexer
struct Lexer {
  nikola::String source;
  nikola::FilePath path;
  
  nikola::i32 current = 0; 
  nikola::i32 start   = 0;  
  nikola::i32 lines   = 1;

  nikola::DynamicArray<ListToken>* tokens;
};

static Lexer s_lexer;
/// Lexer
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static void raise_error(const char* msg) {
  NIKOLA_LOG_ERROR("[NBR-LIST]: Error at %s (%i, %i)", s_lexer.path.c_str(), s_lexer.lines, s_lexer.current, msg);
}

/// Different utility checks to make sure everything is correct before loading the file
static bool list_sanity_check(const nikola::FilePath& list_path) {
  // Check for the extension  
  if(nikola::filepath_extension(list_path) != ".nbrlist") {
    NIKOLA_LOG_ERROR("[NBR-LIST]: Invalid extension of NBRList at \'%s\'", list_path.c_str());
    return false;
  }

  // Load the file if the path is correct
  nikola::File file; 
  if(!nikola::file_open(&file, list_path, (nikola::i32)(nikola::FILE_OPEN_READ))) {
    NIKOLA_LOG_ERROR("[NBR-LIST]: Failed to read NBRList file at \'%s\'", list_path.c_str());
    return false;
  }

  // Load the contents from the file
  s_lexer.path   = list_path; 
  s_lexer.source = nikola::file_read_string(file);

  nikola::file_close(file);
  return true;
}

static bool is_eof() {
  return s_lexer.current >= s_lexer.source.size();
}

static void token_push(const ListTokenType type, const nikola::String& lexeme) {
  s_lexer.tokens->push_back(ListToken{type, lexeme, s_lexer.lines});
}

static char char_consume() {
  s_lexer.current++; 
  return s_lexer.source[s_lexer.current - 1];
}

static char char_peek() {
  if(is_eof()) {
    return '\0';
  }

  return s_lexer.source[s_lexer.current];
}

static char char_peek_next() {
  if((s_lexer.current + 1) > s_lexer.source.size()) {
    return '\0';
  }

  return s_lexer.source[s_lexer.current + 1];
}

static bool char_is_alpha(const char ch) {
  return (ch >= 'a' && ch <= 'z')   || 
         (ch >= 'A' && ch <= 'Z')   || 
         (ch == '_') || (ch == '.') ||
         (ch == '/') || (ch == '\\');
}

static void section_iden() {
  char next = char_consume();
  if(next != ':') {
    raise_error("Incomplete section identifier");
    return;
  }

  token_push(LIST_TOKEN_SECTION, "");
}

static void comment_iden() {
  char ch = char_consume();
  while(ch != '\n') {
    ch = char_consume();
  }
  
  s_lexer.lines++;
  token_push(LIST_TOKEN_COMMENT, "");
}

static void string_lit() {
  char ch = char_consume();
  while(ch != '\n' && ch != ' ') {
    if(ch == '\n') {
      s_lexer.lines++;
    }

    ch = char_consume();
  }
  
  nikola::String lexeme = s_lexer.source.substr(s_lexer.start, (s_lexer.current - s_lexer.start) - 1);
  token_push(LIST_TOKEN_STRING_LITERAL, lexeme);
}

static bool lexer_start() {
  while(!is_eof()) {
    s_lexer.start = s_lexer.current;
    char ch       = char_consume();

    switch(ch) {
      case ':': 
        section_iden();
        break;
      case '$':
        token_push(LIST_TOKEN_LOCAL, "");
        break;
      case '@':
        token_push(LIST_TOKEN_PARAM, "");
        break;
      case '#':
        comment_iden();
        break;
      case ' ':
      case '\t':
      case '\r':
        break;
      case '\n':
        s_lexer.lines++;
        break;
      default:
        if(char_is_alpha(ch)) {
          string_lit();
        }
        else {
          NIKOLA_LOG_ERROR("%c", ch);
          raise_error("Unknown identifier found");
        }
        break;
    }
  }

  token_push(LIST_TOKEN_EOF, "");
  return true;
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// List lexer functions

bool list_lexer_init(const nikola::FilePath& list_path, nikola::DynamicArray<ListToken>* tokens) {
  s_lexer        = {}; 
  s_lexer.tokens = tokens;

  if(!list_sanity_check(list_path)) {
    return false;
  }

  return lexer_start();
}

/// List lexer functions
/// ----------------------------------------------------------------------

/// *** Lexer ***
/// ---------------------------------------------------------------------------------------------------------

} // End of nbr

//////////////////////////////////////////////////////////////////////////
