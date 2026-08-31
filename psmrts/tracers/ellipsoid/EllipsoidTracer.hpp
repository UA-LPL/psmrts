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
#include <Eigen/Dense>

#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/core/products/ProductFeature.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductCart.hpp>
#include <psmrts/algorithms/TracingBasics.hpp>

namespace psmrts  {
  /**
   * @brief Ellipsoid ShapeModel supports spheres, spheroids and ellipsoids
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
   * The center of this ellipsoid is assumed to be {0, 0 0}.
   *
   * --------------------------------------------------------------------------- 
   *      This algorithm was developed with the assistance of Claude.
   * 
   * Numerically stable ray/ellipsoid intersection.
   *
   * PROBLEM
   * -------
   * The textbook approach transforms the ray into the ellipsoid's unit-sphere
   * space and solves:
   *
   *      A t^2 + B t + C = 0
   *      A = dot(d,d)
   *      B = 2*dot(o,d)
   *      C = dot(o,o) - 1
   *
   * where o, d are the (radii-scaled) ray origin and direction.
   *
   * This works fine when the origin is close to the ellipsoid. But when the
   * origin is very far away (e.g. a camera millions/billions of units from a
   * planet), |o| is huge, so B and C are huge, and the discriminant
   * B^2 - 4*A*C becomes the difference of two enormous, nearly-equal
   * floating point numbers -> catastrophic cancellation. Symptoms: missed
   * intersections, negative discriminants for rays that actually hit, and
   * hit points that are wrong by a significant fraction of the ellipsoid's
   * own size.
   *
   * FIX
   * ---
   * 1) Re-center the quadratic at the point on the infinite ray closest to
   *    the ellipsoid's center (parameter tm), and solve for the *offset* s
   *    from that point instead of solving directly at the (possibly huge)
   *    origin. The recentered point `om` has magnitude close to 1 (it's near
   *    the ellipsoid) regardless of how far away the original origin was, so
   *    B' and C' stay small and the cancellation disappears.
   * 2) Use the "Citardauq" / stable quadratic formula to compute the two
   *    roots without subtracting nearly-equal numbers a second time.
   * 3) Normalize the ray direction first so `t` is a clean world-space
   *    distance and A doesn't carry an arbitrary direction-length scale.
   *
   * LIMITS OF THIS FIX
   * -------------------
   * This removes the cancellation *inside the intersection math itself*.
   * It cannot fix precision lost *before* this function is called -- e.g.
   * if you represent both the camera and the ellipsoid center as huge
   * world-space coordinates and subtract them to get `rayOrigin - center`,
   * that subtraction can itself lose precision if both operands are huge
   * and comparable in size. For truly extreme scales (light-years from an
   * Earth-sized ellipsoid), combine this with camera-relative / floating
   * origin rendering (keep the ellipsoid center at/near the coordinate
   * origin, or use double-double / long double for the origin subtraction)
   * so that `rayOrigin - center` itself is computed accurately.
   *
   * All math here uses double precision. Swap `double` -> `long double`
   * throughout if you need extra headroom and your platform supports 80-bit
   * extended precision.
   * ---------------------------------------------------------------------------
   * 
   * @history 2025-08-12 Kris J. Becker - Restructured to use private
   *                       implementation 
   * @history 2026-07-27 Kris J. Becker - Rewrote with assistance from Claude
   *                       thus removing NAIF toolkit dependency and creating a
   *                       thread-safe version of the ellipsoid tracer
   */
  class EllipsoidTracer : public PsmrtsProduct {

    public:
      using ProductInfo     = ProductSpecification::ProductInfo;
      using ProductFeatures = ProductSpecification::ProductFeatures;

