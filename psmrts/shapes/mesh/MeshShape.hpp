/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef MeshShape_hpp
#define MeshShape_hpp

#include <string>

#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsMeshData.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductCart.hpp>


namespace psmrts {
    /**
     * @brief Generic mesh shape product
     * 
     */
    class MeshShape : public PsmrtsProduct {
      public:
        using ProductInfo     = ProductSpecification::ProductInfo;
        using ProductFeatures = ProductSpecification::ProductFeatures;
        using SharedMeshData  = std::shared_ptr<PsmrtsMeshData>;


        MeshShape() : PsmrtsProduct( "mesh", "shape", "mesh"),
                      m_mesh( make_shared_copy( PsmrtsMeshData() ) ),
                      m_config( init_mesh( "mesh") )  { }
        MeshShape( const PsmrtsMeshData &mesh, 
                   const std::string &name = "mesh") : 
                   PsmrtsProduct( name, "shape", "mesh" ),
                   m_mesh( make_shared_copy( mesh ) ),
                   m_config( init_mesh(  mesh, name ) ) { }
        MeshShape( const ProductCart &processed_cart ) :
                   PsmrtsProduct( processed_cart.name(), "shape", "mesh" ) {
          this->create( processed_cart );
        }                      
        virtual ~MeshShape() = default;
         
        static inline ProductSpecification product_specifications() {
          ProductInfo  info( "mesh", { 
                        ProductOption( "name",        "mesh"),
                        ProductOption( "product",     "shape"),
                        ProductOption( "description", "Provides support for a genric user defined shape" ) } );
          ProductFeature product( "shape", {
                                  ProductOption( "name", "shape" ),
                                  ProductOption( "type", "string" ),
                                  ProductOption( "description", "Describe the product type" ),
                                  ProductOption( "status", "optional" ),
                                  ProductOption( "default", "mesh" ),
                                  ProductOption( "valid", "mesh" ) } );                          
          ProductFeature source( "mesh_name", {
                                  ProductOption( "name", "mesh_name"),
                                  ProductOption( "type", "string"),
                                  ProductOption( "description", "Name of mesh data" ),
                                  ProductOption( "status", "required"),
                                  ProductOption( "aliases", { "mesh", "source", "shapefile" } ) } );
          ProductFeature dtype( "mesh_data_type", {
                                  ProductOption( "name", "mesh_data_type"),
                                  ProductOption( "type", "string"),
                                  ProductOption( "description", "Type of mesh vector data provided" ),
                                  ProductOption( "status", "optional"),
                                  ProductOption( "aliases", "data_type" ), 
                                  ProductOption( "default", "double" ),
                                  ProductOption( "valid", { "double", "float" } ) });

          // This validates the JSON structure and provides product info to callers
          return ( ProductSpecification( info, { product, source, dtype } ) );             
        }

        inline const PsmrtsMeshData &get_mesh() const {
           return ( *m_mesh );
        }

        inline const ProductConfiguration &config() const {
           return m_config;
        }

        PSMRTS_PROCESS_CATCHALL( "MeshShape" )

      protected:
        SharedMeshData       m_mesh;
        ProductConfiguration m_config;

        inline ProductConfiguration init_mesh( const std::string &name ) {
          ProductConfiguration config( name, PsmrtsMeshData().config() );
          config.add( ProductOption( "shape", "mesh" ) );
          config.add( ProductOption( "mesh_name", name ) );
          return ( config );
        }

        inline ProductConfiguration init_mesh( const PsmrtsMeshData &mesh, const std::string &name ) {
          ProductConfiguration config( name, mesh.config() );
          config.add( ProductOption( "shape", "mesh" ) );
          config.add( ProductOption( "mesh_name", name) );
          return ( config );
        }

        void create( const ProductCart &cart ) {

            std::string name_t = cart.configuration().name();

            // Check for valid shape type
            if (cart.error_count() > 0 ) {
              std::string mess = "MeshShape::create(" + name_t + 
                                ") has config/spec processing errors: \n" +
                                  cart.errors_to_string();
              throw std::runtime_error( mess );          
            }

            ProductConfiguration v_conf = cart.configuration();
            if (cart.error_count() > 0 ) {
              std::string mess = "MeshShape::create(" + name_t + ") has errors: " +
                                  cart.errors_to_string();
              throw std::runtime_error( mess );          
            }

            m_config = cart.configuration();
            if ( m_config.contains( "shape" ) ) {
              if ( m_config.find( "shape" ).to_string() != "mesh" ) {
                std::string mess = "MeshShape::create() - shape type must be \"mesh\""
                                  " but found " + m_config.find("shape").to_string();
                throw std::runtime_error( mess );
              }
            }

            // Set the name of the mesh
            this->set_name( m_config.find( "mesh_name" ).to_string() );

            // Create an empty mesh
            if ( m_config.contains( "mesh_data_type") && 
                ( m_config.find( "mesh_data_type" ).to_string() == "float" ) ) {
              m_mesh = make_shared_copy( PsmrtsMeshData( PsmrtsVector3i(), PsmrtsVector3f() ) );
            }
            else {
              m_mesh = make_shared_copy( PsmrtsMeshData( PsmrtsVector3i(), PsmrtsVector3d() ) );
            }

            m_config.add_metadata( ProductOption( "shape_uid", PsmrtsUID::to_string( this->uid() ) ) );

            return;
          }          
    };  

} // namespace psmrts

#endif
