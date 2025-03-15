### Variables ###

$working_dir = Get-Location
$debug_dir   = "..\build-debug"
$release_dir = "..\build-release"

$build_config = "Debug" 
$build_dir    = $debug_dir 
$build_flags  = ""

$can_run_testbed = $false 
$can_reload_res  = $false

### Variables ###

### Functions ###

function Log-Msg {
  param (
    [string]$msg,
    [string]$log_level 
  )

  $color = "White"
  switch ($log_level) {
    "WARN"  { $color = "Yellow" }
    "INFO"  { $color = "Blue" }
    "ERROR" { $color = "Red" }
    Default {}
  }

  Write-Host $msg -ForegroundColor $color
}

function Show-Help {
  Log-Msg -msg "[Usage]: .\build-nikola.ps1 [options]"                  -log_level "WARN"
  Log-Msg -msg "An easy to use build script to build Nikola on Windows" -log_level "WARN"
  Log-Msg -msg "   --clean          = Have a new fresh build"              -log_level "WARN"
  Log-Msg -msg "   --debug          = Build for the debug configuration"   -log_level "WARN"
  Log-Msg -msg "   --rel            = Build for the release configuration" -log_level "WARN"
  Log-Msg -msg "   --jobs [threads] = Threads to use when building"        -log_level "WARN"
  Log-Msg -msg "   --run-testbed    = Run the testbed examples"            -log_level "WARN"
  Log-Msg -msg "   --reload-res     = Reload the resources in the .nbrlist file" -log_level "WARN"
  Log-Msg -msg "   --help           = Display this help message"           -log_level "WARN"
}

function Check-Build-Dir {
  param (
    [string]$dir
  )

  # The directory exists. Everything is good
  if(Test-Path $dir) {
    return
  }

  # Create the directory otherwise
  mkdir $dir
  $build_dir = $dir
}

function Check-Exit-Code {
  param (
    [string]$msg
  )

  if($LASTEXITCODE -ne 0) {
    Log-Msg -msg "[ERROR]: $msg" -log_level "ERROR" 
    cd $working_dir
    exit $LASTEXITCODE
  }
}

### Functions ###

### Parse the arguments ### 

# No arguments given
if ($Args.Count -le 1) {
  Show-Help
  exit 1
}

for ($i = 0; $i -lt $Args.Count; $i++) {
  $arg = $($args[$i]);
  
  switch ($arg) {
    "--clean"       { $build_flags += "--target clean " }
    "--debug"       { Check-Build-Dir -dir $debug_dir;   $build_config = "Debug" }
    "--rel"         { Check-Build-Dir -dir $release_dir; $build_config = "Release" }
    "--jobs"        { $i++; $build_flags += "--parallel $($args[$i])" }
    "--run-testbed" { $can_run_testbed = $true }
    "--reload-res"  { $can_reload_res  = $true }
    "--help"        { Show-Help; exit 1 }
    Default { 
      Log-Msg -msg "Invalid argument '$arg' passed" -log_level "ERROR"; 
      Show-Help; 
      exit 1
    }
  }
}

### Parse the arguments ### 

### Build ### 

Log-Msg -msg "Building for the '$build_config' configuration..." -log_level "INFO"
cd $build_dir

cmake ..
Check-Exit-Code -msg "Failed to generate CMake files..."

cmake --build . --config $build_config $build_flags 
Check-Exit-Code -msg "Failed to build.."

cd $working_dir

### Build ### 

### Run ### 

if($can_reload_res) {
  .\reload-resources.ps1 "$build_dir\NBR\$build_config" "..\res" "$build_dir\testbed\res" "list_example"
  Check-Exit-Code -msg "Failed to reload resources..."
}

if ($can_run_testbed) {
  .\run-testbed.ps1 "--$($build_config.ToLower())" "NikolaTestbed"
  Check-Exit-Code -msg "Failed to run testbed..."
}

### Run ### 
