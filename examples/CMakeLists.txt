cmake_minimum_required(VERSION 3.27)
project(NikolExamples)

### CMake Variables ###
############################################################
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_PREFIX_PATH ../build/)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
############################################################

### Libraries ###
############################################################
find_library(NikolCore NikolCore)
############################################################

### Project Sources ###
############################################################
set(SOURCES 
  src/gl_example.cpp
)
############################################################

### Final Build ###
############################################################
add_executable(${PROJECT_NAME} ${SOURCES})
############################################################

### Linking ###
############################################################
target_include_directories(${PROJECT_NAME} PUBLIC BEFORE ../ ../libs/glm/)
target_link_libraries(${PROJECT_NAME} PUBLIC ${NikolCore})
############################################################

### Compiling Options ###
############################################################
target_compile_options(${PROJECT_NAME} PUBLIC -lm -Wall)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
############################################################
