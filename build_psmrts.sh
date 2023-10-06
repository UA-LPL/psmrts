#!/bin/bash

export VCPKG_ROOT=$PWD/vcpkg

testopts=""
codecovopts=""
extraopts=""

while getopts ":htcx" o; do
    case "${o}" in
        t)
            testopts="-DBUILD_TESTING=ON"
            ;;
        c)
            codecovopts="-DBUILD_COVERAGE=ON"
            testopts="-DBUILD_TESTING=ON"

            ;;
        x)
            extraopts="-DBUILD_EXTRAS=ON"
            ;;
        *)
            echo "Usage: $0 [-t]"
            return 1
            ;;
    esac
done
shift $((OPTIND-1))


mkdir -p build
cmake -B build . -DCMAKE_BUILD_TYPE=Release ${testopts} ${codecovopts} ${extraopts} -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
make  -C build
