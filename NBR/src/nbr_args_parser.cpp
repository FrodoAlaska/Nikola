#include "nbr.hpp"

#include <nikola/nikola_engine.hpp>

#include <cstdlib>
#include <cstdio>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nbr

/// ----------------------------------------------------------------------
/// Consts

const int RESOURCE_TYPES_MAX = 6;

/// Consts
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Macros
#define RAISE_ERROR(msg, ...) {NIKOLA_LOG_ERROR(msg, ##__VA_ARGS__); exit(-1);}
/// Macros
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ArgParser
struct ArgParser {
  nikola::DynamicArray<ArgToken> tokens;
  int current_token = 0;

  bool is_directory = false;
  bool can_recurse  = false;

  nikola::ResourceType current_res_type;

  nikola::DynamicArray<nikola::FilePath> src_paths;
  nikola::FilePath resource_dir, nbr_output_dir; 
};

static ArgParser s_parser = {};
/// ArgParser
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks 

static void directory_iterate_func(const nikola::FilePath& base_dir, nikola::FilePath current_path, void* user_data) {
  s_parser.src_paths.push_back(current_path);
}

/// Callbacks 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static void show_help() {
  printf("\n\n### Welcome to NBR ### \n\n");
  printf("NBR (Nikola Binary Resource) is a tool to convert any\nresources to the NBR format used by the Nikola engine\n\n");
  
  printf("[Usage]: nbr [--resource-type -rt] [--dir -d] [--recurse -r] <src_path> <dest_dir>\n\n");
  printf("  --resource-type, -rt = Specify the resource type you wish to convert\n");
  printf("  --directory, -d      = Will treat the given src_path as a directory\n");
  printf("  --recurse, -r        = Recursively go through all of the resources in src_path\n");
}

static bool is_eof() {
  return s_parser.tokens[s_parser.current_token].type == ARG_TOKEN_EOF; 
}

static ArgToken token_previous() {
  if(s_parser.current_token == 0) {
    return s_parser.tokens[0];
  }

  return s_parser.tokens[s_parser.current_token - 1];
}

static ArgToken token_next() {
  if((s_parser.current_token + 1) > s_parser.tokens.size()) {
    return s_parser.tokens[s_parser.current_token];
  }

  return s_parser.tokens[s_parser.current_token + 1];
}

static ArgToken token_consume() {
  if(is_eof()) {
    return s_parser.tokens[s_parser.current_token];
  }

  s_parser.current_token++;
  return s_parser.tokens[s_parser.current_token - 1];
}

static ArgToken token_current() {
  return s_parser.tokens[s_parser.current_token];
}

static nikola::ResourceType get_resource_type(const nikola::String& res_type) {
  if(res_type == "TEXTURE") {
    return nikola::RESOURCE_TYPE_TEXTURE;
  } 
  else if(res_type == "CUBEMAP") {
    return nikola::RESOURCE_TYPE_CUBEMAP;
  } 
  else if(res_type == "SHADER") {
    return nikola::RESOURCE_TYPE_SHADER;
  } 
  else if(res_type == "MODEL") {
    return nikola::RESOURCE_TYPE_MODEL;
  } 
  else if(res_type == "FONT") {
    return nikola::RESOURCE_TYPE_FONT;
  }

  RAISE_ERROR("NBR: Unkown resource type given \'%s\'", res_type.c_str());
}

static void check_resource_literal() {
  ArgToken param = token_consume();

  // The next token should be the param
  if(param.type != ARG_TOKEN_PARAM) {
    RAISE_ERROR("NBR: Expected an argument passed after \'%s\'", param.arg.c_str());
  }

  // Convert the argument into a valid resource type enum
  s_parser.current_res_type = get_resource_type(param.arg);
}

static void check_final_path() {
  s_parser.resource_dir = token_previous().arg;
  ArgToken next_token   = token_consume();

  // Set a default output as the current directory
  s_parser.nbr_output_dir = nikola::filesystem_current_path();

  // The optional output param has been set
  if(next_token.type != ARG_TOKEN_EOF) {
    s_parser.nbr_output_dir = next_token.arg;
  }

  // Recursively go through the directories and add the paths for later
  if(s_parser.can_recurse && s_parser.is_directory) {
    nikola::filesystem_directory_recurse_iterate(s_parser.resource_dir, directory_iterate_func);
  }
  // Just go over one directory
  else if(s_parser.is_directory) {
    nikola::filesystem_directory_iterate(s_parser.resource_dir, directory_iterate_func);
  }
  // It is just a single file otherwise
  else {
    s_parser.src_paths.push_back(s_parser.resource_dir);
  }
}

