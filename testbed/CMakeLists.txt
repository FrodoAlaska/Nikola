cmake_minimum_required(VERSION 3.27)
project(NikolaTestbed)

### Project Variables ###
############################################################
set(TESTBED_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TESTBED_INCLUDE_DIR ${TESTBED_SRC_DIR})
set(TESTBED_LIBS_DIR libs)

set(TESTBED_LIBRARIES 
  nikola
  ${NIKOLA_LIBRARIES}
)

set(TESTBED_INCLUDES 
  ${NIKOLA_INCLUDES}
  ${TESTBED_INCLUDE_DIR}
)

set(TESTBED_TYPE 
  # "engine_testbed"
  "scene_example"
  # "core_testbed"
  # "font_testbed"
  # "audio_testbed"
)
############################################################

### Project Sources ###
############################################################
set(TESTBED_SOURCES 
  ${TESTBED_SRC_DIR}/${TESTBED_TYPE}/main.cpp
  ${TESTBED_SRC_DIR}/${TESTBED_TYPE}/app.cpp

  # Scene example
  ${TESTBED_SRC_DIR}/scene_example/scenes/game_scene.cpp
  ${TESTBED_SRC_DIR}/scene_example/scenes/scene_manager.cpp
)
############################################################

### Final Build ###
############################################################
add_executable(${PROJECT_NAME} ${TESTBED_SOURCES})
############################################################

### Linking ###
############################################################
# Make sure that Nikola is built before attempting to compile the testbeds
add_dependencies(${PROJECT_NAME} nikola)

target_include_directories(${PROJECT_NAME} PUBLIC BEFORE ${TESTBED_INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${TESTBED_LIBRARIES})
############################################################

### Compiling Options ###
############################################################
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_options(${PROJECT_NAME} PUBLIC ${NIKOLA_BUILD_FLAGS})
############################################################
