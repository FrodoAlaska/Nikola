#!/bin/bash

# Variables for an easier time
nbr_path="$1/nbr"
res_dir="$2"
out_dir="$3"
list_dir="$4.nbrlist"

./$nbr_path --parent-dir $res_dir --bin-dir $out_dir $list_dir
