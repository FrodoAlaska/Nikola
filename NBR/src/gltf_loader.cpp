#include "nbr.h"

#include <nikola/nikola.h>

#include <cgltf/cgltf.h>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nbr

/// ----------------------------------------------------------------------
/// ObjData
struct ObjData {
  nikola::DynamicArray<nikola::NBRMesh> meshes;
  nikola::DynamicArray<nikola::NBRMaterial> materials;
  nikola::DynamicArray<nikola::NBRTexture> textures;

  nikola::FilePath parent_dir;

  nikola::NBRTexture default_texture;
};
/// ObjData
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static bool is_valid_extension(const nikola::FilePath& ext) {
  return ext == ".gltf" || 
         ext == ".glb";
}

static void load_primitives(const cgltf_mesh* mesh, ObjData* out_data) {
  for(nikola::sizei i = 0; i < mesh->primitives_count; i++) {
    NIKOLA_LOG_TRACE("    Attributes = %zu", mesh->primitives[i].attributes_count);
    for(nikola::sizei j = 0; j < mesh->primitives[i].attributes_count; j++) {
      NIKOLA_LOG_TRACE("      Name  = %s", mesh->primitives[i].attributes[j].name);
      NIKOLA_LOG_TRACE("      Type  = %i", (int)mesh->primitives[i].attributes[j].type);
      NIKOLA_LOG_TRACE("      Index = %i", mesh->primitives[i].attributes[j].index);
    } 
  }
}

static void load_meshes(const cgltf_data* gltf, ObjData* out_data) {
  NIKOLA_LOG_TRACE("Meshes count = %zu", gltf->meshes_count);
 
  for(nikola::sizei i = 0; i < gltf->meshes_count; i++) {
    NIKOLA_LOG_TRACE("Mesh name = %s", gltf->meshes[i].name);
    NIKOLA_LOG_TRACE("  Primitives = %zu", gltf->meshes[i].primitives_count);
    load_primitives(&gltf->meshes[i], out_data);

    NIKOLA_LOG_TRACE("  Weights    = %zu", gltf->meshes[i].weights_count);
    NIKOLA_LOG_TRACE("  Targets    = %zu", gltf->meshes[i].target_names_count);
  } 
}

static void load_images(const cgltf_data* gltf, ObjData* out_data) {
  NIKOLA_LOG_TRACE("Images count = %zu", gltf->images_count);

  for(nikola::sizei i = 0; i < gltf->images_count; i++) {
    NIKOLA_LOG_TRACE("  URI  = %s", nikola::filepath_append(out_data->parent_dir, gltf->images[i].uri).c_str());
  }
}

static void load_materials(const cgltf_data* gltf, ObjData* out_data) {
  NIKOLA_LOG_TRACE("Materials count = %zu", gltf->materials_count);
 
  for(nikola::sizei i = 0; i < gltf->materials_count; i++) {
    NIKOLA_LOG_TRACE("  Name = %s", gltf->materials[i].name);

    // Basically diffuse and specular textures

    if(gltf->materials[i].has_pbr_metallic_roughness) {
      NIKOLA_LOG_TRACE("  Diffuse index = %zu", 
                       cgltf_texture_index(gltf, gltf->materials[i].pbr_metallic_roughness.base_color_texture.texture));
      NIKOLA_LOG_TRACE("  Specular index = %zu", 
                       cgltf_texture_index(gltf, gltf->materials[i].pbr_metallic_roughness.metallic_roughness_texture.texture));
    }

    // Normal texture
    NIKOLA_LOG_TRACE("  Normal index = %zu", 
        cgltf_texture_index(gltf, gltf->materials[i].normal_texture.texture));
  }
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Model loader functions

bool model_loader_load(nikola::NBRModel* model, const nikola::FilePath& path) {
  nikola::FilePath ext = nikola::filepath_extension(path);
  if(!is_valid_extension(ext)) {
    NIKOLA_LOG_ERROR("No valid model loader for \'%s\'", ext.c_str());
    return false;
  } 

  // Load the GLTF file

  cgltf_options options;
  nikola::memory_zero(&options, sizeof(cgltf_options)); 

  cgltf_data* gltf_data = (cgltf_data*)nikola::memory_allocate(sizeof(cgltf_data));
  cgltf_result res      = cgltf_parse_file(&options, path.c_str(), &gltf_data); 
  
  if(res != cgltf_result_success) {
    NIKOLA_LOG_ERROR("Could not load GLTF file at \'%s\'", path.c_str());
    return false;
  }

  // Loading everything into `ObjData`
  
  ObjData data; 
  data.parent_dir = nikola::filepath_parent_path(path); // Usually, `path` will refer to the 3D model file directly so we need its immediate parent

  // Meshes init 
  
  load_meshes(gltf_data, &data);

  // model->meshes_count  = data.meshes.size();
  // model->meshes        = (nikola::NBRMesh*)nikola::memory_allocate(sizeof(nikola::NBRMesh) * model->meshes_count);
  // nikola::memory_copy(model->meshes, data.meshes.data(), data.meshes.size() * sizeof(nikola::NBRMesh));
  
  // Materials init
  
  load_materials(gltf_data, &data);  
  // model->materials_count = data.materials.size();
  // model->materials       = (nikola::NBRMaterial*)nikola::memory_allocate(sizeof(nikola::NBRMaterial) * model->materials_count);
  // nikola::memory_copy(model->materials, data.materials.data(), data.materials.size() * sizeof(nikola::NBRMaterial));

  // Textures init
 
  load_images(gltf_data, &data);

  /*
   * @NOTE (29/3/2025): 
   *
   * Just in case the material has no textures, we will 
   * give it a default white texture to use instead.
  */
  // if(data.textures.empty()) {
  //   data.default_texture = {
  //     .width    = 1, 
  //     .height   = 1, 
  //     .channels = 4, 
  //     .pixels   = nikola::memory_allocate(4), // 4 = width * height * channels
  //   };
  //   nikola::memory_set(data.default_texture.pixels, 0xFF, 4);
  //   data.textures.push_back(data.default_texture); 
  // }

  // model->textures_count = data.textures.size(); 
  // model->textures       = (nikola::NBRTexture*)nikola::memory_allocate(sizeof(nikola::NBRTexture) * model->textures_count);
  // nikola::memory_copy(model->textures, data.textures.data(), data.textures.size() * sizeof(nikola::NBRTexture));

  cgltf_free(gltf_data);
  return true;
}

void model_loader_unload(nikola::NBRModel& model) {
#if 0
  // Unload meshes
  for(nikola::sizei i = 0; i < model.meshes_count; i++) {
    nikola::memory_free(model.meshes[i].vertices); 
    nikola::memory_free(model.meshes[i].indices); 
  }
  nikola::memory_free(model.meshes);

  // Unload materials 
  nikola::memory_free(model.materials);

  // The first texture is usually a white texture that wasn't 
  // loaded by STB.
  nikola::memory_free(model.textures[0].pixels);

  // Unload textures 
  for(nikola::sizei i = 1; i < model.textures_count; i++) {
    image_loader_unload_texture(model.textures[i]);
  }
  nikola::memory_free(model.textures);
#endif
}

/// Model loader functions
/// ----------------------------------------------------------------------


} // End of nbr

//////////////////////////////////////////////////////////////////////////
