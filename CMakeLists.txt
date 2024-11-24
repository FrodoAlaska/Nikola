cmake_minimum_required(VERSION 3.27)
project(NikolCore)

### Project Variables ###
############################################################
set(NIKOL_SRC_DIR src)
set(NIKOL_LIBS_DIR libs)
set(NIKOL_LIBRARIES glfw)

set(NIKOL_BUILD_TYPE SHARED)
set(NIKOL_BUILD_OPTS "-lm -Wall")
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
  ${NIKOL_SRC_DIR}/core/base/memory.cpp
  ${NIKOL_SRC_DIR}/core/base/input.cpp
  ${NIKOL_SRC_DIR}/core/base/nikol_clock.cpp
  
  # Core/Gfx
  ${NIKOL_SRC_DIR}/core/gfx/gl_backend.cpp
)

set(NIKOL_HEADERS
  ${CMAKE_SOURCE_DIR}/nikol_core.h
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
target_include_directories(${PROJECT_NAME} PUBLIC BEFORE ${NIKOL_LIBS_DIR} ${NIKOL_LIBS_DIR}/GLFW/include ${NIKOL_SRC_DIR} ${CMAKE_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${NIKOL_LIBRARIES})
############################################################

### Compiling Options ###
############################################################
target_compile_definitions(${PROJECT_NAME} PUBLIC ${NIKOL_BUILD_TYPE} GLFW_INCLUDE_NONE)
target_compile_options(${PROJECT_NAME} PUBLIC -lm -Wall)
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
