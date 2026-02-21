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
- See [TODO](TODO.md)

## [0.5.1] 2026-02-21
- Work on documentation.
- Fix JSON object insertion for arrays in PsmrtsJson.
- Add psmrts_product_specs applicaton.
- Add test for PsrtsJson.
- Add PSMRTS Developer documentation.
- Remove CONTRIBUTING document for now.
- Update version to v0.5.1.
- Update CHANGELOG.


## [0.5.0] 2026-02-11
- Add string and string array support to C API.
- Add product configuration and creation functions to C API.
- Add explicit void variant class ProductVoidVariant.
- Add PsmrtsTranslations class providing support for environment variable and
  (ISIS mission) parameter file path substitutions.
- Complete support for product invoicing providing a local inventory of
  products shapes, tracers, priority tracers. It is designed to create a
  priority tracer that may have additional tracers added for building complex
  systems of tracers. This is manifest in the classes ProductCart, ProductOrder,
  ProductInvoice.
- Greatly improved handling of JSON structures in PSMRTS.
- Improved DskShape dependency requirements and DSK segment support.
- Improved product configuration/specification processing.
- Improvements and code cleanup of shapes and tracers implementations.
- Moved Product* classes to new directory psmrts/core/products.
- Renamed documentation target from "doxy_docs" to "docs".
- Removed the "isis" directory as it is no longer compatible with the current
  state of PSMRTS.
- Update version to 0.5.0.
- Update CHANGELOG.

## [0.4.1] 2026-02-03
- Add PSMRTS licensing to repo
- Add CC0-1.0 LICENSE file
- Add AUTHORS file
- Modified CONTRIBUTORS.md file
- Update README.md with license information and some edits
- Add copyright/license content to all PSMRTS source files
- Updated version to [0.4.1]
- Updated CHANGELOG.md with previous *and* current version changes
- Update TODO.

## [0.4.0] 2026-01-22

### Fixed/updated ...
- documentation, versioning, typos, formatting
- code coverage, build scripts, CMAKE
- separated public/private code
- export/import of PSMRTS targets & build environments
- privatized PLY reader sources, Ellipsoid Tracer, updated tests & cmakes
- Bullet library, target, tests
- JSON code
- null_uid() method
- PsmrtsUID usage
- bug in ProductOption to return number of elements instead of string size
- PsmrtsCache tests
- vcpkg.json (removed obsolete library & link requirements)
- ISIS cspice
- set library target names of CSPICE for direct use
- shared library linkage & formatting
- MacOS shared library runtime paths
- export targets to support install devs
- Powershell script update to CMake changes
- return null UID
- tests for UID ray tracer identifier
- case sensitivity maps for strings
- find_file call with proper parameters
- cspice target include directories
- mesh extraction in PsmrtShape to return const reference
- Reduce & track sizes of shapes/tracers
- link errors in Linux systems by converting Bullet tracer to object files
- consistent use of const auto references
- OptionStrings
- specs/configs on NAIF DSK tracer/ellipsoid
- refactor options visitor extractor/comparitors
- C API update: autogenerate export header via CMAKE
- shapes/tracers & their product configurations
- renamed visit method get_to() to visitor()
- significant system refactor/update
- bugs in Options classes
- loop counters

### Removed ...
- non-const iterators and test
- spdlog dependency
- vcpkg catch.hpp include
- PsmrtsProduct
- unused key mappers
- references to old visitor functors
- OptionDoubles, OptionStrings
- type visitors from ProductOption.hpp (favoring new Options classes)
- PsmrtsProduct from PsmrtsTracer

