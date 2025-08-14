#!/bin/bash

export PSMRTS_ROOT=${PWD}
export VCPKG_ROOT=${PWD}/vcpkg

./scripts/install_vcpkg.sh
./scripts/build_psmrts.sh "$@"
