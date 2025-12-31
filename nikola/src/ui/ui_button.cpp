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

static void adjust_button(UIButton& button) {
  Vec2 half_padding = button.padding * 0.5f;
  button.size       = button.text.bounds + button.padding;
  
  button.position.x = button.text.position.x - half_padding.x;
  button.position.y = button.text.position.y - button.text.bounds.y - half_padding.y;
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIButton functions

void ui_button_create(UIButton* button, const UIButtonDesc& desc) {
  NIKOLA_ASSERT(button, "Invalid UIButton given to ui_button_create");
  NIKOLA_ASSERT(RESOURCE_IS_VALID(desc.font_id), "Invalid font given to ui_button_create");

  // Text init
  
  UITextDesc text_desc = {
    .string = desc.text, 

    .font_id   = desc.font_id, 
    .font_size = desc.font_size,

    .anchor        = desc.anchor, 
    .canvas_bounds = desc.canvas_bounds,

    .offset = desc.offset,
    .color  = desc.text_color,
  };
  ui_text_create(&button->text, text_desc);

  // Variables init

  button->id      = desc.bind_id; 
  button->offset  = desc.offset; 
  button->padding = desc.padding; 
 
  adjust_button(*button);

  button->color             = desc.color; 
  button->outline_color     = desc.outline_color;
  button->outline_thickness = desc.outline_thickness;
  button->border_radius     = desc.border_radius;

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

void ui_button_update(UIButton& button) {
  if(!button.is_active) {
    return;
  }

  // Some useful variables...
  
  Vec2 mouse_pos;
  input_mouse_position(&mouse_pos.x, &mouse_pos.y);

  button.color.a = 1.0f;

  // Check if the button is hovered

  bool is_hovered = point_in_rect(mouse_pos, button.position, button.size);
  if(is_hovered) {
    button.color.a = 0.8f;

    event_dispatch(Event{
      .type   = EVENT_UI_BUTTON_HOVERED, 
      .button = &button, 
    });
  }

  // Check if the button is pressed
  
  if(is_hovered && input_action_pressed("ui-click")) {
    button.color.a = 0.5f;

    event_dispatch(Event{
      .type   = EVENT_UI_BUTTON_CLICKED, 
      .button = &button, 
    });
  }
}

void ui_button_render(UIButton& button) {
  if(!button.is_active) {
    return;
  }

  // Render the outline
  
  batch_render_quad(button.position - (button.outline_thickness / 2.0f), 
                    button.size + button.outline_thickness,
                    button.border_radius,
                    button.outline_color);

  // Render the button
  batch_render_quad(button.position, button.size, button.border_radius, button.color);

  // Render the text
  ui_text_render(button.text);
}

/// UIButton functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
