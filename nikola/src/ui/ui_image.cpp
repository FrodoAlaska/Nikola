#include "nikola/nikola_ui.h"
#include "nikola/nikola_render.h"
#include "nikola/nikola_resources.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// UIImage functions

void ui_image_create(UIImage* image, const UIImageDesc& desc) {
  NIKOLA_ASSERT(image, "Invalid UIImage given to ui_image_create");
  
  image->size   = desc.size; 
  image->anchor = desc.anchor; 
  image->offset = desc.offset; 
  
  image->texture = resources_get_texture(desc.texture_id);

  image->tint      = desc.tint;
  image->is_active = true;

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

  batch_render_texture(image.texture, image.position, image.size, image.tint);
}

/// UIImage functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
