#ifndef PsmrtsShape_hpp
#define PsmrtsShape_hpp

#include <exception>
#include <string>
#include <variant>

#include <Eigen/Geometry>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsCache.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <psmrts/core/ProductProcessDispatch.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/ProductOption.hpp>

#include <psmrts/shapes/dsk/DskShape.hpp>
#include <psmrts/shapes/obj/ObjShape.hpp>
#include <psmrts/shapes/ply/PlyShape.hpp>


namespace psmrts {

  class PsmrtsShape : public ProductProcessDispatch< MissingProcessRequestHandler, DskShape, ObjShape, PlyShape>, 
                      public PsmrtsProduct {
    public:
      using Shape = ProductProcessDispatch::ProductType;
      using UIDType = PsmrtsUID::UIDType;

      PsmrtsShape( ) : PsmrtsProduct("shape") {  }
      PsmrtsShape( const Shape &shape ) : ProductProcessDispatch( shape ), PsmrtsProduct("shape") {  }
      virtual ~PsmrtsShape() { }

      inline bool isValid() const {
        return ( !std::holds_alternative<MissingProcessRequestHandler>( m_product ) );
      }

      inline const PsmrtsMeshData get_mesh() const {
        inline static const PsmrtsMeshData empty_mesh{};
        const auto visitor = overload{
          [](const DskShape &dsk) { return ( dsk.get_mesh() ); },            
          [](const ObjShape &obj) { return ( obj.get_mesh() ); },            
          [](const PlyShape &ply) { return ( ply.get_mesh() ); },            
          [](auto &&args) { return ( empty_mesh ); } // Default lambda for all other types
        };
        return std::visit(visitor, m_product);
      }

    private:
      inline static PsmrtsCache<UIDType, PsmrtsShape> m_cache{};

  };
} // namespace psmrts

#endif
