
# **PSMRTS Product Specifications**

  **Date**: May 24, 2025  
  **Author**: Kris J. Becker

### PSMRTS Product Concept
`PSMRTS` product request system is similar to a transactional system that allows users to select and configure mesh data sources and apply them in ray tracing enviroments. These environments are typically tracing libraries or systems, that can be customized for planetary shape/body ray tracers created by `PSMRTS`. Priority tracers contain more than one tracer where ray tracing operations can be specialized (e.g., prioritized, nearest-observer) and customized to suite.

`PSMRTS`' prime objective is to create customized/optimized ray tracing configurations primarily for small irregular planetary bodies and shapes while efficiently managing these resources for large systematic processing operations.

Each element - meshes, tracers, even priority tracers - are products. Meshes come from several different file-based format sources: Wavefront OBJ, PLY and NAIF DSK BDS kernel files. Ray tracing systems in `PSMRTS` are Bullet, NAIF DSK and ellipsoids (which have no mesh). Each of these products must provide parameter specifications and customized configurations.  The configuration of a priority tracer is a merge of all the configurations of `PSMRTS` products.

The implementation plan is described in this document.

#### Product Description
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

#### ProductSpecification class 
The `ProductSpecification` class defines the format for product specification. This class will maintain a product name and type along with parameterization specifications and driver information. Each product driver must provide the details shown below and subsequently discussed in the following example. This static method is required in each `PSMRTS` product implementation.

The example shown here is the specification for the `tinyobjloader` OBJ reader. It also reads materials. Additional functionality can be added to support reading/retaining the materials structure and provide an object functor interface to get access to the materials data read by `tinyobjloader`.

```
      static inline ProductSpecification product_specifications() {
        char text[] = R"(
        {
          "name": "obj",
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


#### ProductParameter class
The `ProductParameter` class that maintains and provides operations on the JSON array of `"parameters"` structures. These object instances of the parsed JSON content of the `"parameters"` array will provide operations as needed such as validation of the required format and comparisons of other instances. It should also allow storage of a _value_ keyword that will be added to the ProductParameter object when specified from a user. 

This class will maintain a single product parameter for each element extracted from the `"parameters"` array as shown above. Specifically, structures of the following format will be supported/expected:
```
char parameter_spec[] = R"(
  {
    "name": "obj_file",
    "type": "file",
    "description": "Name of OBJ file to read",
    "status": "required",
    "aliases": ["file", "obj_mesh", "mesh_file"],
    "file_suffixes": [ "obj", "OBJ" ]
  } )";

 return ( ProductParameter( json_utils::parse_json_string( parameter_spec )));

 // Example of direct parameter creation support for configuration purposes
  auto parameter = ProductParameter( "name", "obj_file");
```
Support for a `file` _type_ will be included in match/compare operations by checking for a _file_suffix_ in a _value_ keyword. The _value_ keyword will be explicitly added to the `ProductParameter`' JSON object before passing the JSON config to a `ProductRequest`.

A match validation call will be of the form:
```
  bool is_serviceable = product_parameter.validate( other_parameter );
