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
  button.size = button.text.bounds + button.padding;
  
  button.position.x = button.text.position.x - (button.padding.x / 2.0f);
  button.position.y = button.text.position.y - (button.text.bounds.y - (button.padding.y / 2.0f));
}

static const bool is_hovered(const UIButton& button) {
  Vec2 mouse_pos;
  input_mouse_position(&mouse_pos.x, &mouse_pos.y);

  Vec2 min = button.position;
  Vec2 max = button.position + button.size;

  return (mouse_pos.x > min.x && mouse_pos.x < max.x) && 
         (mouse_pos.y > min.y && mouse_pos.y < max.y);
}

static const bool is_pressed(const UIButton& button) {
  return is_hovered(button) && input_action_pressed("ui-click");
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

  button->is_active   = true;
  button->was_hovered = false;
}

void ui_button_set_anchor(UIButton& button, const UIAnchor& anchor) {
  ui_text_set_anchor(button.text, anchor);
  adjust_button(button);
}

void ui_button_set_string(UIButton& button, const String& new_string) {
  ui_text_set_string(button.text, new_string);
  adjust_button(button);
}

void ui_button_render(UIButton& button) {
  if(!button.is_active) {
    return;
  }

  Vec4 color = button.color;

  // Check input state 

  // Hovered 
  if(is_hovered(button)) {
    // Just entered the button
    if(!button.was_hovered) {
      Event event = {
        .type   = EVENT_UI_BUTTON_ENTERED, 
        .button = &button, 
      };
      event_dispatch(event);
    }
    
    color.a            = 0.5f;
    button.was_hovered = true;
  }

  // Pressed
  if(is_pressed(button)) {
    color.a = 0.1f;

    Event event = {
      .type   = EVENT_UI_BUTTON_CLICKED,
      .button = &button,
    };
    event_dispatch(event);
  }

  // Exited
  if(!button.was_hovered) {
    Event event = {
      .type   = EVENT_UI_BUTTON_EXITED,
      .button = &button,
    };
    event_dispatch(event);
  }

  // Render the button
  batch_render_quad(button.position - (Vec2(button.outline_thickness) / 2.0f), 
                    button.size + Vec2(button.outline_thickness),
                    button.outline_color);
  batch_render_quad(button.position, button.size, color);

  // Render the text
  ui_text_render(button.text);

  // Reset the hovered state
  button.was_hovered = false;
}

/// UIButton functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
