cmake_minimum_required(VERSION 3.27)
project(nikola)

### Project Variables ###
############################################################
set(NIKOLA_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nikola/src)
set(NIKOLA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nikola/include)
set(NIKOLA_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

set(NIKOLA_BUILD_DEFS GLFW_INCLUDE_NONE)

set(NIKOLA_LIBRARIES glfw)
set(NIKOLA_INCLUDES 
  ${NIKOLA_INCLUDE_DIR} 
  ${NIKOLA_LIBS_DIR} 
  ${NIKOLA_LIBS_DIR}/glfw/include
  ${NIKOLA_LIBS_DIR}/glm
)

set(NIKOLA_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

# Set platform-specific variables  
if(WIN32)
  set(NIKOLA_BUILD_FLAGS
    "/W0"
  )
elseif(UNIX OR LINUX)
  set(NIKOLA_BUILD_FLAGS
    "-lm" 
    "-w"
  )
endif()
############################################################

### Project Options ###
############################################################
option(NIKOLA_BUILD_SHARED  "Build Nikola as a shared library" OFF)
option(NIKOLA_BUILD_TESTBED "Build the testbeds with Nikola" ON)
option(NIKOLA_BUILD_NBR     "Build the NBR tool with Nikola" ON)

# Set it to shared
if(NIKOLA_BUILD_SHARED)
  set(NIKOLA_BUILD_TYPE SHARED)
  set(BUILD_SHARED_LIBS ON)
else()
  set(NIKOLA_BUILD_TYPE STATIC)
  set(BUILD_SHARED_LIBS OFF)
endif()

if(WIN32)
  # Thanks, CMake...
  set(NIKOLA_LIBRARY_DIR ${NIKOLA_OUTPUT_DIR}/"$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>"/nikolad.lib)
elseif(LINUX)
  set(NIKOLA_LIBRARY_DIR ${NIKOLA_OUTPUT_DIR}/libnikola.a)
endif()
############################################################

### CMake Variables ###
############################################################
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
############################################################

### Libraries ###
############################################################
add_subdirectory(libs/glfw)
############################################################

### Project Sources ###
############################################################
set(NIKOLA_SOURCES 
  # Core/Base
  ${NIKOLA_SRC_DIR}/core/base/nikola.cpp
  ${NIKOLA_SRC_DIR}/core/base/logger.cpp
  ${NIKOLA_SRC_DIR}/core/base/event.cpp
  ${NIKOLA_SRC_DIR}/core/base/window.cpp
  ${NIKOLA_SRC_DIR}/core/base/nikola_memory.cpp
  ${NIKOLA_SRC_DIR}/core/base/input.cpp
  ${NIKOLA_SRC_DIR}/core/base/nikola_clock.cpp
  
  # Core/Gfx
  ${NIKOLA_SRC_DIR}/core/gfx/gl_backend.cpp
  ${NIKOLA_SRC_DIR}/core/gfx/dx11_backend.cpp
  
  # Engine
  ${NIKOLA_SRC_DIR}/engine/engine.cpp
  ${NIKOLA_SRC_DIR}/engine/filesystem.cpp

  # Engine/Math 
  ${NIKOLA_SRC_DIR}/engine/math/math_common.cpp
  ${NIKOLA_SRC_DIR}/engine/math/random.cpp
  ${NIKOLA_SRC_DIR}/engine/math/vector_types.cpp
  ${NIKOLA_SRC_DIR}/engine/math/matrix_types.cpp
  ${NIKOLA_SRC_DIR}/engine/math/quaternion.cpp
  ${NIKOLA_SRC_DIR}/engine/math/transform.cpp
  
  # Engine/Resources
  ${NIKOLA_SRC_DIR}/engine/resources/resource_manager.cpp
  ${NIKOLA_SRC_DIR}/engine/resources/material.cpp
  ${NIKOLA_SRC_DIR}/engine/resources/nbr_file.cpp
  
  # Engine/Resources/Loaders 
  ${NIKOLA_SRC_DIR}/engine/resources/loaders/mesh_loader.cpp
  ${NIKOLA_SRC_DIR}/engine/resources/loaders/material_loader.cpp
  ${NIKOLA_SRC_DIR}/engine/resources/loaders/skybox_loader.cpp
  
  # Engine/Renderer 
  ${NIKOLA_SRC_DIR}/engine/renderer/camera.cpp
  ${NIKOLA_SRC_DIR}/engine/renderer/renderer.cpp
)

set(NIKOLA_HEADERS
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_core.hpp
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_engine.hpp
)
############################################################

### Library Sources ###
############################################################
set(LIBS_SOURCES 
  ${NIKOLA_LIBS_DIR}/glad/glad.c
)
############################################################

### Final Build ###
############################################################
add_library(${PROJECT_NAME} ${NIKOLA_BUILD_TYPE} ${NIKOLA_SOURCES} ${NIKOLA_HEADERS} ${LIBS_SOURCES})
############################################################

### Linking ###
############################################################
target_include_directories(${PROJECT_NAME} PUBLIC BEFORE ${NIKOLA_INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${NIKOLA_LIBRARIES})
############################################################

### Compiling Options ###
############################################################
target_compile_options(${PROJECT_NAME} PUBLIC ${NIKOLA_BUILD_FLAGS})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_definitions(${PROJECT_NAME} PUBLIC ${NIKOLA_BUILD_DEFS})
############################################################

### Sub Projects ###
############################################################
if(NIKOLA_BUILD_TESTBED) 
  add_subdirectory(testbed)
endif()

if(NIKOLA_BUILD_NBR) 
  add_subdirectory(NBR)
endif()
############################################################

### Library Install ###
############################################################
install(TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/nikola/include DESTINATION include)
############################################################
