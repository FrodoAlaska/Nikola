One header file for everything. Every implementation of every function/type will be in its own translation unit. However, all the definitions are going to be in the "nikol_core.h".

# Nikol Core
It's going to be a mix between Raylib, SDL, and SFML. A framework that will wrap around various different libraries. However, it will _not_ be an engine. Only a framework to build other application like games and game engines. Nikol Core itself, though, is not an engine. It will not have a renderer. It will not have a physics system. It will not have a fully-fledged audio system. Still, it will handle things like loading textures, audio files, models, create meshes, graphic buffers, fonts, shaders, and so on. How these come together, however, is left to the user of this framework. 

## Base
- Window
- Input 
- Asserts
- Logging 
- Containers 
- Memory 
- Defines 
- Platform-specific stuff 
- Clock
- Threads?

## GFX 
- Context 
- Vertex buffer 
- Index buffer 
- Pipeline?

## Math 
- Random 
- Vector
- Matrix 
- Quaternion
- Utils

## File 
- Path string 
- File system abstraction 

# Nikol Engine 
- Main entry point abstraction
- Physics 
- A decent 3D and 2D renderer 
- A robust resource manager 
- An entity system 
- A scene system 
- Particles
- Model animations 
- Camera

## Math
- Transform 
- Vertex 

## Resources
- Texture 
- Font 
- Music 
- Sound 
- Vertex buffer 
- Index buffer 
- Shader 
- Mesh 
- Model 

# Nikol-Ed
Just a normal editor that will work with the engine part of Nikol. It is essentially an application of the Nikol Engine. 

# Nikol Build 
A CLI tool to safely and easily build any version of nikol with the required dependencies. It will handle everything like installing and managing dependencies, building a specific version of nikol (like just building `core` for example), and maybe some extra other functionality.

# API 

## Core 
nikol::init(nikol::INIT_LOGGER);

nikol::Window* window = nikol::window_open(1280, 720, "Nikol", window_flags);
if(!window) {
    nikol::log(nikol::LOG_FATAL, "Some message %s", a_string_of_some_kind);
    return -1; 
}

while(nikol::window_is_open(window)) {
    nikol::window_poll_events(window); // For input and accurate timing
    nikol::window_swap(window); // Swaping the graphics buffer
}

nikol::window_close(window);
nikol::shutdown();

## Engine 
struct App {
    nikol::Window* window; 
    nikol::Renderer* renderer;
    nikol::PhysicsSystem* physics; 
    nikol::SceneManager* scenes; 
    nikol::ResourceManager* resources;
};

nikol::engine_init(app, 1280, 720, "Engine", other_flags);
nikol::engine_run();
nikol::engine_shutdown();
