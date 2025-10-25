# TODO:

- [x] Additional UI widgets 
    - [x] Add `UI_IMAGE_ENTERED` and `UI_IMAGE_EXITED` events 
    - [x] Add a `UIMenu` widget
- [x] Integrate Tracy
- [] Run-time performance
    - [] Improve instancing completely. It's currently awfully architected. Take into account the animations and so on.
        - [] NOTE: Perhaps have an internal cache in the renderer of all the materials being used? 
        - [] NOTE: We can then use this cache to determine how many instances of a mesh needs to be drawn. 
        - [] NOTE: Think about it, materials are the primitives that is most valuable to an instanced draw call.
        - [] NOTE: That way, the user will not have to worry about what's instanced and what's not.
    - [] Improve the performance of the renderer by using the GPU to dispatch draw calls, using indirect buffers.
    - [] Improve lighting performance using clustered rendering (This is optional if the performance is fine).
    - [] Documentation
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
- [] Start-Time performance
    - [] Improve the load times of the resource manager
    - [] NBR conversion time enhancement
- [] Threading
    - [] A better job system using fibers
    - [] Thread pools
    - [] Documentation
- [] Renderer: Beautify v2.0 
    - This one is just a thinking exersice. We don't want to add every rendering technique under the sun to "beautify" the engine's look. 
    - [] Gaussian blur
    - [] Bloom integration 
    - [] Diffuse irradiance IBL (at least)
    - [] Global illumination (GI) 
- [] Renderer: Extra primitives 
    - [] More shapes like planes, capsules, and so on.
    - [] Decal rendering 
    - [] Add noise functions to the math library
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

- (Engine): Perhaps have a config file format working in the engine for things like volume, resolution, and such.

- (Renderer): Capsules cannot be rendererd. Need to render those for debug purposes. 
- (Renderer): Spheres are not so perfect. Fix that. 
- (Renderer): Smooth out the pass system a bit. Appending and referring to passes is a bit weird. Paas chain? Passes pool? WTH?

- Check all of the `TODO`, `FIX`, and `TEMP` in the codebase.
