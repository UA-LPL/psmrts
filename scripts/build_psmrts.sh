#!/bin/bash


testopts=""
codecovopts=""
extraopts=""
buildopts="-DCMAKE_BUILD_TYPE=Release"
usecpus=""
doxyopts=""
sharedopts=""
vcpkg_specs=""
conda_specs=""

while getopts ":hstcdxDVCj:" o; do
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
        V)
            vcpkg_specs="-DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake"
            export VCPKG_ROOT=$PWD/vcpkg
            ;;
        C)
            conda_specs="-DCMAKE_PREFIX_PATH=${CONDA_PREFIX}"
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


cmake  -B build -S . ${sharedopts} ${buildopts} ${testopts} ${codecovopts} ${extraopts} ${doxyopts} ${vcpkg_specs} ${conda_specs}
cmake  --build build ${usecpus}
#cmake  --install build --prefix install
