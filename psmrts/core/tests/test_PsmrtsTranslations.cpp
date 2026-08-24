#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsInventory.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>


TEST_CASE( "PSMRTS Translations Default", "[translations][inventory][default]") {

  psmrts::PsmrtsTranslations translations;

  CHECK( translations.name()              == "translations" );
  CHECK( translations.parameters().size() == 0 );
  CHECK( translations.parameters().name() == "parameters" );

  CHECK( translations.environment().size() == 0 );
  CHECK( translations.environment().name() == "environment" );

}

TEST_CASE("PSMRTS Translation Environment", "[translations][inventory][environment]") {
  psmrts::PsmrtsTranslations translations = psmrts::PsmrtsTranslations::create();

  // Check environment is initially empty
  CHECK( translations.environment().size() > 0 );

  // Check that common environment variables exist
  // These checks are conditional because CI/test environments may vary
  if ( translations.environment().contains("PATH")) {
    CHECK(translations.environment().find("PATH").empty() == false);
  }

#ifdef _WIN32
  if (translations.environment().contains("USERNAME")) {
    CHECK(translations.environment().find("USERNAME").empty() == false);
  }
#else
  if (translations.environment().contains("USER")) {
    CHECK( translations.environment().find("USER").empty() == false );
    CHECK( translations.environment().contains("user")     == false );
  }
#endif

  // Store current size after first load
  const size_t size_after_first_merge = translations.environment().size();

  // Check access to a known key
  for (const auto& key : translations.environment().keys()) {
    CHECK_NOTHROW(translations.environment().find(key));
  }

  // Remove one environment variable
  std::string first_key = translations.environment().keys().front();
  translations.remove_environment(first_key);
  CHECK(translations.environment().contains(first_key) == false);

  // Re-add the removed variable manually
  translations.add_environment(first_key, "manually_added_value");
  CHECK(translations.environment().contains(first_key));
  CHECK(translations.environment().find(first_key) == "manually_added_value");
}


TEST_CASE("PSMRTS Translation Environment", "[translations][inventory][parameters]") {

  psmrts::PsmrtsTranslations translations = psmrts::PsmrtsTranslations::create();

  translations.add_environment( "ISISDATA", "/opt/isis/data" );
  translations.add_environment( "ISIS3DATA", "$ISISDATA" );

  translations.add_parameter( "OsirisRex", "$ISISDATA/osirisrex" );
  translations.add_parameter( "mRo", "$ISIS3DATA/mro" );

  // Only translate with one iteration
  CHECK( translations.translate_path( "$ISIS3DATA", 1)   == "$ISISDATA" );

  // Environment variables are case sensitive
  CHECK( translations.environment().find("ISISDATA")     == "/opt/isis/data" );
  CHECK( translations.environment().contains("isisdata") == false );
  CHECK( translations.environment().find("ISIS3DATA")    == "$ISISDATA" );

  // Parameter variables are case insensitive
  CHECK( translations.parameters().find("osirisrex")     == "$ISISDATA/osirisrex" );
  CHECK( translations.parameters().contains("OsirisRex") == true );
  CHECK( translations.parameters().contains("mrO")       == true);

  CHECK( translations.translate_path( "$osirisrex")             == "/opt/isis/data/osirisrex" );
  CHECK( translations.translate_path( "$osirisrex/kernels/dsk") == "/opt/isis/data/osirisrex/kernels/dsk" );

  CHECK( translations.translate_path( "$mro")             == "/opt/isis/data/mro" );
  CHECK( translations.translate_path( "$MRO/kernels/dsk") == "/opt/isis/data/mro/kernels/dsk" );
}
