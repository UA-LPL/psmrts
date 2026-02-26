param(
    [switch]$a,
    [switch]$A,
    [switch]$s,
    [switch]$t,
    [switch]$c,
    [switch]$x,
    [switch]$d,
    [switch]$Doxy,
    [switch]$V,
    [string]$Trip,
    [switch]$Conda,
    [int]$j
)


$sharedopts = ""
$testopts = ""
$codecovopts = ""
$extraopts = ""
$buildapps="-DPSMRTS_BUILD_APPS=ON"
$buildopts = "-DCMAKE_BUILD_TYPE=Release"
$doxyopts = ""
$vcpkg_specs = ""
$vcpkg_triplet = ""
$uselocalpsmrtsvcpkg = 0
$conda_specs = ""



if ($a) { $buildapps = "-DPSMRTS_BUILD_APPS=ON" }
if ($A) { $buildapps = "-DPSMRTS_BUILD_APPS=OFF" }
if ($s) { $sharedopts = "-DPSMRTS_BUILD_SHARED=ON" }
if ($t) { $testopts = "-DPSMRTS_BUILD_TESTS=ON" }
if ($c) { 
    $codecovopts = "-DPSMRTS_BUILD_COVERAGE=ON"
    $testopts = "-DPSMRTS_BUILD_TESTS=ON"
}
if ($x) { $extraopts = "-DPSMRTS_BUILD_EXTRAS=ON" }
if ($d) { $buildopts = "-DCMAKE_BUILD_TYPE=Debug" }
if ($Doxy) { $doxyopts = "-DPSMRTS_BUILD_DOCS=ON" }

# vcpkg logic
if ($V) {
    if ($env:VCPKG_ROOT) {
        $uselocalpsmrtsvcpkg = 0
    }
    else {
        # local vcpkg bootstrap
        $uselocalpsmrtsvcpkg = 1
        $env:VCPKG_ROOT = Join-Path $PWD "vcpkg"
    }
    $vcpkg_specs = "-DCMAKE_TOOLCHAIN_FILE=$($env:VCPKG_ROOT)/scripts/buildsystems/vcpkg.cmake"
}

if ($Trip) {
    $vcpkg_triplet = "-DVCPKG_TARGET_TRIPLET=$Trip"
}

if ($Conda -and $env:CONDA_PREFIX) {
    $conda_specs = "-DCMAKE_PREFIX_PATH=$($env:CONDA_PREFIX)"
}


if ($IsWindows) {
    $buildopts = ""   # Visual Studio ignores CMAKE_BUILD_TYPE
    $single_config_release = @(
        "-DCMAKE_CONFIGURATION_TYPES=Release"
        "-DCMAKE_BUILD_TYPE=Release"   # helps vcpkg
        "-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE=$PWD/Release"
        "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE=$PWD/Release"
        "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE=$PWD/Release"
        "-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG=$PWD/Debug"
        "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG=$PWD/Debug"
        "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG=$PWD/Debug"
    )
}

# Install local vcpkg if needed
if ($uselocalpsmrtsvcpkg -eq 1) {
    Write-Host "Installing local vcpkg instance..."
    & ./scripts/install_vcpkg.ps1
}

# Triplet handling
if ($env:VCPKG_ROOT -and -not $vcpkg_triplet) {
    $sysname = $env:OS
    $arch = (Get-CimInstance Win32_OperatingSystem).OSArchitecture

    # Windows defaults
    if ($arch -match "64") {
        $vcpkg_triplet = "-DVCPKG_TARGET_TRIPLET=x64-windows"
    }
    else {
        $vcpkg_triplet = "-DVCPKG_TARGET_TRIPLET=x86-windows"
    }
}

if (-NOT (Test-Path -Path "build")) {
    New-Item -ItemType Directory -Path "build"
}


$cmake_configure = @(
    "-B", "build"
    "-S", "."
    $sharedopts
    $buildopts
    $testopts
    $codecovopts
    $extraopts
    $buildapps
    $doxyopts
    $vcpkg_specs
    $vcpkg_triplet
    $conda_specs
)

if ($IsWindows) {
    $cmake_configure += $single_config_release
}

# Remove empty strings
$cmake_configure = $cmake_configure | Where-Object { $_ -ne "" }

$cmake_build = @("--build", "build", "--config", "Release")

if ($j) {
    $cmake_build += "--parallel", $j
}

# Run build with crafted parameters
Write-Host "Configuring with CMake..."
cmake @cmake_configure

Write-Host "Building core library first..."
$buildArgs = @("--build", "build", "--config", "Release", "--target", "psmrts")
if ($j) {
    $buildArgs += "--parallel"
    $buildArgs += $j
}
cmake @buildArgs

Write-Host "Building remaining targets (tests, extras)..."
$buildArgs = @("--build", "build", "--config", "Release")
if ($j) {
    $buildArgs += "--parallel"
    $buildArgs += $j
}
cmake @buildArgs



Write-Host "Build complete."

# Examples:
# cmake --install build --prefix install
# cmake --build build --target coverage
# cmake --build build --target lcov_coverage
# cmake --build build --target docs

