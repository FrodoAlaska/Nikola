#include "nbr.hpp"

#include <nikola/nikola_engine.hpp>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nbr

/// ----------------------------------------------------------------------
/// Consts

const int VALID_OPTIONS_MAX = 4;

/// Consts
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static void argv_to_args(nikola::Args* args, int argc, char** argv) {
  for(int i = 0; i < argc; i++) {
    args->push_back(argv[i]);
  }
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Lexer functions

bool lexer_init(nikola::DynamicArray<ArgToken>* tokens, int argc, char** argv) {
  nikola::Args args;
  argv_to_args(&args, argc, argv);

  ArgToken valid_options[VALID_OPTIONS_MAX] = {
    {ARG_TOKEN_RESOURCE_TYPE, "--resource-type", "-rt"}, 
    {ARG_TOKEN_DIRECTORY,     "--dir",           "-d"}, 
    {ARG_TOKEN_RECURSE,       "--recurse",       "-r"},
    {ARG_TOKEN_HELP,          "--help",          "-h"},
  };

  for(int i = 1; i < args.size(); i++) {
    // Skip whitespace
    if(args[i] == " ") {
      continue;
    }

    bool is_option = false;
    for(int j = 0; j < VALID_OPTIONS_MAX; j++) {
      // Add the option to the tokens to be parsed later
      if((args[i] == valid_options[j].arg) || (args[i] == valid_options[j].alt_arg)) {
        tokens->push_back(valid_options[j]);
        is_option = true;

        break;
      }
    }

    if(!is_option) {
      // Otherwise, it is a parametar
      tokens->push_back(ArgToken{ARG_TOKEN_PARAM, args[i]});
    }
  }

  // End of the arguments
  tokens->push_back(ArgToken{ARG_TOKEN_EOF, ""});
  return true;
}

/// Lexer functions
/// ----------------------------------------------------------------------

/// *** Lexer ***
/// ---------------------------------------------------------------------------------------------------------

} // End of nbr

//////////////////////////////////////////////////////////////////////////
