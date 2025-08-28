#include "nikola/nikola_ui.h"
#include "nikola/nikola_render.h"
#include "nikola/nikola_math.h"
#include "nikola/nikola_event.h"
#include "nikola/nikola_input.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// Callbacks

static bool on_mouse_move(const nikola::Event& event, const void* dispatcher, const void* listener) {
  if(!listener) {
    return true;
  }

  UISlider* slider = (UISlider*)listener;
  if(slider->is_hovering && input_action_down("ui-click")) {
    Vec2 diff       = vec2_normalize(Vec2(event.mouse_pos_x, event.mouse_pos_y) - slider->old_mouse_position);
    *slider->value += (diff.x > 0.0f) ? slider->step : -slider->step;
    
    Event slider_event = {
      .type   = EVENT_UI_SLIDER_CHANGED, 
      .slider = slider, 
    };
    event_dispatch(slider_event);
  }

  slider->old_mouse_position = Vec2(event.mouse_pos_x, event.mouse_pos_y);
  return true;
}

/// Callbacks
/// ----------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static const bool is_hovered(const UISlider& slider) {
  Vec2 mouse_pos;
  input_mouse_position(&mouse_pos.x, &mouse_pos.y);

  Vec2 min = slider.notch_position;
  Vec2 max = slider.notch_position + slider.notch_size;

  return (mouse_pos.x > min.x && mouse_pos.x < max.x) && 
         (mouse_pos.y > min.y && mouse_pos.y < max.y);
}

static const f32 to_slider_space(const UISlider& slider) {
  return remap(slider.notch_position.x,
               slider.position.x, 
               slider.position.x + slider.size.x,
               slider.min,   
               slider.max);
}

static const f32 to_screen_space(const UISlider& slider) {
  f32 min = slider.position.x;
  f32 max = slider.position.x + slider.size.x - (slider.notch_size.x);

  return remap(*slider.value, // Value
               slider.min,    // Old min
               slider.max,    // Old max
               min,           // New min
               max);          // New max
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UISlider

void ui_slider_create(UISlider* slider, Window* window, const UISliderDesc& desc) {
  NIKOLA_ASSERT(slider, "Invalid UISlider given to ui_slider_create");
  NIKOLA_ASSERT(desc.value, "Invalid value given to ui_slider_create");
  NIKOLA_ASSERT(window, "Invalid Window struct given to ui_slider_create");
  
  // Variables init

  slider->anchor = desc.anchor; 
  slider->offset = desc.offset;
  
  input_mouse_position(&slider->old_mouse_position.x, &slider->old_mouse_position.y);
  
  slider->size       = Vec2(250.0f, 12.0f);
  slider->notch_size = Vec2(24.0f);

  slider->id         = desc.bind_id;
  slider->window_ref = window;
  
  slider->value = desc.value; 
  slider->min   = desc.min;
  slider->max   = desc.max;
  slider->step  = desc.step;

  ui_slider_set_anchor(*slider, slider->anchor);

  slider->color       = desc.color; 
  slider->notch_color = desc.notch_color;

  slider->is_active   = true;
  slider->is_hovering = false;

  event_listen(EVENT_MOUSE_MOVED, on_mouse_move, slider);
}

void ui_slider_set_anchor(UISlider& slider, const UIAnchor& anchor) {
  slider.anchor = anchor;

  i32 width, height; 
  window_get_size(slider.window_ref, &width, &height);

  Vec2 window_size   = Vec2(width, height);
  Vec2 window_center = window_size / 2.0f;

  Vec2 half_size = slider.size / 2.0f; 
  Vec2 padding   = Vec2(10.0f);
  
  switch(slider.anchor) {
    case UI_ANCHOR_TOP_LEFT:  
      slider.position = padding;
      break;
    case UI_ANCHOR_TOP_CENTER:
      slider.position.x = (window_center.x - half_size.x); 
      slider.position.y = padding.y; 
      break;
    case UI_ANCHOR_TOP_RIGHT:
      slider.position.x = (window_size.x - slider.size.x - padding.x); 
      slider.position.y = padding.y;  
      break;
    case UI_ANCHOR_CENTER_LEFT:  
      slider.position.x = padding.x;
      slider.position.y = (window_center.y - half_size.y - padding.y); 
      break;
    case UI_ANCHOR_CENTER:
      slider.position = (window_center - half_size);
      break;
    case UI_ANCHOR_CENTER_RIGHT:
      slider.position.x = (window_size.x - slider.size.x - padding.x); 
      slider.position.y = (window_center.y - half_size.y - padding.y); 
      break;
    case UI_ANCHOR_BOTTOM_LEFT:  
      slider.position.x = padding.x + slider.offset.x;
      slider.position.y = (window_size.y - slider.size.y - padding.y); 
      break;
    case UI_ANCHOR_BOTTOM_CENTER:
      slider.position.x = (window_center.x - half_size.x);
      slider.position.y = (window_size.y - slider.size.y - padding.y); 
      break;
    case UI_ANCHOR_BOTTOM_RIGHT:
      slider.position = window_size - slider.size - padding; 
      break;
  }

  slider.position += slider.offset;

  slider.notch_position.x = to_screen_space(slider) + (slider.notch_size.x / 2.0f);
  slider.notch_position.y = slider.position.y - (slider.size.y / 2.0f);
}

void ui_slider_render(UISlider& slider) {
  if(!slider.is_active) {
    return;
  }

  Vec4 notch_color   = slider.notch_color;
  slider.is_hovering = is_hovered(slider);

  // Check state

  // Hovered
  notch_color.a = slider.is_hovering ? 0.8f : slider.notch_color.a;

  // Clicked
  if(slider.is_hovering && input_action_pressed("ui-click")) {
    Event event = {
      .type   = EVENT_UI_SLIDER_CLICKED, 
      .slider = &slider, 
    };
    event_dispatch(event);
  }

  // Clamp the value to the min and the max
  *slider.value = nikola::clamp_float(*slider.value, slider.min, slider.max);

  // Get the actual position from the slider space
  slider.notch_position.x = to_screen_space(slider);

  // Render the quad
  batch_render_quad(slider.position, slider.size, slider.color);

  // Render the notch
  batch_render_quad(slider.notch_position, slider.notch_size, notch_color);
}

/// UISlider
///---------------------------------------------------------------------------------------------------------------------


} // End of nikola

//////////////////////////////////////////////////////////////////////////
