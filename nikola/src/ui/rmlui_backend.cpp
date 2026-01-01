#include "nikola/nikola_ui.h"
#include "nikola/nikola_math.h"
#include "nikola/nikola_event.h"
#include "nikola/nikola_input.h"
#include "nikola/nikola_timer.h"

#include <RmlUi/Core.h>
#include <RmlUi/Core/Types.h>
#include <RmlUi/Core/Input.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/ElementUtilities.h>
#include <RmlUi/Core/Elements/ElementProgress.h>
#include <RmlUi/Core/Elements/ElementFormControlInput.h>
#include <RmlUi/Core/Elements/ElementFormControlSelect.h>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Function declarations

static Rml::Input::KeyIdentifier get_rml_key_code(const Key key);
static Rml::Input::KeyModifier get_rml_key_modifier(const Key key);
static Key get_nikola_key_code(const Rml::Input::KeyIdentifier key);

/// Function declarations
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// NKEventListener
class NKEventListener : public Rml::EventListener {
  public:
    NKEventListener() = default;

  public:
    void ProcessEvent(Rml::Event& event) override {
      Event nk_event = {
        .type = EVENT_INVALID,
      };
      
      // Follow the our event struct depending on the event type

      switch(event.GetId()) {
        case Rml::EventId::Click:
        case Rml::EventId::Mousedown:
          nk_event.type                 = EVENT_UI_ELEMENT_CLICKED;
          nk_event.element              = event.GetTargetElement();
          nk_event.mouse_button_pressed = event.GetParameter<i32>("button", 0);
          break;
        case Rml::EventId::Dblclick:
          nk_event.type                 = EVENT_UI_ELEMENT_DOUBLE_CLICKED;
          nk_event.element              = event.GetTargetElement();
          nk_event.mouse_button_pressed = event.GetParameter<i32>("button", 0);
          break;
        case Rml::EventId::Mousescroll:
        case Rml::EventId::Scroll:
          nk_event.type               = EVENT_UI_ELEMENT_SCROLLED;
          nk_event.element            = event.GetTargetElement();
          nk_event.mouse_scroll_value = event.GetParameter<f32>("wheel_delta_y", 0.0f);
          break;
        case Rml::EventId::Mouseover:
          nk_event.type    = EVENT_UI_ELEMENT_ENTERED;
          nk_event.element = event.GetTargetElement();
          break;
        case Rml::EventId::Mouseout:
          nk_event.type    = EVENT_UI_ELEMENT_EXITED;
          nk_event.element = event.GetTargetElement();
          break;
        case Rml::EventId::Mousemove:
          nk_event.type    = EVENT_UI_ELEMENT_MOUSE_MOVED;
          nk_event.element = event.GetTargetElement();
          break;
        case Rml::EventId::Keydown:
          nk_event.type        = EVENT_UI_ELEMENT_KEY_DOWN;
          nk_event.element     = event.GetTargetElement();
          nk_event.key_pressed = get_nikola_key_code(event.GetParameter<Rml::Input::KeyIdentifier>("key_identifier", Rml::Input::KeyIdentifier::KI_UNKNOWN));
          break;
        case Rml::EventId::Keyup:
          nk_event.type         = EVENT_UI_ELEMENT_KEY_UP;
          nk_event.element      = event.GetTargetElement();
          nk_event.key_released = get_nikola_key_code(event.GetParameter<Rml::Input::KeyIdentifier>("key_identifier", Rml::Input::KeyIdentifier::KI_UNKNOWN));
          break;
        case Rml::EventId::Focus:
          nk_event.type    = EVENT_UI_ELEMENT_FOCUSED;
          nk_event.element = event.GetTargetElement();
          break;
        case Rml::EventId::Blur:
          nk_event.type    = EVENT_UI_ELEMENT_BLURRED;
          nk_event.element = event.GetTargetElement();
          break;
        case Rml::EventId::Load:
          nk_event.type = EVENT_UI_DOCUMENT_LOADED;
          break;
        case Rml::EventId::Unload:
          nk_event.type = EVENT_UI_DOCUMENT_UNLOADED;
          break;
        case Rml::EventId::Show:
          nk_event.type = EVENT_UI_DOCUMENT_SHOWN;
          break;
        case Rml::EventId::Hide:
          nk_event.type = EVENT_UI_DOCUMENT_HIDDEN;
          break;
        case Rml::EventId::Dragstart:
          nk_event.type            = EVENT_UI_ELEMENT_DRAG_STARTED; 
          nk_event.dragged_element = (UIElement*)event.GetParameter<UIElement*>("drag_element", nullptr);
          break;
        case Rml::EventId::Dragend:
          nk_event.type            = EVENT_UI_ELEMENT_DRAG_ENDED; 
          nk_event.dragged_element = (UIElement*)event.GetParameter<UIElement*>("drag_element", nullptr);
          break;
        case Rml::EventId::Drag:
          nk_event.type            = EVENT_UI_ELEMENT_DRAGED; 
          nk_event.element         = event.GetTargetElement();
          nk_event.dragged_element = (UIElement*)event.GetParameter<UIElement*>("drag_element", nullptr);
          break;
        case Rml::EventId::Dragover:
          nk_event.type            = EVENT_UI_ELEMENT_DRAG_ENTERED; 
          nk_event.element         = event.GetTargetElement();
          nk_event.dragged_element = (UIElement*)event.GetParameter<UIElement*>("drag_element", nullptr);
          break;
        case Rml::EventId::Dragout:
          nk_event.type            = EVENT_UI_ELEMENT_DRAG_EXITED; 
          nk_event.element         = event.GetTargetElement();
          nk_event.dragged_element = (UIElement*)event.GetParameter<UIElement*>("drag_element", nullptr);
          break;
        case Rml::EventId::Dragmove:
          nk_event.type            = EVENT_UI_ELEMENT_DRAG_MOVED; 
          nk_event.element         = event.GetTargetElement();
          nk_event.dragged_element = (UIElement*)event.GetParameter<UIElement*>("drag_element", nullptr);
          break;
        case Rml::EventId::Dragdrop:
          nk_event.type            = EVENT_UI_ELEMENT_DRAG_DROPPED; 
          nk_event.dragged_element = (UIElement*)event.GetParameter<UIElement*>("drag_element", nullptr);
          break;
        case Rml::EventId::Animationend:
          nk_event.type    = EVENT_UI_ELEMENT_ANIMATION_ENDED;
          nk_event.element = event.GetTargetElement();
          break;
        case Rml::EventId::Transitionend:
          nk_event.type    = EVENT_UI_ELEMENT_TRANSITION_ENDED;
          nk_event.element = event.GetTargetElement();
          break;
        case Rml::EventId::Tabchange:
          nk_event.type      = EVENT_UI_ELEMENT_TAB_CHANGED;
          nk_event.tab_index = event.GetParameter<i32>("tab_index", 0);
          nk_event.element   = event.GetTargetElement();
          break;
        default:
          break;
      }

      // Dispatch the event
    
      if(nk_event.type != EVENT_INVALID) {
        event_dispatch(nk_event);
      }
    }
};

