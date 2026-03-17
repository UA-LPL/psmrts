@page designProductSpecs Product Specifications
@tableofcontents

@section designProductConcept PSMRTS Product Concept

Priority tracers contain more than one tracer where ray tracing operations can be specialized (e.g., prioritized, nearest-observer) and customized to suit. The configuration of a priority tracer is a merge of all the configurations of `PSMRTS` products.

@section designProductDescription PSMRTS Product Description

Each `PSMRTS` product, such as a `mesh`, `tracer`, must have a description that defines the product API specification and software driver information. This specification will be implemented in s/w as a JSON structure. Our objective is to design a product description that considers the following requirements, functionality and flexibility:
- Products must have a unique _name_. For example, `obj`, `ply`, `dsk` (`bds`) for meshes and `bullet`, `naifdsk`, `ellipsoid` (`sphere`, `spheroid`) as tracers. **NOTE: This will also be the default of the software driver name that creates these products.**
- Products must define its _type_. Current options are: `mesh`, `tracer` (`shapetracer`) and `prioritytracer`.
- Add _description_ so parameters/drivers can be self documenting/described.
- As implied above, parameters and options can have (`aliases`). This will allow the same driver to serve more than one product and minimize interfacing complexity and optimize product mapping/lookup strategies.
- Product (_driver_) keyword options are crafted after [GDAL options](https://gdal.org/en/stable/user/configoptions.html) specifications (however, `PSMRTS` keywords are lowercase). `PSMRTS` JSON options keyword names should be preceded by the driver name and an underscore. For example, users can specify `obj_file="g_00880mm_alt_ptm_0000n00000_v020.obj"` and the `obj` driver will be selected to provide the mesh given in the `obj_file` parameter.
- **Characteristics of Product Parameters/Keywords**
  - `PSMRTS` will need/require a product description/specification, much like what is found in web-based shopping product profiles.
  - Parameters should also contain a short _description_ parameter
  - The product specification will contain a list/set of _parameters_. 
  - Parameters can be `required` or `optional` (options). If their _status_ is not specified, the default will be **required**.
  - Parameters can contain a list of _aliases_ that are synonymous with the parameter _name_. For example, `file` can be aliased with `obj_file` whereas the suffix of `file` will be checked for an `obj` extension and satisfy a user request if present.
  - Parameters may contain a list of _valid_ values that are checked when creating a product from a user config.
  - Parameters may specify the _default_ value to use for the keyword.
  - Parameters may have to specify type and count of parameters (ex. ellipsoid with 3 radii values: `ellipsoid_radii=[0.245,0.284,0250]`)
- This configuration is a critical part of the registration procedure that adds drivers and functionality to the `PSMRTS` system.
- Drivers are constructed for each product and registered in a lookup table for the driver name. The table must be iterable as well to support match operations to determine if a user config matches a product specification, which would typically result in a `create()` call.

@section designProductSpecClass PSMRTS Product ProductSpecification Class

The `ProductSpecification` class defines the format for product specification. This class will maintain a product name and type along with parameterization specifications and driver information. Each product driver must provide the details shown below and subsequently discussed in the following example. This static method is required in each `PSMRTS` product implementation.

@section designShapeSpec PSMRTS Shape Specification

The example shown here is the specification for the `tinyobjloader` OBJ reader. It also reads materials. Additional functionality can be added to support reading/retaining the materials structure and provide an object functor interface to get access to the materials data read by `tinyobjloader`. We are currently recommending this specification belongs in the product process class, such as ObjShape.hpp.

```
      static inline ProductSpecification product_specifications() {
        char text[] = R"(
        {
          "name": "obj",
          "product": "shape",
          "type": "mesh"
          "description": "Reads Wavefront OBJ mesh files and creates a PMRTS mesh object",
          "driver": {
            "name": "obj",
            "type": "system",
            "aliases": [ "OBJ" ]
          }
          "parameters": [
            {
              "name": "obj_file",
              "type": "file",
              "description": "Name of OBJ file to read",
              "status": "required",
              "aliases": ["file", "obj_mesh", "mesh_file"],
              "file_suffixes": [ "obj", "OBJ" ]
            },
            {
              "name": "obj_string",
              "type": "string",
              "description": "Format-compatible string containing contents of an OBJ file",
              "status": "optional",
              "aliases": ["obj_mesh_string"],
            },            
            {
              "name": "obj_data_type",
              "type": "string",
              "description": "Type of mesh vector data requested/read",
              "status": "optional",
              "aliases": ["mesh_data_type"],
              "valid": ["double","float"],
              "default": "double"
            },         
            {
              "name": "obj_mtl_search_path",
              "type": "directory",
              "description": "Directory path to OBJ materials files",
              "status": "optional",
              "aliases": ["obj_materials_dir"]
            }   
          ]       
        } )";

        // This validates the JSON structure and provides product info to callers
        return ( ProductSpecification( "obj", "mesh", json_utils::parse_json_string( text )));
      }
```

Note this approach also provides some interesting possibilitites. Developers can add their own parameters that could be used to turn on debugging, logging and I/O options for analysis/debugging. We should anticipate a need for global `psmrts` parameter options (ex: `psmrts_maximum_threads=0`) that drivers could recognize and apply within their scope. Note that special types like `file` can include methods that check the _value_ of the type for a _file_suffixes_ to satisfy its required file extension. Note that most string values, excluding `file` and `directory`, are converted to lower case (such as JSON keyword names should all be lowercase - enforced in the `ProductParameter` class).

@section designTracerSpec PSMRTS Tracer Specification

The Bullet system configuration provides users the abilility to use a bounding volume hierarchy, use compression and apply measures to ensure single thread safety when running a ray trace. Here is the prelimimary specfication for Bullet:
```
      static inline ProductSpecification product_specifications() {
        char text[] = R"(
        {
          "name": "bullet",
          "product": "tracer",
          "type": "tracer"
          "description": "The Bullet Physics ray tracing system specification",
          "driver": {
            "name": "bullet",
            "type": "system"
          }
          "parameters": [
            {
              "name": "bullet_optimize_bvh",
              "type": "bool",
              "description": "Use optimized bounding volume hierachy (BVH) when created",
              "status": "optional",
              "default": "false",
              "valid": ["true", "1", "yes", "false", "0", "no"]
            },
            {
              "name": "bullet_compressed",
              "type": "bool",
              "description": "Compress Bullet data during construction",
              "status": "optional",
              "default": "false",
              "valid": ["true", "1", "yes", "false", "0", "no"]
            },            
            {
              "name": "bullet_thread_safety",
              "type": "bool",
              "description": "Utilize thread locking before Bullet ray traces are run",
              "status": "optional",
              "default": "false",
              "valid": ["true", "1", "yes", "false", "0", "no"]
            }
          ]       
        } )";

        // This validates the JSON structure and provides product info to callers
        return ( ProductSpecification( "obj", "mesh", json_utils::parse_json_string( text )));
      }
```

@section designProductRequest ProductRequest Class
The `ProductRequest` class maintains a configuration for a particular product. `ProductRequest` objects originate ultimately from users, but developers produce the class instances. This class should interact directly with the `ProductSpecification` class to determine if the driver has all the required parameters and proper values for options in the product configuration. I am thinking this class can inherit the `ProductParameter` class and the ability to interogate values is added in methods here. Search capabilties on, at least, parameter names in both `ProductSpecification` and `ProductRequest`.

An instance of a `ProductRequest` is intended to be a simple, linear set of keywords that define all the products needed along with customized parameters to configure a product, ultimately a `shapetracer`. This design needs to support at a minimum our three defined types: `mesh` (formats), `tracer` (`shapetracer`) and `prioritytracer`. The idea here is that each product will be checked for what is needed and calls are made in the `ProductManufacturing` class starting with tracer calls and if tracers need a mesh, it will request a mesh product from its contents. Note that validation of the configuration should be validated with JSON diff() function on the remaining parameters after each product request is fullfilled. A successful configuration occurs when no JSON keys remain after processing a product request in a `ProductRequest`.

@section designTracerConfig Tracer Configurations

This is an example showing how to a Bullet tracer with a DSK file is requested and its configured status:

```
char bullet_config[] = R"(
{
  "name": "g_00880mm_alt_ptm_0000n00000_v020.obj",
  "type": "tracer",
  "tracer": "bullet",
  "bullet_optimizedbvh": false,
  "format": "obj",
  "file": "g_00880mm_alt_ptm_0000n00000_v020.obj"
} )";

return ( ProductRequest( json_utils::parse_json_string( bullet_config )));
```

Thus configuration can be minimized to the following:
```
char minimal_bullet_config[] = R"(
{
  "tracer": "bullet",
  "obj_file": "g_00880mm_alt_ptm_0000n00000_v020.obj"
} )";
```
Configure a DSK shape tracer with segment with a specific NAIF identifier:
```
char dsk_config[] = R"(
{
  "tracer": "naifdsk",
  "dsk_file": "g_00880mm_alt_ptm_0000n00000_v020.bds",
  "dsk_surface_id": 20001
} )";

// Alternative C++ construction
return ( ProductConfiguration( {  
                                  ProductParameter( "tracer", "naifdsk"), 
                                  ProductParameter( "dsk_file", "g_00880mm_alt_ptm_0000n00000_v020.bds"), 
                                  ProductParameter( "dsk_surface_id", 20001) 
                               }
                             ) 
  );
```

Configure a Bullet shape tracer with a DSK mesh from a specific segment number:
```
char bullet_dsk_config[] = R"(
{
  "tracer": "bullet",
  "dsk_file": "g_00880mm_alt_ptm_0000n00000_v020.bds",
  "dsk_segment_number": 0
} )";
```

Configure a couple of ellipsoids:
```
char ellipsoid_config[] = R"(
{
  "tracer": "ellipsoid",
  "radii": [0.245,0.287,0.250]
} )";

char spheroid_config[] = R"(
{
  "tracer": "spheroid",
  "radii": [0.245,0.287]
} )";

char sphere_config[] = R"(
{
  "tracer": "sphere",
  "radius": 0.250
} )";
```

@section designPriorityTracerConfig PriorityTracer Configurations

The `PSMRTS` `prioritytracer` configuration is simply an ordered array of `ProductRequest`s contained in JSON format. While construction of tracers w/wo meshes can be achieved using simple `keyword=value`, which is easy for C or Python users, creating a `priortytracer` requires a chain of ordered tracers. This will require additional support for those users to create a `prioritytracer` in `PSMRTS`, which we will provide. 

The format of `prioritytracer`s are of the following form:
```
char bennu_tag_config[] = R"(
{
  "name": "bennu_tag_tracer",
  "type": "prioritytracer",
  "tracers": [
    {
      "tracer": "bullet",
      "dsk_file": "l_00050mm_alt_ptm_5595n04217_v020.bds",
      "priority": 0
    },    
    {
      "tracer": "ellipsoid",
      "radii": [0.283065,0.271215,0.249720],
      "priority": 1
    }    
  ]
} )";
```
The `bennu_tag_tracer` user config provides high resolution (5cm GSD) coverage at the OSIRIS_REx Bennu TAG site whilst also constructing a backup ellipsoid to ensure global geometric coverage for all of Bennu. This type of configuration is indicated in ISIS when running `campt` as both poles are check for visibility as well as sub-s/c and sub-solar surface intercepts are needed. By leaving out `priority` values in the `tracer` specs, they will be ordered as they occur in the configuration. (__NOTE__: this requires strict ordering in JSON as data objects are read/constructed, but that requirement is not part of the JSON spec. We use [nholmann::json](https://github.com/nlohmann/json) which has this support provided with [nholmann::ordered_json](https://json.nlohmann.me/api/ordered_json/) which is standardized use within PSMRTS).

One thing to note: `PSMRTS` will not share any instances of `ellipsoid`s. They typically have very little overhead and it simplifies support for mathematically defined shape models in `PSMRTS` to simply create unique instances of ellipsoids.

@section designConfigSyntax Product Configuration Syntax: Rules and Examples

The PSMRTS system contains C and C++ API componets that provide product parameterization techniques. They are designed with user specification considerations which mainly entails string syntax product configurations. The preferred format is JSON, but admittedly, it can be tedious with all the double quotes, curly braces and square brackets. So we have embrace support for parameter-value language (PVL) as an option to simplify the user configuration process while imposing some JSON syntax into the rules. Note that this format is supported for both the C and C++ developer where indicated - which is certainly testing processes!

The JSON string format is highly preferred as the parsing rules are standardized across many third party tools. One thing we rely on that is not a JSON standard is retaining the order of JSON keywords, which are typically ordered alphabetically.  We use nlohmann::json in our implementation that provides an `ordered_json` type. This type retains the order of keywords as they are parsed in strings. We also generate full JSON strings to convey nearly all our data to users. The PVL format satisfies configuration requirements and simplifies syntax for many users.

@section designCreateProducts Processes to Create Products

Process requests, or PRQs, are used to configure PSMRTS products. Ultimately, this process results in compound ray tracing objects for individual targets. Targets can be global or regional for most any composition. These instances are scalable and can result in complex configurations.

PSMRTS products are primarily comprised of shapes, tracers and priority tracers. Shapes are typically stored in files containing tesselated plate/mesh-like formatted data. Currently, PSMRTS supports OBJ, PLY and NAIF DSK file formats. Users can also map data to PMSRTS buffers that can also be used in PSMRTS tracers. (DEMONSTRATE THIS!!)

Tracers may or may not require a mesh object. For example, spheres, spheroids (two axes) and (triaxial) ellipsoids are dataless mathematical models of shapes. PSMRTS must accomodate construction of these types of objects and more.

Priority tracers are set of two or more tracers that act on the same body. Complex multi-body tracing can be achieved, but requires frame transformations of observer positions and look direction rays from the observer into body fixed coordinates. Thus, target body shapes must be represented in body fixed coordinates where the center of the body is the origin. Frame rotations are not currently support directly in PSMRTS but is planned in future releases. Users can also create custom tracing enviroments for these and other special purposes.

PSMRTS products are constructed from a user/dev supplied configuration. Individual product configurations are contained in the ProductConfiguration object. The PRQConfiguration class, utilizing a fundamental request/process technique, contains one or more product configurations that ultimately end up as a tracer or a priority tracer. Shapes are consumed by tracers and tracers are comprised of multiple tracers.

@section designCreateProductSteps Steps to Create Products

The steps required to create PSMRTS products are outlined below. Generally, users must construct a ProductConfiguration comprised of product options specified to the desired object to create. Fundamentally, ProductConfigurations are designed toward tracers. As such, the configuration of a shape is included within a ProductConfiguration as required by the tracer. Tracer shape requirements can vary significantly. For example, spheres, spheroids and ellipsoids have no physical data associated with its mathematical model other than radii; NAIF DSK tracers support only one file format (NAIF DSK .bds) and are not physically loaded but disk bound; Bullet can accept any triangular/facet-based mesh (including float and double types - PSMRTS standardizes on double precison mesh data). However, a configuration can specify only a shape. Priority tracers are created from individual tracers where each has a ProductConfiguration.

Here are preconditions/actions/considerations that occur in the creation of PSRMTS tracer products:
1. ProductConfigurations (`config` hereafter) are added to the PRQProduct (hereafter just `PRQ` unless otherwise specified) object.
1. For each individual tracer, a new `config` is added to the `PRQ`.
1. The `PRQ` is processed by the PsmrtsFactory::process(`PRQ`) method.
1. After processing, the PRQ contains all its products (minimally) that can be used to construct additional compound structures in a PsmrtsInventory, the basis of the PSMRTS caching system.
1. The desired products can be extracted from the `PRQ` results.
1. The `PRQ` result can be `merged` into the main PSMRTS cache, `psmrts`, added to the PsmrtsFactory cache as a standalone cache for future reference.
1. Then, `PRQ` requests can be restricted to unique cache names (may be a file name) by `adding` its name to a list of inventories to search for existing products into the `PRQ` directly.
1. Unique caches are not included in searches unless added by name to the `PRQ`, otherwise the system cache `psmrts` is used.
1. If the preliminary search for an existing product fails, it is created using the product `config`.
1. Priority tracers can efficiently and effectively be stored as an entire PsmrtsInventory. However, all the components of a priorty tracer are contained in the cache system individually and constructed on demand unless the priority tracer is constructed by name.


Here are the processing steps ran in PsmrtsFactory::process(`PRQ`) to create PSRMTS tracer products:
1. Evalute `config` to identify individual products using `ProductSpecification` as stored in the factory caching system.
1. After each product has been identified, check to determine if any residual options remain.
    1. Unconsumed options indicate an ill-configured composite product. These may well be typos in named parameters.
    1. An error condition is logged and the process is terminated.
1. For each defined product, exiting PSMRTS product caches are executed as defined above.
1. If cached products don't exist, a new one is created and added to the `PRQ` local result inventory for future reference (remember the local cache is also searched first).
1. Priority tracers can efficiently and effectively be stored as an entire PsmrtsInventory. 
