#include "nbr.h"

#include <nikola/nikola.h>
#include <nbr_pch.h>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nbr

/// ---------------------------------------------------------------------------------------------------------
/// *** List *** 

/// ----------------------------------------------------------------------
/// ConvertEntry
struct ConvertEntry {
  nikola::FilePath in_path;  // The input path from an entry from the `.nbrlist` file
  nikola::FilePath out_path; // ListSection.out_dir

  nikola::ResourceType res_type;
};

static nikola::DynamicArray<ConvertEntry> s_entries;
/// ConvertEntry
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

static void iterate_resources(const nikola::FilePath& base_dir, const nikola::FilePath& current_path, void* user_data) {
  ListSection* section = (ListSection*)user_data;

  ConvertEntry entry = {
    .in_path  = current_path, 
    .out_path = section->out_dir,
    .res_type = section->type,
  };
  s_entries.push_back(entry);
}

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static bool check_section_dirs(const ListSection& section) {
  if(!nikola::filesystem_exists(section.local_dir)) {
    NIKOLA_LOG_ERROR("Invalid section local directory \'%s\'", section.local_dir.c_str());
    return false;
  }
  
  // Create the output directory if it doesn't exist
  //
  // @NOTE (20/5/2025, Mohamed): The `filesystem_create_directories` function returns `true` 
  // if the directories were successfully created. Otherwise, it returns 
  // `false` if the directories already exist. 
  //
  // We check here if the function returns true (meaning, the directories did not exist and need to be created) 
  // and give the user a warning just to inform them. Comlpex, I know. Sorry about that.
  
  if(nikola::filesystem_create_directories(section.out_dir)) {
    NIKOLA_LOG_WARN("Invalid section output directory \'%s\'. Creating new directories.", section.out_dir.c_str());
  }

  return true;
}

static bool open_nbr_file(nikola::FilePath& path, nikola::File* file, const nikola::ResourceType& type) {
  nikola::filepath_set_extension(path, "nbr");

  if(!nikola::file_open(file, path, (int)(nikola::FILE_OPEN_WRITE | nikola::FILE_OPEN_BINARY))) {
    NIKOLA_LOG_ERROR("Failed to open NBR file at '\%s\'", path.c_str());
    return false;
  }

  nikola::NBRHeader header = {
    .identifier    = nikola::NBR_VALID_IDENTIFIER, 
    .major_version = nikola::NBR_VALID_MAJOR_VERSION, 
    .minor_version = nikola::NBR_VALID_MINOR_VERSION, 
    .resource_type = (nikola::u16)type,
  };
  nikola::file_write_bytes(*file, header);
}

static bool convert_texture(const ConvertEntry& entry) {
  nikola::NBRTexture texture; 
  if(!image_loader_load_texture(&texture, entry.in_path)) {
    return false;
  }

  // Save the texture

  nikola::File file;
  nikola::FilePath path = nikola::filepath_append(entry.out_path, nikola::filepath_stem(entry.in_path));
  if(!open_nbr_file(path, &file, entry.res_type)) {
    return false;
  }
  nikola::file_write_bytes(file, texture);

  // Unload the image data
  image_loader_unload_texture(texture);
 
  NIKOLA_LOG_INFO("[NBR]: Converted texture \'%s\' to \'%s\'...", entry.in_path.c_str(), path.c_str());
  nikola::file_close(file);
  
  return true;
}

static bool convert_cubemap(const ConvertEntry& entry) {
  nikola::NBRCubemap cubemap; 
  if(!image_loader_load_cubemap(&cubemap, entry.in_path)) {
    return false;
  }
   
  // Save the cubemap

  nikola::File file;
  nikola::FilePath path = nikola::filepath_append(entry.out_path, nikola::filepath_stem(entry.in_path));
  if(!open_nbr_file(path, &file, entry.res_type)) {
    return false;
  }
  nikola::file_write_bytes(file, cubemap);

  // Unload the image
  image_loader_unload_cubemap(cubemap);
  
  NIKOLA_LOG_INFO("[NBR]: Converted cubemap \'%s\' to \'%s\'...", entry.in_path.c_str(), path.c_str());
  nikola::file_close(file);
  
  return true;
}

static bool convert_shader(const ConvertEntry& entry) {
  nikola::NBRShader shader; 
  if(!shader_loader_load(&shader, entry.in_path)) {
    return false;
  }

  // Save the shader

  nikola::File file;
  nikola::FilePath path = nikola::filepath_append(entry.out_path, nikola::filepath_stem(entry.in_path));
  if(!open_nbr_file(path, &file, entry.res_type)) {
    return false;
  }
  nikola::file_write_bytes(file, shader);

  // Unload the shader
  shader_loader_unload(shader);
  
  NIKOLA_LOG_INFO("[NBR]: Converted shader \'%s\' to \'%s\'...", entry.in_path.c_str(), path.c_str());
  nikola::file_close(file);
  
  return true;
}

static bool convert_model(const ConvertEntry& entry) {
  nikola::NBRModel model; 
  if(!model_loader_load(&model, entry.in_path)) {
    return false;
  }

  // Save the model
  
  nikola::File file;
  nikola::FilePath path = nikola::filepath_append(entry.out_path, 
                                                  nikola::filepath_stem(nikola::filepath_parent_path(entry.in_path)));
  if(!open_nbr_file(path, &file, entry.res_type)) {
    return false;
  }
  nikola::file_write_bytes(file, model);

  // Unload the model
  model_loader_unload(model);
  
  NIKOLA_LOG_INFO("[NBR]: Converted model \'%s\' to \'%s\'...", entry.in_path.c_str(), path.c_str());
  nikola::file_close(file);
  
  return true;
}

