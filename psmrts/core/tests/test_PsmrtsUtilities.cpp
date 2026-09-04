#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <thread>
#include <cmath>
#include <chrono>

TEST_CASE( "PSMRTS Clock Time Tests", "[utilities][time]" ) {

    using namespace std::literals;

    psmrts::PsmrtsThreadSafeCounter counter_t;
    CHECK( counter_t.count() == 0 );
    CHECK( counter_t.hitme() == 1 );
    CHECK( counter_t.count() == 1 );
    CHECK( counter_t.now() == psmrts::current_time() ); // new
    CHECK( counter_t++       == 2 );
    CHECK( counter_t.count() == 2 );

    INFO( to_string( counter_t.snapshot() ) );

    std::time_t first_timer = psmrts::current_time(); 
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::time_t sec_timer = psmrts::current_time();
    CHECK( ( sec_timer - first_timer ) >= 1 );

    psmrts::PSMRTS_SYSTEM_CLOCK_TIME currently = psmrts::system_clock_time();
    psmrts::PSMRTS_SYSTEM_CLOCK_TIME plus_one_s = currently + 1s;
    CHECK( psmrts::elapsed_clock_time_s( currently, plus_one_s ) == 1.0 );
    CHECK( psmrts::elapsed_clock_time_ms( currently, plus_one_s ) == 1000.0 );
    CHECK( counter_t.runtime_s() != 0.0 );
    
    // Seems to stop test in their tracks: (because mutex locker?)
    psmrts::PsmrtsThreadSafeCounter counter_two = counter_t.clone();
    CHECK( counter_two.born_on_date() == counter_t.born_on_date() );
    CHECK( counter_two.start_time() == counter_t.start_time() );
    
    std::shared_ptr<float> float_ptr;
    std::shared_ptr<uint8_t> uint_ptr;
    CHECK_NOTHROW( uint_ptr = psmrts::cast_shared_ptr<uint8_t, float>(float_ptr) );
    
    double null_val = psmrts::null();
    CHECK( psmrts::isnull(null_val) == true );

    Eigen::Vector3d ev_one = { 1.0, 2.0, 3.0 };
    Eigen::Vector3d ev_two = { 1.0, 2.0, 3.0 };
    Eigen::Vector3d ev_three = { 3.0, 3.0, 3.0 };
    Eigen::Vector3d ev_four = { 0.99999999998, 1.99999999998, 2.99999999998 };
    Eigen::Vector3d ev_five = { 0.999999999999, 1.999999999999, 2.999999999999 };
    CHECK( psmrts::isEqual(ev_one, ev_two) == true );
    CHECK( psmrts::isEqual(ev_two, ev_three) == false );
    CHECK( psmrts::isEqual(ev_one, ev_four) == false );
    CHECK( psmrts::isEqual(ev_one, ev_five) == true ); // Base tolerance: 1.0e-12
    CHECK( psmrts::isEqual(ev_one, ev_five, 1.0e-16) == false );
}

TEST_CASE( "PSMRTS String Functions Test", "[utilities][string]" ) {
  CHECK( psmrts::psmrts_tolower( "tHiS iS A LOWERcasE strINg") == "this is a lowercase string" );
  CHECK( psmrts::psmrts_tolower( "1. THIS? is *ALSO* lowerCASE&") == "1. this? is *also* lowercase&" );
  CHECK( psmrts::psmrts_toupper( "tHiS iS A upPErcasE strINg") == "THIS IS A UPPERCASE STRING" );
  CHECK( psmrts::psmrts_toupper("2. this *IS* also --UPPER--case!!") == "2. THIS *IS* ALSO --UPPER--CASE!!" );

  std::string ellipsoid1 = "ellipsoid:0.298,0.1,0.3";
  std::string ellipsoid2 = "ellipsoid:0.298";
  std::string ellipsoid3 = "ellipsoid:0.298,0.1";
  std::string empty_e    = "ellipsoid:";
  std::string empty_all  = "";
  

  CHECK( psmrts::string_tokenizer(ellipsoid1)       == std::vector<std::string> {"ellipsoid:0.298", "0.1", "0.3"} );
  CHECK( psmrts::string_tokenizer(ellipsoid2)       == std::vector<std::string> {"ellipsoid:0.298"} );
  CHECK( psmrts::string_tokenizer(ellipsoid3)       == std::vector<std::string> {"ellipsoid:0.298", "0.1"} );
  CHECK( psmrts::string_tokenizer(ellipsoid1, ":,") == std::vector<std::string> {"ellipsoid", "0.298", "0.1", "0.3"} );
  CHECK( psmrts::string_tokenizer(ellipsoid2, ":,") == std::vector<std::string> {"ellipsoid", "0.298"} );
  CHECK( psmrts::string_tokenizer(ellipsoid3, ":,") == std::vector<std::string> {"ellipsoid", "0.298", "0.1"} );
  CHECK( psmrts::string_tokenizer(empty_e)          == std::vector<std::string> {"ellipsoid:"} );
  CHECK( psmrts::string_tokenizer(empty_all)        == std::vector<std::string> { "" } );

  CHECK( psmrts::is_bool( "T" )     == true );
  CHECK( psmrts::is_bool( "F" )     == false );
  CHECK( psmrts::is_bool( "yEs" )   == true );
  CHECK( psmrts::is_bool( "nO" )    == false );
  CHECK_THROWS( psmrts::is_bool( "invalid" ) );

}


