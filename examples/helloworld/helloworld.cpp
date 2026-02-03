/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>


#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>


inline std::string to_string( const Eigen::Vector3d &v ) {
  std::ostringstream s;
  s << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
  return ( s.str() );
}

/**
 * @brief Main function of the psmrts_capi_features application.
 *
 * This application serves as a demonstration of the Bullet shape tracing sytem
 * in the PSMRTS library.
 *
 * @param argc Number of command line arguments.
 * @param argv Array of strings representing command line arguments.
 * @return Integer indicating exit status of the application. Returns 0 upon
 *         successful execution, non-zero otherwise.
 */
int main( int argc, char *argv[] ) {

  std::cout << "\nPSMRTS Hello World!" << std::endl;

  psmrts::PsmrtsTracer ellipsoid = psmrts::PsmrtsTracer::ellipsoid( 1.0, 2.0, 3.0, "myellipsoid" );
  Eigen::Vector3d observer = psmrts::lonlatrad_to_xyz_d( { 45.0, 45.0, 10.0} );
  Eigen::Vector3d lookdir = - observer;
  std::cout << "\nTracing " << ellipsoid.name() << " at coordinate = { 45.0, 45.0, 10.0 }." << std::endl;

  psmrts::PRQRayTrace ray_t( observer, lookdir );
  bool status = ellipsoid.process( ray_t );
  
  if ( !status ) {
    std::cout << "Trace " << ellipsoid.name() << " at " << to_string( observer ) << " failed!" << std::endl;
    return ( 1 );
  }

  // Report the trace result
  std::cout << "Trace from observer " << to_string ( observer ) 
            << ", lookdir " << to_string( lookdir ) << " succeeded!" << std::endl;
  std::cout << "Surface Intercept point(lon,lat,radius):  " << to_string( psmrts::xyz_to_lonlatrad_d( ray_t.trace().xyz()) ) << "." << std::endl;

  return ( 0 );
}
