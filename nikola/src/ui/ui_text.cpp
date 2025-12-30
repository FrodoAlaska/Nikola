#include "nikola/nikola_ui.h"
#include "nikola/nikola_resources.h"
#include "nikola/nikola_render.h"
#include "nikola/nikola_math.h"

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

static void apply_animation_fade(UIText& text, const i32 dir, const f32 duration) {
  // We need to "undo" the current alpha in order 
  // to see the effect happeninig.
  //
  // i.e: if we're fading _in_ (increasing the alpha), 
  // and the alpha is already at 1.0f, then we won't see 
  // the effect.
  
  if(dir > 0 && text.color.a == 1.0f) {
    text.color.a = 0.0f;
  }
  else if(dir < 0 && text.color.a == 0.0f) {
    text.color.a = 1.0f;
  }

  // "Animate" the fade effect
  
  text.color.a += (dir * duration) * niclock_get_delta_time();
  text.color.a = clamp_float(text.color.a, 0.001f, 0.99f);
}

static void apply_animation_ballon(UIText& text, const i32 dir, const f32 duration) {
  // Animate the ballon effect by increasing or decreasing the font size
  text.font_size += (dir * duration) * niclock_get_delta_time();

  // Re-position the text to not lose the anchor position 
  ui_text_set_anchor(text, text.anchor, text.canvas_bounds);
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
  ui_text_set_anchor(*text, text->anchor, desc.canvas_bounds);

  text->is_active = true;
}

void ui_text_set_anchor(UIText& text, const UIAnchor& anchor, const Vec2& bounds) {
  text.anchor = anchor;

  text.position    = ui_anchor_get_position(text.anchor, bounds, text.bounds, Vec2(10.0f), text.offset);
  text.position.y += text.bounds.y;
}

void ui_text_set_string(UIText& text, const String& new_string) {
  text.string = new_string;
  text.bounds = measure_bounds(text);

  ui_text_set_anchor(text, text.anchor, text.canvas_bounds);
}

void ui_text_apply_animation(UIText& text, const UITextAnimation anim_type, const f32 duration) {
  if(!text.is_active) {
    return;
  }

  switch(anim_type) {
    case UI_TEXT_ANIMATION_FADE_IN:
      apply_animation_fade(text, 1, duration);
      break;
    case UI_TEXT_ANIMATION_FADE_OUT:
      apply_animation_fade(text, -1, duration);
      break;
    case UI_TEXT_ANIMATION_BALLON_UP:
      apply_animation_ballon(text, 1, duration);
      break;
    case UI_TEXT_ANIMATION_BALLON_DOWN:
      apply_animation_ballon(text, -1, duration);
      break;
    case UI_TEXT_ANIMATION_SLIDE_UP:
      text.position.y -= duration * niclock_get_delta_time();
      break;
    case UI_TEXT_ANIMATION_SLIDE_DOWN:
      text.position.y += duration * niclock_get_delta_time();
      break;
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
