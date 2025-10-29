#include "nikola/nikola_resources.h"
#include "nikola/nikola_event.h"
#include "nikola/nikola_render.h"
#include "nikola/nikola_file.h"
#include "nikola/nikola_thread.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// ResourceGroup 
struct ResourceGroup {
  String name; 
  FilePath parent_dir;
  ResourceGroupID id;

  DynamicArray<GfxBuffer*> buffers;
  DynamicArray<GfxTexture*> textures;
  DynamicArray<GfxCubemap*> cubemaps;
  DynamicArray<GfxShader*> shaders;
  DynamicArray<AudioBufferID> audio_buffers;
  
  DynamicArray<Mesh*> meshes;
  DynamicArray<Material*> materials;
  DynamicArray<ShaderContext*> shader_contexts;
  DynamicArray<Skybox*> skyboxes;
  DynamicArray<Model*> models;
  DynamicArray<Animation*> animations;
  DynamicArray<Font*> fonts;

  HashMap<String, ResourceID> named_ids;
};
/// ResourceGroup 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// ResourceManager 
struct ResourceManager {
  HashMap<ResourceGroupID, ResourceGroup> groups;
};

static ResourceManager s_manager;
/// ResourceManager 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Macros (Unfortunately)

#define DESTROY_CORE_RESOURCE_MAP(group, map, clear_func) { \
  for(auto& res : group->map) {                             \
    clear_func(res);                                        \
  }                                                         \
}

#define DESTROY_COMP_RESOURCE_MAP(group, map) { \
  for(auto& res : group->map) {                 \
    delete res;                                 \
  }                                             \
}

#define PUSH_RESOURCE(group, resources, res, type, res_id) { \
  group->resources.push_back(res);                           \
  res_id._type = type;                                       \
  res_id._id   = (u16)group->resources.size() - 1;           \
  res_id.group = group->id;                                  \
}

#define GROUP_CHECK(group_id) NIKOLA_ASSERT((group_id != RESOURCE_GROUP_INVALID), "Cannot push a resource to an invalid group")

/// Macros (Unfortunately)
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Private functions 

static bool open_and_check_nbr_file(const FilePath& parent_dir, const FilePath& nbr_path, File* file, NBRHeader* header) {
  FilePath path = filepath_append(parent_dir, nbr_path);

  // Open the NBR file and read the header 
  
  if(!file_open(file, path, (i32)(FILE_OPEN_READ | FILE_OPEN_BINARY))) {
    NIKOLA_LOG_ERROR("Cannot load NBR file at \'%s\'", path.c_str());
    return false;
  }

  file_read_bytes(*file, header);

  // Check the validity of the reosurce type
  
  NIKOLA_ASSERT((header->resource_type != RESOURCE_TYPE_INVALID), 
                "Invalid resource type found in NBR file!");
  
  // Check for the validity of the identifier
  
  if(header->identifier != NBR_VALID_IDENTIFIER) {
    NIKOLA_LOG_ERROR("Invalid identifier found in NBR file at \'%s\'. Expected \'%i\' got \'%i\'", 
                      path.c_str(), NBR_VALID_IDENTIFIER, header->identifier);

    return false;
  }  

  // Check for the validity of the versions
  
  bool is_valid_version = ((header->major_version == NBR_VALID_MAJOR_VERSION) || 
                           (header->minor_version == NBR_VALID_MINOR_VERSION));
  if(!is_valid_version) {
    NIKOLA_LOG_ERROR("Invalid version found in NBR file at \'%s\'", path.c_str());
    return false;
  }

  return true;
}

static const char* buffer_type_str(const GfxBufferType type) {
  switch(type) {
    case GFX_BUFFER_VERTEX: 
      return "GFX_BUFFER_VERTEX";
    case GFX_BUFFER_INDEX: 
      return "GFX_BUFFER_INDEX";
    case GFX_BUFFER_UNIFORM: 
      return "GFX_BUFFER_UNIFORM";
    case GFX_BUFFER_SHADER_STORAGE: 
      return "GFX_BUFFER_SHADER_STORAGE";
    default:
      return "INVALID BUFFER TYPE";
  }
}

static const char* texture_type_str(const GfxTextureType type) {
  switch(type) {
    case GFX_TEXTURE_1D:
      return "GFX_TEXTURE_1D";
    case GFX_TEXTURE_2D:
      return "GFX_TEXTURE_2D";
    case GFX_TEXTURE_3D:
      return "GFX_TEXTURE_3D";
    case GFX_TEXTURE_2D_ARRAY:
      return "GFX_TEXTURE_2D_ARRAY";
    case GFX_TEXTURE_IMAGE_1D:
      return "GFX_TEXTURE_IMAGE_1D";
    case GFX_TEXTURE_IMAGE_2D:
      return "GFX_TEXTURE_IMAGE_2D";
    case GFX_TEXTURE_IMAGE_3D:
      return "GFX_TEXTURE_IMAGE_3D";
    case GFX_TEXTURE_DEPTH_TARGET:
      return "GFX_TEXTURE_DEPTH_TARGET";
    case GFX_TEXTURE_STENCIL_TARGET:
      return "GFX_TEXTURE_STENCIL_TARGET";
    case GFX_TEXTURE_DEPTH_STENCIL_TARGET:
      return "GFX_TEXTURE_DEPTH_STENCIL_TARGET";
    default:
      return "INVALID TEXTURE TYPE";
  }
}

static const char* geo_type_str(const GeometryType type) {
  switch(type) {
    case GEOMETRY_CUBE:
      return "GEOMETRY_CUBE";
    case GEOMETRY_SPHERE:
      return "GEOMETRY_SPHERE";
    case GEOMETRY_SKYBOX:
      return "GEOMETRY_SKYBOX";
    case GEOMETRY_BILLBOARD:
      return "GEOMETRY_BILLBOARD";
    case GEOMETRY_DEBUG_CUBE:
      return "GEOMETRY_DEBUG_CUBE";
    case GEOMETRY_DEBUG_SPHERE:
      return "GEOMETRY_DEBUG_SPHERE";
    default:
      return "INVALID GEOMETRY TYPE";
  }
}

static const char* audio_format_str(const AudioBufferFormat format) {
  switch(format) {
    case AUDIO_BUFFER_FORMAT_U8:
      return "AUDIO_BUFFER_FORMAT_U8";
    case AUDIO_BUFFER_FORMAT_I16:
      return "AUDIO_BUFFER_FORMAT_I16";
    case AUDIO_BUFFER_FORMAT_F32:
      return "AUDIO_BUFFER_FORMAT_F32";
    default:
      return "INVALID AUDIO BUFFER FORMAT";
  }
}

