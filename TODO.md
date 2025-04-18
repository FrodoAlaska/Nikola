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
[x] 9. Renderer v0.3 
    - [x] Refine the render command system
    - [x] Improve render pass system
    - [x] Documentation
[x] 10. Lighting v0.1 
    - [x] Implement lighting data structures that can be represented in a scene 
    - [x] Decide on whether the lighting calculation/pipeline should be part of the engine or the application 
    - [x] Perhaps add a `FrameData` structure that will have all the available lights as well as the scene camera per frame (obviously).
[x] 11. GFX v1.0? 
    - [x] Let every reasource have a `_use` function
    - [x] Refactor the `GfxPipeline` 
    - [x] Improve framebuffers (`gfx_framebuffer_copy(src, dest, pos, size)`, incorporate `glDrawBuffers`)
    - [x] Test
    - [x] A better way to set the framebuffer or the "render target".
    - [x] Improve resource updating functions
    - [x] Documentation
[] 12. Renderer v0.4 
    - [] HDR pipeline 
    - [] Bloom integration 
    - [] Emissive materials
[] 13. GUI v0.2
    - [] Refine the GUI system.
    - [] Include more engine-specific types that can be edited, including resources.
[] 14. Fonts v0.1 
    - [] Convert font formats into the internal `nbrfont` format. 
    - [] Add fonts to the resource manager 
    - [] Font rendering
    - [] Documentation.
[] 15. UI v0.1
    - [] A canvas system for 2D game UI.
    - [] A menu system
    - [] A better system to handle debug UI.
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

## (Engine) BUGS: 
- (Filesystem): There is a bug with the filesystem where the string gets allocated and de-allocated wrong I think? It crashes the program when we pass a normal C-string (i.e "string"), but it runs okay 
if we give it a string variable instead (i.e `func(path_string)`). Perhaps there is something wrong with the lifetime of strings? I'm not sure.
- (Resources): It's probably better not to have an internal cache specified by the engine itself. It's probably better to let the user (me) create that cache if need be. That way, we don't have to have specific hard-coded path values and whatnot. 

## (Core) BUGS: 
- EMPTY FOR NOW...

## TEST: 
- (GL) Shader attribute semantics (e.g, does Mat4 work there)

## FUTURE PLANS: 
- Levels, entities, and scenes.
- Phyiscs. Yes. Just physics.
- GPU particles
- NUSL: Nikola Uniform Shading Language or shader-generating materials
