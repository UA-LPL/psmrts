/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef DskShape_hpp
#define DskShape_hpp

#include <string>
#include <memory>

#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsMeshData.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductCart.hpp>


namespace psmrts {
  class DskShape : public PsmrtsProduct {
    public:
      using ProductInfo     = ProductSpecification::ProductInfo;
      using ProductFeatures = ProductSpecification::ProductFeatures;
      using SharedMeshData  = std::shared_ptr<PsmrtsMeshData>;

      DskShape( ) : PsmrtsProduct( "null", "tracer", "dsk" ), 
                    m_mesh(), m_config( { ProductConfiguration("dsk") } )  {
        m_mesh = make_shared_copy ( PsmrtsMeshData() );
      }
      DskShape( const std::string &dsk_file, const int segnum = 0 );
      DskShape( const ProductCart &processed_cart );
      virtual ~DskShape() = default;
     

      inline bool process( PRQFeatures &features ) const {
        features.add_feature( this->product_specifications().to_json() );
        return ( true );
      }

      static inline ProductSpecification product_specifications() {
        ProductInfo  info( "dsk", { 
                                 ProductOption( "name", "dsk"),
                                 ProductOption( "product", "shape"),
                                 ProductOption( "description", "Reads SPICE DSK (bds) surface shape models and creates a PMRTS mesh object") } );
        ProductFeature product( "shape", {
                                 ProductOption( "name", "shape" ),
                                 ProductOption( "type", "string" ),
                                 ProductOption( "description", "Describe the product type" ),
                                 ProductOption( "status", "optional" ),
                                 ProductOption( "default", "dsk" ),
                                 ProductOption( "valid", "dsk" ) } );                                   
        ProductFeature dfile( "dsk_file", {
                                 ProductOption( "name", "dsk_file"),
                                 ProductOption( "type", "file"),
                                 ProductOption( "description", "Name of DSK file to read"),
                                 ProductOption( "status", "required"),
                                 ProductOption( "aliases", {"file", "filename", "dsk_mesh", "mesh_file", "shapefile" } ),
                                 ProductOption( "file_suffixes", { "bds", "BDS" } ) } );
        ProductFeature dtype( "dsk_data_type", {
                                 ProductOption( "name", "dsk_data_type"),
                                 ProductOption( "type", "string"),
                                 ProductOption( "description", "Type of mesh vector data requested/read"),
                                 ProductOption( "status", "optional"),
                                 ProductOption( "aliases", { "data_type", "mesh_data_type" } ), 
//                                 ProductOption( "valid", { "double", "float"} ),
                                 ProductOption( "valid", "double" ),
                                 ProductOption( "default", "double" ) } );
        ProductFeature bodyid( "dsk_body_id", {
                                 ProductOption( "name", "dsk_body_id"),
                                 ProductOption( "type", "int"),
                                 ProductOption( "description", "NAIF ID of the target body whose surface is described"),
                                 ProductOption( "status", "optional"),
                                 ProductOption( "aliases", { "target_id", "naif_id" } ) } );
        ProductFeature segid( "dsk_segment_index", {
                                 ProductOption( "name", "dsk_segment_index"),
                                 ProductOption( "type", "int"),
                                 ProductOption( "description", "NAIF ID of the target body whose surface is described"),
                                 ProductOption( "status", "optional"),
                                 ProductOption( "aliases", { "segment", "dsk_segment"} ) } );
                                 ProductOption( "default", static_cast<int>( 0 ) );

        // This validates the JSON structure and provides product info to callers
        return ( ProductSpecification( info, { product, dfile, dtype, bodyid, segid } ) );        
      }

      inline const PsmrtsMeshData &get_mesh() const {
        return ( *m_mesh ); 
      }

      inline const ProductConfiguration &config( ) const {
        return ( m_config );
      }

    protected:
      SharedMeshData       m_mesh;
      ProductConfiguration m_config;

  };
}

#endif