template<typename T> 
static T get_resource(const ResourceID& id, DynamicArray<T>& res, const ResourceType type) {
  NIKOLA_ASSERT((id._type == type), "Invalid type when trying to retrieve a resource");
  NIKOLA_ASSERT((id._id >= 0 && id._id <= (u16)res.size()), "Invalid ID when trying to retrieve a resource");
  NIKOLA_ASSERT(RESOURCE_IS_VALID(id), "Cannot retrieve a resource from an invalid group");

  return res[id._id];
}

static bool load_texture_nbr(ResourceGroup* group, GfxTexture* texture, const FilePath& nbr_path) {
  //
  // Load the NBR file
  // 

  NBRHeader header;
  File file;
  
  if(!open_and_check_nbr_file(group->parent_dir, nbr_path, &file, &header)) {
    file_close(file);
    return false;
  }

  if(header.resource_type != RESOURCE_TYPE_TEXTURE) {
    NIKOLA_LOG_ERROR("Unexpected resource type found in NBR file '\%s\'", nbr_path.c_str());
    return false;
  }

  NBRTexture nbr_texture;
  file_read_bytes(file, &nbr_texture);

  //
  // Convert the NBR format to a valid texture
  // 

  GfxTextureDesc tex_desc; 
  tex_desc.width  = nbr_texture.width; 
  tex_desc.height = nbr_texture.height; 
  tex_desc.depth  = 0; 
  tex_desc.mips   = 1; 
  tex_desc.type   = GFX_TEXTURE_2D; 
  tex_desc.format = (GfxTextureFormat)nbr_texture.format; 
  tex_desc.data   = nbr_texture.pixels;

  if(!gfx_texture_load(texture, tex_desc)) {
    NIKOLA_LOG_ERROR("Failed to load texture at '\%s\'", nbr_path.c_str());
    return false;
  } 

  //
  // Freeing NBR data
  // 

  memory_free(nbr_texture.pixels);
  file_close(file); 

  // Some useful info dump

  NIKOLA_LOG_DEBUG("Group \'%s\' pushed texture:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Size = %i X %i", tex_desc.width, tex_desc.height);
  NIKOLA_LOG_DEBUG("     Type = %s", texture_type_str(tex_desc.type));
  NIKOLA_LOG_DEBUG("     Path = %s", nbr_path.c_str());

  // Done!
  return true;
}

static bool load_cubemap_nbr(ResourceGroup* group, GfxCubemap* cubemap, const FilePath& nbr_path) {
  ///
  // Load the NBR file
  // 

  NBRHeader header;
  File file;
  
  if(!open_and_check_nbr_file(group->parent_dir, nbr_path, &file, &header)) {
    file_close(file); 
    return false;
  }

  if(header.resource_type != RESOURCE_TYPE_CUBEMAP) {
    NIKOLA_LOG_ERROR("Unexpected resource type found in NBR file '\%s\'", nbr_path.c_str());
    return false;
  }

  NBRCubemap nbr_cubemap;
  file_read_bytes(file, &nbr_cubemap);

  //
  // Convert the NBR format to a valid cubemap
  // 

  GfxCubemapDesc cube_desc; 
  cube_desc.width       = nbr_cubemap.width; 
  cube_desc.height      = nbr_cubemap.height; 
  cube_desc.mips        = 1; 
  cube_desc.format      = (GfxTextureFormat)nbr_cubemap.format; 
  cube_desc.faces_count = nbr_cubemap.faces_count; 

  for(sizei i = 0; i < cube_desc.faces_count; i++) {
    cube_desc.data[i] = nbr_cubemap.pixels[i];
  }

  gfx_cubemap_load(cubemap, cube_desc);

  //
  // Freeing NBR data
  // 

  for(sizei i = 0; i < nbr_cubemap.faces_count; i++) {
    memory_free(nbr_cubemap.pixels[i]);
  }
  file_close(file); 

  // Some useful info dump

  NIKOLA_LOG_DEBUG("Group \'%s\' pushed cubemap:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Size  = %i X %i", cube_desc.width, cube_desc.height);
  NIKOLA_LOG_DEBUG("     Faces = %i", cube_desc.faces_count);
  NIKOLA_LOG_DEBUG("     Path  = %s", nbr_path.c_str());
  
  // Done!
  return true;
}

static bool load_shader_nbr(ResourceGroup* group, GfxShader* shader, const FilePath& nbr_path) {
  //
  // Load the NBR file
  // 

  NBRHeader header;
  File file;

  if(!open_and_check_nbr_file(group->parent_dir, nbr_path, &file, &header)) {
    file_close(file); 
    return false;
  }
  
  if(header.resource_type != RESOURCE_TYPE_SHADER) {
    NIKOLA_LOG_ERROR("Unexpected resource type found in NBR file '\%s\'", nbr_path.c_str());
    return false;
  }

  NBRShader nbr_shader;
  file_read_bytes(file, &nbr_shader);

  //
  // Convert the NBR format to a valid shader
  //

  GfxShaderDesc shader_desc = {};
  shader_desc.vertex_source  = nbr_shader.vertex_source;
  shader_desc.pixel_source   = nbr_shader.pixel_source;
  shader_desc.compute_source = nbr_shader.compute_source;

  gfx_shader_load(shader, shader_desc);

  //
  // Freeing NBR data
  // 

  if(nbr_shader.vertex_source) {
    memory_free(nbr_shader.vertex_source);
    memory_free(nbr_shader.pixel_source);
  }
  else {
    memory_free(nbr_shader.compute_source);
  }
  file_close(file); 

  // Some useful info dump

  NIKOLA_LOG_DEBUG("Group \'%s\' pushed shader:", group->name.c_str());
  if(shader_desc.vertex_source) {
    NIKOLA_LOG_DEBUG("     Vertex source length = %zu", strlen(shader_desc.vertex_source));
    NIKOLA_LOG_DEBUG("     Pixel source length  = %zu", strlen(shader_desc.pixel_source));
  }
  else {
    NIKOLA_LOG_DEBUG("     Compute source length = %zu", strlen(shader_desc.compute_source));
  }
  NIKOLA_LOG_DEBUG("     Path = %s", nbr_path.c_str());
  
  // Done!
  return true;
}

static void load_mesh_nbr(ResourceGroup* group, Mesh* mesh, NBRMesh& nbr_mesh) {
  // Loading the data   

  mesh->vertices.assign(nbr_mesh.vertices, nbr_mesh.vertices + nbr_mesh.vertices_count);
  mesh->indices.assign(nbr_mesh.indices, nbr_mesh.indices + nbr_mesh.indices_count);
  
  mesh->material_index = nbr_mesh.material_index;

  // Freeing NBR data
  
  memory_free(nbr_mesh.vertices);
  memory_free(nbr_mesh.indices);
}

