# TODO:

- [] Run-time performance
    - [] Improve instancing completely. It's currently awfully architected.
        - [x] Add an instanced buffer to the `GfxPipelineDesc`
        - [x] Make sure that the layouts of the vertex attributes for both the instance and vertex buffers are correct 
        - [] Ensure that the pipelines have an extra set of vertex attributes for the instance data 
        - [] Start off by changing the PBR shader to include instanced vertex attributes 
        - [] Create a seperate array specifically for instance transforms 
        - [] Implement the rest of the instanced queue functions of the renderer 
        - [] Update the instance buffer with the instanced transforms data
    - [] Billboard and debug passes fuck everything up apparently
    - [] SHADOWS!!!!
    - [] Frustum culling
    - [] Documentation
- [] Threading
    - [] A better job system using `std::function`
    - [] Thread pools
    - [] Documentation
- [] Scenes?
- [] Animations 
    - [] Test animations with multiple instances.
    - [] Documentation
- [] Resources 
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

- Check all of the `TODO`, `FIX`, and `TEMP` in the codebase.
