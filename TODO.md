# Road To A Game 
[X] 1. Filesystem
[x] 2. GUI v0.1
[x] 3. Renderer v0.1
    - [x] Create a 2D batch renderer that can handle basic shapes and textures.
    - [x] A way to have default renderer resources (like the matrices buffer and a white texture).
    - [x] Support for diffuse and specular maps.
    - [x] Some of the textures on models get loaded weirdly
    - [x] Need to find a better way to send default uniforms with materials.
[x] 4. Resource Manager v0.3
    - [x] A better way to refer to the resources
    - [x] Find a way to list all of the required resources for an application
    - [x] Resource groups
    - [x] File watcher system using C++'s std::filesystem
    - [x] Completely remove any pointer to resources. Just refer to resoruces with their IDs
    - [x] Implement `nbr_import` functions that will convert NBR resource formats into engine resource formats.
    - [x] Reload resources on the fly
    - [x] Documentation
[x] 5. NBR v0.2 
    - [x] Have the `nbrlist` file be a list of all the resoruces required by a game. The tool will read said NBRList file and convert all of them to the `.nbr*` format.
    - [x] Be able to rename an output file 
    - [x] Have the ability to go through a directory and convert cubemaps 
    - [x] If the `--resource-type` flag is omitted, let the tool "guess" which resource is being converted. Perhaps get rid of the glag all together? 
    - [x] Implement a job system or, rather, multi-threading for the tool to make conversions faster, especially for resources like 3D models.
    - [x] Improve the README
[x] 6. Build System Improvement v0.2 
    - [x] Improve the CMake situation
    - [x] Better header names
    - [x] Test and improve the build scripts for Linux
[x] 7. Render Passes v0.2. 
    - [x] Multiple render passes
    - [x] Some meshes draw the specular texture even though they don't need it? Perhaps find a better way to use textures?
    - [x] A more pipelined post-processing system.
[x] 8 Resource Rendering v0.1
    - [x] Create a `ShaderContext` structure to encapsulate shader functionality.
    - [x] Seperate the shader and the materials from each other. The materials should only be a description of the physical properties of any renderable type. 
    - [x] Add the `ShaderContext` as a new resource type.
    - [x] Incorporate `ShaderContext` into rendering
    - [x] Improve the rendering of 3D models (perhaps each mesh is a specific render command?) 
    - [x] Make render passes be associated with a specific `ShaderContext` instead of a `Material`.
    - [x] The model loader cannot load models that posses no textures
[] 9. Scenes v0.1
    - [x] Scene system ground work
    - [x] Entity system ground work
    - [] Test 
    - [] Documentation
[] 10. GUI v0.2 
    - [] A canvas system for 2D game UI.
    - [] A better system to handle debug UI.
    - [] Documentation.
[] 11. Serlializer v0.1
[] 12. GFX v1.0? 
    - [x] Let every reasource have a `_use` function
    - [x] Refactor the `GfxPipeline` 
    - [x] Improve framebuffers (`gfx_framebuffer_copy(src, dest, pos, size)`, incorporate `glDrawBuffers`)
    - [x] Test
    - [] Instancing 
    - [] Documentation
[] 13. Renderer v0.3 
    - [] Improve render pass system
    - [] Better material system 
    - [] More intuitive and faster method to render primitives using materials
[] 14. Camera v0.2
[] 15. Fonts v0.1 
    - [] Choose a font-loading library 
    - [] Make and implement the `NBRFont` specification 
    - [] Add support for fonts in the NBR tool 
    - [] Add font loading support in the resource manager 
    - [] Have a minimal version of font rendering (both 2D and 3D)
[] 16. Multi-threading v0.2 
    - [] Implement both performance timers and normal timers
    - [] Which systems can benefit most from multi-threading? 
    - [] Should we implement a job system? If yes, how can this be done within the confines of the engine? 
    - [] Try to improve the load times for the resource manager as a test of the new multi-threading system.
[] 17. Audio System v0.1 
    - [] Lay out what you actually want the audio system to be and what it does. 
    - [] Decide on an audio backend. OpenAL-soft, SoLoud, and Miniaudio are the options.
    - [] Create an audio backend with a context 
    - [] A way to create an audio buffer that could be played with the context 
    - [] 2D and 3D sound and music. 
    - [] A fully-fledged audio pipeline that can play, position, pitch, control the volume, and apply effects to sound and music
[] 18. Physics v0.1
[] 19. 3D Animations v0.1

## (Engine) TODO: 
* General 
    - Probably create my own `DynamicArray` instead of the STL one
* Resources 
    - Try to replace the `_loader` functions. For many of the resources, they are useless.
    - A memory pool/arena for resources?
* Renderer 
    - The goal of this renderer is to be like a lego set. It will provide the user with as much building blocks as possible while not commiting to any one technique. It's up to the user to pick and choose the renderer.
    - Improve the material and shader uniform system 
    - Have the `ShaderContext` read and retrieve all uniforms and their locations instead of caching them from first use
    - A better technique of rendering resources 
    - Improve the render pass system

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
- Levels, entities, and scenes.
- Phyiscs. Yes. Just physics.
- GPU particles
- NUSL: Nikola Uniform Shading Language. 
