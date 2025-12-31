#include "nikola/nikola_ui.h"
#include "nikola/nikola_resources.h"
#include "nikola/nikola_render.h"
#include "nikola/nikola_math.h"
#include "nikola/nikola_event.h"
#include "nikola/nikola_input.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static const Vec2 measure_bounds(const UIText& text) {
  Vec2 result    = Vec2(0.0f);
  f32 font_scale = text.font_size / NBR_FONT_IMPORT_SCALE; 

  for(auto& ch : text.string) {
    Font::Glyph* glyph = &text.font->glyphs[ch];
    
    // Make sure to take on the highest glyph
   
    f32 scaled_height = glyph->size.y * font_scale;
    if(result.y < scaled_height) {
      result.y = scaled_height;
    }

    // Give some love to the Y-axis as well
    
    if(ch == '\n') {
      result.x  = 0.0f; 
      result.y += (text.font->ascent - text.font->descent + text.font->line_gap) * font_scale;

      continue; 
    }

    // Take into account the spaces as well as normal characters
    
    if(ch == ' ' || ch == '\t') {
      result.x += (text.font_size * font_scale) * 2;
      continue;
    }
     
    result.x += glyph->advance_x * font_scale;
  }

  return result;
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIText functions

void ui_text_create(UIText* text, const UITextDesc& desc) {
  NIKOLA_ASSERT(text, "Invalid UIText given to ui_text_create");
  NIKOLA_ASSERT(RESOURCE_IS_VALID(desc.font_id), "Invalid font given to ui_text_create");

  // Variables init

  text->string    = desc.string;
  text->font      = resources_get_font(desc.font_id);
  text->font_size = desc.font_size;

  text->position = nikola::Vec2(0.0f);
  text->offset   = desc.offset;
  text->bounds   = measure_bounds(*text);

  text->anchor        = desc.anchor;
  text->canvas_bounds = desc.canvas_bounds;
  text->color         = desc.color;
  ui_text_set_anchor(*text, text->anchor);

  text->is_active = true;
}

void ui_text_set_anchor(UIText& text, const UIAnchor& anchor) {
  text.anchor = anchor;

  text.position    = ui_anchor_get_position(text.anchor, text.canvas_bounds, text.bounds, Vec2(10.0f), text.offset);
  text.position.y += text.bounds.y;
}

void ui_text_set_string(UIText& text, const String& new_string) {
  text.string = new_string;
  text.bounds = measure_bounds(text);

  ui_text_set_anchor(text, text.anchor);
}

void ui_text_update(UIText& text) {
  if(!text.is_active) {
    return;
  }

  // Some useful variables...
  
  Vec2 mouse_pos;
  input_mouse_position(&mouse_pos.x, &mouse_pos.y);

  // Check if the text is hovered

  bool is_hovered = point_in_rect(mouse_pos, text.position - Vec2(0.0f, text.bounds.y), text.bounds);
  if(is_hovered) {
    Event event = {
      .type = EVENT_UI_TEXT_HOVERED, 
      .text = &text, 
    };
    event_dispatch(event);
  }

  // Check if the text has been pressed
  
  if(is_hovered && input_action_pressed("ui-click")) {
    event_dispatch(Event{
      .type = EVENT_UI_TEXT_CLICKED, 
      .text = &text, 
    });
  }
}

void ui_text_render(UIText& text) {
  if(!text.is_active) {
    return;
  }

  batch_render_text(text.font, text.string, text.position, text.font_size, text.color);
}

/// UIText functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
