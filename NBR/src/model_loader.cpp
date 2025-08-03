#include "nbr.h"

#include <nikola/nikola.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static bool is_valid_extension(const nikola::FilePath& ext) {
  return ext == ".obj"  || 
         ext == ".fbx"  || 
         ext == ".gltf" || 
         ext == ".glb";
}

static void load_node_mesh(aiMesh* mesh, nikola::NBRMesh* nbr_mesh) {
  // Set a default vertex type (for now at least)
  nbr_mesh->vertex_type = (nikola::u8)nikola::VERTEX_TYPE_PNTIWUV;

  nikola::DynamicArray<nikola::f32> vertices;
  vertices.reserve(mesh->mNumVertices);

  // Go through all of the vertices of the given `mesh` and 
  // convert them to the appropriate vertex type. 

  for(nikola::sizei iv = 0; iv < mesh->mNumVertices; iv++) {
    // Adding the position
    
    nikola::f32 pos_x = mesh->mVertices[iv].x;
    nikola::f32 pos_y = mesh->mVertices[iv].y;
    nikola::f32 pos_z = mesh->mVertices[iv].z;

    vertices.push_back(pos_x); vertices.push_back(pos_y); vertices.push_back(pos_z);
    
    // Adding the normal 

    nikola::f32 normal_x = 0.0f;
    nikola::f32 normal_y = 0.0f;
    nikola::f32 normal_z = 0.0f;
    
    if(mesh->HasNormals()) {
      normal_x = mesh->mNormals[iv].x;
      normal_y = mesh->mNormals[iv].y;
      normal_z = mesh->mNormals[iv].z;
    } 
   
    vertices.push_back(normal_x); vertices.push_back(normal_y); vertices.push_back(normal_z);

    // Adding tangents 

    nikola::f32 tan_x = 0.0f;
    nikola::f32 tan_y = 0.0f;
    nikola::f32 tan_z = 0.0f;

    if(mesh->HasTangentsAndBitangents()) {
      tan_x = mesh->mTangents[iv].x;
      tan_y = mesh->mTangents[iv].y;
      tan_z = mesh->mTangents[iv].z;
    }

    vertices.push_back(tan_x); vertices.push_back(tan_y); vertices.push_back(tan_z);
    
    // Adding joints data

    nikola::sizei bone_count = 0;
    nikola::Vec4 ids(-2.0f); // If the values are still -2 after the the loop below, it means this particular mesh does not have bones...
    nikola::Vec4 weights(0.0f);

    if(mesh->HasBones()) {
      ids = nikola::Vec4(-1.0f);

      for(nikola::u32 ib = 0; ib < mesh->mNumBones; ib++) {
        for(nikola::u32 iw = 0; iw < mesh->mBones[ib]->mNumWeights; iw++) {
          // We only care about the bone influencing _this_ vertex
          if(mesh->mBones[ib]->mWeights[iw].mVertexId != iv) {
            continue; 
          }

          // Another bone added to the pile!

          ids[bone_count]     = ib;
          weights[bone_count] = iw;
          bone_count++;
        }
      }
    }

    vertices.push_back(ids.x);     vertices.push_back(ids.y);     vertices.push_back(ids.z);     vertices.push_back(ids.w);
    vertices.push_back(weights.x); vertices.push_back(weights.y); vertices.push_back(weights.z); vertices.push_back(weights.w);

    // Adding texture coordinates
    
    nikola::f32 coord_u = 0.0f; 
    nikola::f32 coord_v = 0.0f; 

    if(mesh->mTextureCoords[0]) {
      coord_u = mesh->mTextureCoords[0][iv].x;
      coord_v = mesh->mTextureCoords[0][iv].y;
    } 
   
    vertices.push_back(coord_u); vertices.push_back(coord_v);
  }

  // Add the material index of the mesh to refrence it later on. Much later on.
  nbr_mesh->material_index = mesh->mMaterialIndex;

  nikola::DynamicArray<nikola::u32> indices;
  indices.reserve(mesh->mNumFaces);

  // Go through all of the faces to retrieve the indices
  
  for(nikola::sizei i = 0; i < mesh->mNumFaces; i++) {
    aiFace* face = &mesh->mFaces[i];

    for(nikola::sizei j = 0; j < face->mNumIndices; j++) {
      indices.push_back((nikola::u32)face->mIndices[j]);
    }
  } 
  
  nikola::sizei bytes_size;

  // Allocate a new vertices array for the mesh
  
  nbr_mesh->vertices_count = vertices.size(); 
  bytes_size               = sizeof(nikola::f32) * nbr_mesh->vertices_count;
  nbr_mesh->vertices       = (nikola::f32*)nikola::memory_allocate(bytes_size);
  
  nikola::memory_copy(nbr_mesh->vertices, vertices.data(), bytes_size);
  
  // Allocate a new indices array for the mesh
  
  nbr_mesh->indices_count = indices.size();
  bytes_size              = sizeof(nikola::u32) * nbr_mesh->indices_count;
  nbr_mesh->indices       = (nikola::u32*)nikola::memory_allocate(bytes_size);
  
  nikola::memory_copy(nbr_mesh->indices, indices.data(), bytes_size);
}

