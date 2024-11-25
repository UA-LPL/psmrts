#ifndef EllipsoidShapeTracer_hpp
#define EllipsoidShapeTracer_hpp

#include <string>

#include <EllipsoidTracerModel.hpp>
#include <PsmrtsRequest.hpp>

namespace psmrts  {
  /**
   * @brief Ellipsoid ShapeModel
   * 
   * 
   */
  class EllipsoidShapeTracer {
    public:
     EllipsoidShapeTracer( ) {  }
     EllipsoidShapeTracer( const Eigen::Vector3d &radii,
                           const std::string &source = "ellipsoid"  ) : 
                           m_model( radii, source ) { }     
      virtual ~EllipsoidShapeTracer() { }


      inline bool process ( PRQRayTrace &trace ) const {
        Eigen::Vector3d observer( trace.trace().observer() );
        Eigen::Vector3d lookdir( trace.trace().lookdir() );
        return (this->ray_trace( observer, lookdir, trace.trace() ) );
      }

#if 1
      /** Report no this feature is not available */
      inline bool process( PRQFacet &facet ) const {
         return ( m_model.get_facet( facet.trace(), facet.facet() ) );
      }
#endif

      inline bool process( PRQPhotometricTrace &trace_p ) const {
        if ( this->process( trace_p.observer() ) ) {
          if ( trace_p.compute_sun_lookdir() ) {
            return ( this->process( trace_p.sunpos() ) );
          }
        }

        return ( false );
      }

      inline bool process( PRQFeatures &features ) const {
        psmrts_json f_e;
        f_e["name"] = "ellisoid" ;
        f_e["product"] = "shapetracer" ;
        f_e["mesh"] = false ;
        f_e["radii"] = { 1, 2, 3 } ;
        features.add_feature( f_e );
        return ( true );
      }

      virtual bool ray_trace( const Eigen::Vector3d &observer,
                              const Eigen::Vector3d &lookdir,
                              PsmrtsRayTrace &ray ) const {
        // this->local_tracker()++;
        return ( m_model.ray_trace( observer, lookdir, ray ) );
      }
  
    protected:
      EllipsoidTracerModel m_model;
  };

} // namespace psmrts

#endif
