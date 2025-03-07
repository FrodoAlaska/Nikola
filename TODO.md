# Road To A Game 
[X] 1. Filesystem
[x] 2. GUI
[] 3. Renderer v0.1
    - [x] Create a 2D batch renderer that can handle basic shapes and textures.
    - [x] A way to have default renderer resources (like the matrices buffer and a white texture).
    - [x] Support for diffuse and specular maps.
    - [x] Some of the textures on models get loaded weirdly
    - [x] Need to find a better way to send default uniforms with materials.
[] 4. Audio Backend v0.1 
    - [] Create an audio backend with a context 
    - [] A way to create an audio buffer that could be played with the context 
[] 5. Audio System v0.1 
    - [] 2D sound and music. 
    - [] Control the pitch and volume of a sound.
[] 6. Add more resources and improve it
    - [] Fonts
    - [] Add audio resources
    - [] Reload shaders on the fly?
[] 7. Renderer v0.2. 
    - [] Multiple render passes
    - [] Better uniform and material system
    - [] A more pipelined post-processing system.
    - [] Apply the Blin-Phong shading model.
    - [] Render 2D and 3D fonts
    - [] ?
[] 8. TBD

## (Engine) TODO: 
* General 
    - Switch from `git submodule` to `FetchContent` for fetching dependencies and perhaps rework the whole CMake section a bit.
    - Probably create my own `DynamicArray` instead of the STL one
* All about the UI 
    - Make a UICanvas/game canvas system and test it throughly.
    - Write some documentation for the whole editor section.
    - Keep adding types as they arise.
* Renderer 
    - Render 2D and 3D fonts
* Demos
    - Just a bunch of people spining while the Nutcracker by Tchaikovsy plays in the background. First there's only one model that spins, then a few, then a whole _building_, and so on.
    - An ominious spinning behelit under some nice lighting effects with the Guts theme playing in the background.
* NBR 
    - Be able to rename an output file 
* Resources 
    - Font loading 
    - A memory pool/arena for resources?
    - Perhaps a file that includes all of the paths for the resources to be loaded? 

## (Core) TODO: 
* General
    - Potentially fix the memory allocater functions. So instead of normal wrappers around C allocation functions, perhaps have something more sophisticated
    - Get rid of GLFW and do it the old-fashioned way? Misery.
* Logger 
    - Batch logging? 
    - Log into a file
* GFX 
    - Since DirectX is out of the picture, remove the little bits and pieces of its effect.
    - Seperate the `gl_backend.cpp` file into several files for better visualization
    - A function to sub image or slice a texture 
    - Put some thought into instancing
* Audio 
    - Audio context and audio buffers

## (Engine) BUGS: 
- EMPTY FOR NOW...

## (Core) BUGS: 
- EMPTY FOR NOW...

## TEST: 
- (GL) Shader attribute semantics (e.g, does Mat4 work there)

## FUTURE PLANS: 
- Some kind of level serlializer?
- Phyiscs. Yes. Just physics.
- Hot reloading
- NUSL: Nikola Uniform Shading Language. 
- GPU particles
- Multi-threading