static bool load_model_nbr(ResourceGroup* group, Model* model, const FilePath& nbr_path) {
  //
  // Load the NBR model
  // 
  
  NBRHeader header;
  File file;

  if(!open_and_check_nbr_file(group->parent_dir, nbr_path, &file, &header)) {
    file_close(file);
    return false;
  }
  
  if(header.resource_type != RESOURCE_TYPE_MODEL) {
    NIKOLA_LOG_ERROR("Unexpected resource type found in NBR file '\%s\'", nbr_path.c_str());
    return false;
  }

  NBRModel nbr_model;
  file_read_bytes(file, &nbr_model);
 
  //
  // Convert the NBR format to a valid model
  // 

  // Make some space for the arrays for some better performance?
 
  model->meshes.reserve(nbr_model.meshes_count);
  model->materials.reserve(nbr_model.materials_count);
  
  DynamicArray<ResourceID> texture_ids; // @FIX (Resource): This is bad. Don't do this!
  
  // Convert the textures
  
  for(sizei i = 0; i < nbr_model.textures_count; i++) {
    NBRTexture* nbr_texture = &nbr_model.textures[i];

    GfxTextureDesc desc; 
    desc.format    = GFX_TEXTURE_FORMAT_RGBA8; 
    desc.filter    = GFX_TEXTURE_FILTER_MIN_MAG_LINEAR; 
    desc.wrap_mode = GFX_TEXTURE_WRAP_CLAMP;
    desc.width     = nbr_texture->width; 
    desc.height    = nbr_texture->height; 
    desc.depth     = 0; 
    desc.mips      = 1; 
    desc.type      = GFX_TEXTURE_2D; 
    desc.data      = nbr_texture->pixels;
  
    texture_ids.push_back(resources_push_texture(group->id, desc));
  }
  
  // Convert the material 
  
  for(sizei i = 0; i < nbr_model.materials_count; i++) {
    NBRMaterial* nbr_mat = &nbr_model.materials[i];

    Vec3 color = Vec3(nbr_mat->color[0], 
                      nbr_mat->color[1], 
                      nbr_mat->color[2]);
    
    i8 albedo_index    = nbr_mat->albedo_index;
    i8 roughness_index = nbr_mat->roughness_index;
    i8 metallic_index  = nbr_mat->metallic_index;
    i8 normal_index    = nbr_mat->normal_index;
    i8 emissive_index  = nbr_mat->emissive_index;

    MaterialDesc mat_desc = {
      .albedo_id    = albedo_index    != -1 ? texture_ids[albedo_index]    : ResourceID{}, 
      .roughness_id = roughness_index != -1 ? texture_ids[roughness_index] : ResourceID{},
      .metallic_id  = metallic_index  != -1 ? texture_ids[metallic_index]  : ResourceID{},
      .normal_id    = normal_index    != -1 ? texture_ids[normal_index]    : ResourceID{},
      .emissive_id  = emissive_index  != -1 ? texture_ids[emissive_index]  : ResourceID{},

      .color     = color,
      .roughness = nbr_mat->roughness, 
      .metallic  = nbr_mat->metallic, 
      .emissive  = nbr_mat->emissive,
    };
    ResourceID mat_id = resources_push_material(group->id, mat_desc);

    model->materials.push_back(resources_get_material(mat_id)); 
  }
  
  // Convert the meshes 
  
  for(sizei i = 0; i < nbr_model.meshes_count; i++) {
    ResourceID mesh_id = resources_push_mesh(group->id, nbr_model.meshes[i]);
    model->meshes.push_back(resources_get_mesh(mesh_id));
  }

  // Freeing NBR data

  for(sizei i = 0; i < nbr_model.textures_count; i++) {
    memory_free(nbr_model.textures[i].pixels);
  }

  memory_free(nbr_model.meshes);
  memory_free(nbr_model.materials);
  memory_free(nbr_model.textures);
  
  file_close(file); 

  // Some useful info dump

  NIKOLA_LOG_DEBUG("Group \'%s\' pushed model:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Meshes    = %zu", model->meshes.size());
  NIKOLA_LOG_DEBUG("     Materials = %zu", model->materials.size());
  NIKOLA_LOG_DEBUG("     Textures  = %i", nbr_model.textures_count);
  NIKOLA_LOG_DEBUG("     Path      = %s", nbr_path.c_str());
  
  // done!
  return true;
}

