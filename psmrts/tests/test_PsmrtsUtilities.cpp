#include <psmrts_catch2_environment.hpp>

#include <PsmrtsUtilities.hpp>
#include <PsmrtsRayTrace.hpp>
#include <thread>
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
    CHECK( ( sec_timer - first_timer ) == 1 );

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

std::string path_base = psmrts::psmrts_file_basename( path );
CHECK ( path_base == "test_PsmrtsUtilities.cpp" );

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

TEST_CASE( "PSMRTS Latitudinal to Rectangular Coordinate Conversion Test", "[utilities][lat2rect][conversion]") {
    const double tolerance = 1.0e-6;

    Eigen::Vector3d llr_d; // lon, lon in degrees; radius in km
    Eigen::Vector3d xyz;   // km

    // test at with latitude > 90.0 (should clamp to 90.0)
    llr_d[0] = 0.0;   // longitude
    llr_d[1] = 100.0; // latitude
    llr_d[2] = 1.0;   // radius

    xyz = psmrts::lonlatrad_to_xyz_d( llr_d );

    CHECK_THAT( xyz[0], Catch::Matchers::WithinAbs( 0.0, tolerance ));
    CHECK_THAT( xyz[1], Catch::Matchers::WithinAbs( 0.0, tolerance ));
    CHECK_THAT( xyz[2], Catch::Matchers::WithinAbs( 1.0, tolerance ));

    // test at with latitude < 90.0 (should clamp to -90.0)
    llr_d[0] =    0.0; // longitude
    llr_d[1] = -100.0; // latitude
    llr_d[2] =    1.0; // radius

    xyz = psmrts::lonlatrad_to_xyz_d( llr_d );

    CHECK_THAT( xyz[0], Catch::Matchers::WithinAbs(  0.0, tolerance ));
    CHECK_THAT( xyz[1], Catch::Matchers::WithinAbs(  0.0, tolerance ));
    CHECK_THAT( xyz[2], Catch::Matchers::WithinAbs( -1.0, tolerance ));

    // test at lon = 0 in the XY plane
    llr_d[0] = 0.0; // longitude
    llr_d[1] = 0.0; // latitude
    llr_d[2] = 1.0;  // radius

    xyz = psmrts::lonlatrad_to_xyz_d( llr_d );

    CHECK_THAT( xyz[0], Catch::Matchers::WithinAbs( 1.0, tolerance ));
    CHECK_THAT( xyz[1], Catch::Matchers::WithinAbs( 0.0, tolerance ));
    CHECK_THAT( xyz[2], Catch::Matchers::WithinAbs( 0.0, tolerance ));

    // test at lon = 90 in the XY plane
    llr_d[0] = 90.0; // longitude
    llr_d[1] = 0.0; // latitude
    llr_d[2] = 1.0;  // radius

    xyz = psmrts::lonlatrad_to_xyz_d( llr_d );

    CHECK_THAT( xyz[0], Catch::Matchers::WithinAbs( 0.0, tolerance ));
    CHECK_THAT( xyz[1], Catch::Matchers::WithinAbs( 1.0, tolerance ));
    CHECK_THAT( xyz[2], Catch::Matchers::WithinAbs( 0.0, tolerance ));

    // test at lon = 180 in the XY plane
    llr_d[0] = 180.0; // longitude
    llr_d[1] = 0.0; // latitude
    llr_d[2] = 1.0;  // radius

    xyz = psmrts::lonlatrad_to_xyz_d( llr_d );

    CHECK_THAT( xyz[0], Catch::Matchers::WithinAbs( -1.0, tolerance ));
    CHECK_THAT( xyz[1], Catch::Matchers::WithinAbs( 0.0, tolerance ));
    CHECK_THAT( xyz[2], Catch::Matchers::WithinAbs( 0.0, tolerance ));

    // test at lon = 270 in the XY plane
    llr_d[0] = 270.0; // longitude
    llr_d[1] = 0.0; // latitude
    llr_d[2] = 1.0;  // radius

    xyz = psmrts::lonlatrad_to_xyz_d( llr_d );

    CHECK_THAT( xyz[0], Catch::Matchers::WithinAbs( 0.0, tolerance ));
    CHECK_THAT( xyz[1], Catch::Matchers::WithinAbs( -1.0, tolerance ));
    CHECK_THAT( xyz[2], Catch::Matchers::WithinAbs( 0.0, tolerance ));

    // test at lon = 45, lat = 45
    llr_d[0] = 45.0; // longitude
    llr_d[1] = 45.0; // latitude
    llr_d[2] = 1.0;  // radius

    xyz = psmrts::lonlatrad_to_xyz_d( llr_d );

    CHECK_THAT( xyz[0], Catch::Matchers::WithinAbs( 0.5, tolerance ));
    CHECK_THAT( xyz[1], Catch::Matchers::WithinAbs( 0.5, tolerance ));
    CHECK_THAT( xyz[2], Catch::Matchers::WithinAbs( 0.707106, tolerance ));

    // test at lon = -45, lat = -45
    llr_d[0] = -45.0; // longitude
    llr_d[1] = -45.0; // latitude
    llr_d[2] = 1.0;  // radius

    xyz = psmrts::lonlatrad_to_xyz_d( llr_d );

    CHECK_THAT( xyz[0], Catch::Matchers::WithinAbs( 0.5, tolerance ));
    CHECK_THAT( xyz[1], Catch::Matchers::WithinAbs( -0.5, tolerance ));
    CHECK_THAT( xyz[2], Catch::Matchers::WithinAbs( -0.707106, tolerance ));

    // test at lon = 135, lat = 45
    llr_d[0] = 135.0; // longitude
    llr_d[1] = 45.0; // latitude
    llr_d[2] = 1.0;  // radius

    xyz = psmrts::lonlatrad_to_xyz_d( llr_d );

    CHECK_THAT( xyz[0], Catch::Matchers::WithinAbs( -0.5, tolerance ));
    CHECK_THAT( xyz[1], Catch::Matchers::WithinAbs( 0.5, tolerance ));
    CHECK_THAT( xyz[2], Catch::Matchers::WithinAbs( 0.707106, tolerance ));

    // test at lon = -135, lat = -45
    llr_d[0] = -135.0; // longitude
    llr_d[1] = -45.0; // latitude
    llr_d[2] = 1.0;  // radius

    xyz = psmrts::lonlatrad_to_xyz_d( llr_d );

    CHECK_THAT( xyz[0], Catch::Matchers::WithinAbs( -0.5, tolerance ));
    CHECK_THAT( xyz[1], Catch::Matchers::WithinAbs( -0.5, tolerance ));
    CHECK_THAT( xyz[2], Catch::Matchers::WithinAbs( -0.707106, tolerance ));
 }

