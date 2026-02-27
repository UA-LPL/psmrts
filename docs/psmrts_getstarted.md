@page gettingStarted Getting Started

The Planetary Shape Model and Ray Tracing System (PSMRTS) enables the efficient utilization of shape models for applications including planetary mapping, etc, etc.



## Importing PSMRTS into your Project
To add `PSMRS` to your project, `PSMRTS` can be built as a dependency from source in
your CMakeLists.txt project file using CMake's
[FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html)
module. When using this method, you must ensure `PSMRTS` dependencies are
installed and avaliable in your build environment prior to the following import
statements in your CMakeLists.txt file. Here is an example of how to add
`PSMRTS` as a dependency to your project.

```
set(PSMRTS_BUILD_APPS      ON)   # Build PSMRTS applicaions 
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

<h2>Contributors</h2>

PSMRTS is an open source Planetary Shape Model and Ray Tracing System built and supported by the Planetary Science community.

Notable Thanks to the following Contributors to PSMRTS:

  - Kris J. Becker
  - Kyle A. Becker


