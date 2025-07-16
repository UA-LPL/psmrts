#include <stdio.h>
#include <string>

#include <PsmrtsRequest.hpp>
#include <PsmrtsUtilities.hpp>
#include <PsmrtsShapeTracer.hpp>

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
  if (argc < 2) {
    const char *exename = strrchr(argv[0], '/');
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

  printf("creating observer and lookdir vectors\n");

  PSMRTS_Vector3d observer;
  observer.longitude = psmrts::degrees_to_radians( 45.0 );
  observer.latitude = psmrts::degrees_to_radians ( 45.0 );
  observer.radius = 1.0; // observer should be 5.0, 5.0, 7.071068

  // output original observer in lon, lat, radius
  printf("\nobserver in lon, lat, radius: %lf %lf %lf\n", observer.longitude,
                                                          observer.latitude,
                                                          observer.radius);

  // scale vector by a factor of 10
  observer = psmrts_scale(&observer, 10.0);

  // convert vectors to rectangular
  observer = psmrts_lonlatrad_to_xyz( &observer );

  // lookdir is negated observer vector
  PSMRTS_Vector3d lookdir = psmrts_negate(&observer);

  // output scaled observer and lookdir vectors in x, y, z
  printf("             observer in xyz: %lf %lf %lf\n", observer.x, observer.y,
                                                        observer.z);
  printf("              lookdir in xyz: %lf %lf %lf\n", lookdir.x, lookdir.y,
                                                        lookdir.z);

  // create trace and add to array
  printf("\ncreating trace from observer and lookdir\n");
  PSMRTS_RayTrace *ray = psmrts_create_ray( &observer, &lookdir );

  // create trace array
  printf("\ncreating trace array\n");
  PSMRTS_TraceArray *tracearray = psmrts_create_trace_array();

  // add trace to array
  printf("\nadding trace to trace array\n");
  psmrts_trace_array_add_trace( tracearray, ray );

  // output # of traces in array
  printf("\n# of traces in array: %zu\n", psmrts_trace_array_size(tracearray) );

  // retrieve trace from array
  printf("\nretrieving trace from array\n");
  const PSMRTS_RayTrace *retrievedtrace
      = psmrts_trace_array_get_trace(tracearray, 0);

  // get observer and lookdir from retrieved trace
  printf("\nretrieving observer and lookdir vectors from array trace\n");
  PSMRTS_Vector3d retrievedobserver = psmrts_ray_observer(retrievedtrace);
  PSMRTS_Vector3d retrievedlookdir = psmrts_ray_lookdir(retrievedtrace);

  // output retrieved observer and lookdir vectors in x, y, z
  printf("   retrieved observer xyz: %lf %lf %lf\n", retrievedobserver.x, retrievedobserver.y,
                                                      retrievedobserver.z);
  printf("    retrieved lookdir xyz: %lf %lf %lf\n", retrievedlookdir.x, retrievedlookdir.y,
                                                     retrievedlookdir.z);

  // Create PsmrtsShapeTracer with input obj file.
  // can't get max radius yet
  // const double max_radius = bt_tracer.maximum_radius();
  // printf("  max radius: %lf\n", max_radius); //should be 0.283065
  PSMRTS_Tracer *bulletTracer = psmrts_create_bullet( objfile.c_str() );

  // validate bullet tracer
  if ( !psmrts_tracer_valid(bulletTracer) ) {
    // extract string pointer to error (this is not defined yet)
    printf("bullet tracer is not valid\n");
    return ( 1 );
  }

  // Create PRQFeatures object. This is a configurable PSMRTS "Product Request"
  psmrts::PRQFeatures features;

  // Report PRQFeatures information before any processing
  // TBD: WHAT ELSE?
  printf( "\nPRQFeatures Basic information Before Processing\n");
  printf( "        name: %s\n", features.name().c_str() );
  printf( "     invoked: %d\n", features.was_invoked() );
  printf( "   run count: %zu\n", features.run_count() );
  printf( " error count: %zu\n", features.error_count() );
  printf( "  error size: %zu\n", features.errors().size() );

  return ( 0 );
}
