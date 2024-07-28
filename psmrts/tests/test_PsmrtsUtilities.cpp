#include <psmrts_catch2_environment.hpp>

#include <PsmrtsUtilities.hpp>


TEST_CASE( "PSMRTS Clock Time Tests", "[utilities][time]" ) {

    using namespace std::literals;

    psmrts::PsmrtsThreadSafeCounter counter_t;
    CHECK( counter_t.count() == 0 );
    CHECK( counter_t.hitme() == 1 );
    CHECK( counter_t.count() == 1 );

    CHECK( counter_t++       == 2 );
    CHECK( counter_t.count() == 2 );

    INFO( to_string( counter_t.snapshot() ) );

    psmrts::SYSTEM_CLOCK_TIME currently = psmrts::system_clock_time();
    psmrts::SYSTEM_CLOCK_TIME plus_one_s = currently + 1s;
    CHECK( psmrts::elapsed_clock_time_s( currently, plus_one_s ) == 1.0 );
    CHECK( psmrts::elapsed_clock_time_ms( currently, plus_one_s ) == 1000.0 );

    CHECK( psmrts::psmrts_tolower( "tHiS iS A LOWERcasE strINg") == "this is a lowercase string" );
    CHECK( psmrts::psmrts_toupper( "tHiS iS A upPErcasE strINg") == "THIS IS A UPPERCASE STRING" );
}

TEST_CASE( "PSMRTS Make Path Test", "[utilities][path]") {
// Test psmrts_make_path --> string conversion for complete path

std::string path = psmrts::psmrts_make_path( "tests", "test_PsmrtsUtilities.cpp" );
std::string directory = psmrts::psmrts_make_path( "tests" );
std::string path_only = psmrts::psmrts_make_path( "", "test_PsmrtsUtilities.cpp" );

CHECK ( path == "tests/test_PsmrtsUtilities.cpp" );
CHECK ( directory == "tests" );
CHECK ( path_only == "test_PsmrtsUtilities.cpp" );

}

TEST_CASE( "PSMRTS Longitude Domain Test - 360 to 180", "[utilities][longitude][180Domain]" ) {

    const double tolerance = 1.0e-6;

    CHECK_THAT( psmrts::to180LongitudeDomain_d( 0.0 ), Catch::Matchers::WithinAbs( 0.0, tolerance ));
    CHECK_THAT( psmrts::to180LongitudeDomain_d( 90.0 ), Catch::Matchers::WithinAbs( 90.0, tolerance ));
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
    // 1000 -> 280? Expected for this value?

}
