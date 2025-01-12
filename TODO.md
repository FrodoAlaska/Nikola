# URGENT TODO: 
- Add framebuffers and custom render targets
- A function to sub image or slice a texture 
- Put some thought into instancing

# FIXES
- (GL) Add a `GFX_CONTEXT_NONE`?

# FUTURE TODO: 
- Audio context and audio buffers
- Context and window dissociation
- Get rid of GLFW and do it the old-fashioned way? Misery.

# BUGS: 
- MSAA still does not work well in OpenGL (configure the samples in the context not in the window... somehow)
- MSAA in D3D11 needs to be configured some more
- Blending is not completely correct in D3D11

# TEST: 
- (GL) Shader attribute semantics (e.g, does Mat4 work there)

# EXAMPLES: 
- Post-processing example
- An obj loader
- A simple and small drawing app
- A simple image manipulation app

# Engine TODO:
- Meshes
- Fonts 
- NUSL -> Nikol Uniform Shading Language
- Resource manager and loaders for each resource
- Scene manager
