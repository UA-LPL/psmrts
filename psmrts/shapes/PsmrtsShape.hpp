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

#include <psmrts/core/ProductProcessDispatch.hpp>
#include <psmrts/shapes/dsk/DskShape.hpp>
#include <psmrts/shapes/obj/ObjShape.hpp>
#include <psmrts/shapes/ply/PlyShape.hpp>
#include <psmrts/shapes/mesh/MeshShape.hpp>


namespace psmrts {

  class PsmrtsShape : public ProductProcessDispatch< MissingProcessRequestHandler, 
                                                     DskShape, 
                                                     ObjShape, 
                                                     PlyShape,
                                                     MeshShape> {
    public:
      using Shape = ProductProcessDispatch::ProductType;
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
        return ( !std::holds_alternative<MissingProcessRequestHandler>( m_product ) );
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
  };
} // namespace psmrts

#endif
