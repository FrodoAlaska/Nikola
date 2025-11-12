#include "nikola/nikola_resources.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_gfx.h"
#include "nikola/nikola_render.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola {

///---------------------------------------------------------------------------------------------------------------------
/// NBR functions

bool nbr_file_is_valid(File& file, const FilePath& path, const ResourceType res_type) {
  NBRHeader header;

  // Check for the extension 
   
  if(filepath_extension(path) != ".nbr") {
    NIKOLA_LOG_ERROR("Invalid NBR extension found at \'%s\'", path.c_str());
    return false;
  }

  // Open the NBR file and read the header 
  
  if(!file_open(&file, path, (i32)(FILE_OPEN_READ | FILE_OPEN_BINARY))) {
    NIKOLA_LOG_ERROR("Cannot load NBR file at \'%s\'", path.c_str());

    file_close(file);
    return false;
  }

  file_read_bytes(file, &header);

  // Check the validity of the reosurce type
  
  NIKOLA_ASSERT((header.resource_type != RESOURCE_TYPE_INVALID), 
                "Invalid resource type found in NBR file!");
  
  // Check for the validity of the identifier
  
  if(header.identifier != NBR_VALID_IDENTIFIER) {
    NIKOLA_LOG_ERROR("Invalid identifier found in NBR file at \'%s\'. Expected \'%i\' got \'%i\'", 
                      path.c_str(), NBR_VALID_IDENTIFIER, header.identifier);

    file_close(file);
    return false;
  }  

  // Check for the validity of the versions
  
  bool is_valid_version = ((header.major_version == NBR_VALID_MAJOR_VERSION) || 
                           (header.minor_version == NBR_VALID_MINOR_VERSION));
  if(!is_valid_version) {
    NIKOLA_LOG_ERROR("Invalid version found in NBR file at \'%s\'", path.c_str());
    
    file_close(file);
    return false;
  }

  // Check for the resource type

  if(header.resource_type != res_type) {
    NIKOLA_LOG_ERROR("Unexpected resource type found in NBR file '\%s\'", path.c_str());
    
    file_close(file);
    return false;
  }

  return true;
}

/// NBR functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
