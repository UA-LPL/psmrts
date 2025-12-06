#!/bin/bash

testopts=""
codecovopts=""
extraopts=""
buildopts="-DCMAKE_BUILD_TYPE=Release"
usecpus=""
doxyopts=""
sharedopts=""
vcpkg_specs=""
vcpkg_triplet=""
uselocalpsmrtsvcpkg=0
conda_specs=""

while getopts ":hstcdxDVT:Cj:" o; do
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
cmake  -B build -S . ${sharedopts} ${buildopts} ${testopts} ${codecovopts} ${extraopts} ${doxyopts} ${vcpkg_specs} ${vcpkg_triplet} ${conda_specs}
cmake  --build build ${usecpus}
set +x

# Example builds of other targets 
# cmake --install build --prefix install
# cmake --build build   --target coverage
# cmake --build build   --target doxy_docs
