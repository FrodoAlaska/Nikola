#include "nbr.hpp"

#include <nikola/nikola_core.hpp>
#include <nikola/nikola_engine.hpp>

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
};
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions

static bool is_valid_extension(const nikola::String& ext) {
  return ext == ".obj" || 
         ext == ".fbx" || 
         ext == ".gltf";
}

static void load_node_mesh(aiMesh* mesh, nikola::NBRMesh* nbr_mesh) {
  // Set a default vertex type (for now at least)
  nbr_mesh->vertex_type = (nikola::u8)nikola::VERTEX_TYPE_PNUV;

  nikola::DynamicArray<nikola::f32> vertices;
  vertices.reserve(mesh->mNumVertices);

  // Go through all of the vertices of the given `mesh` and 
  // convert them to the appropriate vertex type. 
  for(nikola::sizei i = 0; i < mesh->mNumVertices; i++) {
    // Getting the positions
    nikola::f32 pos_x = mesh->mVertices[i].x;
    nikola::f32 pos_y = mesh->mVertices[i].y;
    nikola::f32 pos_z = mesh->mVertices[i].z;

    // Adding the position
    vertices.push_back(pos_x); vertices.push_back(pos_y); vertices.push_back(pos_z);

    // Getting the normals
    nikola::f32 normal_x = 0.0f;
    nikola::f32 normal_y = 0.0f;
    nikola::f32 normal_z = 0.0f;
    
    if(mesh->HasNormals()) {
      normal_x = mesh->mNormals[i].x;
      normal_y = mesh->mNormals[i].y;
      normal_z = mesh->mNormals[i].z;
    } 
   
    // Adding the normal 
    vertices.push_back(normal_x); vertices.push_back(normal_y); vertices.push_back(normal_z);

    // Getting the texture coordinates
    nikola::f32 coord_u = 0.0f; 
    nikola::f32 coord_v = 0.0f; 

    if(mesh->mTextureCoords[0]) {
      coord_u = mesh->mTextureCoords[0][i].x;
      coord_v = mesh->mTextureCoords[0][i].y;
    } 
   
    // Adding the texture coordinates
    vertices.push_back(coord_u); vertices.push_back(coord_v);

    // Getting the colors
    if(mesh->mColors[0]) {
      nbr_mesh->vertex_type = (nikola::u8)nikola::VERTEX_TYPE_PNCUV;  

      nikola::f32 r = mesh->mColors[0][i].r;      
      nikola::f32 g = mesh->mColors[0][i].g;      
      nikola::f32 b = mesh->mColors[0][i].b;      
      nikola::f32 a = mesh->mColors[0][i].a;

      // Adding the color 
      vertices.push_back(r); vertices.push_back(g); vertices.push_back(b); vertices.push_back(a);
    }
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

static void load_material_texture(aiMaterial* material, aiTextureType type, ObjData* data) {
  nikola::sizei textures_count = material->GetTextureCount(type); 

  for(nikola::sizei i = 0; i < textures_count; i++) {
    aiString str;
    material->GetTexture(type, i, &str);

    // Convert into our `NBRTexture`
    nikola::NBRTexture texture;
    image_loader_load_texture(&texture, data->parent_dir / nikola::FilePath(str.C_Str()));
    data->textures.push_back(texture);
  }
}

static void load_scene_materials(const aiScene* scene, ObjData* data) {
  data->materials.reserve(scene->mNumMaterials);

  // Go through each material in the scene 
  for(nikola::sizei i = 0; i < scene->mNumMaterials; i++) {
    aiMaterial* material = scene->mMaterials[i];
    nikola::NBRMaterial nbr_material; 
  
    // Get the ambient color
    aiColor3D ambient;
    material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
    
    // Get the diffuse color
    aiColor3D diffuse;
    material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
    
    // Get the specular color
    aiColor3D specular;
    material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
 
    // Convert the colors
    nbr_material.ambient[0]  = ambient.r;  nbr_material.ambient[1]  = ambient.g;  nbr_material.ambient[2]  = ambient.b;
    nbr_material.diffuse[0]  = diffuse.r;  nbr_material.diffuse[1]  = diffuse.g;  nbr_material.diffuse[2]  = diffuse.b;
    nbr_material.specular[0] = specular.r; nbr_material.specular[1] = specular.g; nbr_material.specular[2] = specular.b;

    // Load the diffuse and specular textures
    load_material_texture(material, aiTextureType_DIFFUSE, data); 
    load_material_texture(material, aiTextureType_SPECULAR, data); 
   
    // Get the diffuse index
    nbr_material.diffuse_index = data->textures.size() == 0 ? 0 : data->textures.size() - 1;

    // Get the diffuse index
    nbr_material.specular_index = 0;
    material->Get(AI_MATKEY_TEXTURE_SPECULAR(i), nbr_material.specular_index);

    // Add a new material
    data->materials.push_back(nbr_material); 
  }
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Model loader functions

bool model_loader_load(nikola::NBRModel* model, const nikola::FilePath& path) {
  if(!is_valid_extension(path.extension().string())) {
    NIKOLA_LOG_ERROR("No valid model loader for \'%s\'", path.extension().string().c_str());
    return false;
  } 

  // Load Assimp file
  Assimp::Importer imp; 
  const aiScene* scene = imp.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
  if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    NIKOLA_LOG_ERROR("Could not load Model at \'%s\', %s", path.string().c_str(), imp.GetErrorString());
    return false;
  }

  // Loading everything into `ObjData`
  ObjData data; 
  data.parent_dir = path.parent_path(); // Usually, `path` will refer to the 3D model file directly so we need its immediate parent
  
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
  model->textures_count = data.textures.size(); 
  model->textures       = (nikola::NBRTexture*)nikola::memory_allocate(sizeof(nikola::NBRTexture) * model->textures_count);
  nikola::memory_copy(model->textures, data.textures.data(), data.textures.size() * sizeof(nikola::NBRTexture));

  return true;
}

/// Model loader functions
/// ----------------------------------------------------------------------


} // End of nbr

//////////////////////////////////////////////////////////////////////////
