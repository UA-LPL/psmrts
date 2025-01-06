# Changelog

All changes that impact users of this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

<!---
This document is intended for users of the applications and API. Changes to things
like tests should not be noted in this document.

When updating this file for a PR, add an entry for your change under Unreleased
and one of the following headings:
 - Added - for new features.
 - Changed - for changes in existing functionality.
 - Deprecated - for soon-to-be removed features.
 - Removed - for now removed features.
 - Fixed - for any bug fixes.
 - Security - in case of vulnerabilities.

If the heading does not yet exist under Unreleased, then add it as a 3rd heading,
with three #.


When preparing for a public release candidate add a new 2nd heading, with two #, under
Unreleased with the version number and the release date, in year-month-day
format. Then, add a link for the new version at the bottom of this document and
update the Unreleased link so that it compares against the latest release tag.


When preparing for a bug fix release create a new 2nd heading above the Fixed
heading to indicate that only the bug fixes and security fixes are in the bug fix
release.
-->

## TODO
- Bullet Physics Tracer Model
  - Implement multi-threaded version of tinyobjloader

## [0.0.10] 2025-01-06
- Implemented an initial PSMRTS Request System, with PSMRTS Functor and Process() methods:
  - PRQRayTrace: Basic ray trace with a defined observer and look direction using body-fixed coordinates.
  - PRQPhotometricTrace: Normal ray trace with ovserver and look direction, but including a trace from the sun to the surface intercept.
  - PRQFacet: Retrieves the facet specified by an index facet.
  - Shape Tracers:
    - BulletShapeTracer
    - EllipsoidShapeTracer
    - NaifDskShapeTracer
