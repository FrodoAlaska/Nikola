# Update 0.2

- [x] UI
    - [x] Text UI elements 
    - [x] Text animation system
    - [x] Button UI elements
    - [x] A layout system for 2D game UI.
    - [x] Checkbox UI elements 
    - [x] Slider UI elements
    - [x] Documentation
- [x] GFX
    - [x] Add instancing
    - [x] Test instancing
    - [x] Improve shader workflow (using `glGetProgramiv` to get various information about the shader)
    - [x] Improve framebuffers
    - [x] State-specific configuration
    - [x] Compute shader support, with dispatching.
    - [x] Memory barrier support. 
    - [x] Images, not textures. 
    - [x] Shader storage buffers.
    - [x] Have a specific type called `GfxBindingDesc` to use per draw call. This can be associated with the function `gfx_context_use_bindings`.
    - [x] Instead of having the `gfx_pipeline_use` function, we can move that to a context-specific function called `gfx_context_use_pipeline`.
    - [x] Documentation
- [x] Resource Manager
    - [x] Instead of using an abstracted `NBR` layer over everything, write extra `file_write_bytes` functions for the various resources, which will write a _compressed_ version of the resources, and load them as well.
    - [x] Let the material be created using a `MaterialDesc` structure for more effecient material usage. 
    - [x] Fix 3D models loading. 
        - [x] I think the renderer assumes the vertex data of the mesh so if a model has even an extra vertex data like color, the renderer messes everything up. Or limit all 3D models to a certain vertx data set. 
        - [x] Make sure that materials are loaded correctly using Assimp (colors and other uniforms).
    - [x] Documentation
- [x] Particles
    - [x] CPU-based particle system, giving each particle a position, velocity, and color. Use instancing to render a particle batch.
    - [x] Fix that lifetime bug.
    - [x] Add it to the GUI
    - [x] Better distribution of particles with more user control over the "shape" of the distribution.
    - [x] Documentation
- [x] 3D Animations
    - [x] Load animations from Assimp into the equivalent `NBR` resource.
    - [x] Load the `NBR` animation resource into the engine, creating an `Animator` component in the process to control the animation 
    - [x] Use the new animation resource and find out a way to render it and shade it properly.
    - [x] Try to have a good system for switching between animations
    - [x] Documentation
- [x] Renderer
    - [x] Improve draw calls and render passes
        - [x] Fix the render commands
        - [x] Improve the render pass system to be more versitile. It's very "stiff" right now.
        - [x] Improve the current render passes and test with client-side render passes.
    - [x] Add new rendering features/effects 
        - [x] Improve the Blinn-Phong lighting model by actually using specular maps and maybe fixing the material system a bit. 
        - [x] Normal/roughness mapping.
    - [x] Increase performance/improve workflow
        - [x] Change the instance data to become an instance uniform buffer that gets updated every frame. Each instance can aquire its data using `gl_InstanceID` instead of per-vertex instance data.
        - [x] Look into making the renderer "instanced" rather than rendering everything as is.
        - [x] If performance is still an issue, make all of the scene's lights into a buffer and send it to the shader as a buffer update instead of uniforms.
    - [x] Documentation
- [x] Performance Craze
    - [x] Implement both performance timers and normal timers.
    - [x] Create a simple job system 
    - [x] Test the new job system with the NBR conversion tool. 
    - [x] Documentation
- [x] GFX
    - [x] Separate the `gfx_*_create` functions for resources into `gfx_*_create` and `gfx_*_load`.  
    - [x] Documentation
- [x] Resource Manager
    - [x] Separate the `resources_push_*` functions into `resources_create_*` and `resources_load_*`. One would allocate and generate the ID and the other would actually load the resource.
- [x] GUI
    - [x] Replace the current ImGui version with the docking branch for a _way_ better GUI 
    - [x] Add a debug panel specifically for the window 
    - [x] Disable the mouse when we're editing sliders
    - [x] Documentation

# Update 0.3 (Animation and QOL improvements)

- [x] UI 
    - [x] Have a `UIImage` that will just be a regular texture, but it will conform to a UI layout. 
    - [x] There is a problem that arises when there is more than one slider in a `UILayout`
    - [x] Documentation
- [x] GUI 
    - [x] Add the `FrameDesc` to the GUI
    - [x] Make the GUI have an incremental optional mode (when pressing `SHIFT`, for example)
- [x] Animations
    - [x] Currently, not all animations that are imported work 
    - [x] I believe that the animation loader works with just GLTF for now, so try to make it work with Collada as well
    - [x] Add the animator to the GUI

## Update 0.4 (Renderer, performance, and particles)

- [] Renderer
    - [] PBR? Now? Please? Shit looks bad, man...
    - [] Look into GPU rendering with indirect drawing.
    - [] Gaussian blur
    - [] Bloom integration 
    - [] Decal rendering 
    - [] Terrain rendering and procedural generation (terrain generation using `stb_perlin` and loading from heightmap)
    - [] Improve lighting performance using clustered rendering (if needed).
    - [] Documentation
- [] Particles
    - [] GPU-based particle system, using compute shaders.
    - [] Documentation
- [] Animations 
    - [] Fix animations that have child nodes
    - [] Test animations with multiple instances 
    - [] Jobify with the animator or just make it more performant
    - [] Documentation

## Update 0.4.5 (Tiny performance improvements and data structures)

- [] Data Structures
    - [] Create your own string library 
    - [] Dynamic array 
    - [] Custom memory allocaters
    - [] Singly-linked and doubly-linked lists
    - [] Stacks and queues
    - [] Ring buffers
    - [] Hash map
- [] Threading
    - [] A better job system using fibers

## Update 0.5 (Physics stuff)

- [] Physics
    - [] A debug pass? After the HDR pass?
    - [] Implement mouse to screen and mouse to world for editing levels easier
    - [] Deleting bodies has some problems since the world delets bodies by keeping a "world index" inside the internal body data
    - [] Listen, just remove the physics library. It's really not all that useful. It brings more pain than joy

## BUGS:
- (Window & Renderer): When resizing the window or changing the fullscreen state, the renderer really does not hold up. I'm guessing it's because of the render passes? They need to update their own frame sizes when the window resizes? 
Maybe find a better way to do this.

- (Renderer): Shadows. They're turned off right now. Fix everything about em. Fuck em. Fix em. Marry em?

- Check all of the `TODO`, `FIX`, and `TEMP` in the codebase.
