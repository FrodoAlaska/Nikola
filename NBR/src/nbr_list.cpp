#include "nbr.hpp"

#include <nikola/nikola_engine.hpp>

#include <cstdlib>
#include <cstdio>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nbr

/// ---------------------------------------------------------------------------------------------------------
/// *** List *** 

/// ----------------------------------------------------------------------
/// Private functions

static bool check_section_dirs(const ListSection& section) {
  if(!nikola::filesystem_exists(section.local_dir)) {
    NIKOLA_LOG_ERROR("Invalid section local directory \'%s\'", section.local_dir.c_str());
    return false;
  }
  else if(!nikola::filesystem_exists(section.out_dir)) {
    NIKOLA_LOG_ERROR("Invalid section output directory \'%s\'", section.out_dir.c_str());
    return false;
  }

  return true;
}

static bool convert_texture(ListSection& section, const nikola::FilePath& path) {
  nikola::NBRTexture texture; 
  nikola::NBRFile nbr; 

  if(!image_loader_load_texture(&texture, path)) {
    return false;
  }
   
  // Save the texture
  nikola::nbr_file_save(nbr, texture, nikola::filepath_append(section.out_dir, nikola::filepath_filename(path)));

  // Unload the image
  image_loader_unload_texture(texture);
  
  NIKOLA_LOG_INFO("[NBR]: Converted texture \'%s\' to \'%s\'...", path.c_str(), nbr.path.c_str());
  return true;
}

static bool convert_cubemap(ListSection& section, const nikola::FilePath& path) {
  nikola::NBRCubemap cubemap; 
  nikola::NBRFile nbr; 

  if(!image_loader_load_cubemap(&cubemap, path)) {
    return false;
  }
   
  // Save the cubemap
  nikola::nbr_file_save(nbr, cubemap, nikola::filepath_append(section.out_dir, nikola::filepath_filename(path)));

  // Unload the image
  image_loader_unload_cubemap(cubemap);
  
  NIKOLA_LOG_INFO("[NBR]: Converted cubemap \'%s\' to \'%s\'...", path.c_str(), nbr.path.c_str());
  return true;
}

static bool convert_shader(ListSection& section, const nikola::FilePath& path) {
  nikola::NBRShader shader; 
  nikola::NBRFile nbr; 

  if(!shader_loader_load(&shader, path)) {
    return false;
  }

  // Save the shader
  nikola::nbr_file_save(nbr, shader, nikola::filepath_append(section.out_dir, nikola::filepath_filename(path)));

  // Unload the shader
  shader_loader_unload(shader);
  
  NIKOLA_LOG_INFO("[NBR]: Converted shader \'%s\' to \'%s\'...", path.c_str(), nbr.path.c_str());
  return true;
}

static bool convert_model(ListSection& section, const nikola::FilePath& path) {
  nikola::NBRModel model; 
  nikola::NBRFile nbr; 

  if(!model_loader_load(&model, path)) {
    return false;
  }

  // Save the model
  nikola::nbr_file_save(nbr, model, nikola::filepath_append(section.out_dir, nikola::filepath_filename(path)));

  // Unload the model
  model_loader_unload(model);
  
  NIKOLA_LOG_INFO("[NBR]: Converted model \'%s\' to \'%s\'...", path.c_str(), nbr.path.c_str());
  return true;
}

static void convert_by_type(ListSection& section, const nikola::FilePath& path) {
  switch(section.type) {
    case nikola::RESOURCE_TYPE_TEXTURE:
      convert_texture(section, path);
      break;
    case nikola::RESOURCE_TYPE_CUBEMAP:
      convert_cubemap(section, path);
      break;
    case nikola::RESOURCE_TYPE_SHADER:
      convert_shader(section, path);
      break;
    case nikola::RESOURCE_TYPE_MODEL:
      convert_model(section, path);
      break;
  }
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

static void iterate_resources(const nikola::FilePath& base_dir, nikola::FilePath current_path, void* user_data) {
  ListSection* section = (ListSection*)user_data;
  convert_by_type(*section, current_path);
}

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// List context functions 

void list_context_create(const nikola::FilePath& path, ListContext* list) {
  // Lex
  nikola::DynamicArray<ListToken> tokens;
  if(!list_lexer_init(path, &tokens)) {
    return;
  }

  // Parse
  if(!list_parser_init(tokens, list)) {
    return;
  }
}

void list_context_convert_by_type(ListContext* list, const nikola::ResourceType type) {
  for(auto& section : list->sections) {
    if(section.type != type) {
      continue;
    }

    // Check if all the paths are correct
    if(!check_section_dirs(section)) {
      continue;
    }

    // Convert all the resource paths
    for(auto& res : section.resources) {
      if(nikola::filepath_is_dir(res)) {
        nikola::filesystem_directory_iterate(res, iterate_resources, &section);
        continue;
      }

      convert_by_type(section, res);
    }
  }
}

void list_context_convert_all(ListContext* list) {
  for(auto& section : list->sections) {
    // Check if all the paths are correct
    if(!check_section_dirs(section)) {
      continue;
    }

    // Convert all the resource paths
    for(auto& res : section.resources) {
      if(nikola::filepath_is_dir(res)) {
        nikola::filesystem_directory_iterate(res, iterate_resources, &section);
        continue;
      }

      convert_by_type(section, res);
    }
  }
}

/// List context functions 
/// ----------------------------------------------------------------------

/// *** List *** 
/// ---------------------------------------------------------------------------------------------------------


} // End of nbr

//////////////////////////////////////////////////////////////////////////
