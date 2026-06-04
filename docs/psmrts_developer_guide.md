@page developerGuide PSMRTS Developer Guide

The Planetary Shape Model Ray Tracing System (PSMRTS) provides C++ and C APIs
that ultimately produces prioritized ray tracing systems. This is an order
series of customizable ray tracers that operate on tesselated plates, or
facet-based meshes, and mathmatical models of ellipsoids.

## Psmrts Products
The PSMRTS system has two main types of _products_. _Products_ in PSMRTS are
shapes (or meshes) and tracers. Shapes are typically provided in several types
of file formats. Current support formats are PLY, OBJ and NAIF DSK files. From
each of these files, a mesh is read and convert to a set of mesh vertices and
facet indexes. Once read, ray tracing systems will use these to create tracers.
Tracers use meshes or may be mathematical models, such as spheres, spheroids or
triaxial ellipsoids. PSMRTS tracer products are Bullet, NaifDsk and Ellipsoids
(spheres and spheroids are just special cases of the ellispsoid). 

## Product Specifications and Configurations
Within the PSMRTS system, each of these products can be configurable using
product specification options called _features_. Each PSMRTS product maintains
its own set of features that are (or should be) unique to its specification.
Values for these features are specified in product _configurations_. Product
configurations contain _options_ that are feature keywords and value pairs where
the keywords are product specification feature keywords and values conform to
the type and valid values of the feature. Each product configuration can define
no more than one shape and/or one tracer. There are some tracers that accept
shapes specified by a filename. Each of the shape products contain a list of
file suffixes that are used to unquely and easily identify a supported format
from which a mesh is read and processed.

Each product contains a [ProductSpecification](../psmrts/core/products/ProductSpecification.hpp) definition that accepts a
[ProductConfiguration](../psmrts/core/products/ProductConfiguration.hpp) that contains feature option keyword/value pairs that are
compared to its specifications. Each option is evaluated against features by
name or _alias_. A feature _alias_ is simply another keyword that can be
substituded for the real feature option name so that generic configurations can
be used.

The PSMRTS application,
[psmrts_product_specs](../psmrts/apps/psmrts_product_specs.cpp) provides the
the product interface specifications of all shapes and tracers. The output is
parseable JSON format.

## PsmrtsShape Product Configurations
The class [PsrmtsShape](../psmrts/shapes/PsmrtsShape.hpp) maintains a set of
shape file readers. The supported shape formats are provided NAIF DSKs by
[DskShape](../psmrts/shapes/dsk/DskShape.hpp), OBJ by
[ObjShape](../psmrts/shapes/obj/ObjShape.hpp), PLY by
[PlyShape](../psmrts/shapes/ply/PlyShape.hpp) and generic mesh in
[MeshShape](../psmrts/shapes/mesh/MeshShape.hpp). 

To configure a shape, refer to the shape only product configuration as reported
by by running the command `psmrts_product_specs --shapes` or with a specific
shape using `psmrts_product_specs obj`. For the `obj` shape, you will see the
following configuration:
```
{
 "shapes": [
  {
   "obj": {
    "info": {
     "name": "obj",
     "product": "shape",
     "description": "Reads Wavefront OBJ mesh files and creates a PMRTS mesh object"
    },
    "features": [
     {
      "name": "shape",
      "type": "string",
      "description": "Describe the product type",
      "status": "optional",
      "default": "obj",
      "valid": "obj"
     },
     {
      "name": "obj_file",
      "type": "file",
      "description": "Name of OBJ file/string to read",
      "status": "required",
      "aliases": [
       "file",
       "filename",
       "obj_mesh",
       "mesh_file",
       "shapefile"
      ],
      "file_suffixes": [
       "obj",
       "OBJ"
      ]
     },
     {
      "name": "obj_string",
      "type": "string",
      "description": "Format-compatible string containing contents of an OBJ file",
      "status": "optional",
      "aliases": "obj_mesh_string"
     },
     {
      "name": "obj_data_type",
      "type": "string",
      "description": "Type of mesh vector data requested/read",
      "status": "optional",
      "aliases": [
       "data_type",
       "mesh_data_type"
      ],
      "valid": [
       "double",
       "float"
      ],
      "default": "double"
     },
     {
      "name": "obj_mtl_search_path",
      "type": "directory",
      "description": "Directory path to OBJ materials files",
      "status": "optional",
      "aliases": [
       "obj_materials_dir",
       "obj_materials_directory"
      ]
     }
    ]
   }
  }
 ]
}
```
The minimum requrement to specify this shape is to provide a `obj_file` option
naming a specific file name, such as
`bennu_g_00880mm_alt_obj_0000n00000_v021.obj`, using the
[ProductOption](../psmrts/core/products/ProductOption.hpp). To more
fully specify the `obj` format directy, you can use the _product_ and _name_
options shown in the __info__ section of the above JSON structure. To configure
this shape in PSMRTS, use the following code:
```
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductCart.hpp>
#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductMaker.hpp>
#include <psmrts/core/PsmrtsShape.hpp>

namespace psmrts {

  std::string objfile = "bennu_g_00880mm_alt_obj_0000n00000_v021.obj";
  ProductConfiguration obj_config( objfile,
                                  { ProductOption( "shape", "obj" ), 
                                    ProductOption( "obj_file", objfile ) } );


  // Create the product and read the mesh
  PsmrtsMaker<PsmrtsShape> maker_s( "maker" );
  if ( !maker_s.process_config( obj_config ) ) {
    maker_s.throw_errors();  // To invoke std::runtime_error() or get the errors
                            // using the PsmrtsMaker::error_to_string() method.
  }

  // Get the shape/mesh
  PsmrtShape obj_s = maker_s.product(); // Will be invalid if not created
  ProductCart obj_c = maker_s.cart();   // Contains the ProductSpecification matched

  // PsmrtsUid.is_valid_uid( obj_c.get_shape_id() )  == true;
  // PsmrtsUid.is_valid_uid( obj_c.get_tracer_id() ) == false;
  if ( !obj_c.has_valid_content() ) {
    // If there are residuals, then the config has unrecognized options which
    //  could be an error
    if ( obj_c.residual_size() > 0 ) {
      throw std::runtime_error( "*** Error - shape has residuals which is invalid: " + 
                                obj_c.residual_config().to_json().dump(-1) );
    }
  }

  // Get the mesh if needed
  PsmrtsMeshData mesh = obj_s.get_mesh();
} // namespace psmrts

```

