/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#include <string>

#include "../EllipsoidTracer.hpp"

#include <cspice/SpiceUsr.h>


namespace psmrts  {
 

  bool EllipsoidTracer::ray_trace( PsmrtsRayTrace &ray ) const {

    // Let the model do it!
    ray.datum().m_hit = this->ray_trace( ray.observer().data(), 
                                            ray.lookdir().data(), 
                                            ray.datum().m_xyz.data(), 
                                            ray.datum().m_normal.data() );
    return ( ray.hasHit() );
  }

  bool EllipsoidTracer::ray_trace( const double *observer, 
                                 const double *lookdir,
                                 double *xyz, 
                                 double *normal ) const {

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


  /** Retuns the vector normal of the input point */
  void EllipsoidTracer::compute_normal( const double *point, double *normal ) const {  
    (void) surfnm_c( m_radii[0], m_radii[1], m_radii[2], point, normal );
    return;
  }

} // namespace psmrts

