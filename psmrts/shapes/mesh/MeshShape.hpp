#ifndef MeshShape_hpp
#define MeshShape_hpp

#include <string>

#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsMeshData.hpp>
#include <psmrts/core/ProductSpecification.hpp>
#include <psmrts/core/ProductConfiguration.hpp>

namespace psmrts {
    /**
     * @brief Generic mesh shape product
     * 
     */
    class MeshShape : public PsmrtsProduct {
        public:
        using ProductInfo     = ProductSpecification::ProductInfo;
        using ProductFeatures = ProductSpecification::ProductFeatures; 

         MeshShape() : PsmrtsProduct( "mesh", "mesh"),
                       m_mesh( ), m_config( init_mesh( "mesh") )  { }
         MeshShape( const PsmrtsMeshData &mesh, 
                    const std::string &name = "mesh") : 
                    PsmrtsProduct( name, "mesh" ),
                    m_mesh( mesh ),
                    m_config( mesh.config() ) { }
         virtual ~MeshShape() = default;
         
         /**
          * @brief PRQFeatures holding Format-relevant specification data
          *  - Possibly needs removal
          * @param features 
          * @return true 
          * @return false 
          */
         inline bool process( PRQFeatures &features ) const {
            features.add_feature( this->product_specifications().to_json() );
            return ( true );
         }
        

         static inline ProductSpecification product_specifications() {
          ProductInfo  info( "mesh", { 
                        ProductOption( "name",        "mesh"),
                        ProductOption( "product",     "shape"),
                        ProductOption( "description", "Provides support for a genric user defined shape" ) } );
          ProductFeature source( "mesh_name", {
                                  ProductOption( "name", "mesh_name"),
                                  ProductOption( "type", "file"),
                                  ProductOption( "description", "Name of mesh data" ),
                                  ProductOption( "status", "required"),
                                  ProductOption( "aliases", { "mesh", "source" } ) } );
          ProductFeature dtype( "mesh_data_type", {
                                  ProductOption( "name", "data_type"),
                                  ProductOption( "type", "string"),
                                  ProductOption( "description", "Type of mesh vector data provided" ),
                                  ProductOption( "status", "optional"),
                                  ProductOption( "aliases", "mesh_data_type" ), 
                                  ProductOption( "valid", { "double", "float" } ) });

          // This validates the JSON structure and provides product info to callers
          return ( ProductSpecification( info, { source, dtype } ) );             
         }

         inline const PsmrtsMeshData &get_mesh() const {
            return m_mesh;
         }

         inline const ProductConfiguration &config() const {
            return m_config;
         }

        inline bool matches( const ProductConfiguration &conf ) const {
          return ( this->config().matches( conf ) );
        }

         PSMRTS_PROCESS_CATCHALL( "MeshShape" )

        protected:
          PsmrtsMeshData m_mesh;
          ProductConfiguration m_config;

          inline ProductConfiguration init_mesh( const std::string &name ) {
            ProductConfiguration config( name, PsmrtsMeshData().config() );
            config.add( ProductOption( "file", "mesh" ) );
            return ( config );
          }

          inline ProductConfiguration init_mesh( const PsmrtsMeshData &mesh, const std::string &name ) {
            ProductConfiguration config( name, mesh.config() );
            config.add( ProductOption( "file", "mesh" ) );
            return ( config );
          }
    };  

} // namespace psmrts

#endif
