#include <stdio.h>
#include <iostream>
#include <string>

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

  printf( "input obj file: %s\n", objfile.c_str() );

  PSMRTS_RayTrace *sunray = nullptr;
  PSMRTS_Vector3d xyz, raypt, normal, llr, sunpos, sundir;
  double slant_d, raypt_d, radius_pt;
  double emission, incidence, phase;

  printf( "\n***creating observer and lookdir vectors\n" );
  PSMRTS_Vector3d observer;
  observer.longitude = degrees_to_radians( 45.0 );
  observer.latitude = degrees_to_radians( 45.0 );
  observer.radius = 1.0;

  printf( "\n  observer lon (d), lat (d), radius (km): %lf, %lf, %lf\n", 45.0, 45.0, 1.0 );
  printf( "  observer lon (r), lat (r), radius (km):  %lf,  %lf, %lf\n", observer.longitude,
                                                                         observer.latitude,
                                                                         observer.radius );

  // convert observer from lon, lat, radius to xyz
  observer = psmrts_lonlatrad_to_xyz( &observer );
  PSMRTS_Vector3d lookdir  = psmrts_negate( &observer );

  printf( "\n   observer xyz (km):  %lf,  %lf,  %lf\n", observer.x, observer.y, observer.z) ;
  printf( "    lookdir xyz (km): %lf, %lf, %lf\n", lookdir.x, lookdir.y, lookdir.z );

  // create and validate bullet tracer
  printf( "\n***creating/validating bullet tracer from input obj file\n" );
  PSMRTS_Tracer *bulletTracer = psmrts_create_bullet( objfile.c_str() );
  if ( !psmrts_tracer_valid( bulletTracer ) ) {
//  printf("\n*** PSMRTS-C - create errors:\n%s\n", psmrts_tracer_error_string( bulletTracer ) );
    printf("\n*** PSMRTS-C - errors: create bullet tracer failed\n exiting..." );
    exit ( 1 );
  }

  // Trace a ray on the input mesh obj. Create a reusable ray structure
  // to minimize memory create/free overhead.
  printf("\n***creating ray trace on input mesh\n");
  PSMRTS_RayTrace *ray = psmrts_create_ray( &observer, &lookdir );
  ray = psmrts_ray_trace( ray, bulletTracer );

  // if trace has a hit, output ray content
  if ( psmrts_ray_has_hit( ray ) ) {
    printf( "\n***ray has a hit!\n" );

    /* Retrieve/calculate data from trace */
    xyz        = psmrts_ray_xyz( ray );           // intercept in x,y,z
    llr        = psmrts_xyz_to_lonlatrad( &xyz ); // intercept in lon, lat, radius
    raypt      = psmrts_ray_raypt( ray );
    normal     = psmrts_ray_normal( ray );
    slant_d    = psmrts_ray_intercept_slant_distance( ray );
    raypt_d    = psmrts_length( &raypt );
    radius_pt  = psmrts_length( &xyz );

    printf( "\n              trace surface intercept (xyz):  %lf,  %lf,  %lf\n", xyz.x, xyz.y, xyz.z ) ;
    printf( "              trace surface intercept (llr):  %lf,  %lf,  %lf\n", llr.x, llr.y, llr.z ) ;
    printf( "          vector along look direction (xyz): %lf, %lf, %lf\n", raypt.x, raypt.y, raypt.z ) ;
    printf( "   normal vector at surface intercept (xyz):  %lf,  %lf,  %lf\n", normal.x, normal.y, normal.z );
    printf( "        slant distance at surface intercept:  %lf\n", slant_d );
    printf( " length of look direction vector to surface:  %lf\n", raypt_d );
    printf( "    target radius at surface intercept (km): %lf\n", radius_pt );


    // Use sunpos to get observational geometry */
    sunpos = psmrts_vector3d( 300, 1000, 2000 );
    sundir = psmrts_subtract( &xyz, &sunpos );

    printf( "\n                               sun position:  %lf,  %lf,  %lf\n", sunpos.x, sunpos.y, sunpos.z );
    printf( "                              sun direction: %lf,  %lf, %lf\n", sundir.x, sundir.y, sundir.z );

    // Trace from sun position to surface intercept point
    sunray = psmrts_ray_trace( psmrts_create_ray( &sunpos, &sundir ), bulletTracer );
    if ( psmrts_ray_has_hit( sunray ) ) {
      printf( "\n***sunray has a hit!\n" );

      emission  = psmrts_emission( ray );
      incidence = psmrts_incidence( ray, sunray );
      phase     = psmrts_phase( ray, sunray );
      double ed = radians_to_degrees(emission);
      double id = radians_to_degrees(incidence);
      double pd = radians_to_degrees(phase);

      printf("\nphotometric angles\n");
      printf("                          e, i, p (radians):  %lf  %lf  %lf\n", emission, incidence, phase);
      printf("                          e, i, p (degrees): %lf %lf %lf\n", ed, id, pd);
    }
    else {
      printf( "\n*** PSMRTS-C - Trace from sun failed!\n" );
    }
  }

  // free objects
  psmrts_free_ray( ray );
  psmrts_free_ray( sunray );
  psmrts_free_tracer( bulletTracer );

  return ( 0 );
}
