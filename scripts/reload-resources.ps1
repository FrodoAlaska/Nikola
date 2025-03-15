$nbr_path = "$($args[0])\nbr.exe"
$res_dir  = $($args[1])
$out_dir  = $($args[2])
$list_dir = "$res_dir\$($args[3]).nbrlist"

# Loading the reosurces
& $nbr_path "--parent-dir" "$res_dir" "--bin-dir" "$out_dir" "$list_dir"
