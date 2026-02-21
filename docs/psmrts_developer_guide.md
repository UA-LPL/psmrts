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
triaxial ellipsoids. PSMRTS tracer products are Bullet, NaifDslk and Ellipsoids
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
be used. All 

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

  // Get the tracer
  PsmrtTracer bullet_t = maker_t.product();
}
```

Once you have a tracer, you can now create a rays to process with the tracer.

## PsmrtsRayTrace Processing

## PsmrtsPriorityTracer Creation - The Utimate PSMRTS Tracing System


<h2>Contributors</h2>

PSMRTS is an open source Planetary Shape Model and Ray Tracing System built and supported by the Planetary Science community.

Notable Thanks to the following Contributors to PSMRTS:

  - Kris J. Becker
  - Kyle A. Becker