      EllipsoidTracer( ) : PsmrtsProduct( "ellipsoid", "tracer", "ellipsoid" ), 
                           m_radii{ 1.0, 1.0, 1.0 },
                           m_config( init_config( "ellipsoid", { 1.0 }, "ellipsoid" ) ) {  }
      EllipsoidTracer( const double radius,
                       const std::string &name = "sphere") :
                       PsmrtsProduct( name, "tracer", "sphere" ),
                       m_radii{ radius, radius, radius },  
                       m_config( init_config( name, { radius }, "sphere" ) ) { }
      EllipsoidTracer( const double a, const double c,
                       const std::string &name = "spheroid") :
                       PsmrtsProduct( name, "tracer", "spheroid" ), 
                       m_radii{ a, a, c },
                       m_config( init_config( name, { a, c }, "spheroid" )  ) { }     
      EllipsoidTracer( const double a, const double b, const double c,
                       const std::string &name = "ellipsoid") :
                       PsmrtsProduct( name, "tracer", "ellipsoid" ),
                       m_radii{ a, b, c },
                       m_config( init_config( name, { a, b, c }, "ellipsoid" ) )  { }     
      EllipsoidTracer( const Eigen::Vector3d &radii,
                       const std::string &name = "ellipsoid" ) : 
                       PsmrtsProduct( name, "tracer", "ellipsoid" ),
                       m_radii{ radii[0], radii[1], radii[2] },
                       m_config( init_config( name, { radii[0], radii[1], radii[2] }, "ellipsoid" ) ) { }
      EllipsoidTracer( const ProductCart &processed_cart ) :
                       PsmrtsProduct( processed_cart.configuration().name(), "tracer", "ellipsoid" ) {
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
      bool ray_trace( PsmrtsRayTrace &ray ) const {

        // Set up access to ray elements
        const Eigen::Vector3d &lookdir  = ray.lookdir();
        const Eigen::Vector3d &observer = ray.observer();
        auto &datum_r                   = ray.datum();

        double ldirlen_t = lookdir.norm();
        if ( ldirlen_t  == 0.0 ) {
           datum_r.m_hit = false;
           return ( false );          
        }

        Eigen::Vector3d lookdir_u = lookdir * ( 1.0 / ldirlen_t );
        Eigen::Vector3d lookdir_t = lookdir_u.array() / m_radii.array();  // d
        Eigen::Vector3d origin_t  = observer.array()  / m_radii.array();  // o

        double A_t = lookdir_t.dot( lookdir_t );
        if ( A_t <= 0.0 ) {
           datum_r.m_hit = false;
           return ( false );             
        }

        // Recenter at point on the ray closest to ellipsoid center
        double center_e          = -origin_t.dot( lookdir_t ) / A_t;      // tm
        Eigen::Vector3d center_m =  origin_t + ( lookdir_t * center_e );  // om

        double B_t = 2.0 * center_m.dot( lookdir_t );
        double C_t = center_m.dot( center_m ) - 1.0;

        double discriminant = ( B_t * B_t ) - 4.0 * A_t * C_t;
        if ( discriminant < 0.0 ) {
           datum_r.m_hit = false;
           return ( false );
        }  

      // Stable quadratic (Citardauq): avoids cancellation when picking roots.
        double d_sqrt = std::sqrt( discriminant );
        double q_t = -0.5 * (B_t + std::copysign( d_sqrt, B_t ) );

        double s0, s1;
        if (q_t != 0.0) {
          s0 = q_t / A_t;
          s1 = C_t / q_t;
        } else {
          s0 = s1 = 0.0;
        }
        if (s0 > s1) std::swap(s0, s1);

        datum_r.m_hit = true;
        double t0_l = center_e + s0;
        double t1_l = center_e + s1;

        // Convert back to original scale to get surface intercept point
        Eigen::Vector3d point_scaled = center_m + ( lookdir_t * s0 );
        datum_r.m_xyz = point_scaled.array() * m_radii.array();

        // Compute normal from scaled point
        Eigen::Vector3d normal_s = point_scaled.array() / m_radii.array();
        datum_r.m_normal = normal_s.normalized();

        datum_r.m_hit = true;
        ray.set_tracer_id( this->uid() );
        return ( true );
      }

      /**
       * @brief The EllipsoidTracer product specification data
       * 
       * This static method returns the product specification for the EllipsoidTracer.
       * 
       * @return ProductSpecification Configuration elements of ellipsoid tracer
       */
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
                                 ProductOption( "default", "ellipsoid" ),
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
                                 ProductOption( "default", "ellipsoid" ),
                                 ProductOption( "aliases", "model" ) } );                                 

