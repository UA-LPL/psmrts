#!/bin/bash
# vcpkg classic mode install of PSRMTS dependencies

# Get system type
sysname=`uname -s`
archname=`uname -m`

# Based upon platform, install dependencies with appropriate triplet
if [ "${sysname}" == "Linux" ]; then
  export VCPKG_TARGET_TRIPLET=x64-linux
  ./vcpkg/vcpkg --triplet=x64-linux install
elif [ "${sysname}" == "Darwin" ]; then
  if [ "${archname}" == "arm64" ]; then
    export VCPKG_TARGET_TRIPLET=arm64-osx
    ./vcpkg/vcpkg --triplet=arm64-osx install
  else
    export VCPKG_TARGET_TRIPLET=x64-osx
    ./vcpkg/vcpkg --triplet=x64-osx install
  fi
else
  ./vcpkg/vcpkg install
fi

