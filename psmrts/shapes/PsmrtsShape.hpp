/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef PsmrtsShape_hpp
#define PsmrtsShape_hpp

#include <string>
#include <variant>
#include <optional>

#include <Eigen/Geometry>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsCache.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductModelDispatch.hpp>
#include <psmrts/core/products/ProductVoidVariant.hpp>
#include <psmrts/shapes/dsk/DskShape.hpp>
#include <psmrts/shapes/obj/ObjShape.hpp>
#include <psmrts/shapes/ply/PlyShape.hpp>
#include <psmrts/shapes/mesh/MeshShape.hpp>


namespace psmrts {

  class PsmrtsShape : public ProductModelDispatch< ProductVoidVariant, 
                                                     DskShape, 
                                                     ObjShape, 
                                                     PlyShape,
                                                     MeshShape> {
    public:
      using Shape    = ProductModelDispatch::Model;
      using Variants = Shape;  // Standardization for ProductMaker
      using UIDType = PsmrtsUID::UIDType;

      PsmrtsShape( ) : ProductModelDispatch ( ProductVoidVariant( "void" ) ) {
        init_product();
      }

      PsmrtsShape( const std::string &filename ) : 
                    ProductModelDispatch ( ProductVoidVariant( filename ) ) { 

        std::string fext_t = psmrts_tolower( psmrts_file_extension( filename ) );
        if ( "obj" == fext_t ) {
          this->set_model( ObjShape( filename ) );
        }
        else if ( "ply" == fext_t ) {
          this->set_model( PlyShape( filename ) );
        }
        else if ( "bds" == fext_t ) {
          this->set_model( DskShape( filename ) );
        }
        else {
          std::string badfile = "Shape(" + fext_t + ") format is not supported!";
          throw std::runtime_error( badfile );
        }
        init_product();
      }

      PsmrtsShape( const Shape &shape ) : ProductModelDispatch( shape ) { 
        init_product();                    
      }

      virtual ~PsmrtsShape() { }

      inline bool isValid() const {
        return ( !std::holds_alternative<ProductVoidVariant>( m_model ) );
      }

      inline const PsmrtsProduct &product() const {
        return ( m_product ); 
      }

      inline const std::string &name() const {
        return ( this->product().name() ); 
      }  

      inline const std::string &type() const {
        return ( this->product().type() ); 
      } 

      inline const std::string &model() const {
        return ( this->product().model() ); 
      }  

      inline const UIDType &uid() const {
        return ( this->product().uid() ); 
      }      

      inline const PsmrtsMeshData &get_mesh() const {
        static const PsmrtsMeshData empty_mesh{};
        const auto visitor = overload{
          [](const DskShape &dsk) -> const PsmrtsMeshData&   { return ( dsk.get_mesh() ); },            
          [](const ObjShape &obj) -> const PsmrtsMeshData&   { return ( obj.get_mesh() ); },            
          [](const PlyShape &ply) -> const PsmrtsMeshData&   { return ( ply.get_mesh() ); },            
          [](const MeshShape &mesh) -> const PsmrtsMeshData& { return ( mesh.get_mesh() ); },            
          [](auto &&args) -> const PsmrtsMeshData& { return ( empty_mesh ); } // Default lambda for all other types
        };
        return std::visit(visitor, m_model );
      }

      inline ProductSpecification specs() const {
        const auto visitor = overload {            
                  []( auto &&shape ) -> ProductSpecification {
                       return ( shape.product_specifications() ); 
                  }
        };
       
        return ( std::visit(visitor, m_model ) ); 
      } 

      inline const ProductConfiguration &config() const {
        const auto visitor = overload{            
                  [](auto &&shape ) -> const ProductConfiguration & {
                       return ( shape.config() ); 
                  }
        };
       
        return ( std::visit(visitor, m_model) ); 
      }        

       inline bool matches( const ProductConfiguration &conf ) const {
        return ( this->config().matches( conf ) );
      }
      
      inline double minimum_radius() const {
        return this->get_mesh().minimum_radius();
      }
      
      inline double maximum_radius() const {
        return this->get_mesh().maximum_radius();
      }

    private:
      PsmrtsProduct m_product;

      inline void init_product() {
        const auto visitor = overload{            
          [](auto &&shape ) ->PsmrtsProduct {
              return ( shape.product() ); 
          }
        };
        
        m_product = std::visit(visitor, m_model );
      }
              
  };

  /**
   * @brief Shape getter PRQ for tracers that have one
   * 
   * This class is a PRQ request functor that is intended to return the
   * PsmrtsShape contained in a PsrmtsTracer. Only tracer variants that have a
   * shape need implement a process( PRQShape& ) method that will call the
   * set_shape() method to return the active shape within the tracer.
   * 
   */
  class PRQShape : public PsmrtsRequest {
    public:

      PRQShape() : PsmrtsRequest( "PRQShape" ),
                   m_shape( std::nullopt ) { }
      virtual ~PRQShape() { }
 
      using PsmrtsRequest::name;
      using PsmrtsRequest::run_count;
      using PsmrtsRequest::was_invoked;
      using PsmrtsRequest::error_count;
      using PsmrtsRequest::errors;

      inline bool isValid() const {
        return ( m_shape.has_value() );
      }

      inline void set_shape( const PsmrtsShape &shape ) {
        m_shape.emplace( shape );
      }

      inline PsmrtsShape shape() const {
        if ( m_shape.has_value() ) {
          return ( m_shape.value() );
        }
        // If no shape is present
        return ( PsmrtsShape() );
      }

    private:
      std::optional<PsmrtsShape> m_shape;
  };

} // namespace psmrts

#endif
