# **PSMRTS CMAKE Design Objectives/Goals**

  **Date**: February 5, 2025  
  **Author**: Kris J. Becker

### PSMRTS CMAKE Build Manangement System

PSMRTS uses CMake to manage its C++, C API and test code build systems. Along with the management of internal builds, this CMake system will provide three distinct [target EXPORT interfaces](https://cmake.org/cmake/help/latest/guide/importing-exporting/index.html#exporting-targets):

- C++ installation that provides find targets for development (pmsrts::psmsrts).
- C API target that provides a C header (at a minimum) and a static library (psmrts::psmarts_capi)
- Standalone PMSRTS plugin for runtime loading and isolated environment (psmsrts::psmarts_plugin)

#### PSMRTS CMake Objectives and Goals

The objectives and goals of the PSMRTS CMAKE system are provided here. These bullets will also contain certain policy goals that are common in [modern CMAKE](https://cliutils.gitlab.io/modern-cmake/README.html) systems. In general, during development of the PSMRTS CMake system, all best practices of modern CMAKE will be applied that will focus on ease of internal management (i.e., adding/removing products like mesh format readers and ray tracing libraries), external project integration and runtime use.

- Establish a psmrts/cmake directory to populate with CMake scripts, functions and macros.
- A CMakeList.txt file is encouraged for every directory that contains code.
- The top directory of each main PSMRTS feature (e.g., psmrts, tracers, formats) shall have a CMakeLists.txt that maintains its collection of systems.
  - It is in this CMakeLists.txt file where each feature library can be (fully?) integrated into PSMRTS.
- Write a Config.cmake to provide library clients.
- CMake v3.24 as a minimum requirement? (Provides a package finder)
- Do not use IMPORTED keyword - use an ALIAS target [instead](https://cliutils.gitlab.io/modern-cmake/chapters/basics.html).
- Accumulate code coverage Catch2 app names and tests/* sources/directories to exclude from report
- Develop build system around concept of targets.


### Targets and Requirements

This section discusses the main targets within the PSMRTS system and what type of processing and variables are part of each of the major code feature in PSMRTS. Below are some of the targets that are in PSMRTS and thier requirements and characteristics. Here is the current directory structure:
```
./formats
./formats/ply
./formats/ply/tests
./formats/ply/data
./formats/ply/src
./formats/obj
./formats/obj/tests
./formats/obj/data
./formats/obj/src
./cmake
./psmrts
./psmrts/tests
./psmrts/src
./docs
./docs/APEX
./docs/Development
./docs/onboarding
./docs/ISIS
./tracers
./tracers/ellipsoid
./tracers/ellipsoid/tests
./tracers/ellipsoid/src
./tracers/naifdsk
./tracers/naifdsk/tests
./tracers/naifdsk/data
./tracers/naifdsk/src
./tracers/bullet
./tracers/bullet/tests
./tracers/bullet/src
./apps
./isis
./isis/src
./isis/src/base
./isis/src/base/objs
./isis/src/base/objs/PsmrtsIsisShapeModel
```
In this directory structure there are three main areas where PSMRTS development is most prominent: `Formats` (./formats), `Tracers` (./tracers) and the PSMRTS `Main` code section (./psmrts). Within the `Formats` and `Tracers` directories there will be subdirectories containing PSMRTS API interfaces to third party libraries. For example, currently there are is`./formats/ply` developed with the [miniply](https://github.com/vilya/miniply) library and `./formats/obj` develped with the [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader) library. And for `Tracers` there is  `./tracers/bullet`, `./tracers/ellipsoid`, and `./tracers/naifdsk`. These provide interfaces to ray tracing shape models with the [Bullet Physics](https://github.com/bulletphysics/bullet3) library and both `naifdsk` and `ellipsoid` are provided by the [NAIF SPICE Toolkit](https://naif.jpl.nasa.gov/naif/toolkit.html). 

Each new feature library itself may have dependencies, which are all managed by [vcpkg](https://vcpkg.io/en/). The file [vcpkg.json](../../vcpkg.json) is the single file where all libraries and their dependencies are specified/maintained for all of the PSMRTS system. **The installation of vcpkg and ports should be a CMake target in PSMRTS!**

Each of these library sources must have at minimum a CMakeLists.txt configuration in the top directory. This file provide build include and link information via CMake concepts. Also, each PSMRTS library API contains a `./tests` directory that are unique to that feature/datum. This makes adding and, frankly, removing feature libraries easier. **PSMRTS CMake system will provide functions/macros to add a feature library's PROPERTYs and its dependencies easily to the build chain!**

In addition to these directories, the ./docs section needs a CMake target. **PSMRTS will provide a CMake `docs` target to build documentation!**

#### Formats CMake Configuration/Targets
To add a new `Format` (or `Tracer`) support library API to PSMRTS, developers are responsible for intergrating the library and its dependencies into PSMRTS build system developed with CMake. This section outlines the steps and CMake configuration requirements to ingest and install the package in PSMRTS.


