#!/bin/bash

git clone https://github.com/microsoft/vcpkg
./vcpkg/bootstrap-vcpkg.sh -disableMetrics

export VCPKG_ROOT=$PWD/vcpkg

