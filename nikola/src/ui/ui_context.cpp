#include "nikola/nikola_ui.h"
#include "nikola/nikola_math.h"
#include "nikola/nikola_event.h"
#include "nikola/nikola_input.h"
#include "nikola/nikola_timer.h"

#include <RmlUi/Core.h>
#include <RmlUi/Core/Types.h>
#include <RmlUi/Core/Input.h>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static Rml::Input::KeyIdentifier get_rml_key_code(const Key key) {
  switch(key) {
    case KEY_SPACE:
      return Rml::Input::KeyIdentifier::KI_SPACE;
    case KEY_APOSTROPHE:
      return Rml::Input::KeyIdentifier::KI_OEM_8;
    case KEY_COMMA:
      return Rml::Input::KeyIdentifier::KI_OEM_COMMA;
    case KEY_MINUS:
      return Rml::Input::KeyIdentifier::KI_OEM_MINUS;
    case KEY_PERIOD:
      return Rml::Input::KeyIdentifier::KI_OEM_PERIOD;
    case KEY_SLASH:
      return Rml::Input::KeyIdentifier::KI_OEM_2;
    case KEY_ZERO:
      return Rml::Input::KeyIdentifier::KI_0;
    case KEY_1:
      return Rml::Input::KeyIdentifier::KI_1;
    case KEY_2:
      return Rml::Input::KeyIdentifier::KI_2;
    case KEY_3:
      return Rml::Input::KeyIdentifier::KI_3;
    case KEY_4:
      return Rml::Input::KeyIdentifier::KI_4;
    case KEY_5:
      return Rml::Input::KeyIdentifier::KI_5;
    case KEY_6:
      return Rml::Input::KeyIdentifier::KI_6;
    case KEY_7:
      return Rml::Input::KeyIdentifier::KI_7;
    case KEY_8:
      return Rml::Input::KeyIdentifier::KI_8;
    case KEY_9:
      return Rml::Input::KeyIdentifier::KI_9;
    case KEY_SEMICOLON:
      return Rml::Input::KeyIdentifier::KI_OEM_1;
    case KEY_EQUAL:
      return Rml::Input::KeyIdentifier::KI_OEM_PLUS;
    case KEY_A:
      return Rml::Input::KeyIdentifier::KI_A;
    case KEY_B:
      return Rml::Input::KeyIdentifier::KI_B;
    case KEY_C:
      return Rml::Input::KeyIdentifier::KI_C;
    case KEY_D:
      return Rml::Input::KeyIdentifier::KI_D;
    case KEY_E:
      return Rml::Input::KeyIdentifier::KI_E;
    case KEY_F:
      return Rml::Input::KeyIdentifier::KI_F;
    case KEY_G:
      return Rml::Input::KeyIdentifier::KI_G;
    case KEY_H:
      return Rml::Input::KeyIdentifier::KI_H;
    case KEY_I:
      return Rml::Input::KeyIdentifier::KI_I;
    case KEY_J:
      return Rml::Input::KeyIdentifier::KI_J;
    case KEY_K:
      return Rml::Input::KeyIdentifier::KI_K;
    case KEY_L:
      return Rml::Input::KeyIdentifier::KI_L;
    case KEY_M:
      return Rml::Input::KeyIdentifier::KI_M;
    case KEY_N:
      return Rml::Input::KeyIdentifier::KI_N;
    case KEY_O:
      return Rml::Input::KeyIdentifier::KI_O;
    case KEY_P:
      return Rml::Input::KeyIdentifier::KI_P;
    case KEY_Q:
      return Rml::Input::KeyIdentifier::KI_Q;
    case KEY_R:
      return Rml::Input::KeyIdentifier::KI_R;
    case KEY_S:
      return Rml::Input::KeyIdentifier::KI_S;
    case KEY_T:
      return Rml::Input::KeyIdentifier::KI_T;
    case KEY_U:
      return Rml::Input::KeyIdentifier::KI_U;
    case KEY_V:
      return Rml::Input::KeyIdentifier::KI_V;
    case KEY_W:
      return Rml::Input::KeyIdentifier::KI_W;
    case KEY_X:
      return Rml::Input::KeyIdentifier::KI_X;
    case KEY_Y:
      return Rml::Input::KeyIdentifier::KI_Y;
    case KEY_Z:
      return Rml::Input::KeyIdentifier::KI_Z;
    case KEY_LEFT_BRACKET:
      return Rml::Input::KeyIdentifier::KI_OEM_4;
    case KEY_BACKSLASH:
      return Rml::Input::KeyIdentifier::KI_OEM_5;
    case KEY_RIGHT_BRACKET:
      return Rml::Input::KeyIdentifier::KI_OEM_6;
    case KEY_GRAVE_ACCENT:
      return Rml::Input::KeyIdentifier::KI_OEM_3;
    case KEY_WORLD_1:
    case KEY_WORLD_2:
      return Rml::Input::KeyIdentifier::KI_UNKNOWN; // @NOTE: I'm not sure if there are equivelants to these keys in RmlUi
    case KEY_ESCAPE:
      return Rml::Input::KeyIdentifier::KI_ESCAPE;
    case KEY_ENTER:
      return Rml::Input::KeyIdentifier::KI_RETURN;
    case KEY_TAB:
      return Rml::Input::KeyIdentifier::KI_TAB;
    case KEY_BACKSPACE:
      return Rml::Input::KeyIdentifier::KI_BACK;
    case KEY_INSERT:
      return Rml::Input::KeyIdentifier::KI_INSERT;
    case KEY_DELETE:
      return Rml::Input::KeyIdentifier::KI_DELETE;
    case KEY_RIGHT:
      return Rml::Input::KeyIdentifier::KI_RIGHT;
    case KEY_LEFT:
      return Rml::Input::KeyIdentifier::KI_LEFT;
    case KEY_DOWN:
      return Rml::Input::KeyIdentifier::KI_DOWN;
    case KEY_UP:
      return Rml::Input::KeyIdentifier::KI_UP;
    case KEY_PAGE_UP:
      return Rml::Input::KeyIdentifier::KI_PRIOR;
    case KEY_PAGE_DOW:
      return Rml::Input::KeyIdentifier::KI_NEXT;
    case KEY_HOME:
      return Rml::Input::KeyIdentifier::KI_BROWSER_HOME;
    case KEY_END:
      return Rml::Input::KeyIdentifier::KI_END;
    case KEY_CAPS_LOCK:
      return Rml::Input::KeyIdentifier::KI_CAPITAL;
    case KEY_SCROLL_LOCK:
      return Rml::Input::KeyIdentifier::KI_SCROLL;
    case KEY_NUM_LOCK:
      return Rml::Input::KeyIdentifier::KI_NUMLOCK;
    case KEY_PRINT_SCREEN:
      return Rml::Input::KeyIdentifier::KI_PRINT;
    case KEY_PAUSE:
      return Rml::Input::KeyIdentifier::KI_PAUSE;
    case KEY_F1:
      return Rml::Input::KeyIdentifier::KI_F1;
    case KEY_F2:
      return Rml::Input::KeyIdentifier::KI_F2;
    case KEY_F3:
      return Rml::Input::KeyIdentifier::KI_F3;
    case KEY_F4:
      return Rml::Input::KeyIdentifier::KI_F4;
    case KEY_F5:
      return Rml::Input::KeyIdentifier::KI_F5;
    case KEY_F6:
      return Rml::Input::KeyIdentifier::KI_F6;
    case KEY_F7:
      return Rml::Input::KeyIdentifier::KI_F7;
    case KEY_F8:
      return Rml::Input::KeyIdentifier::KI_F8;
    case KEY_F9:
      return Rml::Input::KeyIdentifier::KI_F9;
    case KEY_F10:
      return Rml::Input::KeyIdentifier::KI_F10;
    case KEY_F11:
      return Rml::Input::KeyIdentifier::KI_F11;
    case KEY_F12:
      return Rml::Input::KeyIdentifier::KI_F12;
    case KEY_F13:
      return Rml::Input::KeyIdentifier::KI_F13;
    case KEY_F14:
      return Rml::Input::KeyIdentifier::KI_F14;
    case KEY_F15:
      return Rml::Input::KeyIdentifier::KI_F15;
    case KEY_F16:
      return Rml::Input::KeyIdentifier::KI_F16;
    case KEY_F17:
      return Rml::Input::KeyIdentifier::KI_F17;
    case KEY_F18:
      return Rml::Input::KeyIdentifier::KI_F18;
    case KEY_F19:
      return Rml::Input::KeyIdentifier::KI_F19;
    case KEY_F20:
      return Rml::Input::KeyIdentifier::KI_F20;
    case KEY_F21:
      return Rml::Input::KeyIdentifier::KI_F21;
    case KEY_F22:
      return Rml::Input::KeyIdentifier::KI_F22;
    case KEY_F23:
      return Rml::Input::KeyIdentifier::KI_F23;
    case KEY_F24:
      return Rml::Input::KeyIdentifier::KI_F24;
    case KEY_F25:
      return Rml::Input::KeyIdentifier::KI_UNKNOWN;
    case KEY_KP_0:
      return Rml::Input::KeyIdentifier::KI_NUMPAD0;
    case KEY_KP_1:
      return Rml::Input::KeyIdentifier::KI_NUMPAD1;
    case KEY_KP_2:
      return Rml::Input::KeyIdentifier::KI_NUMPAD2;
    case KEY_KP_3:
      return Rml::Input::KeyIdentifier::KI_NUMPAD3;
    case KEY_KP_4:
      return Rml::Input::KeyIdentifier::KI_NUMPAD4;
    case KEY_KP_5:
      return Rml::Input::KeyIdentifier::KI_NUMPAD5;
    case KEY_KP_6:
      return Rml::Input::KeyIdentifier::KI_NUMPAD6;
    case KEY_KP_7:
      return Rml::Input::KeyIdentifier::KI_NUMPAD7;
    case KEY_KP_8:
      return Rml::Input::KeyIdentifier::KI_NUMPAD8;
    case KEY_KP_9:
      return Rml::Input::KeyIdentifier::KI_NUMPAD9;
    case KEY_KP_DECIMAL:
      return Rml::Input::KeyIdentifier::KI_DECIMAL;
    case KEY_KP_DIVIDE:
      return Rml::Input::KeyIdentifier::KI_DIVIDE;
    case KEY_KP_MULTIPLY:
      return Rml::Input::KeyIdentifier::KI_MULTIPLY;
    case KEY_KP_SUBTRACT:
      return Rml::Input::KeyIdentifier::KI_SUBTRACT;
    case KEY_KP_ADD:
      return Rml::Input::KeyIdentifier::KI_ADD;
    case KEY_KP_ENTER:
      return Rml::Input::KeyIdentifier::KI_NUMPADENTER;
    case KEY_KP_EQUAL:
      return Rml::Input::KeyIdentifier::KI_OEM_NEC_EQUAL;
    case KEY_LEFT_SHIFT:
      return Rml::Input::KeyIdentifier::KI_LSHIFT;
    case KEY_LEFT_CONTROL:
      return Rml::Input::KeyIdentifier::KI_LCONTROL;
    case KEY_LEFT_ALT:
      return Rml::Input::KeyIdentifier::KI_LMENU;
    case KEY_LEFT_SUPER:
      return Rml::Input::KeyIdentifier::KI_LMETA;
    case KEY_RIGHT_SHIFT:
      return Rml::Input::KeyIdentifier::KI_RSHIFT;
    case KEY_RIGHT_CONTROL:
      return Rml::Input::KeyIdentifier::KI_RCONTROL;
    case KEY_RIGHT_ALT:
      return Rml::Input::KeyIdentifier::KI_RMENU;
    case KEY_RIGHT_SUPER:
      return Rml::Input::KeyIdentifier::KI_RMETA;
    case KEY_MENU:
      return Rml::Input::KeyIdentifier::KI_RMENU;
    default:
      return Rml::Input::KeyIdentifier::KI_UNKNOWN;
  }
}

