#include <psmrts_catch2_environment.hpp>
#include "psmrts_c.h"

/**
 * @brief PSMRTS C API Vector3D tests
 *
 * This test exercises PSMRTS_Vector3D functionality in the C API, including ...
 *
 * 1. psmrts_vector3d: Construct a PSMRTS_Vector3d
 * 2. psmrts_negate:   Negate a PSMRTS_Vector3d
 * 3. psmrts_subtract: Subtract two PSMRTS_Vector3d
 * 4. psmrts_add:      Add two PSMRTS_Vector3d
 * 5. psmrts_scale:    Scale a PSMRTS_Vector3d
 * 6. psmrts_length:   Compute the length of a PSMRTS_Vector3d
 *
 */
TEST_CASE ( "PSMRTS C API - Vector3D", "[capi][Vector3D]" ) {
    const double tolerance = 1.0e-6;

    // Construct two PSMRTS_Vector3Ds
    PSMRTS_Vector3d v1 = psmrts_vector3d( 10.256, -39.872, 1013.673 );
    PSMRTS_Vector3d v2 = psmrts_vector3d( 101.625, 41.739, -2057.491 );

    CHECK( v1.x ==    10.256 );
    CHECK( v1.y ==   -39.872 );
    CHECK( v1.z ==  1013.673 );
    CHECK( v2.x ==   101.625 );
    CHECK( v2.y ==    41.739 );
    CHECK( v2.z == -2057.491 );

    // Negate vector
    PSMRTS_Vector3d vnegated = psmrts_negate( &v1 );
    CHECK( vnegated.x ==   -10.256 );
    CHECK( vnegated.y ==    39.872 );
    CHECK( vnegated.z == -1013.673 );

    // Subtract two vectors
    PSMRTS_Vector3d vsubtracted = psmrts_subtract( &v1, &v2 );
    CHECK_THAT( vsubtracted.x,
               Catch::Matchers::WithinAbs( -91.369, tolerance ) );
    CHECK_THAT( vsubtracted.y,
               Catch::Matchers::WithinAbs( -81.611, tolerance ) );
    CHECK_THAT( vsubtracted.z,
               Catch::Matchers::WithinAbs( 3071.164, tolerance ) );

    // Add two vectors
    PSMRTS_Vector3d vadded = psmrts_add( &v1, &v2 );
    CHECK_THAT( vadded.x,
               Catch::Matchers::WithinAbs( 111.881, tolerance ) );
    CHECK_THAT( vadded.y,
               Catch::Matchers::WithinAbs( 1.867, tolerance ) );
    CHECK_THAT( vadded.z,
               Catch::Matchers::WithinAbs( -1043.818, tolerance ) );

    // Scale vector
    PSMRTS_Vector3d vscaled = psmrts_scale( &v1 , 10.0 );
    CHECK( vscaled.x ==   102.56 );
    CHECK( vscaled.y ==  -398.72 );
    CHECK( vscaled.z == 10136.73 );

    // compute vector length
    CHECK_THAT( psmrts_length( &v1 ),
               Catch::Matchers::WithinAbs( 1014.508705, tolerance ) );
}

/**
 * @brief PSMRTS C API Trace tests
 *
 * This test exercises the following PSMRTS_RayTrace functionality in the PSMRTS
 * C API...
 *
 * psmrts_create_ray:         Construct a PSMRTS_RayTrace with observer and look
 *                            direction vectors.
 * psmrts_ray_observer:       Retrieve observer vector from PSMRTS_RayTrace.
 * psmrts_ray_lookdir:        Retrieve look dir vector from PSMRTS_RayTrace.
 * psmrts_create_bullet:      Create a PSMRTS Bullet tracer.
 * psmrts_ray_trace:          Runs trace on ray; update ray with the results.
 * psmrts_ray_has_hit:        Compute length of a PSMRTS_Vector3d
 * psmrts_ray_xyz:            Determine if ray intercepts surface.
 * psmrts_xyz_to_lonlatrad_d: Convert llr vector to xyz.
 * psmrts_ray_raypt:          Get vector along look direction to surface.
 * psmrts_ray_normal:         Get normal vector @surface intercept, if it exists.
 * psmrts_ray_intercept_slant_distance: Get slant distance @surface intercept.
 * psmrts_length:             Get vector length.
 *
 */
