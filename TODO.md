
## PSMRTS TODO

### Product Specs/Configs
- [ ] Implement char * strings transfers b/t C & C++ APIs
- [ ] Implement product resuse/lookup comparisons of exising configurations.
- [ ] Create new products from configs
- [ ] Populate factory with created products

### General Tasks
- [ ] Create PSMRTS Python API. Use [swig](https://www.swig.org) initially. Make this a new repo.
- [ ] Add great circle computation for distances.

### Planetary-based Tasks
- [ ] Create NAIF defined ellipsoids in PSMRTS for NAIF ids and target body names.

### USGS/UA Task list
- Integration/use of PSMRTS in other systems.
  - [ ] Create Python API with SWIG.
- ISIS/community needs for mesh/point clouds and/or tracers.
  - [ ] Add COPC format support. Point clouds coming from the stream in COPC format.
  - [ ] Investigate use of PDAL for this task (concern over the large dependency this will drag into PSMRTS).
  - [ ] Ability to generate a custom mesh on the fly and instantiate a tracer for the mesh.
