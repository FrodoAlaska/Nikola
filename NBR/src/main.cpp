#include "nbr.hpp"

#include <nikola/nikola_core.hpp>

int main(int argc, char** argv) {
  nbr::ListContext list; 
  
  // Setting default values
  list.parent_dir = "..\\res";
  list.bin_dir    = "..\\build-debug\\testbed\\res";

  // Create the context
  nbr::list_context_create(argv[1], &list);

  // Convert the resources
  nbr::list_context_convert_all(&list); 
}
