#ifndef EllipsoidTracerModel_hpp
#define EllipsoidTracerModel_hpp

#include <exception>
#include <string>
#include <vector>
#include <mutex>

#include <cspice/SpiceUsr.h>

// Define the expected type before 
using EllipsoidModel = psmrts::EllipsoidTracerModel;
#include "../EllipsoidTracer.hpp"

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
     EllipsoidTracerModel( ) {  }
     EllipsoidTracerModel( const double *radii,
                           const std::string &source = "ellipsoid" ) : 
                          {  }

      virtual ~EllipsoidTracerModel() { }

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
      bool ray_trace( const double *observer, 
                      const double *lookdir,
                      double *xyz, double *normal ) const {

        SpiceBoolean found;
        (void) surfpt_c( observer, lookdir 
                         m_radii[0], m_radii[1], m_radii[2],
                         xyz, &found );

        bool hasHit = ( SPICETRUE == found );
        if (hasHit ) {
          this->normal( xyz, normal );
        }

        // Returns intercept state
        return ( hasHit );
      }

      void get_radii( double *radii ) const {
        radii[0] = m_radii[0];
        radii[1] = m_radii[1];
        radii[2] = m_radii[2];
        return;
      }

    private:
      double      m_radii[3];
      std::string m_name;

    /** Retuns the vector normal of the input point */
    void compute_normal( const double *point,
                         const double *normal ) const {  
      (void) surfnm_c( m_radii[0], m_radii[1], m_radii[2], point, normal );
      return ( );
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
  bool check_naif_errors( const bool b_reset = true,
                          const bool throw_on_error = true ) const {

    // Check for an error condition                                  
    if ( !failed_c() ) return ( false );

    // Reset the system
    std::string naif_error = get_naif_error_msg();
    if ( b_reset ) {
      reset_c();
    }

    if ( throw_on_error ) {
      throw std::runtime_error( "*** NAIF::Error - " + naif_error + " ***" );
    }

    return ( true );
  }

  };

} // namespace psmrts

#endif
