cmake_minimum_required(VERSION 3.27)
project(nikola)

### Project Variables ###
############################################################
set(NIKOLA_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nikola/src)
set(NIKOLA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nikola/include)
set(NIKOLA_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(NIKOLA_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(NIKOLA_LIBRARIES glfw)
set(NIKOLA_INCLUDES 
  ${NIKOLA_INCLUDE_DIR} 
  ${NIKOLA_LIBS_DIR} 
  ${NIKOLA_LIBS_DIR}/glfw/include
  ${NIKOLA_LIBS_DIR}/glm
)

# This script includes a bunch of platform-specific variables and definitions that
# will be helpful later on.
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
add_subdirectory(libs/glfw)
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