TEST_CASE ( "PSMRTS C API - Tracer", "[Tracer][CAPI]" ) {
    const double tolerance = 1.0e-6;

    // create observer vector at 45d, 45d, 3000km (lon, lat, radius)
    PSMRTS_Vector3d observer1;
    observer1.longitude = psmrts_degrees_to_radians( 45.0 );
    observer1.latitude  = psmrts_degrees_to_radians( 45.0 );
    observer1.radius    = 3000.0;  // Maxiumum radius of input shape from API

    // convert observer from lon, lat, radius to xyz
    observer1 = psmrts_lonlatrad_to_xyz_d( &observer1 );

    // create look direction vector by negating the observer vector
    PSMRTS_Vector3d lookdir1 = psmrts_negate( &observer1 );

    // create PSMRTS_RayTrace with observer and look direction vectors
    PSMRTS_RayTrace *ray = psmrts_create_ray( &observer1, &lookdir1 );

    // retrieve and validate observer and look direction vectors from ray trace
    PSMRTS_Vector3d observercheck = psmrts_ray_observer( ray );
    PSMRTS_Vector3d lookdircheck  = psmrts_ray_lookdir( ray );

    CHECK_THAT( observercheck.x,
               Catch::Matchers::WithinAbs( 2999.436325, tolerance ) );
    CHECK_THAT( observercheck.y,
               Catch::Matchers::WithinAbs( 41.11820038, tolerance ) );
    CHECK_THAT( observercheck.z,
               Catch::Matchers::WithinAbs( 41.12206381, tolerance ) );
    CHECK_THAT( lookdircheck.x,
               Catch::Matchers::WithinAbs( -2999.436325, tolerance ) );
    CHECK_THAT( lookdircheck.y,
               Catch::Matchers::WithinAbs( -41.11820038, tolerance ) );
    CHECK_THAT( lookdircheck.z,
               Catch::Matchers::WithinAbs( -41.12206381, tolerance ) );

    // trace ray on mesh
    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    PSMRTS_Tracer *bulletTracer = psmrts_create_bullet( objfile.c_str() );
    ray = psmrts_ray_trace( ray, bulletTracer );

    // confirm tracer has a hit on input mesh
    CHECK( psmrts_ray_has_hit( ray ) == PSMRTS_TRUE );

    // validate trace content after hit
    PSMRTS_Vector3d xyz = psmrts_ray_xyz( ray ); // intercept in x,y,z
    CHECK_THAT( xyz.x, Catch::Matchers::WithinAbs( 0.262209, tolerance ) );
    CHECK_THAT( xyz.y, Catch::Matchers::WithinAbs( 0.003594, tolerance ) );
    CHECK_THAT( xyz.z, Catch::Matchers::WithinAbs( 0.003595, tolerance ) );

    PSMRTS_Vector3d llr = psmrts_xyz_to_lonlatrad_d( &xyz ); // intercept in lon, lat, radius
    CHECK_THAT( llr.longitude, Catch::Matchers::WithinAbs( 0.785398, tolerance ) );
    CHECK_THAT( llr.latitude, Catch::Matchers::WithinAbs( 0.785398, tolerance ) );
    CHECK_THAT( llr.radius, Catch::Matchers::WithinAbs( 0.262259, tolerance ) );

    PSMRTS_Vector3d raypt = psmrts_ray_raypt( ray );
    CHECK_THAT( raypt.x, Catch::Matchers::WithinAbs( -2999.174116, tolerance ) );
    CHECK_THAT( raypt.y, Catch::Matchers::WithinAbs( -41.114606, tolerance ) );
    CHECK_THAT( raypt.z, Catch::Matchers::WithinAbs( -41.118469, tolerance ) );

    PSMRTS_Vector3d normal = psmrts_ray_normal( ray );
    CHECK_THAT( normal.x, Catch::Matchers::WithinAbs( 0.850651, tolerance ) );
    CHECK_THAT( normal.y, Catch::Matchers::WithinAbs( 0.000000, tolerance ) );
    CHECK_THAT( normal.z, Catch::Matchers::WithinAbs( 0.525731, tolerance ) );

    double slant_d = psmrts_ray_intercept_slant_distance( ray );
    CHECK_THAT( slant_d, Catch::Matchers::WithinAbs( 2999.737741, tolerance ) );

    double raypt_d = psmrts_length( &raypt );
    CHECK_THAT( slant_d, Catch::Matchers::WithinAbs( 2999.737741, tolerance ) );

    double radius = psmrts_length( &xyz );
    CHECK_THAT( radius, Catch::Matchers::WithinAbs( 0.262259, tolerance ) );
}

