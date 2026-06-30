#include <psmrts/core/tests/psmrts_catch2_environment.hpp>
#include <psmrts/capi/psmrts_c.h>

#include <cstring>
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

      // explicit test of psmrts_ray_lookdir method
      PSMRTS_Vector3d lookdir_result = psmrts_ray_lookdir(ray);
      CHECK_THAT( lookdir_result.x,
                  Catch::Matchers::WithinAbs( -1500.0, tolerance ) );
      CHECK_THAT( lookdir_result.y,
                  Catch::Matchers::WithinAbs( -1500.0, tolerance ) );
      CHECK_THAT( lookdir_result.z,
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

    virtual ~bulletTraceFixture() { // tear down code
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
 * @brief PSMRTS C API Test to verify PSMRTS Version and Info strings
 *
 * This test verifies psmrts_version() and psmrts_info() methods in the C API.
 *
 */
TEST_CASE ( "PSMRTS C API - Version and Info", "[capi][c++][version][info]" ) {
  CHECK( std::string(psmrts_version()) == psmrts_version() );
  // CHECK( std::string(psmrts_info()) == "PSMRTS-0.4.1" );
}

/**
 * @brief PSMRTS C API PSMRTS_ProductConfiguration functionality test.
 *
 * This test validates the following PSMRTS_ProductConfiguration methods:
 *   
 *   1) psmrts_create_product_config
 *   2) psmrts_add_config_options_bool
 *   3) psmrts_add_config_options_int
 *   4) psmrts_add_config_options_sizet
 *   5) psmrts_add_config_options_double
 *   6) psmrts_product_config_contains
 *   7) psmrts_product_config_to_string
 *   8) psmrts_free_product_config
 * 
 */
TEST_CASE ( "PSMRTS C API - Product Configuration", "[capi][config][options]" ) {

  // create PSMRTS_ProductConfiguration
  PSMRTS_ProductConfiguration *config = psmrts_create_product_config( "config" );

  // add ProductOptions to config
  psmrts_add_product_string( config, "string", "Casablanca" );
  psmrts_add_product_bool( config, "bool", 1 );
  psmrts_add_product_int( config, "int", -27 );
  psmrts_add_product_sizet( config, "size_t", 27 );
  psmrts_add_product_double( config, "double", 3.141593 );

  double darray[] = {1.1, 2.2, 3.3};
  psmrts_add_product_double_vector( config, "double vector", darray, 3 );

  // verify all ProductOptions are in config
  CHECK( psmrts_product_config_contains( config, "string" ) == PSMRTS_TRUE );
  CHECK( psmrts_product_config_contains( config, "bool" )   == PSMRTS_TRUE );
  CHECK( psmrts_product_config_contains( config, "int" )    == PSMRTS_TRUE );
  CHECK( psmrts_product_config_contains( config, "size_t" ) == PSMRTS_TRUE );
  CHECK( psmrts_product_config_contains( config, "double" ) == PSMRTS_TRUE );
  CHECK( psmrts_product_config_contains( config, "double vector" ) == PSMRTS_TRUE );

  // verify meta data for all ProductOptions added to config
  PSMRTS_String *pstr1 = psmrts_create_string( "" );
  psmrts_product_config_to_string( config, pstr1 );
  CHECK( std::string( psmrts_string_content( pstr1 ) ) == R"({"options":{"string":"Casablanca","bool":1,"int":-27,"size_t":27,"double":3.141593,"double vector":[1.1,2.2,3.3]},"metadata":{}})");

  // replace an existing ProductOption
  psmrts_add_product_int( config, "int", -270 );

  // re-verify meta data for all ProductOptions added to config
  PSMRTS_String *pstr2 = psmrts_create_string( "" );
  psmrts_product_config_to_string( config, pstr2 );
  CHECK( std::string( psmrts_string_content( pstr2 ) ) == R"({"options":{"string":"Casablanca","bool":1,"int":-270,"size_t":27,"double":3.141593,"double vector":[1.1,2.2,3.3]},"metadata":{}})");

  // free strings and product configuration memory
  psmrts_free_string( pstr1 );
  psmrts_free_string( pstr2 );
  psmrts_free_product_config( config );
}

/**
 * @brief PSMRTS C API Default string functionality test.
 *
 * This test verifies the following PSMRTS_String methods:
 * 
 *   1) psmrts_create_string
 *   2) psmrts_string_content
 *   3) psmrts_string_length
 *   4) psmrts_free_string
 *
 */
TEST_CASE ( "PSMRTS C API - Strings", "[capi][strings][default]" ) {
//  using PSTRING = std::unique_ptr<PSMRTS_String, psmrts_free_string>;
//  PSTRING ps = PSTRING( psmrts_create_string( "you talking to me?" ) );
//  CHECK( psmrts_string_length( ps ) == 19 );

  // create pointer to string
  PSMRTS_String *strTest1 = psmrts_create_string( "you talking to me?" );
 
  // confirm string content via c++ approach
  CHECK( std::string( psmrts_string_content( strTest1 ) ) == "you talking to me?" );
  
  // confirm string content vis c approach
  CHECK( strcmp( psmrts_string_content( strTest1 ), "you talking to me?") == 0 );

  // confirm string length
  CHECK( psmrts_string_length( strTest1 ) == strlen("you talking to me?") );

  // free string memory
  psmrts_free_string( strTest1 );
}

/**
 * @brief PSMRTS C API Default string array functionality test.
 *
 * This test verifies the following PSMRTS_StringArray methods:
 * 
 *   1) psmrts_create_string_array
 *   2) psmrts_string_array_size
 *   3) psmrts_string_array_add_string
 *   4) psmrts_string_array_clear
 *   5) psmrts_string_array_get_string
 *   6) psmrts_free_string_array
 *
 */
TEST_CASE ( "PSMRTS C API - String Array", "[capi][string][array][default]" ) {
  // create pointer to string array
  PSMRTS_StringArray *stringarray = psmrts_create_string_array();

  // verify initial string array size is zero
  CHECK( psmrts_string_array_size( stringarray ) == 0 );

  // add strings to array, validating position of each
  size_t string_pos;
  string_pos = psmrts_string_array_add_string( stringarray, "Humphrey Bogart" );
  CHECK( string_pos == 0 );

  string_pos = psmrts_string_array_add_string( stringarray, "Ingrid Bergman" );
  CHECK( string_pos == 1 );

  string_pos = psmrts_string_array_add_string( stringarray, "Paul Henreid" );
  CHECK( string_pos == 2 );

  string_pos = psmrts_string_array_add_string( stringarray, "Claude Rains" );
  CHECK( string_pos == 3 );

  // verify string array size is now four
  CHECK( psmrts_string_array_size( stringarray ) == 4 );

  // retrieve string at position 2 and confirm content
  const PSMRTS_String* checkstring = psmrts_string_array_get_string( stringarray, 2 );
  CHECK( strcmp( psmrts_string_content( checkstring ), "Paul Henreid") == 0 );
  
  // clear string array and verify size is again 0
  psmrts_string_array_clear( stringarray );
  CHECK( psmrts_string_array_size( stringarray ) == 0 );

  // free string array
  psmrts_free_string_array( stringarray );
}

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

    PSMRTS_Vector3i v3i = psmrts_vector3i( 1, 2, 3 );
    CHECK( v3i.i == 1 );
    CHECK( v3i.j == 2 );
    CHECK( v3i.k == 3 );
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
 * 15. psmrts_get_facet:                    Get facet.
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

  // Validate facet
  PSMRTS_Facet facet;
  CHECK( psmrts_get_facet( ray, bulletTracer, &facet ) == PSMRTS_TRUE );
  CHECK( facet.m_has_facet == PSMRTS_TRUE );

  CHECK( facet.m_plateid == 30 );
  CHECK( facet.m_segment == 0 );

  CHECK( facet.m_indexes.i == 11 );
  CHECK( facet.m_indexes.j == 14 );
  CHECK( facet.m_indexes.k == 5 );  

  CHECK_THAT( facet.m_vector1.x,
              Catch::Matchers::WithinAbs( 0.101004, tolerance ) );
  CHECK_THAT( facet.m_vector1.y,
              Catch::Matchers::WithinAbs( 0.000000, tolerance ) );
  CHECK_THAT( facet.m_vector1.z,
              Catch::Matchers::WithinAbs( 0.264431, tolerance ) );
  CHECK_THAT( facet.m_vector2.x,
              Catch::Matchers::WithinAbs( 0.163428, tolerance ) );
  CHECK_THAT( facet.m_vector2.y,
              Catch::Matchers::WithinAbs( 0.163428, tolerance ) );
  CHECK_THAT( facet.m_vector2.z,
              Catch::Matchers::WithinAbs( 0.163428, tolerance ) );
  CHECK_THAT( facet.m_vector3.x,
              Catch::Matchers::WithinAbs( 0.000000, tolerance ) );
  CHECK_THAT( facet.m_vector3.y,
              Catch::Matchers::WithinAbs( 0.264432, tolerance ) );
  CHECK_THAT( facet.m_vector3.z,
              Catch::Matchers::WithinAbs( 0.101004, tolerance ) );
  CHECK_THAT( facet.m_normal.x,
              Catch::Matchers::WithinAbs( 0.000000, tolerance ) );
  CHECK_THAT( facet.m_normal.y,
              Catch::Matchers::WithinAbs( 0.525731, tolerance ) );
  CHECK_THAT( facet.m_normal.z,
              Catch::Matchers::WithinAbs( 0.850651, tolerance ) );
  CHECK_THAT( psmrts_facet_surface_area( &facet ),
              Catch::Matchers::WithinAbs( 0.019405, tolerance ) );
  CHECK_THAT( psmrts_facet_volume( &facet ),
              Catch::Matchers::WithinAbs( 0.001455, tolerance ) );
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
 * 12. psmrts_get_facet:                Get facet.
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
  PSMRTS_RayTrace *ray2 = psmrts_ray_trace_v( &observer2, &lookdir2, bulletTracer );

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

  // Validate facet1 values
  PSMRTS_Facet facet1;
  CHECK( psmrts_get_facet( ray, bulletTracer, &facet1 ) == PSMRTS_TRUE );
  CHECK( facet1.m_has_facet == PSMRTS_TRUE );

  CHECK( facet1.m_plateid == 30 );
  CHECK( facet1.m_segment == 0 );

  CHECK( facet1.m_indexes.i == 11 );
  CHECK( facet1.m_indexes.j == 14 );
  CHECK( facet1.m_indexes.k == 5 );

  CHECK_THAT( facet1.m_vector1.x,
              Catch::Matchers::WithinAbs( 0.101004, tolerance ) );
  CHECK_THAT( facet1.m_vector1.y,
              Catch::Matchers::WithinAbs( 0.000000, tolerance ) );
  CHECK_THAT( facet1.m_vector1.z,
              Catch::Matchers::WithinAbs( 0.264431, tolerance ) );
  CHECK_THAT( facet1.m_vector2.x,
              Catch::Matchers::WithinAbs( 0.163428, tolerance ) );
  CHECK_THAT( facet1.m_vector2.y,
              Catch::Matchers::WithinAbs( 0.163428, tolerance ) );
  CHECK_THAT( facet1.m_vector2.z,
              Catch::Matchers::WithinAbs( 0.163428, tolerance ) );
  CHECK_THAT( facet1.m_vector3.x,
              Catch::Matchers::WithinAbs( 0.000000, tolerance ) );
  CHECK_THAT( facet1.m_vector3.y,
              Catch::Matchers::WithinAbs( 0.2644315, tolerance ) );
  CHECK_THAT( facet1.m_vector3.z,
              Catch::Matchers::WithinAbs( 0.101004, tolerance ) );
  CHECK_THAT( facet1.m_normal.x,
              Catch::Matchers::WithinAbs( 0.000000, tolerance ) );
  CHECK_THAT( facet1.m_normal.y,
              Catch::Matchers::WithinAbs( 0.525731, tolerance ) );
  CHECK_THAT( facet1.m_normal.z,
              Catch::Matchers::WithinAbs( 0.850651, tolerance ) );

  // Verify facets from both traces are identical
  PSMRTS_Facet facet2;
  CHECK( psmrts_get_facet( ray2, bulletTracer, &facet2 ) == PSMRTS_TRUE );
  CHECK( facet2.m_has_facet == facet1.m_has_facet );

  CHECK( facet2.m_plateid == facet1.m_plateid );
  CHECK( facet2.m_segment == facet1.m_segment );

  CHECK( facet2.m_indexes.i == facet1.m_indexes.i );
  CHECK( facet2.m_indexes.j == facet1.m_indexes.j );
  CHECK( facet2.m_indexes.k == facet1.m_indexes.k );

  CHECK( facet2.m_vector1.x == facet1.m_vector1.x );
  CHECK( facet2.m_vector1.y == facet1.m_vector1.y );
  CHECK( facet2.m_vector1.z == facet1.m_vector1.z );

  CHECK( facet2.m_vector2.x == facet1.m_vector2.x );
  CHECK( facet2.m_vector2.y == facet1.m_vector2.y );
  CHECK( facet2.m_vector2.z == facet1.m_vector2.z );

  CHECK( facet2.m_vector3.x == facet1.m_vector3.x );
  CHECK( facet2.m_vector3.y == facet1.m_vector3.y );
  CHECK( facet2.m_vector3.z == facet1.m_vector3.z );

  CHECK( facet2.m_normal.x == facet1.m_normal.x );
  CHECK( facet2.m_normal.y == facet1.m_normal.y );
  CHECK( facet2.m_normal.z == facet1.m_normal.z );

  // free memory allocated for ray2
  psmrts_free_ray( ray2 );
}

/**
 * @brief Compares Bullet and DSK PSMRTS_RayTrace functionality.
 *
 * This test derives from the 'bulletTraceFixture' at the top of this file. A naif dsk tracer
 * is created from a "bds" file equivalent to the "obj" file used to create a bullet tracer.
 * Pertinet values from both are compared, showing that that they are identical (as they should
 * be).
 *
 */
TEST_CASE_METHOD ( bulletTraceFixture, "PSMRTS C API - NAIF vs Bullet", "[capi][c++][NAIFvsBullet]" ) {

    // bulletTraceFixture setup code has already been executed.

    // create naif tracer from bds file, identical to corresponding obj file used for bullet
    std::string dskfile = psmrts_shapes_path( "dsk/data/bennu_20facets.bds" );
    PSMRTS_Tracer *dskTracer = psmrts_create_naifdsk( dskfile.c_str() );

    // validate tracer
    CHECK( psmrts_tracer_valid( dskTracer ) == PSMRTS_TRUE );

    PSMRTS_RayTrace *dsk_ray = psmrts_create_ray( &observer, &lookdir );

    // run trace with dsk_ray and dskTracer
    dsk_ray = psmrts_ray_trace( dsk_ray, dskTracer );

    // confirm tracer has a hit on input mesh
    CHECK( psmrts_ray_has_hit( dsk_ray ) == PSMRTS_TRUE );

    // Verify bullet & naif surface intercepts are identical
    PSMRTS_Vector3d xyz     = psmrts_ray_xyz( ray );
    PSMRTS_Vector3d dsk_xyz = psmrts_ray_xyz( dsk_ray );
    CHECK_THAT( xyz.x, Catch::Matchers::WithinAbs( dsk_xyz.x, tolerance ) );
    CHECK_THAT( xyz.y, Catch::Matchers::WithinAbs( dsk_xyz.y, tolerance ) );
    CHECK_THAT( xyz.z, Catch::Matchers::WithinAbs( dsk_xyz.z, tolerance ) );

    PSMRTS_Vector3d llr = psmrts_xyz_to_lonlatrad_d( &xyz );
    PSMRTS_Vector3d dsk_llr = psmrts_xyz_to_lonlatrad_d( &dsk_xyz );
    CHECK_THAT( llr.longitude, Catch::Matchers::WithinAbs( dsk_llr.longitude, tolerance ) );
    CHECK_THAT( llr.latitude, Catch::Matchers::WithinAbs( dsk_llr.latitude, tolerance ) );
    CHECK_THAT( llr.radius, Catch::Matchers::WithinAbs( dsk_llr.radius, tolerance ) );

    // Verify bullet & naif vectors along PSMRTS_RayTrace look direction to surface
    // are identical
    PSMRTS_Vector3d raypt = psmrts_ray_raypt( ray );
    PSMRTS_Vector3d dsk_raypt = psmrts_ray_raypt( dsk_ray );
    CHECK_THAT( raypt.x, Catch::Matchers::WithinAbs( dsk_raypt.x, tolerance ) );
    CHECK_THAT( raypt.y, Catch::Matchers::WithinAbs( dsk_raypt.y, tolerance ) );
    CHECK_THAT( raypt.z, Catch::Matchers::WithinAbs( dsk_raypt.z, tolerance ) );

    // Verify bullet & naif normals at PSMRTS_RayTrace surface intercept are identical
    PSMRTS_Vector3d normal = psmrts_ray_normal( ray );
    PSMRTS_Vector3d dsk_normal = psmrts_ray_normal( dsk_ray );
    CHECK_THAT( normal.x, Catch::Matchers::WithinAbs( dsk_normal.x, tolerance ) );
    CHECK_THAT( normal.y, Catch::Matchers::WithinAbs( dsk_normal.y, tolerance ) );
    CHECK_THAT( normal.z, Catch::Matchers::WithinAbs( dsk_normal.z, tolerance ) );

    // Verify bullet & naif slant distances to PSMRTS_RayTrace surface intercepts
    // are identical
    CHECK_THAT( psmrts_ray_intercept_slant_distance( ray ),
               Catch::Matchers::WithinAbs( psmrts_ray_intercept_slant_distance( dsk_ray ),
                                           tolerance ) );

    // Verify bullet & naif look direction vector to surface are identical
    // Note this should be identical to the slant distance above as the
    // observation is defined as nadir-looking.
    CHECK_THAT( psmrts_length( &raypt ),
                Catch::Matchers::WithinAbs( psmrts_length( &dsk_raypt ), tolerance ) );

    // Verify bullet & naif target body radii at surface intercepts are identical
    CHECK_THAT( psmrts_ray_intercept_radius( ray ),
                Catch::Matchers::WithinAbs( psmrts_ray_intercept_radius( dsk_ray ),
                                            tolerance ) );


    // Verify bullet & naif facets are identical
    PSMRTS_Facet facet, dsk_facet;
    CHECK( psmrts_get_facet( ray, bulletTracer, &facet ) == PSMRTS_TRUE );
    CHECK( psmrts_get_facet( dsk_ray, dskTracer, &dsk_facet ) == PSMRTS_TRUE );
    CHECK( facet.m_has_facet == dsk_facet.m_has_facet );

    CHECK( facet.m_plateid == dsk_facet.m_plateid );
    CHECK( facet.m_indexes.i == dsk_facet.m_indexes.i );
    CHECK( facet.m_indexes.j == dsk_facet.m_indexes.j );
    CHECK( facet.m_indexes.k == dsk_facet.m_indexes.k );

    CHECK_THAT( facet.m_vector1.x,
               Catch::Matchers::WithinAbs( dsk_facet.m_vector1.x, tolerance ) );
    CHECK_THAT( facet.m_vector1.y,
               Catch::Matchers::WithinAbs( dsk_facet.m_vector1.y, tolerance ) );
    CHECK_THAT( facet.m_vector1.z,
               Catch::Matchers::WithinAbs( dsk_facet.m_vector1.z, tolerance ) );
    CHECK_THAT( facet.m_vector2.x,
               Catch::Matchers::WithinAbs( dsk_facet.m_vector2.x, tolerance ) );
    CHECK_THAT( facet.m_vector2.y,
               Catch::Matchers::WithinAbs( dsk_facet.m_vector2.y, tolerance ) );
    CHECK_THAT( facet.m_vector2.z,
               Catch::Matchers::WithinAbs( dsk_facet.m_vector2.z, tolerance ) );
    CHECK_THAT( facet.m_vector3.x,
               Catch::Matchers::WithinAbs( dsk_facet.m_vector3.x, tolerance ) );
    CHECK_THAT( facet.m_vector3.y,
               Catch::Matchers::WithinAbs( dsk_facet.m_vector3.y, tolerance ) );
    CHECK_THAT( facet.m_vector3.z,
               Catch::Matchers::WithinAbs( dsk_facet.m_vector3.z, tolerance ) );
    CHECK_THAT( facet.m_normal.x,
               Catch::Matchers::WithinAbs( dsk_facet.m_normal.x, tolerance ) );
    CHECK_THAT( facet.m_normal.y,
               Catch::Matchers::WithinAbs( dsk_facet.m_normal.y, tolerance ) );
    CHECK_THAT( facet.m_normal.z,
               Catch::Matchers::WithinAbs( dsk_facet.m_normal.z, tolerance ) );
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

  // create ray trace from observer1 and lookdir1 vectors
  PSMRTS_RayTrace *ray1 = psmrts_create_ray( &observer1, &lookdir1 );

  // add ray1 to trace array and validate returned array index
  CHECK( psmrts_trace_array_add_trace( tracearray, ray1 ) == 0 );

  PSMRTS_Vector3d observer2 = psmrts_vector3d( 45.0, 50.0, 5000.0 );
  observer2 = psmrts_lonlatrad_to_xyz_d( &observer2 );
  CHECK_THAT( observer2.x, Catch::Matchers::WithinAbs( 2272.597388, tolerance ) );
  CHECK_THAT( observer2.y, Catch::Matchers::WithinAbs( 2272.597388, tolerance ) );
  CHECK_THAT( observer2.z, Catch::Matchers::WithinAbs( 3830.222216, tolerance ) );

  PSMRTS_Vector3d lookdir2 = psmrts_negate( &observer1 );

  // create ray trace from observer2 and lookdir2 vectors
  PSMRTS_RayTrace *ray2 = psmrts_create_ray( &observer2, &lookdir2 );

  // add ray2 to trace array and validate returned array index
  CHECK( psmrts_trace_array_add_trace( tracearray, ray2 ) == 1 );

  PSMRTS_Vector3d observer3 = psmrts_vector3d( 45.0, 55.0, 5000.0 );
  observer3 = psmrts_lonlatrad_to_xyz_d( &observer3 );
  CHECK_THAT( observer3.x, Catch::Matchers::WithinAbs( 2027.898938, tolerance ) );
  CHECK_THAT( observer3.y, Catch::Matchers::WithinAbs( 2027.898938, tolerance ) );
  CHECK_THAT( observer3.z, Catch::Matchers::WithinAbs( 4095.760221, tolerance ) );

  PSMRTS_Vector3d lookdir3 = psmrts_negate( &observer3 );

  // create ray trace from observer3 and lookdir3 vectors
  PSMRTS_RayTrace *ray3 = psmrts_create_ray( &observer3, &lookdir3 );

  // add ray3 to trace array and validate returned array index
  CHECK( psmrts_trace_array_add_trace( tracearray, ray3 ) == 2 );

  // process all traces in trace array
  CHECK( psmrts_trace_array_trace( tracearray, bulletTracer ) == PSMRTS_TRUE );

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
  psmrts_trace_array_clear( tracearray );
  psmrts_free_trace_array( tracearray );
}

/**
 * @brief Tests PSMRTS C API functionality for PSMRTS Photometric Trace methods
 *
 * This test exercises the following photometric trace methods in the PSMRTS C API...
 *
 *  1. psmrts_create_photometric_ray
 *  2. psmrts_photo_ray_trace
 *  3. psmrts_photometric_incidence
 *  4. psmrts_photometric_emission
 *  5. psmrts_photometric_phase
 *  6. psmrts_photometric_observer_trace
 *  7. psmrts_photometric_sun_trace
 *  8. psmrts_free_photometric_ray
 *
 */
TEST_CASE ( "PSMRTS C API - Photometric Trace", "[capi][c++][photometric][trace]" ) {
  const double tolerance = 1.0e-6;

  PSMRTS_Tracer *ellipse = psmrts_create_sphere( 1.0, "test" );

  PSMRTS_Vector3d obs = psmrts_vector3d( 45.0, 45.0, 1.0 );
  obs = psmrts_lonlatrad_to_xyz_d( &obs );
  obs = psmrts_scale( &obs, 10.0 );

  PSMRTS_Vector3d surf = psmrts_vector3d( 45.0, 50.0, 1.0 );
  surf = psmrts_lonlatrad_to_xyz_d( &surf );
  surf = psmrts_scale( &surf, 1.5 );
  PSMRTS_Vector3d surf_neg = psmrts_negate( &surf );

  PSMRTS_RayTrace *surf_ray = psmrts_ray_trace_v( &surf, &surf_neg, ellipse );
  PSMRTS_Vector3d surf_xyz = psmrts_ray_xyz( surf_ray );
  double x = surf_xyz.x - obs.x;
  double y = surf_xyz.y - obs.y;
  double z = surf_xyz.z - obs.z;
  PSMRTS_Vector3d lkdr = psmrts_vector3d( x, y, z );

  PSMRTS_RayTrace *observer_ray = psmrts_ray_trace_v( &obs, &lkdr, ellipse );
  PSMRTS_BOOL obs_hit = psmrts_ray_has_hit( observer_ray );
  CHECK( obs_hit == 1 );

  PSMRTS_Vector3d sun_pos = psmrts_vector3d( 20.0, 20.0, 1.0 );
  sun_pos = psmrts_lonlatrad_to_xyz_d( &sun_pos );
  sun_pos = psmrts_scale( &sun_pos, 50.0 );

  PSMRTS_PhotometricRayTrace *p_ray = psmrts_create_photometric_ray( &obs, &lkdr, &sun_pos );
  PSMRTS_PhotometricRayTrace *p_trace = psmrts_photo_ray_trace( p_ray, ellipse );

  CHECK_THAT( psmrts_photometric_incidence( p_ray ), Catch::Matchers::WithinAbs( 0.639547, tolerance ) );
  CHECK_THAT( psmrts_photometric_emission( p_ray ), Catch::Matchers::WithinAbs( 0.096946, tolerance ) );
  CHECK_THAT( psmrts_photometric_phase( p_ray ), Catch::Matchers::WithinAbs( 0.571383, tolerance ) );

  const PSMRTS_RayTrace *obs_ray = psmrts_photometric_observer_trace(p_ray);
  PSMRTS_Vector3d obs_result = psmrts_ray_observer(obs_ray);
  CHECK_THAT( obs_result.x, Catch::Matchers::WithinAbs( obs.x, tolerance ) );
  CHECK_THAT( obs_result.y, Catch::Matchers::WithinAbs( obs.y, tolerance ) );
  CHECK_THAT( obs_result.z, Catch::Matchers::WithinAbs( obs.z, tolerance ) );

  const PSMRTS_RayTrace *sun_ray = psmrts_photometric_sun_trace(p_trace);
  PSMRTS_Vector3d sun_result = psmrts_ray_observer(sun_ray);
  CHECK_THAT( sun_result.x, Catch::Matchers::WithinAbs( sun_pos.x, tolerance ) );
  CHECK_THAT( sun_result.y, Catch::Matchers::WithinAbs( sun_pos.y, tolerance ) );
  CHECK_THAT( sun_result.z, Catch::Matchers::WithinAbs( sun_pos.z, tolerance ) );

  PSMRTS_Vector3d new_obs = psmrts_vector3d( 2.0, 4.0, 6.0 );
  PSMRTS_Vector3d new_lkdr = psmrts_negate( &new_obs );

  psmrts_photometric_ray_set_observation( &new_obs, &new_lkdr, &sun_pos, p_ray );
  const PSMRTS_RayTrace *new_obs_ray = psmrts_photometric_observer_trace( p_ray );
  PSMRTS_Vector3d new_obs_result = psmrts_ray_observer( new_obs_ray );
  CHECK_THAT( new_obs_result.x, Catch::Matchers::WithinAbs( new_obs.x, tolerance ) );
  CHECK_THAT( new_obs_result.y, Catch::Matchers::WithinAbs( new_obs.y, tolerance ) );
  CHECK_THAT( new_obs_result.z, Catch::Matchers::WithinAbs( new_obs.z, tolerance ) );

  psmrts_free_tracer( ellipse );
  psmrts_free_ray( observer_ray );
  psmrts_free_photometric_ray( p_ray );
}

/**
 * @brief Tests PSMRTS C API functionality for PSMRTS Photometric Array methods
 *
 * This test exercises the following photometric array methods in the PSMRTS C API...
 *
 *   1. psmrts_create_photometric_trace_array
 *   2. psmrts_photometric_trace_array_size
 *   3. psmrts_create_photometric_ray
 *   4. psmrts_photometric_trace_array_add_trace
 *   5. psmrts_photometric_trace_array_trace
 *   6. psmrts_photometric_trace_array_get_trace
 *   7. psmrts_photometric_observer_trace
 *   8. psmrts_photometric_trace_array_clear
 *   9. psmrts_free_photometric_trace_array
 *  10. psmrts_free_photometric_ray
 *
 */
TEST_CASE ( "PSMRTS C API - Photometric Array", "[capi][c++][photometric][array]" ) {
  const double tolerance = 1.0e-6;

  PSMRTS_Tracer *ellipse_tracer = psmrts_create_sphere( 1.0, "test" );

  PSMRTS_PhotometricTraceArray *p_array = psmrts_create_photometric_trace_array();
  int size = psmrts_photometric_trace_array_size( p_array );
  CHECK( psmrts_photometric_trace_array_size( p_array ) == 0 );

  PSMRTS_Vector3d sun = psmrts_vector3d( 9.0, 9.0, 9.0 );

  PSMRTS_Vector3d obs1 = psmrts_vector3d( 1.0, 2.0, 3.0 );
  PSMRTS_Vector3d lkdr1 = psmrts_negate( &obs1 );

  PSMRTS_PhotometricRayTrace *p_ray1 = psmrts_create_photometric_ray( &obs1, &lkdr1, &sun );

  PSMRTS_Vector3d obs2 = psmrts_vector3d( 4.0, 5.0, 6.0 );
  PSMRTS_Vector3d lkdr2 = psmrts_negate( &obs2 );

  PSMRTS_PhotometricRayTrace *p_ray2 = psmrts_create_photometric_ray( &obs2, &lkdr2, &sun );

  CHECK( psmrts_photometric_trace_array_add_trace( p_array, p_ray1 ) == 0 );
  CHECK( psmrts_photometric_trace_array_size( p_array ) == 1 );
  CHECK( psmrts_photometric_trace_array_add_trace( p_array, p_ray2 ) == 1 );
  CHECK( psmrts_photometric_trace_array_size( p_array ) == 2 );
  CHECK( psmrts_photometric_trace_array_trace( p_array, ellipse_tracer ) == 1 );

  const PSMRTS_PhotometricRayTrace *target = psmrts_photometric_trace_array_get_trace(p_array, 1);
  const PSMRTS_RayTrace *target_ray = psmrts_photometric_observer_trace(target);
  PSMRTS_Vector3d target_obs = psmrts_ray_observer(target_ray);
  CHECK_THAT( target_obs.x, Catch::Matchers::WithinAbs( obs2.x, tolerance ) );
  CHECK_THAT( target_obs.y, Catch::Matchers::WithinAbs( obs2.y, tolerance ) );
  CHECK_THAT( target_obs.z, Catch::Matchers::WithinAbs( obs2.z, tolerance ) );

  psmrts_photometric_trace_array_clear( p_array );
  CHECK( psmrts_photometric_trace_array_size( p_array ) == 0 );

  psmrts_free_photometric_trace_array( p_array );
  psmrts_free_photometric_ray( p_ray1 );
  psmrts_free_photometric_ray( p_ray2 );
}  

/**
 * @brief Tests PSMRTS C API functionality for the conversion from latitudinal to
 *        rectangular coordinates and vice versa.
 *
 * This test exercises PSMRTS C API functions psmrts_lonlatrad_to_xyz_d and
 *                                            psmrts_xyz_to_lonlatrad_d.
 * 
 * Latitude coordinates are generated every 15 deg from -90 to +90.
 * Longitude coordinates are generated every 30 deg from -180 to +180.
 * Radius is held constant at 1.0.
 * 
 * xyz coordinates are computed via psmrts_lonlatrad_to_xyz_d for every lon, lat, radius combination.
 * Validations are
 *   1) radius is computed from the output xyz coordinates and confirmed to be 1.0
 *   2) for points lying very close to the poles, xyz coordinates are confirmed to be (0,0, ±R)
 *   3) confirm no nan/infinity output
 * 
 * The output xyz coordinates are then converted back to lon, lat, radius via psmrts_xyz_to_lonlatrad_d
 *
 * NOTE: Latitude is assumed to lie within -90 to +90 degree range. If latitude falls
 *       outside of that range, it is clamped to identically -90 or +90 degrees. We
 *       address those conditions in a separate test immediately after this.
 */
TEST_CASE( "PSMRTS C API - Latitudinal to Rectangular Coordinate Conversion", "[capi][c++][utilities][lat2rect][conversion]" ) {
  const double tolerance = 1.0e-9;
  PSMRTS_Vector3d llr_d; // lon, lat in degrees; radius in km

  // Generate latitude every 15 degrees from -90 to 90 (if outside -90 - +90, clamped, see above)
  // Generate longitude every 30 degrees from -180 to +180
  // GENERATE will evaluate all 13 (lat) * 13 (lon) = 169 combinations
  auto lat = GENERATE( range( -90.0, 90.1, 15.0 ) );
  auto lon = GENERATE( range( -180.0, 180.1, 30.0 ) );
        
  llr_d.longitude = lon;
  llr_d.latitude  = lat;
  llr_d.radius    = 1.0;

  SECTION("XYZ coordinates mathematically map correctly") {
    auto xyz = psmrts_lonlatrad_to_xyz_d( &llr_d );

    // Verify radius squared computed from xyz remains constant at 1
    double R2 = xyz.x * xyz.x + xyz.y * xyz.y + xyz.z * xyz.z;
    CAPTURE( lon, lat, xyz.x, xyz.y, xyz.z );
    REQUIRE( R2 == Catch::Approx( 1.0 ).margin( tolerance ) );

    // Verify latitude bounds
    // if latitude is very nearly at the N or S pole, xyz coordinates should be (0, 0, ±R)
    if ( lat == Catch::Approx( 90.0 ).margin(tolerance) ||
         lat == Catch::Approx( -90.0 ).margin( tolerance ) ) {
      REQUIRE( abs( xyz.x ) < tolerance );
      REQUIRE( abs( xyz.y ) < tolerance );
      REQUIRE( abs( abs( xyz.z ) - llr_d.radius ) < tolerance );
    }

    // Ensure no nan/infinity output
    REQUIRE( std::isfinite(xyz.x) );
    REQUIRE( std::isfinite(xyz.y) );
    REQUIRE( std::isfinite(xyz.z) );

    // convert output xyz back to lon, lat, radius
    // and confirm it's equal to the input lon, lat, radius 
    auto llr_out_d = psmrts_xyz_to_lonlatrad_d( &xyz );

    // Verify radius
    REQUIRE( llr_out_d.radius == Catch::Approx( llr_d.radius ).epsilon( tolerance ) );

    // Verify latitude
    REQUIRE( llr_out_d.latitude == Catch::Approx( llr_d.latitude ).epsilon( tolerance ) );

    // Verify Longitude (accounting for 180/-180 meridian wrap-around)
    // e.g. 180 deg == -180 deg mathematically for spherical orientation
    double lonDiff = fmod( abs( llr_out_d.longitude - llr_d.longitude ), 360.0 );
    if ( lonDiff > 180.0 ) {
      lonDiff = 360.0 - lonDiff;
    }
    REQUIRE( lonDiff == Catch::Approx( 0.0 ).margin( tolerance ) );
  }
}

/**
 * @brief Tests PSMRTS C API functionality for the conversion from latitudinal to
 *        rectangular coordinates and vice versa when the latitude coordinate is outside
 *        of the range -90 to +90. In that case the latitude is it is clamped to
 *        identically -90 or +90 degrees.
 *
 * This test exercises PSMRTS C API functions psmrts_lonlatrad_to_xyz_d and
 *                                            psmrts_xyz_to_lonlatrad_d
 * when the latitude coordinate is outside of the range -90 to +90.
 * 
 * Two coordinates are tested with latitudes of -100 and + 100 degrees.
 * 
 * xyz coordinates are computed via psmrts_lonlatrad_to_xyz_d for each point.
 * Validations are
 *   1) radius is computed from the output xyz coordinates and confirmed to be 1.0
 *   2) for points lying very close to the poles, xyz coordinates are confirmed to be (0,0, ±R)
 *   3) confirm no nan/infinity output
 *
 */
TEST_CASE( "PSMRTS C API - Latitudinal to Rectangular Clamped Coordinate Conversion", "[capi][c++][utilities][lat2rect][clamp][conversion]" ) {
  const double tolerance = 1.0e-9;

  // point with latitude less than -90
  PSMRTS_Vector3d llr_d1; // lon, lat in degrees; radius in km
        
  llr_d1.longitude = 45.0;
  llr_d1.latitude  = -100.0;
  llr_d1.radius    = 1.0;

  auto xyz1 = psmrts_lonlatrad_to_xyz_d( &llr_d1 );

  // Verify radius squared computed from xyz remains constant at 1
  double R2 = xyz1.x * xyz1.x + xyz1.y * xyz1.y + xyz1.z * xyz1.z;
  CAPTURE( llr_d1.longitude, llr_d1.latitude, xyz1.x, xyz1.y, xyz1.z );
  REQUIRE( R2 == Catch::Approx( 1.0 ).margin( tolerance ) );

  // Verify latitude bounds
  // if latitude is very nearly at the N or S pole, xyz coordinates should be (0, 0, ±R)
  if ( llr_d1.latitude == Catch::Approx( 90.0 ).margin(tolerance) ||
        llr_d1.latitude == Catch::Approx( -90.0 ).margin( tolerance ) ) {
    REQUIRE( abs( xyz1.x ) < tolerance );
    REQUIRE( abs( xyz1.y ) < tolerance );
    REQUIRE( abs( abs( xyz1.z ) - llr_d1.radius ) < tolerance );
  }

  // Ensure no nan/infinity output
  REQUIRE( isfinite(xyz1.x) );
  REQUIRE( isfinite(xyz1.y) );
  REQUIRE( isfinite(xyz1.z) );

  // point with latitude greater than +90
  PSMRTS_Vector3d llr_d2;
    
  llr_d2.longitude = 45.0;
  llr_d2.latitude  = 100.0;
  llr_d2.radius    = 1.0;

  auto xyz2 = psmrts_lonlatrad_to_xyz_d( &llr_d2 );

  // Verify latitude bounds
  // if latitude is very nearly at the N or S pole, xyz coordinates should be (0, 0, ±R)
  if ( llr_d2.latitude == Catch::Approx( 90.0 ).margin(tolerance) ||
       llr_d2.latitude == Catch::Approx( -90.0 ).margin( tolerance ) ) {
    REQUIRE( abs( xyz2.x ) < tolerance );
    REQUIRE( abs( xyz2.y ) < tolerance );
    REQUIRE( abs( abs( xyz2.z ) - llr_d2.radius ) < tolerance );
  }

  // Ensure no nan/infinity output
  REQUIRE( isfinite(xyz2.x) );
  REQUIRE( isfinite(xyz2.y) );
  REQUIRE( isfinite(xyz2.z) );
}

/**
 * @brief Tests PSMRTS C API psmrts_degrees_to_radians and psmrts_radians_to_degrees
 *        functionality for individual numbers and for PSMRTS_Vector3d.
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

  // Verify no facet for sphere traces
  PSMRTS_Facet facet;
  CHECK( psmrts_get_facet( trace_45_50_02, sphere_tracer, &facet ) == PSMRTS_FALSE );
  CHECK( facet.m_has_facet == PSMRTS_FALSE );
  CHECK( psmrts_get_facet( trace_45_45_10, sphere_tracer, &facet ) == PSMRTS_FALSE );
  CHECK( facet.m_has_facet == PSMRTS_FALSE );

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

  // Verify no facet for spheroid traces
  PSMRTS_Facet facet;
  CHECK( psmrts_get_facet( trace_45_50_02, spheroid_tracer, &facet ) == PSMRTS_FALSE );
  CHECK( facet.m_has_facet == PSMRTS_FALSE );
  CHECK( psmrts_get_facet( trace_45_45_10, spheroid_tracer, &facet ) == PSMRTS_FALSE );
  CHECK( facet.m_has_facet == PSMRTS_FALSE );

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

  // Verify no facet for ellipsoid traces
  PSMRTS_Facet facet;
  CHECK( psmrts_get_facet( trace_45_50_02, e_tracer, &facet ) == PSMRTS_FALSE );
  CHECK( facet.m_has_facet == PSMRTS_FALSE );
  CHECK( psmrts_get_facet( trace_45_45_10, e_tracer, &facet ) == PSMRTS_FALSE );
  CHECK( facet.m_has_facet == PSMRTS_FALSE );

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

  // run trace_45_50_02 with ev_tracer
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

  // run trace_45_45_10 with ev_tracer
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

  // Verify no facet for ellipsoidv traces
  PSMRTS_Facet facet;
  CHECK( psmrts_get_facet( trace_45_50_02, ev_tracer, &facet ) == PSMRTS_FALSE );
  CHECK( facet.m_has_facet == PSMRTS_FALSE );
  CHECK( psmrts_get_facet( trace_45_45_10, ev_tracer, &facet ) == PSMRTS_FALSE );
  CHECK( facet.m_has_facet == PSMRTS_FALSE );

  // free memory
  psmrts_free_tracer( ev_tracer );
  psmrts_free_ray( trace_45_50_02 );
  psmrts_free_ray( trace_45_45_10 );
}

/**
 * @brief Tests PSMRTS C API Mesh methods.
 *
 * This test creates meshes from obj, dsk, and ply files with the methods...
 *   psmrts_create_obj_shape
 *   psmrts_create_dsk_shape
 *   psmrts_create_ply_shape
 * 
 * And further verifies surface area and volume of all three meshes with...
 *   psmrts_mesh_surface_area
 *   psmrts_mesh_volume
 * 
 * And finally frees the memory for all shapes with...
 *   psmrts_free_shape
 */
TEST_CASE( "PSMRTS C API - Mesh Test", "[capi][c++][mesh][obj]" ) {
  double tolerance = 1.0e-6;

  std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
  PSMRTS_Shape *objshape = psmrts_create_obj_shape( objfile.c_str() );
  
  CHECK_THAT( psmrts_mesh_surface_area( objshape ),
              Catch::Matchers::WithinAbs( 0.842492, tolerance ) );
  CHECK_THAT( psmrts_mesh_volume( objshape ),
              Catch::Matchers::WithinAbs( 0.063170, tolerance ) );

  // testing dsk mesh
  std::string dskfile = psmrts_shapes_path( "dsk/data/bennu_20facets.bds" );
  PSMRTS_Shape *dskshape = psmrts_create_dsk_shape( dskfile.c_str() );
  
  CHECK_THAT( psmrts_mesh_surface_area( dskshape ),
              Catch::Matchers::WithinAbs( 0.842492, tolerance ) );
  CHECK_THAT( psmrts_mesh_volume( dskshape ),
              Catch::Matchers::WithinAbs( 0.063170, tolerance ) );

  // testing ply mesh
  std::string plyfile = psmrts_shapes_path( "ply/data/Bennu_Radar.ply" );
  PSMRTS_Shape *plyshape = psmrts_create_ply_shape( plyfile.c_str() );
  
  CHECK_THAT( psmrts_mesh_surface_area( plyshape ),
              Catch::Matchers::WithinAbs( 0.785467, tolerance ) );
  CHECK_THAT( psmrts_mesh_volume( plyshape ),
              Catch::Matchers::WithinAbs( 0.062265, tolerance ) );

  // free memory
  psmrts_free_shape( objshape );
  psmrts_free_shape( dskshape );
  psmrts_free_shape( plyshape );
}

/**
 * @brief Tests PSMRTS C API PSMRTS_Invoice and PSMRTS_Translations methods for ply shape.
 *
 * Methods tested...
 *   psmrts_create_translation*
 *   psmrts_create_config*
 *   psmrts_create_invoice*
 *   psmrts_invoice_error_string*
 *   psmrts_add_config_invoice*
 *   psmrts_generate_priority_tracer*
 *   psmrts_free_translations*
 *   psmrts_free_invoice*
 * 
 */
TEST_CASE( "C API Invoice & Translations Shape Test", "[capi][c++][invoice][translations][shape]" ) {
  // create PSMRTS_Translations
  PSMRTS_Translations *trans_t = psmrts_create_translation();

  // create PSMRTS_ProductConfiguration
  PSMRTS_ProductConfiguration* shape_config = psmrts_create_config( "shape", "ply", nullptr );

  // add products to shape_config
  psmrts_add_product_string( shape_config, "shape", "ply" );
  psmrts_add_product_string( shape_config,
                             "ply_file",
                             psmrts_shapes_path( "ply/data/Bennu_Radar.ply" ).c_str() );

  // retrieve and validate string with product config metadata
  PSMRTS_String *checkstr = psmrts_create_string( "" );
  psmrts_product_config_to_string( shape_config, checkstr );
  CHECK( (std::string( psmrts_string_content( checkstr )).find("options") ) != std::string::npos );
  CHECK( (std::string( psmrts_string_content( checkstr )).find("shape") )   != std::string::npos );
  CHECK( (std::string( psmrts_string_content( checkstr )).find("ply") )     != std::string::npos );
  CHECK( (std::string( psmrts_string_content( checkstr )).find("ply_file") ) != std::string::npos );
  CHECK( (std::string( psmrts_string_content( checkstr )).find("ply/data/Bennu_Radar.ply") ) != std::string::npos );

  CHECK( psmrts_product_config_contains( shape_config, "shape" ) == PSMRTS_TRUE );
  CHECK( psmrts_product_config_contains( shape_config, "ply_file" ) == PSMRTS_TRUE );

  // create invoice
  PSMRTS_Invoice *plyinvoice = psmrts_create_invoice( "plyinvoice", trans_t );

  // check for errors
  PSMRTS_String *error_str = psmrts_invoice_error_string( plyinvoice, nullptr );
  CHECK( std::string( psmrts_string_content( error_str ) ) == "" );

  // add shape_config to invoice
  CHECK( psmrts_add_config_invoice( shape_config, plyinvoice ) == PSMRTS_TRUE );

  // create priority tracer
  PSMRTS_PriorityTracer *ptracer = psmrts_generate_priority_tracer( plyinvoice, nullptr );

  // free memory
  psmrts_free_translations( trans_t );
  psmrts_free_product_config( shape_config );
  psmrts_free_string( checkstr );
  psmrts_free_invoice( plyinvoice );
  psmrts_free_string( error_str );
  psmrts_free_priority_tracer( ptracer );
}

/**
 * @brief Tests PSMRTS C API PSMRTS_Invoice and PSMRTS_Translations methods for bullet tracer.
 *
 * Methods tested...
 *   psmrts_create_translation*
 *   psmrts_create_config*
 *   psmrts_create_invoice*
 *   psmrts_invoice_error_string*
 *   psmrts_add_config_invoice*
 *   psmrts_generate_priority_tracer*
 *   psmrts_free_translations*
 *   psmrts_free_invoice*
 * 
 */
TEST_CASE( "C API Invoice & Translations Tracer Test", "[capi][c++][invoice][translations][tracer]" ) {
  // create PSMRTS_Translations
  PSMRTS_Translations *trans_t = psmrts_create_translation();

  // create PSMRTS_ProductConfiguration
  PSMRTS_ProductConfiguration* tracer_config = psmrts_create_config( "tracer", "bullet", nullptr );

  // add products to shape_config
  psmrts_add_product_string( tracer_config, "shape", "obj" );
  psmrts_add_product_string( tracer_config,
                             "obj_file",
                             psmrts_shapes_path( "obj/data/bennu_20facets.obj" ).c_str() );
  psmrts_add_product_string( tracer_config, "tracer", "bullet" );

  CHECK( psmrts_product_config_contains( tracer_config, "shape" ) == PSMRTS_TRUE );
  CHECK( psmrts_product_config_contains( tracer_config, "tracer" ) == PSMRTS_TRUE );
  CHECK( psmrts_product_config_contains( tracer_config, "obj_file" ) == PSMRTS_TRUE );

  // retrieve and validate string with product config metadata
  PSMRTS_String *checkstr = psmrts_create_string( "" );
  psmrts_product_config_to_string( tracer_config, checkstr );
  CHECK( (std::string( psmrts_string_content( checkstr )).find("shape") ) != std::string::npos );
  CHECK( (std::string( psmrts_string_content( checkstr )).find("tracer") )   != std::string::npos );
  CHECK( (std::string( psmrts_string_content( checkstr )).find("bullet") )     != std::string::npos );
  CHECK( (std::string( psmrts_string_content( checkstr )).find("obj_file") ) != std::string::npos );
  CHECK( (std::string( psmrts_string_content( checkstr )).find("obj/data/bennu_20facets.obj") ) != std::string::npos );

  // create invoice
  PSMRTS_Invoice *bulletinvoice = psmrts_create_invoice( "bulletinvoice", trans_t );

  // check for errors
  PSMRTS_String *errorstr = psmrts_invoice_error_string( bulletinvoice, nullptr );
  CHECK( std::string( psmrts_string_content( errorstr ) ) == "" );

  // add shape_config to invoice
  CHECK( psmrts_add_config_invoice( tracer_config, bulletinvoice ) == PSMRTS_TRUE );

  // create priority tracer
  PSMRTS_PriorityTracer *ptracer = psmrts_generate_priority_tracer( bulletinvoice, nullptr );

  // free memory
  psmrts_free_translations( trans_t );
  psmrts_free_product_config( tracer_config );
  psmrts_free_string( checkstr );
  psmrts_free_invoice( bulletinvoice );
  psmrts_free_string( errorstr );
  psmrts_free_priority_tracer( ptracer );  
}


