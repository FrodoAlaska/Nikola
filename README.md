# *Nikola*
A cross-platform framework for window creation, input handling, audio playback, and rendering using OpenGL 4.5+ or Direct3D11.
 
# Features 
- A fully documented single header file for every functionality in the library.
- Cross-platform window creation. 
- Gamepad, keyboard, and mouse input support.
- Fully-configurable cross-platform rendering API using OpenGL (Linux) and DirectX11 (Windows).
- Robust resource manager 
- Support for loading multiple image formats such as jpeg, png, bmp, tga, and way more. 

# Dependencies
- GLFW3 
- GLAD
- D3D11
- STB 
- GLM

# Build Instructions
Run the following commands to build *Nikola* using _CMake_

```bash
mkdir build 
cd build 
cmake .. 

# For Linux
make 

# For Windows 
cmake --build .
```

# Hello, *Nikola*
Here's a simple example of the _core_ library working in action. The example below will open a basic window and initialze a graphics context.

```c++
#include <nikola/nikola_core.hpp>

int main() {
  // Initialze the library
  if(!nikola::init()) {
    return -1;
  }

  // Openinig the window
  nikola::i32 win_flags = nikola::WINDOW_FLAGS_FOCUS_ON_CREATE | nikola::WINDOW_FLAGS_GFX_HARDWARE;
  nikola::Window* window = nikola::window_open("Hello, Nikola", 1366, 768, win_flags);
  if(!window) {
    return -1;
  }

  // Creating a graphics context
  nikola::GfxContextDesc gfx_desc = {
    .window = window,
    .states = nikola::GFX_STATE_DEPTH | nikola::GFX_STATE_STENCIL,
  };
  nikola::GfxContext* gfx = nikola::gfx_context_init(gfx_desc);
  if(!gfx) {
    return -1;
  }

  // Main loop
  while(nikola::window_is_open(window)) {
    // Will stop the application when F1 is pressed
    if(nikola::input_key_pressed(nikola::KEY_F1)) {
      break;
    }
    
    // Clear the screen to black
    nikola::gfx_context_clear(gfx, 0.0f, 0.0f, 0.0f, 1.0f, nikola::GFX_CONTEXT_FLAGS_ENABLE_VSYNC);
    
    // Swap the internal window buffer
    nikola::gfx_context_present(gfx);
    
    // Poll the window events
    nikola::window_poll_events(window);
  }

  // De-initialze
  nikola::gfx_context_shutdown(gfx);
  nikola::window_close(window);
  nikola::shutdown();
}

```