static bool convert_animation(const ConvertEntry& entry) {
  nikola::NBRAnimation anim; 
  if(!animation_loader_load(&anim, entry.in_path)) {
    return false;
  }

  // Save the animation
  
  nikola::File file;
  nikola::FilePath path = nikola::filepath_append(entry.out_path, 
                                                  nikola::filepath_stem(nikola::filepath_parent_path(entry.in_path)));
  if(!open_nbr_file(path, &file, entry.res_type)) {
    return false;
  }
  nikola::file_write_bytes(file, anim);

  // Unload the animation
  animation_loader_unload(anim);
  
  NIKOLA_LOG_INFO("[NBR]: Converted animation \'%s\' to \'%s\'...", entry.in_path.c_str(), path.c_str());
  nikola::file_close(file);
  
  return true;
}

static bool convert_font(const ConvertEntry& entry) {
  nikola::NBRFont font; 
  if(!font_loader_load(&font, entry.in_path)) {
    return false;
  }

  // Save the font
  
  nikola::File file;
  nikola::FilePath path = nikola::filepath_append(entry.out_path, nikola::filepath_stem(entry.in_path));
  if(!open_nbr_file(path, &file, entry.res_type)) {
    return false;
  }
  nikola::file_write_bytes(file, font);

  // Unload the font
  font_loader_unload(font);
  
  NIKOLA_LOG_INFO("[NBR]: Converted font \'%s\' to \'%s\'...", entry.in_path.c_str(), path.c_str());
  nikola::file_close(file);
  
  return true;
}

static bool convert_audio(const ConvertEntry& entry) {
  nikola::NBRAudio audio; 
  if(!audio_loader_load(&audio, entry.in_path)) {
    return false;
  }

  // Save the audio buffer
  
  nikola::File file;
  nikola::FilePath path = nikola::filepath_append(entry.out_path, nikola::filepath_stem(entry.in_path));
  if(!open_nbr_file(path, &file, entry.res_type)) {
    return false;
  }
  nikola::file_write_bytes(file, audio);

  // Unload the audio buffer
  audio_loader_unload(audio);
  
  NIKOLA_LOG_INFO("[NBR]: Converted audio \'%s\' to \'%s\'...", entry.in_path.c_str(), path.c_str());
  nikola::file_close(file);
  
  return true;
}

static bool convert_by_type(const ConvertEntry& entry) {
  switch(entry.res_type) {
    case nikola::RESOURCE_TYPE_TEXTURE:
      convert_texture(entry);
      break;
    case nikola::RESOURCE_TYPE_CUBEMAP:
      convert_cubemap(entry);
      break;
    case nikola::RESOURCE_TYPE_SHADER:
      convert_shader(entry);
      break;
    case nikola::RESOURCE_TYPE_MODEL:
      convert_model(entry);
      break;
    case nikola::RESOURCE_TYPE_ANIMATION:
      convert_animation(entry);
      break;
    case nikola::RESOURCE_TYPE_FONT:
      convert_font(entry);
      break;
    case nikola::RESOURCE_TYPE_AUDIO_BUFFER:
      convert_audio(entry);
      break;
    default:
      NIKOLA_LOG_ERROR("An unsupported resource type found!");
      return false;
  }

  return true;
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// List context functions 

void list_context_create(ListContext* list, const nikola::FilePath& path) {
  // Lex
  
  nikola::DynamicArray<ListToken> tokens;
  if(!list_lexer_init(path, &tokens)) {
    return;
  }

  // Parse
  
  if(!list_parser_init(tokens, list)) {
    return;
  }

  for(auto& section : list->sections) {
    // Check if all the paths are correct
    if(!check_section_dirs(section)) {
      continue;
    }

    // Retrieve all the paths that need to be converted 
    // in the second pass.

    for(auto& res : section.resources) {
      if(nikola::filepath_is_dir(res)) {
        nikola::filesystem_directory_iterate(res, iterate_resources, &section);
        continue;
      }

      ConvertEntry entry = {
        .in_path  = res, 
        .out_path = section.out_dir,
        .res_type = section.type,
      };
      s_entries.push_back(entry);
    }
  }
}

void list_context_convert_by_type(const ListContext& list, const nikola::ResourceType type, nikola::ThreadPool& pool) {
  NIKOLA_PROFILE_FUNCTION();
  
  for(auto& entry : s_entries) {
    if(entry.res_type != type) {
      continue;
    }

    nikola::thread_pool_push_task(pool, [&]() {
      convert_by_type(entry);
    });
  }
}

void list_context_convert_all(const ListContext& list, nikola::ThreadPool& pool) {
  NIKOLA_PROFILE_FUNCTION();

  // Convert all the resource paths

  for(auto& entry : s_entries) {
    nikola::thread_pool_push_task(pool, [&]() {
      convert_by_type(entry);
    });
  }
}

/// List context functions 
/// ----------------------------------------------------------------------

/// *** List *** 
/// ---------------------------------------------------------------------------------------------------------


} // End of nbr

//////////////////////////////////////////////////////////////////////////