static bool load_animation_nbr(ResourceGroup* group, Animation* anim, const FilePath& nbr_path) {
  // 
  // Load the NBR file 
  //
  
  NBRHeader header;
  File file;
  
  if(!open_and_check_nbr_file(group->parent_dir, nbr_path, &file, &header)) {
    file_close(file);
    return false;
  }
 
  if(header.resource_type != RESOURCE_TYPE_ANIMATION) {
    NIKOLA_LOG_ERROR("Unexpected resource type found in NBR file '\%s\'", nbr_path.c_str());
    return false;
  }

  NBRAnimation nbr_anim{};
  file_read_bytes(file, &nbr_anim);

  //
  // Convert the NBR format to a valid model
  // 
  
  anim->joints.reserve(nbr_anim.joints_count);
  for(u16 i = 0; i < nbr_anim.joints_count; i++) {
    Joint* joint        = new Joint{};
    NBRJoint* nbr_joint = &nbr_anim.joints[i];

    // Convert the positions

    joint->position_samples.reserve(nbr_joint->positions_count);
    for(u16 ip = 0; ip < nbr_joint->positions_count; ip += 4) {
      VectorAnimSample sample;

      sample.value.x = nbr_joint->position_samples[ip + 0];
      sample.value.y = nbr_joint->position_samples[ip + 1];
      sample.value.z = nbr_joint->position_samples[ip + 2];
      sample.time    = nbr_joint->position_samples[ip + 3];

      joint->position_samples.push_back(sample);
    }
    
    // Convert the rotations

    joint->rotation_samples.reserve(nbr_joint->rotations_count);
    for(u16 ir = 0; ir < nbr_joint->rotations_count; ir += 5) {
      QuatAnimSample sample;

      sample.value.x = nbr_joint->rotation_samples[ir + 0];
      sample.value.y = nbr_joint->rotation_samples[ir + 1];
      sample.value.z = nbr_joint->rotation_samples[ir + 2];
      sample.value.w = nbr_joint->rotation_samples[ir + 3];
      sample.time    = nbr_joint->rotation_samples[ir + 4];

      joint->rotation_samples.push_back(sample);
    }
    
    // Convert the scales

    joint->scale_samples.reserve(nbr_joint->scales_count);
    for(u16 is = 0; is < nbr_joint->scales_count; is += 4) {
      VectorAnimSample sample;

      sample.value.x = nbr_joint->scale_samples[is + 0];
      sample.value.y = nbr_joint->scale_samples[is + 1];
      sample.value.z = nbr_joint->scale_samples[is + 2];
      sample.time    = nbr_joint->scale_samples[is + 3];

      joint->scale_samples.push_back(sample);
    }

    // Convert other joint information

    joint->parent_index = (i32)nbr_joint->parent_index;
    f32* matrix         = &nbr_joint->inverse_bind_pose[0];

    joint->inverse_bind_pose = Mat4(matrix[0], matrix[4], matrix[8],  0.0f,
                                    matrix[1], matrix[5], matrix[9],  0.0f,
                                    matrix[2], matrix[6], matrix[10], 0.0f,
                                    matrix[3], matrix[7], matrix[11], 1.0f);

    // Transforming the joint with the default values

    transform_translate(joint->current_transform, joint->position_samples[0].value);
    transform_rotate(joint->current_transform, joint->rotation_samples[0].value);
    transform_scale(joint->current_transform, joint->scale_samples[0].value); // @TEMP (Animation)

    // Default initializing the skinning matrix of the joint (IMPORTANT!)
    anim->skinning_palette[i] = joint->current_transform.transform;

    // Welcome, Mr. Joint!
    anim->joints.push_back(joint);
  }

  anim->duration   = nbr_anim.duration;
  anim->frame_rate = nbr_anim.frame_rate;


  //
  // Freeing NBR data
  // 

  for(nikola::sizei i = 0; i < nbr_anim.joints_count; i++) {
    memory_free(nbr_anim.joints[i].position_samples);
    memory_free(nbr_anim.joints[i].rotation_samples);
    memory_free(nbr_anim.joints[i].scale_samples);
  } 

  memory_free(nbr_anim.joints);
  file_close(file); 

  // Some useful info dump

  NIKOLA_LOG_DEBUG("Group \'%s\' pushed animation:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Joints     = %i", nbr_anim.joints_count);
  NIKOLA_LOG_DEBUG("     Duration   = %f", nbr_anim.duration);
  NIKOLA_LOG_DEBUG("     Frame rate = %f", nbr_anim.frame_rate);
  NIKOLA_LOG_DEBUG("     Path       = %s", nbr_path.c_str());
  
  // done!
  return true;
}

static bool load_font_nbr(ResourceGroup* group, Font* font, const FilePath& nbr_path) {
  // 
  // Load the NBR file 
  //
  
  NBRHeader header;
  File file;
  
  if(!open_and_check_nbr_file(group->parent_dir, nbr_path, &file, &header)) {
    file_close(file);
    return false;
  }
 
  if(header.resource_type != RESOURCE_TYPE_FONT) {
    NIKOLA_LOG_ERROR("Unexpected resource type found in NBR file '\%s\'", nbr_path.c_str());
    return false;
  }
  
  NBRFont nbr_font;
  file_read_bytes(file, &nbr_font);
 
  //
  // Convert the NBR format to a valid font
  //
  
  // Import the font information
  
  font->ascent   = (f32)nbr_font.ascent;
  font->descent  = (f32)nbr_font.descent;
  font->line_gap = (f32)nbr_font.line_gap;

  // Import the glyphs 
  
  for(sizei i = 0; i < nbr_font.glyphs_count; i++) {
    Glyph glyph;

    glyph.unicode = nbr_font.glyphs[i].unicode;

    glyph.size.x = nbr_font.glyphs[i].width;
    glyph.size.y = nbr_font.glyphs[i].height;
    
    glyph.offset.x = nbr_font.glyphs[i].offset_x;
    glyph.offset.y = nbr_font.glyphs[i].offset_y;
    
    glyph.left   = nbr_font.glyphs[i].left;
    glyph.top    = nbr_font.glyphs[i].top;
    glyph.right  = nbr_font.glyphs[i].right;
    glyph.bottom = nbr_font.glyphs[i].bottom;
    
    glyph.advance_x    = nbr_font.glyphs[i].advance_x;
    glyph.kern         = nbr_font.glyphs[i].kern;
    glyph.left_bearing = nbr_font.glyphs[i].left_bearing;

    // We don't care about glyphs that have a "non-size"
    if(glyph.size.x <= 0) {
      continue;
    }
  
    // Importing the texture
    
    GfxTextureDesc face_desc {
      .width  = (u32)nbr_font.glyphs[i].width,
      .height = (u32)nbr_font.glyphs[i].height,
      .depth  = 0, 
      .mips   = 1,

      .type      = GFX_TEXTURE_2D, 
      .format    = GFX_TEXTURE_FORMAT_R8, 
      .filter    = GFX_TEXTURE_FILTER_MIN_MAG_LINEAR, 
      .wrap_mode = GFX_TEXTURE_WRAP_CLAMP,

      .is_bindless = false,
      .data        = (void*)nbr_font.glyphs[i].pixels,
    };
    glyph.texture = resources_get_texture(resources_push_texture(group->id, face_desc));

    font->glyphs[glyph.unicode] = glyph;
  }

  //
  // Freeing NBR data
  //

  for(u32 i = 0; i < nbr_font.glyphs_count; i++) {
    memory_free(nbr_font.glyphs[i].pixels);
  }

  memory_free(nbr_font.glyphs);
  
  file_close(file); 

  // Some useful info dump
  
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed font:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Glyphs   = %zu", font->glyphs.size());
  NIKOLA_LOG_DEBUG("     Ascent   = %0.3f", font->ascent);
  NIKOLA_LOG_DEBUG("     Descent  = %0.3f", font->descent);
  NIKOLA_LOG_DEBUG("     Line gap = %0.3f", font->line_gap);
  NIKOLA_LOG_DEBUG("     Path     = %s", nbr_path.c_str());

  // Done!
  return true;
}

