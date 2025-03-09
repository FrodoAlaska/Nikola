$nbr_path = "$($args[0])\nbr.exe"
$res_dir  = $($args[1])
$out_dir  = $($args[2])

$textures_out_dir = "$out_dir\textures"
$shader_out_dir   = "$out_dir\shaders"
$cubemap_out_dir  = "$out_dir\cubemaps"
$models_out_dir   = "$out_dir\models"

# Loading textures
& $nbr_path -rt "TEXTURE" -d "$res_dir\textures" "$textures_out_dir"

# Loading cubemaps
& $nbr_path -rt "CUBEMAP" "$res_dir\cubemaps\NightSky" "$cubemap_out_dir"
& $nbr_path -rt "CUBEMAP" "$res_dir\cubemaps\Skybox" "$cubemap_out_dir"

# Loading shaders
& $nbr_path -rt "SHADER" -d "$res_dir\shaders" "$shader_out_dir"

# Loading models
# & $nbr_path -rt "MODEL" "$res_dir\models\cottage_home\cottage_obj.obj" "$models_out_dir"
# & $nbr_path -rt "MODEL" "$res_dir\models\SittingBaby\baby.obj" "$models_out_dir"
# & $nbr_path -rt "MODEL" "$res_dir\models\backpack\backpack.obj" "$models_out_dir"
# & $nbr_path -rt "MODEL" "$res_dir\models\low_poly_building\scene.gltf" "$models_out_dir"
# & $nbr_path -rt "MODEL" "$res_dir\models\behelit\behelit.gltf" "$models_out_dir"
# & $nbr_path -rt "MODEL" "$res_dir\models\medieval_bridge\bridge.gltf" "$models_out_dir"
# & $nbr_path -rt "MODEL" "$res_dir\models\stone_house\house.gltf" "$models_out_dir"
# & $nbr_path -rt "MODEL" "$res_dir\models\Kerner\Karner_C.obj" "$models_out_dir"
# & $nbr_path -rt "MODEL" "$res_dir\models\tempel\tempel.gltf" "$models_out_dir"
# & $nbr_path -rt "MODEL" "$res_dir\models\lady\lady.gltf" "$models_out_dir"
# & $nbr_path -rt "MODEL" "$res_dir\models\lowpoly_house\lowpoly_house.gltf" "$models_out_dir"
# & $nbr_path -rt "MODEL" "$res_dir\models\knight\knight.gltf" "$models_out_dir"