TEST_CASE( "PSMRTS Rectangular to Latitudinal Coordinate Conversion Test", "[utilities][rect2lat][conversion]") {
    const double tolerance = 1.0e-6;
    Eigen::Vector3d xyz;
    Eigen::Vector3d llr_d;

    // test zero vector
    xyz[0] = 0.0; // x
    xyz[1] = 0.0; // y
    xyz[2] = 0.0; // z

    // convert to longitude, latitude, radius
    llr_d = psmrts::xyz_to_lonlatrad_d( xyz );

    CHECK_THAT( llr_d[0], Catch::Matchers::WithinAbs( 0.0, tolerance )); // lon
    CHECK_THAT( llr_d[1], Catch::Matchers::WithinAbs( 0.0, tolerance )); // lat
    CHECK_THAT( llr_d[2], Catch::Matchers::WithinAbs( 0.0, tolerance )); // rad

    // point at 45 lon, 45 lat
    xyz[0] = 0.5;         // x
    xyz[1] = 0.5;         // y
    xyz[2] = 0.707106781; // z

    // convert to longitude, latitude, radius
    llr_d = psmrts::xyz_to_lonlatrad_d( xyz );

    CHECK_THAT( llr_d[0], Catch::Matchers::WithinAbs( 45.0, tolerance )); // lon
    CHECK_THAT( llr_d[1], Catch::Matchers::WithinAbs( 45.0, tolerance )); // lat
    CHECK_THAT( llr_d[2], Catch::Matchers::WithinAbs(  1.0, tolerance )); // rad

    // point at 225 lon, -45 lat
    xyz[0] = -0.5;         // x
    xyz[1] = -0.5;         // y
    xyz[2] =  0.707106781; // z

    // convert to longitude, latitude, radius
    llr_d = psmrts::xyz_to_lonlatrad_d( xyz );

    CHECK_THAT( llr_d[0], Catch::Matchers::WithinAbs( 225.0, tolerance )); // lon
    CHECK_THAT( llr_d[1], Catch::Matchers::WithinAbs(  45.0, tolerance )); // lat
    CHECK_THAT( llr_d[2], Catch::Matchers::WithinAbs(   1.0, tolerance )); // rad

    // point at 315 lon, -45 lat
    xyz[0] =  0.5;         // x
    xyz[1] = -0.5;         // y
    xyz[2] = -0.707106781; // z

    // convert to longitude, latitude, radius
    llr_d = psmrts::xyz_to_lonlatrad_d( xyz );

    CHECK_THAT( llr_d[0], Catch::Matchers::WithinAbs( 315.0, tolerance )); // lon
    CHECK_THAT( llr_d[1], Catch::Matchers::WithinAbs( -45.0, tolerance )); // lat
    CHECK_THAT( llr_d[2], Catch::Matchers::WithinAbs(   1.0, tolerance )); // rad

    // point at 135 lon, 45 lat
    xyz[0] = -0.5;         // x
    xyz[1] =  0.5;         // y
    xyz[2] =  0.707106781; // z

    // convert to longitude, latitude, radius
    llr_d = psmrts::xyz_to_lonlatrad_d( xyz );

    CHECK_THAT( llr_d[0], Catch::Matchers::WithinAbs( 135.0, tolerance )); // lon
    CHECK_THAT( llr_d[1], Catch::Matchers::WithinAbs(  45.0, tolerance )); // lat
    CHECK_THAT( llr_d[2], Catch::Matchers::WithinAbs(   1.0, tolerance )); // rad
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
