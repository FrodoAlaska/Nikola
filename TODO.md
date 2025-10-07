# Update 0.3: Being "Feature complete"

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
- [x] The Physics Question
    - [x] Integrate Jolt types
        - [x] Initialization, shutdown, and update of the physics world
        - [x] Fully integrate physics bodies 
        - [x] Integrate box, sphere, and capsule colliders for now 
        - [x] Add the various physics-related events to our event system
        - [x] Raycasts
        - [x] Implement Jolt's character controller 
    - [x] Add physics components to the GUI 
        - [x] Physics body 
        - [x] Character
    - [x] Save the physics components 
        - [x] Physics body 
    - [x] Debug rendering 
        - [x] Create functions for generating debug shapes 
        - [x] Create and implement functions for queuing debug rendering commands 
        - [x] Create a debug render pass that consumes the debug rendering commands 
        - [x] Test with a few debug cubes 
        - [x] Have the phyiscs world go through all of its colliders and render them through the new API 
        - [x] Implement other debug shapes like spheres and capsules
    - [x] Implement mouse to screen and mouse to world for editing levels easier
    - [x] Documentation 
- [x] Renderer: Beautify it v1.0
    - [x] Better attenuation
    - [x] PBR pipeline
    - [x] Emissive materials
    - [x] Fix shadows 
- [] Animations 
    - [] Test animations with multiple instances.
    - [] Documentation
- [] Resources 
    - [] Have a few functions specifically for the `NBR*` types to save, load, and unload them.
    - [] Maybe have a JSON file that associates the resources with each other? 
        - Perhaps we can use this as a way to represent our materials and then save them a `.nbr` files.
        - For example, a texture can be referred in a material and a material can be referred to in a mesh. 
        - That way, we can have the artists (and even myself) use different combinations of resources as we liked.
        - It's almost like a description of how the resources will be used. 
        - We can also take it a step further and make it edit lights and their parameters.
    - [] Documentation
- [] GPU Particles
    - [] GPU-based particle system, using compute shaders.
    - [] Have a lot of options to edit the shape, distribution, and gravity. Perhaps save it in a file?
    - [] Documentation

# Update 0.4: Run-Time And Start-Time Performance Enhancement

- [] Run-time performance
    - [] Improve instancing completely. It's currently awfully architected. Take into account the animations and so on.
    - [] Improve the performance of the renderer by using the GPU to dispatch draw calls, using indirect buffers.
    - [] Improve lighting performance using clustered rendering (This is optional if the performance is fine).
    - [] Jobify with the animator or just make it more performant
    - [] Documentation
- [] Start-Time performance
    - [] Improve the load times of the resource manager
    - [] NBR conversion time enhancement
- [] Threading
    - [] A better job system using fibers
    - [] Thread pools
    - [] Documentation

## Update 0.5

- [] Renderer: Beautify v2.0 
    - This one is just a thinking exersice. We don't want to add every rendering technique under the sun to "beautify" the engine's look. 
    - [] Gaussian blur
    - [] Bloom integration 
    - [] Diffuse irradiance IBL (at least)
    - [] Global illumination (GI) 
- [] Renderer: Extra primitives 
    - [] More shapes like planes, capsules, and so on.
    - [] Decal rendering 
    - [] Terrain rendering and procedural generation (terrain generation using `stb_perlin` and loading from heightmap)
    - [] Documentation
- [] Data Structures
    - [] Create your own string library 
    - [] Dynamic array 
    - [] Custom memory allocaters
    - [] Singly-linked and doubly-linked lists
    - [] Stacks and queues
    - [] Ring buffers
    - [] Hash map
    - [] Documentation

## BUGS:
- (Window & Renderer): When resizing the window or changing the fullscreen state, the renderer really does not hold up. I'm guessing it's because of the render passes? They need to update their own frame sizes when the window resizes? 
Maybe find a better way to do this.

- (Renderer): Capsules cannot be rendererd. Need to render those for debug purposes. 
- (Renderer): Spheres are not so perfect. Fix that. 
- (Renderer): Smooth out the pass system a bit. Appending and referring to passes is a bit weird. Paas chain? Passes pool? WTH?

- Check all of the `TODO`, `FIX`, and `TEMP` in the codebase.
