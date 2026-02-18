/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef EllipsoidTracer_hpp
#define EllipsoidTracer_hpp

#include <string>

#include <Eigen/Geometry>

#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/core/products/ProductFeature.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductCart.hpp>
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
      using ProductInfo     = ProductSpecification::ProductInfo;
      using ProductFeatures = ProductSpecification::ProductFeatures;

      EllipsoidTracer( ) : PsmrtsProduct( "default", "ellipsoid" ), 
                           m_radii{ 1.0, 1.0, 1.0 },
                           m_config( init_config( "default", { 1.0 }, "sphere" ) ) { }
      EllipsoidTracer( const double radius,
                       const std::string &name = "sphere") :
                       PsmrtsProduct( name, "sphere" ),
                       m_radii{ radius, radius, radius },
                       m_config( init_config( name, { radius}, "sphere" ) ) { }
      EllipsoidTracer( const double a, const double c,
                       const std::string &name = "spheroid") :
                       PsmrtsProduct( name, "spheroid" ), 
                       m_radii{ a, a, c },
                       m_config( init_config( name, { a, c}, "spheroid" )  ) { }     
      EllipsoidTracer( const double a, const double b, const double c,
                       const std::string &name = "ellipsoid") :
                       PsmrtsProduct( name, "ellipsoid" ),
                       m_radii{ a, b, c },
                       m_config( init_config( name, { a, b, c}, "ellipsoid" ) )  { }     
      EllipsoidTracer( const Eigen::Vector3d &radii,
                       const std::string &name = "ellipsoid" ) : 
                       PsmrtsProduct( name, "ellipsoid" ),
                       m_radii{ radii[0], radii[1],radii[2] },
                       m_config( init_config( name, { radii[0], radii[1], radii[2] }, "ellipsoid" ) ) { }
      EllipsoidTracer( const ProductCart &processed_cart ){
        this->set_name( processed_cart.name() );
        this->set_type( "ellipsoid" );            
        this->create( processed_cart );
      }                         
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
    
      static inline ProductSpecification product_specifications() {
        ProductInfo  info( "ellipsoid", { 
                                 ProductOption( "name", "ellipsoid"),
                                 ProductOption( "product", "tracer"),
                                 ProductOption( "description", "Ellipsoid, spheroid and sphere ray tracer") } );
        ProductFeature product( "tracer", {
                                 ProductOption( "name", "tracer" ),
                                 ProductOption( "type", "string" ),
                                 ProductOption( "description", "Describe the product type: ellipsoid, spheroid or sphere"),
                                 ProductOption( "status", "optional" ),
                                 ProductOption( "default", "ellipsoid" ) ,
                                 ProductOption( "valid", { "ellipsoid", "spheroid", "sphere" } ) } );                                 
        ProductFeature radii( "radii", {
                                 ProductOption( "name", "radii"),
                                 ProductOption( "type", "double"),
                                 ProductOption( "description", "Radius values of the object: 1, 2 or 3 double values"),
                                 ProductOption( "status", "required"),
                                 ProductOption( "aliases", "radius" ) } );
        ProductFeature model( "name", {
                                 ProductOption( "name", "name"),
                                 ProductOption( "type", "string"),
                                 ProductOption( "description", "Name of the ellipsoid model"),
                                 ProductOption( "status", "optional"),
                                 ProductOption( "aliases", "model" ) } );                                 

        // This validates the JSON structure and provides product info to callers
        return ( ProductSpecification( info, { product, radii, model } ) );
      }

      inline const ProductConfiguration &config() const {
        return ( m_config );
      }

      inline bool matches( const ProductConfiguration &conf ) const {
        return ( this->config().matches( conf ) );
      }


      /** Report all remaining features not available */
      PSMRTS_PROCESS_CATCHALL( "EllipsoidTracer" )

    private:
      double               m_radii[3];
      ProductConfiguration m_config;

      void create( const ProductCart &cart );


      inline ProductConfiguration init_config( const std::string &name, 
                                               const std::initializer_list<double> radii,
                                               const std::string &model = "ellipsoid"  ) {

        ProductConfiguration config( "ellipsoid" );
        config.add( ProductOption( "tracer", model ) );
        config.add( ProductOption( "radii",  radii ) );
        config.add( ProductOption( "name",   name ) );

        return ( config );
      }

  };

} // namespace psmrts

#endif
