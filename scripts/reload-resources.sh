#!/bin/bash

# Variables for an easier time
nbr_path="$1/nbr"
res_dir="$2"
out_dir="$3"

./$nbr_path -rt "TEXTURES" -d "$res_dir/shaders" "$out_dir/nbr"