### Added ...
- catch2_all.hpp
- vcpkg, conda package manager options
- PSMRTS_POINTERS define
- support for conda & vcpk builds, shared libs
- conda YAML conda environment files
- support for vcpkg triplets
- SPICE include files
- M_PI/M_PI_2 definitions for Windows
- Linux RPATH config
- Added MSVC-sensitive data conversions/warnings to double/int visitors in ProductOption.hpp
- PSMRTS helloworld example
- named/invalid tracer object
- tracer lookup & documentation
- non-const iterator based search method
- method that returns ref to config
- MeshShape with ability to add a generic mesh shape provided by the user; plus config and test.
- null_vector() for consistency
- generic vector container
- support for case insensitive keys in ProductInventory
- conversion utilities for string products
- OptionDoubles, OptionBools, OptionInts classes & conversions
- two PsmrtsContainer tests
- BoolsVisitor, IntegersVisitor, SizetsVisitors & tests
- tinyonbjloader & miniply sources
- ability to create shared or static libraries
- priority ordering technique to priority tracer
- ProductOption const methods
- set_trace() convenience methods supporting reuse of existing PRQ objects
- threading support: mutex thread locking for PSRMTS factory, Improved factory threading/interactions, Improved cache threading in constructors
- complete installation of PSMRTS & PSMRTS C applications
- make cache key unique from UIDType to reduce confusion
- Defined case-key inventory & update tests
- Algorithms basic tests
- changes/prep to DskShape & BulletTracer for ProductConfig implementation
- new compare() method
- additional constructor in ConversionTraits.hpp
- tracer ID to PsmrtsRayTrace & PsmrtsShape

## [0.3.7] 2025-11-04
- Additions/updates to product configurations and tracking of products.
- Update tracers to use private implementation, add construction options, tracking/meta data reporting and tests.
- Add/update numerous tests.
- Add Doxygen documentation to CMake system.

## [0.3.6] 2025-09-15
- Removed unnecessary #include's
- Fixed small number of minor misspellings.

## [0.3.5] 2025-09-10
- Implement product/inventory/caching capabilities
- Moved PsmrtsPriorityTracer.hpp to psmrts/tracers.
- Implement PsmrtsInventory.hpp to house PSMRTS products.
- Added CMake/Catch2/Test directories for shapes, added test_PsmrtsShape.cpp, and added to PLY format testing to improve coverage.
- Added tests for DskSegment and PsmrtsTracer, adjusted related CMake files to include them.
- Add ProductInventory and PsmrtsInventory
  - ProductInventory.hpp - Provides cache of individual PSMRTS products.
  - PsmrtsInventory.hpp  - Maintains caches for PSMRTS keyword/value parameters/options, shapes, tracers, prioritity tracers and “environment” variables.
- Integrate ProductInventory and PsmrtsInventory classes
- Clean up name designations of tracers where all are const references to the actual names of shape files or ellipsoids.
- Set product names accordingly for tracers in constructors.
- PsmrtsCache.hpp - Add thread-safety mutex and locking code.
- ProductInventory.hpp - Implement add() and merge() methods.
- ProductOption.hpp - Add special visitor overload templates and uid().
- PsmrtsProduct.hpp - cleanup
- PsmrtsUtilities.hpp - Privatize PsrmtsUID class to be uninstantiable.
- DskShape.hpp, ObjShape.hpp, PlyShape.hpp, PsmrtsShape.hpp - Improve constructor configuration
- PsmrtsPriorityTracer.hpp - Add ShapeTracer cache/inventory
- Add PsmrtsFactory implementation and initial tests.
- PsmrtsInventory.hpp - Add loading of environment variables on both Linux and Windows.
- Update project version to 0.3.4 in CMakeLists.txt.