static void create_nbr_texture() {
  nikola::NBRTexture texture;
  nikola::NBRFile nbr;

  for(auto& path : s_parser.src_paths) {
    // Construct the final path
    nikola::FilePath final_path = nikola::filepath_append(s_parser.nbr_output_dir, nikola::filepath_filename(path)); 
    nikola::filepath_set_extension(final_path, "nbr");
  
    // Load the texture
    bool loaded = image_loader_load_texture(&texture, path);
    if(!loaded) {
      NIKOLA_LOG_ERROR("NBR: Failed to load resource at \'%s\'", path.c_str());
      continue;
    }

    // Save the texture
    nikola::nbr_file_save(nbr, texture, final_path);

    image_loader_unload_texture(texture);
    NIKOLA_LOG_INFO("NBR: Converted texture \'%s\' to \'%s\'...", path.c_str(), final_path.c_str());
  }
}

static void create_nbr_cubemap() {
  nikola::NBRCubemap cubemap;
  nikola::NBRFile nbr;
  
  for(auto& path : s_parser.src_paths) {
    // Construct the final path
    nikola::FilePath final_path = nikola::filepath_append(s_parser.nbr_output_dir, nikola::filepath_filename(path)); 
    nikola::filepath_set_extension(final_path, "nbr");

    // Load the cubemap
    bool loaded = image_loader_load_cubemap(&cubemap, path);
    if(!loaded) {
      NIKOLA_LOG_ERROR("NBR: Failed to load resource at \'%s\'", path.c_str());
      continue;
    }
  
    // Save the cubemap
    nikola::nbr_file_save(nbr, cubemap, final_path);

    image_loader_unload_cubemap(cubemap);
    NIKOLA_LOG_INFO("NBR: Converted cubemap \'%s\' to \'%s\'...", path.c_str(), final_path.c_str());
  }
}

static void create_nbr_shader() {
  nikola::NBRShader shader;
  nikola::NBRFile nbr;
  
  for(auto& path : s_parser.src_paths) {
    // Construct the final path
    nikola::FilePath final_path = nikola::filepath_append(s_parser.nbr_output_dir, nikola::filepath_filename(path)); 
    nikola::filepath_set_extension(final_path, "nbr");

    // Load the shader
    bool loaded = shader_loader_load(&shader, path);
    if(!loaded) {
      NIKOLA_LOG_ERROR("NBR: Failed to load resource at \'%s\'", path.c_str());
      continue;
    }

    // Save the shader
    nikola::nbr_file_save(nbr, shader, final_path);
  
    shader_loader_unload(shader);
    NIKOLA_LOG_INFO("NBR: Converted shader \'%s\' to \'%s\'...", path.c_str(), final_path.c_str());
  }
}

static void create_nbr_model() {
  nikola::NBRModel model;
  nikola::NBRFile nbr;
  
  for(auto& path : s_parser.src_paths) {
    // Construct the final path
    nikola::FilePath final_path = nikola::filepath_append(s_parser.nbr_output_dir, nikola::filepath_filename(path)); 
    nikola::filepath_set_extension(final_path, "nbr");

    // Load the model
    bool loaded = model_loader_load(&model, path);
    if(!loaded) {
      NIKOLA_LOG_ERROR("NBR: Failed to load resource at \'%s\'", path.c_str());
      continue;
    }

    // Save model
    nikola::nbr_file_save(nbr, model, final_path);

    model_loader_unload(model);
    NIKOLA_LOG_INFO("NBR: Converted model \'%s\' to \'%s\'...", path.c_str(), final_path.c_str());
  }
}

static void create_nbr_file() {
  switch(s_parser.current_res_type) {
    case nikola::RESOURCE_TYPE_TEXTURE:
      create_nbr_texture();
      break;
    case nikola::RESOURCE_TYPE_CUBEMAP:
      create_nbr_cubemap();
      break;
    case nikola::RESOURCE_TYPE_SHADER:
      create_nbr_shader();
      break;
    case nikola::RESOURCE_TYPE_MODEL:
      create_nbr_model();
      break;
    case nikola::RESOURCE_TYPE_FONT:
      break;
  }
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Parser functions

bool parser_init(const nikola::DynamicArray<ArgToken>& tokens) {
  s_parser.tokens = tokens;

  // No arguments passed
  if(s_parser.tokens[0].type == ARG_TOKEN_EOF) {
    printf("[NBR-ERROR]: No arguments passed\n");
    show_help();
    return false;
  }
  
  while(!is_eof()) {
    ArgToken current_token = token_consume();

    switch(current_token.type) {
      case ARG_TOKEN_DIRECTORY:
        s_parser.is_directory = true;
        continue;
      case ARG_TOKEN_RECURSE:
        s_parser.can_recurse = true;
        continue;
      case ARG_TOKEN_RESOURCE_TYPE: 
        check_resource_literal();
        continue;
      case ARG_TOKEN_HELP:
        show_help(); 
        return false;
      case ARG_TOKEN_PARAM:
        check_final_path();
        continue;
      case ARG_TOKEN_EOF:
        break;
    }
  }
  
  create_nbr_file();
  return true;
}

/// Parser functions
/// ----------------------------------------------------------------------

} // End of nbr

//////////////////////////////////////////////////////////////////////////
