#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/core/AllOptionConversions.hpp>


TEST_CASE("StringsVisitor Conversion Test", "[conversions][option][strings][visitor]") {

  psmrts::ProductOption option_int( "integer", 100 );
  psmrts::OptionStringsExtractor string_int( option_int );

  CHECK( string_int.name() == "integer" );
  CHECK( string_int.size() == 1 );
  CHECK( string_int.get() == "100" );
  CHECK( string_int.get(1) == "" );

  psmrts::ProductOption option_bool( "bool", true );
  psmrts::OptionStringsExtractor string_bool( option_bool );
  CHECK( string_bool.get() == "true" );

  CHECK(psmrts::OptionStringsComparator::compare(option_int, option_bool ) == true );
}
