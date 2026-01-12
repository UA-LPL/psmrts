#ifndef BulletTracer_hpp
#define BulletTracer_hpp

#include <string>
#include <memory>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/ProductFeature.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/algorithms/TracingBasics.hpp>

namespace psmrts  {

  /**
   * @brief Bullet ShapeModel
   * 
   * 
   */
  class BulletTracer : public PsmrtsProduct {
    public:
      using ProductInfo   = ProductSpecification::ProductInfo;
      using FeatureOption = ProductFeature::FeatureOption;
      using FeatureList   = ProductFeature::FeatureOptionList;

      BulletTracer( );
      BulletTracer( const PsmrtsShape &shape );
      virtual ~BulletTracer();

      double maximum_radius() const;

      double minimum_radius() const;

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
        return ( algorithms::process_basic_trace( *this, trace ) );
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
        return ( algorithms::process_basic_trace_array( *this, tracelist ) );
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
        return ( algorithms::process_basic_facet( *this, facet ) );
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
        return ( algorithms::process_basic_photometric_trace( *this, trace_p ) );
      }

      /**
       * @brief Bullet Photometric Trace Array Processor
       * 
       * This method accepts a PRQPhotometricTraceArray, which represents
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
        return ( algorithms::process_basic_photometric_trace_array( *this, tracelist ) );
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
      
      // These are the tracing and facet implementations in the source class
      bool ray_trace( PsmrtsRayTrace &ray ) const;
      bool get_facet(  const PsmrtsRayTrace &ray, 
                       PsmrtsRayTrace::FacetDatum &facet) const;
            
      static inline ProductSpecification product_specifications() {
        ProductInfo  info( "bullet", { 
                                 FeatureOption( "name", "bullet"),
                                 FeatureOption( "product", "tracer"),
                                 FeatureOption( "description", "The Bullet Physics ray tracing system specification") } );
        ProductFeature bvh( "bullet_optimize_bvh", {
                                 FeatureOption( "name", "bullet_optimize_bvh"),
                                 FeatureOption( "type", "bool"),
                                 FeatureOption( "description", "Use optimized bounding volume hierachy (BVH) when created"),
                                 FeatureOption( "status", "optional"),
                                 FeatureOption( "default", "false"),
                                 FeatureOption( "valid", {"true", "1", "yes", "false", "0", "no"} ) } );
        ProductFeature cmp( "bullet_compressed", {
                                 FeatureOption( "name", "bullet_compressed"),
                                 FeatureOption( "type", "bool"),
                                 FeatureOption( "description", "Compress Bullet data during construction"),
                                 FeatureOption( "status", "optional"),
                                 FeatureOption( "default", "false"),
                                 FeatureOption( "valid", {"true", "1", "yes", "false", "0", "no"} ) } );

        // This validates the JSON structure and provides product info to callers
        return ( ProductSpecification( info, { bvh, cmp } ) );
      }

      inline const PsmrtsShape &shape() const {
        return ( m_shape );
      }

      /** Return the current product configuration */
      inline const ProductConfiguration &config() const {
        return ( m_configured );
      }

      inline bool matches( const ProductConfiguration &conf ) const {
        if ( this->shape().matches( conf ) && this->config().matches( conf ) ) {
          return ( true );
        }

        return ( false );
      }


      /** Report all remaining features not available - e.g., PRQFacet not relevant to Ellipsoid format */
      PSMRTS_PROCESS_CATCHALL( "BulletTracer" )


    private:
      class BulletTracerImpl;
      std::shared_ptr<BulletTracerImpl> m_model;
      PsmrtsShape                       m_shape;
      ProductConfiguration              m_configured;
  };

} // namespace psmrts

#endif
