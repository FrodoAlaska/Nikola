### Project Sources ###
############################################################
set(NIKOLA_SOURCES 
  # Core/Base
  ${NIKOLA_SRC_DIR}/core/base/nikola.cpp
  ${NIKOLA_SRC_DIR}/core/base/logger.cpp
  ${NIKOLA_SRC_DIR}/core/base/event.cpp
  ${NIKOLA_SRC_DIR}/core/base/window.cpp
  ${NIKOLA_SRC_DIR}/core/base/nikola_memory.cpp
  ${NIKOLA_SRC_DIR}/core/base/input.cpp
  ${NIKOLA_SRC_DIR}/core/base/nikola_clock.cpp
  
  # Core/Gfx
  ${NIKOLA_SRC_DIR}/core/gfx/gl_backend.cpp
  
  # Engine
  ${NIKOLA_SRC_DIR}/engine/engine.cpp

  # Engine/Math 
  ${NIKOLA_SRC_DIR}/engine/math/math_common.cpp
  ${NIKOLA_SRC_DIR}/engine/math/random.cpp
  ${NIKOLA_SRC_DIR}/engine/math/vector_types.cpp
  ${NIKOLA_SRC_DIR}/engine/math/matrix_types.cpp
  ${NIKOLA_SRC_DIR}/engine/math/quaternion.cpp
  ${NIKOLA_SRC_DIR}/engine/math/transform.cpp
  ${NIKOLA_SRC_DIR}/engine/math/vertex.cpp
  
  # Engine/Resources
  ${NIKOLA_SRC_DIR}/engine/resources/resource_manager.cpp
  ${NIKOLA_SRC_DIR}/engine/resources/material.cpp
  ${NIKOLA_SRC_DIR}/engine/resources/shader_context.cpp
  ${NIKOLA_SRC_DIR}/engine/resources/nbr_file.cpp
  ${NIKOLA_SRC_DIR}/engine/resources/nbr_importer.cpp

  # Engine/filesystem
  ${NIKOLA_SRC_DIR}/engine/filesystem/file.cpp
  ${NIKOLA_SRC_DIR}/engine/filesystem/filesystem.cpp
  ${NIKOLA_SRC_DIR}/engine/filesystem/filepath.cpp
  ${NIKOLA_SRC_DIR}/engine/filesystem/filewatcher.cpp

  # Engine/Resources/Loaders 
  ${NIKOLA_SRC_DIR}/engine/resources/loaders/mesh_loader.cpp
  ${NIKOLA_SRC_DIR}/engine/resources/loaders/skybox_loader.cpp
  
  # Engine/Renderer 
  ${NIKOLA_SRC_DIR}/engine/renderer/camera.cpp
  ${NIKOLA_SRC_DIR}/engine/renderer/renderer.cpp
  ${NIKOLA_SRC_DIR}/engine/renderer/batch_renderer.cpp
  
  # UI 
  ${NIKOLA_SRC_DIR}/ui/gui.cpp
)

set(NIKOLA_HEADERS
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_base.h
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_gfx.h
  
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_app.h
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_containers.h
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_file.h
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_math.h
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_render.h
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_resources.h
  
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_ui.h
  
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola.h
)
############################################################

### Library Sources ###
############################################################
set(LIBS_SOURCES 
  # GLAD
  ${NIKOLA_LIBS_DIR}/glad/glad.c
  
  # ImGui
  ${NIKOLA_LIBS_DIR}/imgui/backends/imgui_impl_dx11.cpp
  ${NIKOLA_LIBS_DIR}/imgui/backends/imgui_impl_dx11.h
  ${NIKOLA_LIBS_DIR}/imgui/backends/imgui_impl_glfw.cpp
  ${NIKOLA_LIBS_DIR}/imgui/backends/imgui_impl_glfw.h
  ${NIKOLA_LIBS_DIR}/imgui/backends/imgui_impl_opengl3.cpp
  ${NIKOLA_LIBS_DIR}/imgui/backends/imgui_impl_opengl3.h
  ${NIKOLA_LIBS_DIR}/imgui/backends/imgui_impl_opengl3_loader.h
  ${NIKOLA_LIBS_DIR}/imgui/imconfig.h
  ${NIKOLA_LIBS_DIR}/imgui/imgui.cpp
  ${NIKOLA_LIBS_DIR}/imgui/imgui.h
  ${NIKOLA_LIBS_DIR}/imgui/imgui_demo.cpp
  ${NIKOLA_LIBS_DIR}/imgui/imgui_draw.cpp
  ${NIKOLA_LIBS_DIR}/imgui/imgui_internal.h
  ${NIKOLA_LIBS_DIR}/imgui/imgui_stdlib.cpp
  ${NIKOLA_LIBS_DIR}/imgui/imgui_stdlib.h
  ${NIKOLA_LIBS_DIR}/imgui/imgui_tables.cpp
  ${NIKOLA_LIBS_DIR}/imgui/imgui_widgets.cpp
  ${NIKOLA_LIBS_DIR}/imgui/imstb_rectpack.h
  ${NIKOLA_LIBS_DIR}/imgui/imstb_textedit.h
  ${NIKOLA_LIBS_DIR}/imgui/imstb_truetype.h

  # Miniaudio 
  ${NIKOLA_LIBS_DIR}/miniaudio/miniaudio.c
)
############################################################

### Final Build ###
############################################################
add_library(${PROJECT_NAME} ${NIKOLA_BUILD_TYPE} ${NIKOLA_SOURCES} ${NIKOLA_HEADERS} ${LIBS_SOURCES})
############################################################

### Linking ###
############################################################
target_include_directories(${PROJECT_NAME} PUBLIC BEFORE ${NIKOLA_INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${NIKOLA_LIBRARIES})
############################################################

### Compiling Options ###
############################################################
target_compile_options(${PROJECT_NAME} PUBLIC ${NIKOLA_BUILD_FLAGS})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_definitions(${PROJECT_NAME} PUBLIC ${NIKOLA_BUILD_DEFS})
############################################################
