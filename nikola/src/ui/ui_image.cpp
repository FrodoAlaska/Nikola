#include "nikola/nikola_ui.h"
#include "nikola/nikola_render.h"
#include "nikola/nikola_resources.h"
#include "nikola/nikola_input.h"
#include "nikola/nikola_event.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static const bool is_hovered(const UIImage& image) {
  Vec2 mouse_pos;
  input_mouse_position(&mouse_pos.x, &mouse_pos.y);
  
  Vec2 min = image.position;
  Vec2 max = image.position + image.size;
  
  return (mouse_pos.x > min.x && mouse_pos.x < max.x) && 
         (mouse_pos.y > min.y && mouse_pos.y < max.y);
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIImage functions

void ui_image_create(UIImage* image, const UIImageDesc& desc) {
  NIKOLA_ASSERT(image, "Invalid UIImage given to ui_image_create");
  
  image->size   = desc.size; 
  image->anchor = desc.anchor; 
  image->offset = desc.offset; 
  
  image->texture = resources_get_texture(desc.texture_id);

  image->tint        = desc.tint;
  image->is_active   = true;
  image->was_hovered = false;

  ui_image_set_anchor(*image, image->anchor, desc.canvas_bounds);
}

void ui_image_set_anchor(UIImage& image, const UIAnchor& anchor, const Vec2& bounds) {
  image.anchor       = anchor; 
  Vec2 bounds_center = bounds / 2.0f;
  Vec2 half_size     = image.size / 2.0f;

  Vec2 padding       = Vec2(10.0f);

  switch(image.anchor) {
    case UI_ANCHOR_TOP_LEFT:  
      image.position = padding;
      break;
    case UI_ANCHOR_TOP_CENTER:
      image.position.x = (bounds_center.x - half_size.x); 
      image.position.y = padding.y; 
      break;
    case UI_ANCHOR_TOP_RIGHT:
      image.position.x = (bounds.x - image.size.x - padding.x); 
      image.position.y = padding.y;  
      break;
    case UI_ANCHOR_CENTER_LEFT:  
      image.position.x = padding.x;
      image.position.y = (bounds_center.y - half_size.y - padding.y); 
      break;
    case UI_ANCHOR_CENTER:
      image.position = (bounds_center - half_size);
      break;
    case UI_ANCHOR_CENTER_RIGHT:
      image.position.x = (bounds.x - image.size.x - padding.x); 
      image.position.y = (bounds_center.y - half_size.y - padding.y); 
      break;
    case UI_ANCHOR_BOTTOM_LEFT:  
      image.position.x = padding.x + image.offset.x;
      image.position.y = (bounds.y - image.size.y - padding.y); 
      break;
    case UI_ANCHOR_BOTTOM_CENTER:
      image.position.x = (bounds_center.x - half_size.x);
      image.position.y = (bounds.y - image.size.y - padding.y); 
      break;
    case UI_ANCHOR_BOTTOM_RIGHT:
      image.position = bounds - image.size - padding; 
      break;
  }

  image.position += image.offset;
}

void ui_image_set_texture(UIImage& image, const ResourceID& texture_id) {
  image.texture = resources_get_texture(texture_id);
}

void ui_image_render(UIImage& image) {
  if(!image.is_active) {
    return;
  }
  
  // Check input state 

  // Hovered

  if(is_hovered(image)) {
    // Just entered the image
   
    if(!image.was_hovered) {
      Event event = {
        .type = EVENT_UI_IMAGE_ENTERED, 
        .image = &image, 
      };
      event_dispatch(event);
    }

    image.was_hovered = true;
  }
  else {
    image.was_hovered = false;
  }
  
  // Exited
  
  if(!image.was_hovered) {
    Event event = {
      .type = EVENT_UI_IMAGE_EXITED, 
      .image = &image, 
    };
    event_dispatch(event);
  }
  
  // Pressed
  
  if(image.was_hovered && input_action_pressed("ui-click")) {
    Event event = {
      .type = EVENT_UI_IMAGE_CLICKED,
      .image = &image,
    };
    event_dispatch(event);
  }

  batch_render_texture(image.texture, image.position, image.size, image.tint);
}

/// UIImage functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
