#include "nbr.hpp"

#include <nikola/nikola_core.hpp>

int main(int argc, char** argv) {
  nikola::DynamicArray<nbr::ArgToken> tokens; 

  // Lex the CLI arguments
  if(!nbr::lexer_init(&tokens, argc, argv)) {
    return -1;
  }

  // Parse the tokens and take action
  if(!nbr::parser_init(tokens)) {
    return -1;
  }

  return 0; 
}
