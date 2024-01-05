#!/bin/bash

export PSMRTS_ROOT=${PWD}
export VCPKG_ROOT=${PWD}/vcpkg

./install_vcpkg.sh
./install_vcpkg_packages.sh
./build_psmrts.sh "$@"