static Rml::Input::KeyModifier get_rml_key_modifier(const Key key) {
  switch(key) {
    case KEY_LEFT_CONTROL:
    case KEY_RIGHT_CONTROL:
      return Rml::Input::KeyModifier::KM_CTRL;
    case KEY_LEFT_SHIFT:
    case KEY_RIGHT_SHIFT:
      return Rml::Input::KeyModifier::KM_SHIFT;
    case KEY_LEFT_ALT:
    case KEY_RIGHT_ALT:
      return Rml::Input::KeyModifier::KM_ALT;
    case KEY_CAPS_LOCK:
      return Rml::Input::KeyModifier::KM_CAPSLOCK;
    case KEY_NUM_LOCK:
      return Rml::Input::KeyModifier::KM_NUMLOCK;
    case KEY_SCROLL_LOCK:
      return Rml::Input::KeyModifier::KM_SCROLLLOCK;
    default:
      return (Rml::Input::KeyModifier)0;
  }
}

/// Private functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Callbacks

static bool mouse_callbacks(const Event& event, const void* dispatcher, const void* listener) {
  UIContext* ui_ctx = (UIContext*)listener;

  switch(event.type) {
    case EVENT_MOUSE_MOVED:
      ui_ctx->ProcessMouseMove((i32)event.mouse_pos_x, (i32)event.mouse_pos_x, get_rml_key_modifier((Key)event.key_modifier));
      break;
    case EVENT_MOUSE_BUTTON_PRESSED:
      ui_ctx->ProcessMouseButtonDown(event.mouse_button_pressed, get_rml_key_modifier((Key)event.key_modifier));
      break;
    case EVENT_MOUSE_BUTTON_RELEASED:
      ui_ctx->ProcessMouseButtonUp(event.mouse_button_released, get_rml_key_modifier((Key)event.key_modifier));
      break;
    case EVENT_MOUSE_SCROLL_WHEEL:
      ui_ctx->ProcessMouseWheel(Rml::Vector2f(event.mouse_scroll_value, event.mouse_scroll_value), get_rml_key_modifier((Key)event.key_modifier));
      break;
    case EVENT_MOUSE_LEAVE:
      ui_ctx->ProcessMouseLeave();
      break;
    default:
      return true;
  }

  return true;
}

