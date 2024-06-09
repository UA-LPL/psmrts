#!/bin/bash

# Get system type
sysname=`uname -s`
archname=`uname -m`

# Based upon platform, install dependencies
if [ "${sysname}" == "Linux" ]; then
  ./vcpkg/vcpkg --triplet=x64-linux install
elif [ "${sysname}" == "Darwin" ]; then
  if [ "{archname}" == "arm64" ]; then
    ./vcpkg/vcpkg --triplet=arm64-osx install
  else
    ./vcpkg/vcpkg --triplet=x64-osx install
  fi
else
  ./vcpkg/vcpkg install
fi

