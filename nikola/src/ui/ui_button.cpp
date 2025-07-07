#include "nikola/nikola_ui.h"
#include "nikola/nikola_resources.h"
#include "nikola/nikola_render.h"
#include "nikola/nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static void adjust_button(UIButton& button) {
  button.size = button.text.bounds + button.padding;
  
  button.position.x = button.text.position.x - (button.padding.x / 2.0f);
  button.position.y = button.text.position.y - (button.text.bounds.y - (button.padding.y / 2.0f));
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIButton functions

void ui_button_create(UIButton* button, Window* window, const UIButtonDesc& desc) {
  NIKOLA_ASSERT(button, "Invalid UIButton given to ui_button_create");
  NIKOLA_ASSERT(window, "Invalid Window struct given to ui_button_create");
  NIKOLA_ASSERT(RESOURCE_IS_VALID(desc.font_id), "Invalid font given to ui_button_create");

  // Text init
  UITextDesc text_desc = {
    .string = desc.text, 

    .font_id   = desc.font_id, 
    .font_size = desc.font_size,

    .anchor = desc.anchor, 
    .offset = desc.offset,
    .color  = desc.text_color,
  };
  ui_text_create(&button->text, window, text_desc);

  // Variables init

  button->id      = desc.bind_id; 
  button->offset  = desc.offset; 
  button->padding = desc.padding; 
 
  adjust_button(*button);

  button->color             = desc.color; 
  button->outline_color     = desc.outline_color;
  button->outline_thickness = desc.outline_thickness;

  button->state     = UI_BUTTON_STATE_NORMAL;
  button->is_active = true;
}

void ui_button_set_anchor(UIButton& button, const UIAnchor& anchor) {
  ui_text_set_anchor(button.text, anchor);
  adjust_button(button);
}

void ui_button_set_string(UIButton& button, const String& new_string) {
  ui_text_set_string(button.text, new_string);
  adjust_button(button);
}

void ui_button_render(const UIButton& button) {
  if(!button.is_active) {
    return;
  }

  // Render the button
  batch_render_quad(button.position - (Vec2(button.outline_thickness) / 2.0f), 
                    button.size + Vec2(button.outline_thickness),
                    button.outline_color);
  batch_render_quad(button.position, button.size, button.color);

  // Render the text
  ui_text_render(button.text);
}

/// UIButton functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