        // This validates the JSON structure and provides product info to callers
        return ( ProductSpecification( info, { product, radii, model } ) );
      }

      inline const ProductConfiguration &config() const {
        return ( m_config );
      }

      /** Report all remaining features not available */
      PSMRTS_PROCESS_CATCHALL( "EllipsoidTracer" )

    private:
      Eigen::Vector3d      m_radii;  // Radii of ellipsoid
      ProductConfiguration m_config;

      /**
       * @brief Create an ellipsoid from a cart configuration
       * 
       * @param cart Provides the ellipsoid configuration
       */
      void create( const ProductCart &cart ) {
      
        std::string name_t = cart.configuration().name();

        // Check for valid shape type
        if ( cart.error_count() > 0 ) {
          std::string mess = "EllipsoidTracer::create(" + name_t + 
                            ") has config/spec processing errors: \n" +
                              cart.errors_to_string();
          throw std::runtime_error( mess );          
        }

        if ( !cart.isvalid() ) {
          std::string mess = "EllipsoidTracer::create(" + name_t + 
                            ") is invalid with " + 
                            std::to_string( cart.configuration().size() ) +
                            " config options and " +
                            std::to_string( cart.residual().size() ) +
                            " residual options";
          throw std::runtime_error( mess );          
        }

        ProductConfiguration v_conf = cart.configuration();
        std::string model = "ellipsoid";

        if ( v_conf.contains( "tracer" ) ) {
          model = v_conf.find( "tracer" ).to_string();
          std::vector<std::string> valid_s = { "ellipsoid", "spheroid", "sphere" };
          if ( std::find( valid_s.begin(), valid_s.end(), model) == valid_s.end() ) {
            std::string mess = "EllipsoidTracer::create() - tracer must be "
                              "\"ellipsoid\", \"spheroid\" or \"sphere\" "
                                " but found " + model;
            throw std::runtime_error( mess );
          }
        }

        std::vector<double> radii = OptionDoublesExtractor( v_conf.find( "radii" ) ).get_all();
        if ( ( radii.size() < 1 ) || (radii.size() > 3 ) ) {
          std::string mess = "EllipsoidTracer::create() - radii must have 1, 2 or 3 values"
                            " but got " + std::to_string( radii.size() );
          throw std::runtime_error( mess );
        }

        std::string name = "";
        if ( v_conf.contains( "name" ) ) {
          name = v_conf.find( "name" ).to_string();
        }

        if ( radii.size() == 1 ) {
          m_radii[0]  = radii[0];
          m_radii[1]  = radii[0];
          m_radii[2]  = radii[0];
          m_config = init_config( name, { radii[0] }, model );
        }
        else if ( radii.size() == 2 ) {
          m_radii[0]  = radii[0];
          m_radii[1]  = radii[0];
          m_radii[2]  = radii[1];
          m_config = init_config( name, { radii[0], radii[1] }, model);
        }
        else {
          m_radii[0]  = radii[0];
          m_radii[1]  = radii[1];
          m_radii[2]  = radii[2];
          m_config = init_config( name, { radii[0], radii[1], radii[2] }, model);
        }        

      }


      inline ProductConfiguration init_config( const std::string &name, 
                                               const std::initializer_list<double> radii,
                                               const std::string &model = "ellipsoid"  ) {               

        ProductConfiguration config( "ellipsoid" );
        config.add( ProductOption( "tracer", model ) );
        config.add( ProductOption( "radii",  radii ) );
        if ( name.length() > 0 ) config.add( ProductOption( "name",   name ) );

        // Check to ensure none of the radii are invalid
        for ( const double &r : radii ) {
          if ( r <= 0.0 ) {
            std::string mess = config.to_json().dump(-1);
            throw std::runtime_error( "***ERROR - Invalid radii for ellipsoid: " + mess );
          }
        } 
        
        config.add_metadata( ProductOption( "tracer_uid", PsmrtsUID::to_string( this->uid() ) ) );
        return ( config );
      }

  };

} // namespace psmrts

#endif
