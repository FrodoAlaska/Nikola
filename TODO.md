# TODO:

- [x] Additional UI widgets 
    - [x] Add `UI_IMAGE_ENTERED` and `UI_IMAGE_EXITED` events 
    - [x] Add a `UIMenu` widget
- [x] Integrate Tracy
- [] Run-time performance
    - [] Improve the performance of the renderer by using the GPU to dispatch draw calls, using indirect buffers.
        - [x] Change the meshes and skyboxes representation of their data 
        - [x] Change the geometry loader
        - [x] Update the resource manager to accommodate for the new changes
        - [] Change the renderer's queueing functions to the new architecture
        - [] Update the buffers somewhere 
        - [] Change the render passes
    - [] Improve instancing completely. It's currently awfully architected. Take into account the animations and so on.
    - [] Improve lighting performance using clustered rendering (This is optional if the performance is fine).
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
- [] Threading
    - [] A better job system using `std::function`
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

```c++

void renderer_sumbit() {
    // Update the internals buffer

    gfx_buffer_upload_data(renderer.pipe_desc.vertex_buffer, 0, renderer.vertices_buffer.size(), renderer.vertices_buffer.data());
    gfx_buffer_upload_data(renderer.pipe_desc.index_buffer, 0, renderer.indices_buffer.size(), renderer.indices_buffer.data());
    gfx_buffer_upload_data(renderer.matrix_buffer, 0, renderer.transforms_buffer.size(), renderer.transforms_buffer.data());
    gfx_buffer_upload_data(renderer.material_buffer, 0, renderer.materials_buffer.size(), renderer.materials_buffer.data());
    gfx_buffer_upload_data(renderer.command_buffer, 0, renderer.commands_buffer.size(), renderer.commands_buffer.data());

    // Clear all the buffers for the next draw calls...
    
    renderer.vertices_buffer.clear();
    //...

    // Draw call

    
}

```

## BUGS:
- (Window & Renderer): When resizing the window or changing the fullscreen state, the renderer really does not hold up. I'm guessing it's because of the render passes? They need to update their own frame sizes when the window resizes? 
Maybe find a better way to do this.

- (Engine): Perhaps have a config file format working in the engine for things like volume, resolution, and such.

- (Renderer): Capsules cannot be rendererd. Need to render those for debug purposes. 
- (Renderer): Spheres are not so perfect. Fix that. 
- (Renderer): Smooth out the pass system a bit. Appending and referring to passes is a bit weird. Paas chain? Passes pool? WTH?

- Check all of the `TODO`, `FIX`, and `TEMP` in the codebase.