static void load_scene_meshes(const aiScene* scene, ObjData* data, aiNode* node) {
  // Starting at the root node, we will recursively go down 
  // the scene tree and process each mesh to add it to the model. 
  
  for(nikola::sizei i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

    // Convert an `aiMesh` into our `NBRMesh`
    nikola::NBRMesh nbr_mesh; 
    load_node_mesh(mesh, &nbr_mesh);

    // Add the new mesh for later
    data->meshes.push_back(nbr_mesh);
  }

  // The given `node` will also have children of its own. Those 
  // will need to be processed as well, obviously.
  for(nikola::sizei i = 0; i < node->mNumChildren; i++) {
    load_scene_meshes(scene, data, node->mChildren[i]);
  }
}

static void load_material_texture(aiMaterial* material, aiTextureType type, ObjData* data, nikola::i8* index) {
  for(nikola::u32 i = 0; i < material->GetTextureCount(type); i++) {
    aiString str;
    material->GetTexture(type, i, &str);

    // Convert into our `NBRTexture`
    nikola::NBRTexture texture;
    image_loader_load_texture(&texture, nikola::filepath_append(data->parent_dir, str.C_Str()));
    data->textures.push_back(texture);

    *index += (nikola::i8)data->textures.size();
  }
}

static void load_scene_materials(const aiScene* scene, ObjData* data) {
  data->materials.reserve(scene->mNumMaterials);

  // Go through each material in the scene 
  for(nikola::sizei i = 0; i < scene->mNumMaterials; i++) {
    aiMaterial* material = scene->mMaterials[i];
    nikola::NBRMaterial nbr_material; 
 
    // Convert the colors
  
    aiColor3D ambient;
    material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
    
    aiColor3D diffuse;
    material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
    
    aiColor3D specular;
    material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
    
    nbr_material.ambient[0]  = ambient.r;  nbr_material.ambient[1]  = ambient.g;  nbr_material.ambient[2]  = ambient.b;
    nbr_material.diffuse[0]  = diffuse.r;  nbr_material.diffuse[1]  = diffuse.g;  nbr_material.diffuse[2]  = diffuse.b;
    nbr_material.specular[0] = specular.r; nbr_material.specular[1] = specular.g; nbr_material.specular[2] = specular.b;

    // Load textures of each kind
    
    nbr_material.diffuse_index  = -1; 
    nbr_material.specular_index = -1; 
    nbr_material.normal_index   = -1; 

    load_material_texture(material, aiTextureType_DIFFUSE, data, &nbr_material.diffuse_index); 
    load_material_texture(material, aiTextureType_SPECULAR, data, &nbr_material.specular_index); 
    load_material_texture(material, aiTextureType_NORMALS, data, &nbr_material.normal_index); 

    // New material!
    data->materials.push_back(nbr_material); 
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

  // Load Assimp file
  
  int flags = (aiProcess_Triangulate           | 
               aiProcess_FlipUVs               | 
               aiProcess_CalcTangentSpace      |
               aiProcess_GenNormals            | 
               aiProcess_SplitLargeMeshes      |
               aiProcess_ImproveCacheLocality  | 
               aiProcess_JoinIdenticalVertices |
               aiProcess_OptimizeMeshes        | 
               aiProcess_OptimizeGraph         |
               aiProcess_LimitBoneWeights      | 
               aiProcess_GlobalScale); 

  Assimp::Importer imp; 
  imp.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 1.0f);
  imp.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, nikola::NBR_JOINT_WEIGHTS_MAX);

  const aiScene* scene = imp.ReadFile(path, flags);
  if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    NIKOLA_LOG_ERROR("Could not load Model at \'%s\' - %s", path.c_str(), imp.GetErrorString());
    return false;
  }

  // Loading everything into `ObjData`
  
  ObjData data; 
  data.parent_dir = nikola::filepath_parent_path(path); // Usually, `path` will refer to the 3D model file directly so we need its immediate parent

  // Meshes init 
  
  load_scene_meshes(scene, &data, scene->mRootNode);
  model->meshes_count  = data.meshes.size();
  model->meshes        = (nikola::NBRMesh*)nikola::memory_allocate(sizeof(nikola::NBRMesh) * model->meshes_count);
  nikola::memory_copy(model->meshes, data.meshes.data(), data.meshes.size() * sizeof(nikola::NBRMesh));

  // Materials init
  
  load_scene_materials(scene, &data);  
  model->materials_count = data.materials.size();
  model->materials       = (nikola::NBRMaterial*)nikola::memory_allocate(sizeof(nikola::NBRMaterial) * model->materials_count);
  nikola::memory_copy(model->materials, data.materials.data(), data.materials.size() * sizeof(nikola::NBRMaterial));

  // Textures init
  
  /*
   * @NOTE (29/3/2025): 
   *
   * Just in case the material has no textures, we will 
   * give it a default white texture to use instead.
  */
  if(data.textures.empty()) {
    data.default_texture = {
      .width    = 1, 
      .height   = 1, 
      .channels = 4, 
      .pixels   = nikola::memory_allocate(4), // 4 = width * height * channels
    };
    nikola::memory_set(data.default_texture.pixels, 0xFF, 4);
    data.textures.push_back(data.default_texture); 
  }

  model->textures_count = data.textures.size(); 
  model->textures       = (nikola::NBRTexture*)nikola::memory_allocate(sizeof(nikola::NBRTexture) * model->textures_count);
  nikola::memory_copy(model->textures, data.textures.data(), data.textures.size() * sizeof(nikola::NBRTexture));

  return true;
}

void model_loader_unload(nikola::NBRModel& model) {
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
}

/// Model loader functions
/// ----------------------------------------------------------------------


} // End of nbr

//////////////////////////////////////////////////////////////////////////
