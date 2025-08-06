#include <psmrts_catch2_environment.hpp>
#include "psmrts_c.h"

#include <string>

/**
 * @brief bulletTraceFixture class used in some C API tests below to minimize code duplication.
 *
 * The fixture contains a constructor to perform setup code and a destructor to free
 * allocated memory. It exercises the following PSMRTS_RayTrace functionality...
 *
 * 1. psmrts_vector3d:           Construct PSMRTS_Vector3d.
 * 2. psmrts_lonlatrad_to_xyz_d: Convert PSMRTS_Vector3d from lon(d), lat(d), rad(km) to xyz.
 * 3. psmrts_negate:             Negate PSMRTS_Vector3d.
 * 4. psmrts_create_ray:         Create PSMRTS_RayTrace.
 * 5. psmrts_create_bullet:      Create bullet PSMRTS_Tracer.
 * 6. psmrts_tracer_valid:       Validate PSMRTS_Tracer.
 * 7. psmrts_ray_has_hit:        Determine if ray intercepts surface.
 *
 */
class bulletTraceFixture {
  public:
    bulletTraceFixture() { // setup code
      // create observer and look direction vectors
      observer = psmrts_vector3d( 45.0, 45.0, 3000.0 );

      // convert observer from lon, lat, radius to xyz
      observer = psmrts_lonlatrad_to_xyz_d( &observer );

      // create look direction vector by negating the observer vector
      lookdir = psmrts_negate( &observer );

      // create ray trace from observer, lookdir vectors
      ray = psmrts_create_ray( &observer, &lookdir );

      // retrieve/validate observer/look direction vectors from ray trace
      CHECK_THAT( observer.x,
                  Catch::Matchers::WithinAbs( 1500.0, tolerance ) );
      CHECK_THAT( observer.y,
                  Catch::Matchers::WithinAbs( 1500.0, tolerance ) );
      CHECK_THAT( observer.z,
                  Catch::Matchers::WithinAbs( 2121.320344, tolerance ) );
      CHECK_THAT( lookdir.x,
                  Catch::Matchers::WithinAbs( -1500.0, tolerance ) );
      CHECK_THAT( lookdir.y,
                  Catch::Matchers::WithinAbs( -1500.0, tolerance ) );
      CHECK_THAT( lookdir.z,
                  Catch::Matchers::WithinAbs( -2121.320344, tolerance ) );

      // create bullet tracer from input obj file
      std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
      bulletTracer = psmrts_create_bullet( objfile.c_str() );

      // validate tracer
      CHECK( psmrts_tracer_valid( bulletTracer ) == PSMRTS_TRUE );

      // run trace
      // TBD: WOULD THIS METHOD BE BETTER CALLED "psmrts_run_ray_trace"?
      ray = psmrts_ray_trace( ray, bulletTracer );

      // confirm tracer has a hit on input mesh
      CHECK( psmrts_ray_has_hit( ray ) == PSMRTS_TRUE );
    }

    ~bulletTraceFixture() { // tear down code
      psmrts_free_ray( ray );
      psmrts_free_tracer( bulletTracer );
    }

    const double tolerance = 1.0e-6; // tolerance for comparisons
    PSMRTS_Vector3d observer;        // observer vector
    PSMRTS_Vector3d lookdir;         // look direction vector
    PSMRTS_RayTrace *ray;            // ray trace
    PSMRTS_Tracer *bulletTracer;     // bullet tracer
};

/**
 * @brief PSMRTS C API Vector3D tests
 *
 * This test exercises PSMRTS_Vector3D functionality in the C API, including...
 *
 * 1. psmrts_vector3d: Construct a PSMRTS_Vector3d.
 * 2. psmrts_negate:   Negate a PSMRTS_Vector3d.
 * 3. psmrts_subtract: Subtract two PSMRTS_Vector3d.
 * 4. psmrts_add:      Add two PSMRTS_Vector3d.
 * 5. psmrts_scale:    Scale a PSMRTS_Vector3d.
 * 6. psmrts_length:   Compute length of a PSMRTS_Vector3d.
 *
 */
