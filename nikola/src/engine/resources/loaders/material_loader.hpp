#pragma once

#include "nikola/nikola_engine.hpp"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

void material_loader_load(ResourceStorage* storage, 
                          Material* mat, 
                          const ResourceID& diffuse_id, 
                          const ResourceID& specular_id, 
                          const ResourceID& shader_id);

} // End of nikola

//////////////////////////////////////////////////////////////////////////
