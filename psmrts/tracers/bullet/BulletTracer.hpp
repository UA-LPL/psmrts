#ifndef BulletTracer_hpp
#define BulletTracer_hpp

#include <string>

#include "private/PsmrtsBulletWorldModel.hpp"
#include "private/BulletTracerModel.hpp"
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/algorithms/TracingBasics.hpp>

namespace psmrts  {
  /**
   * @brief Bullet ShapeModel
   * 
   * 
   */
  class BulletTracer {
    public:
     BulletTracer( ) {  }
     BulletTracer( const bullet::PsmrtsBulletWorldModel &bt_model) :
                        m_model( bt_model ) { }
      virtual ~BulletTracer() { }

      /** Return the name of the shape file */
      inline const std::string &name() const {
        return ( m_model.shapefile() );
      }

      /**
       * @brief Bullet Ray Trace Processor
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
        return ( algorithms::process_basic_trace( m_model, trace ) );
      }

      /**
       * @brief Bullet Ray Trace Array Processor
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
        return ( algorithms::process_basic_trace_array( m_model, tracelist ) );
      }

      /**
       * @brief Bullet Facet Processor
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
        return ( algorithms::process_basic_facet( m_model, facet ) );
      }

      /**
       * @brief Bullet Photometric Trace Processor
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
        return ( algorithms::process_basic_photometric_trace( m_model, trace_p ) );
      }

      /**
       * @brief Bullet Photometric Trace Array Processor
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
        return ( algorithms::process_basic_photometric_trace_array( m_model, tracelist ) );
      }

      /**
       * @brief Bullet Features Processor
       * 
       * This method accepts a PRQFeatures, and stores into it all the 
       * relevant Bullet information using JSON.
       * 
       * @param features PRQFeatures that holds tracer-relevant information
       *                  in a JSON format
       * @return true    If features were added successfully
       * @return false   If any issues during processing
       */
      inline bool process( PRQFeatures &features ) const {
        psmrts_json f_e;
        f_e["name"]        = "bullet";
        f_e["product"]     = "tracer";
        f_e["mesh"]        = true;
        f_e["optimizebvh"] = false;
        f_e["vectortype"]  = { "double", "float" };
        features.add_feature( f_e );
        return ( true );
      }

      /**
       * @brief Bullet Ray Trace Method
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
      inline bool ray_trace( const Eigen::Vector3d &observer,
                              const Eigen::Vector3d &lookdir,
                              PsmrtsRayTrace &ray ) const {
        // this->local_tracker()++;
        return ( this->ray_trace( ray.reset( observer, lookdir ) ) );
      }
      
      inline bool ray_trace( PsmrtsRayTrace &ray ) const {
        // Let the model do it!
        return ( m_model.ray_trace( ray ) );
      }
            
      static inline ProductSpecification product_specifications() {
        char text[] = R"(
        {
          "name": "bullet",
          "product": "tracer",
          "type": "tracer",
          "description": "The Bullet Physics ray tracing system specification",
          "driver": {
            "name": "bullet",
            "type": "system"
          },
          "features": [
            {
              "name": "bullet_optimize_bvh",
              "type": "bool",
              "description": "Use optimized bounding volume hierachy (BVH) when created",
              "status": "optional",
              "default": "false",
              "valid": ["true", "1", "yes", "false", "0", "no"]
            },
            {
              "name": "bullet_compressed",
              "type": "bool",
              "description": "Compress Bullet data during construction",
              "status": "optional",
              "default": "false",
              "valid": ["true", "1", "yes", "false", "0", "no"]
            },            
            {
              "name": "bullet_thread_safety",
              "type": "bool",
              "description": "Utilize thread locking before Bullet ray traces are run",
              "status": "optional",
              "default": "false",
              "valid": ["true", "1", "yes", "false", "0", "no"]
            }
          ]       
        } )";

        // This validates the JSON structure and provides product info to callers
        return ( ProductSpecification( "bullet", "tracer", "tracer", json_utils::parse_json_string( text )));
      }

      /** Report all remaining features not available - e.g., PRQFacet not relevant to Ellipsoid format */
      PSMRTS_PROCESS_CATCHALL( "BulletTracer" )


    protected:
      bullet::BulletTracerModel  m_model;
  };

} // namespace psmrts

#endif
