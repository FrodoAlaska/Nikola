#include "nikola/nikola_resources.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_gfx.h"
#include "nikola/nikola_file.h"
#include "nikola/nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola 

///---------------------------------------------------------------------------------------------------------------------
/// NBR importer functions

void nbr_import_texture(NBRTexture* nbr, GfxTextureDesc* desc) {
  NIKOLA_ASSERT(nbr, "Invalid NBRTexture while importing");
  NIKOLA_ASSERT(desc, "Invalid GfxTextureDesc while importing");

  desc->width  = nbr->width; 
  desc->height = nbr->height; 
  desc->depth  = 0; 
  desc->mips   = 1; 
  desc->type   = GFX_TEXTURE_2D; 
  desc->data   = nbr->pixels;
}

void nbr_import_cubemap(NBRCubemap* nbr, GfxCubemapDesc* desc) {
  NIKOLA_ASSERT(nbr, "Invalid NBRCubemap while importing");
  NIKOLA_ASSERT(desc, "Invalid GfxCubemapDesc while importing");
  
  desc->width       = nbr->width; 
  desc->height      = nbr->height; 
  desc->mips        = 1; 
  desc->faces_count = nbr->faces_count; 

  for(sizei i = 0; i < desc->faces_count; i++) {
    desc->data[i] = nbr->pixels[i];
  }
}

void nbr_import_shader(NBRShader* nbr, GfxShaderDesc* desc) {
  NIKOLA_ASSERT(nbr, "Invalid NBRShader while importing");
  NIKOLA_ASSERT(desc, "Invalid GfxShaderDesc while importing");

  desc->vertex_source = nbr->vertex_source;
  desc->pixel_source  = nbr->pixel_source;
}

