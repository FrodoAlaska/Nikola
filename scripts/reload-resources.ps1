# Not enough arguments given
if ($Args.Count -le 2) {
  Write-Host "[Usage]: .\reload-resources.ps1 <nbr_path> <resources_dir>" -ForgroundColor yellow
  exit 1
}

$res_dir  = $($args[0])
$nbr_path = $($args[1]) 

$nbr_path -rt TEXTURE -d "$res_dir\textures" "$res_dir\nbr"

$nbr_path -rt CUBEMAP "$res_dir\cubemaps\NightSky" "$res_dir\nbr"
$nbr_path -rt CUBEMAP "$res_dir\cubemaps\Skybox" "$res_dir\nbr"

$nbr_path -rt SHADER -d "$res_dir\shaders" "$res_dir\nbr"