TEST_CASE( "PSMRTS Make Path Test", "[utilities][path]") {
  // Test psmrts_make_path --> string conversion for complete path

  std::string path = psmrts::psmrts_make_path( "tests", "test_PsmrtsUtilities.cpp" );
  std::string directory = psmrts::psmrts_make_path( "tests" );
  std::string path_only = psmrts::psmrts_make_path( "", "test_PsmrtsUtilities.cpp" );
  CHECK ( path == "tests/test_PsmrtsUtilities.cpp" );
  CHECK ( directory == "tests" );
  CHECK ( path_only == "test_PsmrtsUtilities.cpp" );

  std::string path_ext = psmrts::psmrts_file_extension( path );
  CHECK ( path_ext == "cpp" );

  std::string path_file = psmrts::psmrts_file_path( path );
  CHECK ( path_file == "tests" ); // Should be tests? 

  std::string path_base = psmrts::psmrts_filename( path );
  CHECK ( path_base == "test_PsmrtsUtilities.cpp" );

  const std::string test_1( "Tracer:value1,value2,value3" );
  auto parts_1 = psmrts::string_tokenizer( test_1, ":," );
  REQUIRE( parts_1.size() == 4 );
  CHECK( parts_1[0]       == "Tracer" );
  CHECK( parts_1[1]       == "value1" );
  CHECK( parts_1[2]       == "value2" );
  CHECK( parts_1[3]       == "value3" );

  const std::string path_w( "bullet::D:/a/path/to/file/myshape.obj" );
  auto parts_t = psmrts::string_tokenizer_substring( path_w, "::" );
  REQUIRE( parts_t.size() == 2 );
  CHECK( parts_t[0]       == "bullet" );
  CHECK( parts_t[1]       == "D:/a/path/to/file/myshape.obj" );

}

TEST_CASE( "PSMRTS Longitude Domain Test - 360 to 180", "[utilities][longitude][180Domain]" ) {

  const double tolerance = 1.0e-6;

  CHECK_THAT( psmrts::to180LongitudeDomain_d( -700.0 ), Catch::Matchers::WithinAbs( 20, tolerance ));
  CHECK_THAT( psmrts::to180LongitudeDomain_d( -360.0 ), Catch::Matchers::WithinAbs( 0.0, tolerance ));
  CHECK_THAT( psmrts::to180LongitudeDomain_d( -300.0 ), Catch::Matchers::WithinAbs( 60.0, tolerance ));
  CHECK_THAT( psmrts::to180LongitudeDomain_d( -190.0 ), Catch::Matchers::WithinAbs( 170.0, tolerance ));
  CHECK_THAT( psmrts::to180LongitudeDomain_d( -185.0 ), Catch::Matchers::WithinAbs( 175.0, tolerance ));
  CHECK_THAT( psmrts::to180LongitudeDomain_d( -180.0 ), Catch::Matchers::WithinAbs( -180.0, tolerance ));
  CHECK_THAT( psmrts::to180LongitudeDomain_d( -90.0 ), Catch::Matchers::WithinAbs( -90.0, tolerance ));
  CHECK_THAT( psmrts::to180LongitudeDomain_d(   0.0 ), Catch::Matchers::WithinAbs( 0.0, tolerance ));
  CHECK_THAT( psmrts::to180LongitudeDomain_d(  90.0 ), Catch::Matchers::WithinAbs( 90.0, tolerance ));
  CHECK_THAT( psmrts::to180LongitudeDomain_d( 180.0 ), Catch::Matchers::WithinAbs( -180.0, tolerance )); // 180 -> -180
  CHECK_THAT( psmrts::to180LongitudeDomain_d( 190.0 ), Catch::Matchers::WithinAbs( -170.0, tolerance ));
  CHECK_THAT( psmrts::to180LongitudeDomain_d( 300.0 ), Catch::Matchers::WithinAbs( -60.0, tolerance ));
  CHECK_THAT( psmrts::to180LongitudeDomain_d( 360.0 ), Catch::Matchers::WithinAbs( 0.0, tolerance ));
  CHECK_THAT( psmrts::to180LongitudeDomain_d( 700.0 ), Catch::Matchers::WithinAbs( -20, tolerance ));
}

