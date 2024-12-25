cmake_minimum_required(VERSION 3.27)
project(nikol)

### Project Variables ###
############################################################
set(NIKOL_SRC_DIR nikol/src)
set(NIKOL_INCLUDE_DIR nikol/include)
set(NIKOL_LIBS_DIR libs)
set(NIKOL_LIBRARIES glfw)

set(NIKOL_BUILD_OPTS -w -W0)
set(NIKOL_BUILD_DEFS GLFW_INCLUDE_NONE)
set(NIKOL_BUILD_LIBS_DIR ${NIKOL_INCLUDE_DIR} ${NIKOL_LIBS_DIR} ${NIKOL_LIBS_DIR}/GLFW/include)

set(NIKOL_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
############################################################

### Project Options ###
############################################################
option(NIKOL_BUILD_SHARED "Build Nikol as a shared library" ON)

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
)

set(NIKOL_HEADERS
  ${NIKOL_INCLUDE_DIR}/nikol_core.hpp
)
############################################################

### Library Sources ###
############################################################
set(LIBS_SOURCES 
  ${NIKOL_LIBS_DIR}/glad/glad.c
)
############################################################

### Final Build ###
############################################################
add_library(${PROJECT_NAME} ${NIKOL_BUILD_TYPE} ${NIKOL_SOURCES} ${LIBS_SOURCES})
############################################################

### Linking ###
############################################################
target_include_directories(${PROJECT_NAME} PUBLIC BEFORE ${NIKOL_BUILD_LIBS_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${NIKOL_LIBRARIES})
############################################################

### Compiling Options ###
############################################################
target_compile_definitions(${PROJECT_NAME} PUBLIC ${NIKOL_BUILD_DEFS})
target_compile_options(${PROJECT_NAME} PUBLIC ${NIKOL_BUILD_OPTS})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
############################################################

### Library Install ###
############################################################
install(TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include)
############################################################
