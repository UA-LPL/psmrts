#!/bin/bash

export VCPKG_ROOT=$PWD/vcpkg

testopts=""
codecovopts=""
extraopts=""
buildopts="-DCMAKE_BUILD_TYPE=Release"

while getopts ":htcdx" o; do
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
        d)
            buildopts="-DCMAKE_BUILD_TYPE=Debug"
            ;;
        *)
            echo "Usage: $0 [-t] [-c] [-x]"
            return 1
            ;;
    esac
done
shift $((OPTIND-1))


mkdir -p build
cmake -B build . ${buildopts} ${testopts} ${codecovopts} ${extraopts} -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
make  -C build
