#pragma once

#include "nikola_base.h"
#include "nikola_physics.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ---------------------------------------------------------------------

// Some useful forward declarations to avoid circular dependencies...

struct UIButton;
struct UICheckbox;
struct UISlider;
struct UIImage;
struct UIMenu;
struct Entity;

/// ---------------------------------------------------------------------
 
/// ---------------------------------------------------------------------
/// *** Event ***

///---------------------------------------------------------------------------------------------------------------------
/// EventType
enum EventType {
  /// App events 
  
  EVENT_APP_QUIT = 0,

  /// Window events 
  
  EVENT_WINDOW_MOVED,
  EVENT_WINDOW_MINIMIZED, 
  EVENT_WINDOW_MAXIMIZED,
  EVENT_WINDOW_FOCUSED, 
  EVENT_WINDOW_RESIZED, 
  EVENT_WINDOW_FRAMEBUFFER_RESIZED, 
  EVENT_WINDOW_CLOSED, 
  EVENT_WINDOW_FULLSCREEN,

  /// Mouse events 
  
  EVENT_MOUSE_MOVED, 
  EVENT_MOUSE_BUTTON_PRESSED, 
  EVENT_MOUSE_BUTTON_RELEASED,
  EVENT_MOUSE_SCROLL_WHEEL,
  EVENT_MOUSE_CURSOR_SHOWN,
  EVENT_MOUSE_ENTER, 
  EVENT_MOUSE_LEAVE,

  /// Keyboard events 
  
  EVENT_KEY_PRESSED, 
  EVENT_KEY_RELEASED, 

  /// Joystick events
  
  EVENT_JOYSTICK_CONNECTED, 
  EVENT_JOYSTICK_DISCONNECTED, 

  /// UI events
  
  EVENT_UI_BUTTON_CLICKED, 
  EVENT_UI_BUTTON_ENTERED,
  EVENT_UI_BUTTON_EXITED,

  EVENT_UI_CHECKBOX_CLICKED,
  EVENT_UI_CHECKBOX_ENTERED,
  EVENT_UI_CHECKBOX_EXITED,
  
  EVENT_UI_SLIDER_CLICKED,
  EVENT_UI_SLIDER_CHANGED,
  
  EVENT_UI_IMAGE_CLICKED,
  EVENT_UI_IMAGE_ENTERED,
  EVENT_UI_IMAGE_EXITED,

  EVENT_UI_MENU_ITEM_CHANGED,
  EVENT_UI_MENU_ITEM_CLICKED,

  /// Physics events

  EVENT_PHYSICS_BODY_ACTIVATED,
  EVENT_PHYSICS_BODY_DEACTIVATED,
  EVENT_PHYSICS_CONTACT_ADDED,
  EVENT_PHYSICS_CONTACT_REMOVED,
  EVENT_PHYSICS_CONTACT_PERSISTED,
  EVENT_PHYSICS_RAYCAST_HIT,

  /// Entity events
  
  EVENT_ENTITY_ADDED, 
  EVENT_ENTITY_DESTROYED,

  EVENTS_MAX,
};
/// EventType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Event
struct Event {
  /// The event's type.
  EventType type;
  
  /// New poisition of the window.
  i32 window_new_pos_x, window_new_pos_y;

  /// The current focus state of the window.
  bool window_has_focus;       

  /// The window's new size.
  i32 window_new_width, window_new_height;        

  /// The window's new size of the framebuffer.
  i32 window_framebuffer_width, window_framebuffer_height; 
 
  /// The window's new fullscreen state.
  bool window_is_fullscreen;
  
  /// The key that was just pressed.
  i32 key_pressed; 

  /// The key that was just released.
  i32 key_released;

  /// A key modifier that is passed on either 
  /// `EVENT_KEY_PRESSED`, `EVENT_KEY_RELEASED`, 
  /// `EVENT_MOUSE_BUTTON_PRESSED`, or `EVENT_MOUSE_BUTTON_RELEASED`.
  i32 key_modifier;
  
  /// The current mouse position (relative to the screen). 
  f32 mouse_pos_x, mouse_pos_y;    
  
  /// By how much did the mouse move since the last frame? 
  f32 mouse_offset_x, mouse_offset_y; 
   
  /// The mouse button that was just pressed.
  i32 mouse_button_pressed; 
 
  /// The mouse button that was just released.
  i32 mouse_button_released;
 
  /// The value the scroll mouse's wheel moved by. 
  f32 mouse_scroll_value; 

  /// Is the mouse cursor currently visible?
  bool cursor_shown;

  /// The joystick ID given to this event.
  i32 joystick_id; 

  /// The button UI element given to this event. 
  UIButton* button      = nullptr;
  
  /// The checkox UI element given to this event. 
  UICheckbox* checkbox  = nullptr;
  
  /// The slider UI element given to this event. 
  UISlider* slider      = nullptr;
  
  /// The image UI element given to this event. 
  UIImage* image        = nullptr;
  
  /// The menu UI element given to this event. 
  UIMenu* menu          = nullptr;

  /// The physics body that was given to this event 
  /// by either `EVENT_PHYSICS_BODY_ACTIVATED` or 
  /// `EVENT_PHYSICS_BODY_DEACTIVATED`.
  PhysicsBody* body;

  /// The collision data given to this event 
  /// by either `EVENT_PHYSICS_CONTACT_ADDED`,
  /// `EVENT_PHYSICS_CONTACT_REMOVED`, or `EVENT_PHYSICS_CONTACT_PERSISTED`.
  CollisionData collision_data;

  /// The ray cast result given to this event 
  /// by `EVENT_PHYSICS_RAYCAST_HIT`.
  RayCastResult cast_result;

  /// The entity ID given to this event by 
  /// either `EVENT_ENTITY_ADDED` or `EVENT_ENTITY_DESTROYED`.
  u32 entt_id;
};
/// Event
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Event callback

/// The callback signature to use for any events, passing in the `event`, the `dispatcher`, and the `listener`.
using EventFireFn = bool(*)(const Event& event, const void* dispatcher, const void* listener);

/// Event callback
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Event functions

/// Initialze the event system 
NIKOLA_API void event_init();

/// Shutdown the event system and reclaim some memory 
NIKOLA_API void event_shutdown();

/// Attach the given `func` callback to an event of type `type`, passing in the `listener` as well.
NIKOLA_API void event_listen(const EventType type, const EventFireFn& func, const void* listener = nullptr);

/// Call all callbacks associated with `event.type` and pass in the given `event` and the `dispatcher`. 
/// Returns `true` on success.
NIKOLA_API const bool event_dispatch(const Event& event, const void* dispatcher = nullptr);

/// Event functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Event ***
/// ---------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