## PsmrtsTracer Product Configuration
The configuration of PSMRTS tracers are very similar to how shapes are
configured. The `bullet` tracer is perhaps the most versatile and  efficient
tracer in PSMRTS at the moment. It accepts all types of PsmrtsShapes and creates
a very efficient tracer by default. The configuration for this tracer must
include a configuration for a shape as well. This is indicated by the
specfication type of _dependency_ in the "shape" feature option as shown in the
bullet specification produced by `psmrts_product_specs bullet`:
```
{
 "tracers": [
  {
   "bullet": {
    "info": {
     "name": "bullet",
     "product": "tracer",
     "description": "The Bullet Physics ray tracing system specification"
    },
    "features": [
     {
      "name": "tracer",
      "type": "string",
      "description": "Describe the product type",
      "status": "required",
      "default": "bullet",
      "valid": "bullet"
     },
     {
      "name": "shape",
      "type": "string",
      "status": "dependency",
      "description": "Bullet requires a file/mesh shape",
      "aliases": [
       "file",
       "filename",
       "obj_file",
       "obj_mesh",
       "obj_string",
       "ply_file",
       "ply_mesh",
       "dsk_file",
       "dsk_mesh",
       "mesh_file",
       "source"
      ]
     },
     {
      "name": "bullet_optimize_bvh",
      "type": "bool",
      "description": "Use optimized bounding volume hierachy (BVH) when created",
      "status": "optional",
      "default": "true",
      "valid": [
       "true",
       "1",
       "yes",
       "false",
       "0",
       "no"
      ]
     },
     {
      "name": "bullet_compression",
      "type": "bool",
      "description": "Compress Bullet data during construction",
      "status": "optional",
      "default": "true",
      "valid": [
       "true",
       "1",
       "yes",
       "false",
       "0",
       "no"
      ]
     }
    ]
   }
  }
 ]
}
```
This particular specification requires the __tracer=bullet__ configuration
option because it has no other required options other than __shape__ (a
_dependency_ type is a required parameter). Note the _alias_ options for
__shape__. It specifies all the file name feature options and their aliases of
all the valid shape types. This implies that if one of these options is given in
the "bullet" configuration, it is recognized as providing a __shape__ option.
Note that internally, it will be passed as a _residual_ configuration option
that indicates a __shape__ product is required. The `PsmrtsMaker` logic will
then automagically process the shape configuration. Here is the minimal
configuration for a "bullet" tracer.
```
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductCart.hpp>
#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductMaker.hpp>
#include <psmrts/core/PsmrtsShape.hpp>

namespace psmrts {

  std::string objfile = "bennu_g_00880mm_alt_obj_0000n00000_v021.obj";
  ProductConfiguration bullet_config( "bullet" );
  bullet_config.add( ProductOption( "shape", "obj" ) );
  bullet_config.add( ProductOption( "obj_file", obj_file)  ) );
  bullet_config.add( ProductOption( "tracer", "bullet" ) );
  PsmrtsMaker<PsmrtsTracer> maker_t( "maker" );

  if ( !maker_t.process_config( bullet_config ) ) {
    maker_t.throw_errors();  
  }

  // Get the tracer if it has been successfully created.
  PsmrtTracer bullet_t = maker_t.product();
}
```

