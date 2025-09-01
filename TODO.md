# TODO

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
- [] Particles
    - [] GPU-based particle system, using compute shaders.
    - [] Have a lot of options to edit the shape, distribution, and gravity. Perhaps save it in a file?
    - [] Documentation
- [] Resources 
    - [] Have a few functions specifically for the `NBR*` types to save, load, and unload them.
    - [] Finally have implement the parent-child relationship between nodes in 3D models?
    - [] Improve the load times of the resource manager
- [] Renderer
    - [] PBR? Now? Please? Shit looks bad, man...
    - [] Look into GPU rendering with indirect drawing.
    - [] Gaussian blur
    - [] Bloom integration 
    - [] Decal rendering 
    - [] Terrain rendering and procedural generation (terrain generation using `stb_perlin` and loading from heightmap)
    - [] Improve lighting performance using clustered rendering (if needed).
    - [] Documentation
- [] Animations 
    - [] Fix animations that have child nodes
    - [] Test animations with multiple instances 
    - [] Jobify with the animator or just make it more performant
    - [] Documentation
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
