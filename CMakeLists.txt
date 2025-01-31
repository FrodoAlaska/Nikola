cmake_minimum_required(VERSION 3.27)
project(nikol)

### Project Variables ###
############################################################
set(NIKOL_SRC_DIR ${CMAKE_SOURCE_DIR}/nikol/src)
set(NIKOL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/nikol/include)
set(NIKOL_LIBS_DIR ${CMAKE_SOURCE_DIR}/libs)

set(NIKOL_BUILD_OPTS -Wall)
set(NIKOL_BUILD_DEFS GLFW_INCLUDE_NONE)

set(NIKOL_LIBRARIES glfw)
set(NIKOL_INCLUDES 
  ${NIKOL_INCLUDE_DIR} 
  ${NIKOL_LIBS_DIR} 
  ${NIKOL_LIBS_DIR}/glfw/include
  ${NIKOL_LIBS_DIR}/glm
)

set(NIKOL_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
############################################################

### Project Options ###
############################################################
option(NIKOL_BUILD_SHARED "Build Nikol as a shared library" ON)
option(NIKOL_BUILD_EXAMPLES "Build the examples with Nikol" ON)

# Set it to shared
if(NIKOL_BUILD_SHARED)
  set(NIKOL_BUILD_TYPE SHARED)
  set(BUILD_SHARED_LIBS ON)
else()
  set(NIKOL_BUILD_TYPE STATIC)
  set(BUILD_SHARED_LIBS OFF)
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
set(NIKOL_SOURCES 
  # Core/Base
  ${NIKOL_SRC_DIR}/core/base/nikol.cpp
  ${NIKOL_SRC_DIR}/core/base/logger.cpp
  ${NIKOL_SRC_DIR}/core/base/event.cpp
  ${NIKOL_SRC_DIR}/core/base/window.cpp
  ${NIKOL_SRC_DIR}/core/base/nikol_memory.cpp
  ${NIKOL_SRC_DIR}/core/base/input.cpp
  ${NIKOL_SRC_DIR}/core/base/nikol_clock.cpp
  
  # Core/Gfx
  ${NIKOL_SRC_DIR}/core/gfx/gl_backend.cpp
  ${NIKOL_SRC_DIR}/core/gfx/dx11_backend.cpp

  # Engine/Math 
  ${NIKOL_SRC_DIR}/engine/math/math_common.cpp
  ${NIKOL_SRC_DIR}/engine/math/random.cpp
  ${NIKOL_SRC_DIR}/engine/math/vector_types.cpp
  ${NIKOL_SRC_DIR}/engine/math/matrix_types.cpp
  ${NIKOL_SRC_DIR}/engine/math/quaternion.cpp
  ${NIKOL_SRC_DIR}/engine/math/transform.cpp
  
  # Engine/Resources
  ${NIKOL_SRC_DIR}/engine/resources/resource_manager.cpp
  
  # Engine/Resources/Loaders 
  ${NIKOL_SRC_DIR}/engine/resources/loaders/texture_loader.cpp
  ${NIKOL_SRC_DIR}/engine/resources/loaders/cubemap_loader.cpp
)

set(NIKOL_HEADERS
  ${NIKOL_INCLUDE_DIR}/nikol/nikol_core.hpp
  ${NIKOL_INCLUDE_DIR}/nikol/nikol_engine.hpp
)
############################################################

### Library Sources ###
############################################################
set(LIBS_SOURCES 
  ${NIKOL_LIBS_DIR}/glad/glad.c
  ${NIKOL_LIBS_DIR}/stb/stb_image.cpp
)
############################################################

### Final Build ###
############################################################
add_library(${PROJECT_NAME} ${NIKOL_BUILD_TYPE} ${NIKOL_SOURCES} ${NIKOL_HEADERS} ${LIBS_SOURCES})
############################################################

### Linking ###
############################################################
target_include_directories(${PROJECT_NAME} PUBLIC BEFORE ${NIKOL_INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${NIKOL_LIBRARIES})
############################################################

### Compiling Options ###
############################################################
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_definitions(${PROJECT_NAME} PUBLIC ${NIKOL_BUILD_DEFS})
target_compile_options(${PROJECT_NAME} PUBLIC ${NIKOL_BUILD_OPTS})
############################################################

### Examples ###
############################################################
if(NIKOL_BUILD_EXAMPLES) 
  add_subdirectory(examples)
endif()
############################################################

### Library Install ###
############################################################
install(TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include)
############################################################