static bool key_callbacks(const Event& event, const void* dispatcher, const void* listener) {
  UIContext* ui_ctx = (UIContext*)listener;

  switch(event.type) {
    case EVENT_KEY_PRESSED:
      ui_ctx->ProcessKeyDown(get_rml_key_code((Key)event.key_pressed), get_rml_key_modifier((Key)event.key_modifier));
      break;
    case EVENT_KEY_RELEASED:
      ui_ctx->ProcessKeyDown(get_rml_key_code((Key)event.key_released), get_rml_key_modifier((Key)event.key_modifier));
      break;
    default:
      return true;
  }

  return true;
}

/// Callbacks
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIContext functions

UIContext* ui_context_create(const String& name, const IVec2 bounds) {
  UIContext* ctx = Rml::CreateContext(name.c_str(), Rml::Vector2i(bounds.x, bounds.y)); 

  // Listen to events

  event_listen(EVENT_MOUSE_MOVED, mouse_callbacks, ctx);
  event_listen(EVENT_MOUSE_BUTTON_PRESSED, mouse_callbacks, ctx);
  event_listen(EVENT_MOUSE_BUTTON_RELEASED, mouse_callbacks, ctx);
  event_listen(EVENT_MOUSE_SCROLL_WHEEL, mouse_callbacks, ctx);
  event_listen(EVENT_MOUSE_LEAVE, mouse_callbacks, ctx);
  
  event_listen(EVENT_KEY_PRESSED, key_callbacks, ctx);
  event_listen(EVENT_KEY_RELEASED, key_callbacks, ctx);

  return ctx;
}

