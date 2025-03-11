# Road To A Game 
[X] 1. Filesystem
[x] 2. GUI
[x] 3. Renderer v0.1
    - [x] Create a 2D batch renderer that can handle basic shapes and textures.
    - [x] A way to have default renderer resources (like the matrices buffer and a white texture).
    - [x] Support for diffuse and specular maps.
    - [x] Some of the textures on models get loaded weirdly
    - [x] Need to find a better way to send default uniforms with materials.
[] 4. Resource Manager v0.3
    - [x] A better way to refer to the resources
    - [x] Find a way to list all of the required resources for an application
    - [] Resource groups
    - [] File watcher system using C++'s std::filesystem
    - [] Create some sort of system to load resources on the fly 
    - [] Create a lexer and a parser for the `.nbrlist` file format in order to easily convert a number of resources into the `.nbr` format.
[] 6. Renderer v0.2. 
    - [] Multiple render passes
    - [] Better uniform and material system
    - [] A more pipelined post-processing system.
    - [] Apply the Blin-Phong shading model.
    - [] Fonts
    - [] Render 2D and 3D fonts
    - [] ?
[] 5. Audio System v0.1 
    - [] Lay out what you actually want the audio system to be and what it does. 
    - [] Decide on an audio backend. OpenAL-soft, SoLoud, and Miniaudio are the options.
    - [] Create an audio backend with a context 
    - [] A way to create an audio buffer that could be played with the context 
    - [] 2D and 3D sound and music. 
    - [] A fully-fledged audio pipeline that can play, position, pitch, control the volume, and apply effects to sound and music
[] 7. TBD

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
- NUSL: Nikola Uniform Shading Language. 
- GPU particles
- Multi-threading
