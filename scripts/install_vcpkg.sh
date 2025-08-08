#!/bin/bash

if [ ! -d vcpkg ]; then
  git clone https://github.com/microsoft/vcpkg
else
  git -C vcpkg pull
fi

./vcpkg/bootstrap-vcpkg.sh -disableMetrics
./vcpkg/vcpkg integrate install

export VCPKG_ROOT=$PWD/vcpkg

