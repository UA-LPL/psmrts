
## PSMRTS TODO

### Tasks

- [ ] Remove line15 in the test_PsmrtsShapeTracer.cpp as its not used in the test. 
- [ ] Develop the product registration techniques. We need to discuss and review how from Python (interactive) users, C and C++ developers configure a PSMRTS product request. (Q: Do we allow user requests for a shape only?) How does this fit with existing use such as ISIS?
- [ ] Develop product caching mechanism. I am considering caching to be contained/maintained in each product-base class, such as PsmrtsShapeTracer. This lends itself to decisions made at the product level and cache. (Q: Do we provide for named caching for separation prorposes? ) One cache would serve as a sustem cache for default operations. Multiple maintained caches can better support threading. Now we must invent how to selective caching. Good, bad, ugly?
- [ ] How do we prevent memory leaks? The C interface lends itself to far greater opportunity to have lingering cached products. The Python environment is much worse. We should consider a kill switch - a function call to clear all caches. Note this will have no effect on any existing products because they hold no memory references. It will remove cached versions of earlier products. Note I am not going to cache EllipsoidShapeTracers (sphere and spheroids). This because it takes more resources to cache them than to create then on the fly. We may also not need to cache NaifDskShapeTracers. Opened DSK files are available on demand.

### Planetary-based Tasks
- [ ] Create support for creating NAIF defined ellipsoids in PSMRTS.

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
