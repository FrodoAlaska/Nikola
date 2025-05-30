cmake_minimum_required(VERSION 3.27)
project(nikola)

### FetchContent ###
############################################################
include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw 
  URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
)

# GLM
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

# QU3E (CUBE)
set(qu3e_build_demo OFF)

FetchContent_Declare(
  qu3e 
  GIT_REPOSITORY https://github.com/RandyGaul/qu3e
)

FetchContent_MakeAvailable(glfw glm OpenAL qu3e)
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

  ${glfw_SOURCE_DIR}/include
  ${glm_SOURCE_DIR}
  ${OpenAL_SOURCE_DIR}/include
  ${qu3e_SOURCE_DIR}/src
)

set(NIKOLA_LIBRARIES 
  glfw 
  OpenAL
  qu3e
)

# This script includes a bunch of platform-specific variables 
# and definitions that will be helpful later on. 
include(cmake/utils.cmake)
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
############################################################

### CMake Variables ###
############################################################
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
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
