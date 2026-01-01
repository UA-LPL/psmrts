#ifndef MeshShape_hpp
#define MeshShape_hpp

#include <string>

#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/ProductSpecification.hpp>
#include <psmrts/core/PsmrtsMeshData.hpp>

namespace psmrts {
    /**
     * @brief Generic mesh shape product
     * 
     */
    class MeshShape : public PsmrtsProduct {
        public:
         MeshShape() : PsmrtsProduct( "none", "mesh"), m_mesh() { }
         MeshShape( const PsmrtsMeshData &mesh, 
          const std::string &name = "mesh") : PsmrtsProduct( name, "mesh" ),
                                              m_mesh( mesh ) { }
         virtual ~MeshShape() = default;
         
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
                "name": "mesh",
                "product": "shape",
                "type": "mesh",
                "description": "Provides support for a genric user defined shape",
                "driver": {
                    "name": "mesh",
                    "type": "system",
                    "aliases": [ "generic" ]
                },
                "features": [
                    {
                        "name": "mesh_name",
                        "type": "file",
                        "description": "Name of mesh data",
                        "status": "required",
                        "aliases": ["mesh", "name"],
                        "file_suffixes": []
                    },
                    {
                        "name": "mesh_data_type",
                        "type": "string",
                        "description": "Type of mesh vector data provided",
                        "status": "optional",
                        "aliases": ["mesh_data_type"],
                        "valid": ["double", "float"]
                    }
                ]
            })";
            return ( ProductSpecification( "mesh", "mesh", "shape", json_utils::parse_json_string(text)));
         }

         inline const PsmrtsMeshData &get_mesh() const {
            return m_mesh;
         }

         PSMRTS_PROCESS_CATCHALL( "MeshShape" )

        protected:
          PsmrtsMeshData m_mesh;
    };

} // namespace psmrts

#endif