/**
 * @brief PSMRTS C API Photometric Trace tests
 *
 * This test exercises the following PSMRTS_RayTrace functionality in the PSMRTS
 * C API...
 *
 * psmrts_create_ray:         Construct a PSMRTS_RayTrace with observer and look
 *                            direction vectors.
 * psmrts_ray_observer:       Retrieve observer vector from PSMRTS_RayTrace.
 * psmrts_ray_lookdir:        Retrieve look dir vector from PSMRTS_RayTrace.
 * psmrts_create_bullet:      Create a PSMRTS Bullet tracer.
 * psmrts_ray_trace:          Runs trace on ray; update ray with the results.
 * psmrts_ray_has_hit:        Compute length of a PSMRTS_Vector3d
 * psmrts_ray_xyz:            Determine if ray intercepts surface.
 * psmrts_xyz_to_lonlatrad_d: Convert llr vector to xyz.
 * psmrts_ray_raypt:          Get vector along look direction to surface.
 * psmrts_ray_normal:         Get normal vector @surface intercept, if it exists.
 * psmrts_ray_intercept_slant_distance: Get slant distance @surface intercept.
 * psmrts_length:             Get vector length.
 *
 */
 /*
TEST_CASE ( "PSMRTS C API - Photometric Tracer", "[PhotometricTracer][CAPI]" ) {
    const double tolerance = 1.0e-6;

    // create observer vector at 45d, 45d, 3000km (lon, lat, radius)
    PSMRTS_Vector3d observer1;
    observer1.longitude = psmrts_degrees_to_radians( 45.0 );
    observer1.latitude  = psmrts_degrees_to_radians( 45.0 );
    observer1.radius    = 3000.0;  // Maxiumum radius of input shape from API

    // convert observer from lon, lat, radius to xyz
    observer1 = psmrts_lonlatrad_to_xyz_d( &observer1 );

    // create look direction vector by negating the observer vector
    PSMRTS_Vector3d lookdir1 = psmrts_negate( &observer1 );

    // create PSMRTS_RayTrace with observer and look direction vectors
    PSMRTS_RayTrace *ray = psmrts_create_ray( &observer1, &lookdir1 );

    // retrieve and validate observer and look direction vectors from ray trace
    PSMRTS_Vector3d observercheck = psmrts_ray_observer( ray );
    PSMRTS_Vector3d lookdircheck  = psmrts_ray_lookdir( ray );

    CHECK_THAT( observercheck.x,
               Catch::Matchers::WithinAbs( 2999.436325, tolerance ) );
    CHECK_THAT( observercheck.y,
               Catch::Matchers::WithinAbs( 41.11820038, tolerance ) );
    CHECK_THAT( observercheck.z,
               Catch::Matchers::WithinAbs( 41.12206381, tolerance ) );
    CHECK_THAT( lookdircheck.x,
               Catch::Matchers::WithinAbs( -2999.436325, tolerance ) );
    CHECK_THAT( lookdircheck.y,
               Catch::Matchers::WithinAbs( -41.11820038, tolerance ) );
    CHECK_THAT( lookdircheck.z,
               Catch::Matchers::WithinAbs( -41.12206381, tolerance ) );

    // trace ray on mesh
    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    PSMRTS_Tracer *bulletTracer = psmrts_create_bullet( objfile.c_str() );
    ray = psmrts_ray_trace( ray, bulletTracer );

    // confirm tracer has a hit on input mesh
    CHECK( psmrts_ray_has_hit( ray ) == PSMRTS_TRUE );

    // validate trace content after hit
    PSMRTS_Vector3d xyz = psmrts_ray_xyz( ray ); // intercept in x,y,z
    CHECK_THAT( xyz.x, Catch::Matchers::WithinAbs( 0.262209, tolerance ) );
    CHECK_THAT( xyz.y, Catch::Matchers::WithinAbs( 0.003594, tolerance ) );
    CHECK_THAT( xyz.z, Catch::Matchers::WithinAbs( 0.003595, tolerance ) );

    PSMRTS_Vector3d llr = psmrts_xyz_to_lonlatrad_d( &xyz ); // intercept in lon, lat, radius
    CHECK_THAT( llr.longitude, Catch::Matchers::WithinAbs( 0.785398, tolerance ) );
    CHECK_THAT( llr.latitude, Catch::Matchers::WithinAbs( 0.785398, tolerance ) );
    CHECK_THAT( llr.radius, Catch::Matchers::WithinAbs( 0.262259, tolerance ) );

    PSMRTS_Vector3d raypt = psmrts_ray_raypt( ray );
    CHECK_THAT( raypt.x, Catch::Matchers::WithinAbs( -2999.174116, tolerance ) );
    CHECK_THAT( raypt.y, Catch::Matchers::WithinAbs( -41.114606, tolerance ) );
    CHECK_THAT( raypt.z, Catch::Matchers::WithinAbs( -41.118469, tolerance ) );

    PSMRTS_Vector3d normal = psmrts_ray_normal( ray );
    CHECK_THAT( normal.x, Catch::Matchers::WithinAbs( 0.850651, tolerance ) );
    CHECK_THAT( normal.y, Catch::Matchers::WithinAbs( 0.000000, tolerance ) );
    CHECK_THAT( normal.z, Catch::Matchers::WithinAbs( 0.525731, tolerance ) );

    double slant_d = psmrts_ray_intercept_slant_distance( ray );
    CHECK_THAT( slant_d, Catch::Matchers::WithinAbs( 2999.737741, tolerance ) );

    double raypt_d = psmrts_length( &raypt );
    CHECK_THAT( slant_d, Catch::Matchers::WithinAbs( 2999.737741, tolerance ) );

    double radius = psmrts_length( &xyz );
    CHECK_THAT( radius, Catch::Matchers::WithinAbs( 0.262259, tolerance ) );
}
*/
/**
 * @brief PSMRTS C API Photometric Trace tests
 *
 * This test exercises the following PSMRTS_RayTrace functionality in the PSMRTS
 * C API...
 *
 * psmrts_create_ray:         Construct a PSMRTS_RayTrace with observer and look
 *                            direction vectors.
 * psmrts_ray_observer:       Retrieve observer vector from PSMRTS_RayTrace.
 * psmrts_ray_lookdir:        Retrieve look dir vector from PSMRTS_RayTrace.
 * psmrts_create_bullet:      Create a PSMRTS Bullet tracer.
 * psmrts_ray_trace:          Runs trace on ray; update ray with the results.
 * psmrts_ray_has_hit:        Compute length of a PSMRTS_Vector3d
 * psmrts_ray_xyz:            Determine if ray intercepts surface.
 * psmrts_xyz_to_lonlatrad_d: Convert llr vector to xyz.
 * psmrts_ray_raypt:          Get vector along look direction to surface.
 * psmrts_ray_normal:         Get normal vector @surface intercept, if it exists.
 * psmrts_ray_intercept_slant_distance: Get slant distance @surface intercept.
 * psmrts_length:             Get vector length.
 *
 */
