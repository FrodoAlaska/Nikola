#include "nikola/nikola_ui.h"
#include "nikola/nikola_render.h"
#include "nikola/nikola_resources.h"
#include "nikola/nikola_input.h"
#include "nikola/nikola_event.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// UIAnchor functions

Vec2 ui_anchor_get_position(const UIAnchor& anchor, 
                            const Vec2& screen_size, 
                            const Vec2& element_size, 
                            const Vec2& padding,
                            const Vec2& offset) {
  Vec2 result = Vec2(0.0f);

  Vec2 element_center = element_size / 2.0f; 
  Vec2 screen_center  = screen_size / 2.0f;

  switch(anchor) {
    case UI_ANCHOR_TOP_LEFT:  
      result = padding;
      break;
    case UI_ANCHOR_TOP_CENTER:
      result.x = (screen_center.x - element_center.x); 
      result.y = padding.y; 
      break;
    case UI_ANCHOR_TOP_RIGHT:
      result.x = (screen_size.x - element_size.x - padding.x); 
      result.y = padding.y;  
      break;
    case UI_ANCHOR_CENTER_LEFT:  
      result.x = padding.x;
      result.y = (screen_center.y - element_center.y); 
      break;
    case UI_ANCHOR_CENTER:
      result = (screen_center - element_center);
      break;
    case UI_ANCHOR_CENTER_RIGHT:
      result.x = (screen_size.x - element_size.x - padding.x); 
      result.y = (screen_center.y - element_center.y); 
      break;
    case UI_ANCHOR_BOTTOM_LEFT:  
      result.x = padding.x;
      result.y = (screen_size.y - element_size.y - padding.y); 
      break;
    case UI_ANCHOR_BOTTOM_CENTER:
      result.x = (screen_center.x - element_center.x);
      result.y = (screen_size.y - element_size.y - padding.y); 
      break;
    case UI_ANCHOR_BOTTOM_RIGHT:
      result.x = (screen_size.x - element_size.x - padding.x); 
      result.y = (screen_size.y - element_size.y - padding.y); 
      break;
  }

  return result + offset;
}

/// UIAnchor functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
