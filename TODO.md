## Update 0.4:

- [] Improve CPU particles 
    - [x] Make a `ParticleEmitter` struct instead of that whole mess 
    - [x] Find a way to replace 2D billboards for particles to 3D meshes 
    - [x] Replace the `GEOMETRY_DEBUG_CUBE` and `GEOMETRY_DEBUG_SPHERE` to `_SIMPLE` instead 
    - [x] Get rid of the billboard pass and replace it with a particle pass 
    - [x] Find a way to render the particles or sumbit a particles draw command to the renderer
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
    - [] We need to get rid of the NBR conversion tool and instead have the conversion process happen in the engine itself. 
    - [] Documentation
- [] Performance v2.0
    - [] Improve the load times of the resource manager
    - [] NBR conversion time enhancement

## Update 0.5:

- [] GPU Particles
    - [] GPU-based particle system, using compute shaders.
    - [] Have a lot of options to edit the shape, distribution, and gravity. Perhaps save it in a file?
    - [] Documentation
- [] Scenes?
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

## BUGS:

- (Renderer): Capsules cannot be rendererd. Need to render those for debug purposes. 
- (Renderer): Spheres are not so perfect. Fix that. 

- Check all of the `TODO`, `FIX`, and `TEMP` in the codebase.
