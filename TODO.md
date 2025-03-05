# Plan 
[X] 1. Fix the filsystem API and have a wrapper around `std::filesystem::path` and replace any code that uses the old API. 
[x] 2. Create an easy-to-use and completely customizable editor.
[] 3. Work to improve the renderer and make it more streamlined. 
    - [x] Create a 2D batch renderer that can handle basic shapes and textures.
    - [x] A more pipelined post-processing system.
    - [] Reload shaders on the fly
    - [] Apply the blin-phong lighting model. 
    - [] Refraction and global illumination. 
[] 4. Make any viable demos in the `Demos` section with the new tools that you have now.
[] 6. Start working on the audio backend.

## (Engine) TODO: 
* General 
    - Switch from `git submodule` to `FetchContent` for fetching dependencies and perhaps rework the whole CMake section a bit.
    - Probably create my own `DynamicArray` instead of the STL one
* All about the UI 
    - Make a UICanvas/game canvas system and test it throughly.
    - Write some documentation for the whole editor section.
    - Keep adding types as they arise.
* Renderer 
    - Perhaps, in the `pre_pass` function of the renderer, we can take a `RenderData` struct that can provide the renderer with all the required data for rendering a scene. For example, a camera, light positions, and so on.
    - Have a basic default teture to be used for various reasons?
    - Create a `RendererDefault` struct to hold all of the initial default values.
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
