## Update 0.4:

- [] Resources 
    - [] Optimize resource loading 
        - [] Meshes/models: Use `cgltf` and `meshoptimizer` instead of bloated Assimp.
        - [] Textures: This needs to be confirmed, but textures are probably the most demanding resource to load.
        - [] Fonts: I'm not sure with this one as well, but fonts can be _very_ demanding.
    - [] Make a simplified language for things like entity formats, materials, and the like. Call it `nkl`.
    - [] Material file format like `.mat` or something?
    - [] We need to get rid of the NBR conversion tool and instead have the conversion process happen in the engine itself. 
    - [] Documentation
- [] Performance v2.0
    - [] Improve the load times of the resource manager
    - [] NBR conversion time enhancement

## Update 0.5:

- [] Renderer architecture
    - [] The current render pass system needs to go. 
        - Either reform it completely or get rid of it for more "handmade" passes. 
    - [] Instead, we can have "post-process" passes much similiar to the current pass system, except way simpler.
- [] Renderer: Beautify v2.0 
    - This one is just a thinking exersice. We don't want to add every rendering technique under the sun to "beautify" the engine's look. 
    - [] Gaussian blur
    - [] Bloom integration 
    - [] Diffuse irradiance IBL (at least)
    - [] Global illumination (GI) 
- [] GPU Particles
    - [] GPU-based particle system, using compute shaders.
    - [] Have a lot of options to edit the shape, distribution, and gravity. Perhaps save it in a file?
    - [] Documentation
- [] Animation 3.0
    - [] Add inverse kinematics 
    - [] User channels/tracking jobs? 
- [] Scenes?
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
