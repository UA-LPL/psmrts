#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/algorithms/conversions/OptionStrings.hpp>


TEST_CASE("OptionStrings Conversion Test", "[conversions][option][strings]") {

  psmrts::ProductOption option_int( "integer", 100 );
  psmrts::algorithms::conversions::OptionStrings string_int( option_int );

  CHECK( string_int.name() == "integer" );
  CHECK( string_int.size() == 1 );
  CHECK( string_int.get() == "100" );
  CHECK( string_int.get(1) == "" );

  psmrts::ProductOption option_bool( "bool", true );
  psmrts::algorithms::conversions::OptionStrings string_bool( option_bool );
  CHECK( string_bool.get() == "true" );

  CHECK(psmrts::algorithms::conversions::OptionStrings::compare(option_int, option_bool ) == false );
}
