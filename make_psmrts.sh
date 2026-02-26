#!/bin/bash

testopts=""
codecovopts=""
extraopts=""
buildopts="-DCMAKE_BUILD_TYPE=Release"
buildapps="-DPSMRTS_BUILD_APPS=ON"
usecpus=""
doxyopts=""
sharedopts=""
vcpkg_specs=""
vcpkg_triplet=""
uselocalpsmrtsvcpkg=0
conda_specs=""

while getopts ":aAhstcdxDVT:Cj:" o; do
    case "${o}" in
        a) 
            buildapps="-DPSMRTS_BUILD_APPS=ON"
            ;;
        A) 
            buildapps="-DPSMRTS_BUILD_APPS=OFF"
            ;;            
        s)
            sharedopts="-DPSMRTS_BUILD_SHARED=ON"
            ;;    
        t)
            testopts="-DPSMRTS_BUILD_TESTS=ON"
            ;;
        c)
            codecovopts="-DPSMRTS_BUILD_COVERAGE=ON" #  "-DCODE_COVERAGE_VERBOSE=ON"
            testopts="-DPSMRTS_BUILD_TESTS=ON"

            ;;
        x)
            extraopts="-DPSMRTS_BUILD_EXTRAS=ON"
            ;;
        d)
            buildopts="-DCMAKE_BUILD_TYPE=Debug"
            ;;
        D)
            doxyopts="-DPSMRTS_BUILD_DOCS=ON"
            ;;
        V)
            # Check for installed vcpkg system
            if [[ -n "${VCPKG_ROOT}" ]]; then
              uselocalpsmrtsvcpkg=0
            else
              # Create a local version of vcpkg for builds
              uselocalpsmrtsvcpkg=1
              export VCPKG_ROOT=${PWD}/vcpkg
            fi            
            vcpkg_specs="-DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            ;;
        T)
            vcpkg_triplet="-DVCPKG_TARGET_TRIPLET=${OPTARG}"
            ;;            
        C)
            conda_specs="-DCMAKE_PREFIX_PATH=${CONDA_PREFIX}"
            sharedopts="-DPSMRTS_BUILD_SHARED=ON"
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

# Install vcpkg if not available in system. Note this build will remain
# in the psmrts directory above ./build until user manually deletes it.
if [[ $uselocalpsmrtsvcpkg -eq 1 ]]; then
  ./scripts/install_vcpkg.sh
fi

# Attempt to set the appropriate triplet if not specified
if [[ -n "${VCPKG_ROOT}" ]]; then
  if [[ -z "${vcpkg_triplet}" ]]; then
    sysname=`uname -s`
    archname=`uname -m` 
    if [[ "${sysname}" == "Linux" ]]; then
      vcpkg_triplet="-DVCPKG_TARGET_TRIPLET=x64-linux"
    elif [[ "${sysname}" == "Darwin" ]]; then
      if [[ "${archname}" == "arm64" ]]; then
        vcpkg_triplet="-DVCPKG_TARGET_TRIPLET=arm64-osx"
      else
        vcpkg_triplet="-DVCPKG_TARGET_TRIPLET=x64-osx"
      fi
    fi
  fi
fi  

# Do the build 
set -x
cmake  -B build -S . ${sharedopts} ${buildopts} ${testopts} ${codecovopts} ${extraopts} ${buildapps} ${doxyopts} ${vcpkg_specs} ${vcpkg_triplet} ${conda_specs}
cmake  --build build ${usecpus}
set +x

# Example builds of other targets 
# cmake --install build --prefix install
# cmake --build build   --target coverage
# cmake --build build   --target lcov_coverage
# cmake --build build   --target docs
