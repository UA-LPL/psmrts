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

      /**
       * @brief Ellipsoid Ray Trace Processor
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
        trace.trace().validate_lookdir();
        Eigen::Vector3d observer( trace.trace().observer() );
        Eigen::Vector3d lookdir( trace.trace().lookdir() );
        return (this->ray_trace( observer, lookdir, trace.trace() ) );
      }

      /**
       * @brief Ellipsoid Ray Trace Array Processor
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

#if 1
      /** Report no this feature is not available - not relevant to Ellipsoid format */
      inline bool process( PRQFacet &facet ) const {
         return ( m_model.get_facet( facet.trace(), facet.facet() ) );
      }
#endif

      /**
       * @brief Ellipsoid Photometric Trace Processor
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
       * @brief Ellipsoid Photometric Trace Array Processor
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
       * @brief Ellipsoid Features Processor
       * 
       * This method accepts a PRQFeatures, and stores into it all the 
       * relevant Ellipsoid information using JSON.
       * 
       * @param features PRQFeatures that holds tracer-relevant information
       *                  in a JSON format
       * @return true    If features were added successfully
       * @return false   If any issues during processing
       */
      inline bool process( PRQFeatures &features ) const {
        psmrts_json f_e;
        f_e["name"] = "ellisoid" ;
        f_e["product"] = "shapetracer" ;
        f_e["mesh"] = false ;
        f_e["radii"] = { 1, 2, 3 } ;
        features.add_feature( f_e );
        return ( true );
      }

      /**
       * @brief Ellipsoid Virtual Ray Trace Method
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
      EllipsoidTracerModel m_model;
  };

} // namespace psmrts

#endif
