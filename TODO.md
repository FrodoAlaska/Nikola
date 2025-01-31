# (Engine) TODO: 
- Resource loaders -> obj/gltf/model loader, font loader
- A way to startup the engine and to setup an application callback
- Design and implement a resource manager

# (Core) TODO: 
- A function to sub image or slice a texture 
- Put some thought into instancing

# (Engine) FIXES:
- EMPTY FOR NOW...

# (Core) FIXES:
- EMPTY FOR NOW...

# (Engine) BUGS: 
- EMPTY FOR NOW...

# (Core) BUGS: 
- (GL) MSAA still does not work well (configure the samples in the context not in the window... somehow)
- (D3D11) MSAA in D3D11 needs to be configured some more
- (D3D11) Blending is not completely correct in D3D11

# TEST: 
- (GL) Shader attribute semantics (e.g, does Mat4 work there)
- (Engine) Test the resource loaders

# FUTURE PLANS: 
- (Audio) Audio context and audio buffers
- (Window) Get rid of GLFW and do it the old-fashioned way? Misery.
- (GFX) Context and window dissociation
- (GFX) Add support for geometry shaders

# EXAMPLES: 
- An obj loader
- A simple and small drawing app
- A simple image manipulation app
