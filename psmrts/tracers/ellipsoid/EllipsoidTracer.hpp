#pragma once
#include <string>

#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/ProductSpecification.hpp>
#include "private/EllipsoidTracerModel.hpp"

namespace psmrts  {
  /**
   * @brief Ellipsoid ShapeModel supports spheres, spheroids and ellipsiods
   * 
   * In this hybrid implementation, EllipsoidModel must provide an init method
   * constructor, a destructor method that takes a three element Eigen::Vector3d and an option
   * string name for the ellipsoid, i.e., 
   * init_ellipsoid_tracer_model( const Eigen::Vector3d &, const string &).
   * In addition, destruct_ellipsoid_tracer_model() must also be implemented
   * which is called in this ~EllisoidTracer() destructor.
   * 
   * The private implementation also must provide a method called ray_trace()
   * that accepts an observer location, look direction vector, 
   * both Eigen::Vector3d, and a PsmrtsRayTrace. Note that is up to the 
   * private implementation to handle threading/mutex locking as PSMRTS may
   * call these methods in a threaded environment. See the class that PSMRTS
   * uses for mutex locking: psmrts::PsmrtsThreadSafeCounter in 
   * PsmrtsUtilities.hpp.
   * 
   * See ./private for implementation details. 
   * 
   * @history 2025-08-12 Kris J. Becker - Restructured to use private
   *                       implementation 
   */
  class EllipsoidTracer {

    public:
     EllipsoidTracer( ) : m_model( 1.0, 1.0, 1.0 ) {  }
     EllipsoidTracer( const double a, const double b, const double c,
                      const std::string &source = "ellipsoid") :
                      m_model( a, b, c, source) { }     
     EllipsoidTracer( const Eigen::Vector3d &radii,
                      const std::string &source = "ellipsoid" ) : 
                      m_model( radii.data(), source) { }   
        
      virtual ~EllipsoidTracer() { }

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
        f_e["mesh"] = false;

        double radii[3];
        m_model.get_radii( &radii[0] );
        f_e["radii"] = { radii[0], radii[1], radii[2] } ;

        features.add_feature( f_e );
        return ( true );
      }

      /**
       * @brief Ellipsoid Ray Trace Method
       * 
       * Deriving classes must implement this method as is specified for 
       * shape models.
       * 
       * It must include designated values necessary for a Ray Trace (observer, look direction),
       * return true if trace results in an intercept, and store the relevant result data into
       * a PsmrtsRayTrace object. The Ray Trace observer and lookdir vectors are reset with
       * the incoming arguments.
       * 
       * @param observer Location of the observer (s/c) relative to the target body
       * @param lookdir  Look direction of the ray from the observer to
       *                   trace for intersections
       * @param ray      PsmrtsRayTrace returns the results of the trace
       * @return true    If trace intercepts shape
       * @return false   If trace fails to intercept
       */
      inline bool ray_trace( const Eigen::Vector3d &observer,
                             const Eigen::Vector3d &lookdir,
                             PsmrtsRayTrace &ray ) const {

        // reset observer and lookdir with incoming arguments
        ray.reset( observer,lookdir );

        // Let the model do it!
        ray.datum().m_hit = m_model.ray_trace( observer.data(), lookdir.data(), 
                                               ray.datum().m_xyz.data(), 
                                               ray.datum().m_normal.data() );
        return ( ray.hasHit() );
      }
      
      static inline ProductSpecification product_specifications() {
        char text[] = R"(
        {
          "name": "ellipsoid",
          "product": "shapetracer",
          "type": "tracer",
          "description": "Ellipsoid ray tracing system specifications",
          "driver": {
            "name": "ellipsoid",
            "type": "system",
            "aliases": ["shapetracer"]
          },
          "parameters": [
            {
              "name": "ellipsoid_radii",
              "type": "list[double]",
              "description": "Radii of the ellipsoid in kilometers, in the order [a, b, c]",
              "status": "required",
              "default": [1.0, 1.0, 1.0]
            },
            {
              "name": "ellipsoid_source",
              "type": "string",
              "description": "Identifier or source for the ellipsoid - e.g., model or dataset",
              "status": "optional",
              "default": "ellipsoid"
            }
          ]       
        } )";

        // This validates the JSON structure and provides product info to callers
        return ( ProductSpecification( "ellipsoid", "tracer", "shapetracer", json_utils::parse_json_string( text )));
      }

      /** Report all remaining features not available - e.g., PRQFacet not relevant to Ellipsoid format */
      PSMRTS_PROCESS_CATCHALL( "EllipsoidTracer" )

    private:
      EllipsoidTracerModel m_model;   
  };

} // namespace psmrts

