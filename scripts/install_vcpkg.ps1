#!/usr/bin/env pwsh

if (-Not (Test-Path -Path "vcpkg" -PathType Container)) {
    git clone https://github.com/microsoft/vcpkg
} else {
    git -C vcpkg pull
}

.\vcpkg\bootstrap-vcpkg.bat -disableMetrics
.\vcpkg\vcpkg integrate install

$env:VCPKG_ROOT="$PWD\vcpkg"
