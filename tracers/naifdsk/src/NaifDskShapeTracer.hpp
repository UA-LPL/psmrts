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

      /**
       * @brief NAIF Dsk Ray Trace Processor
       * 
       * This method accepts a PRQRayTrace, which is instantiated with 
       * values necessary for a Ray Trace (observer, look direction), 
       * runs the trace, and saves the result back into the PRQRayTrace
       * object - accessible through its' trace() function. It returns
       * true if the trace resulted in an intercept.
       * 
       * @param trace   PRQRayTrace provides observer, lookdir values,
       *                  and stores trace result
       * @return true   If trace intercepts the shape
       * @return false  If no ray trace intercept was found
       */
      inline bool process ( PRQRayTrace &trace ) const {
        Eigen::Vector3d observer ( trace.trace().observer() );
        Eigen::Vector3d lookdir ( trace.trace().lookdir() );
        return ( this->ray_trace( observer, lookdir, trace.trace() ) );
      }

      /**
       * @brief NAIF Dsk Ray Trace Array Processor
       * 
       * This method accepts a PRQRayTraceArray, which represents
       * multiple PRQRayTraces in an array object, runs each trace,
       * and stores the results back into their associated PRQRayTraces
       * in the array.
       * 
       * If at least one of the traces results in an intercept, the
       * method returns true.
       * 
       * @param tracelist PRQRayTraceArray provides individual PRQRayTraces,
       *                    each holding both values and post-trace results.
       * @return true     If at least one trace intercepts the shape
       * @return false    If no trace intercepts were found
       */
      inline bool process ( PRQRayTraceArray &tracelist ) const {
        size_t n_good = 0;
        for ( auto &trace : tracelist.traces() ) {
          if ( this->process( trace ) ) {
            n_good++;
          }
        }
        
        return ( n_good > 0 );
      }

      /**
       * @brief NAIF Dsk Facet Processor
       * 
       * This method accepts a PRQFacet, usually instantiated with a ray
       * trace, and processes it - storing the facet associated with the 
       * trace's intercept back into the PRQFacet. The resulting facet
       * can be accessed using the PRQFacet's facet() function.
       * 
       * It returns true if the process results in a valid facet.
       * 
       * @param facet   PRQFacet provides desired ray trace, and stores
       *                  resulting facet data
       * @return true   If process results in valid facet intercept
       * @return false  If process fails to find facet/intercept
       */
      inline bool process( PRQFacet &facet ) const {
         return ( m_model.get_facet( facet.trace(), facet.facet() ) );
      }

      /**
       * @brief NAIF Dsk Photometric Trace Processor
       * 
       * This method accepts a PRQPhotometricTrace, which is instantiated with 
       * values necessary for a Photometric Trace (observer, look direction,
       * sun position), runs the traces relative to both observer/look direction
       * and the sun's position, and saves the result back into the PRQPhotometricTrace
       * object. It returns true if the trace resulted in both intercepting.
       * 
       * @param trace   PRQRayTrace provides observer, lookdir values,
       *                  and stores trace result
       * @return true   If observer/look direction and sun traces intercepts
       *                  the shape
       * @return false  If either does not intercept the shape
       */
      inline bool process( PRQPhotometricTrace &trace_p ) const {
        if ( this->process( trace_p.observer() ) ) {
          if ( trace_p.compute_sun_lookdir() ) {
            return ( this->process( trace_p.sunpos() ) );
          }
        }

        return ( false );
      }

       /**
       * @brief NAIF Dsk Photometric Trace Array Processor
       * 
       * This method accepts a PRQPhotomericTraceArray, which represents
       * multiple PRQPhotometricTraces in an array object, runs each trace,
       * and stores the results back into their associated PRQPhotometricTraces
       * in the array.
       * 
       * If at least one of the traces results in a photometrically appropriate 
       * intercept, the method returns true.
       * 
       * @param tracelist PRQPhotometricTraceArray provides individual PRQPhotometricTraces,
       *                    each holding both values and post-trace results.
       * @return true     If at least one of the traces intercepts the shape
       * @return false    If no appropriate trace intercepts were found
       */
       inline bool process ( PRQPhotometricTraceArray &tracelist ) const {
        size_t n_good = 0;
        for ( auto &trace : tracelist.traces() ) {
          if ( this->process( trace ) ) {
            n_good++;
          }
        }
        
        return ( n_good > 0 );
      }

       /**
       * @brief NAIF Dsk Features Processor
       * 
       * This method accepts a PRQFeatures, and stores into it all the 
       * relevant DSK information using JSON.
       * 
       * @param features PRQFeatures that holds tracer-relevant information
       *                  in a JSON format
       * @return true    If features were added successfully
       * @return false   If any issues during processing
       */
      inline bool process( PRQFeatures &features ) const {
        psmrts_json f_e;
        f_e += { "name" , "naifdsk" };
        f_e += { "product" , "shapetracer" };
        f_e += { "mesh" , true };
        features.add_feature( f_e );
        return ( true );
      }

      /**
       * @brief NAIF Dsk Virtual Ray Trace Method
       * 
       * Deriving classes must implement this method as is specified for 
       * shape models.
       * 
       * It must include designated values necessary for a Ray Trace (observer, look direction),
       * return true if trace results in an intercept, and store the relevant result data into 
       * a PsmrtsRayTrace object.
       * 
       * @param observer Location of the observer (s/c) relative to the target body
       * @param lookdir  Look direction of the ray from the observer to
       *                   trace for intersections
       * @param ray      PsmrtsRayTrace returns the results of the trace
       * @return true    If trace intercepts shape
       * @return false   If trace fails to intercept
       */
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