static NKEventListener s_listener;
/// NKEventListener
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Private functions

static Rml::Input::KeyIdentifier get_rml_key_code(const Key key) {
  switch(key) {
    case KEY_SPACE:
      return Rml::Input::KeyIdentifier::KI_SPACE;
    case KEY_APOSTROPHE:
      return Rml::Input::KeyIdentifier::KI_OEM_7;
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

static Key get_nikola_key_code(const Rml::Input::KeyIdentifier key) {
  switch(key) {
    case Rml::Input::KeyIdentifier::KI_SPACE:
      return KEY_SPACE;
    case Rml::Input::KeyIdentifier::KI_OEM_8:
      return KEY_APOSTROPHE;
    case Rml::Input::KeyIdentifier::KI_OEM_COMMA:
      return KEY_COMMA;
    case Rml::Input::KeyIdentifier::KI_OEM_MINUS:
      return KEY_MINUS;
    case Rml::Input::KeyIdentifier::KI_OEM_PERIOD:
      return KEY_PERIOD;
    case Rml::Input::KeyIdentifier::KI_OEM_2:
      return KEY_SLASH;
    case Rml::Input::KeyIdentifier::KI_0:
      return KEY_ZERO;
    case Rml::Input::KeyIdentifier::KI_1:
      return KEY_1;
    case Rml::Input::KeyIdentifier::KI_2:
      return KEY_2;
    case Rml::Input::KeyIdentifier::KI_3:
      return KEY_3;
    case Rml::Input::KeyIdentifier::KI_4:
      return KEY_4;
    case Rml::Input::KeyIdentifier::KI_5:
      return KEY_5;
    case Rml::Input::KeyIdentifier::KI_6:
      return KEY_6;
    case Rml::Input::KeyIdentifier::KI_7:
      return KEY_7;
    case Rml::Input::KeyIdentifier::KI_8:
      return KEY_8;
    case Rml::Input::KeyIdentifier::KI_9:
      return KEY_9;
    case Rml::Input::KeyIdentifier::KI_OEM_1:
      return KEY_SEMICOLON;
    case Rml::Input::KeyIdentifier::KI_OEM_PLUS:
      return KEY_EQUAL;
    case Rml::Input::KeyIdentifier::KI_A:
      return KEY_A;
    case Rml::Input::KeyIdentifier::KI_B:
      return KEY_B;
    case Rml::Input::KeyIdentifier::KI_C:
      return KEY_C;
    case Rml::Input::KeyIdentifier::KI_D:
      return KEY_D;
    case Rml::Input::KeyIdentifier::KI_E:
      return KEY_E;
    case Rml::Input::KeyIdentifier::KI_F:
      return KEY_F;
    case Rml::Input::KeyIdentifier::KI_G:
      return KEY_G;
    case Rml::Input::KeyIdentifier::KI_H:
      return KEY_H;
    case Rml::Input::KeyIdentifier::KI_I:
      return KEY_I;
    case Rml::Input::KeyIdentifier::KI_J:
      return KEY_J;
    case Rml::Input::KeyIdentifier::KI_K:
      return KEY_K;
    case Rml::Input::KeyIdentifier::KI_L:
      return KEY_L;
    case Rml::Input::KeyIdentifier::KI_M:
      return KEY_M;
    case Rml::Input::KeyIdentifier::KI_N:
      return KEY_N;
    case Rml::Input::KeyIdentifier::KI_O:
      return KEY_O;
    case Rml::Input::KeyIdentifier::KI_P:
      return KEY_P;
    case Rml::Input::KeyIdentifier::KI_Q:
      return KEY_Q;
    case Rml::Input::KeyIdentifier::KI_R:
      return KEY_R;
    case Rml::Input::KeyIdentifier::KI_S:
      return KEY_S;
    case Rml::Input::KeyIdentifier::KI_T:
      return KEY_T;
    case Rml::Input::KeyIdentifier::KI_U:
      return KEY_U;
    case Rml::Input::KeyIdentifier::KI_V:
      return KEY_V;
    case Rml::Input::KeyIdentifier::KI_W:
      return KEY_W;
    case Rml::Input::KeyIdentifier::KI_X:
      return KEY_X;
    case Rml::Input::KeyIdentifier::KI_Y:
      return KEY_Y;
    case Rml::Input::KeyIdentifier::KI_Z:
      return KEY_Z;
    case Rml::Input::KeyIdentifier::KI_OEM_4:
      return KEY_LEFT_BRACKET;
    case Rml::Input::KeyIdentifier::KI_OEM_5:
      return KEY_BACKSLASH;
    case Rml::Input::KeyIdentifier::KI_OEM_6:
      return KEY_RIGHT_BRACKET;
    case Rml::Input::KeyIdentifier::KI_OEM_3:
      return KEY_GRAVE_ACCENT;
    case Rml::Input::KeyIdentifier::KI_ESCAPE:
      return KEY_ESCAPE;
    case Rml::Input::KeyIdentifier::KI_RETURN:
      return KEY_ENTER;
    case Rml::Input::KeyIdentifier::KI_TAB:
      return KEY_TAB;
    case Rml::Input::KeyIdentifier::KI_BACK:
      return KEY_BACKSPACE;
    case Rml::Input::KeyIdentifier::KI_INSERT:
      return KEY_INSERT;
    case Rml::Input::KeyIdentifier::KI_DELETE:
      return KEY_DELETE;
    case Rml::Input::KeyIdentifier::KI_RIGHT:
      return KEY_RIGHT;
    case Rml::Input::KeyIdentifier::KI_LEFT:
      return KEY_LEFT;
    case Rml::Input::KeyIdentifier::KI_DOWN:
      return KEY_DOWN;
    case Rml::Input::KeyIdentifier::KI_UP:
      return KEY_UP;
    case Rml::Input::KeyIdentifier::KI_PRIOR:
      return KEY_PAGE_UP;
    case Rml::Input::KeyIdentifier::KI_NEXT:
      return KEY_PAGE_DOW;
    case Rml::Input::KeyIdentifier::KI_BROWSER_HOME:
      return KEY_HOME;
    case Rml::Input::KeyIdentifier::KI_END:
      return KEY_END;
    case Rml::Input::KeyIdentifier::KI_CAPITAL:
      return KEY_CAPS_LOCK;
    case Rml::Input::KeyIdentifier::KI_SCROLL:
      return KEY_SCROLL_LOCK;
    case Rml::Input::KeyIdentifier::KI_NUMLOCK:
      return KEY_NUM_LOCK;
    case Rml::Input::KeyIdentifier::KI_PRINT:
      return KEY_PRINT_SCREEN;
    case Rml::Input::KeyIdentifier::KI_PAUSE:
      return KEY_PAUSE;
    case Rml::Input::KeyIdentifier::KI_F1:
      return KEY_F1;
    case Rml::Input::KeyIdentifier::KI_F2:
      return KEY_F2;
    case Rml::Input::KeyIdentifier::KI_F3:
      return KEY_F3;
    case Rml::Input::KeyIdentifier::KI_F4:
      return KEY_F4;
    case Rml::Input::KeyIdentifier::KI_F5:
      return KEY_F5;
    case Rml::Input::KeyIdentifier::KI_F6:
      return KEY_F6;
    case Rml::Input::KeyIdentifier::KI_F7:
      return KEY_F7;
    case Rml::Input::KeyIdentifier::KI_F8:
      return KEY_F8;
    case Rml::Input::KeyIdentifier::KI_F9:
      return KEY_F9;
    case Rml::Input::KeyIdentifier::KI_F10:
      return KEY_F10;
    case Rml::Input::KeyIdentifier::KI_F11:
      return KEY_F11;
    case Rml::Input::KeyIdentifier::KI_F12:
      return KEY_F12;
    case Rml::Input::KeyIdentifier::KI_F13:
      return KEY_F13;
    case Rml::Input::KeyIdentifier::KI_F14:
      return KEY_F14;
    case Rml::Input::KeyIdentifier::KI_F15:
      return KEY_F15;
    case Rml::Input::KeyIdentifier::KI_F16:
      return KEY_F16;
    case Rml::Input::KeyIdentifier::KI_F17:
      return KEY_F17;
    case Rml::Input::KeyIdentifier::KI_F18:
      return KEY_F18;
    case Rml::Input::KeyIdentifier::KI_F19:
      return KEY_F19;
    case Rml::Input::KeyIdentifier::KI_F20:
      return KEY_F20;
    case Rml::Input::KeyIdentifier::KI_F21:
      return KEY_F21;
    case Rml::Input::KeyIdentifier::KI_F22:
      return KEY_F22;
    case Rml::Input::KeyIdentifier::KI_F23:
      return KEY_F23;
    case Rml::Input::KeyIdentifier::KI_F24:
      return KEY_F24;
    case Rml::Input::KeyIdentifier::KI_NUMPAD0:
      return KEY_KP_0;
    case Rml::Input::KeyIdentifier::KI_NUMPAD1:
      return KEY_KP_1;
    case Rml::Input::KeyIdentifier::KI_NUMPAD2:
      return KEY_KP_2;
    case Rml::Input::KeyIdentifier::KI_NUMPAD3:
      return KEY_KP_3;
    case Rml::Input::KeyIdentifier::KI_NUMPAD4:
      return KEY_KP_4;
    case Rml::Input::KeyIdentifier::KI_NUMPAD5:
      return KEY_KP_5;
    case Rml::Input::KeyIdentifier::KI_NUMPAD6:
      return KEY_KP_6;
    case Rml::Input::KeyIdentifier::KI_NUMPAD7:
      return KEY_KP_7;
    case Rml::Input::KeyIdentifier::KI_NUMPAD8:
      return KEY_KP_8;
    case Rml::Input::KeyIdentifier::KI_NUMPAD9:
      return KEY_KP_9;
    case Rml::Input::KeyIdentifier::KI_DECIMAL:
      return KEY_KP_DECIMAL;
    case Rml::Input::KeyIdentifier::KI_DIVIDE:
      return KEY_KP_DIVIDE;
    case Rml::Input::KeyIdentifier::KI_MULTIPLY:
      return KEY_KP_MULTIPLY;
    case Rml::Input::KeyIdentifier::KI_SUBTRACT:
      return KEY_KP_SUBTRACT;
    case Rml::Input::KeyIdentifier::KI_ADD:
      return KEY_KP_ADD;
    case Rml::Input::KeyIdentifier::KI_NUMPADENTER:
      return KEY_KP_ENTER;
    case Rml::Input::KeyIdentifier::KI_OEM_NEC_EQUAL:
      return KEY_KP_EQUAL;
    case Rml::Input::KeyIdentifier::KI_LSHIFT:
      return KEY_LEFT_SHIFT;
    case Rml::Input::KeyIdentifier::KI_LCONTROL:
      return KEY_LEFT_CONTROL;
    case Rml::Input::KeyIdentifier::KI_LMENU:
      return KEY_LEFT_ALT;
    case Rml::Input::KeyIdentifier::KI_LMETA:
      return KEY_LEFT_SUPER;
    case Rml::Input::KeyIdentifier::KI_RSHIFT:
      return KEY_RIGHT_SHIFT;
    case Rml::Input::KeyIdentifier::KI_RCONTROL:
      return KEY_RIGHT_CONTROL;
    case Rml::Input::KeyIdentifier::KI_RMENU:
      return KEY_RIGHT_ALT;
    case Rml::Input::KeyIdentifier::KI_RMETA:
      return KEY_RIGHT_SUPER;
    default:
      return KEY_UNKNOWN;
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
      ui_ctx->ProcessMouseMove((i32)event.mouse_pos_x, (i32)event.mouse_pos_y, get_rml_key_modifier((Key)event.key_modifier));
      break;
    case EVENT_MOUSE_BUTTON_PRESSED:
      ui_ctx->ProcessMouseButtonDown(event.mouse_button_pressed, get_rml_key_modifier((Key)event.key_modifier));
      break;
    case EVENT_MOUSE_BUTTON_RELEASED:
      ui_ctx->ProcessMouseButtonUp(event.mouse_button_released, get_rml_key_modifier((Key)event.key_modifier));
      break;
    case EVENT_MOUSE_SCROLL_WHEEL:
      ui_ctx->ProcessMouseWheel(-(f32)event.mouse_scroll_value, get_rml_key_modifier((Key)event.key_modifier));
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
      ui_ctx->ProcessKeyUp(get_rml_key_code((Key)event.key_released), get_rml_key_modifier((Key)event.key_modifier));
      break;
    default:
      return true;
  }

  return true;
}

static bool resize_callback(const Event& event, const void* dispatcher, const void* listener) {
  UIContext* ui_ctx = (UIContext*)listener;

  switch(event.type) {
    case EVENT_WINDOW_FRAMEBUFFER_RESIZED:
      ui_ctx->SetDimensions(Rml::Vector2i(event.window_framebuffer_width, event.window_framebuffer_width));
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
  
  event_listen(EVENT_WINDOW_FRAMEBUFFER_RESIZED, resize_callback, ctx);

  return ctx;
}

void ui_context_destroy(UIContext* ui_ctx) {
  if(!ui_ctx) {
    return;
  }

  Rml::RemoveContext(ui_ctx->GetName());
}

void ui_context_update(UIContext* ui_ctx) {
  NIKOLA_PROFILE_FUNCTION();
  ui_ctx->Update();
}

void ui_context_render(UIContext* ui_ctx) {
  NIKOLA_PROFILE_FUNCTION();
  ui_ctx->Render();
}

void ui_context_enable_mouse_cursor(UIContext* ui_ctx, const bool enable) {
  ui_ctx->EnableMouseCursor(enable);
}

void ui_context_activate_theme(UIContext* ui_ctx, const String& theme_name, const bool active) {
  ui_ctx->ActivateTheme(theme_name, active);
}

bool ui_context_is_theme_active(UIContext* ui_ctx, const String& theme_name) {
  return ui_ctx->IsThemeActive(theme_name);
}

UIElement* ui_context_get_hover_element(UIContext* ui_ctx) {
  return ui_ctx->GetHoverElement();
}

UIElement* ui_context_get_focus_element(UIContext* ui_ctx) {
  return ui_ctx->GetFocusElement();
}

UIElement* ui_context_get_root_element(UIContext* ui_ctx) {
  return ui_ctx->GetRootElement();
}

sizei ui_context_get_documents_count(UIContext* ui_ctx) {
  return (sizei)ui_ctx->GetNumDocuments();
}

UIDocument* ui_context_get_document(UIContext* ui_ctx, const String& doc_id) {
  return ui_ctx->GetDocument(doc_id);
}

UIDocument* ui_context_get_document(UIContext* ui_ctx, const sizei index) {
  return ui_ctx->GetDocument((i32)index);
}

/// UIContext functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIDocument functions

UIDocument* ui_document_load(UIContext* ui_ctx, const FilePath& doc_path) {
  NIKOLA_ASSERT(ui_ctx, "Invalid UIContext given to ui_document_load");
  return ui_ctx->LoadDocument(doc_path);
}

UIDocument* ui_document_load_from_memory(UIContext* ui_ctx, const String& doc_src) {
  NIKOLA_ASSERT(ui_ctx, "Invalid UIContext given to ui_document_load_from_memory");
  return ui_ctx->LoadDocumentFromMemory(doc_src);
}

UIDocument* ui_document_create(UIContext* ui_ctx, const String& maker_name) {
  NIKOLA_ASSERT(ui_ctx, "Invalid UIContext given to ui_document_create");
  return ui_ctx->CreateDocument(maker_name);
}

void ui_document_close(UIDocument* ui_doc) {
  ui_document_disable_events(ui_doc);
  ui_doc->Close();
}

void ui_document_show(UIDocument* ui_doc) {
  ui_doc->Show();
}

void ui_document_hide(UIDocument* ui_doc) {
  ui_doc->Hide();
}

bool ui_document_is_shown(UIDocument* ui_doc) {
  return ui_doc->IsVisible();
}

void ui_document_enable_events(UIDocument* ui_doc) {
  ui_doc->AddEventListener("show", &s_listener);
  ui_doc->AddEventListener("hide", &s_listener);
  ui_doc->AddEventListener("load", &s_listener);
  ui_doc->AddEventListener("unload", &s_listener);
}

void ui_document_disable_events(UIDocument* ui_doc) {
  ui_doc->RemoveEventListener("show", &s_listener);
  ui_doc->RemoveEventListener("hide", &s_listener);
  ui_doc->RemoveEventListener("load", &s_listener);
  ui_doc->RemoveEventListener("unload", &s_listener);
}

void ui_document_pull_to_front(UIDocument* ui_doc) {
  ui_doc->PullToFront();
}

void ui_document_push_to_back(UIDocument* ui_doc) {
  ui_doc->PushToBack();
}

void ui_document_reload_stylesheet(UIDocument* ui_doc) {
  ui_doc->ReloadStyleSheet();
}

void ui_document_append_child(UIDocument* ui_doc, UIElementPtr element) {
  ui_doc->AppendChild(std::move(element));
}

void ui_document_insert_before(UIDocument* ui_doc, UIElementPtr element, UIElement* adjacent_element) {
  ui_doc->InsertBefore(std::move(element), adjacent_element);
}

void ui_document_replace_child(UIDocument* ui_doc, UIElementPtr element, UIElement* other_element) {
  ui_doc->ReplaceChild(std::move(element), other_element);
}

void ui_document_remove_child(UIDocument* ui_doc, UIElement* element) {
  ui_doc->RemoveChild(element);
}

void ui_document_set_title(UIDocument* ui_doc, const String& title) {
  ui_doc->SetTitle(title);
}

const String& ui_document_get_title(const UIDocument* ui_doc) {
  return ui_doc->GetTitle();
}

const String& ui_document_get_source_url(const UIDocument* ui_doc) {
  return ui_doc->GetSourceURL();
}

UIContext* ui_document_get_context(UIDocument* ui_doc) {
  return ui_doc->GetContext();
}

UIElement* ui_document_get_element_by_id(UIDocument* ui_doc, const String& id) {
  return ui_doc->GetElementById(id);
}

void ui_document_get_elements_by_tag(UIDocument* ui_doc, const String& tag_name, DynamicArray<UIElement*>& out_elements) {
  return ui_doc->GetElementsByTagName(out_elements, tag_name);
}

void ui_document_get_elements_by_class(UIDocument* ui_doc, const String& class_name, DynamicArray<UIElement*>& out_elements) {
  return ui_doc->GetElementsByClassName(out_elements, class_name);
}

UIElement* ui_document_query_selector(UIDocument* ui_doc, const String& selector_name) {
  return ui_doc->QuerySelector(selector_name);
}

void ui_document_query_selector_all(UIDocument* ui_doc, const String& selector_name, DynamicArray<UIElement*>& out_elements) {
  return ui_doc->QuerySelectorAll(out_elements, selector_name);
}

/// UIDocument functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIElement functions

UIElementPtr ui_element_create(UIDocument* ui_doc, const String& name) {
  NIKOLA_ASSERT(ui_doc, "An invalid UIDocument given to ui_element_create");
  return ui_doc->CreateElement(name);
}

void ui_element_enable_events(UIElement* ui_element) {
  ui_element->AddEventListener("scroll", &s_listener);
  ui_element->AddEventListener("focus", &s_listener);
  ui_element->AddEventListener("blur", &s_listener);
  
  ui_element->AddEventListener("keydown", &s_listener);
  ui_element->AddEventListener("keyup", &s_listener);
  
  ui_element->AddEventListener("click", &s_listener);
  ui_element->AddEventListener("dbclick", &s_listener);
  
  ui_element->AddEventListener("mousedown", &s_listener);
  ui_element->AddEventListener("mousescroll", &s_listener);
  ui_element->AddEventListener("mouseover", &s_listener);
  ui_element->AddEventListener("mouseout", &s_listener);
  ui_element->AddEventListener("mousemove", &s_listener);
  
  ui_element->AddEventListener("dragstart", &s_listener);
  ui_element->AddEventListener("dragend", &s_listener);
  ui_element->AddEventListener("drag", &s_listener);
  
  ui_element->AddEventListener("dragover", &s_listener);
  ui_element->AddEventListener("dragout", &s_listener);
  ui_element->AddEventListener("dragmove", &s_listener);
  ui_element->AddEventListener("dragdrop", &s_listener);
  
  ui_element->AddEventListener("animationend", &s_listener);
  ui_element->AddEventListener("transitionend", &s_listener);
  
  ui_element->AddEventListener("tabchange", &s_listener);
}

void ui_element_disable_events(UIElement* ui_element) {
  ui_element->RemoveEventListener("scroll", &s_listener);
  ui_element->RemoveEventListener("focus", &s_listener);
  ui_element->RemoveEventListener("blur", &s_listener);
  
  ui_element->RemoveEventListener("keydown", &s_listener);
  ui_element->RemoveEventListener("keyup", &s_listener);
  
  ui_element->RemoveEventListener("click", &s_listener);
  ui_element->RemoveEventListener("dbclick", &s_listener);
  
  ui_element->RemoveEventListener("mousedown", &s_listener);
  ui_element->RemoveEventListener("mousescroll", &s_listener);
  ui_element->RemoveEventListener("mouseover", &s_listener);
  ui_element->RemoveEventListener("mouseout", &s_listener);
  ui_element->RemoveEventListener("mousemove", &s_listener);
  
  ui_element->RemoveEventListener("dragstart", &s_listener);
  ui_element->RemoveEventListener("dragend", &s_listener);
  ui_element->RemoveEventListener("drag", &s_listener);
  
  ui_element->RemoveEventListener("dragover", &s_listener);
  ui_element->RemoveEventListener("dragout", &s_listener);
  ui_element->RemoveEventListener("dragmove", &s_listener);
  ui_element->RemoveEventListener("dragdrop", &s_listener);
  
  ui_element->RemoveEventListener("animationend", &s_listener);
  ui_element->RemoveEventListener("transitionend", &s_listener);
  
  ui_element->RemoveEventListener("tabchange", &s_listener);
}

UIElementPtr ui_element_clone(UIElement* ui_element) {
  return ui_element->Clone();
}

void ui_element_focus(UIElement* ui_element) {
  ui_element->Focus();
}

void ui_element_blur(UIElement* ui_element) {
  ui_element->Blur();
}

void ui_element_append_child(UIElement* ui_element, UIElementPtr element) {
  ui_element->AppendChild(std::move(element));
}

void ui_element_insert_before(UIElement* ui_element, UIElementPtr element, UIElement* adjacent_element) {
  ui_element->InsertBefore(std::move(element), adjacent_element);
}

void ui_element_replace_child(UIElement* ui_element, UIElementPtr element, UIElement* other_element) {
  ui_element->ReplaceChild(std::move(element), other_element);
}

void ui_element_remove_child(UIElement* ui_element, UIElement* element) {
  ui_element->RemoveChild(element);
}

void ui_element_set_id(UIElement* ui_element, const String& id) {
  ui_element->SetId(id);
}

void ui_element_set_inner_html(UIElement* ui_element, const String& html) {
  ui_element->SetInnerRML(html);
}

UIElement* ui_element_get_element_by_id(UIElement* ui_element, const String& id) {
  return ui_element->GetElementById(id);
}

void ui_element_get_elements_by_tag(UIElement* ui_element, const String& tag_name, DynamicArray<UIElement*>& out_elements) {
  ui_element->GetElementsByTagName(out_elements, tag_name);
}

void ui_element_get_elements_by_class(UIElement* ui_element, const String& class_name, DynamicArray<UIElement*>& out_elements) {
  ui_element->GetElementsByClassName(out_elements, class_name);
}

UIElement* ui_element_query_selector(UIElement* ui_element, const String& selector_name) {
  return ui_element->QuerySelector(selector_name);
}

void ui_element_query_selector_all(UIElement* ui_element, const String& selector_name, DynamicArray<UIElement*>& out_elements) {
  ui_element->QuerySelectorAll(out_elements, selector_name);
}

UIContext* ui_element_get_context(UIElement* ui_element) {
  return ui_element->GetContext();
}

UIDocument* ui_element_get_document(UIElement* ui_element) {
  return ui_element->GetOwnerDocument();
}

UIElement* ui_element_get_parent(UIElement* ui_element) {
  return ui_element->GetParentNode();
}

UIElement* ui_element_get_next_sibling(UIElement* ui_element) {
  return ui_element->GetNextSibling();
}

UIElement* ui_element_get_previous_sibling(UIElement* ui_element) {
  return ui_element->GetPreviousSibling();
}

UIElement* ui_element_get_first_child(UIElement* ui_element) {
  return ui_element->GetFirstChild();
}

UIElement* ui_element_get_last_child(UIElement* ui_element) {
  return ui_element->GetLastChild();
}

UIElement* ui_element_get_child(UIElement* ui_element, const sizei index) {
  return ui_element->GetChild((i32)index);
}

const sizei ui_element_get_children_count(UIElement* ui_element) {
  return (sizei)ui_element->GetNumChildren();
}

const String& ui_element_get_tag(UIElement* ui_element) {
  return ui_element->GetTagName();
}

const String& ui_element_get_id(UIElement* ui_element) {
  return ui_element->GetId();
}

String ui_element_get_inner_html(UIElement* ui_element) {
  return ui_element->GetInnerRML();
}

bool ui_element_is_visible(UIElement* ui_element) {
  return ui_element->IsVisible();
}

bool ui_element_has_children(UIElement* ui_element) {
  return ui_element->HasChildNodes();
}

bool ui_element_matches(UIElement* ui_element, const String& selector) {
  return ui_element->Matches(selector);
}

/// UIElement functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIText functions

UIText* ui_text_create(UIDocument* ui_doc, const String& text) {
  NIKOLA_ASSERT(ui_doc, "An invalid UIDocument given to ui_text_create");
  return (UIText*)ui_doc->CreateTextNode(text).get();
}

void ui_text_add_line(UIText* ui_text, const Vec2& line_offset, const String& line_text) {
  ui_text->AddLine(Rml::Vector2f(line_offset.x, line_offset.y), line_text);
}

void ui_text_clear_lines(UIText* ui_text) {
  ui_text->ClearLines();
}

void ui_text_set_string(UIText* ui_text, const String& str) {
  ui_text->SetText(str);
}

const String& ui_text_get_string(UIText* ui_text) {
  return ui_text->GetText();
}

const sizei ui_text_get_width(UIText* ui_text) {
  return (sizei)Rml::ElementUtilities::GetStringWidth(ui_text, ui_text_get_string(ui_text));
}

/// UIText functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIProgress functions

void ui_progress_set_value(UIProgress* ui_progress, const f32 value) {
  return ui_progress->SetValue(value);
}

void ui_progress_set_max(UIProgress* ui_progress, const f32 max) {
  return ui_progress->SetMax(max);
}

f32 ui_progress_get_value(UIProgress* ui_progress) {
  return ui_progress->GetValue();
}

f32 ui_progress_get_max(UIProgress* ui_progress) {
  return ui_progress->GetMax();
}

/// UIProgress functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIControlInput functions

void ui_control_input_set_value(UIControlInput* ui_input, const String& value) {
  ui_input->SetValue(value);
}

void ui_control_input_set_selection_range(UIControlInput* ui_input, i32 selection_start, i32 selection_end) {
  ui_input->SetSelectionRange(selection_start, selection_end);
}

String ui_control_input_get_value(UIControlInput* ui_input) {
  return ui_input->GetValue();
}

void ui_control_input_get_selection_range(UIControlInput* ui_input, i32* selection_start, i32* selection_end, String* selected_text) {
  ui_input->GetSelection(selection_start, selection_end, selected_text);
}

bool ui_control_input_is_submitted(UIControlInput* ui_input) {
  return ui_input->IsSubmitted();
}

/// UIControlInput functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UIControlSelect functions

void ui_control_select_add(UIControlSelect* ui_select, const String& html, const String& value, i32 before, bool selectable) {
  ui_select->Add(html, value, before, selectable);
}

void ui_control_select_remove(UIControlSelect* ui_select, const i32 index) {
  ui_select->Remove(index);
}

void ui_control_select_remove_all(UIControlSelect* ui_select) {
  ui_select->RemoveAll();
}

void ui_control_select_show_box(UIControlSelect* ui_select) {
  ui_select->ShowSelectBox();
}

void ui_control_select_hide_box(UIControlSelect* ui_select) {
  ui_select->HideSelectBox();
}

void ui_control_select_set_value(UIControlSelect* ui_select, const String& value) {
  ui_select->SetValue(value);
}

void ui_control_select_set_selection(UIControlSelect* ui_select, const i32 selection) {
  ui_select->SetSelection(selection);
}

bool ui_control_select_is_box_shown(UIControlSelect* ui_select) {
  return ui_select->IsSelectBoxVisible();
}

UIElement* ui_control_select_get_option(UIControlSelect* ui_select, const i32 index) {
  return ui_select->GetOption(index);
}

String ui_control_select_get_value(UIControlSelect* ui_select) {
  return ui_select->GetValue();
}

i32 ui_control_select_get_selection(UIControlSelect* ui_select) {
  return ui_select->GetSelection();
}

sizei ui_control_select_get_options_count(UIControlSelect* ui_select) {
  return (sizei)ui_select->GetNumOptions();
}

/// UIControlSelect functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// UITabMenu functions

void ui_tab_menu_set_tab(UITabMenu* ui_tab, const i32 index, const String& html) {
  ui_tab->SetTab(index, html);
}

void ui_tab_menu_set_panel(UITabMenu* ui_tab, const i32 index, const String& html) {
  ui_tab->SetPanel(index, html);
}

void ui_tab_menu_set_tab(UITabMenu* ui_tab, const i32 index, UIElementPtr element) {
  ui_tab->SetTab(index, std::move(element));
}

void ui_tab_menu_set_panel(UITabMenu* ui_tab, const i32 index, UIElementPtr element) {
  ui_tab->SetPanel(index, std::move(element));
}

void ui_tab_menu_set_active_tab(UITabMenu* ui_tab, const i32 index) {
  ui_tab->SetActiveTab(index);
}

void ui_tab_menu_remove_tab(UITabMenu* ui_tab, const i32 index) {
  ui_tab->RemoveTab(index);
}

i32 ui_tab_menu_get_active_tab(UITabMenu* ui_tab) {
  return ui_tab->GetActiveTab();
}

sizei ui_tab_menu_get_tabs_count(UITabMenu* ui_tab) {
  return ui_tab->GetNumTabs();
}

/// UITabMenu functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
