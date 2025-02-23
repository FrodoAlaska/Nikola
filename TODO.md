# (Engine) TODO: 
- Better build scripts for Linux
- Font loading 
- Batch rendering 
- Render 2D and 3D fonts
- Better materials
- Better resource IDs generation

# (Core) TODO: 
- Audio
- Seperate the `gl_backend.cpp` file into several files for better visualization
- Potentially fix the memory allocater functions
- A function to sub image or slice a texture 
- Put some thought into instancing

# (Engine) FIXES:
- Fix the various memory leaks in `NBR` and `resource_manager`
- Make the resource loaders faster and less memory intensive

# (Core) FIXES:
- EMPTY FOR NOW...

# (Engine) BUGS: 
- EMPTY FOR NOW...

# (Core) BUGS: 
- EMPTY FOR NOW...

# TEST: 
- (GL) Shader attribute semantics (e.g, does Mat4 work there)
- (Engine) Test the resource loaders

# FUTURE PLANS: 
- (Audio) Audio context and audio buffers
- (Window) Get rid of GLFW and do it the old-fashioned way? Misery.
- (GFX) Context and window dissociation
- (GFX) Add support for geometry shaders
- (UI) Start working on the editor (basically a wrapper around ImGui)
- Phyiscs. Yes. Just physics.
- GPU particles
- Hot reloading