TEST_CASE ( "PSMRTS C API - Vector3D", "[capi][c++][Vector3D]" ) {
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
 * @brief Tests PSMRTS C API functionality for a single PSMRTS_RayTrace.
 *
 * This test derives from the 'bulletTraceFixture' at the top of this file. In addition to C API
 * calls in 'bulletTraceFixture', it exercises the following PSMRTS_RayTrace functionality...
 *
 *  1. psmrts_create_ray:                   Construct PSMRTS_RayTrace w/observer & look dir vectors.
 *  2. psmrts_ray_observer:                 Retrieve observer vector from PSMRTS_RayTrace.
 *  3. psmrts_ray_lookdir:                  Retrieve look direction vector from PSMRTS_RayTrace.
 *  4. psmrts_create_bullet:                Create a PSMRTS Bullet tracer.
 *  5. psmrts_ray_trace:                    Runs trace on ray; update ray with the results.
 *  6. psmrts_ray_has_hit:                  Compute length of a PSMRTS_Vector3d
 *  7. psmrts_ray_xyz:                      Determine if ray intercepts surface.
 *  8. psmrts_xyz_to_lonlatrad_d:           Convert llr vector to xyz.
 *  9. psmrts_ray_raypt:                    Get vector along look direction to surface.
 * 10. psmrts_ray_normal:                   Get normal vector @surface intercept, if it exists.
 * 11. psmrts_ray_intercept_slant_distance: Get slant distance @surface intercept.
 * 12. psmrts_length:                       Get vector length.
 * 13. psmrts_ray_intercept_radius:         Get target body radius @surface intercept.
 * 14. psmrts_ray_set_observation:          Reset ray with different observer and/or look direction.
 *
 */
TEST_CASE_METHOD ( bulletTraceFixture, "PSMRTS C API - One Trace", "[capi][c++][OneTrace]" ) {

  // bulletTraceFixture setup code has been executed.

  // Validate surface intercept of PSMRTS_RayTrace in xyz.
  PSMRTS_Vector3d xyz = psmrts_ray_xyz( ray );
  CHECK_THAT( xyz.x, Catch::Matchers::WithinAbs( 0.130118, tolerance ) );
  CHECK_THAT( xyz.y, Catch::Matchers::WithinAbs( 0.130118, tolerance ) );
  CHECK_THAT( xyz.z, Catch::Matchers::WithinAbs( 0.1840143, tolerance ) );

  // Validate surface intercept of PSMRTS_RayTrace in lon, lat, radius.
  PSMRTS_Vector3d llr = psmrts_xyz_to_lonlatrad_d( &xyz );
  CHECK_THAT( llr.longitude, Catch::Matchers::WithinAbs( 45.0, tolerance ) );
  CHECK_THAT( llr.latitude, Catch::Matchers::WithinAbs( 45.0, tolerance ) );
  CHECK_THAT( llr.radius, Catch::Matchers::WithinAbs( 0.260236, tolerance ) );

  // Validate vector along PSMRTS_RayTrace look direction to surface.
  PSMRTS_Vector3d raypt = psmrts_ray_raypt( ray );
  CHECK_THAT( raypt.x, Catch::Matchers::WithinAbs( -1499.869882, tolerance ) );
  CHECK_THAT( raypt.y, Catch::Matchers::WithinAbs( -1499.869882, tolerance ) );
  CHECK_THAT( raypt.z, Catch::Matchers::WithinAbs( -2121.136329, tolerance ) );

  // Validate normal vector at PSMRTS_RayTrace surface intercept.
  PSMRTS_Vector3d normal = psmrts_ray_normal( ray );
  CHECK_THAT( normal.x, Catch::Matchers::WithinAbs( 0.000000, tolerance ) );
  CHECK_THAT( normal.y, Catch::Matchers::WithinAbs( 0.525731, tolerance ) );
  CHECK_THAT( normal.z, Catch::Matchers::WithinAbs( 0.850651, tolerance ) );

  // Validate slant distance to PSMRTS_RayTrace surface intercept.
  CHECK_THAT( psmrts_ray_intercept_slant_distance( ray ),
              Catch::Matchers::WithinAbs( 2999.739764, tolerance ) );

  // Validate length of PSMRTS_RayTrace look direction vector to surface
  // Note this should be identical to the slant distance above as the
  // observation is defined as nadir-looking.
  CHECK_THAT( psmrts_length( &raypt ),
              Catch::Matchers::WithinAbs( 2999.739764, tolerance ) );

  // Validate target body radius at PSMRTS_RayTrace surface intercept.
  CHECK_THAT( psmrts_ray_intercept_radius( ray ),
              Catch::Matchers::WithinAbs( 0.260236, tolerance ) );

  // reset ray to have observer at 46d, 46d, 3000km (lon, lat, radius)
  PSMRTS_Vector3d newobserver = psmrts_vector3d( 46.0, 46.0, 3000.0 );
  newobserver = psmrts_lonlatrad_to_xyz_d( &newobserver );
  PSMRTS_Vector3d newlookdir  = psmrts_negate( &newobserver );
  ray = psmrts_ray_set_observation( &newobserver, &newlookdir, ray );

  // run trace
  // TBD: WOULD THIS METHOD BE BETTER CALLED "psmrts_run_ray_trace"?
  ray = psmrts_ray_trace( ray, bulletTracer );

  // Validate surface intercept of updated ray in xyz.
  xyz = psmrts_ray_xyz( ray );
  CHECK_THAT( xyz.x, Catch::Matchers::WithinAbs( 0.124106, tolerance ) );
  CHECK_THAT( xyz.y, Catch::Matchers::WithinAbs( 0.128515, tolerance ) );
  CHECK_THAT( xyz.z, Catch::Matchers::WithinAbs( 0.185005, tolerance ) );

  // Validate surface intercept of updated ray in lon, lat, radius.
  llr = psmrts_xyz_to_lonlatrad_d( &xyz );
  CHECK_THAT( llr.longitude, Catch::Matchers::WithinAbs( 46.0, tolerance ) );
  CHECK_THAT( llr.latitude, Catch::Matchers::WithinAbs( 46.0, tolerance ) );
  CHECK_THAT( llr.radius, Catch::Matchers::WithinAbs( 0.257187, tolerance ) );

  // Validate vector along updated ray look direction to surface.
  raypt = psmrts_ray_raypt( ray );
  CHECK_THAT( raypt.x, Catch::Matchers::WithinAbs( -1447.526649, tolerance ) );
  CHECK_THAT( raypt.y, Catch::Matchers::WithinAbs( -1498.957725, tolerance ) );
  CHECK_THAT( raypt.z, Catch::Matchers::WithinAbs( -2157.834396, tolerance ) );

  // Validate normal vector at updated ray surface intercept.
  // NOTE: interesting that this doesn't change, have we hit the same facet?
  normal = psmrts_ray_normal( ray );
  CHECK_THAT( normal.x, Catch::Matchers::WithinAbs( 0.000000, tolerance ) );
  CHECK_THAT( normal.y, Catch::Matchers::WithinAbs( 0.525731, tolerance ) );
  CHECK_THAT( normal.z, Catch::Matchers::WithinAbs( 0.850651, tolerance ) );

  // Validate slant distance to updated ray surface intercept.
  CHECK_THAT( psmrts_ray_intercept_slant_distance( ray ),
              Catch::Matchers::WithinAbs( 2999.742813, tolerance ) );

  // Validate length of updated ray look direction vector to surface
  // Note this should be identical to the slant distance above as the
  // observation is defined as nadir-looking.
  CHECK_THAT( psmrts_length( &raypt ),
              Catch::Matchers::WithinAbs( 2999.742813, tolerance ) );

  // Validate target body radius at updated ray surface intercept.
  CHECK_THAT( psmrts_ray_intercept_radius( ray ),
              Catch::Matchers::WithinAbs( 0.257187, tolerance ) );
}

/**
 * @brief Tests PSMRTS C API functionality for two PSMRTS_RayTraces.
 *
 * This test derives from the bulletTraceFixture at the top of this file. It exercises the
 * following functionality in the PSMRTS C API...
 *
 *  1. psmrts_vector3d:                 Construct PSMRTS_Vector3d.
 *  2. psmrts_xyz_to_lonlatrad_d:       Convert lon(d), lat(d), radius(km) vector to xyz.
 *  3. psmrts_negate:                   Negate PSMRTS_Vector3d.
 *  4. psmrts_ray_trace_v:              Create PSMRTS_RayTrace w/observer & look dir vectors.
 *  5. psmrts_ray_has_hit:              Determine if ray intercepts surface.
 *  6. psmrts_ray2ray_distance:         Compute distance between surface intercepts of 2 rays.
 *  7. psmrts_separation_angle_radians: Compute separation angle (radians) between 2 rays.
 *  8. psmrts_isNear:                   Confirm "nearness" of 2 rays, given a tolerance.
 *  9. psmrts_incidence:                Compute incidence angle (radians) between 2 rays.
 * 10. psmrts_phase:                    Compute phase angle (radians) between 2 rays.
 * 11. psmrts_emission:                 Compute emission angle of ray.
 *
 */
TEST_CASE_METHOD ( bulletTraceFixture,  "PSMRTS C API - Two Traces", "[capi][c++][TwoTraces]" ) {

  // bulletTraceFixture setup code has been executed.

  // create observer2 vector at 46d, 46d, 3000km (lon, lat, radius)
  PSMRTS_Vector3d observer2 = psmrts_vector3d( 46.0, 46.0, 3000.0 );

  // convert observer2 from lon, lat, radius to xyz
  observer2 = psmrts_lonlatrad_to_xyz_d( &observer2 );

  // create look direction vector by negating the observer vector
  PSMRTS_Vector3d lookdir2 = psmrts_negate( &observer2 );

  // create PSMRTS_RayTrace with observer2, lookdir2 vectors and tracer and run trace
  // TBD: WOULD THIS METHOD BE BETTER CALLED "psmrts_run_ray_trace_v"? And why v?
  PSMRTS_RayTrace *ray2 =
      psmrts_ray_trace_v( &observer2, &lookdir2, bulletTracer );

  // confirm ray2 tracer has a hit on input mesh
  CHECK( psmrts_ray_has_hit( ray2 ) == PSMRTS_TRUE );

  // distance between ray1 and ray2 intercepts (km)
  CHECK_THAT( psmrts_ray2ray_distance( ray, ray2 ),
              Catch::Matchers::WithinAbs( 0.006300, tolerance ) );

  // separation angle (radians) between ray and ray2
  CHECK_THAT( psmrts_separation_angle_radians( &observer, &observer2 ),
              Catch::Matchers::WithinAbs( 0.021313, tolerance ) );

  // validate "nearness" of ray1 and ray2
  CHECK( psmrts_isNear( ray, ray2, 0.3 ) == PSMRTS_TRUE );

  // validate incidence and phase angles (radians) between ray1 and ray2
  CHECK_THAT( psmrts_incidence( ray, ray2 ),
              Catch::Matchers::WithinAbs( 0.506161, tolerance ) );
  CHECK_THAT( psmrts_phase( ray, ray2 ),
              Catch::Matchers::WithinAbs( 0.021313, tolerance ) );

  // validate emission angles for ray1 and ray2
  CHECK_THAT( psmrts_emission( ray ),
              Catch::Matchers::WithinAbs( 0.526907, tolerance ) );
  CHECK_THAT( psmrts_emission( ray2 ),
              Catch::Matchers::WithinAbs( 0.506161, tolerance ) );

  // free memory allocated for ray2
  psmrts_free_ray( ray2 );
}

/**
 * @brief Tests PSMRTS C API functionality for PSMRTS_TraceArray.
 *
 * This test exercises the following functionality in the PSMRTS C API...
 *
 *  1. psmrts_create_trace_array:    Construct PSMRTS_TraceArray.
 *  2. psmrts_trace_array_size:      Get size of PSMRTS_TraceArray.
 *  3. psmrts_trace_array_add_trace: Add trace to PSMRTS_TraceArray.
 *  4. psmrts_trace_array_get_trace: Get trace from PSMRTS_TraceArray.
 *
 */
TEST_CASE ( "PSMRTS C API - Bullet Trace Array", "[capi][c++][BulletTraceArray]" ) {
  const double tolerance = 1.0e-6;

  // create trace array
  PSMRTS_TraceArray *tracearray = psmrts_create_trace_array();

  // verify array is empty
  CHECK( psmrts_trace_array_size( tracearray ) == 0 );

  // create bullet tracer from input obj file
  std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
  PSMRTS_Tracer *bulletTracer = psmrts_create_bullet( objfile.c_str() );

  // validate tracer
  CHECK( psmrts_tracer_valid( bulletTracer ) == PSMRTS_TRUE );

  // create three tracers and add to PSMRTS_TraceArray
  PSMRTS_Vector3d observer1 = psmrts_vector3d( 45.0, 45.0, 3000.0 );
  observer1 = psmrts_lonlatrad_to_xyz_d( &observer1 );
  CHECK_THAT( observer1.x, Catch::Matchers::WithinAbs( 1500.000000, tolerance ) );
  CHECK_THAT( observer1.y, Catch::Matchers::WithinAbs( 1500.000000, tolerance ) );
  CHECK_THAT( observer1.z, Catch::Matchers::WithinAbs( 2121.320344, tolerance ) );

  PSMRTS_Vector3d lookdir1 = psmrts_negate( &observer1 );
  PSMRTS_RayTrace *ray1 = psmrts_create_ray( &observer1, &lookdir1 );
  ray1 = psmrts_ray_trace( ray1, bulletTracer );
  psmrts_trace_array_add_trace( tracearray, ray1 );

  PSMRTS_Vector3d observer2 = psmrts_vector3d( 45.0, 50.0, 5000.0 );
  observer2 = psmrts_lonlatrad_to_xyz_d( &observer2 );
  CHECK_THAT( observer2.x, Catch::Matchers::WithinAbs( 2272.597388, tolerance ) );
  CHECK_THAT( observer2.y, Catch::Matchers::WithinAbs( 2272.597388, tolerance ) );
  CHECK_THAT( observer2.z, Catch::Matchers::WithinAbs( 3830.222216, tolerance ) );

  PSMRTS_Vector3d lookdir2 = psmrts_negate( &observer1 );
  PSMRTS_RayTrace *ray2 = psmrts_create_ray( &observer2, &lookdir2 );
  ray2 = psmrts_ray_trace( ray2, bulletTracer );
  psmrts_trace_array_add_trace( tracearray, ray2 );

  PSMRTS_Vector3d observer3 = psmrts_vector3d( 45.0, 55.0, 5000.0 );
  observer3 = psmrts_lonlatrad_to_xyz_d( &observer3 );
  CHECK_THAT( observer3.x, Catch::Matchers::WithinAbs( 2027.898938, tolerance ) );
  CHECK_THAT( observer3.y, Catch::Matchers::WithinAbs( 2027.898938, tolerance ) );
  CHECK_THAT( observer3.z, Catch::Matchers::WithinAbs( 4095.760221, tolerance ) );

  PSMRTS_Vector3d lookdir3 = psmrts_negate( &observer3 );
  PSMRTS_RayTrace *ray3 = psmrts_create_ray( &observer3, &lookdir3 );
  ray3 = psmrts_ray_trace( ray3, bulletTracer );
  psmrts_trace_array_add_trace( tracearray, ray3 );

  // verify array size is three
  CHECK( psmrts_trace_array_size(tracearray ) == 3 );

  // retrieve arbitrary trace from array
  const PSMRTS_RayTrace *ray2check = psmrts_trace_array_get_trace( tracearray, 1 );
  PSMRTS_Vector3d ray2observer = psmrts_ray_observer( ray2check );

  // validate xyz coordinates of trace
  CHECK_THAT( ray2observer.x, Catch::Matchers::WithinAbs( observer2.x, tolerance ) );
  CHECK_THAT( ray2observer.y, Catch::Matchers::WithinAbs( observer2.y, tolerance ) );
  CHECK_THAT( ray2observer.z, Catch::Matchers::WithinAbs( observer2.z, tolerance ) );

  // free memory
  psmrts_free_ray( ray1 );
  psmrts_free_ray( ray2 );
  psmrts_free_ray( ray3 );
  psmrts_free_trace_array( tracearray );
}

/**
 * @brief Tests PSMRTS C API functionality for the conversion from latitudinal to
 *        rectangular coordinates.
 *
 * This test exercises the PSMRTS C API function psmrts_lonlatrad_to_xyz_d.
 *
 */
TEST_CASE( "PSMRTS C API - Latitudinal to Rectangular Coordinate Conversion", "[capi][c++][utilities][lat2rect][conversion]" ) {
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

/**
 * @brief Tests PSMRTS C API functionality for the conversion from rectangular to
 *        latitudinal coordinates.
 *
 * This test exercises the PSMRTS C API function psmrts_xyz_to_lonlatrad_d.
 *
 */
TEST_CASE( "PSMRTS C API - Rectangular to Latitudinal Coordinate Conversion", "[capi][c++][utilities][rect2lat][conversion]" ) {
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
 * @brief Tests PSMRTS C API degree2radian and radian2degree conversions for
 *        individual numbers and for PSMRTS_Vector3d.
 *
 * This test exercises degree to radian (and vice versa) conversion in the
 * PSMRTS C API...
 *
 * 1. psmrts_degrees_to_radians:  Convert number from degrees to radians.
 * 2. psmrts_radians_to_degrees:  Convert number from radians to degrees.
 * 3. psmrts_vector3d_to_radians: Convert lon/lat in PSMRTS_Vector3d from degrees to radians.
 * 4. psmrts_vector3d_to_degrees: Convert lon/lat in PSMRTS_Vector3d from radians to degrees.
 */
TEST_CASE( "PSMRTS C API - Degree Radian Conversion", "[capi][c++][utilities][deg2rad][rad2deg]" ) {
    const double tolerance = 1.0e-6;

    double deg = 45.7;
    CHECK_THAT( psmrts_degrees_to_radians( deg ),
               Catch::Matchers::WithinAbs( 0.797615, tolerance ) );

    double rad = -4.742;
    CHECK_THAT( psmrts_radians_to_degrees( rad ),
               Catch::Matchers::WithinAbs( -271.696586, tolerance ) );

    PSMRTS_Vector3d llr_d1 = {47.0, 178.0, 3000.0 }; // lon (d), lat (d), radius (km)

    CHECK_THAT( llr_d1.longitude, Catch::Matchers::WithinAbs( 47.0, tolerance ) );
    CHECK_THAT( llr_d1.latitude, Catch::Matchers::WithinAbs( 178.0, tolerance ) );
    CHECK_THAT( llr_d1.radius, Catch::Matchers::WithinAbs( 3000.0, tolerance ) );

    // convert lon, lat in llr_d1 vector from degrees to radians
    PSMRTS_Vector3d llr_r = psmrts_vector3d_to_radians( &llr_d1 );

    CHECK_THAT( llr_r.longitude, Catch::Matchers::WithinAbs( 0.820305, tolerance ) );
    CHECK_THAT( llr_r.latitude, Catch::Matchers::WithinAbs( 3.106686, tolerance ) );
    CHECK_THAT( llr_r.radius, Catch::Matchers::WithinAbs( 3000.0, tolerance ) );

    // convert radians in llr_r vector from radians back to degrees
    PSMRTS_Vector3d llr_d2 = psmrts_vector3d_to_degrees( &llr_r );

    CHECK_THAT( llr_d2.longitude, Catch::Matchers::WithinAbs( 47.0, tolerance ) );
    CHECK_THAT( llr_d2.latitude, Catch::Matchers::WithinAbs( 178.0, tolerance ) );
    CHECK_THAT( llr_d2.radius, Catch::Matchers::WithinAbs( 3000.0, tolerance ) );
}

/**
 * @brief Tests PSMRTS C API Sphere Shape Tracer.
 *
 * This test creates a 1 km radius sphere tracer (PSMRTS_Tracer) with the C API function
 * "psmrts_create_sphere." A nadir-looking observer is positioned 1 km above the sphere's
 * surface at 45° longitude, 50° latitude, 2 km radius. A second observer is positioned at 45°
 * longitude, 45° latitude, 10 km radius looking at 45°, 45°, 1 km on the sphere. We create
 * traces from both observer positions, verify they hit the surface where we expect, and
 * verify the the surface normals and radii at both intercepts.
 *
 */
TEST_CASE( "PSMRTS C API - Sphere Shape Tracer Test", "[capi][c++][sphere][shapetracer]" ) {
  const double tolerance_km = 1.0e-6;

  // create unit sphere tracer (radius = 1.0)
  PSMRTS_Tracer *sphere_tracer = psmrts_create_sphere( 1.0, "sphere" );

  // validate tracer by confirming pointer is not null
  CHECK( psmrts_tracer_valid( sphere_tracer ) == PSMRTS_TRUE );

  // create observer at 45d, 50d, 2km (lon, lat, radius); convert to xyz coordinates
  PSMRTS_Vector3d observer_45_50_02 = psmrts_vector3d( 45.0, 50.0, 2.0 );
  observer_45_50_02 = psmrts_lonlatrad_to_xyz_d( &observer_45_50_02 );

  // create ray trace object with observer = observer_45_50_02 and lookdir = -observer_45_50_02.
  // This is a nadir-looking observation that should intersect the sphere at the subspacecraft
  // point; 45d, 50d, 1km
  PSMRTS_Vector3d look_45_50_02 = psmrts_negate( &observer_45_50_02 );
  PSMRTS_RayTrace *trace_45_50_02 = psmrts_create_ray( &observer_45_50_02, &look_45_50_02 );

  // process trace with sphere_tracer and trace_45_50_02
  trace_45_50_02 = psmrts_ray_trace( trace_45_50_02, sphere_tracer );

  // verify trace hits the sphere
  CHECK( psmrts_ray_has_hit( trace_45_50_02 ) == PSMRTS_TRUE );

  // verify trace intercepts the sphere where we expect; at 45d, 50d, 1km
  PSMRTS_Vector3d intercept_45_50_02_xyz = psmrts_ray_xyz( trace_45_50_02 );
  PSMRTS_Vector3d intercept_45_50_02_llr = psmrts_xyz_to_lonlatrad_d( &intercept_45_50_02_xyz );

  CHECK_THAT( intercept_45_50_02_llr.longitude, Catch::Matchers::WithinAbs( 45.0, tolerance_km ));
  CHECK_THAT( intercept_45_50_02_llr.latitude, Catch::Matchers::WithinAbs( 50.0, tolerance_km ));
  CHECK_THAT( intercept_45_50_02_llr.radius, Catch::Matchers::WithinAbs( 1.0, tolerance_km ));

  // create 2nd observer at 45d, 45d, 10km (lon, lat, radius); convert to xyz coordinates
  PSMRTS_Vector3d observer_45_45_10 = psmrts_vector3d( 45.0, 45.0, 10.0 );
  observer_45_45_10 = psmrts_lonlatrad_to_xyz_d( &observer_45_45_10 );

  // create 2nd observer look direction and ray trace object such that it looks at
  // intercept_45_50_02_xyz
  PSMRTS_Vector3d look_45_45_10 = psmrts_subtract( &intercept_45_50_02_xyz, &observer_45_45_10 );
  PSMRTS_RayTrace *trace_45_45_10 = psmrts_create_ray( &observer_45_45_10, &look_45_45_10 );

  // process 2nd observer trace
  trace_45_45_10 = psmrts_ray_trace( trace_45_45_10, sphere_tracer );

  // verify trace hits the sphere
  CHECK( psmrts_ray_has_hit( trace_45_45_10 ) == PSMRTS_TRUE );

  // verify trace intercepts the sphere where we expect; again at 45d, 50d, 1km
  PSMRTS_Vector3d intercept_45_45_10_xyz = psmrts_ray_xyz( trace_45_45_10 );
  PSMRTS_Vector3d intercept_45_45_10_llr = psmrts_xyz_to_lonlatrad_d( &intercept_45_45_10_xyz );

  CHECK_THAT( intercept_45_45_10_llr.longitude, Catch::Matchers::WithinAbs( 45.0, tolerance_km ));
  CHECK_THAT( intercept_45_45_10_llr.latitude, Catch::Matchers::WithinAbs( 50.0, tolerance_km ));
  CHECK_THAT( intercept_45_45_10_llr.radius, Catch::Matchers::WithinAbs( 1.0, tolerance_km ));

  // retrieve surface normals from each ray trace, they should be identical
  PSMRTS_Vector3d normal_45_45_10 = psmrts_ray_normal( trace_45_45_10 );
  PSMRTS_Vector3d normal_45_50_02 = psmrts_ray_normal( trace_45_50_02 );

  CHECK_THAT( normal_45_45_10.x, Catch::Matchers::WithinAbs( normal_45_50_02.x, tolerance_km ));
  CHECK_THAT( normal_45_45_10.y, Catch::Matchers::WithinAbs( normal_45_50_02.y, tolerance_km ));
  CHECK_THAT( normal_45_45_10.z, Catch::Matchers::WithinAbs( normal_45_50_02.z, tolerance_km ));

  // verify radii from both surface intercepts are equivalent
  CHECK_THAT( psmrts_ray_intercept_radius( trace_45_45_10 ),
              Catch::Matchers::WithinAbs( psmrts_ray_intercept_radius( trace_45_50_02 ), tolerance_km ) );

  // TBD: add facet functionality, e.g.
  // psmrts::PRQFacet prq_facet( prq_ray.trace() );
  // CHECK( prq_facet.isValid()                == true ); // Returns ray validity?
  // CHECK( e_tracer.process( prq_facet )      == false );
  // CHECK( prq_facet.trace().segment_number() == -1 );
  // CHECK( prq_facet.trace().plateid()        == -1 );
  // etc

  // free memory
  psmrts_free_tracer( sphere_tracer );
  psmrts_free_ray( trace_45_50_02 );
  psmrts_free_ray( trace_45_45_10 );
}

/**
 * @brief Tests PSMRTS C API Spheroid Shape Tracer.
 *
 * This test creates an oblate (or flattened) spheroid (radii: a,b = 1.0; c = 0.5) PSMRTS_Tracer
 * with the C API function "psmrts_create_spheroid." A nadir-looking observer is positioned 1 km
 * above the sphere's surface at 45°, 50°, 2 km (lon, lat, radius). A second observer is
 * positioned at 45°, 45°, 10 km (lon, lat, radius) looking at 45°, 50° on the sphere. We create
 * traces from both observer positions, verify they hit the sphere surface where we expect, and
 * verify the surface normals and radii at both intercepts.
 *
 * TBD: VERIFY INTERCEPT RADII ARE CORRECT
 *
 */
TEST_CASE( "PSMRTS C API - Spheroid Shape Tracer Test", "[capi][c++][spheroid][shapetracer]" ) {
  const double tolerance_km = 1.0e-6;

  // create spheroid tracer with radii: a,b = 1.0, c = 0.5 (i.e. an oblate or flattened spheroid)
  PSMRTS_Tracer *spheroid_tracer = psmrts_create_spheroid( 1.0, 0.5, "spheroid" );

  // validate tracer (confirms pointer is not null)
  CHECK( psmrts_tracer_valid( spheroid_tracer ) == PSMRTS_TRUE );

  // create observer at 45°, 50°, 2km (lon, lat, radius); convert to xyz (rectangular) coordinates
  PSMRTS_Vector3d observer_45_50_02 = psmrts_vector3d( 45.0, 50.0, 2.0 );
  observer_45_50_02 = psmrts_lonlatrad_to_xyz_d( &observer_45_50_02 );

  // create PSMRTS_RayTrace with observer = observer_45_50_02 and lookdir = -observer_45_50_02.
  // This is a nadir-looking observation that should intersect the spheroid at the subspacecraft
  // point; 45° lon, 50° lat.
  PSMRTS_Vector3d look_45_50_02 = psmrts_negate( &observer_45_50_02 );
  PSMRTS_RayTrace *trace_45_50_02 = psmrts_create_ray( &observer_45_50_02, &look_45_50_02 );

  // run trace_45_50_02 with spheroid_tracer
  trace_45_50_02 = psmrts_ray_trace( trace_45_50_02, spheroid_tracer );

  // verify trace hits the spheroid
  CHECK( psmrts_ray_has_hit( trace_45_50_02 ) == PSMRTS_TRUE );

  // verify trace_45_50_02 intercepts spheroid surface where we expect; at 45°, 50° (lon, lat)
  PSMRTS_Vector3d intercept_45_50_02_xyz = psmrts_ray_xyz( trace_45_50_02 );
  PSMRTS_Vector3d intercept_45_50_02_llr = psmrts_xyz_to_lonlatrad_d( &intercept_45_50_02_xyz );

  CHECK_THAT( intercept_45_50_02_llr.longitude, Catch::Matchers::WithinAbs( 45.0, tolerance_km ));
  CHECK_THAT( intercept_45_50_02_llr.latitude, Catch::Matchers::WithinAbs( 50.0, tolerance_km ));
  CHECK_THAT( intercept_45_50_02_llr.radius, Catch::Matchers::WithinAbs( 0.601878, tolerance_km ));
  // NOTE: VERIFY RADIUS IS CORRECT!!!

  // create 2nd observer at 45°, 45°, 10km (lon, lat, radius); convert to xyz coordinates
  PSMRTS_Vector3d observer_45_45_10 = psmrts_vector3d( 45.0, 45.0, 10.0 );
  observer_45_45_10 = psmrts_lonlatrad_to_xyz_d( &observer_45_45_10 );

  // create 2nd observer look direction and ray trace such that it looks at intercept_45_50_02_xyz
  PSMRTS_Vector3d look_45_45_10 = psmrts_subtract( &intercept_45_50_02_xyz, &observer_45_45_10 );
  PSMRTS_RayTrace *trace_45_45_10 = psmrts_create_ray( &observer_45_45_10, &look_45_45_10 );

  // run trace_45_45_10 with spheroid_tracer
  trace_45_45_10 = psmrts_ray_trace( trace_45_45_10, spheroid_tracer );

  // verify trace hits the spheroid
  CHECK( psmrts_ray_has_hit( trace_45_45_10 ) == PSMRTS_TRUE );

  // verify trace intercepts the spheroid where we expect; again at 45°, 50° (lon, lat)
  PSMRTS_Vector3d intercept_45_45_10_xyz = psmrts_ray_xyz( trace_45_45_10 );
  PSMRTS_Vector3d intercept_45_45_10_llr = psmrts_xyz_to_lonlatrad_d( &intercept_45_45_10_xyz );

  CHECK_THAT( intercept_45_45_10_llr.longitude, Catch::Matchers::WithinAbs( 45.0, tolerance_km ));
  CHECK_THAT( intercept_45_45_10_llr.latitude, Catch::Matchers::WithinAbs( 50.0, tolerance_km ));
  CHECK_THAT( intercept_45_45_10_llr.radius, Catch::Matchers::WithinAbs( 0.601878, tolerance_km ));
  // NOTE: VERIFY RADIUS IS CORRECT!!!

  // get surface normals from each trace, should be identical as surface intercept is the same
  PSMRTS_Vector3d normal_45_45_10 = psmrts_ray_normal( trace_45_45_10 );
  PSMRTS_Vector3d normal_45_50_02 = psmrts_ray_normal( trace_45_50_02 );

  CHECK_THAT( normal_45_45_10.x, Catch::Matchers::WithinAbs( normal_45_50_02.x, tolerance_km ));
  CHECK_THAT( normal_45_45_10.y, Catch::Matchers::WithinAbs( normal_45_50_02.y, tolerance_km ));
  CHECK_THAT( normal_45_45_10.z, Catch::Matchers::WithinAbs( normal_45_50_02.z, tolerance_km ));

  // verify radii from both surface intercepts are equivalent
  CHECK_THAT( psmrts_ray_intercept_radius( trace_45_45_10 ),
              Catch::Matchers::WithinAbs( psmrts_ray_intercept_radius( trace_45_50_02 ),
                                          tolerance_km ) );

  // TBD: add facet functionality
  // psmrts::PRQFacet prq_facet( prq_ray.trace() );
  // CHECK( prq_facet.isValid()                == true ); // Returns ray validity?
  // CHECK( e_tracer.process( prq_facet )      == false );
  // CHECK( prq_facet.trace().segment_number() == -1 );
  // CHECK( prq_facet.trace().plateid()        == -1 );
  // etc

  // free memory
  psmrts_free_tracer( spheroid_tracer );
  psmrts_free_ray( trace_45_50_02 );
  psmrts_free_ray( trace_45_45_10 );
}

/**
 * @brief Tests PSMRTS C API Ellipsoid Shape Tracer.
 *
 * This test creates an ellipsoid PSMRTS_Tracer (radii (a,b,c): 2.0, 1.0, 0.5) with the C API
 * function "psmrts_create_ellipsoid." A nadir-looking observer is positioned above the ellipsoid
 * surface at 45°, 50°, 2 km (lon, lat, radius). A second observer is positioned at 45°, 45°,
 * 10 km (lon, lat, radius) looking at 45°, 45° (lon, lat) on the ellipsoid. We create traces from
 * both observer positions, verify they hit the surface where we expect, and verify the surface
 * normals and radii at both intercepts.
 *
 * TBD: VERIFY INTERCEPT RADII ARE CORRECT
 *
 */
TEST_CASE( "PSMRTS C API - Ellipsoid Shape Tracer Test", "[capi][c++][ellipsoid][shapetracer]" ) {
  const double tolerance_km = 1.0e-6;

  // create ellipsoid tracer a = 2.0, b = 1.0, c = 0.5
  PSMRTS_Tracer *e_tracer = psmrts_create_ellipsoid( 2.0, 1.0, 0.5, "ellipsoid" );

  // validate tracer (confirms pointer is not null)
  CHECK( psmrts_tracer_valid( e_tracer ) == PSMRTS_TRUE );

  // create observer at 45°, 50°, 2km (lon, lat, radius); convert to xyz (rectangular) coordinates
  PSMRTS_Vector3d observer_45_50_02 = psmrts_vector3d( 45.0, 50.0, 2.0 );
  observer_45_50_02 = psmrts_lonlatrad_to_xyz_d( &observer_45_50_02 );

  // create PSMRTS_RayTrace with observer = observer_45_50_02 and lookdir = -observer_45_50_02.
  // This is a nadir-looking observation that should intersect the ellipsoid at the
  // subspacecraft point; 45°, 50° (lon, lat)
  PSMRTS_Vector3d look_45_50_02 = psmrts_negate( &observer_45_50_02 );
  PSMRTS_RayTrace *trace_45_50_02 = psmrts_create_ray( &observer_45_50_02, &look_45_50_02 );

  // run trace_45_50_02 with e_tracer
  trace_45_50_02 = psmrts_ray_trace( trace_45_50_02, e_tracer );

  // verify trace hits the ellipsoid
  CHECK( psmrts_ray_has_hit( trace_45_50_02 ) == PSMRTS_TRUE );

  // verify trace intercepts the ellipsoid where we expect; at 45°, 50° (lon, lat)
  PSMRTS_Vector3d intercept_45_50_02_xyz = psmrts_ray_xyz( trace_45_50_02 );
  PSMRTS_Vector3d intercept_45_50_02_llr = psmrts_xyz_to_lonlatrad_d( &intercept_45_50_02_xyz );

  CHECK_THAT( intercept_45_50_02_llr.longitude, Catch::Matchers::WithinAbs( 45.0, tolerance_km ));
  CHECK_THAT( intercept_45_50_02_llr.latitude, Catch::Matchers::WithinAbs( 50.0, tolerance_km ));
  CHECK_THAT( intercept_45_50_02_llr.radius, Catch::Matchers::WithinAbs( 0.619515, tolerance_km ));
  // NOTE: VERIFY RADIUS IS CORRECT!!!

  // create 2nd observer at 45°, 45°, 10km (lon, lat, radius); convert to xyz coordinates
  PSMRTS_Vector3d observer_45_45_10 = psmrts_vector3d( 45.0, 45.0, 10.0 );
  observer_45_45_10 = psmrts_lonlatrad_to_xyz_d( &observer_45_45_10 );

  // create 2nd observer look direction & PSMRTS_RayTrace such that it looks at intercept_45_50_02_xyz
  PSMRTS_Vector3d look_45_45_10 = psmrts_subtract( &intercept_45_50_02_xyz, &observer_45_45_10 );
  PSMRTS_RayTrace *trace_45_45_10 = psmrts_create_ray( &observer_45_45_10, &look_45_45_10 );

  // run trace_45_45_10 with e_tracer
  trace_45_45_10 = psmrts_ray_trace( trace_45_45_10, e_tracer );

  // verify trace hits the ellipsoid
  CHECK( psmrts_ray_has_hit( trace_45_45_10 ) == PSMRTS_TRUE );

  // verify trace intercepts the ellipsoid where we expect; again at 45°, 50° (lon, lat)
  PSMRTS_Vector3d intercept_45_45_10_xyz = psmrts_ray_xyz( trace_45_45_10 );
  PSMRTS_Vector3d intercept_45_45_10_llr = psmrts_xyz_to_lonlatrad_d( &intercept_45_45_10_xyz );

  CHECK_THAT( intercept_45_45_10_llr.longitude, Catch::Matchers::WithinAbs( 45.0, tolerance_km ));
  CHECK_THAT( intercept_45_45_10_llr.latitude, Catch::Matchers::WithinAbs( 50.0, tolerance_km ));
  CHECK_THAT( intercept_45_45_10_llr.radius, Catch::Matchers::WithinAbs( 0.619515, tolerance_km ));
  // NOTE: VERIFY RADIUS IS CORRECT!!!

  // get surface normals from each trace, should be identical as surface intercept is the same
  PSMRTS_Vector3d normal_45_45_10 = psmrts_ray_normal( trace_45_45_10 );
  PSMRTS_Vector3d normal_45_50_02 = psmrts_ray_normal( trace_45_50_02 );

  CHECK_THAT( normal_45_45_10.x, Catch::Matchers::WithinAbs( normal_45_50_02.x, tolerance_km ));
  CHECK_THAT( normal_45_45_10.y, Catch::Matchers::WithinAbs( normal_45_50_02.y, tolerance_km ));
  CHECK_THAT( normal_45_45_10.z, Catch::Matchers::WithinAbs( normal_45_50_02.z, tolerance_km ));

  // verify radii from both surface intercepts are equivalent
  CHECK_THAT( psmrts_ray_intercept_radius( trace_45_45_10 ),
              Catch::Matchers::WithinAbs( psmrts_ray_intercept_radius( trace_45_50_02 ), tolerance_km ) );

  // TBD: add facet functionality
  // psmrts::PRQFacet prq_facet( prq_ray.trace() );
  // CHECK( prq_facet.isValid()                == true ); // Returns ray validity?
  // CHECK( e_tracer.process( prq_facet )      == false );
  // CHECK( prq_facet.trace().segment_number() == -1 );
  // CHECK( prq_facet.trace().plateid()        == -1 );
  // etc

  // free memory
  psmrts_free_tracer( e_tracer );
  psmrts_free_ray( trace_45_50_02 );
  psmrts_free_ray( trace_45_45_10 );
}

/**
 * @brief Tests PSMRTS C API Ellipsoid V Shape Tracer.
 *
 * This test creates an ellipsoid PSMRTS_Tracer (radii (a,b,c): 2.0, 1.0, 0.5) with the C API
 * function "psmrts_create_ellipsoid_v." This differs from the previous test in that the radii
 * are set via a PSMRTS_Vector3d. A nadir-looking observer is positioned above the ellipsoid
 * surface at 45°, 50°, 2 km (lon, lat, radius). A second observer is positioned at 45°, 45°,
 * 10 km (lon, lat, radius) looking at 45°, 45° (lon, lat) on the ellipsoid. We create traces
 * from both observer positions, verify they hit the surface where we expect, and verify the
 * surface normals and radii at both intercepts.
 *
 * TBD: VERIFY INTERCEPT RADII ARE CORRECT
 *
 */
TEST_CASE( "PSMRTS C API - Ellipsoid V Shape Tracer Test", "[capi][c++][ellipsoidv][shapetracer]" ) {
  const double tolerance_km = 1.0e-6;

  // create ellipsoid tracer a = 2.0, b = 1.0, c = 0.5
  PSMRTS_Vector3d radii = psmrts_vector3d(2.0, 1.0, 0.5);
  PSMRTS_Tracer *ev_tracer = psmrts_create_ellipsoid_v( &radii, "ellipsoidv" );

  // validate tracer (confirms pointer is not null)
  CHECK( psmrts_tracer_valid( ev_tracer ) == PSMRTS_TRUE );

  // create observer at 45°, 50°, 2km (lon, lat, radius); convert to xyz (rectangular) coordinates
  PSMRTS_Vector3d observer_45_50_02 = psmrts_vector3d( 45.0, 50.0, 2.0 );
  observer_45_50_02 = psmrts_lonlatrad_to_xyz_d( &observer_45_50_02 );

  // create PSMRTS_RayTrace with observer = observer_45_50_02 and lookdir = -observer_45_50_02.
  // This is a nadir-looking observation that should intersect the ellipsoid at the
  // subspacecraft point; 45°, 50° (lon, lat)
  PSMRTS_Vector3d look_45_50_02 = psmrts_negate( &observer_45_50_02 );
  PSMRTS_RayTrace *trace_45_50_02 = psmrts_create_ray( &observer_45_50_02, &look_45_50_02 );

  // run trace_45_50_02 with e_tracer
  trace_45_50_02 = psmrts_ray_trace( trace_45_50_02, ev_tracer );

  // verify trace hits the ellipsoid
  CHECK( psmrts_ray_has_hit( trace_45_50_02 ) == PSMRTS_TRUE );

  // verify trace intercepts the ellipsoid where we expect; at 45°, 50° (lon, lat)
  PSMRTS_Vector3d intercept_45_50_02_xyz = psmrts_ray_xyz( trace_45_50_02 );
  PSMRTS_Vector3d intercept_45_50_02_llr = psmrts_xyz_to_lonlatrad_d( &intercept_45_50_02_xyz );

  CHECK_THAT( intercept_45_50_02_llr.longitude, Catch::Matchers::WithinAbs( 45.0, tolerance_km ));
  CHECK_THAT( intercept_45_50_02_llr.latitude, Catch::Matchers::WithinAbs( 50.0, tolerance_km ));
  CHECK_THAT( intercept_45_50_02_llr.radius, Catch::Matchers::WithinAbs( 0.619515, tolerance_km ));
  // NOTE: VERIFY RADIUS IS CORRECT!!!

  // create 2nd observer look direction & PSMRTS_RayTrace such that it looks at intercept_45_50_02_xyz
  PSMRTS_Vector3d observer_45_45_10 = psmrts_vector3d( 45.0, 45.0, 10.0 );
  observer_45_45_10 = psmrts_lonlatrad_to_xyz_d( &observer_45_45_10 );

  // create 2nd observer look direction & PSMRTS_RayTrace such that it looks at intercept_45_50_02_xyz
  PSMRTS_Vector3d look_45_45_10 = psmrts_subtract( &intercept_45_50_02_xyz, &observer_45_45_10 );
  PSMRTS_RayTrace *trace_45_45_10 = psmrts_create_ray( &observer_45_45_10, &look_45_45_10 );

  // run trace_45_45_10 with e_tracer
    trace_45_45_10 = psmrts_ray_trace( trace_45_45_10, ev_tracer );

  // verify trace hits the ellipsoid
  CHECK( psmrts_ray_has_hit( trace_45_45_10 ) == PSMRTS_TRUE );

  // verify trace intercepts the ellipsoid where we expect; again at 45°, 50° (lon, lat)
  PSMRTS_Vector3d intercept_45_45_10_xyz = psmrts_ray_xyz( trace_45_45_10 );
  PSMRTS_Vector3d intercept_45_45_10_llr = psmrts_xyz_to_lonlatrad_d( &intercept_45_45_10_xyz );

  CHECK_THAT( intercept_45_45_10_llr.longitude, Catch::Matchers::WithinAbs( 45.0, tolerance_km ));
  CHECK_THAT( intercept_45_45_10_llr.latitude, Catch::Matchers::WithinAbs( 50.0, tolerance_km ));
  CHECK_THAT( intercept_45_45_10_llr.radius, Catch::Matchers::WithinAbs( 0.619515, tolerance_km ));
  // NOTE: VERIFY RADIUS IS CORRECT!!!

  // get surface normals from each trace, should be identical as surface intercept is the same
  PSMRTS_Vector3d normal_45_45_10 = psmrts_ray_normal( trace_45_45_10 );
  PSMRTS_Vector3d normal_45_50_02 = psmrts_ray_normal( trace_45_50_02 );

  CHECK_THAT( normal_45_45_10.x, Catch::Matchers::WithinAbs( normal_45_50_02.x, tolerance_km ));
  CHECK_THAT( normal_45_45_10.y, Catch::Matchers::WithinAbs( normal_45_50_02.y, tolerance_km ));
  CHECK_THAT( normal_45_45_10.z, Catch::Matchers::WithinAbs( normal_45_50_02.z, tolerance_km ));

  // verify radii from both surface intercepts are equivalent
  CHECK_THAT( psmrts_ray_intercept_radius( trace_45_45_10 ),
              Catch::Matchers::WithinAbs( psmrts_ray_intercept_radius( trace_45_50_02 ),
                                          tolerance_km ) );

    // TBD: add facet functionality
    // psmrts::PRQFacet prq_facet( prq_ray.trace() );
    // CHECK( prq_facet.isValid()                == true ); // Returns ray validity?
    // CHECK( e_tracer.process( prq_facet )      == false );
    // CHECK( prq_facet.trace().segment_number() == -1 );
    // CHECK( prq_facet.trace().plateid()        == -1 );
    // etc

    // free memory
    psmrts_free_tracer( ev_tracer );
    psmrts_free_ray( trace_45_50_02 );
    psmrts_free_ray( trace_45_45_10 );
}
