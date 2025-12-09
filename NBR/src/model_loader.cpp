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

  nikola::HashMap<nikola::String, nikola::sizei> bone_map;
  nikola::sizei bone_count = 0; 

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

static void push_bone(ObjData* data, const aiScene* ai_scene, aiNode* node) {
  // Add the bone to the map

  nikola::String bone_name  = node->mName.C_Str();
  data->bone_map[bone_name] = data->bone_map.size(); 

  // Go through the children

  for(nikola::u32 i = 0; i < node->mNumChildren; i++) {
    push_bone(data, ai_scene, node->mChildren[i]);
  }
}

static void load_bone_map(ObjData* data, const aiScene* ai_scene) {
  /* @NOTE (3/12/2025, Mohamed):
  *
  * Unfortunately, Assimp has no way to refer to the root bone
  * (not that I know of at least), and so I'm subjected to use some 
  * very terrible methods. With all the animations that _I_ use, the 
  * root joing is always called `_rootJoint`. Jusst like that. 
  * Here, we search for that bone and then traverse its children, 
  * adding them to the map one by one. 
  *
  * In the future, we'll use a specific format instead of a library that 
  * tries (terribly) to generalize all formats. It's probably going to be 
  * `cgltf`, since that's what we use for loading skeletons and animations.
  */

  aiBone* root_bone = ai_scene->findBone(aiString("_rootJoint"));
  if(!root_bone) {
    NIKOLA_LOG_DEBUG("The bone name \'_rootJoint\' is not the root. Ignore this message if this mesh is not animated.");
    return;
  }

  push_bone(data, ai_scene, root_bone->mNode);
}

static void load_node_mesh(ObjData* data, aiMesh* mesh, nikola::NBRMesh* nbr_mesh) {
  // Set the components that should exist in the model
  
  nbr_mesh->vertex_component_bits = (nikola::u8)nikola::VERTEX_COMPONENT_POSITION | 
                                    (nikola::u8)nikola::VERTEX_COMPONENT_NORMAL   |
                                    (nikola::u8)nikola::VERTEX_COMPONENT_TANGENT  |
                                    (nikola::u8)nikola::VERTEX_COMPONENT_TEXTURE_COORDS;

  // Go through all of the vertices of the given `mesh` and 
  // convert them to the appropriate vertex type. 
  
  nikola::DynamicArray<nikola::f32> vertices;
  vertices.reserve(mesh->mNumVertices);

  for(nikola::sizei iv = 0; iv < mesh->mNumVertices; iv++) {
    // Adding the position
    
    nikola::f32 pos_x = mesh->mVertices[iv].x;
    nikola::f32 pos_y = mesh->mVertices[iv].y;
    nikola::f32 pos_z = mesh->mVertices[iv].z;

    vertices.push_back(pos_x); vertices.push_back(pos_y); vertices.push_back(pos_z);
    
    // Adding the normal 

    nikola::f32 normal_x = mesh->mNormals[iv].x;
    nikola::f32 normal_y = mesh->mNormals[iv].y;
    nikola::f32 normal_z = mesh->mNormals[iv].z;
    
    vertices.push_back(normal_x); vertices.push_back(normal_y); vertices.push_back(normal_z);

    // Adding tangents 

    nikola::f32 tan_x = mesh->mTangents[iv].x;
    nikola::f32 tan_y = mesh->mTangents[iv].y;
    nikola::f32 tan_z = mesh->mTangents[iv].z;

    vertices.push_back(tan_x); vertices.push_back(tan_y); vertices.push_back(tan_z);
    
    // Adding joints data

    nikola::sizei vec_index = 0; 
    nikola::Vec4 ids        = nikola::Vec4(-2.0f); // If the values are still -2 after the the loop below, it means this mesh is not animated
    nikola::Vec4 weights    = nikola::Vec4(0.0f);

    if(mesh->HasBones()) {
      nbr_mesh->vertex_component_bits |= (nikola::u8)nikola::VERTEX_COMPONENT_JOINT_ID |
                                         (nikola::u8)nikola::VERTEX_COMPONENT_JOINT_WEIGHT;

      ids = nikola::Vec4(0.0f);

      for(nikola::u32 ib = 0; ib < mesh->mNumBones; ib++) {
        aiBone* bone = mesh->mBones[ib];

        for(nikola::u32 iw = 0; iw < bone->mNumWeights; iw++) {
          aiVertexWeight* weight = &bone->mWeights[iw];

          // Vertex IDs are incremental. If it's greater than the 
          // current vertex index, then there is no reason to keep going... 

          if(weight->mVertexId > iv) {
            break;
          }

          // We only care about the bone influencing _this_ vertex
          
          if(weight->mVertexId != iv) {
            continue; 
          }

          // Another bone added to the pile!

          nikola::String bone_name = bone->mNode->mName.C_Str();

          ids[vec_index]     = data->bone_map[bone_name];
          weights[vec_index] = weight->mWeight;
          vec_index++;
        }
      }
    }

    vertices.push_back(ids.x);     
    vertices.push_back(ids.y); 
    vertices.push_back(ids.z);
    vertices.push_back(ids.w);
    
    vertices.push_back(weights.x); 
    vertices.push_back(weights.y); 
    vertices.push_back(weights.z);
    vertices.push_back(weights.w);

    // Adding texture coordinates
    
    nikola::f32 coord_u = mesh->mTextureCoords[0][iv].x; 
    nikola::f32 coord_v = mesh->mTextureCoords[0][iv].y; 
   
    vertices.push_back(coord_u); vertices.push_back(coord_v);
  }
  
  // Add the material index of the mesh to refrence it later on. Much later on.
  nbr_mesh->material_index = mesh->mMaterialIndex;

  // Load indices

  nikola::DynamicArray<nikola::u32> indices;
  indices.reserve(mesh->mNumFaces);
  
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
    load_node_mesh(data, mesh, &nbr_mesh);

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
    if(!image_loader_load_texture(&texture, nikola::filepath_append(data->parent_dir, str.C_Str()))) {
      continue;
    }

    data->textures.push_back(texture);
    *index += (nikola::i8)data->textures.size();
  }
}

