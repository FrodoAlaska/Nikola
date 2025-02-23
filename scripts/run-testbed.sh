#!/bin/bash

# Variables for an easier time
debug_path="../build-debug"
release_path="../build-release"
working_dir="./"

# Options
build_config="debug"
build_path="$debug_path"
test_name="testbed"

# Some fun colors
red="\033[0;31m"
blue="\033[0;34m"

### Functions ### 
#########################################################
show_help() {
  echo -e "${red} An easy to use run script to run the testbeds of Nikola"
  echo -e "${red}    --debug  -d: Run the debug configuration of the testbeds"
  echo -e "${red}    --rel    -r: Run the release configuration of the testbeds"
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
    --debug) 
      build_config="debug" 
      build_path=$debug_path 
      ;;
    --rel) 
      build_config="release" 
      build_path=$release_path 
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

### Run process ###
#########################################################

echo -e "${blue} Running $test_name..."
cd $build_path/$test_name

cmake --build . 
./NikolaTestbed

cd $working_dir
#########################################################
