# (Engine) TODO: 
* General 
    - Check the todos in the code and fix little things
    - Probably create my own `DynamicAllocater` instead of the STL one
* Resources 
    - Fix the various memory leaks in `NBR` and `resource_manager`
    - Unload the resources without crash
    - Better resource IDs generation. Maybe use handles? Perhaps make the IDs more like indices?
    - Font loading 
    - A memory pool/arena for resources?
* Renderer 
    - Better materials/shaders. Have preset variable names as consts that could be given to functions which will send the intended uniforms so the shader.
    - Maybe let the material have a hashmap to store all of the uniform locations a priori?
    - Basic lighting
    - Create a `RendererDefault` struct to hold all of the initial default values.
    - Batch rendering 
    - Render 2D and 3D fonts

# (Core) TODO: 
* General
    - Potentially fix the memory allocater functions. So instead of normal wrappers around C allocation functions, perhaps have something more sophisticated
    - Get rid of GLFW and do it the old-fashioned way? Misery.
* Logger 
    - Batch logging? 
    - Log into a file
* GFX 
    - Seperate the `gl_backend.cpp` file into several files for better visualization
    - A function to sub image or slice a texture 
    - Put some thought into instancing
    - Add support for geometry shaders
* Audio 
    - Audio context and audio buffers

# (Engine) BUGS: 
- EMPTY FOR NOW...

# (Core) BUGS: 
- EMPTY FOR NOW...

# TEST: 
- (GL) Shader attribute semantics (e.g, does Mat4 work there)

# FUTURE PLANS: 
- (UI) Start working on the editor (basically a wrapper around ImGui)
- Phyiscs. Yes. Just physics.
- GPU particles
- Hot reloading
- Multi-threading
