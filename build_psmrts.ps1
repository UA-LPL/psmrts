param (
    [Switch]$t,
    [Switch]$c,
    [Switch]$x,
    [Switch]$d,
    [int]$j
)

$env:VCPKG_ROOT="$PWD\vcpkg"

$testops = ""
$codecovopts = ""
$extraopts = ""
$buildopts = "-DCMAKE_BUILD_TYPE=Release"
$usepcus = ""

function Show-Usage {
    Write-Host "Usage: build_psmrts.ps1 [-t][-c][-x][-d][-j <number of cpus>]"
    exit 1
}

if ($t) {
    $testopts = "-DBUILD_tESTING=ON"
}

if ($c) {
    $codecovopts = "-DEBUILD_COVERAGE=ON"
    $testopts = "-DEBUILD_TESTING=ON"
}

if ($x) {
    $extraopts = "-DEBUILD_EXTRAS=ON"
}

if ($d) {
    $buildopts = "-DCMAKE_BUILD_TYPE=Debug"
}

if ($j) {
    $usecpus = "-j $j"
}

if (-NOT (Test-Path -Path "build")) {
    New-Item -ItemType Directory -Path "build"
}

# Above commands are placeholders. 
# This is min required for testing purposes:
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DCMAKE_TOOLCHAIN-DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
cmake  --build build