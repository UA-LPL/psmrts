/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#include <stdio.h>

#include <psmrts/capi/psmrts_c.h>

int main( int argc, char *argv[] ) {

  printf("\n\nPSMRTS Tracer Shape\n");
  printf("PSMRTS Version: %s\n", psmrts_version() );

  if ( --argc != 2 ) {
    printf("\nUsage: %s <shape> <tracer>\n", <argv[0] );
    return ( 1 );
  }
  
  char *shape, *tracer;
  shape = argv[1];
  tracer = argv[2];
  PSMRTSShapeTracer *tracer;
  tracer  = *psmrts_load_shape( shape, tracer );
  if ( !tracer ) {
    printf("\nFailed to open %s with a %s tracer\n", shape, tracer );
    return ( 2 );
  }

  /* Run a trace at the poles and equator */
  PSMRTSRayTrace *ray;
  double scpos[3];
  double lookdir[3];
  double radius;

  radius = 1000.0;

  /* Construct position and look direction of ray */
  psmrts_lonlat_to_xyz( 40.0, 0.0, radius, scpos );
  psmrts_vector_scale( scpos, -1.0, lookdir );

  printf("\nSCPOS   = %g, %g, %g\n", scpos[0], scpos[1], scpos[2] );
  printf("\nLOOKDIR = %g, %g, %g\n", lookdir[0], lookdir[1], lookdir[2] );

  /* run the trace */
  ray = psmrts_ray_trace( tracer, scpos, lookdir );
  if ( psmrts_isvalid( ray ) == PSMRTS_TRUE ) {
    double xyz[3];
    psmrts_surface_xyz( ray, xyz );
    printf("\nXYZ = %f, %f, %d\n", xyz[0], xyz[1], xyz[2] );
  }
  else {
    printf("\nTrace failed!\n");
    return ( 3 );
  }

  psmrts_free( ray );
  psmrts_free( tracer );

  return ( 0 );
}
