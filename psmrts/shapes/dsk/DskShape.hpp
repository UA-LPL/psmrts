#ifndef DskShape_hpp
#define DskShape_hpp

#include <string>

#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/ProductSpecification.hpp>
#include <psmrts/shapes/dsk/private/PsmrtsDSKFormat.hpp>

namespace psmrts {
  class DskShape : public PsmrtsProduct {
    public:
      DskShape( ) : PsmrtsProduct("none", "dsk"), 
                    m_model(), m_mesh() /**m_configured("dsk")*/ { }
      DskShape( const psmrts::PsmrtsDSKFormat &dsk_t ) :
                PsmrtsProduct(dsk_t.dsk_source(), "dsk"), 
                m_model( dsk_t ), m_mesh( dsk_t.get_mesh() )
                /**m_configured( dsk_t.get_metadata() )*/ { }
      DskShape( const std::string &dsk_file ) :
                PsmrtsProduct( dsk_file, "dsk"), 
                m_model( dsk_file ), 
                m_mesh( m_model.get_mesh() )
                /**m_configured( m_model.get_metadata() )*/ { }
      virtual ~DskShape() { } 
     

      inline bool process( PRQFeatures &features ) const {
        features.add_feature( this->product_specifications().json_specs() );
        return ( true );
      }

      static inline ProductSpecification product_specifications() {
        char text[] = R"(
        {
            "name": "dsk",
            "product": "shape",
            "type": "mesh",
            "description": "Reads SPICE DSK (.bds) surface shape models and creates a PMRTS mesh object",
            "driver": {
                "name": "dsk",
                "type": "system",
                "aliases": [ "DSK", "bds", "BDS" ]
            },
            "features": [
                {
                    "name": "dsk_string",
                    "type": "string",
                    "description": "Binary string containing contents of a DSK file",
                    "status": "optional",
                    "aliases": ["dsk_binary_string"]
                },
                {
                    "name": "dsk_data_type",
                    "type": "string",
                    "description": "Type of surface vector data requested/read",
                    "status": "optional",
                    "aliases": ["surface_data_type"],
                    "valid": ["double", "float"],
                    "default": "double"
                },
                {
                    "name": "dsk_body_id",
                    "type": "int",
                    "description": "NAIF ID of the target body whose surface is described",
                    "status": "optional",
                    "aliases": ["target_id", "naif_id"]
                },
                {
                    "name": "dsk_segment_index",
                    "type": "int",
                    "description": "Index of DSK segment to load, if file contains multiple segments",
                    "status": "optional",
                    "aliases": ["segment", "shape_segment"],
                    "default": 0
                }
            ]
        })";
        return ( ProductSpecification( "dsk", "mesh", "shape", json_utils::parse_json_string( text ) ) );
      }

      inline const PsmrtsMeshData &get_mesh() const {
        return m_mesh; 
      }

      inline const ProductConfiguration &config() const {
        return ( m_configured );
      }

    protected:
       psmrts::PsmrtsDSKFormat m_model; // Need to address this, .cpp?
       psmrts::PsmrtsMeshData m_mesh;
       psmrts::ProductConfiguration m_configured;
    };
}

#endif
