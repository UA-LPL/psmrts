#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/algorithms/conversions/AllConversionsVisitors.hpp>

using DoublesVisitor   = psmrts::algorithms::conversions::DoublesVisitor;
using StringsVisitor   = psmrts::algorithms::conversions::StringsVisitor;

using DoublesExtractor = psmrts::algorithms::conversions::Extractor<psmrts::ProductOption, DoublesVisitor>;
using StringsExtractor = psmrts::algorithms::conversions::Extractor<psmrts::ProductOption, StringsVisitor>;

using StringsComparator = psmrts::algorithms::conversions::Comparator<psmrts::ProductOption, StringsVisitor>;
using DoublesComparator = psmrts::algorithms::conversions::Comparator<psmrts::ProductOption, DoublesVisitor>;

TEST_CASE("StringsVisitor Conversion Test", "[conversions][option][strings][visitor]") {

  psmrts::ProductOption option_int( "integer", 100 );
  StringsExtractor string_int( option_int );

  CHECK( string_int.name() == "integer" );
  CHECK( string_int.size() == 1 );
  CHECK( string_int.get() == "100" );
  CHECK( string_int.get(1) == "" );

  psmrts::ProductOption option_bool( "bool", true );
  StringsExtractor string_bool( option_bool );
  CHECK( string_bool.get() == "true" );

  CHECK(StringsComparator::compare(option_int, option_bool ) == true );
}
