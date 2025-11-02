# TODO:

- [] Threading
    - [] A better job system using `std::function`
    - [] Different threads for different tasks. 
        - [] Perhaps there's no need to do a job manager. Just spin up a few threads to handle different areas of the engine. i.e, render thread, physics thread, etc.
        - [] Each thread will await certain jobs. The resource thread will await and resolve resource loading threads, and etc.
        - [] The `physics_world_step` function can run on a thread like a server
        - [] Perhaps even have a certain thread specifically for game logic
    - [] Thread pools
        - For implementation use [moodycamel](https://github.com/cameron314/concurrentqueue)? 
    - [] Documentation
- [] Animations 
    - [] OZZ animation? I mean, it's, like, fucking _amazing_. More research needed.
    - [] Test animations with multiple instances.
    - [] Documentation
- [] Resources 
    - [] Material file format like `.mat` or something?
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
- [] Scenes?
- [] Performance v2.0
    - [] Improve the load times of the resource manager
    - [] NBR conversion time enhancement
    - [] Simple frustum culling
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

- (Renderer): Capsules cannot be rendererd. Need to render those for debug purposes. 
- (Renderer): Spheres are not so perfect. Fix that. 

- Check all of the `TODO`, `FIX`, and `TEMP` in the codebase.
