#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/algorithms/conversions/OptionDoubles.hpp>


TEST_CASE("OptionDoubles Conversion Test", "[conversions][option][doubles]") {

  psmrts::ProductOption option_int( "integer", 100 );
  psmrts::algorithms::conversions::OptionDoubles string_int( option_int );

  CHECK( string_int.name() == "integer" );
  CHECK( string_int.size() == 1 );
  CHECK( string_int.get() == 100.0 );
  CHECK( psmrts::isnull( string_int.get(1) ) );

  psmrts::ProductOption option_bool( "bool", true );
  psmrts::algorithms::conversions::OptionDoubles string_bool( option_bool );
  CHECK( string_bool.get() == 1.0 );

  CHECK(psmrts::algorithms::conversions::OptionDoubles::compare(option_int, option_bool ) == false );
}