```

Along with `ProductSpecification`, this class should provide much of the needs to support product specification, driver configuration, user parameterization and validation for every product.

### ProductRequest class
The `ProductRequest` class maintains a configuration for a particular product. `ProductRequest` objects originate ultimately from users, but developers produce the class instances. This class should interact directly with the `ProductSpecification` class to determine if the driver has all the required parameters and proper values for options in the product configuration. I am thinking this class can inherit the `ProductParameter` class and the ability to interogate values is added in methods here. Search capabilties on, at least, parameter names in both `ProductSpecification` and `ProductRequest`.

An instance of a `ProductRequest` is intended to be a simple, linear set of keywords that define all the products needed along with customized parameters to configure a product, ultimately a `shapetracer`. This design needs to support at a minimum our three defined types: `mesh` (formats), `tracer` (`shapetracer`) and `prioritytracer`. The idea here is that each product will be checked for what is needed and calls are made in the `ProductManufacturing` class starting with tracer calls and if tracers need a mesh, it will request a mesh product from its contents. Note that validation of the configuration should be validated with JSON diff() function on the remaining parameters after each product request is fullfilled. A successful configuration occurs when no JSON keys remain after processing a product request in a `ProductRequest`.

#### Tracer configurations
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
return ( ProductRequest( {  
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

#### PriorityTracer configurations
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

## Usage in the C API
This framework lays the foundation for the PSMRTS C/C++ and user interface. It is not lost that some configurations can be quite complex in conveying parameterization for each product requested by a user - particularly when using text strings only. The configurations the `ellisoid`, `spheroid` and `sphere` products are the most basic types. The string configurations expected from the user and applied in the `PSMRTS` C api has the following form:

```
const char *ellipsoid_s = "tracer=ellipsoid;radii=[0.283065,0.271215,0.249720]"; /* or ellipsoid_radii=... */
const char *spheriod_s  = "tracer=spheroid;radii=[0.283065,0.249720]"; /* or spheroid_radii=[0.283065,0.249720] */
const char *sphere_s    = "tracer=sphere;radii=250.0";  /* or sphere_radius=250 */
```
These examples are consistent with configuration strings specified by users that are absorbed and processed by `PSMRTS` that results in a tracer object that either exists in the `PSMRTS` cache (for efficient sharing of resources) or is created anew and added to `PSMRTS`'s internal caching system for efficient reuse. (__OPTION__: Should users be able to opt out of caching their product and return an unshared product at create time?)

So how does this look in a `PSMRTS` C application? The following is a simple C main example demonstrating initialization of the PSMRTS factory system (with the default factory), create an ellipsoid, trace an arbitrary observer/lookdir, compute observational geometry from sun position and finally clean up resources and shut down the factory. This is similar to the [PROJ C API](https://proj.org/en/stable/development/quickstart.html).

```
#include <stdio.h>
#include <psmrts_c.h>

int main( int argc, char *argv[] ) {

  PSMRTS_Factory       *p_factory;
  PSMRTS_Tracer        *ellipsoid;
  PSMRTS_Ray           *ray, *sunray;
  PSMRTS_Vector3d      observer, lookdir, sunpos, sundir, position_v, look_v;
  PSMRTS_Vector3d      emission, incidence, phase, normal, sepang, xyz, surfpt, radlonlat;

  double slant_d, surft_dist;
  double radius_km;

  /* Get the standard factory from PSMRTS to kick things off... */
  p_factory = psmrts_get_factory();

  /* Create an ellipsoid tracer */
  const char *ellipsoid_s = "tracer=ellipsoid;ellisoid=[0.283065,0.271215,0.249720]";
  ellipsoid = psmrts_create_tracer_from_string( p_factory, ellipsoid_s );
  if ( !psmrts_tracer_isvalid( ellipsoid ) )  ) {
    printf("\n*** PSMRTS-C - create errors:\n%s\n", psmrts_tracer_error_str( ellipsoid ) );
    exit ( 1 );
  }

  /** Trace a ray */
  observer = psmrt_vector3d( 0.3, 0.0, 0.0 );
  lookdir  = psmrt_negate( observer );

  /* Trace a ray on the ellipsoid. Create a reusable ray structure  */
  /* to minimize memory create/free overhead. */
  ray = psmrts_create_ray( observer, lookdir );
  ray = psmrts_ray_trace( ray, ellipsoid );
  if ( psmrts_ray_has_hit( ray ) ) {

    /* Retrieve/calculate data from trace */
    xyz       = psmrts_ray_xyz( ray );
    surfpt    = psmrts_ray_surfpt( ray );

    normal    = psmrts_ray_normal( ray );

    slant_d   = psmrts_ray_slant_distance( ray );
    surfpt_d  = psmrts_norm( surfpt );

    radius_km = psmrts_ray_radius( ray );
    radius_pt  = psmrts_norm( xyz );

    radlonlat = psmrts_xyz_to_geo( xyz ); /* Consistent with NAIF */

    /* Use sunpos to get observational geometry */
    sunpos = psmrt_vector3d( 300, 1000, 2000 );
    sundir = psmrts_subtract( psmrts_ray_xyz( ray ), sunpos );

    /* Trace from sun position to surface intercept point */
    sunray = psmrts_ray_trace( psmrts_create_ray( sunpos, sundir ), ellipsoid );
    if ( psmrts_ray_has_hit( sunray ) ) {
      emission  = psmrts_emission( ray );
      incidence = psmrts_incidence( ray, sunray );
      phase     = psmrts_phase( ray, sunray );
    }
    else {
      printf("\n*** PSMRTS-C - Trace from sun failed!\n");
    }
  }

  /* Resource cleanup and factory shutdown */
  psmrts_free_ray( ray );
  psmrts_free_ray( sunray );
  psmrts_free_tracer( ellipsoid );
  psmrts_shutdown_factory( p_factory );

  return ( 0 );
}