TEST_CASE( "PSMRTS Longitude Domain Test - 180 to 360", "[utilities][longitude][360Domain]") {

  const double tolerance = 1.0e-6;

  CHECK_THAT( psmrts::to360LongitudeDomain_d( 0.0 ), Catch::Matchers::WithinAbs( 0.0, tolerance ));
  CHECK_THAT( psmrts::to360LongitudeDomain_d( 180.0 ), Catch::Matchers::WithinAbs( 180.0, tolerance ));
  CHECK_THAT( psmrts::to360LongitudeDomain_d( -170.0 ), Catch::Matchers::WithinAbs( 190.0, tolerance ));
  CHECK_THAT( psmrts::to360LongitudeDomain_d( -90.0 ), Catch::Matchers::WithinAbs( 270.0, tolerance ));
  CHECK_THAT( psmrts::to360LongitudeDomain_d( -60.0 ), Catch::Matchers::WithinAbs( 300.0, tolerance ));
  CHECK_THAT( psmrts::to360LongitudeDomain_d( 1000.0 ), Catch::Matchers::WithinAbs( 280.0, tolerance ));
 
}

/**
 * @brief Tests PSMRTS functionality for the conversion from latitudinal to
 *        rectangular coordinates and vice versa.
 *
 * This test exercises PSMRTS functions psmrts::lonlatrad_to_xyz_d and
 *                                      psmrt::xyz_to_lonlatrad_d.
 * 
 * Latitude coordinates are generated every 15 deg from -90 to +90.
 * Longitude coordinates are generated every30 deg from -360 to +360.
 * Radius is held constant at 1.0.
 * 
 * xyz coordinates are computed via psmrts::lonlatrad_to_xyz_d for every lon, lat, radius combination.
 * Validations are
 *   1) radius is computed from the output xyz coordinates and confirmed to be 1.0
 *   2) for points lying very close to the poles, xyz coordinates are confirmed to be (0,0, ±R)
 *   3) confirm no nan/infinity output
 * 
 * The output xyz coordinates are then converted back to lon, lat, radius via psmrts::xyz_to_lonlatrad_d
 *
 * NOTE: Latitude is assumed to lie within -90 to +90 degree range. If latitude falls
 *       outside of that range, it is clamped to identically -90 or +90 degrees. We
 *       address those conditions in a separate test immediately after this.
 */
