### Project Sources ###
############################################################
set(NIKOLA_SOURCES 
  # Base
  ${NIKOLA_SRC_DIR}/base/nikola.cpp
  ${NIKOLA_SRC_DIR}/base/logger.cpp
  ${NIKOLA_SRC_DIR}/base/event.cpp
  ${NIKOLA_SRC_DIR}/base/window.cpp
  ${NIKOLA_SRC_DIR}/base/nikola_memory.cpp
  ${NIKOLA_SRC_DIR}/base/input.cpp
  ${NIKOLA_SRC_DIR}/base/nikola_clock.cpp
  
  # Gfx
  ${NIKOLA_SRC_DIR}/gfx/gl_backend.cpp
  
  # Engine
  ${NIKOLA_SRC_DIR}/engine.cpp

  # Math 
  ${NIKOLA_SRC_DIR}/math/math_common.cpp
  ${NIKOLA_SRC_DIR}/math/random.cpp
  ${NIKOLA_SRC_DIR}/math/vector_types.cpp
  ${NIKOLA_SRC_DIR}/math/matrix_types.cpp
  ${NIKOLA_SRC_DIR}/math/quaternion.cpp
  ${NIKOLA_SRC_DIR}/math/transform.cpp
  ${NIKOLA_SRC_DIR}/math/vertex.cpp
  
  # Physics 
  ${NIKOLA_SRC_DIR}/physics/physics.cpp
  
  # Resources
  ${NIKOLA_SRC_DIR}/resources/resource_manager.cpp
  ${NIKOLA_SRC_DIR}/resources/material.cpp
  ${NIKOLA_SRC_DIR}/resources/shader_context.cpp
  ${NIKOLA_SRC_DIR}/resources/nbr_file.cpp
  ${NIKOLA_SRC_DIR}/resources/nbr_importer.cpp

  # Resources/Loaders 
  ${NIKOLA_SRC_DIR}/resources/loaders/geometry_loader.cpp

  # Filesystem
  ${NIKOLA_SRC_DIR}/filesystem/file.cpp
  ${NIKOLA_SRC_DIR}/filesystem/filesystem.cpp
  ${NIKOLA_SRC_DIR}/filesystem/filepath.cpp
  ${NIKOLA_SRC_DIR}/filesystem/filewatcher.cpp
  
  # Timer 
  ${NIKOLA_SRC_DIR}/time/timer_utils.cpp
  
  # Renderer 
  ${NIKOLA_SRC_DIR}/renderer/camera.cpp
  ${NIKOLA_SRC_DIR}/renderer/renderer.cpp
  ${NIKOLA_SRC_DIR}/renderer/batch_renderer.cpp
  
  # Audio 
  ${NIKOLA_SRC_DIR}/audio/audio_openal.cpp
  
  # UI 
  ${NIKOLA_SRC_DIR}/ui/gui.cpp
  ${NIKOLA_SRC_DIR}/ui/ui_text.cpp
  ${NIKOLA_SRC_DIR}/ui/ui_button.cpp
  ${NIKOLA_SRC_DIR}/ui/ui_checkbox.cpp
  ${NIKOLA_SRC_DIR}/ui/ui_slider.cpp
  ${NIKOLA_SRC_DIR}/ui/ui_layout.cpp
)

set(NIKOLA_HEADERS
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_base.h
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_gfx.h
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_event.h
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_window.h
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_input.h
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_app.h
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_containers.h
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_file.h
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_math.h
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_render.h
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_resources.h
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_ui.h
  
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola.h
)

set(NIKOLA_PCH
  ${NIKOLA_INCLUDE_DIR}/nikola/nikola_pch.h
)
############################################################

### Library Sources ###
############################################################
set(LIBS_SOURCES 
  # GLAD
  ${NIKOLA_LIBS_DIR}/glad/glad.c
  
  # ImGui
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
)
############################################################

### Final Build ###
############################################################
add_library(${PROJECT_NAME} ${NIKOLA_BUILD_TYPE} ${NIKOLA_SOURCES} ${NIKOLA_HEADERS} ${LIBS_SOURCES})
############################################################

### Linking ###
############################################################
target_include_directories(${PROJECT_NAME} PRIVATE BEFORE 
  ${NIKOLA_INCLUDES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
  ${NIKOLA_LIBRARIES}
)

target_precompile_headers(${PROJECT_NAME} PRIVATE 
  "$<$<COMPILE_LANGUAGE:CXX>:${NIKOLA_PCH}>"
)
############################################################

### Compiling Options ###
############################################################
target_compile_options(${PROJECT_NAME} PUBLIC ${NIKOLA_BUILD_FLAGS})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_definitions(${PROJECT_NAME} PUBLIC ${NIKOLA_BUILD_DEFS})
############################################################
