#ifndef NaifDskShapeTracer_hpp
#define NaifDskShapeTracer_hpp

#include <string>

#include <NaifDskTracerModel.hpp>
#include <PsmrtsRequest.hpp>

namespace psmrts  {
  /**
   * @brief NAIF DSK ShapeModel
   * 
   * 
   */
  class NaifDskShapeTracer {
    public:
      NaifDskShapeTracer( ) {  }
      NaifDskShapeTracer( const naif::DskKernelModel &dsktracer ) : 
                          m_model( dsktracer ) {  }
      NaifDskShapeTracer( const std::string &dsk ) : 
                          m_model( dsk ) {  }
      virtual ~NaifDskShapeTracer() { }

      inline bool process ( PRQRayTrace &trace ) const {
        Eigen::Vector3d observer ( trace.trace().observer() );
        Eigen::Vector3d lookdir ( trace.trace().lookdir() );
        return ( this->ray_trace( observer, lookdir, trace.trace() ) );
      }

      inline bool process ( PRQRayTraceArray &tracelist ) const {
        size_t n_good = 0;
        for ( auto &trace : tracelist.traces() ) {
          if ( this->process( trace ) ) {
            n_good++;
          }
        }
        
        return ( n_good > 0 );
      }


      inline bool process( PRQFacet &facet ) const {
         return ( m_model.get_facet( facet.trace(), facet.facet() ) );
      }

      inline bool process( PRQPhotometricTrace &trace_p ) const {
        if ( this->process( trace_p.observer() ) ) {
          if ( trace_p.compute_sun_lookdir() ) {
            return ( this->process( trace_p.sunpos() ) );
          }
        }

        return ( false );
      }

       inline bool process ( PRQPhotometricTraceArray &tracelist ) const {
        size_t n_good = 0;
        for ( auto &trace : tracelist.traces() ) {
          if ( this->process( trace ) ) {
            n_good++;
          }
        }
        
        return ( n_good > 0 );
      }

      inline bool process( PRQFeatures &features ) const {
        psmrts_json f_e;
        f_e += { "name" , "naifdsk" };
        f_e += { "product" , "shapetracer" };
        f_e += { "mesh" , false };
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
       NaifDskTracerModel  m_model;
  };

} // namespace psmrts

#endif
