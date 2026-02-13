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

#include <Eigen/Geometry>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsCache.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/ProductSpecification.hpp>

#include <psmrts/core/ProductProcessDispatch.hpp>
#include <psmrts/core/ProductVoidVariant.hpp>
#include <psmrts/shapes/dsk/DskShape.hpp>
#include <psmrts/shapes/obj/ObjShape.hpp>
#include <psmrts/shapes/ply/PlyShape.hpp>
#include <psmrts/shapes/mesh/MeshShape.hpp>


namespace psmrts {

  class PsmrtsShape : public ProductProcessDispatch< ProductVoidVariant, 
                                                     DskShape, 
                                                     ObjShape, 
                                                     PlyShape,
                                                     MeshShape> {
    public:
      using Shape    = ProductProcessDispatch::ProductType;
      using Variants = Shape;  // Standardization for ProductMaker
      using UIDType = PsmrtsUID::UIDType;

      PsmrtsShape( ) {  }
      PsmrtsShape( const std::string &filename ) {  
        std::string fext_t = psmrts_tolower( psmrts_file_extension( filename ) );
        if ( "obj" == fext_t ) {
          m_product = ObjShape( filename );
        }
        else if ( "ply" == fext_t ) {
          m_product = PlyShape( filename );
        }
        else if ( "bds" == fext_t ) {
          m_product = DskShape( filename );
        }
        else {
          std::string badfile = "Shape(" + fext_t + ") format is not supported!";
          throw std::runtime_error( badfile );
        }
      }
      PsmrtsShape( const Shape &shape ) :
                   ProductProcessDispatch( shape ) {  }
      virtual ~PsmrtsShape() { }

      inline bool isValid() const {
        return ( !std::holds_alternative<ProductVoidVariant>( m_product ) );
      }

      inline UIDType uid() const {
        const auto visitor = overload{            
                  [](auto &&shape ) -> UIDType {
                       return ( shape.uid() ); 
                  }
        };
       
        return ( std::visit(visitor, m_product ) ); 
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
        return std::visit(visitor, m_product);
      }

      inline ProductSpecification specs() const {
        const auto visitor = overload{            
                  [](auto &&shape ) -> ProductSpecification {
                       return ( shape.product_specifications() ); 
                  }
        };
       
        return ( std::visit(visitor, m_product ) ); 
      } 

      inline const ProductConfiguration &config() const {
        const auto visitor = overload{            
                  [](auto &&shape ) -> const ProductConfiguration & {
                       return ( shape.config() ); 
                  }
        };
       
        return ( std::visit(visitor, m_product ) ); 
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

  };
} // namespace psmrts

#endif
