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

  // Reserve some space for the UI elements

  layout->texts.reserve(8);
  layout->buttons.reserve(8);
  layout->checkboxes.reserve(8);
  layout->images.reserve(8);
  layout->sliders.reserve(8);
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
                           const i32 bind_id,
                           const Vec2& layout_padding) {
  // Create the button

  layout.buttons.push_back(UIButton{});
  UIButton* button = &layout.buttons[layout.buttons.size() - 1];

  UIButtonDesc button_desc = {
    .text = str, 

    .font_id   = layout.font_id, 
    .font_size = size,
    .anchor    = layout.current_anchor,
    
    .bind_id = (bind_id == -1) ? (u32)(layout.buttons.size() - 1) : bind_id,

    .offset  = layout.current_offset + layout_padding,
    .padding = layout.buttons_padding,

    .color         = color,
    .outline_color = outline_color,
    .text_color    = text_color,

    .outline_thickness = layout.buttons_outline_thickness,
  };
  ui_button_create(button, layout.window_ref, button_desc);

  // Add the extra offset for the next UI element
  layout.current_offset += layout.extra_offset;
}

void ui_layout_push_checkbox(UILayout& layout, 
                             const f32 size, 
                             const Vec4& color, 
                             const Vec4& outline_color, 
                             const i32 bind_id,
                             const Vec2& layout_padding) {
  // Create the checkbox

  layout.checkboxes.push_back(UICheckbox{});
  UICheckbox* checkbox = &layout.checkboxes[layout.checkboxes.size() - 1];

  UICheckboxDesc desc = {
    .size = size, 

    .anchor = layout.current_anchor,
    .offset = layout.current_offset + layout_padding, 
    
    .bind_id = (bind_id == -1) ? (u32)(layout.checkboxes.size() - 1) : bind_id,

    .color         = color,
    .outline_color = outline_color,
  };
  ui_checkbox_create(checkbox, layout.window_ref, desc);

  // Add the extra offset for the next UI element
  layout.current_offset += layout.extra_offset;
}

void ui_layout_push_slider(UILayout& layout, 
                           f32* value,
                           const f32 min, 
                           const f32 max,
                           const f32 step,
                           const Vec4& color, 
                           const Vec4& notch_color, 
                           const i32 bind_id,
                           const Vec2& layout_padding) {
  // Create the slider

  layout.sliders.push_back(UISlider{});
  UISlider* slider = &layout.sliders[layout.sliders.size() - 1];

  UISliderDesc desc = {
    .anchor = layout.current_anchor,
    .offset = layout.current_offset + layout_padding, 
   
    .value = value, 
    .min   = min, 
    .max   = max,
    .step  = step,

    .bind_id = (bind_id == -1) ? (u32)(layout.sliders.size() - 1) : bind_id,

    .color       = color,
    .notch_color = notch_color,
  };
  ui_slider_create(slider, layout.window_ref, desc);

  // Add the extra offset for the next UI element
  layout.current_offset += layout.extra_offset;
}

void ui_layout_push_image(UILayout& layout, 
                          const ResourceID& texture_id, 
                          const Vec2& size, 
                          const Vec4& tint, 
                          const Vec2& layout_padding) {
  // Create the image

  layout.images.push_back(UIImage{});
  UIImage* image = &layout.images[layout.images.size() - 1];

  UIImageDesc desc = {
    .texture_id = texture_id, 
    .size       = size, 
    .anchor     = layout.current_anchor, 
    .offset     = layout.current_offset + layout_padding, 
    .tint       = tint,
  };
  ui_image_create(image, layout.window_ref, desc);

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
  
  // Render the checkboxes
  for(auto& checkbox : layout.checkboxes) {
    ui_checkbox_render((UICheckbox&)checkbox);
  }
  
  // Render the sliders
  for(auto& slider : layout.sliders) {
    ui_slider_render((UISlider&)slider);
  }
  
  // Render the images
  for(auto& image : layout.images) {
    ui_image_render((UIImage&)image);
  }
}

/// UILayout functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
