cmake_minimum_required(VERSION 3.27)
project(nbr)

### Project Variables ###
############################################################
set(NBR_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(NBR_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(NBR_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

set(NBR_LIBRARIES ${NIKOLA_LIBRARY_DIR} glfw assimp)
set(NBR_INCLUDES 
  ${NBR_INCLUDE_DIR} 
  ${NBR_LIBS_DIR} 
  ${NBR_LIBS_DIR}/assimp/include
  ${NIKOLA_INCLUDES}
)

set(NBR_OUTPUT_DIR  ${CMAKE_CURRENT_BINARY_DIR})
set(NBR_BUILD_FLAGS ${NIKOLA_BUILD_FLAGS})
set(NBR_BUILD_DEFS  ${NIKOLA_BUILD_DEFS})
############################################################

### CMake Variables ###
############################################################
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)
set(CMAKE_CXX_STANDARD 20)
############################################################

### Project Sources ###
############################################################
set(NBR_SOURCES 
  ${NBR_SRC_DIR}/main.cpp
  ${NBR_SRC_DIR}/nbr_list_lexer.cpp
  ${NBR_SRC_DIR}/nbr_list_parser.cpp
  ${NBR_SRC_DIR}/nbr_list.cpp
  
  ${NBR_SRC_DIR}/image_loader.cpp
  ${NBR_SRC_DIR}/shader_loader.cpp
  ${NBR_SRC_DIR}/model_loader.cpp
)
############################################################

### Library Sources ###
############################################################
set(LIBS_SOURCES 
  ${NBR_LIBS_DIR}/stb/stb_image.cpp
)
############################################################

### Dependencies ###
############################################################
# Setting some options for ASSIMP
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_SAMPLES                  OFF)
set(ASSIMP_BUILD_TESTS                    OFF)
set(ASSIMP_BUILD_ASSIMP_VIEW              OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS             OFF)
set(ASSIMP_NO_EXPORT                      OFF)

set(ASSIMP_BUILD_OBJ_IMPORTER             ON)
set(ASSIMP_BUILD_FBX_IMPORTER             ON)
set(ASSIMP_BUILD_GLTF_IMPORTER            ON)

add_subdirectory(libs/assimp)
############################################################

### Final Build ###
############################################################
add_executable(${PROJECT_NAME} ${NBR_SOURCES} ${LIBS_SOURCES})
############################################################

### Linking ###
############################################################
target_include_directories(${PROJECT_NAME} PUBLIC BEFORE ${NBR_INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${NBR_LIBRARIES})
############################################################

### Compiling Options ###
############################################################
target_compile_options(${PROJECT_NAME} PUBLIC ${NBR_BUILD_FLAGS})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_definitions(${PROJECT_NAME} PUBLIC ${NBR_BUILD_DEFS})
############################################################

### Library Install ###
############################################################
install(TARGETS ${PROJECT_NAME} nbr DESTINATION bin)
############################################################
