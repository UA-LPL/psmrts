
## PSMRTS TODO

### Tasks for PSMRTS Restructure PR#24
- [x] Objective is to get the system building with all tests passing on all supported platforms (Linux (Ubuntu 22), MacOS (Intel and Arm) and Windows) under the new system. This goal brings us to the same state before the reorganization.
- [x] Reorganize the project directory hierarchy that requires one include path to headers (an additional path is required for the C API as it is buildable as an option).
- [x] Reimplement a CMAKE system to manage the new structure.
- [x] Devise strategy to support independent testing and code coverage.
  - [ ] Get code coverage actually reporting results.
- [ ] Develop export and packaging options.
- [x] Create private directory structures to support flexibility in how producer's code. For example, some developers can choose to code completely implement code that is not distributed in a binary install.
- [ ] Minimize impact on adding/removing/changing drivers status in PSMRTS.
- [ ] Support builds types of STATIC, SHARED and MODULE.
- [ ] Improve PSMRTS overall ease of use and import/export.

### Tasks
- [ ] Add `PSMRTS` facet request C API functions.
- [ ] Design/implement `psmrts_json` to replace our use/exposure of nlohmann::json in any of `PSMRTS`'s public APIs. This is essentail primarily because if developers should happen to use a different version of nlohmann:json (Anaconda's perhaps), then bad things will happen when they pass an _ordered_json_ object to PSMRTS. nlohmann::json is not ABI compatible from one release to the next. See also [VTKs reasoning](https://discourse.vtk.org/t/nlohmann-json-and-vtks-public-api/15131) on this.
- [ ] Add `explicit` to constructors with just a string and another with just an ordered json object. They conflict. There could be others. Best practice is use explicit when in doubt.
- [ ] Add PRQSpecifications class that collects any number of product specifications.
- [ ] Add PRQForInfo class that returns product specific data in JSON format.
- [ ] Add support for  system-wide keyword cache to support `$tag` substitition (e.g., environment variables or ISIS `$mission` tags)
  - [ ] Implement parsing and substitution for file path name (whilst tracking both original file name and expanded).
- [ ] Finish PsmrtsPriorityTracer classes and infrastructure.
- [ ] Create PSMRTS Python API. Use [swig](https://www.swig.org) initially. Make this a new repo.
- [ ] Major cleanup of CMake, code set. For example, completely eliminate BulltTracerModel.hpp - its not needed.
  - [ ] Consider renaming files to remove/limit `Psmrts` redundancy.
- [ ] Rework PSMRTS include file strategy to use full paths to make external installs, maintanence and usage.
- [ ] Fix CMake to install external include files and a library for export to other installations.
- [ ] Add tawnee::args to main C++ applications that parses input arguments into mains apps.
- [ ] Remove line 15 in the test_PsmrtsShapeTracer.cpp as its not used in the test. 
- [ ] Develop the product registration techniques. We need to discuss and review how from Python (interactive) users, C and C++ developers configure a PSMRTS product request. (Q: Do we allow user requests for a shape only?) How does this fit with existing use such as ISIS?
- [ ] Develop product caching mechanism. I am considering caching to be contained/maintained in each product-base class, such as PsmrtsShapeTracer. This lends itself to decisions made at the product level and cache. (Q: Do we provide for named caching for separation prorposes? ) One cache would serve as a system cache for default operations. Multiple maintained caches can better support threading. Now we must invent how to selective caching. Good, bad, ugly?
- [ ] How do we prevent memory leaks? The C interface lends itself to far greater opportunity to have lingering cached products. The Python environment is much worse. We should consider a kill switch - a function call to clear all caches. Note this will have no effect on any existing products because they hold no memory references. It will remove cached versions of earlier products. Note I am not going to cache EllipsoidShapeTracers (sphere and spheroids). This because it takes more resources to cache them than to create then on the fly. We may also not need to cache NaifDskShapeTracers. Opened DSK files are available on demand.

### Planetary-based Tasks
- [ ] Create NAIF defined ellipsoids in PSMRTS for NAIF ids and target body names.

### Enhancements/Nice-to-haves
- [ ] Add support for environment variables and substitution processing in path strings.
- [ ] Add support for user defined variables that behave just like environment variables. This is specifically to support ISIS-like mission alias used in kernel path management.


### USGS/UA Task list
- ISIS Interfacing with PSMRTS
  - [ ] Need ability to translate environment variables.
- Integration/use of PSMRTS in other systems.
  - [ ] Create Python API with SWIG.
- ISIS/community needs for mesh/point clouds and/or tracers.
  - [ ] Add COPC format support. Point clouds coming from the stream in COPC format.
  - [ ] Investigate use of PDAL for this task (concern over the large dependency this will drag into PSMRTS).
  - [ ] Ability to generate a custom mesh on the fly and instantiate a tracer for the mesh.