static bool load_audio_nbr(ResourceGroup* group, AudioBufferDesc* desc, const FilePath& nbr_path) {
  //
  // Load the NBR file
  // 
  
  NBRHeader header;
  File file;
  
  if(!open_and_check_nbr_file(group->parent_dir, nbr_path, &file, &header)) {
    file_close(file);
    return false;
  }
  
  if(header.resource_type != RESOURCE_TYPE_AUDIO_BUFFER) {
    NIKOLA_LOG_ERROR("Unexpected resource type found in NBR file '\%s\'", nbr_path.c_str());
    return false;
  }
  
  NBRAudio nbr_audio;
  file_read_bytes(file, &nbr_audio);

  //
  // Convert the NBR format to a valid audio buffer desc
  //
  
  desc->format      = (AudioBufferFormat)nbr_audio.format; 
  desc->channels    = nbr_audio.channels; 
  desc->sample_rate = nbr_audio.sample_rate;
  desc->size        = nbr_audio.size;
  desc->data        = (void*)nbr_audio.samples;

  // 
  // Freeing NBR data
  // 

  memory_free(nbr_audio.samples);
  file_close(file); 
 
  // Some useful info dump
  NIKOLA_LOG_DEBUG("     Name        = %s", filepath_stem(nbr_path).c_str());

  // Done!
  return true;
}

/// Private functions 
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Callbacks

static void resource_entry_iterate(const FilePath& base, const FilePath& path, void* user_data) {
  ResourceGroup* group = (ResourceGroup*)user_data;

  if(!filesystem_exists(path)) {
    NIKOLA_LOG_ERROR("Cannot push non-existent resource at \'%s\'", path.c_str());
    return;
  }
  
  // Load the NBR header
  NBRHeader header;
  File file;
  if(!open_and_check_nbr_file(base, filepath_filename(path), &file, &header)) {
    return;
  }
  file_close(file);

  switch (header.resource_type) {
    case RESOURCE_TYPE_TEXTURE:
      resources_push_texture(group->id, path);
      break;
    case RESOURCE_TYPE_CUBEMAP:
      resources_push_cubemap(group->id, path);
      break;
    case RESOURCE_TYPE_SHADER:
      resources_push_shader(group->id, path);
      break;
    case RESOURCE_TYPE_MODEL:
      resources_push_model(group->id, path);
      break;
    case RESOURCE_TYPE_ANIMATION:
      resources_push_animation(group->id, path);
      break;
    case RESOURCE_TYPE_FONT:
      resources_push_font(group->id, path);
      break;
    case RESOURCE_TYPE_AUDIO_BUFFER:
      resources_push_audio_buffer(group->id, path);
      break;
    default:
      NIKOLA_LOG_ERROR("Invalid resource type \'%s\'", path.c_str());
      break;
  }
}

static void resource_entry_update(const FileStatus status, const FilePath& path, void* user_data) {
  // We only care if the resource was modified 
  
  if(status != FILE_STATUS_MODIFIED) {
    return;
  }
  
  // Load the NBR header
  
  NBRHeader header;
  File file;
  if(!open_and_check_nbr_file(path, "", &file, &header)) {
    return;
  }
 
  // Get the stem of the path to identify the resource
  
  FilePath filename = filepath_stem(path); 

  ResourceGroup* group = (ResourceGroup*)user_data;
  ResourceID res_id    = resources_get_id(group->id, filename);

  // Reload the the resource based on its type
  
  switch (header.resource_type) {
    case RESOURCE_TYPE_TEXTURE:
      load_texture_nbr(group, resources_get_texture(res_id), path);
      break;
    case RESOURCE_TYPE_CUBEMAP:
      load_cubemap_nbr(group, resources_get_cubemap(res_id), path);
      break;
    case RESOURCE_TYPE_SHADER:
      load_shader_nbr(group, resources_get_shader(res_id), path);
      break;
    case RESOURCE_TYPE_MODEL:
      // @TODO (Resource)
      break;
    case RESOURCE_TYPE_ANIMATION:
      // @TODO (Resource)
      break;
    case RESOURCE_TYPE_FONT:
      // @TODO (Resource)
      break;
    case RESOURCE_TYPE_AUDIO_BUFFER:
      // @TODO (Resource)
      break;
    default:
      NIKOLA_LOG_ERROR("Unsupported resource type for reloading");
      break;
  }

  file_close(file);
}

/// Callbacks
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Resource manager functions

void resource_manager_init() {
  s_manager.groups[RESOURCE_CACHE_ID] = ResourceGroup {
    .name       = "cache", 
    .parent_dir = "resource_cache",
    .id         = RESOURCE_CACHE_ID,
  };

  NIKOLA_LOG_INFO("Successfully initialized the resource manager");
}

void resource_manager_shutdown() {
  resources_destroy_group(RESOURCE_CACHE_ID);
  NIKOLA_LOG_INFO("Successfully shutdown the resource manager");
}

u16 resources_create_group(const String& name, const FilePath& parent_dir) {
  ResourceGroupID group_id   = random_u32(RESOURCE_CACHE_ID + 1, RESOURCE_GROUP_INVALID - 1); 
  s_manager.groups[group_id] = ResourceGroup {
    .name       = name, 
    .parent_dir = parent_dir,
    .id         = group_id,
  };

  // Create the parent directory if it doesn't exist
  if(!filesystem_exists(parent_dir)) {
    filesystem_create_directory(parent_dir);
  }

  // Add a file watcher to the parent directory
  filewatcher_add_dir(parent_dir, resource_entry_update, &s_manager.groups[group_id]);

  NIKOLA_LOG_INFO("Successfully created a resource group \'%s\' at \'%s\'", name.c_str(), parent_dir.c_str());
  return group_id;
}

void resources_clear_group(const ResourceGroupID& group_id) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  group->buffers.clear();
  group->textures.clear();
  group->cubemaps.clear();
  group->shaders.clear();
  group->audio_buffers.clear();

  group->meshes.clear();
  group->materials.clear();
  group->shader_contexts.clear();
  group->skyboxes.clear();
  group->models.clear();
  group->animations.clear();
  group->fonts.clear();
  
  NIKOLA_LOG_INFO("Resource group \'%s\' was successfully cleared", group->name.c_str());
}

