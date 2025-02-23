$nbr_path = "$($args[0])\nbr.exe"
$res_dir  = $($args[1])
$out_dir  = $($args[2])

# & $nbr_path -rt "TEXTURE" -d "$res_dir\textures" "$out_dir\nbr"
#
# & $nbr_path -rt "CUBEMAP" "$res_dir\cubemaps\NightSky" "$out_dir\nbr"
# & $nbr_path -rt "CUBEMAP" "$res_dir\cubemaps\Skybox" "$out_dir\nbr"

# & $nbr_path -rt "SHADER" -d "$res_dir\shaders" "$out_dir\nbr"

# & $nbr_path -rt "MODEL" "$res_dir\models\cottage_home\cottage_obj.obj" "$out_dir\nbr"
& $nbr_path -rt "MODEL" "$res_dir\models\SittingBaby\baby.obj" "$out_dir\nbr"
# & $nbr_path -rt "MODEL" "$res_dir\models\backpack\backpack.obj" "$out_dir\nbr"
# & $nbr_path -rt "MODEL" "$res_dir\models\Kerner\Karner_C.obj" "$out_dir\nbr"
