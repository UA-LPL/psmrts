#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>

#include <psmrts_c.h>

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

  // Confirm number of arguments. We expect one argument (the obj filename)
  // after the executable name in argv
  if ( argc < 2 ) {
    const char *exename = strrchr( argv[0], '/' );
    if ( exename ) {
      ++exename;
    }
    else {
      exename = argv[0];
    }

    std::cerr << "Usage: " << exename << std::endl
              << "       obj filename is missing"
              << std::endl;

    return ( 1 ); // error
  }

  // retrieve obj file name from argv[1]
  std::string objfile = argv[1];

  // create and validate bullet tracer
  printf( "\n***creating/validating bullet tracer from input obj file %s\n",
          objfile.c_str() );

  PSMRTS_Tracer *bulletTracer = psmrts_create_bullet( objfile.c_str() );
  if ( !psmrts_tracer_valid( bulletTracer ) ) {
    //  printf("\n*** PSMRTS-C - create errors:\n%s\n", psmrts_tracer_error_string( bulletTracer ) );
    printf("\n*** PSMRTS-C - errors: create bullet tracer failed\n exiting..." );
    exit ( 1 );
  }

  PSMRTS_RayTrace *sunray = nullptr;
  PSMRTS_Vector3d xyz, raypt, normal, llr, sunpos, sundir;
  double slant_d, raypt_d, radius_pt;
  double emission, incidence, phase;

  // create ray traces and add to ray trace array
  printf( "\n***creating ray traces and adding to ray trace array\n" );

  PSMRTS_Vector3d observer1;
  observer1.longitude = psmrts_degrees_to_radians( 45.0 );
  observer1.latitude  = psmrts_degrees_to_radians( 45.0 );
  observer1.radius    = 3000.0;  // Maxiumum radius of input shape from API

  printf( "\n  observer1 lon (d), lat (d), radius (km): %lf, %lf, %lf\n",
         45.0, 45.0, observer1.radius );

  // convert observer from lon, lat, radius to xyz
  observer1 = psmrts_lonlatrad_to_xyz_d( &observer1 );
  PSMRTS_Vector3d lookdir1 = psmrts_negate( &observer1 );

  // create ray trace 'ray1' from observer and look direction vector
  PSMRTS_RayTrace *ray1 = psmrts_create_ray( &observer1, &lookdir1 );
  ray1 = psmrts_ray_trace( ray1, bulletTracer );

  PSMRTS_Vector3d observer2 = psmrts_vector3d( 46.0, 46.0, 3000.0 );

  printf( " observer2 lon (d), lat (d), radius (km): %lf, %lf, %lf\n",
          46.0, 46.0, observer2.radius );

  // convert observer2 from lon, lat, radius to xyz
  observer2 = psmrts_lonlatrad_to_xyz_d( &observer2 );
  PSMRTS_Vector3d lookdir2  = psmrts_negate( &observer2 );

  // create ray trace 'ray2' from observer and look direction vector
  PSMRTS_RayTrace *ray2 = psmrts_create_ray( &observer2, &lookdir2 );
  ray2 = psmrts_ray_trace( ray2, bulletTracer );

  // create ray trace vector and add traces
  PSMRTS_TraceArray *tracearray = psmrts_create_trace_array();

  psmrts_trace_array_add_trace( tracearray, ray1 );
  psmrts_trace_array_add_trace( tracearray, ray2 );

  // loop over trace array and output ray content if trace has a hit
  printf("\nLooping over traces in trace array...\n");
  size_t ntraces = psmrts_trace_array_size( tracearray );
  for ( size_t s = 0; s < ntraces; s++ ) {
    const PSMRTS_RayTrace *r = psmrts_trace_array_get_trace( tracearray, s );

    if ( psmrts_ray_has_hit( r ) == PSMRTS_FALSE ) {
      printf("*** ray does not hit\n");
      continue;
    }

    // ray has hit, output content
    printf("\n*** ray %zu has a hit!\n", s+1);
    xyz        = psmrts_ray_xyz( r );             // intercept in x,y,z
    llr        = psmrts_xyz_to_lonlatrad_d( &xyz ); // intercept in lon, lat, radius
    raypt      = psmrts_ray_raypt( r );
    normal     = psmrts_ray_normal( r );
    slant_d    = psmrts_ray_intercept_slant_distance( r );
    raypt_d    = psmrts_length( &raypt );
    radius_pt  = psmrts_length( &xyz );

    printf( "\n              trace surface intercept (xyz):  %lf,  %lf,  %lf\n", xyz.x, xyz.y, xyz.z ) ;
    printf( "              trace surface intercept (llr):  %lf,  %lf,  %lf\n", llr.x, llr.y, llr.z ) ;
    printf( "          vector along look direction (xyz): %lf, %lf, %lf\n", raypt.x, raypt.y, raypt.z ) ;
    printf( "   normal vector at surface intercept (xyz):  %lf,  %lf,  %lf\n", normal.x, normal.y, normal.z );
    printf( "        slant distance at surface intercept:  %lf\n", slant_d );
    printf( " length of look direction vector to surface:  %lf\n", raypt_d );
    printf( "    target radius at surface intercept (km):  %lf\n", radius_pt );

    // Use sunpos to get observational geometry
    sunpos = psmrts_vector3d( 30000, 100000, 200000 );
    sundir = psmrts_subtract( &xyz, &sunpos );
    printf( "\n                               sun position:  %lf,  %lf,  %lf\n", sunpos.x, sunpos.y, sunpos.z );
    printf( "                              sun direction: %lf,  %lf, %lf\n", sundir.x, sundir.y, sundir.z );

    // Trace from sun position to surface intercept point
    sunray = psmrts_ray_trace( psmrts_create_ray( &sunpos, &sundir ), bulletTracer );
    if ( psmrts_ray_has_hit( sunray ) ) {
      printf( "\n***sunray has a hit!\n" );
      emission  = psmrts_emission( r );
      incidence = psmrts_incidence( r, sunray );
      phase     = psmrts_phase( r, sunray );
      double ed = psmrts_radians_to_degrees(emission);
      double id = psmrts_radians_to_degrees(incidence);
      double pd = psmrts_radians_to_degrees(phase);

      printf("\nphotometric angles\n");
      printf("                          e, i, p (radians):  %lf  %lf  %lf\n", emission, incidence, phase);
      printf("                          e, i, p (degrees): %lf %lf %lf\n", ed, id, pd);
    }
    else {
      printf( "\n*** PSMRTS-C - Trace from sun failed!\n" );
    }
  }
