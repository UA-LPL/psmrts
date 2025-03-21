#!/usr/bin/env pwsh

$env:PSMRTS_ROOT=$PWD
$env:VCPKG_ROOT="$PWD\vcpkg"

.\install_vcpkg.ps1
.\build_psmrts.ps1