- CodeCoverage.cmake - Code Coverage updated to exclude test files when outputing results.
- Doxyfile added for Doxygen documentation config.
- PsmrtsOBJImplementation.hpp, PsmrtsPLYImplementation.hpp - removed, functionality changed to PsmrtsOBJFormat.hpp, PsmrtsPLYFormat.hpp.
- PsmrtsPLYFormat.hpp - Removed tinyply implementation, changed to reader utilizing [miniply](https://github.com/vilya/miniply) instead. Adjusted JSON config handling. Added test file, test_PsmrtsPLYFormat.cpp.
- Bennu_Radar.obj, Bennu_Radar.ply, icosahedron.ply, icosahedron_binary.ply - Files added for PLY format testing.
- PsmrtsBufferData.hpp - Minor text formatting fix in validate function.
- PsmrtsMeshData.hpp - Minor code clean up.
- PsmrtsRayTrace.hpp - added validate_lookdir function to handle invalid look direction values.
- PsmrtsRequest.hpp - Added, including ray and photometric trace array PRQ functors.
- PsmrtsUtilities.hpp - Added versioning, microsecond timing, null-check function, and updated JSON config.
- PsmrtsVector3.hpp - Added new template function to convert vector arrays.
- test_PsmrtsStridingBuffer.cpp - Minor checks added to test invalid stride conditions, and improve code coverage.
- test_PsmrtsVector3.cpp - Minor check added to test slice constructor.
- BulletShapeTracer.hpp - Added, implementated with base PRQ functors and PRQRayTraceArray / PRQPhotometricTraceArray processes. Slight edit to include lookdir validation. Added related test file, test_BulletShapeTracer.cpp - edited for correct process checking, THROWS vs FALSE conditions.
- PsmrtsBulletClosestRayCallback.hpp - Changed point() and normal() function returns to vector of nulls in case of no hits. 
- test_BulletTracerModel.cpp - Added.
- EllipsoidShapeTracer.hpp - Added, implemented with base PRQ functors and PRQRayTraceArray / PRQPhotometricTraceArray processes. Added related test file, test_EllipsoidShapeTracer.cpp.
- test_EllipsoidTracerModel.cpp - Small bug fix.
- NaifDskShapeTracer.hpp - Added, implemented with base PRQ functors and PRQRayTraceArray / PRQPhotometricTraceArray processes. Added related test file, test_NaifDskShapeTracer.cpp.
- NaifDskTracerModel.hpp - Bug fixes, correcting various function returns.
- README.md - Included lcov/gcovr instruction note.
- build_psmrts.sh - Added -j parameter to indicate desired number of CPUs used for builds.
- vcpkg.json - Adjusted for PLY changes, tinyply replaced with miniply. 
- Some CmakeLists.txt and related configurations adjusted to reflect files additions and changes. 
- Updated CHANGELOG.md and set version to [0.0.10]

## [0.0.9] 2024-08-01
- The Bullet Physics ray tracing system has been integrated into a PSMRTS tracer model. Lots of refactoring and improvements were also made during this cycle to accomodate the flexible, but complex, striding buffer concept. This provides the BulletTracerModel class PsmrtsTracerModel, the PSMRTS tracer base class of al tracers, which is an abstract base class (ABC).
- Developed a flexible data buffering class hierarchy system to streamline data access and efficient management. Every buffer is shared although deep copies can be made. Arbitrary data types are allocated and managed by the fundamental class PsmrtsBufferData. Next is a type-agnostic mapping of stride size lengths into a PsmrtsBufferData call the PsmrtsStridingBuffer. This the fundamental reference to type data that can be mapped into the PsmrtsBuffer<T> template class. Finally, the PsmrtsVector3<T> class provides generic typing of 3-element vectors of arbitrary types. PSMRTS main data mesh types are PsmrtVector3<int> for indexes, and PsmrtsVector3<double> or PsmrtsVector3<float> vectors.
- The PsmrtsMeshData class serves up a mesh comprised of indexes (or facets) and vectors of double or float. PSMRTS strives for accuracy so the default type for mesh vectors is double.
- Modifed the OBJ format reader to utilize this new mesh structure and optimize if for use in tracers such as Bullet.
- The PsmrtsBulletMeshData is the specialized mesh buffering. Bullet has limit restrictions on the number of facets/part in order to use BVH optimization. This class provides Bullet part partitioning of a PsmrtsMeshData with additional configurations of the system available to PSRMTS users. This is the primary input to the BulletTracerModel.
- Modifined the NaifDsk tracer to utilize this design. The NaifDsk tracer system also provides full extraction the DSK mesh into a PsmrtsMeshData of double or float vectors. The NaifDskTracerModel derived from the PsmrtsTracerModel APC provides the PSMRTS compatible interface.
- Introduced the EllipsoidTracerModel class to provide the full class of ellipsoid models. This includes spheres/spheroids, ellipsoids and trixial ellipsoids.
- This release contains three complete PSMRTS tracer models (BulletTracerModel, NaifDskTracerModel and EllipsoidTracerModel) and support for two mesh/shape formats OBJ (PsrmtsOBJFormat) and DSK (directly from an DskKernelModel).
- Also added a thread safe performance tracking system. This mechanism provides a *performance snapshot*, which contains elapsed time in seconds and milliseconds from creation of the tracker. The snapshot report also contains the born-on date, which is the creation time of the tracker, and the time of the snapshot. Included in this system is a thread-safe counter. This counter is used to track the total number rays of a source tracer (like Bullet) as well as individual tracing metrics per shared copy (this will measure ray traces/#shared ratio). The number of shared copied of meshes, tracers and (combined) mesh shape tracers are tracked by the count of shared instances of each source provided in the API.
- Integrated JSON support using the nlohmann::json library for reporting snapshots. This produces an inheritance hierarchy of data for each element in PSMRTS. This also provides the foundation of data and tracer parameterization.
- Update the PSMRTS version model in CMakeLists.txt (which is used to generate a C++ header with this data).
- Updated CHANGELOG.md and set version to [0.0.9]

## [0.0.8] 2024-06-26
- Big refactor renames several classes
  - The RayTrace class was renamed to PsmrtsRayTrace. Source and test file contents were changed accordingly
  - The PsmrtsOBJAsset class was renamed to PsmrtsOBJFormat. Source and test file contents were changed accordingly
   Some CMakeLists.txt configurations were changed to accomodate renaming of files
- Update CHANGELOG.md and set version to [0.0.8] 

## [0.0.7] 2024-06-26
- PsmrtsPriorityTracer.hpp - Add add_tracer() and clear() methods
- NaifEllipsoidShape.hpp - Add c radius to constructor; Add explicit constructors for sphere, ellipsoid and triaxial ellipsoid; provide shapefile() type;
- tracers/naifdsk/tests/CmakeLists.txt - add new shape tracer (test_NaifShapeTracerAdapter.cpp) and priority shape tracer (test_PsmrtsPriorityTracer.cpp) tests to CMake configuration
- test_RayTrace.cpp - Add RayTrace class tests
- Move the UA/ISIS Bullet code to tracers/bullet/import/src directory
- DskKernelModel.hpp - Make all indexs into facet vector 0-based index references to be consistent/compliant with other implementations
- PsmrtsMeshData.hpp - New class and tests to store index and vertex data of a triangular/facet mesh shape model
- PsmsrtsDataModel.hpp - added buffer segment constructor; added slice() method to produce a segment of the data; new method deep_copy() will create a distinct copy of the data; compute distance() to data origin.
- PsmrtsOBJAsset.hpp - Class that reads Wavefront OBJ files as double precision 3-vector facet/plate shape model data
- test_PsmrtsOBJAsset.cpp - Testing of OBJ reader
- RayTrace.hpp - Add nornal to facet data as it may come from another source
- psmrts_catch2_environment.hpp - Added path to ./psmrts/formats for testing to load data files
- CMakeLists.txt - Add ./psmrts/formats directory and testing updates
- Add Bullet system classes BulletSystemModel.hpp, PsmrtsBulletMeshMap.hpp, PsmrtsBulletAllHitsRayCallback.hpp, PsmrtsBulletWorldModel.hpp and PsmrtsBulletClosestRayCallback.hpp
- Add Bullet Test infrastructure and implement tests
- Add initial implementation to support the PLY format
- Update to version [0.0.7]

## [0.0.6] 2024-06-06
- Migrate PSMRTS BitBucket repo to Github
- Added auto-detection of native arm64-osx build for MacOSX (can set Terminal/Shell in Rosetta mode on ARM platforms to get the x64-osx build)
- Updated documentation URLs for PSMRTS new repo home
- Update minor release version

## [0.0.5] 2024-04-26
- Add code coverage using GCOVR and [CodeCoverage.cmake](https://github.com/bilke/cmake-modules/blob/master/CodeCoverage.cmake).
- Revisted PSMRTS version and added VERSION_DATE ot CMakeList.txt

## [0.0.4] 2024-04-09
- Initial release of PMSRTS/ISIS integration (PR #10)
- psmrts_isis_integration.md - Provides a comprehensive guide to PMSRTS/ISIS
- PsmrtsModelFactory.hpp - The PSMRTS/ISIS shape model loader/initialization
- Numerous code changes while testing PSMRTS/ISIS integration
- Removed splog dependency as it was causing some issues with intergration
- PsmrtsIsisShapeModel.cpp/h - Added PSMRTS shape model tracer to ISIS
- Updates to PSMRTS/ISIS integration code 
- Reduce build dependency requirements until needed
- test_NaifPsmrtsPriorityTracer.cpp, test_NaifShapeTracerAdapter.cpp - Add adapater/tracer tests
- PsmrtsShapeTracerAdapter.hpp - Add generic adapter framework for PSMRTS models
- Adjustments to integrate PSMRTS into the ISIS system
- PsmrtsPriorityTracer.hpp, PsmrtsTracerModel.hpp - Added initial PSMRTS external common C++ API
- PsmrtsUtilities.hpp - Added null(), isnull(), degrees_to_radians(), radians_to_degrees() functions
- DskKernelModel.hpp - Add support for externalizations of tracers/shapes
- RayTrace.hpp - Additional methods adds photometric support
- test_DskKernelModel.cpp - Fixed issues with DSK facet tests and memory bugs; DSK file API test added (PR #9)
- PsmrtsDataModel.hpp - Fix bug in data array allocation to the shared_ptr
- test_DskKernelModel.cpp - Indexes / Vectors errors in test_DskKernelModel, and outputs added to DskKernelModel get_facet
- test_DskKernelModel.cpp - Base ray trace test attempt, and added tracer id, model type and name tests - super basic
- DskKernelModel.hpp - Fix issues with loading DSK content and getting valid metrics
- test_DskKernelModel.cpp - Updated kernel tests/dsk index and vectorm models
- NAIF Kernel management fixes and improvements (PR #8)
- RayTrace.hpp - remove NAIF include dependency
- Don't attempt to reinstall vcpg if it already exists
- Clean CMake configuration
- Move UA/ISIS iNAIF DSK sorces to import/src

## [0.0.3] 2024-02-29
- DskKernelModel.hpp - Add shape/tracer tracking/id methods (PR #7)
- DskKernelModel.hpp - Fixed tracking of new DSK kernel in inventory
- DskKernelModel.hpp - Implement DSK shape model inventory system; DskKernelModel now maintains the lists of active DSKs. New instances are derived from this inventory
- psmrts_catch2_main.cpp - Report correct PSMRTS software version
- NaifUtilities.hpp - Update docmentation and ensure NAIF errors reset
- Add configure include files generated by CMake; Running Catch2 tracer test applications directly were not getting the correct data directory unless it was ran through ctest. This commit fixes file references which can now be generated with absolute paths; Added versioning of the PSMRTS system in a compile time include file.
- DskKernelMode.hpp, DskSegment.hpp - Added documentation
- KernelFileSystem.hpp - Added documentation; added mutex locking for access to the kernel inventory
- test_DskKernelModel.cpp - Modifications and corrections to tests for changes
- DskSegment.hpp - Separates the implementation of this class due to the complex nature of the data. The DskKernelModel now is complete and provides all the nessary methods to safely use a DSK for shared tracing activities.
- KernelFileSystem.hpp - Add robust handling of handle-based kernels; The NAIF file system does not load a kernel more than one time. The additions made here help ensure DSKs, particularly are not closed prematurely. Additional modifications may be required.
- Add RayTrace.hpp class for maintaining ray traces
- PsmrtsUtilities.hpp - Add a mutex wrapper template class for datums
- test_DskKernelModel.cpp - Added robust DSK Basic Load/Init model and segment test
- DskKernelMode.hpp - Use surface id rather than body id for a segment
- Inital version of DSK kernel model code
- KernelFileSystem.hpp - Minor fix to validity test
- Slight adjustment to PSMRTS Make Path test, and negative / zero value testing added to default test in NAIF file (PR #6)
- Use c++17 as the standard
- test_PsmrtsDataModel.cpp - Fix at() references in data calls
- Add validation of radii in NaifEllipsoidShape
- Fix NaifEllipsoidShape source and test. the previous commit of test_NaifEllipsoidShape.cpp was replaced with this fixed version
- Debugged Ray Trace Range test, removed unnecessary boolean check and corrected lkdr to surf - ob
- Slight edit to NaifUtilities(et -> isoc), Ray Trace value range tests, isoc -> et time test, and added naif0012 kernel
- Added test_KernelFileSystem.cpp
- Move psmrts_make_path() to PsmrtsUtitlities.hpp
- Added kernel test to CMakeList, added ray trace test to ellipsoid test, and refining utilites test
- Enhanced setting of NAIF error system
- Add directory access/construction methods
- KernelFileSystem.hpp  - a few doc changes
- NaifEllipsoidShape.hpp - updated error checking.
- test_NaifUtilities.cpp - updated error checking.
- NaifUtilities.hpp - renamed check_for_errors() to check_naif_errors().
- Fix trapping/reporting of NAIF Errors in C++ code
- Add OREX/OCAMS test IK kernel
- NAIF Utilities IK Kernel, NAIF Ellipsoid Max/Min Radius Test, and naifdsk data ocams file 
- Tag Changes in PsmrtsUtilities, and removing unnecessary #define in test_NaifEllipsoidShape (PR #5)
- Develop KernelFileSystem.hpp for the basic NAIF kernel file system API
- Add PSMRTS Longitude Domain testing 360/180, GENERATE Output test, and PSMRTS 360/180 routines
- Add PsmrtsUtilities.hpp containing useful functions
- Add initial version of (NAIF) ellipsoid models with tests
- Add the NAIF SPICE toolkit as the first tracer in PSMRTS

## [0.0.2] 2024-01-11
- Added float test to test_PsmrtsDataModel.cpp
- Few more data index changes for consistency
- Fixed typedefs and cleaned up interface
- Added additional tests for existing data
- Fix mapping of existing data
- Added additional tests
- Add integrate of vcpkg install for efficiency
- Add clean script
- Add instructions to install pkg-config for MacOS
- Fix passing of parameters to build scripts
- Update PSMRTS README
- Fix Catch2 test configuration and initial tests
- Add spdlog and tinyobjloader dependencies
- Container and tests for PSMRTS data
- Build script for PSMRTS
- Onboarding documentation for PSMRTS
- Add options to build tests and extras

## [0.0.1] 2023-10-06
### Initial Version

This is the initial import of the Planetary Shape Model and Ray Tracing System (PSMRTS)
