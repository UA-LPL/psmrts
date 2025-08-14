#!/bin/bash

export VCPKG_ROOT=$PWD/vcpkg

testopts=""
codecovopts=""
extraopts=""
buildopts="-DCMAKE_BUILD_TYPE=Release"
usecpus=""

while getopts ":htcdxj:" o; do
    case "${o}" in
        t)
            testopts="-DBUILD_TESTS=ON"
            ;;
        c)
            codecovopts="-DBUILD_COVERAGE=ON"
            testopts="-DBUILD_TESTS=ON"

            ;;
        x)
            extraopts="-DBUILD_EXTRAS=ON"
            ;;
        d)
            buildopts="-DCMAKE_BUILD_TYPE=Debug"
            ;;
        j)
            usecpus="-j ${OPTARG}"
            ;;
        *)
            echo "Usage: $0 [-t] [-c] [-x]"
            return 1
            ;;
    esac
done
shift $((OPTIND-1))


cmake  -B build -S . ${buildopts} ${testopts} ${codecovopts} ${extraopts} -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
cmake  --build build ${usecpus}
