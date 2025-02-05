cmake_minimum_required(VERSION 3.27)
project(NikolaTestbed)

### Project Variables ###
############################################################
set(TESTBED_SRC_DIR ${CMAKE_SOURCE_DIR}/testbed)
set(TESTBED_INCLUDE_DIR ${TESTBED_SRC_DIR})
set(TESTBED_LIBS_DIR libs)

if(WIN32)
  # Thanks, CMake...
  set(NIKOLA_LIBRARY_DIR ${NIKOLA_OUTPUT_DIR}/"$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>"/nikola.lib)
elseif(UNIX OR LINUX)
  set(NIKOLA_LIBRARY_DIR ${NIKOLA_OUTPUT_DIR}/libnikola.a)
endif()

set(TESTBED_LIBRARIES ${NIKOLA_LIBRARY_DIR} glfw)

set(TESTBED_INCLUDES 
  ${NIKOLA_INCLUDES}
  ${TESTBED_INCLUDE_DIR}
)

if(WIN32)
  set(TESBED_EXE_TYPE "WIN32")
else()
  set(TESBED_EXE_TYPE "")
endif()
############################################################

### Project Sources ###
############################################################
set(TESTBED_SOURCES 
  ${TESTBED_SRC_DIR}/engine_testbed/main.cpp
  ${TESTBED_SRC_DIR}/engine_testbed/app.cpp
)
############################################################

### Final Build ###
############################################################
add_executable(${PROJECT_NAME} ${TESBED_EXE_TYPE} ${TESTBED_SOURCES})
############################################################

### Linking ###
############################################################
add_dependencies(${PROJECT_NAME} nikola)

target_include_directories(${PROJECT_NAME} PUBLIC BEFORE ${TESTBED_INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${TESTBED_LIBRARIES})
############################################################

### Compiling Options ###
############################################################
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_options(${PROJECT_NAME} PUBLIC ${NIKOLA_BUILD_FLAGS})
############################################################
