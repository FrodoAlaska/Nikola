#include "nikola/nikola_ui.h"
#include "nikola/nikola_resources.h"
#include "nikola/nikola_render.h"
#include "nikola/nikola_math.h"
#include "nikola/nikola_event.h"
#include "nikola/nikola_input.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// UILayout functions

void ui_layout_create(UILayout* layout, Window* window, const ResourceID& font_id) {
  NIKOLA_ASSERT(layout, "Invalid UILayout given to ui_layout_create");
  NIKOLA_ASSERT(window, "Invalid Window struct given to ui_button_create");
  NIKOLA_ASSERT(RESOURCE_IS_VALID(font_id), "Invalid font given to ui_layout_create");
  
  // Variables init

  layout->font_id    = font_id; 
  layout->window_ref = window;

  layout->current_anchor = UI_ANCHOR_TOP_LEFT;
  layout->extra_offset   = Vec2(0.0f);
  layout->current_offset = Vec2(0.0f);

  layout->buttons_outline_thickness = 7.0f; 
  layout->buttons_padding           = Vec2(40.0f, 10.0f);

  layout->is_active = true;
}

void ui_layout_begin(UILayout& layout, const UIAnchor anchor, const Vec2& offset) {
  layout.current_anchor = anchor; 
  layout.extra_offset   = offset;
}

void ui_layout_end(UILayout& layout) {
  layout.current_offset = Vec2(0.0f); 
  layout.extra_offset   = Vec2(0.0f);
}

void ui_layout_push_text(UILayout& layout, 
                         const String& str, 
                         const f32 size, 
                         const Vec4& color, 
                         const Vec2& layout_padding) {
  // Create the text

  layout.texts.push_back(UIText{});
  UIText* text = &layout.texts[layout.texts.size() - 1];

  UITextDesc text_desc = {
    .string = str, 

    .font_id   = layout.font_id, 
    .font_size = size,

    .anchor = layout.current_anchor,
    .offset = layout.current_offset + layout_padding,

    .color = color,
  };
  ui_text_create(text, layout.window_ref, text_desc);

  // Add the extra offset for the next UI element
  layout.current_offset += layout.extra_offset;
}

void ui_layout_push_button(UILayout& layout, 
                           const String& str, 
                           const f32 size, 
                           const Vec4& color, 
                           const Vec4& outline_color, 
                           const Vec4& text_color, 
                           const Vec2& layout_padding) {
  // Create the button

  layout.buttons.push_back(UIButton{});
  UIButton* button = &layout.buttons[layout.buttons.size() - 1];

  UIButtonDesc button_desc = {
    .text = str, 

    .font_id   = layout.font_id, 
    .font_size = size,
    .anchor    = layout.current_anchor,
    
    .bind_id = (u32)(layout.buttons.size() - 1),

    .offset  = layout.current_offset + layout_padding,
    .padding = layout.buttons_padding,

    .color         = color,
    .outline_color = outline_color,
    .text_color    = outline_color,

    .outline_thickness = layout.buttons_outline_thickness,
  };
  ui_button_create(button, layout.window_ref, button_desc);

  // Add the extra offset for the next UI element
  layout.current_offset += layout.extra_offset;
}

void ui_layout_render(const UILayout& layout) {
  if(!layout.is_active) {
    return;
  }

  // Render the texts
  for(auto& txt : layout.texts) {
    ui_text_render(txt);
  }

  // Render the buttons
  for(auto& button : layout.buttons) {
    ui_button_render((UIButton&)button);
  }
}

/// UILayout functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
