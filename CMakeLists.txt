cmake_minimum_required(VERSION 3.27)
project(nikola)

### Project Options ###
############################################################
option(NIKOLA_BUILD_SHARED  "Build Nikola as a shared library" OFF)
option(NIKOLA_BUILD_TESTBED "Build the testbeds with Nikola"   OFF)
option(NIKOLA_BUILD_NBR     "Build the NBR tool with Nikola"   ON)
option(NIKOLA_DISTRIBUTE    "Enable the distribution build"    OFF)

if(NIKOLA_BUILD_SHARED)
  set(NIKOLA_BUILD_TYPE SHARED)
  set(BUILD_SHARED_LIBS ON)
else()
  set(NIKOLA_BUILD_TYPE STATIC)
  set(BUILD_SHARED_LIBS OFF)
endif()

if(NIKOLA_DISTRIBUTE)
  add_definitions("-DNIKOLA_BUILD_DISTRIBUTION")
endif()
############################################################

### FetchContent ###
############################################################
include(FetchContent)

if(WIN32)
  if(USE_STATIC_MSVC_RUNTIME_LIBRARY)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  endif()
endif()

# GLFW

FetchContent_Declare(
  glfw-3.4 
  URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
)

# GLM

set(GLM_ENABLE_SIMD_AVX ON)

FetchContent_Declare(
  glm 
  URL https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip
)

# OpenAL-Soft

set(ALSOFT_EXAMPLES OFF)
set(LIBTYPE STATIC)

FetchContent_Declare(
  OpenAL 
  URL https://github.com/kcat/openal-soft/archive/refs/tags/latest.zip
)

# JoltPhysics

FetchContent_Declare(
  jolt 
  URL           https://github.com/jrouwe/JoltPhysics/archive/refs/tags/v5.3.0.zip
  SOURCE_SUBDIR "Build"
)

# Tracy

FetchContent_Declare(
  tracy 
  URL https://github.com/wolfpld/tracy/archive/refs/tags/v0.12.2.zip
)

# OZZ Animation

set(ozz_build_tools    OFF)
set(ozz_build_fbx      OFF)
set(ozz_build_gltf     OFF)
set(ozz_build_data     OFF)
set(ozz_build_samples  OFF)
set(ozz_build_howtos   OFF)
set(ozz_build_tests    OFF)
set(ozz_build_simd_ref ON)

FetchContent_Declare(
  ozz 
  URL https://github.com/guillaumeblanc/ozz-animation/archive/refs/tags/0.16.0.zip
)

FetchContent_MakeAvailable(glfw-3.4 glm OpenAL jolt tracy ozz)
############################################################

### Project Variables ###
############################################################
set(NIKOLA_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nikola/src)
set(NIKOLA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nikola/include)
set(NIKOLA_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(NIKOLA_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/nikola)

set(NIKOLA_INCLUDES 
  ${NIKOLA_INCLUDE_DIR}
  ${NIKOLA_LIBS_DIR}

  ${glfw-3.4_SOURCE_DIR}/include
  ${glm_SOURCE_DIR}
  ${OpenAL_SOURCE_DIR}/include
  ${jolt_SOURCE_DIR}/Jolt
  ${tracy_SOURCE_DIR}/public
  ${ozz_SOURCE_DIR}/include
)

set(NIKOLA_LIBRARIES 
  glfw 
  OpenAL
  Jolt
  TracyClient
  ozz_animation
  ozz_animation_offline
)

# This script includes a bunch of platform-specific variables 
# and definitions that will be helpful later on. 
include(cmake/utils.cmake)
############################################################

### Sub Directories ###
############################################################
add_subdirectory(nikola)

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
