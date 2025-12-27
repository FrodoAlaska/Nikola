#include "nbr.h"

#include <nikola/nikola.h>

#include <stb/stb_truetype.h>

//////////////////////////////////////////////////////////////////////////

namespace nbr { // Start of nbr

/// ----------------------------------------------------------------------
/// Private functions

static nikola::u8* load_font_data(const nikola::FilePath& path) {
  // Open the file

  nikola::File file;
  if(!nikola::file_open(&file, path, (int)(nikola::FILE_OPEN_READ | nikola::FILE_OPEN_BINARY))) {
    NIKOLA_LOG_ERROR("[NBR-ERROR]: Could not load font at \'%s\'", path.c_str());
    return nullptr;
  } 

  // Load the actual data from the file

  nikola::sizei data_size = nikola::filesystem_get_size(path);
  nikola::u8* font_data   = (nikola::u8*)nikola::memory_allocate(data_size);
  nikola::file_read_bytes(file, font_data, data_size); 

  return font_data;
}

static nikola::f32 load_font_information(nikola::NBRFont* font, stbtt_fontinfo* info) {
  // This will return a value that will get used constantly to put all the values 
  // that come out of the stb in "scaled" coordinates.  
  //
  // @NOTE (4/5/2025, Mohamed): The `256.0f` is supposed to be a "base size"
  // the font will be scaled to. This COULD be something the user controls. 
  // However, in reality, the _actual_ size of the font will be determined 
  // when the font is drawn. This is just to make the font "look good".
  // I could be wrong, though. Very possible.
  //
  nikola::f32 scale_factor = stbtt_ScaleForPixelHeight(info, nikola::NBR_FONT_IMPORT_SCALE);

  // Get the ascent, descent, and line gap of the font and put them in 
  // the scaled space 
  
  nikola::i32 ascent, descent, line_gap;
  stbtt_GetFontVMetrics(info, &ascent, &descent, &line_gap);
  
  // "Scaling" the values given in order to use them correctly later
  
  font->ascent   = ascent * scale_factor;
  font->descent  = descent * scale_factor;
  font->line_gap = line_gap * scale_factor; 
  NIKOLA_LOG_TRACE("%f, %f, %f", font->ascent, font->descent, font->line_gap);

  // Get the bounding box around all possible characters 

  nikola::i32 left, top, right, bottom;
  stbtt_GetFontBoundingBox(info, &left, &top, &right, &bottom);

  font->left   = left * scale_factor;
  font->right  = right * scale_factor;
  font->top    = top * scale_factor;
  font->bottom = bottom * scale_factor;

  // Done!
  return scale_factor;
}

static void load_glyphs_data(nikola::HashMap<char, nikola::NBRFont::NBRGlyph>* font_glyphs, const nikola::f32 scale_factor, stbtt_fontinfo* info) {
  for(nikola::u32 i = 0; i < info->numGlyphs; i++) { 
    nikola::NBRFont::NBRGlyph glyph;
    glyph.codepoint = i + 32;

    // This functions will return 0 if the given unicode is not in 
    // the font. Thus, to speed up the loop, we just skip these unicodes.
    
    nikola::i32 glyph_index = stbtt_FindGlyphIndex(info, glyph.codepoint);
    if(glyph_index == 0) {
      continue;
    }

    // Pixels of the specific codepoint, offset, and size 
    
    nikola::i32 width, height, offset_x, offset_y; 
    glyph.pixels = stbtt_GetGlyphBitmap(info, 
                                        0,
                                        scale_factor, 
                                        glyph_index, 
                                        &width, 
                                        &height, 
                                        &offset_x, 
                                        &offset_y);

    if(!glyph.pixels) { // Could not load the bitmap for some reason...
      continue;
    }
  
    // Otherwise, load the glyph info

    glyph.width    = (float)width;
    glyph.height   = (float)height;
    glyph.offset_x = (float)offset_x;
    glyph.offset_y = (float)offset_y;

    // Get the bounding box of the glyph
    
    nikola::i32 left, top, right, bottom; 
    stbtt_GetGlyphBitmapBox(info, 
                            glyph_index, 
                            scale_factor, 
                            scale_factor, 
                            &left, 
                            &top, 
                            &right, 
                            &bottom);
   
    glyph.left   = left * scale_factor;
    glyph.top    = top * scale_factor;
    glyph.right  = right * scale_factor;
    glyph.bottom = bottom * scale_factor;

    // Getting the advance and the left side bearing of the specific codepoint/glyph.
    // The advance is the value required to "advance" to the next glyph
    
    nikola::i32 advance, left_side_bearing;
    stbtt_GetGlyphHMetrics(info, glyph_index, &advance, &left_side_bearing);
  
    glyph.left_bearing = left_side_bearing * scale_factor;
    glyph.advance_x    = advance * scale_factor;

    // A valid glyph that was loaded 
    font_glyphs->emplace((char)glyph.codepoint, glyph);
  }
}

/// Private functions
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Font loader functions

bool font_loader_load(nikola::NBRFont* font, const nikola::FilePath& path) {
  // Load the data from the font file
  
  nikola::u8* font_data = load_font_data(path);
  if(!font_data) {
    return false;
  }
  
  // Load the font metrics
  
  stbtt_fontinfo info;
  if(stbtt_InitFont(&info, font_data, stbtt_GetFontOffsetForIndex(font_data, 0)) == 0) {
    NIKOLA_LOG_ERROR("[NBR-ERROR]: Could not initialize STB truetype library");
    return false;
  }

  // Load the font information 
  nikola::f32 scale_factor = load_font_information(font, &info);

  // Load all the data of the glyphs
  
  nikola::HashMap<char, nikola::NBRFont::NBRGlyph> glyphs;
  load_glyphs_data(&glyphs, scale_factor, &info);
 
  // Apply the glyphs map onto the map
  
  font->glyphs_count = (nikola::u32)glyphs.size();
  font->glyphs       = (nikola::NBRFont::NBRGlyph*)nikola::memory_allocate(sizeof(nikola::NBRFont::NBRGlyph) * font->glyphs_count);
  
  nikola::u32 index = 0;
  for(auto& [key, value] : glyphs) {
    font->glyphs[index] = value;
    index++;
  }

  // Done!

  nikola::memory_free(font_data);
  return true;
}

void font_loader_unload(nikola::NBRFont& font) {
  // Free the glyphs

  for(nikola::u32 i = 0; i < font.glyphs_count; i++) {
    if(!font.glyphs[i].pixels) {
      continue;
    }

    nikola::memory_free(font.glyphs[i].pixels);
  }

  nikola::memory_free(font.glyphs);
}

/// Font loader functions
/// ----------------------------------------------------------------------

} // End of nbr

//////////////////////////////////////////////////////////////////////////
