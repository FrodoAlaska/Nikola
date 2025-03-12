# Road To A Game 
[X] 1. Filesystem
[x] 2. GUI v0.1
[x] 3. Renderer v0.1
    - [x] Create a 2D batch renderer that can handle basic shapes and textures.
    - [x] A way to have default renderer resources (like the matrices buffer and a white texture).
    - [x] Support for diffuse and specular maps.
    - [x] Some of the textures on models get loaded weirdly
    - [x] Need to find a better way to send default uniforms with materials.
[] 4. Resource Manager v0.3
    - [x] A better way to refer to the resources
    - [x] Find a way to list all of the required resources for an application
    - [x] Resource groups
    - [x] File watcher system using C++'s std::filesystem
    - [x] Completely remove any pointer to resources. Just refer to resoruces with their IDs
    - [x] Implement `nbr_import` functions that will convert NBR resource formats into engine resource formats.
    - [x] Reload resources on the fly
    - [] Improve the NBR converter tool
    - [] Documentation for everything new
[] 5. GUI v0.2 
[] 6. Renderer v0.2. 
    - [] `material_set_shader_context`?
    - [] Multiple render passes
    - [] Better uniform and material system
    - [] A more pipelined post-processing system.
    - [] Apply the Blin-Phong shading model.
    - [] Fonts
    - [] Render 2D and 3D fonts
    - [] ?
[] 7. Audio System v0.1 
    - [] Lay out what you actually want the audio system to be and what it does. 
    - [] Decide on an audio backend. OpenAL-soft, SoLoud, and Miniaudio are the options.
    - [] Create an audio backend with a context 
    - [] A way to create an audio buffer that could be played with the context 
    - [] 2D and 3D sound and music. 
    - [] A fully-fledged audio pipeline that can play, position, pitch, control the volume, and apply effects to sound and music

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
* NBR 
    - Have the TOML file be a list of all the resoruces required by a game. The tool will read said TOML file and convert all of them to the `.nbr` format.
    - Be able to rename an output file 
    - Have the ability to go through a directory an convert cubemaps 
    - If the `--resource-type` flag is omitted, let the tool "guess" which resource is being converted. Perhaps get rid of the glag all together? 
* Resources 
    - Font loading 
    - A memory pool/arena for resources?

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