/*
TEST_CASE ( "PSMRTS C API - Tracer Array", "[Tracer][Array][CAPI]" ) {
    const double tolerance = 1.0e-6;

    // create observer vector at 45d, 45d, 3000km (lon, lat, radius)
    PSMRTS_Vector3d observer1;
    observer1.longitude = psmrts_degrees_to_radians( 45.0 );
    observer1.latitude  = psmrts_degrees_to_radians( 45.0 );
    observer1.radius    = 3000.0;  // Maxiumum radius of input shape from API

    // convert observer from lon, lat, radius to xyz
    observer1 = psmrts_lonlatrad_to_xyz_d( &observer1 );

    // create look direction vector by negating the observer vector
    PSMRTS_Vector3d lookdir1 = psmrts_negate( &observer1 );

    // create PSMRTS_RayTrace with observer and look direction vectors
    PSMRTS_RayTrace *ray = psmrts_create_ray( &observer1, &lookdir1 );

    // retrieve and validate observer and look direction vectors from ray trace
    PSMRTS_Vector3d observercheck = psmrts_ray_observer( ray );
    PSMRTS_Vector3d lookdircheck  = psmrts_ray_lookdir( ray );

    CHECK_THAT( observercheck.x,
               Catch::Matchers::WithinAbs( 2999.436325, tolerance ) );
    CHECK_THAT( observercheck.y,
               Catch::Matchers::WithinAbs( 41.11820038, tolerance ) );
    CHECK_THAT( observercheck.z,
               Catch::Matchers::WithinAbs( 41.12206381, tolerance ) );
    CHECK_THAT( lookdircheck.x,
               Catch::Matchers::WithinAbs( -2999.436325, tolerance ) );
    CHECK_THAT( lookdircheck.y,
               Catch::Matchers::WithinAbs( -41.11820038, tolerance ) );
    CHECK_THAT( lookdircheck.z,
               Catch::Matchers::WithinAbs( -41.12206381, tolerance ) );

    // trace ray on mesh
    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    PSMRTS_Tracer *bulletTracer = psmrts_create_bullet( objfile.c_str() );
    ray = psmrts_ray_trace( ray, bulletTracer );

    // confirm tracer has a hit on input mesh
    CHECK( psmrts_ray_has_hit( ray ) == PSMRTS_TRUE );

    // validate trace content after hit
    PSMRTS_Vector3d xyz = psmrts_ray_xyz( ray ); // intercept in x,y,z
    CHECK_THAT( xyz.x, Catch::Matchers::WithinAbs( 0.262209, tolerance ) );
    CHECK_THAT( xyz.y, Catch::Matchers::WithinAbs( 0.003594, tolerance ) );
    CHECK_THAT( xyz.z, Catch::Matchers::WithinAbs( 0.003595, tolerance ) );

    PSMRTS_Vector3d llr = psmrts_xyz_to_lonlatrad_d( &xyz ); // intercept in lon, lat, radius
    CHECK_THAT( llr.longitude, Catch::Matchers::WithinAbs( 0.785398, tolerance ) );
    CHECK_THAT( llr.latitude, Catch::Matchers::WithinAbs( 0.785398, tolerance ) );
    CHECK_THAT( llr.radius, Catch::Matchers::WithinAbs( 0.262259, tolerance ) );

    PSMRTS_Vector3d raypt = psmrts_ray_raypt( ray );
    CHECK_THAT( raypt.x, Catch::Matchers::WithinAbs( -2999.174116, tolerance ) );
    CHECK_THAT( raypt.y, Catch::Matchers::WithinAbs( -41.114606, tolerance ) );
    CHECK_THAT( raypt.z, Catch::Matchers::WithinAbs( -41.118469, tolerance ) );

    PSMRTS_Vector3d normal = psmrts_ray_normal( ray );
    CHECK_THAT( normal.x, Catch::Matchers::WithinAbs( 0.850651, tolerance ) );
    CHECK_THAT( normal.y, Catch::Matchers::WithinAbs( 0.000000, tolerance ) );
    CHECK_THAT( normal.z, Catch::Matchers::WithinAbs( 0.525731, tolerance ) );

    double slant_d = psmrts_ray_intercept_slant_distance( ray );
    CHECK_THAT( slant_d, Catch::Matchers::WithinAbs( 2999.737741, tolerance ) );

    double raypt_d = psmrts_length( &raypt );
    CHECK_THAT( slant_d, Catch::Matchers::WithinAbs( 2999.737741, tolerance ) );

    double radius = psmrts_length( &xyz );
    CHECK_THAT( radius, Catch::Matchers::WithinAbs( 0.262259, tolerance ) );
}
*/
/**
 * @brief PSMRTS C API Photometric Trace tests
 *
 * This test exercises the following PSMRTS_RayTrace functionality in the PSMRTS
 * C API...
 *
 * psmrts_create_ray:         Construct a PSMRTS_RayTrace with observer and look
 *                            direction vectors.
 * psmrts_ray_observer:       Retrieve observer vector from PSMRTS_RayTrace.
 * psmrts_ray_lookdir:        Retrieve look dir vector from PSMRTS_RayTrace.
 * psmrts_create_bullet:      Create a PSMRTS Bullet tracer.
 * psmrts_ray_trace:          Runs trace on ray; update ray with the results.
 * psmrts_ray_has_hit:        Compute length of a PSMRTS_Vector3d
 * psmrts_ray_xyz:            Determine if ray intercepts surface.
 * psmrts_xyz_to_lonlatrad_d: Convert llr vector to xyz.
 * psmrts_ray_raypt:          Get vector along look direction to surface.
 * psmrts_ray_normal:         Get normal vector @surface intercept, if it exists.
 * psmrts_ray_intercept_slant_distance: Get slant distance @surface intercept.
 * psmrts_length:             Get vector length.
 *
 */
 /*
TEST_CASE ( "PSMRTS C API - PhotometricTracer Array", "[capi][Photometric[[Tracer][Array]" ) {
    const double tolerance = 1.0e-6;

    // create observer vector at 45d, 45d, 3000km (lon, lat, radius)
    PSMRTS_Vector3d observer1;
    observer1.longitude = psmrts_degrees_to_radians( 45.0 );
    observer1.latitude  = psmrts_degrees_to_radians( 45.0 );
    observer1.radius    = 3000.0;  // Maxiumum radius of input shape from API

    // convert observer from lon, lat, radius to xyz
    observer1 = psmrts_lonlatrad_to_xyz_d( &observer1 );

    // create look direction vector by negating the observer vector
    PSMRTS_Vector3d lookdir1 = psmrts_negate( &observer1 );

    // create PSMRTS_RayTrace with observer and look direction vectors
    PSMRTS_RayTrace *ray = psmrts_create_ray( &observer1, &lookdir1 );

    // retrieve and validate observer and look direction vectors from ray trace
    PSMRTS_Vector3d observercheck = psmrts_ray_observer( ray );
    PSMRTS_Vector3d lookdircheck  = psmrts_ray_lookdir( ray );

    CHECK_THAT( observercheck.x,
               Catch::Matchers::WithinAbs( 2999.436325, tolerance ) );
    CHECK_THAT( observercheck.y,
               Catch::Matchers::WithinAbs( 41.11820038, tolerance ) );
    CHECK_THAT( observercheck.z,
               Catch::Matchers::WithinAbs( 41.12206381, tolerance ) );
    CHECK_THAT( lookdircheck.x,
               Catch::Matchers::WithinAbs( -2999.436325, tolerance ) );
    CHECK_THAT( lookdircheck.y,
               Catch::Matchers::WithinAbs( -41.11820038, tolerance ) );
    CHECK_THAT( lookdircheck.z,
               Catch::Matchers::WithinAbs( -41.12206381, tolerance ) );

    // trace ray on mesh
    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    PSMRTS_Tracer *bulletTracer = psmrts_create_bullet( objfile.c_str() );
    ray = psmrts_ray_trace( ray, bulletTracer );

    // confirm tracer has a hit on input mesh
    CHECK( psmrts_ray_has_hit( ray ) == PSMRTS_TRUE );

    // validate trace content after hit
    PSMRTS_Vector3d xyz = psmrts_ray_xyz( ray ); // intercept in x,y,z
    CHECK_THAT( xyz.x, Catch::Matchers::WithinAbs( 0.262209, tolerance ) );
    CHECK_THAT( xyz.y, Catch::Matchers::WithinAbs( 0.003594, tolerance ) );
    CHECK_THAT( xyz.z, Catch::Matchers::WithinAbs( 0.003595, tolerance ) );

    PSMRTS_Vector3d llr = psmrts_xyz_to_lonlatrad_d( &xyz ); // intercept in lon, lat, radius
    CHECK_THAT( llr.longitude, Catch::Matchers::WithinAbs( 0.785398, tolerance ) );
    CHECK_THAT( llr.latitude, Catch::Matchers::WithinAbs( 0.785398, tolerance ) );
    CHECK_THAT( llr.radius, Catch::Matchers::WithinAbs( 0.262259, tolerance ) );

    PSMRTS_Vector3d raypt = psmrts_ray_raypt( ray );
    CHECK_THAT( raypt.x, Catch::Matchers::WithinAbs( -2999.174116, tolerance ) );
    CHECK_THAT( raypt.y, Catch::Matchers::WithinAbs( -41.114606, tolerance ) );
    CHECK_THAT( raypt.z, Catch::Matchers::WithinAbs( -41.118469, tolerance ) );

    PSMRTS_Vector3d normal = psmrts_ray_normal( ray );
    CHECK_THAT( normal.x, Catch::Matchers::WithinAbs( 0.850651, tolerance ) );
    CHECK_THAT( normal.y, Catch::Matchers::WithinAbs( 0.000000, tolerance ) );
    CHECK_THAT( normal.z, Catch::Matchers::WithinAbs( 0.525731, tolerance ) );

    double slant_d = psmrts_ray_intercept_slant_distance( ray );
    CHECK_THAT( slant_d, Catch::Matchers::WithinAbs( 2999.737741, tolerance ) );

    double raypt_d = psmrts_length( &raypt );
    CHECK_THAT( slant_d, Catch::Matchers::WithinAbs( 2999.737741, tolerance ) );

    double radius = psmrts_length( &xyz );
    CHECK_THAT( radius, Catch::Matchers::WithinAbs( 0.262259, tolerance ) );
}
*/