void resources_destroy_group(const ResourceGroupID& group_id) {
  if(group_id == RESOURCE_GROUP_INVALID) {
    NIKOLA_LOG_WARN("Cannot destroy an invalid resource group");
    return;
  }

  ResourceGroup* group = &s_manager.groups[group_id];

  // Destroy compound resources
  
  DESTROY_COMP_RESOURCE_MAP(group, meshes);
  DESTROY_COMP_RESOURCE_MAP(group, materials);
  DESTROY_COMP_RESOURCE_MAP(group, shader_contexts);
  DESTROY_COMP_RESOURCE_MAP(group, skyboxes);
  DESTROY_COMP_RESOURCE_MAP(group, models);
  DESTROY_COMP_RESOURCE_MAP(group, fonts);

  // @TODO (Resource): No. 
  for(auto& anim : group->animations) {
    for(auto& joint : anim->joints) {
      joint->position_samples.clear();
      joint->rotation_samples.clear();
      joint->scale_samples.clear();

      delete joint;
    }

    delete anim;
  }

  // Destroy core resources
  
  DESTROY_CORE_RESOURCE_MAP(group, buffers, gfx_buffer_destroy);
  DESTROY_CORE_RESOURCE_MAP(group, textures, gfx_texture_destroy);
  DESTROY_CORE_RESOURCE_MAP(group, cubemaps, gfx_cubemap_destroy);
  DESTROY_CORE_RESOURCE_MAP(group, shaders, gfx_shader_destroy);
  DESTROY_CORE_RESOURCE_MAP(group, audio_buffers, audio_buffer_destroy);

  NIKOLA_LOG_INFO("Resource group \'%s\' was successfully destroyed", group->name.c_str());
  s_manager.groups.erase(group_id);
}

ResourceID resources_push_buffer(const ResourceGroupID& group_id, const GfxBufferDesc& buff_desc) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Create the buffer

  GfxBuffer* buffer = gfx_buffer_create(renderer_get_context());

  ResourceID id; 
  PUSH_RESOURCE(group, buffers, buffer, RESOURCE_TYPE_BUFFER, id);

  // Load the buffer's data
  gfx_buffer_load(buffer, buff_desc);

  NIKOLA_LOG_DEBUG("Group \'%s\' pushed buffer:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Size = %zu", buff_desc.size);
  NIKOLA_LOG_DEBUG("     Type = %s", buffer_type_str(buff_desc.type));
  return id;
}

ResourceID resources_push_texture(const ResourceGroupID& group_id, const GfxTextureDesc& desc) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Create and push the texture
  
  GfxTexture* texture = gfx_texture_create(renderer_get_context(), desc.type);

  ResourceID id; 
  PUSH_RESOURCE(group, textures, texture, RESOURCE_TYPE_TEXTURE, id);
  
  gfx_texture_load(texture, desc);

  NIKOLA_LOG_DEBUG("Group \'%s\' pushed texture:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Size = %i X %i", desc.width, desc.height);
  NIKOLA_LOG_DEBUG("     Type = %s", texture_type_str(desc.type));
  return id;
}

ResourceID resources_push_texture(const ResourceGroupID& group_id, const FilePath& nbr_path) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Create the texture
 
  GfxTexture* texture = gfx_texture_create(renderer_get_context(), GFX_TEXTURE_2D);

  ResourceID id; 
  PUSH_RESOURCE(group, textures, texture, RESOURCE_TYPE_TEXTURE, id);
  
  // Add the resource to the named resources
  group->named_ids[filepath_stem(nbr_path)] = id;
  
  load_texture_nbr(group, texture, nbr_path);
  return id;
}

ResourceID resources_push_texture(const ResourceGroupID& group_id, const MaterialTextureType& type) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  GfxTextureDesc tex_desc = {
    .width  = 16, 
    .height = 16,
    .depth  = 0, 
    .mips   = 1, 

    .type      = GFX_TEXTURE_2D,
    .format    = GFX_TEXTURE_FORMAT_RGBA8, 
    .filter    = GFX_TEXTURE_FILTER_MIN_MAG_NEAREST, 
    .wrap_mode = GFX_TEXTURE_WRAP_MIRROR, 
  };

  u8 pixels[1024]; // width * height * channels

  switch(type) {
    case MATERIAL_TEXTURE_ALBEDO: 
    case MATERIAL_TEXTURE_ROUGHNESS: 
    case MATERIAL_TEXTURE_METALLIC: 
      memory_set(pixels, 0xffffffff, sizeof(pixels));
      break;
    case MATERIAL_TEXTURE_EMISSIVE: 
      memory_set(pixels, 0, sizeof(pixels));
      break;
    case MATERIAL_TEXTURE_NORMAL: {
      // Initializing the R and G channels to 0 
      // and the B and A pixels to 1. That way, 
      // normal textures look towards the camera 
      // with complete transparency.
      
      for(u64 i = 0; i < 16; i++) {
        for(u64 j = 0; j < 16; j++) {
          u64 index = (i * 16) + j;
          u64 pixel = index * 4; // Index * channels

          pixels[pixel + 0] = 0;
          pixels[pixel + 1] = 0;
          pixels[pixel + 2] = 255;
          pixels[pixel + 3] = 255;
        }
      }
    } break;
  }

  tex_desc.data = &pixels;

  return resources_push_texture(group_id, tex_desc);
}

ResourceID resources_push_cubemap(const ResourceGroupID& group_id, const GfxCubemapDesc& cubemap_desc) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Create and push the cubemap
  
  GfxCubemap* cubemap = gfx_cubemap_create(renderer_get_context());

  ResourceID id; 
  PUSH_RESOURCE(group, cubemaps, cubemap, RESOURCE_TYPE_CUBEMAP, id);
 
  // Load the cubemap's data
  gfx_cubemap_load(cubemap, cubemap_desc); 
  
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed cubemap:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Size  = %i X %i", cubemap_desc.width, cubemap_desc.height);
  NIKOLA_LOG_DEBUG("     Faces = %i", cubemap_desc.faces_count);
  return id;
}

ResourceID resources_push_cubemap(const ResourceGroupID& group_id, const FilePath& nbr_path) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Create the cubemap
  
  GfxCubemap* cubemap = gfx_cubemap_create(renderer_get_context());

  ResourceID id; 
  PUSH_RESOURCE(group, cubemaps, cubemap, RESOURCE_TYPE_CUBEMAP, id);

  // Add the resource to the named resources
  group->named_ids[filepath_stem(nbr_path)] = id;

  load_cubemap_nbr(group, cubemap, nbr_path); 
  return id;
}

ResourceID resources_push_shader(const ResourceGroupID& group_id, const GfxShaderDesc& shader_desc) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Create and load the shader
  
  GfxShader* shader = gfx_shader_create(renderer_get_context());
  gfx_shader_load(shader, shader_desc); 

  // New shader!

  ResourceID id; 
  PUSH_RESOURCE(group, shaders, shader, RESOURCE_TYPE_SHADER, id);
  
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed shader:", group->name.c_str());
  if(shader_desc.vertex_source) {
    NIKOLA_LOG_DEBUG("     Vertex source length = %zu", strlen(shader_desc.vertex_source));
    NIKOLA_LOG_DEBUG("     Pixel source length  = %zu", strlen(shader_desc.pixel_source));
  }
  else {
    NIKOLA_LOG_DEBUG("     Compute source length = %zu", strlen(shader_desc.compute_source));
  }
  return id;
}