/*
  // resetting ray trace array and add more traces
  printf("\nResetting ray trace array...\n\n");
//  psmrts_trace_array_clear( tracearray );

  observer.longitude = psmrts_degrees_to_radians( 47.0 );
  observer.latitude = psmrts_degrees_to_radians( 47.0 );
  observer.radius = 3000.0;  // Maxiumum radius of input shape from API

  printf( "\n  observer lon (d), lat (d), radius (km): %lf, %lf, %lf\n",
         47.0, 47.0, observer.radius );

  // convert observer from lon, lat, radius to xyz
  observer = psmrts_lonlatrad_to_xyz_d( &observer );
  lookdir  = psmrts_negate( &observer );
  psmrts_ray_set_observation( &observer, &lookdir, ray );

  // Trace the ray on the input mesh obj
  ray = psmrts_ray_trace( ray, bulletTracer );

  observer1.longitude = psmrts_degrees_to_radians( 48.0 );
  observer1.latitude = psmrts_degrees_to_radians( 48.0 );
  observer1.radius = 3000.0;  // Maxiumum radius of input shape from API

  printf( " observer1 lon (d), lat (d), radius (km): %lf, %lf, %lf\n",
         48.0, 48.0, observer1.radius );

  // convert observer from lon, lat, radius to xyz
  observer1 = psmrts_lonlatrad_to_xyz_d( &observer1 );
  lookdir1  = psmrts_negate( &observer1);
  psmrts_ray_set_observation( &observer1, &lookdir1, ray1 );

  // Trace the ray on the input mesh obj
  ray1 = psmrts_ray_trace( ray1, bulletTracer );

  psmrts_trace_array_add_trace( tracearray, ray );
  psmrts_trace_array_add_trace( tracearray, ray1 );

  // loop again over trace array and output ray content if trace has a hit
  printf("\nLooping over traces in trace array...\n");
  ntraces = psmrts_trace_array_size( tracearray );
  for ( size_t s = 0; s < ntraces; s++ ) {
      const PSMRTS_RayTrace *r = psmrts_trace_array_get_trace( tracearray, s );

      if ( psmrts_ray_has_hit( r ) == PSMRTS_FALSE ) {
          printf("*** ray does not hit\n");
          continue;
      }

      // ray has hit, output content
      printf("\n*** ray %zu has a hit!\n", s+1);
      xyz        = psmrts_ray_xyz( r );             // intercept in x,y,z
      llr        = psmrts_xyz_to_lonlatrad_d( &xyz ); // intercept in lon, lat, radius
      raypt      = psmrts_ray_raypt( r );
      normal     = psmrts_ray_normal( r );
      slant_d    = psmrts_ray_intercept_slant_distance( r );
      raypt_d    = psmrts_length( &raypt );
      radius_pt  = psmrts_length( &xyz );

      printf( "\n              trace surface intercept (xyz):  %lf,  %lf,  %lf\n", xyz.x, xyz.y, xyz.z ) ;
      printf( "              trace surface intercept (llr):  %lf,  %lf,  %lf\n", llr.x, llr.y, llr.z ) ;
      printf( "          vector along look direction (xyz): %lf, %lf, %lf\n", raypt.x, raypt.y, raypt.z ) ;
      printf( "   normal vector at surface intercept (xyz):  %lf,  %lf,  %lf\n", normal.x, normal.y, normal.z );
      printf( "        slant distance at surface intercept:  %lf\n", slant_d );
      printf( " length of look direction vector to surface:  %lf\n", raypt_d );
      printf( "    target radius at surface intercept (km):  %lf\n", radius_pt );

      // Use sunpos to get observational geometry
      sunpos = psmrts_vector3d( 30000, 100000, 200000 );
      sundir = psmrts_subtract( &xyz, &sunpos );
      printf( "\n                               sun position:  %lf,  %lf,  %lf\n", sunpos.x, sunpos.y, sunpos.z );
      printf( "                              sun direction: %lf,  %lf, %lf\n", sundir.x, sundir.y, sundir.z );

      // Trace from sun position to surface intercept point
      sunray = psmrts_ray_trace( psmrts_create_ray( &sunpos, &sundir ), bulletTracer );
      if ( psmrts_ray_has_hit( sunray ) ) {
          printf( "\n***sunray has a hit!\n" );
          emission  = psmrts_emission( r );
          incidence = psmrts_incidence( r, sunray );
          phase     = psmrts_phase( r, sunray );
          double ed = psmrts_radians_to_degrees(emission);
          double id = psmrts_radians_to_degrees(incidence);
          double pd = psmrts_radians_to_degrees(phase);

          printf("\nphotometric angles\n");
          printf("                          e, i, p (radians):  %lf  %lf  %lf\n", emission, incidence, phase);
          printf("                          e, i, p (degrees): %lf %lf %lf\n", ed, id, pd);
      }
      else {
          printf( "\n*** PSMRTS-C - Trace from sun failed!\n" );
      }
  }
*/
  // free objects
  psmrts_free_ray( ray1 );
  psmrts_free_ray( ray2 );
  psmrts_free_ray( sunray );
  psmrts_free_tracer( bulletTracer );

  return ( 0 );
}
