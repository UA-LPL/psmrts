
## PSMRTS TODO

### Inventory/Caching, fullfilling product request
- [ ] Develop PSMRTS inventory caching capability
  - [x] Initially support four basic types
    - [x] Shapes
    - [x] Tracers
    - [x] Priority Tracers
    - [x] Generalized Parameters
    - [x] Each `type` needs the following methods (all go in PsmrtsInventory)
      - [x] contains_`type`( UIDType product_id ) const;
      - [x] find_`type`_by_id( UIDType product_id ) const;
      - [x] find_`type`_by_config( PRQProductConfig &config );  - naming is TBD
      - [x] remove_`type`( `type` &product_id );
      - [x] remove_`type`( PsmrtsInventory &`type`_i );
      - [x] add_`type`( `type` &product_id );
      - [x] add_`type`( PsmrtsInventory &`type`_i );
- [ ] Set up product factory with main inventory
  - [x] I see this as a single static source of all PSMRTS product inventories
  - [x] Use PsmrtsInventory to stage product building process and then send residual to Factory for persistent caching
  - [x] Keeps static PSMRTS-wide primary inventory (other uses of PsmrtsInventory can fill product configs)
  - [x] Add caching/inventory requests for each type inventoried
  - [x] Supports local (non-static) inventories to house and transfer cache ownership
- [ ] Finish PsmrtsTracer implementation  
- [ ] Create product configuration with product options used to create/retrieve product (including product_id)
- [ ] Modify PsmrtsSpecification to accept a ProductConfiguration/PRQ that contains user/dev options to construct one or more products
  - [ ] Track all resolved options (retain in each product as its config state)
  - [ ] Return remaining set of parameter/options from product creation in conjuction with feature/option processing above
  - [ ] Any remaining parameters must be retained for further product creation or detect invalid options (that remain)
- [ ] Finish product registration process
  - [ ] Retains `driver` information and aids in product creation/management  
- [ ] Add support for system-wide keyword cache to support `$tag` substitition (e.g., environment variables or ISIS `$mission` tags)
- [ ] Build product options parsing tools as algorithms
- [ ] Implement generic product specification/creation
- [ ] Finish PsmrtsPriorityTracer classes and infrastructure.
- [ ] Retain product options, parameters and specs in the base class PsmrtsProduct
- [x] Add Catch2 tests for DskSegment.hpp.
- [ ] Add Catch2 tests for Bullet ray callback classes.
- [ ] Add Catch2 tests PsmrtsTracer.hpp.
- [x] Add Catch2 tests PsmrtsShape.hpp.
  - [x] Create tests directory and CMakeLists.txt - see psmrts/tracers/.
- [ ] Add Catch2 tests PsmrtsProductDispatch.hpp.
- [ ] Implement C API error handling
- [ ] Implement char * strings transfers b/t C & C++ APIs
- [ ] Add PRQShape, PRQTracer and PRQPriorityTracer classes for C API interfacing


### General Tasks

- [ ] Support builds types of STATIC, SHARED and MODULE.
- [ ] Develop export and packaging options.
- [ ] Design/implement `psmrts_json` to replace our use/exposure of nlohmann::json in any of `PSMRTS`'s public APIs. This is essentail primarily because if developers should happen to use a different version of nlohmann:json (Anaconda's perhaps), then bad things will happen when they pass an _ordered_json_ object to PSMRTS. nlohmann::json is not ABI compatible from one release to the next. See also [VTKs reasoning](https://discourse.vtk.org/t/nlohmann-json-and-vtks-public-api/15131) on this.
- [ ] Add PRQSpecifications class that collects any number of product specifications.
  - [ ] Implement parsing and substitution for file path name (whilst tracking both original file name and expanded).
- [ ] Create PSMRTS Python API. Use [swig](https://www.swig.org) initially. Make this a new repo.
- [ ] Fix CMake to install external include files and a library for export to other installations.
- [ ] Add tawnee::args to main C++ applications that parses input arguments into mains apps.
- [ ] Develop the product registration techniques. We need to discuss and review how from Python (interactive) users, C and C++ developers configure a PSMRTS product request. (Q: Do we allow user requests for a shape only?) How does this fit with existing use such as ISIS?
- [ ] How do we prevent memory leaks? The C interface lends itself to far greater opportunity to have lingering cached products. The Python environment is much worse. We should consider a kill switch - a function call to clear all caches. Note this will have no effect on any existing products because they hold no memory references. It will remove cached versions of earlier products. Note I am not going to cache EllipsoidShapeTracers (sphere and spheroids). This because it takes more resources to cache them than to create then on the fly. We may also not need to cache NaifDskShapeTracers. Opened DSK files are available on demand.
- [ ] Add great circle computation for distances.

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
