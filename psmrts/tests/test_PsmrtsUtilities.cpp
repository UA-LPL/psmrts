
#include <psmrts_catch2_environment.hpp>

#include <PsmrtsUtilities.hpp>

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