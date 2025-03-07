$nbr_path = "$($args[0])\nbr.exe"
$res_dir  = $($args[1])
$out_dir  = $($args[2])

# Loading textures
# & $nbr_path -rt "TEXTURE" -d "$res_dir\textures" "$out_dir\nbr"

# Loading cubemaps
# & $nbr_path -rt "CUBEMAP" "$res_dir\cubemaps\NightSky" "$out_dir\nbr"
# & $nbr_path -rt "CUBEMAP" "$res_dir\cubemaps\Skybox" "$out_dir\nbr"

# Loading shaders
& $nbr_path -rt "SHADER" -d "$res_dir\shaders" "$out_dir\nbr"

# Loading models
# & $nbr_path -rt "MODEL" "$res_dir\models\cottage_home\cottage_obj.obj" "$out_dir\nbr"
# & $nbr_path -rt "MODEL" "$res_dir\models\SittingBaby\baby.obj" "$out_dir\nbr"
# & $nbr_path -rt "MODEL" "$res_dir\models\backpack\backpack.obj" "$out_dir\nbr"
# & $nbr_path -rt "MODEL" "$res_dir\models\low_poly_building\scene.gltf" "$out_dir\nbr"
# & $nbr_path -rt "MODEL" "$res_dir\models\behelit\behelit.gltf" "$out_dir\nbr"
# & $nbr_path -rt "MODEL" "$res_dir\models\medieval_bridge\bridge.gltf" "$out_dir\nbr"
# & $nbr_path -rt "MODEL" "$res_dir\models\stone_house\house.gltf" "$out_dir\nbr"
# & $nbr_path -rt "MODEL" "$res_dir\models\Kerner\Karner_C.obj" "$out_dir\nbr"
# & $nbr_path -rt "MODEL" "$res_dir\models\tempel\tempel.gltf" "$out_dir\nbr"
# & $nbr_path -rt "MODEL" "$res_dir\models\lady\lady.gltf" "$out_dir\nbr"
# & $nbr_path -rt "MODEL" "$res_dir\models\lowpoly_house\lowpoly_house.gltf" "$out_dir\nbr"
& $nbr_path -rt "MODEL" "$res_dir\models\knight\knight.gltf" "$out_dir\nbr"

