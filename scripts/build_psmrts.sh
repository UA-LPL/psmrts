#!/bin/bash

export VCPKG_ROOT=$PWD/vcpkg

testopts=""
codecovopts=""
extraopts=""
buildopts="-DCMAKE_BUILD_TYPE=Release"
usecpus=""
doxyopts=""
sharedopts=""

while getopts ":hstcdxDj:" o; do
    case "${o}" in
        s)
            sharedopts="-DBUILD_SHARED=ON"
            ;;    
        t)
            testopts="-DBUILD_TESTING=ON"
            ;;
        c)
            codecovopts="-DBUILD_COVERAGE=ON" #  "-DCODE_COVERAGE_VERBOSE=ON"
            testopts="-DBUILD_TESTING=ON"

            ;;
        x)
            extraopts="-DBUILD_EXTRAS=ON"
            ;;
        d)
            buildopts="-DCMAKE_BUILD_TYPE=Debug"
            ;;
        D)
            doxyopts="-DBUILD_DOCS=ON"
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


cmake  -B build -S . ${sharedopts} ${buildopts} ${testopts} ${codecovopts} ${extraopts} ${doxyopts} -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
cmake  --build build ${usecpus}
#cmake  --install build --prefix install