TEST_CASE( "PSMRTS Latitudinal to Rectangular Coordinate Conversion Test", "[utilities][lat2rect][conversion]") {
  const double tolerance = 1.0e-13;

  Eigen::Vector3d llr_d; // lon, lon in degrees; radius in km

  // Generate latitude every 15 degrees from -90 to 90 (if outside -90 - +90, clamped, see above)
  // Generate longitude every 30 degrees from -360 to 360
  // GENERATE will evaluate all 13 (lat) * 25 (lon) = 325 combinations
  auto lon = GENERATE( range( -360.0, 360.1, 30.0 ) );
  auto lat = GENERATE( range( -90.0, 90.1, 15.0 ) );

  llr_d[0] = lon;
  llr_d[1] = lat;
  llr_d[2] = 1.0;

  SECTION("XYZ coordinates mathematically map correctly") {
    auto xyz = psmrts::lonlatrad_to_xyz_d( llr_d ); // converts to 360 domain if necessary

    // Verify radius squared computed from xyz remains constant at 1
    double R2 = xyz[0] * xyz[0] + xyz[1] * xyz[1] + xyz[2] * xyz[2];
    CAPTURE( lon, lat, xyz[0], xyz[1], xyz[2] );
    REQUIRE( R2 == Catch::Approx( 1.0 ).margin( tolerance ) );

    // Verify latitude bounds
    // if latitude is very nearly at the N or S pole, xyz coordinates should be (0, 0, ±R)
    if ( lat == Catch::Approx( 90.0 ).margin(tolerance) ||
         lat == Catch::Approx( -90.0 ).margin( tolerance ) ) {
      REQUIRE( std::abs( xyz[0] ) < tolerance );
      REQUIRE( std::abs( xyz[1] ) < tolerance );
      REQUIRE( std::abs( std::abs( xyz[2] ) - llr_d[2] ) < tolerance );
    }

    // Ensure no nan/infinity output
    REQUIRE( std::isfinite( xyz[0] ) );
    REQUIRE( std::isfinite( xyz[1] ) );
    REQUIRE( std::isfinite( xyz[2] ) );

    // convert output xyz back to lon, lat, radius
    // and confirm it's equal to the input lon, lat, radius 
    auto llr_out_d = psmrts::xyz_to_lonlatrad_d( xyz );

    // Verify radius
    REQUIRE( llr_out_d[2] == Catch::Approx( llr_d[2] ).epsilon( tolerance ) );

    // Verify latitude
    REQUIRE( llr_out_d[1] == Catch::Approx( llr_d[1] ).epsilon( tolerance ) );

    // Verify Longitude (accounting for 180/-180 meridian wrap-around)
    // e.g. 180 deg == -180 deg mathematically for spherical orientation
    double lonDiff = std::fmod( std::abs( llr_out_d[0] - llr_d[0] ), 360.0 );
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
 * This test exercises PSMRTS functions psmrts::lonlatrad_to_xyz_d and
 *                                      psmrts::xyz_to_lonlatrad_d
 * when the latitude coordinate is outside of the range -90 to +90.
 * 
 * Two coordinates are tested with latitudes of -100 and + 100 degrees.
 * 
 * xyz coordinates are computed via psmrts::lonlatrad_to_xyz_d for each point.
 * Validations are
 *   1) radius is computed from the output xyz coordinates and confirmed to be 1.0
 *   2) for points lying very close to the poles, xyz coordinates are confirmed to be (0,0, ±R)
 *   3) confirm no nan/infinity output
 *
 */
