#include "nikola/nikola_resources.h"
#include "nikola/nikola_base.h"
#include "nikola/nikola_gfx.h"
#include "nikola/nikola_file.h"
#include "nikola/nikola_math.h"
#include "nikola/nikola_audio.h"

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

void nbr_import_mesh(NBRMesh* nbr, const ResourceGroupID& group_id, Mesh* mesh) {
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
  mesh->vertex_buffer = resources_get_buffer(resources_push_buffer(group_id, buff_desc));
    
  // Create a index buffer
  buff_desc = {
    .data  = (void*)nbr->indices,
    .size  = nbr->indices_count * sizeof(u32), 
    .type  = GFX_BUFFER_INDEX, 
    .usage = GFX_BUFFER_USAGE_STATIC_DRAW,
  };
  mesh->index_buffer = resources_get_buffer(resources_push_buffer(group_id, buff_desc));

  // Vertex buffer init 
  mesh->pipe_desc.vertex_buffer  = mesh->vertex_buffer;
  mesh->pipe_desc.vertices_count = nbr->vertices_count;  
  
  // Index buffer init
  mesh->pipe_desc.index_buffer  = mesh->index_buffer;
  mesh->pipe_desc.indices_count = nbr->indices_count;  

  // Layout init
  vertex_type_layout((VertexType)nbr->vertex_type, mesh->pipe_desc.layout, &mesh->pipe_desc.layout_count);
  
  // Draw mode init
  mesh->pipe_desc.draw_mode = GFX_DRAW_MODE_TRIANGLE;
}

void nbr_import_material(NBRMaterial* nbr, const ResourceGroupID& group_id, Material* material) {
  NIKOLA_ASSERT(nbr, "Invalid NBRMaterial while importing");
  NIKOLA_ASSERT(material, "Invalid Material while importing");
}

void nbr_import_model(NBRModel* nbr, const ResourceGroupID& group_id, Model* model) {
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
    ResourceID mat_id = resources_push_material(group_id, texture_ids[nbr->materials[i].diffuse_index]);
    Material* mat     = resources_get_material(mat_id);
    
    // Insert a valid specular texture (if one exists)
    i8 specular_index = nbr->materials[i].specular_index;
    mat->specular_map = specular_index != -1 ? resources_get_texture(texture_ids[specular_index]) : mat->specular_map;

    // Convert the NBRMaterial into an engine Material
    mat->color = Vec4(nbr->materials[i].diffuse[0], nbr->materials[i].diffuse[1], nbr->materials[i].diffuse[2], 1.0f); 

    // Add the material 
    model->materials.push_back(mat); 
  }
  
  // Convert the vertices 
  for(sizei i = 0; i < nbr->meshes_count; i++) {
    // Create and add a new mesh
    ResourceID mesh_id = resources_push_mesh(group_id, nbr->meshes[i]);
    model->meshes.push_back(resources_get_mesh(mesh_id));

    // Add a new index
    model->material_indices.push_back(nbr->meshes[i].material_index);
  }
}

void nbr_import_font(NBRFont* nbr, const ResourceGroupID& group_id, Font* font) {
  NIKOLA_ASSERT(nbr, "Invalid NBRFont while importing");
  NIKOLA_ASSERT(font, "Invalid Font while importing");

  // Import the font information
  font->ascent   = (f32)nbr->ascent;
  font->descent  = (f32)nbr->descent;
  font->line_gap = (f32)nbr->line_gap;

  // Import the glyphs 
  for(sizei i = 0; i < nbr->glyphs_count; i++) {
    Glyph glyph;

    // Importing the glyph's unicode
    glyph.unicode = nbr->glyphs[i].unicode;

    // Importing the glyph's size
    glyph.size.x = nbr->glyphs[i].width;
    glyph.size.y = nbr->glyphs[i].height;
    
    // Importing the glyph's offset
    glyph.offset.x = nbr->glyphs[i].offset_x;
    glyph.offset.y = nbr->glyphs[i].offset_y;
    
    // Importing the glyph's bounds
    glyph.left   = nbr->glyphs[i].left;
    glyph.top    = nbr->glyphs[i].top;
    glyph.right  = nbr->glyphs[i].right;
    glyph.bottom = nbr->glyphs[i].bottom;
    
    // Importing glyph information
    glyph.advance_x    = nbr->glyphs[i].advance_x;
    glyph.kern         = nbr->glyphs[i].kern;
    glyph.left_bearing = nbr->glyphs[i].left_bearing;

    // We don't care about glyphs that have a "non-size"
    if(glyph.size.x <= 0) {
      continue;
    }
  
    // Importing the texture
    GfxTextureDesc face_desc {
      .width  = (u32)nbr->glyphs[i].width,
      .height = (u32)nbr->glyphs[i].height,
      .depth  = 0, 
      .mips   = 1,

      .type      = GFX_TEXTURE_2D, 
      .format    = GFX_TEXTURE_FORMAT_R8, 
      .filter    = GFX_TEXTURE_FILTER_MIN_MAG_LINEAR, 
      .wrap_mode = GFX_TEXTURE_WRAP_CLAMP,
      
      .data = (void*)nbr->glyphs[i].pixels,
    };
    glyph.texture = resources_get_texture(resources_push_texture(group_id, face_desc));

    // Adding the new glyph
    font->glyphs[glyph.unicode] = glyph;
  }
}

void nbr_import_audio(NBRAudio* nbr, const ResourceGroupID& group_id, AudioBufferDesc* desc) {
  NIKOLA_ASSERT(nbr, "Invalid NBRAudio given while importing");
  NIKOLA_ASSERT(desc, "Invalid AudioBufferDesc given while importing");

  desc->format      = (AudioBufferFormat)nbr->format; 
  desc->channels    = nbr->channels; 
  desc->sample_rate = nbr->sample_rate;
  desc->size        = nbr->size;
  desc->data        = (void*)nbr->samples;
}

/// NBR importer functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
