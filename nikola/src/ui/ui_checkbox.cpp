#include "nikola/nikola_ui.h"
#include "nikola/nikola_render.h"
#include "nikola/nikola_math.h"
#include "nikola/nikola_event.h"
#include "nikola/nikola_input.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static const bool is_hovered(const UICheckbox& checkbox) {
  Vec2 mouse_pos;
  input_mouse_position(&mouse_pos.x, &mouse_pos.y);

  Vec2 min = checkbox.position;
  Vec2 max = checkbox.position + checkbox.size;

  return (mouse_pos.x > min.x && mouse_pos.x < max.x) && 
         (mouse_pos.y > min.y && mouse_pos.y < max.y);
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UICheckbox

void ui_checkbox_create(UICheckbox* checkbox, const UICheckboxDesc& desc) {
  NIKOLA_ASSERT(checkbox, "Invalid UICheckbox given to ui_checkbox_create");

  // Variables init

  checkbox->id = desc.bind_id;

  checkbox->anchor = desc.anchor; 
  checkbox->size   = Vec2(desc.size);
  checkbox->offset = desc.offset;
 
  ui_checkbox_set_anchor(*checkbox, checkbox->anchor, desc.canvas_bounds);

  checkbox->color         = desc.color;
  checkbox->outline_color = desc.outline_color;

  checkbox->is_active   = true;
  checkbox->is_checked  = desc.initial_checked;
  checkbox->was_hovered = false;
}

void ui_checkbox_set_anchor(UICheckbox& checkbox, const UIAnchor& anchor, const Vec2& bounds) {
  checkbox.anchor    = anchor;
  Vec2 bounds_center = bounds / 2.0f;

  Vec2 half_size = checkbox.size / 2.0f; 
  Vec2 padding   = Vec2(10.0f);
  
  switch(checkbox.anchor) {
    case UI_ANCHOR_TOP_LEFT:  
      checkbox.position = padding;
      break;
    case UI_ANCHOR_TOP_CENTER:
      checkbox.position.x = (bounds_center.x - half_size.x); 
      checkbox.position.y = padding.y; 
      break;
    case UI_ANCHOR_TOP_RIGHT:
      checkbox.position.x = (bounds.x - checkbox.size.x - padding.x); 
      checkbox.position.y = padding.y;  
      break;
    case UI_ANCHOR_CENTER_LEFT:  
      checkbox.position.x = padding.x;
      checkbox.position.y = (bounds_center.y - half_size.y - padding.y); 
      break;
    case UI_ANCHOR_CENTER:
      checkbox.position = (bounds_center - half_size);
      break;
    case UI_ANCHOR_CENTER_RIGHT:
      checkbox.position.x = (bounds.x - checkbox.size.x - padding.x); 
      checkbox.position.y = (bounds_center.y - half_size.y - padding.y); 
      break;
    case UI_ANCHOR_BOTTOM_LEFT:  
      checkbox.position.x = padding.x + checkbox.offset.x;
      checkbox.position.y = (bounds.y - checkbox.size.y - padding.y); 
      break;
    case UI_ANCHOR_BOTTOM_CENTER:
      checkbox.position.x = (bounds_center.x - half_size.x);
      checkbox.position.y = (bounds.y - checkbox.size.y - padding.y); 
      break;
    case UI_ANCHOR_BOTTOM_RIGHT:
      checkbox.position = bounds - checkbox.size - padding; 
      break;
  }

  checkbox.position += checkbox.offset;
}

void ui_checkbox_render(UICheckbox& checkbox) {
  if(!checkbox.is_active) {
    return;
  }

  Vec4 color = checkbox.color;

  // Check the input state

  // Hovered
  if(is_hovered(checkbox)) {
    // Just entered the UI element
    if(!checkbox.was_hovered) {
      Event event = {
        .type     = EVENT_UI_CHECKBOX_ENTERED, 
        .checkbox = &checkbox, 
      };
      event_dispatch(event);
    }
    
    color.a              = 0.8f;
    checkbox.was_hovered = true;
  }
  else {
    checkbox.was_hovered = false;
  }

  // Pressed
  if(checkbox.was_hovered && input_action_pressed("ui-click")) {
    color.a = 0.5f;

    Event event = {
      .type     = EVENT_UI_CHECKBOX_CLICKED,
      .checkbox = &checkbox,
    };
    event_dispatch(event);

    checkbox.is_checked = !checkbox.is_checked;
  }

  // Exited
  if(!checkbox.was_hovered) {
    Event event = {
      .type     = EVENT_UI_CHECKBOX_EXITED,
      .checkbox = &checkbox,
    };
    event_dispatch(event);
  }

  // Render the outline
  batch_render_quad(checkbox.position, checkbox.size, checkbox.outline_color);

  // Render the checkbox (if it's active)
  if(checkbox.is_checked) {
    batch_render_quad(checkbox.position + 5.0f, checkbox.size - 10.0f, color);
  }
}

/// UICheckbox
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