TEST_CASE( "PSMRTS CAPI Latitudinal to Rectangular Coordinate Conversion", "[capi][utilities][lat2rect][conversion]") {
  const double tolerance = 1.0e-6;

  PSMRTS_Vector3d llr_d; // lon, lat in degrees; radius in km
  PSMRTS_Vector3d xyz;   // km

  // test at with latitude > 90.0 (should clamp to 90.0)
  llr_d.longitude = 0.0;
  llr_d.latitude  = 100.0;
  llr_d.radius    = 1.0;

  // convert vector from lon (d), lat (d), r (km) to xyz (km)
  xyz = psmrts_lonlatrad_to_xyz_d( &llr_d );

  CHECK_THAT( xyz.x, Catch::Matchers::WithinAbs( 0.0, tolerance ));
  CHECK_THAT( xyz.y, Catch::Matchers::WithinAbs( 0.0, tolerance ));
  CHECK_THAT( xyz.z, Catch::Matchers::WithinAbs( 1.0, tolerance ));

  // test at with latitude < 90.0 (should clamp to -90.0)
  llr_d.longitude =  0.0;
  llr_d.latitude  = -100.0;
  llr_d.radius    =  1.0;

  xyz = psmrts_lonlatrad_to_xyz_d( &llr_d );

  CHECK_THAT( xyz.x, Catch::Matchers::WithinAbs(  0.0, tolerance ));
  CHECK_THAT( xyz.y, Catch::Matchers::WithinAbs(  0.0, tolerance ));
  CHECK_THAT( xyz.z, Catch::Matchers::WithinAbs( -1.0, tolerance ));

  // test at lon = 0 in the XY plane
  llr_d.longitude = 0.0;
  llr_d.latitude  = 0.0;
  llr_d.radius    = 1.0;

  xyz = psmrts_lonlatrad_to_xyz_d( &llr_d );

  CHECK_THAT( xyz.x, Catch::Matchers::WithinAbs( 1.0, tolerance ));
  CHECK_THAT( xyz.y, Catch::Matchers::WithinAbs( 0.0, tolerance ));
  CHECK_THAT( xyz.z, Catch::Matchers::WithinAbs( 0.0, tolerance ));

  // test at lon = 90 in the XY plane
  llr_d.longitude = 90.0;
  llr_d.latitude  =  0.0;
  llr_d.radius    =  1.0;

  xyz = psmrts_lonlatrad_to_xyz_d( &llr_d );

  CHECK_THAT( xyz.x, Catch::Matchers::WithinAbs( 0.0, tolerance ));
  CHECK_THAT( xyz.y, Catch::Matchers::WithinAbs( 1.0, tolerance ));
  CHECK_THAT( xyz.z, Catch::Matchers::WithinAbs( 0.0, tolerance ));

    // test at lon = 180 in the XY plane
  llr_d.longitude = 180.0;
  llr_d.latitude  =   0.0;
  llr_d.radius    =   1.0;

  xyz = psmrts_lonlatrad_to_xyz_d( &llr_d );

  CHECK_THAT( xyz.x, Catch::Matchers::WithinAbs( -1.0, tolerance ));
  CHECK_THAT( xyz.y, Catch::Matchers::WithinAbs(  0.0, tolerance ));
  CHECK_THAT( xyz.z, Catch::Matchers::WithinAbs(  0.0, tolerance ));

  // test at lon = 270 in the XY plane
  llr_d.longitude = 270.0;
  llr_d.latitude  =   0.0;
  llr_d.radius    =   1.0;

  xyz = psmrts_lonlatrad_to_xyz_d( &llr_d );

  CHECK_THAT( xyz.x, Catch::Matchers::WithinAbs(  0.0, tolerance ));
  CHECK_THAT( xyz.y, Catch::Matchers::WithinAbs( -1.0, tolerance ));
  CHECK_THAT( xyz.z, Catch::Matchers::WithinAbs(  0.0, tolerance ));

    // test at lon = 45, lat = 45
  llr_d.longitude = 45.0;
  llr_d.latitude  = 45.0;
  llr_d.radius    =  1.0;

  xyz = psmrts_lonlatrad_to_xyz_d( &llr_d );

  CHECK_THAT( xyz.x, Catch::Matchers::WithinAbs( 0.5, tolerance ));
  CHECK_THAT( xyz.y, Catch::Matchers::WithinAbs( 0.5, tolerance ));
  CHECK_THAT( xyz.z, Catch::Matchers::WithinAbs( 0.707106, tolerance ));

  // test at lon = -45, lat = -45
  llr_d.longitude = -45.0;
  llr_d.latitude  = -45.0;
  llr_d.radius    =  1.0;

  xyz = psmrts_lonlatrad_to_xyz_d( &llr_d );

  CHECK_THAT( xyz.x, Catch::Matchers::WithinAbs(  0.5, tolerance ));
  CHECK_THAT( xyz.y, Catch::Matchers::WithinAbs( -0.5, tolerance ));
  CHECK_THAT( xyz.z, Catch::Matchers::WithinAbs( -0.707106, tolerance ));

  // test at lon = 135, lat = 45
  llr_d.longitude = 135.0;
  llr_d.latitude  =  45.0;
  llr_d.radius    =   1.0;

  xyz = psmrts_lonlatrad_to_xyz_d( &llr_d );

  CHECK_THAT( xyz.x, Catch::Matchers::WithinAbs( -0.5, tolerance ));
  CHECK_THAT( xyz.y, Catch::Matchers::WithinAbs(  0.5, tolerance ));
  CHECK_THAT( xyz.z, Catch::Matchers::WithinAbs(  0.707106, tolerance ));

  // test at lon = -135, lat = -45
  llr_d.longitude = -135.0;
  llr_d.latitude  =  -45.0;
  llr_d.radius    =    1.0;

  xyz = psmrts_lonlatrad_to_xyz_d( &llr_d );

  CHECK_THAT( xyz.x, Catch::Matchers::WithinAbs( -0.5, tolerance ));
  CHECK_THAT( xyz.y, Catch::Matchers::WithinAbs( -0.5, tolerance ));
  CHECK_THAT( xyz.z, Catch::Matchers::WithinAbs( -0.707106, tolerance ));
}

