#pragma once
#include <string>

#include <Eigen/Geometry>

#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/ProductSpecification.hpp>
#include <psmrts/algorithms/TracingBasics.hpp>

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
  class EllipsoidTracer : public PsmrtsProduct {

    public:
      EllipsoidTracer( ) : PsmrtsProduct( "ellipsoid", "tracer" ), 
                           m_radii{ 1.0, 1.0, 1.0 } {  }
      EllipsoidTracer( const double radius,
                      const std::string &source = "sphere") :
                      PsmrtsProduct( source, "tracer" ),
                      m_radii{ radius, radius, radius } { 
        init_sphere( 1.0 , source );                        
      }
      EllipsoidTracer( const double a, const double c,
                      const std::string &source = "spheroid") :
                      PsmrtsProduct( source, "tracer" ),                      
                      m_radii{ a, a, c } {
        init_spheroid( a, c, source );                        

      }     
      EllipsoidTracer( const double a, const double b, const double c,
                      const std::string &source = "ellipsoid") :
                      PsmrtsProduct( source, "tracer" ),
                      m_radii{ a, b, c } { 
        init_ellipsoid( a, b, c, source );                    

      }     
      EllipsoidTracer( const Eigen::Vector3d &radii,
                      const std::string &source = "ellipsoid" ) : 
                      PsmrtsProduct( source, "tracer" ),
                      m_radii{ radii[0], radii[1],radii[2] } { 
        init_ellipsoid( radii[0], radii[1], radii[2], source );                    
      }
      // EllipsoidTracer( const ProductConfiguration &config_p ) :
      //                  PsmrtsProduct( "none", "tracer" ), 
      //                  m_radii{0, 0, 0} { 
      //  init_config( config_p );
      // }                         
        
      virtual ~EllipsoidTracer() = default;
 

      /** Returns value of a */
      const double &a() const {
        return ( m_radii[0] );
      }

      /** Returns value of b */
      const double &b() const {
        return ( m_radii[1] );
      }

      /** Returns value of c */
      const double &c() const {
        return ( m_radii[2] );
      }
      
      /** Return the maximum radius of the ellipsoid */
      inline double maximum_radius() const {
        return ( std::max( std::max( m_radii[0], m_radii[1]), m_radii[2] ) );
      }

      /** Return the minimum radius of the ellipsoid  */
      inline double minimum_radius() const {
        return ( std::min( std::min( m_radii[0], m_radii[1]), m_radii[2] ) );
      }


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
        return ( psmrts::algorithms::process_basic_trace( *this, trace ) );
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
        return ( psmrts::algorithms::process_basic_trace_array( *this, tracelist ) );
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
        return ( psmrts::algorithms::process_basic_photometric_trace(*this, trace_p ) );
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
        return ( psmrts::algorithms::process_basic_photometric_trace_array( *this, tracelist ) );
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

        f_e["radii"] = { m_radii[0], m_radii[1], m_radii[2] } ;

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
        return ( this->ray_trace( ray.reset( observer, lookdir ) ) );
      }

 
    /**
     * @brief Ray Trace method for Ellipsoid Shape - PsmrtsRayTrace Result
     * 
     * The main method used to run individual body-fixed ray traces from 
     * an observer point and a look direction vector. The origin of the
     * "observer" vector is the origin of the planet body and extends
     * outward, presumeably, beyond the maximum radius of the surface in
     * this model. From that point, is the origin of the "lookdir" vector
     * from which to trace for an intersection with the shape model
     * surface. 
     * 
     * The PsmrtsRayTrace class contains the results of the ray trace and can
     * be used in subsequent operations.
     * 
     * @param observer Location of the observer (s/c) relative to the
     *                   center of the target body
     * @param lookdir  Look direction of the ray from the observer to
     *                   trace for intersections
     * @param ray      PsmrtsRayTrace returns the results of the trace
     * @return true    If the trace intercepts the shape
     * @return false   If no ray trace intercept was found
     */
      bool ray_trace( PsmrtsRayTrace &ray ) const;

      /** IMPL function to compute the rays */
      bool ray_trace( const double *observer, const double *lookdir,
                      double *xyz, double *normal ) const;

      /** Retuns the vector normal of an input point */
      void compute_normal( const double *point, double *normal ) const;
    
    
      static inline ProductSpecification sphere_product_spec() {
        char text[] = R"(
        {
          "name": "sphere",
          "product": "tracer",
          "type": "tracer",
          "description": "Sphere ray tracing system specifications",
          "driver": {
            "name": "sphere",
            "type": "system",
            "aliases": ["tracer"]
          },
          "features": [
            {
              "name": "sphere_radius",
              "type": "double",
              "description": "Radius of the sphere in kilometers",
              "status": "required"
            },
            {
              "name": "sphere_source",
              "type": "string",
              "description": "Identifier or source of the sphere - e.g., naif IAU, planet",
              "status": "optional",
              "default": "sphere"
            }
          ]       
        } )";

        // This validates the JSON structure and provides product info to callers
        return ( ProductSpecification( "sphere", "tracer", "tracer", json_utils::parse_json_string( text )));
      }

      static inline ProductSpecification spheroid_product_spec() {
        char text[] = R"(
        {
          "name": "spheroid",
          "product": "tracer",
          "type": "tracer",
          "description": "spheroid ray tracing system specifications",
          "driver": {
            "name": "spheroid",
            "type": "system",
            "aliases": ["tracer"]
          },
          "features": [
            {
              "name": "spheroid_radii",
              "type": "double",
              "description": "Radii (a,c) of the spheroid in kilometers",
              "status": "required"
            },
            {
              "name": "spheroid_source",
              "type": "string",
              "description": "Identifier or source of the spheroid - e.g., naif IAU, planet",
              "status": "optional",
              "default": "spheroid"
            }
          ]       
        } )";

        // This validates the JSON structure and provides product info to callers
        return ( ProductSpecification( "spheroid", "tracer", "tracer", json_utils::parse_json_string( text )));
      }

      static inline ProductSpecification ellipsoid_product_spec() {
        char text[] = R"(
        {
          "name": "ellipsoid",
          "product": "tracer",
          "type": "tracer",
          "description": "ellipsoid ray tracing system specifications",
          "driver": {
            "name": "ellipsoid",
            "type": "system",
            "aliases": ["tracer"]
          },
          "features": [
            {
              "name": "ellipsoid_radii",
              "type": "double",
              "description": "Radii (a,b,c) of the ellipsoid in kilometers",
              "status": "required"
            },
            {
              "name": "ellipsoid_source",
              "type": "string",
              "description": "Identifier or source of the ellipsoid - e.g., naif IAU, planet",
              "status": "optional",
              "default": "ellipsoid"
            }
          ]       
        } )";

        // This validates the JSON structure and provides product info to callers
        return ( ProductSpecification( "ellipsoid", "tracer", "tracer", json_utils::parse_json_string( text )));
      }

      inline ProductConfiguration &config() const {
        return ( m_configured );
      }

      /** Report all remaining features not available */
      PSMRTS_PROCESS_CATCHALL( "EllipsoidTracer" )

    private:
      double      m_radii[3];
      ProductConfiguration m_configured;



      inline ProductConfiguration init_sphere(const double radius, const std::string &source ) {
        auto config =  ProductConfiguration( source, { ProductOption( "tracer", "sphere" ),
                                                       ProductOption( "sphere_radius", radius ),
                                                       ProductOption( "sphere_source", source ) } );
        return (config );
      }

      inline ProductConfiguration init_spheroid( const double a, const double c, 
                                 const std::string &source  ) {
        auto config =  ProductConfiguration( source, { ProductOption( "tracer", "spheroid" ),
                                                       ProductOption( "spheroid_radii", { a, c } ),
                                                       ProductOption( "spheriod_source", source ) } );
        return ( config );
      }
      
      inline ProductConfiguration init_ellipsoid(const double a, const double b, const double c,
                                 const std::string &source  ) {
        auto config = ProductConfiguration( source, { ProductOption( "tracer", "ellipsoid" ),
                                                      ProductOption( "ellipsoid_radii", { a, b, c } ),
                                                      ProductOption( "ellipsoid_source", source ) } );
        return ( config );
      }

      inline ProductConfiguration init_config( const ProductConfiguration &config,
                                               const std::string &name = "" ) {

        std::string source = ( name.size() >  0 ) ? name : config.name();
        
        // Check for sphere first
        ProductConfiguration config_c;
        if ( EllipsoidTracer::sphere_product_spec().extract( config, config_c ) ) {
          // Add parameter extraction and complete sphere configuration
          return ( config_c );
        }

        // Check for a spheroid condig
        if ( EllipsoidTracer::spheroid_product_spec().extract( config, config_c ) ) {
          // Add parameter extraction and complete spheroid configuration
          return ( config_c );
        }        

        // Check for an ellipsoid 
        if ( EllipsoidTracer::ellipsoid_product_spec().extract( config, config_c ) ) {
          // Add parameter extraction and complete ellipsoid configuration
          return ( config_c );
        }

        // Return a null config with 0 options
        return ( ProductConfiguration( name ) );
      }

  };

} // namespace psmrts

