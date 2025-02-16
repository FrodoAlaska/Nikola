#!/bin/bash

# Variables for an easier time
debug_path="../build-debug"
release_path="../build-release"

# Some fun colors
red="\033[0;31m"
blue="\033[0;34m"
white="\033[0m"

if [[ -d $debug_path ]]; then
  cd $debug_path
elif [[ -d $release_path ]]; then
  cd $release_path
else 
  echo -e "${red}Make sure to build Nikola first..."
  exit
fi

echo -e "${blue}Building NBR..."
cmake --build . 

echo -e "${blue}Running NBR with the passed arguments... ${white}"
./NBR/nbr $@

echo -e "\n${blue}Back to the beginning..."
cd ../../
