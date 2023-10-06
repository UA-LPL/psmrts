#!/bin/bash

# Get system type
sysname=`uname -s`

# Based upon platform, install dependencies
if [ "${sysname}" == "Linux" ]; then
  ./vcpkg/vcpkg --triplet=x64-linux install
elif [ "${sysname}" == "Darwin" ]; then
  ./vcpkg/vcpkg --triplet=x64-osx install
else
  ./vcpkg/vcpkg install
fi

