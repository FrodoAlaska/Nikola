# Road To A Game 
[X] Filesystem
[x] GUI v0.1
[x] Renderer v0.1
    - [x] Create a 2D batch renderer that can handle basic shapes and textures.
    - [x] A way to have default renderer resources (like the matrices buffer and a white texture).
    - [x] Support for diffuse and specular maps.
    - [x] Some of the textures on models get loaded weirdly
    - [x] Need to find a better way to send default uniforms with materials.
[x] Resource Manager v0.3
    - [x] A better way to refer to the resources
    - [x] Find a way to list all of the required resources for an application
    - [x] Resource groups
    - [x] File watcher system using C++'s std::filesystem
    - [x] Completely remove any pointer to resources. Just refer to resoruces with their IDs
    - [x] Implement `nbr_import` functions that will convert NBR resource formats into engine resource formats.
    - [x] Reload resources on the fly
    - [x] Documentation
[x] NBR v0.2 
    - [x] Have the `nbrlist` file be a list of all the resoruces required by a game. The tool will read said NBRList file and convert all of them to the `.nbr*` format.
    - [x] Be able to rename an output file 
    - [x] Have the ability to go through a directory and convert cubemaps 
    - [x] If the `--resource-type` flag is omitted, let the tool "guess" which resource is being converted. Perhaps get rid of the glag all together? 
    - [x] Implement a job system or, rather, multi-threading for the tool to make conversions faster, especially for resources like 3D models.
    - [x] Improve the README
[x] Build System Improvement v0.2 
    - [x] Improve the CMake situation
    - [x] Better header names
    - [x] Test and improve the build scripts for Linux
[x] Render Passes v0.2. 
    - [x] Multiple render passes
    - [x] Some meshes draw the specular texture even though they don't need it? Perhaps find a better way to use textures?
    - [x] A more pipelined post-processing system.
[x] Resource Rendering v0.1
    - [x] Create a `ShaderContext` structure to encapsulate shader functionality.
    - [x] Seperate the shader and the materials from each other. The materials should only be a description of the physical properties of any renderable type. 
    - [x] Add the `ShaderContext` as a new resource type.
    - [x] Incorporate `ShaderContext` into rendering
    - [x] Improve the rendering of 3D models (perhaps each mesh is a specific render command?) 
    - [x] Make render passes be associated with a specific `ShaderContext` instead of a `Material`.
    - [x] The model loader cannot load models that posses no textures
[x] Renderer v0.3 
    - [x] Refine the render command system
    - [x] Improve render pass system
    - [x] Documentation
[x] Lighting v0.1 
    - [x] Implement lighting data structures that can be represented in a scene 
    - [x] Decide on whether the lighting calculation/pipeline should be part of the engine or the application 
    - [x] Perhaps add a `FrameData` structure that will have all the available lights as well as the scene camera per frame (obviously).
[x] GFX v1.0 
    - [x] Let every reasource have a `_use` function
    - [x] Refactor the `GfxPipeline` 
    - [x] Improve framebuffers (`gfx_framebuffer_copy(src, dest, pos, size)`, incorporate `glDrawBuffers`)
    - [x] Test
    - [x] A better way to set the framebuffer or the "render target".
    - [x] Improve resource updating functions
    - [x] Documentation
[x] Renderer v0.4 
    - [x] HDR pipeline 
    - [x] Improve lighting structures
[x] GUI/Editor v0.2
    - [x] Add more serializable engine-specific types
    - [x] Semi-scene serializer
    - [x] Include more engine-specific types that can be edited, including resources.
    - [x] Add a "render gui" callback to the applications.
    - [x] Documentation (for `nikola_ui.h` and `nikola_file.h`)
[x] 2D Renderer v0.2
    - [x] Decide whether to keep the 2D renderer seperate or part of the 3D renderer 
    - [x] Basic 2D shapes like, lines, triangles, quads, circles, and textured quads.
