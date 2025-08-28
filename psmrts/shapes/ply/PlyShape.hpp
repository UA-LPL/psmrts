#ifndef PlyShape_hpp
#define PlyShape_hpp

#include <string>

#include <psmrts/shapes/ply/private/PsmrtsPLYFormat.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>

namespace psmrts {
    /**
     * @brief PLY format mesh process class
     * 
     */
    class PlyShape {
        public:
         PlyShape() {}
         PlyShape( const psmrts::PsmrtsPLYFormat &ply_t ) :
                            m_model( ply_t ) { }
         virtual ~PlyShape() { }
         
         /**
          * @brief PRQFeatures holding Format-relevant specification data
          *  - Possibly needs removal
          * @param features 
          * @return true 
          * @return false 
          */
         inline bool process( PRQFeatures &features ) const {
            features.add_feature( this->product_specifications().json_specs() );
            return ( true );
         }
        

         static inline ProductSpecification product_specifications() {
            char text[] = R"(
            {
                "name": "ply",
                "product": "shape",
                "type": "mesh",
                "description": "Reads ASCII and Binary PLY mesh files and creates a PSMRTS mesh object",
                "driver": {
                    "name": "ply",
                    "type": "system",
                    "aliases": [ "PLY" ]
                },
                "features": [
                    {
                        "name": "ply_file",
                        "type": "file",
                        "description": "Name of PLY file to read",
                        "status": "optional",
                        "aliases": ["file", "ply_mesh", "mesh_file"],
                        "file_suffixes": ["ply", "PLY"]
                    },
                    {
                        "name": "ply_data_type",
                        "type": "string",
                        "description": "Type of mesh vector data requested/read",
                        "status": "optional",
                        "aliases": ["mesh_data_type"],
                        "valid": ["double", "float"],
                        "default": "double"
                    }
                ]
            })";
            return ( ProductSpecification( "ply", "mesh", "shape", json_utils::parse_json_string(text)));
         }

         PSMRTS_PROCESS_CATCHALL( "PlyShape" )

        protected:
         psmrts::PsmrtsPLYFormat m_model;
    };

} // namespace psmrts

#endif
