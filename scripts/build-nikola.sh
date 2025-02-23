#!/bin/bash

# Variables for an easier time
debug_path="build-debug"
release_path="build-release"

# Options
build_config="Debug"
build_path="$debug_path"
build_flags=""

# Some fun colors
red="\033[0;31m"
blue="\033[0;34m"

### Functions ### 
#########################################################
show_help() {
  echo -e "${red} An easy to use build script to build Nikola on Linux"
  echo -e "${red}   --clean          = Have a new fresh build"              
  echo -e "${red}   --debug          = Build for the debug configuration"   
  echo -e "${red}   --rel            = Build for the release configuration" 
  echo -e "${red}   --jobs [threads] = Threads to use when building"        
  echo -e "${red}   --run-testbed    = Run the testbed examples"            
  echo -e "${red}   --reload-res     = Reload the resources cache"          
  echo -e "${red}   --help           = Display this help message"           
}

create_config_dir() {
  if [[ ! -d ../$build_path ]]; then
    mkdir ../$build_path
  fi
}
#########################################################

### Main loop ###
#########################################################
# No arguments passed
if [[ $# -eq 0 ]]; then 
  echo -e "${red}No arguments passed"
  show_help
  exit
fi

i=1
while [[ $i -le $# ]]; do
  arg="${!i}"

  case "$arg" in  
    -n | --new) 
      build_flags+=" --target clean"
      ;; 
    -d | --debug) 
      build_config="Debug" 
      build_path=$debug_path 
      create_config_dir
      ;;
    -r | --rel) 
      build_config="Release" 
      build_path=$release_path 
      create_config_dir
      ;; 
    -j | --jobs) 
      build_flags+="-j $arg" 
      ((i++))
      ;; 
    -h | --help) 
      show_help 
      exit 
      ;; 
    *) 
      echo -e "${red}Unsupported argument '$arg' passed" 
      show_help 
      exit 
      ;;
  esac

  ((i++))
done
#########################################################

### Build process ###
#########################################################
echo -e "${blue} Building the '$build_config' configuration of Nikola..."
cd ../$build_path
cmake .. -DCMAKE_BUILD_TYPE=$build_config && cmake --build . $build_flags
cd ..
#########################################################