## [0.3.4] 2025-09-02
- Converted facet_datum struct to FacetDatum class in PsmrtsRayTrace.hpp.
- Added facet surface_area and volume and mesh surface area, and volume functions to psmrts_c.cpp and defined PSMRTS_Shape opaque pointer to psmrts::PsmrtsShape instead of psmrts::PsmrtsMeshData.
- Modified TEST_CASE_METHOD “PSMRTS C API - One Trace” in test_PsmrtsCAPI.cpp to check facet surface area and volume.
- In  PsmrtsUtilities.hpp added facet_surface_area and facet_volume functions.
- Added TEST_CASE “PSMRTS Facet Geometry Test” to test_PsmrtsUtilities.cpp, testing facet surface area and volume.
- In test_DskShape.cpp added checks to TEST_CASE(“PSMRTS Product DSK Specification Test” to test mesh_surface_area and mesh_volume.
- In test_ObjShape.cpp added checks to TEST_CASE(“PSMRTS Product Obj Specification Test” to test mesh_surface_area and mesh_volume.

## [0.3.3] 2025-09-02
- Renamed PsmrtsParameter to PsmrtsOption as it better fits is usage.
- Rename ProductParameter to ProductFeature to better apply to its usage.
- Rename ProductRequest to ProductConfiguration to better apply to its usage. Add initial implemenation.
- Implemented PsmrtsCache for generic caching of products and other containers.
- Added PsmrtsProduct as the base class of all products and integrated them into PSMRTS product classes.
- Fix C API include issues by using the full path.
- Add PsmrtsUID class to provide unique object IDs for all PSMRTS products in PsmrtsUtilities.hpp.
- Added test_PsmrtsProduct.cpp to CMake file, added get_mesh() visitor function to PsmrtsShape, adjusted shape files to include mesh getters, and created tests for the related shapes changes.
- Remove CMAKE_CATCH_DISCOVER_TESTS_DISCOVERY_MODE from CMakeLists.txt files. The Catch2 directive CMAKE_CATCH_DISCOVER_TESTS_DISCOVERY_MODE was used for some of the Catch2 CMakeList.txt files but not all. Note that this can be set at the command line if needed.
- Freeze VCPKG version baseline due to compliation issues with recent changes in the Eigen port.
- Updated documentation for various methods and files.
- Update CHANGELOG.md

## [0.3.2] 2025-08-22
- Removed/merged obsolete, redundant files/classes/tests to clean up system. 
```
	deleted:    psmrts/core/PsmrtsShapeTracerAdapter.hpp
	deleted:    psmrts/core/PsmrtsTracerModel.hpp
	deleted:    psmrts/core/tests/test_PsmrtsShapeTracerAdapter.cpp
	deleted:    psmrts/tracers/naifdsk/private/NaifDskTracerModel.hpp
	deleted:    psmrts/tracers/naifdsk/private/NaifEllipsoidShape.hpp
	deleted:    psmrts/tracers/naifdsk/tests/test_NaifDskTracerModel.cpp
	deleted:    psmrts/tracers/naifdsk/tests/test_NaifEllipsoidShape.cpp
	deleted:    psmrts/tracers/naifdsk/tests/test_NaifPsmrtsPriorityTracer.cpp
	deleted:    psmrts/tracers/naifdsk/tests/test_NaifShapeTracerAdapter.cpp
	deleted:    psmrts/tracers/naifdsk/tests/test_PsmrtsPriorityTracer.cpp
	deleted:    psmrts/tracers/naifdsk/tests/test_PsmrtsRayTrace.cpp
	deleted:    psmrts/tracers/naifdsk/tests/test_PsmrtsShapeTracerAdapter.cpp
```
- Added the `psmrts/algorithms` directory. Created the TracingBasics.hpp file containing generic ray tracing processes. Using these algorithms will remove/minimize the burden of maintaining common process methods.
- Reworked the PsmrtsParameter.hpp (formerly PsmrtsParameters.hpp) to support PSMRTS intrinsic types for user/dev parameterization support.
- Made progress on the priority tracer.

## [0.3.1] 2025-08-21
- Added facet functionality to PSMRTS C API with supporting methods, tests, and documentation. Also added plate id and segment to C API Facet structure with updated conversion methods and tests.

## [0.3.0] 2025-08-15
- Added facet functionality to C API with tests and documentation.
- Major restructure of PSMRTS directory hierarchy and CMake implementation to simplify future development/maintenance. Includes necessary modifications to include paths, scripts, and tests to ensure successful building, testing, and code coverage across all platforms (Linux, OSX, Windows, etc.).
- PSMRTS version updated to [0.3.0] in CHANGELOG.md, CMakeLists.txt, and cmocka_test.c files.

## [0.2.1] 2025-08-06
- C++ version of C API documentation and testing updated.
- C version of C API cmocka tests and documentation added.

## [0.2.0] 2025-07-22
- Update CHANGELOG.md, CMakeLists.txt to version [0.2.0].
- Renamed formats directory to shapes.
- Simplified, renamed all instances of ShapeTracer to Tracer.
- Changed psmrts_formats_path() to psmrts_shapes_path() in psmrts_catch2_environment.hpp.
- C API documentation updated and included in Doxygen.
- Updated product_specifications.md to reflect relevant changes.


## [0.1.0] 2025-07-16
- Update CHANGELOG.md.
- Update TODO.md which is now tracking development.
- CMakeLists.txt - Updated to version [0.1.0]. 
- Create C API initial implementation in ./capi:
  - Added initial implementations of psmrts_c.h and psmrts_c.cpp.
  - Added ./apps directory and infrastructure: CMakeLists.txt and test directories.
  - Added first implementation of C app psmrts_capi_features.cpp. 
  - Added first implementation of C app psmrts_tracer_shape.cpp.
- product_specification.md - NEW docs file that is the most up-to-date design document we are maintaining.
- Documentation has been improved in a sweep through most all classes.
- test_PsmrtsPLYFormat.cpp - Updated tests to comply with recent changes.
- test_PsmrtsPriorityTracer.hpp - Added new tests using new tracers.
- psmrts/tests/CMakeLists.txt - Added Bullet link libraries to tests.
- DskShape.hpp - PSMRTS process request implementation to read a DSK mesh from a DSK .bds file.
- test_DskShape.cpp - Test of DskShape.
- ObjShape.hpp - PSMRTS process request implementation to read a OBJ mesh from a .obj file.
- test_ObjShape.cpp - Test of ObjShape.
- PlyShape.hpp - PSMRTS process request implementation to read a PLY mesh from a .ply file.
- test_PlyShape.cpp - Test of PlyShape.
- PsmrtsOBJFormat.hpp - Added product specification; create option using specification data.
- ProductParameter.hpp - Updates to support comparisons of config options and specifications.
- ProductSpecification.hpp - Use std::deque instead of std::vector; rework initialization an use of ProductParameters to track keyword and JSON manipulation.
- test_ProductSpecification.cpp - Added test for ProductSpecification class.
- PsmrtsRequest.hpp - Use std::deque rather than std::vector. The std::deque container does not ever reallocate or invalid references when prepending or appending data. This promotes safer pointer references in the C API; Provide/preserve constructors with std::vectors; removed the preliminary/proposed technique to dispatch PRQs to process methods using compiler directives (templates achieve the same things); removed psmrts::traits namespace associated with the obsolete dispatch approach; updated documentation.
- test_PsmrtsRequest.cpp - Removed tests of obsolete process dispatch technique.
- PsmrtsUtilities.hpp - Added latlonrad_d(), xyz_to_latlonrad_d(), string_tokenizer() functions.
- test_PsmrtsUtilities.cpp - Added test for new functions.
- psmrts_version.h.in - Changed template for PSMRTS CMake versioning scheme.
- BulletShapeTracer.hpp - Added product specification for the Bullet tracer.
- test_BulletShapeTracer.cpp - Updated to test new features.
- EllipsoidShapeTracer.hpp - Added product specification for the Ellipsoid tracer.
- test_EllipsoidShapeTracer.cpp - Updated to test new features.
- NaifDskShapeTracer.hpp - Added product specification for the NaifDsk tracer.
- test_NaifDskShapeTracer.cpp - Updated to test new features.

## [0.0.14] 2025-06-30
- PsmrtsDskFormat.hpp - fixed formatting issues for config().
- PsmrtsOBJFormat.hpp - Add comprehensive configuration data and add support for parameters.
- PsmrtsPLYFormat.hpp - Add comprehensive configuration data and add support for parameters; fix propert_type_size() to use proper intrisic types.
- ProductParameter.hpp - Develop product parameter support PSMRTS products.
- ProductProductDispatch.hpp - Develop technique to execute process( PRQ ) functor methods in a generic framework.
- ProductRequest.hpp - Progress in product request mechanisms; updated to handle missing process( PRQ) methods and trap errors when called; added runtime tracking.
- ProductSpecification.hpp - Development of product specs system for describing products by parameterization and tracking configurations.
- PsrmtsProduct.hpp - Variant container template class to manage all PRG functor for related products using a common API.
- PsrmtsShapeTracer.hpp - Develop class to maintain all PSMRTS shape tracers using variant features; add in (likely temporary) methods to construct shape tracers.
- PsrmtsUtilities.hpp - Updated to use high resolution system clock rather than a steady clock; added character string trimming methods; improved how elapsed times are computed.
- BulletShapeTracer.hpp - Added PSMRTS_PROCESS_CATCHALL() marcro to round out process( PRQ ) processes; change virtual declvals to inline (not needed).
- NaifDskShapeTracer.hpp - Added PSMRTS_PROCESS_CATCHALL() marcro to round out process( PRQ ) processes; change virtual declvals to inline (not needed).
- EllipsoidShapeTracer.hpp - Added PSMRTS_PROCESS_CATCHALL() marcro to round out process( PRQ ) processes; change virtual declvals to inline (not needed); removed process( PRQFacet ) use as its obsolete.
- NaifEllipsoidShape.hpp - Fix use of c radius for second radii in constructor of spheroids.
- Renamed PsrmrtsRayTrace::surfpt() to PsmrtsRayTrace::raypt() since the definition of surfpt in NAIF conflicts with this name. raypt() returns the vector from  the observer to the surface point intercept, along the lookdir vector. The length of this vector is the sland distance (km).
- This affected mostly tests including test_PsmrtsRayTrace.cpp, tst_EllipsoidShapeTracer.cpp, test_DskKernelModel.cpp, and test_NaifDskShapeTracer.cpp.
- PsmrtsRequest.hpp - Add distinct tracking of process method invocation status; retain process return status independent of invocation status; clean up process runtime tracking; Set a limit on the maximum number of errors that wil be cached. It implements a FIFO cache with a maximum of 20 errors. After that, the first error in the cache is poped off the queue and the new one is appended.
- PsrmrtsProductDispatch.hpp - Track proper completion status.
- test_PsrmtsRequest.cpp, test_PsmrtsShapeTracer.cpp - updates process tracking and timing.
- vcpkg.json - Removed embree3 as it was causing build errors. 
- Add new and update existing tests to CMake system.
- test_PsmrtsOBJFormat.cpp - Update tests for config and parameters.
- test_PsmrtsPLYFormat.cpp - Update tests for config and parameters.
- test_ProductParameter.cpp - Add tests for ProductParameter class.
- test_ProductSpecification.cpp - Add tests for ProductSpecification class.
- test_PsmrtsProduct.cpp - Add tests for PsmrtsProduct class.
- test_PsmrtsShapeTracer.cpp - Add tests for PsmrtsShapeTracer class; fix failing tests n Windows and Linux.
- test_PsmrtsUtilities.cpp - Update for timing type changes.
- CMakeLists.txt - Update release version.
- CHANGELOG.md - Update for this PR.

## [0.0.13] 2025-03-19
- General improvements to the PSMRTS CMake build system
- Reworked top level build in make_psmrts.sh to not directly install the vcpg dependencies. This essentially eliminates the need for install_vcpkg_packages.sh (CMake does this nicely); use cmake commands for the configure and build instructions.
- Add dsk format to psmrts_format.cmake
- CMakeLists.txt - improved CMake structure to better streamline adding formats and tracers; define PSMRTS dependency and system targets for internal and external use.
- Updated format, tracer and psmrts test CMakeLists.txt files to streamline building/linking PSMRTS tests.
- Updated CHANGELOG.md and set version to [0.0.13]

## [0.0.12] 2025-02-14
- PsmrtsDSKFormat.hpp - Added DSK implementation to formats.
- test_PsmrtsDSKFormat.cpp - Added, includes base DSK format testing. Included related Bennu file to directory for test assistance.
- CMakeLists.txt - Added/adjusted related CMake files to include above dsk tests and
implementation.
- dsk_catch2_main.cpp - Added, DSK format to be included in Catch2 configuration.
- KernelFileSystem.hpp - Small adjustments, includes NAIF error system bugfix.
- Updated CHANGELOG.md and set version to [0.0.12]

## [0.0.11] 2025-02-07
- PR #10 and PR #11 Changes - Documentation and Code Coverage Improvements
- README.md - minor typo fix.
- CMakeLists.txt - Expanded test suite dependencies, improved test directory exclusions in main Cmake file. Added bullet/tinyobjloader library links
and tinyobjloader package to psmrts/tests CMake file. 
- Comment documentation added to: PsmrtsOBJFormat.hpp, PsmrtsPlyFormat.hpp, PsmrtsBufferData.hpp, PsmrtsMeshData.hpp, PsmrtsParameters.hpp, PsmrtsPriorityTracer.hpp,
PsmrtsRayTrace.hpp, PsmrtsShapeTracerAdapter.hpp, PsmrtsStridingBuffer.hpp, PsmrtsUtilities.hpp, PsmrtsVector3.hpp, BulletShapeTracer.hpp, BulletTracerModel.hpp,
PsmrtsBulletMeshMap.hpp, PsmrtsBulletWorldModel.hpp, EllipsoidShapeTracer.hpp, EllipsoidTracerModel.hpp, DskKernelModel.hpp, DskSegment.hpp, NaifDskShapeTracer.hpp,
NaifDskTracerModel.hpp, and NaifEllipsoidShape.hpp.
- test_EllipsoidTracerModel.cpp - Improved clone testing and coverage.
- test_PsmrtsPLYFormat.cpp - Improved json output comparisons for better readability.
- test_PsmrtsPriorityTracer.cpp - Added format testing, improvements.
- test_PsmrtsVector3.cpp - Improved test for coverage and validation catching.
- Updated CHANGELOG.md and set version to [0.0.11]
 
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
- PsmrtsRayTrace.hpp - added validate_lookdir function to handle invalid look direction values.
- PsmrtsRequest.hpp - Added, including ray and photometric trace array PRQ functors.
- PsmrtsUtilities.hpp - Added versioning, microsecond timing, null-check function, and updated JSON config.
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
- Some CmakeLists.txt and related configurations adjusted to reflect files additions and changes. 
- Updated CHANGELOG.md and set version to [0.0.10]

## [PR #8] 2024-11-10
- PsmrtsOBJImplementation.hpp, PsmrtsPLYImplementation.hpp - removed, functionality changed to PsmrtsOBJFormat.hpp, PsmrtsPLYFormat.hpp.
- PsmrtsPLYFormat.hpp - Removed tinyply implementation, changed to reader utilizing [miniply](https://github.com/vilya/miniply) instead. Adjusted JSON config handling. Added test file, test_PsmrtsPLYFormat.cpp.
- Bennu_Radar.obj, Bennu_Radar.ply, icosahedron.ply, icosahedron_binary.ply - Files added for PLY format testing.
- PsmrtsBufferData.hpp - Minor text formatting fix in validate function.
- PsmrtsMeshData.hpp - Minor code clean up.
- PsmrtsVector3.hpp - Added new template function to convert vector arrays.
- vcpkg.json - Adjusted for PLY changes, tinyply replaced with miniply. 

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
