$working_dir = Get-Location
$debug_dir   = "..\build-debug"
$release_dir = "..\build-release"

$build_dir    = $debug_dir
$build_config = "Debug"
$testbed_name = "NikolaTestbed" 

# No arguments given
if ($Args.Count -le 1) {
  Write-Host "[Usage]: .\run-testbed.ps1 [--debug] [--rel] <testbed_name>" -ForegroundColor yellow
  exit 1
}

switch ($($args[0])) {
  "--debug" { $build_dir = $debug_dir;   $build_config = "Debug" }
  "--rel"   { $build_dir = $release_dir; $build_config = "Release" }
  Default { Write-Host "Invalid argument '$arg' passed" -ForgroundColor red; exit 1}
}

$testbed_name = $($args[1])

Write-Host "Running the '$build_config' configuration of '$testbed_name'..." -ForegroundColor blue
cd $build_dir\testbed
& ".\$build_config\$testbed_name.exe"
cd $working_dir
