#ifndef EllipsoidTracerModel_hpp
#define EllipsoidTracerModel_hpp

#include <exception>
#include <string>
#include <vector>
#include <mutex>

#include <Eigen/Geometry>
#include <cspice/SpiceUsr.h>

namespace psmrts  {
  /**
   * @brief Private class implemeting ellipsiud tracing
   * 
   * This private class provides a generic interface to ray tracing systems
   * that can be used in the PSMRTS system. These method function declarations
   * defines the minimum set of functions that the ellipsid PSMRTS-compatible ray
   * tracer must implement.
   * 
   */
  class EllipsoidTracerModel {
    public:
     EllipsoidTracerModel( ) { 
        m_radii[0] = 0.0;
        m_radii[1] = 0.0;
        m_radii[2] = 0.0;
        m_name = "ellipsoid";
     }
     EllipsoidTracerModel( const double a, const double b, const double c,
                           const std::string &source = "ellipsoid") { 
        m_radii[0] = a;
        m_radii[1] = b;
        m_radii[2] = c;
        m_name = source;
     }     
     EllipsoidTracerModel( const Eigen::Vector3d &radii,
                           const std::string &source = "ellipsoid" ) { 
       this->set_radii( radii.data() );
       m_name = source;                    
     }
     EllipsoidTracerModel( const double *radii,
                           const std::string &source = "ellipsoid" ) { 
        this->set_radii( radii );
        m_name = source;                    
      }

      virtual ~EllipsoidTracerModel() { }


      inline const std::string &name() const {
        return ( m_name );
      }

      inline double maximum_radius() const {
        return ( std::max( std::max( m_radii[0], m_radii[1]), m_radii[2] ) );
      }

      inline double minimum_radius() const {
        return ( std::min( std::min( m_radii[0], m_radii[1]), m_radii[2] ) );
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
      inline bool ray_trace( const double *observer, 
                      const double *lookdir,
                      double *xyz, double *normal ) const {

        SpiceBoolean found;
        (void) surfpt_c( observer, lookdir,
                         m_radii[0], m_radii[1], m_radii[2],
                         xyz, &found );

        bool hasHit = ( SPICETRUE == found );
        if (hasHit ) {
          this->compute_normal( xyz, normal );
        }

        // Returns intercept state
        return ( hasHit );
      }

      inline void get_radii( double *radii ) const {
        radii[0] = m_radii[0];
        radii[1] = m_radii[1];
        radii[2] = m_radii[2];
        return;
      }

    private:
      double      m_radii[3];
      std::string m_name;

    inline void set_radii( const double *radii ) {
      m_radii[0] = radii[0];
      m_radii[1] = radii[1];
      m_radii[2] = radii[2];
      return;
    }

    /** Retuns the vector normal of the input point */
    inline void compute_normal( const double *point,
                                double *normal ) const {  
      (void) surfnm_c( m_radii[0], m_radii[1], m_radii[2], point, normal );
      return;
    }


    /**
     * @brief Check for NAIF errors with behavior control
     * 
     * This function will check for a NAIF error and take requested action.
     * 
     * @see get_naif_error_msg()
     * 
     * @param b_reset        If an error has occurred, reset the error system
     * @param throw_on_error Throw a runtime_error if an error occured
     * @return true          If no errror occurs
     * @return false         If an error occured
     */
    inline bool check_naif_errors( const bool b_reset = true,
                            const bool throw_on_error = true ) const {

      // Check for an error condition                                  
      if ( !failed_c() ) return ( false );

      // Reset the system
      const int NAIF_ERROR_STRING_SIZE = 2000;
      SpiceChar errmsg[NAIF_ERROR_STRING_SIZE];
      getmsg_c("LONG", NAIF_ERROR_STRING_SIZE, errmsg );

      std::string naif_errors( errmsg );
      if ( b_reset ) {
        reset_c();
      }

      if ( throw_on_error ) {
        throw std::runtime_error( "*** NAIF::Error - " + naif_errors + " ***" );
      }

      return ( true );
    }

  };

} // namespace psmrts

#endif