TEST_CASE( "PSMRTS Latitudinal to Rectangular Clamped Coordinate Conversion", "[c++][utilities][lat2rect][clamp][conversion]" ) {
  const double tolerance = 1.0e-13;

  // point with latitude less than -90
  Eigen::Vector3d llr_d1; // lon, lat in degrees; radius in km
        
  llr_d1[0] =   45.0;  // longitude
  llr_d1[1] = -100.0;  // latitude
  llr_d1[2] =    1.0;  // radius

  auto xyz1 = psmrts::lonlatrad_to_xyz_d( llr_d1 );

  // Verify radius squared computed from xyz remains constant at 1
  double R2 = xyz1[0] * xyz1[0] + xyz1[1] * xyz1[1] + xyz1[2] * xyz1[2];
  CAPTURE( llr_d1[0], llr_d1[1], xyz1[0], xyz1[1], xyz1[2] );
  REQUIRE( R2 == Catch::Approx( 1.0 ).margin( tolerance ) );

  // Verify latitude bounds
  // if latitude is very nearly at the N or S pole, xyz coordinates should be (0, 0, ±R)
  if ( llr_d1[1] == Catch::Approx( 90.0 ).margin(tolerance) ||
       llr_d1[1] == Catch::Approx( -90.0 ).margin( tolerance ) ) {
    REQUIRE( std::abs( xyz1[0] ) < tolerance );
    REQUIRE( std::abs( xyz1[1] ) < tolerance );
    REQUIRE( std::abs( std::abs( xyz1[2] ) - llr_d1[2] ) < tolerance );
  }

  // Ensure no nan/infinity output
  REQUIRE( std::isfinite(xyz1[0]) );
  REQUIRE( std::isfinite(xyz1[1]) );
  REQUIRE( std::isfinite(xyz1[2]) );

  // point with latitude greater than +90
  Eigen::Vector3d llr_d2;
    
  llr_d2[0] =  45.0;  // longitude
  llr_d2[1] = 100.0;  // latitude
  llr_d2[2] =   1.0;  // radius

  auto xyz2 = psmrts::lonlatrad_to_xyz_d( llr_d2 );

  // Verify latitude bounds
  // if latitude is very nearly at the N or S pole, xyz coordinates should be (0, 0, ±R)
  if ( llr_d2[1] == Catch::Approx( 90.0 ).margin(tolerance) ||
       llr_d2[1] == Catch::Approx( -90.0 ).margin( tolerance ) ) {
    REQUIRE( abs( xyz2[0] ) < tolerance );
    REQUIRE( abs( xyz2[1] ) < tolerance );
    REQUIRE( abs( abs( xyz2[2] ) - llr_d2[2] ) < tolerance );
  }

  // Ensure no nan/infinity output
  REQUIRE( std::isfinite(xyz2[0]) );
  REQUIRE( std::isfinite(xyz2[1]) );
  REQUIRE( std::isfinite(xyz2[2]) );
}

 /**
 * @brief Tests PSMRTS utility for the computation of the surface area and
 *        volume of a mesh facet as defined by three 3 Eigen::Vector3ds.
 *
 * This test exercises the PSMRTS utility functions facet_surface_area and
 * facet_volume.
 *
 */
 TEST_CASE( "PSMRTS Facet Geometry Test", "[utilities][facet][geometry]") {
   const double tolerance = 1.0e-6;

   psmrts::PRQFacet facet1;

   facet1.m_facet.m_has_facet = true;
   facet1.m_facet.m_plateid = 30;
   facet1.m_facet.m_segment = 0;
   facet1.m_facet.m_indexes[0] = 11;
   facet1.m_facet.m_indexes[1] = 14;
   facet1.m_facet.m_indexes[2] = 5;

   facet1.m_facet.m_vector1[0] = 0.101004;
   facet1.m_facet.m_vector1[1] = 0.0;
   facet1.m_facet.m_vector1[2] = 0.264431;

   facet1.m_facet.m_vector2[0] = 0.163428;
   facet1.m_facet.m_vector2[1] = 0.163428;
   facet1.m_facet.m_vector2[2] = 0.163428;

   facet1.m_facet.m_vector3[0] = 0.0;
   facet1.m_facet.m_vector3[1] = 0.264432;
   facet1.m_facet.m_vector3[2] = 0.101004;

   CHECK_THAT( facet1.facet().surface_area(),
               Catch::Matchers::WithinAbs( 0.019405, tolerance ) );

   CHECK_THAT( facet1.facet().volume(),
               Catch::Matchers::WithinAbs( 0.001455, tolerance ) );

   // now create the same facet, but reverse the vertex winding order, which
   // should negate the resulting volume

   psmrts::PRQFacet facet2;

   facet2.m_facet.m_has_facet = true;
   facet2.m_facet.m_plateid = 30;
   facet2.m_facet.m_segment = 0;
   facet2.m_facet.m_indexes[0] = 11;
   facet2.m_facet.m_indexes[1] = 14;
   facet2.m_facet.m_indexes[2] = 5;

   facet2.m_facet.m_vector1[0] = 0.0;
   facet2.m_facet.m_vector1[1] = 0.264432;
   facet2.m_facet.m_vector1[2] = 0.101004;

   facet2.m_facet.m_vector2[0] = 0.163428;
   facet2.m_facet.m_vector2[1] = 0.163428;
   facet2.m_facet.m_vector2[2] = 0.163428;

   facet2.m_facet.m_vector3[0] = 0.101004;
   facet2.m_facet.m_vector3[1] = 0.0;
   facet2.m_facet.m_vector3[2] = 0.264431;

   CHECK_THAT( facet2.facet().surface_area(),
               Catch::Matchers::WithinAbs( 0.019405, tolerance ) );

   CHECK_THAT( facet2.facet().volume(),
               Catch::Matchers::WithinAbs( -0.001455, tolerance ) );
 }

/*
No longer relevant?

TEST_CASE( "PSMRTS Normal Computation Test", "[utilites][normal][raytrace]") {
    // TODO
    
    Create a simple ray trace, and use the multiple-eigen-point normal function
    to compare to the utilities compute normal. 
    

    Eigen::Vector3d vec1 = {0.1634276539482, -0.1634276539482, 0.1634276539482};
    Eigen::Vector3d vec2 = {0.2644314943232, -0.1010038565354, 0.0000000000000};
    Eigen::Vector3d vec3 = {0.2644314943232,  0.1010038565354, 0.0000000000000};

   // Using first facet of bennu obj (19, 3, 2), compare to dsk normal
   // try two:
   // first one is 19, 3, 2 
   // second is 2, 3, 19 
   // make sure they're opposite (exact negatives, and abs value == )

}
*/
