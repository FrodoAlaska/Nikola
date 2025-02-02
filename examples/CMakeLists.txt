cmake_minimum_required(VERSION 3.27)
project(NikolaExamples)

### Project Variables ###
############################################################
set(EXAMPLES_SRC_DIR ${CMAKE_SOURCE_DIR}/examples)
set(EXAMPLES_INCLUDE_DIR ${EXAMPLES_SRC_DIR})
set(EXAMPLES_LIBS_DIR libs)

if(WIN32)
  # Thanks, CMake...
  set(NIKOLA_LIBRARY_DIR ${NIKOLA_OUTPUT_DIR}/"$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>"/nikola.lib)
elseif(UNIX OR LINUX)
  set(NIKOLA_LIBRARY_DIR ${NIKOLA_OUTPUT_DIR}/libnikola.a)
endif()

set(EXAMPLES_LIBRARIES ${NIKOLA_LIBRARY_DIR} glfw)

set(EXAMPLES_INCLUDES 
  ${NIKOLA_INCLUDES}
  ${EXAMPLES_INCLUDE_DIR}
)
############################################################

### Project Sources ###
############################################################
set(EXAMPLES_SOURCES 
  ${EXAMPLES_SRC_DIR}/basic_example/main.cpp
)
############################################################

### Final Build ###
############################################################
add_executable(${PROJECT_NAME} ${EXAMPLES_SOURCES})
############################################################

### Linking ###
############################################################
add_dependencies(${PROJECT_NAME} nikola)

target_include_directories(${PROJECT_NAME} PUBLIC BEFORE ${EXAMPLES_INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${EXAMPLES_LIBRARIES})
############################################################

### Compiling Options ###
############################################################
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_options(${PROJECT_NAME} PUBLIC ${NIKOLA_BUILD_FLAGS})
############################################################
