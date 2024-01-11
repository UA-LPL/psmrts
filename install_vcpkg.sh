#!/bin/bash

git clone https://github.com/microsoft/vcpkg
./vcpkg/bootstrap-vcpkg.sh -disableMetrics
./vcpkg/vcpkg integrate install

export VCPKG_ROOT=$PWD/vcpkg

