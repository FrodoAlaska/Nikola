#include "nbr.h"

#include <nikola/nikola.h>

#include <chrono>

/// ----------------------------------------------------------------------
/// Consts

const nikola::sizei OPTIONS_MAX = 4;

const nikola::i32 NBR_VERSION   = 4;  

/// Consts
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Macros

#define ARG_VERSION       "--version", "-v"
#define ARG_PARENT_DIR    "--parent-dir", "-pd"
#define ARG_BIN_DIR       "--bin-dir", "-bd"
#define ARG_RESOURCE_TYPE "--resource-type", "-rt"
#define ARG_HELP          "--help", "-h"

/// Macros
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static void show_help() {
  NIKOLA_LOG_INFO("<-------> Welcome to NBR Converter <------->");
  NIKOLA_LOG_INFO("Usage: nbr [--version -v] [--parent-dir, -pd] [--bin-dir, -bd], [--resource-type, -rt] <path/to/list.nbrlist>");
  NIKOLA_LOG_INFO("   --version       = Get the current version of the NBR tool.");
  NIKOLA_LOG_INFO("   --parent-dir    = The directory where all the input resources live.");
  NIKOLA_LOG_INFO("   --bin-dir       = The directory where all the output resources will be placed.");
  NIKOLA_LOG_INFO("   --resource-type = Specify a certain resource type to convert. If omitted, resources of all types will be converted.");
  NIKOLA_LOG_INFO("   --help          = Show this help message.");
}

static bool check_arg(const char* arg, const nikola::String& opt, const nikola::String& alt_opt) {
  return ((opt == arg) || (alt_opt == arg));
}

static nikola::ResourceType get_resource_type(const char* type) {
  nikola::String str_type = type;

  // @TEMP: Yuck!

  if(str_type == "TEXTURE" || str_type == "texture") {
    return nikola::RESOURCE_TYPE_TEXTURE;
  }
  else if(str_type == "CUBEMAP" || str_type == "cubemap") {
    return nikola::RESOURCE_TYPE_CUBEMAP;
  }
  else if(str_type == "SHADER" || str_type == "shader") {
    return nikola::RESOURCE_TYPE_SHADER;
  }
  else if(str_type == "MODEL" || str_type == "model") {
    return nikola::RESOURCE_TYPE_MODEL;
  }
  else if(str_type == "ANIMATION" || str_type == "animation") {
    return nikola::RESOURCE_TYPE_ANIMATION;
  }
  else if(str_type == "FONT" || str_type == "font") {
    return nikola::RESOURCE_TYPE_FONT;
  }
  else if(str_type == "AUDIO" || str_type == "audio") {
    return nikola::RESOURCE_TYPE_AUDIO_BUFFER;
  }
  
  NIKOLA_LOG_ERROR("Invalid resource type given \'%s\'", type);
  return (nikola::ResourceType)-1;
}

static bool lex_args(int argc, char** argv, nbr::ListContext* list, nikola::i32* res_type) {
  NIKOLA_PROFILE_FUNCTION();

  nikola::FilePath path = "DI"; 

  for(int i = 1; i < 6; i++) {
    if(check_arg(argv[i], ARG_VERSION)) {
      NIKOLA_LOG_ERROR("[NBR]: Version = %i.%i", nikola::NBR_VALID_MAJOR_VERSION, nikola::NBR_VALID_MINOR_VERSION);
      return false;
    }
    else if(check_arg(argv[i], ARG_PARENT_DIR)) {
      list->parent_dir = argv[++i]; 
    }
    else if(check_arg(argv[i], ARG_BIN_DIR)) {
      list->bin_dir = argv[++i]; 
    }
    else if(check_arg(argv[i], ARG_RESOURCE_TYPE)) {
      *res_type = (nikola::i32)get_resource_type(argv[++i]);
    }
    else if(check_arg(argv[i], ARG_HELP)) {
      show_help();
      return false;
    }
    else {
      path = argv[i];
    }
  }

  // Some sanity checks
  if(path == "DI") {
    NIKOLA_LOG_ERROR("[NBR]: No path to .nbrlist file was given");
    return false;
  }

  // Create the context
  nbr::list_context_create(path, list);
  
  return true;
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Main function
int main(int argc, char** argv) {
  if(argc <= 1) {
    show_help();
    return -1;
  }

  // Job manager init (this is usually done by the engine, but oh well)
  nikola::job_manager_init(32); 
  
  // Setting default values

  nbr::ListContext list; 
  list.parent_dir = nikola::filesystem_current_path();
  list.bin_dir    = list.parent_dir;

  // Extract the command line arguments
 
  nikola::i32 resource_type = -1;
  if (!lex_args(argc, argv, &list, &resource_type)) {
    nikola::job_manager_shutdown();
    return -1;
  }

  // Actually convert the resources (if all goes well)

  nikola::PerfTimer timer;
  const char* func_name = "nbr::list_context_convert_all";
  NIKOLA_PERF_TIMER_BEGIN(timer);

  // Convert the resources
  if(resource_type == -1) {
    nbr::list_context_convert_all(&list); 
  } 
  else {
    nbr::list_context_convert_by_type(&list, (nikola::ResourceType)resource_type); 
    func_name = "nbr::list_context_convert_by_type";
  }

  nikola::job_manager_shutdown();
  NIKOLA_PERF_TIMER_END(timer, func_name);
  
  return 0;
}
/// Main function
/// ----------------------------------------------------------------------
