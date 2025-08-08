#define CATCH_CONFIG_MAIN
#include <psmrts_catch2_environment.hpp>

static const std::string test_version = "0.1.0";

TEST_CASE( "naif test suite", "[naif][introduction]") {

  auto version = test_version;
  // spdlog::info("naif Catch Test Suite version {}", version );
  INFO("naif Catch Test Suite version " << test_version);
  REQUIRE( version == test_version );

  auto const &catch2_version = Catch::libraryVersion();
  auto major_version = catch2_version.majorVersion;
  auto minor_version = catch2_version.minorVersion;
  auto patch_number  = catch2_version.patchNumber;
  std::string catch_vstr = std::to_string(major_version) + "." +
                           std::to_string(minor_version) + "." +
                           std::to_string(patch_number);

  INFO("Catch2 Version " << catch_vstr);
  REQUIRE( catch_vstr == catch_vstr );
}
