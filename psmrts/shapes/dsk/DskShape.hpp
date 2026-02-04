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

#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/ProductSpecification.hpp>
#include <psmrts/shapes/dsk/private/PsmrtsDSKFormat.hpp>

namespace psmrts {
  class DskShape : public PsmrtsProduct {
    public:
      using ProductInfo     = ProductSpecification::ProductInfo;
      using ProductFeatures = ProductSpecification::ProductFeatures;

      DskShape( ) : PsmrtsProduct("none", "dsk"), 
                    m_model(), m_mesh(), 
                    m_configured( { ProductConfiguration("dsk") } )  { }
      DskShape( const psmrts::PsmrtsDSKFormat &dsk_t ) :
                PsmrtsProduct(dsk_t.dsk_source(), "dsk"), 
                m_model( dsk_t ), m_mesh( dsk_t.get_mesh() ), 
                m_configured( dsk_t.config() )  { }
      DskShape( const std::string &dsk_file ) :
                PsmrtsProduct( dsk_file, "dsk"), 
                m_model( dsk_file ), 
                m_mesh( m_model.get_mesh() ),
                m_configured( m_model.config() )  { }
      virtual ~DskShape() { } 
     

      inline bool process( PRQFeatures &features ) const {
        features.add_feature( this->product_specifications().to_json() );
        return ( true );
      }

      static inline ProductSpecification product_specifications() {
        ProductInfo  info( "dsk", { 
                                 ProductOption( "name", "dsk"),
                                 ProductOption( "product", "shape"),
                                 ProductOption( "description", "Reads SPICE DSK (bds) surface shape models and creates a PMRTS mesh object") } );
        ProductFeature dfile( "dsk_file", {
                                 ProductOption( "name", "dsk_file"),
                                 ProductOption( "type", "file"),
                                 ProductOption( "description", "Name of DSK file to read"),
                                 ProductOption( "status", "required"),
                                 ProductOption( "aliases", {"file", "dsk_mesh", "mesh_file"} ),
                                 ProductOption( "file_suffixes", { "bds", "BDS" } ) } );

        ProductFeature dtype( "dsk_data_type", {
                                 ProductOption( "name", "dsk_data_type"),
                                 ProductOption( "type", "string"),
                                 ProductOption( "description", "Type of mesh vector data requested/read"),
                                 ProductOption( "status", "optional"),
                                 ProductOption( "aliases", { "data_type", "mesh_data_type" } ), 
                                 ProductOption( "valid", { "double", "float"} ),
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
        return ( ProductSpecification( info, { dfile, dtype, bodyid, segid } ) );        
      }

      inline const PsmrtsMeshData &get_mesh() const {
        return m_mesh; 
      }

      inline const ProductConfiguration &config( const size_t index = 0 ) const {
        return ( m_configured[index] );
      }

      inline bool matches( const ProductConfiguration &conf ) const {
        return ( this->config().matches( conf ) );
      }

    protected:
      using DskSegmentConfigList = PsmrtsDSKFormat::DskSegmentConfigList;
      PsmrtsDSKFormat      m_model; // Need to address this, .cpp?
      PsmrtsMeshData       m_mesh;
      DskSegmentConfigList m_configured;
  };
}

#endif
