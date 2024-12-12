# DX11 TODO: 
- Textures 
- Enable Blending 

# URGENT TODO: 
- Remove `NIKOL_MAIN`. That is more of an engine thing.
- Put the "nikol_core.h" file in the "src" directory
- Better testing env
- Audio context and audio buffers

# FUTURE TODO: 
- Get rid of GLFW and do it the old-fashioned way? Misery.
- Rename the whole repo to "Nikol" or "NikolLib" or "NikolEngine" instead of just "NikolCore".

# EXAMPLES: 
- A simple and small drawing app

# GFX Flags: 
- Enabling MSAA and configuring the amount of subsamples 
- Enabling the depth and stencil tests and configuring their functions 
- Enabling blending and configuring its function 
- Enabling culling and configuring its function and if it should cull from the back or front face 

# Engine TODO:
- Meshes
- Make a 3D example
- Fonts 
- NUSL -> Nikol Uniform Shading Language
- Resource manager and loaders for each resource
- Scene manager

sizei uniform_index = gfx_shader_create_uniform_buffer(gfx, shader, sizeof(1024));

GfxUniformDesc uni_desc = {
    .index = uniform_index, 
    .type  = GFX_LAYOUT_MAT4,
    .data  = &view_projection, 
    .size  = sizeof(Mat4),
};
gfx_shader_upload_uniform(gfx, shader, uni_desc); <- Will only _update_ the constant/uniform buffer. But it will _not_ upload it to the shader.

// This will _actually_ upload the uniform buffer to the shader 

