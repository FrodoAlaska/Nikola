## Update 0.4:

- [] UI Revision
    - [] Add different UI elements 
        - [] Text 
        - [] Progress 
        - [] Tabbed menus  
        - [] Scroll bar
    - [] Make the RmlUI Debugger work
    - [] Tests
        - [] A simple main menu 
        - [] Somewhat of an involved settings menu 
        - [] The perk carousel
    - [] Documentation
- [] Performance v2.0
    - [] Optimize resource loading 
        - [] Meshes/models: Use `cgltf` and `meshoptimizer` instead of bloated Assimp.
        - [] Textures: This needs to be confirmed, but textures are probably the most demanding resource to load.
        - [] Fonts: I'm not sure with this one as well, but fonts can be _very_ demanding.

## Update 0.5:

- [] Renderer: Beautify v2.0 
    - This one is just a thinking exersice. We don't want to add every rendering technique under the sun to "beautify" the engine's look. 
    - [] Diffuse irradiance IBL (at least)
    - [] Global illumination (GI) 
    - [] Gaussian blur
    - [] Bloom integration 
- [] Renderer architecture
    - [] The current render pass system needs to go. 
        - Either reform it completely or get rid of it for more "handmade" passes. 
    - [] Instead, we can have "post-process" passes much similiar to the current pass system, except way simpler.
- [] Resources 
    - [] We need to get rid of the NBR conversion tool and instead have the conversion process happen in the engine itself. 
    - [] Documentation
- [] GPU Particles
    - [] GPU-based particle system, using compute shaders.
    - [] Have a lot of options to edit the shape, distribution, and gravity. Perhaps save it in a file?
    - [] Documentation
- [] Animation 3.0
    - [] Add inverse kinematics 
    - [] User channels/tracking jobs? 
    - [] Additive and partial blending 
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