ResourceID resources_push_shader(const ResourceGroupID& group_id, const FilePath& nbr_path) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Create and load the shader
  
  GfxShader* shader = gfx_shader_create(renderer_get_context());
  load_shader_nbr(group, shader, nbr_path);
 
  // New shader loaded!

  ResourceID id; 
  PUSH_RESOURCE(group, shaders, shader, RESOURCE_TYPE_SHADER, id);
  
  group->named_ids[filepath_stem(nbr_path)] = id;
  
  return id;
}

ResourceID resources_push_shader_context(const ResourceGroupID& group_id, const ResourceID& shader_id) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
  
  // Allocate the context
  
  ShaderContext* ctx = new ShaderContext{};
  ctx->shader        = resources_get_shader(shader_id);

  // Create the context
  
  ResourceID id; 
  PUSH_RESOURCE(group, shader_contexts, ctx, RESOURCE_TYPE_SHADER_CONTEXT, id);

  // Query the shader for uniform information

  GfxShaderQueryDesc query_desc = {};
  gfx_shader_query(ctx->shader, &query_desc);

  for(sizei i = 0; i < query_desc.uniforms_count; i++) {
    GfxUniformDesc* uniform = &query_desc.active_uniforms[i];
    if(uniform->location == -1) { // Invalid uniform. Why??
      continue;
    }
    
    ctx->uniforms_cache[uniform->name] = uniform->location;
  }

  // Set a default matrices buffer 
  
  GfxBuffer* matrix_buffer = renderer_get_defaults().matrices_buffer;
  shader_context_set_uniform_buffer(ctx, SHADER_MATRICES_BUFFER_INDEX, matrix_buffer);

  // New context added!
  
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed shader context:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Attributes count      = %i", query_desc.attributes_count);
  NIKOLA_LOG_DEBUG("     Uniforms count        = %i", query_desc.uniforms_count);
  NIKOLA_LOG_DEBUG("     Uniform buffers count = %i", query_desc.uniform_blocks_count);
  return id;
}

ResourceID resources_push_shader_context(const ResourceGroupID& group_id, const FilePath& shader_path) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
 
  // Get the shader first
  ResourceID shader_id = resources_push_shader(group_id, shader_path);

  // New context added!
  return resources_push_shader_context(group_id, shader_id);
}

ResourceID resources_push_mesh(const ResourceGroupID& group_id, NBRMesh& nbr_mesh) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Allocate and load the mesh
  
  Mesh* mesh = new Mesh{};
  load_mesh_nbr(group, mesh, nbr_mesh);
  
  // New mesh added!
  
  ResourceID id; 
  PUSH_RESOURCE(group, meshes, mesh, RESOURCE_TYPE_MESH, id);
  
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed mesh:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Vertices = %zu", mesh->vertices.size());
  NIKOLA_LOG_DEBUG("     Indices  = %zu", mesh->indices.size());
  return id;
}

ResourceID resources_push_mesh(const ResourceGroupID& group_id, const GeometryType type) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
  
  // Allocate and load the mesh
  
  Mesh* mesh = new Mesh{};
  geometry_loader_load(mesh->vertices, mesh->indices, type);

  // New mesh added!
  
  ResourceID id; 
  PUSH_RESOURCE(group, meshes, mesh, RESOURCE_TYPE_MESH, id);

  NIKOLA_LOG_DEBUG("Group \'%s\' pushed mesh:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Vertices      = %zu", mesh->vertices.size());
  NIKOLA_LOG_DEBUG("     Indices       = %zu", mesh->indices.size());
  NIKOLA_LOG_DEBUG("     Geomatry type = %s", geo_type_str(type));
  
  return id;
}

ResourceID resources_push_material(const ResourceGroupID& group_id, const MaterialDesc& desc) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
  
  // Allocate the material
  Material* material = new Material{};
  
  material->albedo_map    = renderer_get_defaults().albedo_texture;
  material->roughness_map = renderer_get_defaults().roughness_texture;
  material->metallic_map  = renderer_get_defaults().metallic_texture;
  material->normal_map    = renderer_get_defaults().normal_texture;
  material->emissive_map  = renderer_get_defaults().emissive_texture;

  material->color        = desc.color;
  material->roughness    = desc.roughness;
  material->metallic     = desc.metallic;
  material->emissive     = desc.emissive;
  material->transparency = desc.transparency;

  material->depth_mask  = desc.depth_mask;
  material->stencil_ref = desc.stencil_ref;

  // Textures init
  
  if(RESOURCE_IS_VALID(desc.albedo_id)) {
    material->albedo_map  = resources_get_texture(desc.albedo_id);
    material->map_flags  |= MATERIAL_TEXTURE_ALBEDO;
  }
  
  if(RESOURCE_IS_VALID(desc.roughness_id)) {
    material->roughness_map = resources_get_texture(desc.roughness_id);
    material->map_flags    |= MATERIAL_TEXTURE_ROUGHNESS;
  }
  
  if(RESOURCE_IS_VALID(desc.metallic_id)) {
    material->metallic_map  = resources_get_texture(desc.metallic_id);
    material->map_flags    |= MATERIAL_TEXTURE_METALLIC;
  }
  
  if(RESOURCE_IS_VALID(desc.normal_id)) {
    material->normal_map = resources_get_texture(desc.normal_id);
    material->map_flags |= MATERIAL_TEXTURE_NORMAL;
  }
  
  if(RESOURCE_IS_VALID(desc.emissive_id)) {
    material->emissive_map = resources_get_texture(desc.emissive_id);
    material->map_flags   |= MATERIAL_TEXTURE_EMISSIVE;
  }

  // Create material
  ResourceID id;
  PUSH_RESOURCE(group, materials, material, RESOURCE_TYPE_MATERIAL, id);

  // New material added
  NIKOLA_LOG_DEBUG("Group \'%s\' pushed material:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Color        = \'%s\'", vec3_to_string(material->color).c_str());
  NIKOLA_LOG_DEBUG("     Roughness    = \'%f\'", material->roughness);
  NIKOLA_LOG_DEBUG("     Metallic     = \'%f\'", material->metallic);
  NIKOLA_LOG_DEBUG("     Emissive     = \'%f\'", material->emissive);
  NIKOLA_LOG_DEBUG("     Transparency = \'%f\'", material->transparency);
  return id;
}