void nbr_import_mesh(NBRMesh* nbr, const u16 group_id, Mesh* mesh) {
  NIKOLA_ASSERT(nbr, "Invalid NBRMesh while importing");
  NIKOLA_ASSERT(mesh, "Invalid Mesh while importing");
  
  // Default initialize the loader
  mesh->pipe_desc = {}; 
  
  // Create a vertex buffer
  GfxBufferDesc buff_desc = {
    .data  = (void*)nbr->vertices,
    .size  = nbr->vertices_count * sizeof(f32), 
    .type  = GFX_BUFFER_VERTEX, 
    .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  mesh->vertex_buffer = resources_push_buffer(group_id, buff_desc);
    
  // Create a index buffer
  buff_desc = {
    .data  = (void*)nbr->indices,
    .size  = nbr->indices_count * sizeof(u32), 
    .type  = GFX_BUFFER_INDEX, 
    .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  mesh->index_buffer = resources_push_buffer(group_id, buff_desc);

  // Vertex buffer init 
  mesh->pipe_desc.vertex_buffer  = resources_get_buffer(mesh->vertex_buffer);
  mesh->pipe_desc.vertices_count = nbr->vertices_count;  
  
  // Index buffer init
  mesh->pipe_desc.index_buffer  = resources_get_buffer(mesh->index_buffer);
  mesh->pipe_desc.indices_count = nbr->indices_count;  

  // Layout init
  vertex_type_layout((VertexType)nbr->vertex_type, mesh->pipe_desc.layout, &mesh->pipe_desc.layout_count);
  
  // Draw mode init
  mesh->pipe_desc.draw_mode = GFX_DRAW_MODE_TRIANGLE;
}

void nbr_import_material(NBRMaterial* nbr, const u16 group_id, Material* material) {
  NIKOLA_ASSERT(nbr, "Invalid NBRMaterial while importing");
  NIKOLA_ASSERT(material, "Invalid Material while importing");

  // Default values for the textures
  material->specular_map = ResourceID{};
  
  material->ambient_color  = Vec3(nbr->ambient[0], nbr->ambient[1], nbr->ambient[2]); 
  material->diffuse_color  = Vec3(nbr->diffuse[0], nbr->diffuse[1], nbr->diffuse[2]); 
  material->specular_color = Vec3(nbr->specular[0], nbr->specular[1], nbr->specular[2]); 
}

void nbr_import_model(NBRModel* nbr, const u16 group_id, Model* model) {
  NIKOLA_ASSERT(nbr, "Invalid NBRModel while importing");
  NIKOLA_ASSERT(model, "Invalid Model while importing");
  
  // Make some space for the arrays for some better performance  
  model->meshes.reserve(nbr->meshes_count);
  model->materials.reserve(nbr->materials_count);
  model->material_indices.reserve(nbr->meshes_count);
  
  DynamicArray<ResourceID> texture_ids; // @FIX (Resource): This is bad. Don't do this!

  // Convert the textures
  for(sizei i = 0; i < nbr->textures_count; i++) {
    GfxTextureDesc desc; 
    desc.format    = GFX_TEXTURE_FORMAT_RGBA8; 
    desc.filter    = GFX_TEXTURE_FILTER_MIN_MAG_LINEAR; 
    desc.wrap_mode = GFX_TEXTURE_WRAP_CLAMP;
    nbr_import_texture(&nbr->textures[i], &desc);
  
    texture_ids.push_back(resources_push_texture(group_id, desc));
  }
  
  // Convert the material 
  for(sizei i = 0; i < nbr->materials_count; i++) {
    // Create a new material 
    ResourceID mat_id = resources_push_material(group_id);
    Material* mat     = resources_get_material(mat_id);

    // Default values for the textures
    mat->diffuse_map  = ResourceID{};
    mat->specular_map = ResourceID{};

    // Insert a valid diffuse texture 
    mat->diffuse_map = texture_ids[nbr->materials[i].diffuse_index];
    
    // Insert a valid specular texture 
    i8 specular_index = nbr->materials[i].specular_index;
    if(specular_index != -1) {
      mat->specular_map = texture_ids[specular_index];
    }

    // Convert the NBRMaterial into an engine Material
    nbr_import_material(&nbr->materials[i], group_id, mat);

    // Add the material 
    model->materials.push_back(mat_id); 
  }
  
  // Convert the vertices 
  for(sizei i = 0; i < nbr->meshes_count; i++) {
    // Create and add a new mesh
    model->meshes.push_back(resources_push_mesh(group_id, nbr->meshes[i]));

    // Add a new index
    model->material_indices.push_back(nbr->meshes[i].material_index);
  }
}

void nbr_import_font(NBRFont* nbr, const u16 group_id, Font* font) {
  NIKOLA_ASSERT(nbr, "Invalid NBRFont while importing");
  NIKOLA_ASSERT(font, "Invalid Font while importing");

  // Import the font information
  font->glyphs.resize(nbr->glyphs_count); 
  font->ascent   = (f32)nbr->ascent;
  font->descent  = (f32)nbr->descent;
  font->line_gap = (f32)nbr->line_gap;

  // Import the glyphs 
  for(sizei i = 0; i < font->glyphs.size(); i++) {
    font->glyphs[i].unicode = nbr->glyphs[i].unicode;

    font->glyphs[i].size.x = nbr->glyphs[i].width;
    font->glyphs[i].size.y = nbr->glyphs[i].height;
    
    font->glyphs[i].offset.x = nbr->glyphs[i].offset_x;
    font->glyphs[i].offset.y = nbr->glyphs[i].offset_y;
    
    font->glyphs[i].left   = nbr->glyphs[i].left;
    font->glyphs[i].top    = nbr->glyphs[i].top;
    font->glyphs[i].right  = nbr->glyphs[i].right;
    font->glyphs[i].bottom = nbr->glyphs[i].bottom;
    
    font->glyphs[i].advance_x    = nbr->glyphs[i].advance_x;
    font->glyphs[i].kern         = nbr->glyphs[i].kern;
    font->glyphs[i].left_bearing = nbr->glyphs[i].left_bearing;
  }
}

/// NBR importer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