[x] Fonts v0.1 
    - [x] Convert font formats into the internal `nbrfont` format. 
    - [x] Add fonts to the resource manager 
    - [x] Don't forget to add `glPixelStori` somewhere 
    - [x] Font rendering
    - [x] Documentation (`batch_renderer`, `nbr_font`, `font resources`).
[x] Audio v0.1 
    - [x] Lay out what you actually want the audio system to be and what it does. 
    - [x] Decide on an audio backend. OpenAL-soft, SoLoud, and Miniaudio are the options.
    - [x] A way to create an audio buffer that could be played with the context 
    - [x] Create an audio backend with a context 
    - [x] Create the NBR version of the audio resource. 
    - [x] Add it to the resource manager
    - [x] Try to load, convert, and manage the new audio buffer with the NBR tool.
    - [x] Add one listener and manage it by the user
    - [x] Improved source and listener management. Perhaps have an audio system?
    - [x] Add the new types to the GUI
    - [x] Documentation
[x] Annoying Bugs To Fix 
    - [x] The wonderful and useful `resoruces_push_dir` function doesn't work and seg faults for some reason. Please fix.
    - [x] Actually, the whole filewatcher system might be _extremely_ slow. At least that's what I saw in the data. Remove it? Find an alternative? Make it faster? I don't know.
    - [x] The NBR tool needs to create a certain directory if it doesn't exist, so I don't have to do it on my own.
    - [x] Resources like `ShaderContext`, `Skybox`, and others that take in other resources as arguments, should be given a path as well.
    - [x] Make resources be less dependent on `ResourceID`. The ID should be given when the resources are pushed, but not to be used _in place_ of resources.
    - [x] Fix the way the `Transform` type is edited through the GUI
    - [x] Add OGG support in the NBR tool
[x] Collision Detection
    - [x] Add AABB VS. AABB collision detection 
    - [x] Add raycasting 
    - [x] Add AABB VS. Ray collision detection
    - [x] Add Spheres and their associated collision detection functions
[] Physics v0.1
    - [x] Create a physics world, physics body, and what have you
    - [] Add collision callbacks and debug render capabilities for the colliders
    - [] Test the physics logic by making some testbeds.
    - [] Integrate the physics layer into the engine proper.
    - [] Ray testing 
    - [] Add the newly-added physics types to be edited by the GUI 
    - [] Documentation
[] Performance Craze 0.1 
    - [x] Implement both performance timers and normal timers
    - [x] Run some tests through an instrumentation tool of some kind to know _truly_ what is slowing down the application.
    - [] Create an abstraction layer over threads, mutexes, aotmics, and all things multi-threading. 
    - [] Create a simple job system 
    - [] Improve resource loading time by adding asynchronous resource loading.
    - [] Documentation
[] UI v0.1
    - [] A canvas system for 2D game UI.
    - [] A menu system
    - [] Documentation
[] Renderer v0.6 
    - [] YOU MUST ADD INSTANCING NOOOOOOWWW!!!!
    - [] Allow toggleablity of render passes
    - [] Bloom integration 
    - [] Improve lighting using clustered rendering
    - [] Compute shaders 
    - [] Better bloom for testing the new compute shaders
    - [] Cascaded shadow maps
    - [] Documentation
[] Particles v0.1 
    - [] Add CPU-based particles that can render different shapes, sizes, and colors (maybe even certain textures?) 
    - [] Implement a GPU-based particle system, using something like instancing.
    - [] Documentation
[] 3D Animations v0.1

## (Engine) BUGS: 
- EMPTY FOR NOW...

## (Core) BUGS: 
- EMPTY FOR NOW...

## TEST: 
- (GL) Shader attribute semantics (e.g, does Mat4 work there)

## FUTURE PLANS: 
- Phyiscs. Yes. Just physics.
- GPU particles
- NUSL: Nikola Uniform Shading Language or shader-generating materials