ResourceID resources_push_skybox(const ResourceGroupID& group_id, const ResourceID& cubemap_id) {
  NIKOLA_ASSERT(RESOURCE_IS_VALID(cubemap_id), "Cannot push a new skybox with an invalid cubemap");
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Allocate and load the skybox

  DynamicArray<u32> dummy_indices; // @TEMP (Resources)

  Skybox* skybox = new Skybox{};
  geometry_loader_load(skybox->vertices, dummy_indices, GEOMETRY_SKYBOX);

  // Set the cubemap
  skybox->cubemap = resources_get_cubemap(cubemap_id);

  // New skybox added!
  
  ResourceID id;
  PUSH_RESOURCE(group, skyboxes, skybox, RESOURCE_TYPE_SKYBOX, id);

  NIKOLA_LOG_DEBUG("Group \'%s\' pushed skybox:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Vertices = %zu", skybox->vertices.size());
  
  return id;
}

ResourceID resources_push_skybox(const ResourceGroupID& group_id, const FilePath& cubemap_path) {
  // New skybox added!
  
  ResourceID cubemap_id = resources_push_cubemap(group_id, cubemap_path);
  return resources_push_skybox(group_id, cubemap_id);
}

ResourceID resources_push_model(const ResourceGroupID& group_id, const FilePath& nbr_path) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Allocate the model
  Model* model = new Model{};

  // Load the NBR data into the model
  
  if(!load_model_nbr(group, model, nbr_path)) {
    delete model;
    NIKOLA_LOG_ERROR("Failed to load NBR model file at \'%s\'", nbr_path.c_str());

    return ResourceID{};
  }

  // New model added!
  
  ResourceID id;
  PUSH_RESOURCE(group, models, model, RESOURCE_TYPE_MODEL, id);

  group->named_ids[filepath_stem(nbr_path)] = id;

  return id;
}

ResourceID resources_push_animation(const ResourceGroupID& group_id, const FilePath& nbr_path) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Allocate the animation
  Animation* anim = new Animation{};
  
  // Load the NBR data into the animation
 
  if(!load_animation_nbr(group, anim, nbr_path)) {
    delete anim;
    NIKOLA_LOG_ERROR("Failed to load NBR animation file at \'%s\'", nbr_path.c_str());

    return ResourceID{};
  }
  
  // New animation added!
  
  ResourceID id;
  PUSH_RESOURCE(group, animations, anim, RESOURCE_TYPE_ANIMATION, id);
  
  group->named_ids[filepath_stem(nbr_path)] = id;

  return id;
}

ResourceID resources_push_font(const ResourceGroupID& group_id, const FilePath& nbr_path) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Allocate the model
  Font* font = new Font{};

  // Load the NBR data into the font

  if(!load_font_nbr(group, font, nbr_path)) {
    delete font;
    NIKOLA_LOG_ERROR("Failed to load NBR font at \'%s\'", nbr_path.c_str());

    return ResourceID{};
  }

  // New font added!
  
  ResourceID id;
  PUSH_RESOURCE(group, fonts, font, RESOURCE_TYPE_FONT, id);
  
  group->named_ids[filepath_stem(nbr_path)] = id;

  return id;
}

ResourceID resources_push_audio_buffer(const ResourceGroupID& group_id, const AudioBufferDesc& desc) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Create a new audio buffer
  AudioBufferID buffer = audio_buffer_create(desc);

  // New audio buffer added!
  
  ResourceID id;
  PUSH_RESOURCE(group, audio_buffers, buffer, RESOURCE_TYPE_AUDIO_BUFFER, id);

  NIKOLA_LOG_DEBUG("Group \'%s\' pushed an audio buffer:", group->name.c_str());
  NIKOLA_LOG_DEBUG("     Format      = %s", audio_format_str(desc.format));
  NIKOLA_LOG_DEBUG("     Channels    = %i", desc.channels);
  NIKOLA_LOG_DEBUG("     Size        = %zu", desc.size);
  NIKOLA_LOG_DEBUG("     Sample Rate = %zu", desc.sample_rate);
  return id;
}

ResourceID resources_push_audio_buffer(const ResourceGroupID& group_id, const FilePath& nbr_path) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];

  // Load the NBR data into the audio buffer

  AudioBufferDesc desc = {};
  if(!load_audio_nbr(group, &desc, nbr_path)) {
    NIKOLA_LOG_ERROR("Failed to load NBR audio buffer at \'%s\'", nbr_path.c_str());
    return ResourceID{};
  }

  // New audio buffer added!
 
  ResourceID id = resources_push_audio_buffer(group_id, desc);
  group->named_ids[filepath_stem(nbr_path)] = id;
  
  return id;
}

void resources_push_dir(const ResourceGroupID& group_id, const FilePath& dir, const bool async) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
 
  // Retrieve all of the paths
  filesystem_directory_iterate(filepath_append(group->parent_dir, dir), resource_entry_iterate, group);
}

ResourceID& resources_get_id(const ResourceGroupID& group_id, const nikola::String& filename) {
  GROUP_CHECK(group_id);
  ResourceGroup* group = &s_manager.groups[group_id];
 
  // The resource was not found
  
  if(group->named_ids.find(filename) == group->named_ids.end()) {
    NIKOLA_LOG_ERROR("Could not find resource \'%s\' in resource group \'%s\'", filename.c_str(), group->name.c_str());
    return group->named_ids["invalid"];
  }

  return group->named_ids[filename];
}

GfxBuffer* resources_get_buffer(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->buffers, RESOURCE_TYPE_BUFFER);
}

GfxTexture* resources_get_texture(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->textures, RESOURCE_TYPE_TEXTURE);
}

GfxCubemap* resources_get_cubemap(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->cubemaps, RESOURCE_TYPE_CUBEMAP);
}

GfxShader* resources_get_shader(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->shaders, RESOURCE_TYPE_SHADER);
}

ShaderContext* resources_get_shader_context(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->shader_contexts, RESOURCE_TYPE_SHADER_CONTEXT);
}

Mesh* resources_get_mesh(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->meshes, RESOURCE_TYPE_MESH);
}

Material* resources_get_material(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->materials, RESOURCE_TYPE_MATERIAL);
}

Skybox* resources_get_skybox(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->skyboxes, RESOURCE_TYPE_SKYBOX);
}

Model* resources_get_model(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->models, RESOURCE_TYPE_MODEL);
}

Animation* resources_get_animation(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->animations, RESOURCE_TYPE_ANIMATION);
}

Font* resources_get_font(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->fonts, RESOURCE_TYPE_FONT);
}

AudioBufferID resources_get_audio_buffer(const ResourceID& id) {
  ResourceGroup* group = &s_manager.groups[id.group];
  return get_resource(id, group->audio_buffers, RESOURCE_TYPE_AUDIO_BUFFER);
}

/// Resource manager functions
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
