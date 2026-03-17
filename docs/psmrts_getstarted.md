@page gettingStarted Getting Started

The Planetary Shape Model and Ray Tracing System (PSMRTS) enables the efficient utilization of shape models for applications including planetary mapping, etc, etc.

@section install Installation

PSMRTS dependencies are provided by the Microsoft Visual Code package manager, vcpkg. Packages used by PSMRTS are contained in the vcpkg.json file. Currently there are few dependencies and they are generally stable so vcpkg is not included as a submodule or specifically included in the repo. Rather, it is installed during PSMRTS installation and packages are installed/built when the PSMRTS is built. This may create issues in the future so be aware of the potential for vcpkg install/build problems as new dependencies are added and systems evolve. We will attempt to maintain concurrency with vcpkg updates as we continuously build our system.

  - To clone the entire PSMRTS repository: git clone https://github.com/UA-LPL/psmrts.git 
  - To pull a particular branch: git clone -b feature/psmrts-api-first-light https://github.com/UA-LPL/psmrts.git 

@section importing Importing PSMRTS into your Project

To add `PSMRS` to your project, `PSMRTS` can be built as a dependency from source in
your CMakeLists.txt project file using CMake's <a href="https://cmake.org/cmake/help/latest/module/FetchContent.html" target="_blank" rel="noopener">FetchContent</a> module. When using this method, you must ensure `PSMRTS` dependencies are installed and available in your build environment prior to the following import statements in your CMakeLists.txt file. Here is an example of how to add `PSMRTS` as a dependency to your project.

```
set(PSMRTS_BUILD_APPS      ON)   # Build PSMRTS applications 
set(PSMRTS_BUILD_SHARED    ON)   # When using Conda  
FetchContent_Declare(
  psmrts
  GIT_REPOSITORY https://github.com/UA-LPL/psmrts.git
  GIT_TAG        main
 )
FetchContent_MakeAvailable(psmrts)
```
This will download `PSMRTS` from the Github main branch (it is recommended to
use a commit hash here instead of "main" to select a specific version, branch or
tag). It is installed in  the `build/_deps` directory and configured when you
run the CMake configuration for your project. `PSMRTS` will then build
(typically) before your project code and provide the __psmrts::psmrts__ and
__psmrts::psmrts_c__ targets to link to your targets.

`PSMRTS` will also execute its install system when you run _cmake --install build
--prefix installdir_, where _installdir_ is typically CMAKE_INSTALL_PREFIX.