void ui_context_destroy(UIContext* ui_ctx) {
  if(!ui_ctx) {
    return;
  }

  Rml::RemoveContext(ui_ctx->GetName());
}

void ui_context_update(UIContext* ui_ctx) {
  NIKOLA_ASSERT(ui_ctx, "Invalid UIContext given to ui_context_update");
  NIKOLA_PROFILE_FUNCTION();

  ui_ctx->Update();
}

void ui_context_render(UIContext* ui_ctx) {
  NIKOLA_ASSERT(ui_ctx, "Invalid UIContext given to ui_context_render");
  NIKOLA_PROFILE_FUNCTION();
  
  ui_ctx->Render();
}

void ui_context_enable_mouse_cursor(UIContext* ui_ctx, const bool enable) {
  NIKOLA_ASSERT(ui_ctx, "Invalid UIContext given to ui_context_enable_mouse_cursor");
  ui_ctx->EnableMouseCursor(enable);
}

void ui_context_activate_theme(UIContext* ui_ctx, const String& theme_name, const bool active) {
  NIKOLA_ASSERT(ui_ctx, "Invalid UIContext given to ui_context_activate_theme");
  ui_ctx->ActivateTheme(theme_name, active);
}

bool ui_context_is_theme_active(UIContext* ui_ctx, const String& theme_name) {
  NIKOLA_ASSERT(ui_ctx, "Invalid UIContext given to ui_context_is_theme_active");
  return ui_ctx->IsThemeActive(theme_name);
}

/// UIContext functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