## PSMRTS Ray Tracing System
Once you have a tracer, you can now create a rays to process with the tracer. A
ray in PSRMTS follows standard definition of the an observer/look direction
three dimensional (3D) vector system. In this system, there are two vectors that
define a ray trace: a vector describing the position of the observer and a
second unit vector that represents the direction from the observer to perform a
trace. These two vectors combined is a PSMRTS ray trace and are stored in the
[PsmrtsRayTrace](../psmrts/core/PsmrtsRayTrace.hpp) class.

### Observer/Look Direction Coordinate System
PSMRTS performs ray traces in the planet body-fixed coordinate system. The
position of the observer is a ray that extends from the center of the planet
body to a point in 3D space. The look vector is a unit vector where the orgin
is from the observer position presumably toward the plant body surface. These
two vectors are parameters to constructors provided in the
[PsmrstRayTrace](../psmrts/core/PsmrtsRayTrace.hpp) class. This class is the
fundamental basis for a system of extended classes that perform different types
and varieties of traces. However, the result of every trace in the PSRMTS
system is retained in a PsmrtsRayTrace object after the PSMRTS tracer calculates
surface intercepts of the ray path along look vectors. If the ray successfully
intercepts the planet surface, methods in this class provides or computes other
cartographic/observational geometry and surface properties.

### Photometric Ray Tracing Operations
PSMRTS provides a suite of helper tracing classes that extend tracing operations.
A common operation in planetary observation geometry is to compute photometric
angles. These angles are computed using a second ray from the sun to the
observer/look direction ray trace surface intercept point. This ray requires
the precise position of the sun at the time the observer/look direction vector
was calculated. Therefore, this operation requires three vectors: the two
observer position and unit look vector and the position of the sun relative
to the body of intercept. The sun relative position vector is also in body-fixed
coordinates where the origin is the center of the body. The photometric ray look
direction vector is computed as the vector from the sun to the surface intercept
computed from the observer/look direction vector. 

### Process Request Ray Tracing Classes
PSMRTS provides a _process request_ system of classes, called `PRQ`, that
extends ray tracing capabilites. These classes compute one or more ray trace
computations. These classes are defined in the file
[PsmrtsRequest](../psmrts/core/PsmrtsRequest.hpp). These classes are special
processing class that inherit `PsmrtsRequest` as a base class that tracks errors
and performance metrics. The primary purpose of these classes are to provide
conditional execution of PRQ request class processing operations. 

This is necessary since not all tracers possess the same properties. For example,
mathematical models such as ellipsoids, spheroids and spheres do not have
facets. One datum provided within the PsrmtsRayTrace is a __facet__. A __facet__
is comprised of a 3-vector set required to describe a mesh triangle in 3D space
that originates from the shape mesh and an integer-based vector where each value
in the vector is the index into the mesh data buffer containing the surface
vectors. For mathematical models, there are no
facets. Facets are retrieved in a `PRQFacet` class that is passed to a tracer
method called of the form __process( PRQFacet &facet)__ that must be implemented
in tracers that actually have facets. For tracers that do not have facets, they
don't implement these methods.

In fact, any tracer that does have certain PRQ properties, do not have to
implement any of the tracer process() methods. This minimizes the burden of
writting dummy methods and minimizes code maintanence.

Here is a list and brief description of all the PRQ classes contained in the
[PsrmtsRequest](../psmrts/core/PsmrtsRequest.hpp) file:

- __PQRRayTrace__ contains a single ray trace and is the fundamental class for all
  ray tracers. It is primarily for precision tracking of ray trace operations.
- __PRQRayTraceArray__ contains a vector of ray traces that are performed in
  sequence (or in threads).
- __PRQPhotometricTrace__ contains two traces, one for observer look direction
  and a second for sun/surface intercept tracing. The sun look direction vector
  is computed from a succesful observer/look direction surface intercept.
- __PRQPhotometricTraceArray__ contains an array of photometric trace instances
  containing a vector photometric ray traces.
- __PRQFacet__ Retrieves the mesh facet datum from the tracer of intercept if it
  exists. If it does not exist, then PsmrtsRequest::was_invoked() returns false.

`PsmrtsRequest` contains methods that return timing information from the time a
process() method was invoked and returned (see runtime_ms()), now many times a
process method was called (see run_count()) and if any errors occured (see
error_count()) among other inforamation. It is recommend these PRQs be utilized
and developed in tracers. 

