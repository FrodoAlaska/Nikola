#include "nikola/nikola_ui.h"
#include "nikola/nikola_resources.h"
#include "nikola/nikola_render.h"
#include "nikola/nikola_math.h"
#include "nikola/nikola_event.h"
#include "nikola/nikola_input.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// UIMenu functions

void ui_menu_create(UIMenu* menu, const UIMenuDesc& desc) {
  NIKOLA_ASSERT(menu, "Invalid UIMenu given to ui_menu_create");
  NIKOLA_ASSERT(RESOURCE_IS_VALID(desc. font_id), "Invalid font given to ui_menu_create");

  // Variables init

  menu->bounds  = desc.bounds; 
  menu->font_id = desc.font_id;

  menu->title_anchor = desc.title_anchor;
  menu->item_anchor  = desc.item_anchor;

  menu->current_offset = Vec2(0.0f);
  menu->title_offset   = desc.title_offset;
  menu->item_offset    = desc.item_offset;
  menu->item_padding   = desc.item_padding;

  menu->current_item   = desc.starting_item;
  menu->selector_color = desc.selector_color; 
}

void ui_menu_set_title(UIMenu& menu, 
                       const String& str,
                       const f32 size, 
                       const Vec4& color, 
                       const Vec2& padding) {
  UITextDesc text_desc = {
    .string = str, 

    .font_id   = menu.font_id, 
    .font_size = size,

    .anchor        = menu.title_anchor,
    .canvas_bounds = menu.bounds, 

    .offset = menu.title_offset + padding,
    .color  = color,
  };
  ui_text_create(&menu.title, text_desc);
}

UIText& ui_menu_push_item(UIMenu& menu, 
                          const String& str,
                          const f32 size, 
                          const Vec4& color, 
                          const Vec2& padding) {
  menu.items.push_back(UIText{});
  UIText& item = menu.items[menu.items.size() - 1];
   
  UITextDesc text_desc = {
    .string = str, 

    .font_id   = menu.font_id, 
    .font_size = size,

    .anchor        = menu.item_anchor,
    .canvas_bounds = menu.bounds, 

    .offset = menu.current_offset + padding,
    .color  = color,
  };
  ui_text_create(&item, text_desc);

  menu.current_offset += menu.item_offset;
  return item;
}

void ui_menu_process_input(UIMenu& menu) {
  if(!menu.is_active) {
    return;
  }

  // Input management
  
  if(input_action_pressed("ui-nav-up")) {
    menu.current_item--;

    Event event = {
      .type = EVENT_UI_MENU_ITEM_CHANGED,
      .menu = &menu,
    };
    event_dispatch(event);
  }
  else if(input_action_pressed("ui-nav-down")) {
    menu.current_item++;

    Event event = {
      .type = EVENT_UI_MENU_ITEM_CHANGED,
      .menu = &menu,
    };
    event_dispatch(event);
  }
  else if(input_action_pressed("ui-click")) {
    Event event = {
      .type = EVENT_UI_MENU_ITEM_CLICKED,
      .menu = &menu,
    };
    event_dispatch(event);
  }

  // Some upkeep
  menu.current_item = clamp_int(menu.current_item, 0, menu.items.size() - 1);
}

void ui_menu_render(UIMenu& menu) {
  if(!menu.is_active) {
    return;
  }

  // Render the title
  ui_text_render(menu.title);

  // Render the items

  for(auto& item : menu.items) {
    ui_text_render(item);
  }

  // Render the selector

  UIText* text = &menu.items[menu.current_item];

  // @NOTE: I don't know. It just works. Just leave it be, okay? 
  Vec2 position = text->position - Vec2(0.0f, ((text->bounds.y + text->font_size) / 2.0f) - (text->font_size / 4.0f)); 
  Vec2 size     = text->bounds + menu.item_padding;

  batch_render_quad(position - (menu.item_padding / 2.0f), size, menu.selector_color);
}

/// UIMenu functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
