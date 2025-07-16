#ifndef DskShape_hpp
#define DskShape_hpp

#include <string>

#include <PsmrtsDSKFormat.hpp>
#include <PsmrtsRequest.hpp>
#include <ProductSpecification.hpp>

namespace psmrts {
    class DskShape {
     public:
      DskShape( ) { }
      DskShape( const psmrts::PsmrtsDSKFormat &dsk_t ) :
                         m_model( dsk_t ) { }
        virtual ~DskShape() { } 
     
        /** 
     inline bool process( PRQFeatures &features ) const {
        features.add_feature( this->product_specifications().specs() );
        return ( true );
     }
        */

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
            "parameters": [
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

     protected:
       psmrts::PsmrtsDSKFormat m_model;
    };
}

#endif
