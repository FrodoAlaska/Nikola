# Plan 
[X] 1. Work on the `general` side of the engine TODOs. Fix the filsystem API and have a wrapper around `std::filesystem::path` and replace any code that uses the old API. 
[] 2. Create an easy-to-use and completely customizable editor as well as everything in the `UI` section of the engine TODOs. 
[] 3. Work to improve the renderer. 
    - We are probably going to impelement a deffered renderer in order to improve perforamance and to easily get post-processing effects applied. 
[] 4. Make any viable demos in the `Demos` section with the new tools that you have now.

## (Engine) TODO: 
* General 
    - Switch from `git submodule` to `FetchContent` for fetching dependencies
    - Probably create my own `DynamicArray` instead of the STL one
* All about the UI 
    - Make a panel for each editable type in the engine.
    - Make a batch renderer that renders basic shapes, 2D textures, and texts. 
    - Make a UICanvas/game canvas system and test it throughly.
    - Write some documentation for the whole editor section.
* Renderer 
    - Basic lighting
    - Configure post-processing step and some dope deffered rendering 
    - Render 2D and 3D fonts
    - Have a basic default teture to be used for various reasons?
    - Create a `RendererDefault` struct to hold all of the initial default values.
    - Batch rendering 
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
- NUSL: Nikola Uniform Shading Language
- Phyiscs. Yes. Just physics.
- GPU particles
- Hot reloading
- Some kind of level serlializer?
- Multi-threading