static void load_scene_materials(const aiScene* scene, ObjData* data) {
  data->materials.reserve(scene->mNumMaterials);

  // Go through each material in the scene 
  for(nikola::sizei i = 0; i < scene->mNumMaterials; i++) {
    aiMaterial* material = scene->mMaterials[i];
    nikola::NBRMaterial nbr_material{}; 
 
    // Convert the colors
  
    aiColor3D diffuse;
    material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
    
    nbr_material.color[0] = diffuse.r;  
    nbr_material.color[1] = diffuse.g; 
    nbr_material.color[2] = diffuse.b;

    // Load individual values

    material->Get(AI_MATKEY_METALLIC_FACTOR, nbr_material.metallic);
    material->Get(AI_MATKEY_ROUGHNESS_FACTOR, nbr_material.roughness);
    material->Get(AI_MATKEY_EMISSIVE_INTENSITY, nbr_material.emissive);

    // Load textures of each kind

    load_material_texture(material, aiTextureType_BASE_COLOR, data, &nbr_material.albedo_index); 
    load_material_texture(material, aiTextureType_METALNESS, data, &nbr_material.metallic_index); 
    load_material_texture(material, aiTextureType_DIFFUSE_ROUGHNESS, data, &nbr_material.roughness_index); 
    load_material_texture(material, aiTextureType_NORMALS, data, &nbr_material.normal_index); 
    load_material_texture(material, aiTextureType_EMISSIVE, data, &nbr_material.emissive_index); 

    // @TEMP: Temporary fix since `AI_MATKEY_EMISSIVE_INTENSITY` apparently 
    // doesn't work. We're settings the emissive factor to `1.0f` if there 
    // is an emissive texture at all.
    
    if(nbr_material.emissive_index != -1) {
      nbr_material.emissive = 1.0f;
    }

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
    NIKOLA_LOG_ERROR("No valid model loader for \'%s\'. Currently supported formats: GLTF, GLB", ext.c_str());
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
               aiProcess_PopulateArmatureData  |
               aiProcess_GlobalScale); 

  Assimp::Importer imp; 
  imp.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, nikola::NBR_MODEL_IMPORT_SCALE);
  imp.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, nikola::NBR_JOINT_WEIGHTS_MAX);

  const aiScene* scene = imp.ReadFile(path, flags);
  if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    NIKOLA_LOG_ERROR("Could not load Model at \'%s\' - %s", path.c_str(), imp.GetErrorString());
    return false;
  }

  // Loading everything into `ObjData`
  
  ObjData data; 
  data.parent_dir = nikola::filepath_parent_path(path); // Usually, `path` will refer to the 3D model file directly so we need its immediate parent

  // Load the bone map 
  load_bone_map(&data, scene);

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