TEST_CASE( "PSMRTS CAPI Rectangular to Latitudinal Coordinate Conversion", "[capi][utilities][rect2lat][conversion]") {
  const double tolerance = 1.0e-6;

  PSMRTS_Vector3d xyz;   // km
  PSMRTS_Vector3d llr_d; // lon, lat in degrees; radius in km

  // test zero vector
  xyz.x = 0.0;
  xyz.y = 0.0;
  xyz.z = 0.0;

  // convert to longitude, latitude, radius
  llr_d = psmrts_xyz_to_lonlatrad_d( &xyz );

  CHECK_THAT( llr_d.longitude, Catch::Matchers::WithinAbs( 0.0, tolerance ) );
  CHECK_THAT( llr_d.latitude, Catch::Matchers::WithinAbs( 0.0, tolerance ) );
  CHECK_THAT( llr_d.radius, Catch::Matchers::WithinAbs( 0.0, tolerance ) );

  // point at 45 lon, 45 lat
  xyz.x = 0.5;
  xyz.y = 0.5;
  xyz.z = 0.707106781;

  // convert to longitude, latitude, radius
  llr_d = psmrts_xyz_to_lonlatrad_d( &xyz );

  CHECK_THAT( llr_d.longitude, Catch::Matchers::WithinAbs( 45.0, tolerance ) );
  CHECK_THAT( llr_d.latitude, Catch::Matchers::WithinAbs( 45.0, tolerance ) );
  CHECK_THAT( llr_d.radius, Catch::Matchers::WithinAbs( 1.0, tolerance ) );

  // point at 225 lon, -45 lat
  xyz.x = -0.5;
  xyz.y = -0.5;
  xyz.z =  0.707106781;

  // convert to longitude, latitude, radius
  llr_d = psmrts_xyz_to_lonlatrad_d( &xyz );

  CHECK_THAT( llr_d.longitude, Catch::Matchers::WithinAbs( 225.0, tolerance ) );
  CHECK_THAT( llr_d.latitude, Catch::Matchers::WithinAbs( 45.0, tolerance ) );
  CHECK_THAT( llr_d.radius, Catch::Matchers::WithinAbs( 1.0, tolerance ) );

  // point at 315 lon, -45 lat
  xyz.x =  0.5;
  xyz.y = -0.5;
  xyz.z = -0.707106781;

  // convert to longitude, latitude, radius
  llr_d = psmrts_xyz_to_lonlatrad_d( &xyz );

  CHECK_THAT( llr_d.longitude, Catch::Matchers::WithinAbs( 315.0, tolerance ) );
  CHECK_THAT( llr_d.latitude, Catch::Matchers::WithinAbs( -45.0, tolerance ) );
  CHECK_THAT( llr_d.radius, Catch::Matchers::WithinAbs( 1.0, tolerance ) );

  // point at 135 lon, 45 lat
  xyz.x = -0.5;
  xyz.y =  0.5;
  xyz.z =  0.707106781;

  // convert to longitude, latitude, radius
  llr_d = psmrts_xyz_to_lonlatrad_d( &xyz );

  CHECK_THAT( llr_d.longitude, Catch::Matchers::WithinAbs( 135.0, tolerance ) );
  CHECK_THAT( llr_d.latitude, Catch::Matchers::WithinAbs( 45.0, tolerance ) );
  CHECK_THAT( llr_d.radius, Catch::Matchers::WithinAbs( 1.0, tolerance ) );
}

/**
 * @brief PSMRTS C API degree2radian and radian2degree Conversion tests
 *
 * This test exercises degree to radian (and vice versa) conversion in the
 * PSMRTS C API ...
 *
 * psmrts_degrees_to_radians: Convert number from degrees to radians.
 * psmrts_radians_to_degrees: Convert number from radians to degrees.
 */
TEST_CASE ( "PSMRTS C API - Degree Radian Conversion", "[capi][utilities][deg2rad][rad2deg]" ) {
    const double tolerance = 1.0e-6;

    double deg = 45.7;
    CHECK_THAT( psmrts_degrees_to_radians( deg ),
               Catch::Matchers::WithinAbs( 0.797615, tolerance ) );

    double rad = -4.742;
    CHECK_THAT( psmrts_radians_to_degrees( rad ),
               Catch::Matchers::WithinAbs( -271.696586, tolerance ) );
}
