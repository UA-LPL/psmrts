param (
    [Switch]$t,
    #[Switch]$c,  (Not currently supported for Windows)
    [Switch]$x,
    [Switch]$d,
    [int]$j
)

$env:PSMRTS_ROOT=$PWD
$env:VCPKG_ROOT="$PWD\vcpkg"

$testopts = ""
$codecovopts = ""
$extraopts = ""
$buildopts = "-DCMAKE_BUILD_TYPE=Release"

function Show-Usage {
    Write-Host "Usage: make_psmrts.ps1 [-t][-x][-d][-j <number of cpus>]"
    exit 1
}

.\install_vcpkg.ps1

if ($t) {
    $testopts = "-DBUILD_TESTING=ON"
}

#if ($c) {
#    $codecovopts = "-DEBUILD_COVERAGE=ON"
#    $testopts = "-DEBUILD_TESTING=ON"
#}

if ($x) {
    $extraopts = "-DEBUILD_EXTRAS=ON"
}

if ($d) {
    $buildopts = "-DCMAKE_BUILD_TYPE=Debug"
}


if (-NOT (Test-Path -Path "build")) {
    New-Item -ItemType Directory -Path "build"
}

# Above commands are placeholders. 
# This is min required for testing purposes:
#cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DCMAKE_TOOLCHAIN-DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
cmake -B build -S . $buildopts $testopts $codecovopts $extraopts -DCMAKE_TOOLCHAIN-DCMAKE_TOOLCHAIN_FILE="$PWD\vcpkg\scripts\buildsystems\vcpkg.cmake"
if ($j) {
    Write-Host " ---- CPUs Selected: $j ---- "
    cmake --build build -j $j
}
else {
    cmake --build build
}
#cmake  --build build


#!/usr/bin/env pwsh

#$env:PSMRTS_ROOT=$PWD
#$env:VCPKG_ROOT="$PWD\vcpkg"

#.\install_vcpkg.ps1
#.\build_psmrts.ps1 $args