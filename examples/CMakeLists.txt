cmake_minimum_required(VERSION 3.27)
project(NikolExamples)

### CMake Variables ###
############################################################
set(CMAKE_BUILD_TYPE DEBUG)
############################################################

### Libraries ###
############################################################
############################################################

### Project Sources ###
############################################################
set(SOURCES 
  src/gl_example.cpp
  # src/dx11_example.cpp
)
############################################################

### Final Build ###
############################################################
add_executable(${PROJECT_NAME} ${SOURCES})
############################################################

### Linking ###
############################################################
target_include_directories(${PROJECT_NAME} PUBLIC BEFORE ${NIKOL_BUILD_LIBS_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${NIKOL_OUTPUT_DIR}/libnikol.so ${NIKOL_LIBRARIES})
############################################################

### Compiling Options ###
############################################################
target_compile_options(${PROJECT_NAME} PUBLIC -w -Wall)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
############################################################
