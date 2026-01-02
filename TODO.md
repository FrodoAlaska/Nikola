## 2026 Plan 

- Improve the graphics fidelity of the engine. Improve it _a ton_. It is one of the weakest points of the engine, and it needs work.
- Improve upon the performance of the engine. While the engine is not slow, there are a lot of systems that _need_ a performance boost. 
- Add a capabale GPU particle system with a wide variety of abilities for shaping the particles and controling everything about them.
- Add the ability to render heightmaps, as well as interact with them on the physics level.
- Add procedural generation tools. Math functions using the `stb_perlin` library, and components in the entity system to easily use proc-gen. 

## Update 0.4:

- [] UI Revision
    - [] Tests
        - [] Dialogue box
        - [] A simple main menu 
        - [] Somewhat of an involved settings menu 
        - [] The perk carousel
- [] Performance v2.0
    - [] Optimize resource loading 
        - [] Meshes/models: Use `cgltf` and `meshoptimizer` instead of bloated Assimp.
        - [] Textures: This needs to be confirmed, but textures are probably the most demanding resource to load.
        - [] Fonts: I'm not sure with this one as well, but fonts can be _very_ demanding.
    - [] Custom memory pools 
        - We need to consider creating memory pools to avoid memory allocations as much as possible. 
        - The gfx backend and the renderers are especially in need of this. They allocate shit left and right. 
        - [] Create a simple pool allocater and pass it into the various functions that need it. 
        - [] Perhaps override the `new` and `delete` operator so that we can watch the allocations on the STL side as well?
        - Memory. Just think about memory.

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
