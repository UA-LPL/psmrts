#ifndef PsmrtsTracer_hpp
#define PsmrtsTracer_hpp

#include <string>
#include <variant>

#include <Eigen/Geometry>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsCache.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/ProductProcessDispatch.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/bullet/BulletTracer.hpp>
#include <psmrts/tracers/ellipsoid/EllipsoidTracer.hpp>
#include <psmrts/tracers/naifdsk/NaifDskTracer.hpp>

namespace psmrts {

  class PsmrtsTracer : public ProductProcessDispatch< MissingProcessRequestHandler, EllipsoidTracer, BulletTracer, NaifDskTracer>, 
                       public PsmrtsProduct {
    public:
      using Tracer = ProductProcessDispatch::ProductType;
      using UIDType = PsmrtsUID::UIDType;

      PsmrtsTracer( ) : PsmrtsProduct("tracer") {  }
      PsmrtsTracer( const Tracer &tracer,
                    const std::string &name = "tracer" ) : 
                    ProductProcessDispatch( tracer ), 
                    PsmrtsProduct(name, "tracer") {  }
      virtual ~PsmrtsTracer() { }

      inline static PsmrtsTracer sphere( const double radius_km, const std::string &name="sphere" ) {
        Eigen::Vector3d radii( { radius_km, radius_km, radius_km } );
        return ( PsmrtsTracer( EllipsoidTracer( radii, name ), name ) );
      }

      inline static PsmrtsTracer spheroid( const double a_km, const double c_km, 
                                                const std::string &name="spheroid" ) {
        Eigen::Vector3d radii( { a_km, a_km, c_km} ); // testing
        return ( PsmrtsTracer( EllipsoidTracer( radii, name ), name ) );
      }

      inline static PsmrtsTracer ellipsoid( const double a_km,  const double b_km, const double c_km, 
                                                const std::string &name="ellipsoid" ) {
        Eigen::Vector3d radii( { a_km, b_km, c_km } );
        return ( PsmrtsTracer( EllipsoidTracer( radii, name ), name ) ); 
      }

      inline static PsmrtsTracer ellipsoid( const Eigen::Vector3d radii, 
                                                const std::string &name="ellipsoid" ) {
        return ( PsmrtsTracer( EllipsoidTracer( radii, name ), name ) ); 
      }

      inline static PsmrtsTracer bullet( const std::string &meshfile ) {
        return ( PsmrtsTracer( BulletTracer( PsmrtsShape( meshfile ) ) ) );
      }

      inline static PsmrtsTracer naifdsk( const std::string &dskfile ) {
        return ( PsmrtsTracer( NaifDskTracer( dskfile ), dskfile ) ); 
      }

      inline bool isValid() const {
        return ( !std::holds_alternative<MissingProcessRequestHandler>( m_product ) );
      }

  };

} // namespace psmrts

#endif
