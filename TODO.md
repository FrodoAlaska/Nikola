# URGENT TODO: 
- A function to sub image or slice a texture 
- Put some thought into instancing
- Redocument the graphics context section

# FIXES
- (OpenGL) Have the textures binded to a specific sampler unit?

# FUTURE TODO: 
- Audio context and audio buffers
- Context and window dissociation
- Get rid of GLFW and do it the old-fashioned way? Misery.

# BUGS: 
- MSAA still does not work well in OpenGL
- MSAA in D3D11 needs to be configured some more
- Blending is not completely correct in D3D11

# TEST: 
- (OpenGL) Shader attribute semantics (e.g, does Mat4 work there)

# EXAMPLES: 
- A simple contrived 3D example with rotating cubes. Yes, cube(_S_). Plural.
- An obj loader
- A simple and small drawing app
- A simple image manipulation app

# Engine TODO:
- Meshes
- Fonts 
- NUSL -> Nikol Uniform Shading Language
- Resource manager and loaders for each resource
- Scene manager
