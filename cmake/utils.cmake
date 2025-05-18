set(NIKOLA_COMMON_DEFS GLFW_INCLUDE_NONE)

if(WIN32)
  ### Definitions ###
  ###############################
  set(NIKOLA_BUILD_DEFS 
    ${NIKOLA_COMMON_DEFS}
    "WIN32"
    "NIKOLA_PLATFORM_WINDOWS"
    "$<$<CONFIG:Debug>:DEBUG; _DEBUG>$<$<CONFIG:Release>:NDEBUG>"
  )
  ###############################
  
  ### Variables ###
  ###############################
  set(STATIC_LIB_EXTENSION "lib")
  set(SHARED_LIB_EXTENSION "dll")
  
  # Thanks, CMake...
  set(NIKOLA_LIBRARY_DIR ${NIKOLA_OUTPUT_DIR}/"$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>"/nikola.${STATIC_LIB_EXTENSION})
  ###############################
  
  ### Build Flags ### 
  ###############################
  set(NIKOLA_BUILD_FLAGS
    "/W0"
  )
  ###############################
elseif(LINUX) 
  if(CMAKE_BUILD_TYPE STREQUAL "Debug") 
    set(BUILD_FLAGS 
      "NIKOLA_BUILD_DEBUG"
      "DEBUG"
      "_DEBUG"
    )
  else()
    set(BUILD_FLAGS 
      "NIKOLA_BUILD_RELEASE"
      "RELEASE"
    )
  endif()

  ### Definitions ###
  ###############################
  set(NIKOLA_BUILD_DEFS 
    ${NIKOLA_COMMON_DEFS}
    "NIKOLA_PLATFORM_LINUX"
    ${BUILD_FLAGS}
  )
  ###############################
  
  ### Variables ###
  ###############################
  set(STATIC_LIB_EXTENSION "a")
  set(SHARED_LIB_EXTENSION "so")
  
  set(NIKOLA_LIBRARY_DIR ${NIKOLA_OUTPUT_DIR}/libnikolad.${STATIC_LIB_EXTENSION})
  ###############################
 
  ### Build Flags ### 
  ###############################
  set(NIKOLA_BUILD_FLAGS
    "-lm" 
    "-w"
  )
  ###############################
endif()