There is a special template class called __MissingProcessRequestHandler__ that
is part of a defined macro called __PSMRTS_PROCESS_CATCHALL()__ that is designed
to be added at the bottom of the ray tracer classes to trap all unimplemented
`process( PRQ )` methods and log errors and tracking data.

## PsmrtsPriorityTracer Creation - The Utimate PSMRTS Tracing System
In previous discussion of PSMRTS product creation, the products created are
unique instances of shapes and tracers. One of the most critical properties of
PSMRTS is the sharing of products to minimize use and manage memory resources.
The [PsrmtsFactory](../psmrts/core/PsmrtsFactory.hpp) class maintains a
system-wide inventory of shapes, tracers and priority tracers primarly for reuse
purposes. The fundamental storage mechanism is the
[PsrmtsInventory](../psmrts/core/PsmrtsInventory.hpp) database. It consists of
product storage containers of PSMRTS shapes, tracers, priority tracers and file
path translators. 

### File Path Translations 
File path translators, called
[PsmrtsTranslations](../psmrts/core/PsmrtsTranslations.hpp) contain OS shell
environment variables and ISIS DataDirectory-like parameters that are primarly used
to substitute instances of file path elements containing tags that start with a
"$". The file path substitution algorithm performs a parameterized count of
iterations of parameter/environment file path substitutions to ensure multiple
levels or substitutions are expanded properly. These are applied automatically
in the configuration/specification processing phase of product creation. The
expanded paths are stored in configuration _metadata_ containers.

### PsmrtsInvoice - Resource Management Strategies
The [PsmrtsInvoice](../psmrts/core/PsmrtsInvoice.hpp) class is the primary
resource management system that should be used to take full advantage of all
PSRMTS capabilites. It contains a local PsmrtsInventory of all shapes, tracers,
priority tracers and translators for maximum efficiency. It also integrates use
of the system-wide PSRMTS factory system for centralized and coordinated
resource management. See
[ProductProcessing](../psmrts/core/products/ProductProcessing.hpp) for
additional details.

PsmrtsInvoice applies a factory/invoicing model to create priorty tracers from
all existing resources. Here is an example of how to utililize this resource to
create the ultimate tracing system of multiple resources and tracing conditions.

```
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductOrder.hpp>
#include <psmrts/core/products/ProductMaker.hpp>
#include <psmrts/core/products/ProductCart.hpp>
#include <psmrts/core/products/ProductProcessing.hpp>
#include <psmrts/core/PsmrtsInvoice.hpp>

namespace psmrts { 
  using UIDType = psmrts::PsmrtsInventory::UIDType;

  PsmrtsTranslations trans_t = psmrts::PsmrtsTranslations::create();
  PsmrtsInvoice invoice_t( "myisiscube", trans_t );

  std::string objfile = "bennu_g_00880mm_alt_obj_0000n00000_v021.obj";
  ProductConfiguration bullet_t("bulletmaker");
  bullet_t.add( psmrts::ProductOption( "shape", "obj" ) );
  bullet_t.add( psmrts::ProductOption( "obj_file", objfile ) );
  bullet_t.add( psmrts::ProductOption( "tracer", "bullet" ) );
  invoice_t.add_product( bullet_t );

  std::string dskfile = "bennu_g_00400mm_alt_ptm_0000n00000_v021.bds";
  ProductConfiguration naifdsk_t("naifdskmaker");
  naifdsk_t.add( psmrts::ProductOption( "tracer", "naifdsk" ) );
  naifdsk_t.add( psmrts::ProductOption( "dsk_file", dskfile)  );
  invoice_t.add_product( naifdsk_t );

  ProductConfiguration ellipsoid_t("ellipsoidkmaker");
  ellipsoid_t.add( ProductOption( "tracer", "ellipsoid" ) );
  ellipsoid_t.add( ProductOption( "radii", { 0.283065, 0.271215, 0.249720 } ) );
  ellipsoid_t.add( ProductOption( "name", "bennu" ) );
  invoice_t.add_product( ellipsoid_t );

  PsmrtsPriorityTracer priority_t = invoice_t.get_priority_tracer( );

} // namspace psmrts
```

This example shows three separate instances of representations of Bennu shape
models in one priority tracer. You can also apply algorithms to order shapes
directly in a priorty tracer using the PsmrtsPriorityTracer::prioritize(). See
the PsmrtsPriorityTracer::reverse_order() method to apply a lambda reverse the
order of tracers in a priority tracer.

<h2>Contributors</h2>

PSMRTS is an open source Planetary Shape Model and Ray Tracing System built and supported by the Planetary Science community.

Notable Thanks to the following Contributors to PSMRTS:

  - Kris J. Becker
  - Kyle A. Becker