```
This implies `psmrts_create_tracer_from_string()` always returns an allocated structure that requires memory to be deleted. This is also true of `psmrts_ray_trace( )`.

Note that this style of a C API tends to play nicely with C++, particularly memory management. The pointer based variables can be configured with explicit memory resource release function specified in the creation of a shared point. The recommended form for C++ using this configuration are similar to this:
```
std::unique_ptr<PSMRTS_Factory> factory( psmrts_get_factory(), psmrts_shutdown_factory );
std::shared_ptr<PSMRTS_Tracer>  ellipsoid( psmrts_create_tracer_from_string( p_factory, ellipsoid_s ), psmrts_free_tracer );
std::shared_ptr<PSMRTS_Ray>     ray( psmrts_ray_trace( ellisoid, observer, lookdir ), psmrts_free_ray );
```
This form works really well to ensure your applications are neatly memory manageable.

#### PSMRTS_Vector3d C Structure
You can see where the PSMRTS_Vector3d plays a large part in the code above. This structure is nothing more that an analogy to Eigen::Vector3d. It is defined as a union of several convenient forms of 3 double precision data values.
```
typedef union {
  struct {
    double x;
    double y;
    double z;
  };
  struct {
    double a;
    double b;
    double c;
  };
  double data[3];
} PSMRTS_Vector3d;
```
You can see an similar approach as this [PROJ_COORD](https://proj.org/en/stable/development/reference/datatypes.html#c.PJ_COORD) structure in the PROJ system. We may need to have formal types for some of the structs just as PROJ has. Note, just to be sure, we must add a Catch2 REQUIRES test to confirm the structure is properly handled by compilers.
```
REQUIRE( sizeof(PSMRTS_Vector3d) == (3 * sizeof(double)) );
```

#### PSMRTS_Ray C Structure
The `PSMRTS_Ray` type consists of observer position and look direction vectors. Both vectors are provided in units of kilometers (km) although the direction vectors are typically unitless and can be normalized. `PSMRTS_Ray` is an opaque pointer to a ray tracer object. In this context, they are actually _PSMRTS request functor_ (PRQ) objects. Specifically, the `PSMRTS_Ray` C API type is actually mapped to the `PRQRayTrace` functor object. These PRQ functor objects are well suited for this feature as they all contain inherent error checking/catching, with full accounting of its execution processing. All PRQ functors are contained in the header file `PsmrtsRequest.hpp`.

To implement the opaque pointer method using this technique, declarations occur in both the psmrts_c.h and psmrts_c.cpp files. Keep in mind that content of `psmrts_c.h` must typically contain only code that can be compiled by both the C and C++ compilers. `psmrts_c.cpp` contain C++ elements that define the C++ interface.

#### psmrts_c.cpp
```
using PSMRTS_Ray = psmrts::PRQRayTrace;
```


#### psmrts_c.h

