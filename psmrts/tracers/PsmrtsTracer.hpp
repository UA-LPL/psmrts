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

  /**
   * @brief PsmrtsTracer container for all PSMRTS tracers
   * 
   * This class contains all the active/supported tracers in the PSMRTS system.
   * All the tracer business is in the ProductProcessDispath, a generic process
   * dispatch container class that uses templated process( Product T ) methods
   * to execute the requested operation.
   * 
   * Any one of the supported tracers can be passed to the constructor. Its
   * instance is copied into the ProductProcessDispatch class. Each tracer
   * implements all the process( T ) method it supports. The design of the
   * process dispatcher actually accepts any call made to this object, where
   * non-existant process( T ) methods are trapped and an error is recorded in
   * the corresponding T class. The T class must contain a PRQRequest base class
   * to properly handle this feature.
   * 
   * To use this class, the following sequence can be used:
   * @code 
   *   psmrts::PsmrtsTracer tracer( psmrts::PsmrtsTracer::sphere( 100 ) );
   *   psmrts::PRQRayTrace ray( observer, lookdir );
   *   bool status = tracer.process( ray ); // == ray.isValid() = ray.trace().hasHit()
   *   if ( true == status ) {
   *     // Trace successfully intercepts surface
   *   }
   *   elseif ( false == ray.was_invoked() ) {
   *      // Indicates the PRQ has no process(PRQRayTrace) method
   *   }
   *   elseif ( ray.error_count() > 0 ) {
   *      // An error was encountered and can be retrieved using
   *      // ray.errors_to_string()     
   *   }
   * @endcode
   * 
   * See PsmrtsRequest.hpp and ProductProcessDispatch.hpp for details.
   */
  class PsmrtsTracer : public ProductProcessDispatch< MissingProcessRequestHandler,
                                                      EllipsoidTracer, 
                                                      BulletTracer, 
                                                      NaifDskTracer> {
    public:
      using Tracer = ProductProcessDispatch::ProductType;
      using UIDType = PsmrtsUID::UIDType;

      PsmrtsTracer( ) : ProductProcessDispatch (  ) {  }
      PsmrtsTracer( const Tracer &tracer,
                    const std::string &name = "tracer" ) : 
                    ProductProcessDispatch( tracer ) {  }
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
