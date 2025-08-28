#include "nikola/nikola_ui.h"
#include "nikola/nikola_resources.h"
#include "nikola/nikola_render.h"
#include "nikola/nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static const Vec2 measure_bounds(const UIText& text) {
  Vec2 result(0.0f, text.font_size);

  f32 font_scale   = (text.font_size / 256.0f); // @TODO: This is an engine problem, but the `256` should be a constant. This is _REALLY_ bad.
  f32 prev_advance = 0.0f;

  for(auto& ch : text.string) {
    Glyph* glyph = &text.font->glyphs[ch];
     
    // Give some love to the Y-axis as well
    if(ch == '\n') {
      result.y += text.font_size;
      result.x  = 0.0f; 

      continue; 
    }

    // Take into account the spaces as well as normal characters
    if(ch == ' ' || ch == '\t') {
      result.x += prev_advance * font_scale;
      continue;
    }
     
    result.x    += glyph->advance_x;
    prev_advance = glyph->advance_x;
  }

  return Vec2(result.x * font_scale, result.y);
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
  ui_text_set_anchor(text, text.anchor);
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIText functions

void ui_text_create(UIText* text, Window* window, const UITextDesc& desc) {
  NIKOLA_ASSERT(text, "Invalid UIText given to ui_text_create");
  NIKOLA_ASSERT(window, "Invalid Window struct given to ui_text_create");
  NIKOLA_ASSERT(RESOURCE_IS_VALID(desc.font_id), "Invalid font given to ui_text_create");

  // Variables init

  text->window_ref = window;

  text->string    = desc.string;
  text->font      = resources_get_font(desc.font_id);
  text->font_size = desc.font_size;

  text->position = nikola::Vec2(0.0f);
  text->offset   = desc.offset;
  text->bounds   = measure_bounds(*text);

  text->anchor = desc.anchor;
  text->color  = desc.color;
  ui_text_set_anchor(*text, text->anchor);

  text->is_active = true;
}

void ui_text_set_anchor(UIText& text, const UIAnchor& anchor) {
  text.anchor = anchor;

  int width, height; 
  window_get_size(text.window_ref, &width, &height);

  text.bounds      = measure_bounds(text);
  Vec2 text_center = text.bounds / 2.0f;
  
  Vec2 window_size   = Vec2(width, height);
  Vec2 window_center = window_size / 2.0f;
  
  Vec2 padding = Vec2(15.0f, text.font_size);

  switch(text.anchor) {
    case UI_ANCHOR_TOP_LEFT:  
      text.position = padding + text.offset;
      break;
    case UI_ANCHOR_TOP_CENTER:
      text.position.x = (window_center.x - text_center.x) + text.offset.x; 
      text.position.y = padding.y + text.offset.y; 
      break;
    case UI_ANCHOR_TOP_RIGHT:
      text.position.x = (window_size.x - text.bounds.x - padding.x - 15.0f) + text.offset.x; 
      text.position.y = padding.y + text.offset.y;  
      break;
    case UI_ANCHOR_CENTER_LEFT:  
      text.position.x = padding.x + text.offset.x;
      text.position.y = (window_center.y - text_center.y) + text.offset.y; 
      break;
    case UI_ANCHOR_CENTER:
      text.position = (window_center - text_center) + text.offset;
      break;
    case UI_ANCHOR_CENTER_RIGHT:
      text.position.x = (window_size.x - text.bounds.x - padding.x - 15.0f) + text.offset.x; 
      text.position.y = (window_center.y - text_center.y) + text.offset.y; 
      break;
    case UI_ANCHOR_BOTTOM_LEFT:  
      text.position.x = padding.x + text.offset.x;
      text.position.y = (window_size.y - text.bounds.y) + text.offset.y + (text.font_size / 2.0f); 
      break;
    case UI_ANCHOR_BOTTOM_CENTER:
      text.position.x = (window_center.x - text_center.x) + text.offset.x;
      text.position.y = (window_size.y - text.bounds.y) + text.offset.y + (text.font_size / 2.0f); 
      break;
    case UI_ANCHOR_BOTTOM_RIGHT:
      text.position.x = (window_size.x - text.bounds.x - padding.x - 15.0f) + text.offset.x; 
      text.position.y = (window_size.y - text.bounds.y) + text.offset.y + (text.font_size / 2.0f); 
      break;
  }
}

void ui_text_set_string(UIText& text, const String& new_string) {
  text.string = new_string;
  ui_text_set_anchor(text, text.anchor);
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

void ui_text_render(const UIText& text) {
  if(!text.is_active) {
    return;
  }

  batch_render_text(text.font, text.string, text.position, text.font_size, text.color);
}

/// UIText functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
