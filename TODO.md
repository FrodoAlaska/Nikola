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
- [] The Physics Question
    - [x] Integrate Jolt types
        - [x] Initialization, shutdown, and update of the physics world
        - [x] Fully integrate physics bodies 
        - [x] Integrate box, sphere, and capsule colliders for now 
        - [x] Add the various physics-related events to our event system
        - [x] Raycasts
        - [x] Implement Jolt's character controller 
    - [] Add physics components to the GUI 
        - [x] Physics body 
        - [x] Character
        - [] Colliders 
    - [] Save the physics components 
        - [x] Physics body 
        - [] Colliders 
    - [] Debug rendering 
        - [x] Create functions for generating debug shapes 
        - [x] Create and implement functions for queuing debug rendering commands 
        - [] Create a debug render pass that consumes the debug rendering commands 
        - [] Test with a few debug cubes 
        - [] Have the phyiscs world go through all of its colliders and render them through the new API 
        - [] Implement other debug shapes like spheres and capsules
    - [] Implement mouse to screen and mouse to world for editing levels easier
    - [] Documentation
- [] Resources 
    - [] Have a few functions specifically for the `NBR*` types to save, load, and unload them.
    - [] Improve the load times of the resource manager
    - [] Finally implement the parent-child relationship between nodes in 3D models?
- [] Renderer: Performance enhancement
    - [] Improve instancing completely. It's currently awfully architected. Take into account the animations and so on.
    - [] Collapse the `ShaderContext` and the `Material` into one?
    - [] Improve the performance of the renderer by using the GPU to dispatch draw calls, using indirect buffers.
    - [] Improve lighting performance using clustered rendering (This is optional if the performance is fine up until this point).
    - [] Documentation
- [] Animations 
    - [] Test animations with multiple instances.
    - [] Jobify with the animator or just make it more performant
    - [] Documentation
- [] GPU Particles
    - [] GPU-based particle system, using compute shaders.
    - [] Have a lot of options to edit the shape, distribution, and gravity. Perhaps save it in a file?
    - [] Documentation
- [] Renderer: Beautify it
    - [] PBR? Now? Please? Shit looks bad, man...
    - [] Gaussian blur
    - [] Bloom integration 
    - [] Documentation
- [] Threading
    - [] A better job system using fibers
    - [] Thread pools
- [] Renderer: Extra primitives 
    - [] More shapes like planes, spheres, and so on.
    - [] Add a debug pass that just copies its framebuffer like the billboard pass. It's fine because it's just for debug purposes
    - [] Decal rendering 
    - [] Terrain rendering and procedural generation (terrain generation using `stb_perlin` and loading from heightmap)
- [] Data Structures
    - [] Create your own string library 
    - [] Dynamic array 
    - [] Custom memory allocaters
    - [] Singly-linked and doubly-linked lists
    - [] Stacks and queues
    - [] Ring buffers
    - [] Hash map

```c++
renderer_queue_debug_cube_instanced(position, scale, color, inst_count);
renderer_queue_debug_sphere_instanced(position, radius, color, inst_count);
renderer_queue_debug_capsule_instanced(position, radius, height, color, inst_count);

renderer_queue_debug_cube(position, scale, color);
renderer_queue_debug_sphere(position, radius, color);
renderer_queue_debug_capsule(position, radius, height, color);
```

## BUGS:
- (Window & Renderer): When resizing the window or changing the fullscreen state, the renderer really does not hold up. I'm guessing it's because of the render passes? They need to update their own frame sizes when the window resizes? 
Maybe find a better way to do this.

- (Renderer): Shadows. They're turned off right now. Fix everything about em. Fuck em. Fix em. Marry em?

- Check all of the `TODO`, `FIX`, and `TEMP` in the codebase.
