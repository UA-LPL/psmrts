#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/algorithms/conversions/Comparator.hpp>
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/algorithms/conversions/AllConversionsVisitors.hpp>

// Visitor, Extractor, and Comparator types
using BoolsVisitor    = psmrts::algorithms::conversions::BoolsVisitor;
using DoublesVisitor  = psmrts::algorithms::conversions::DoublesVisitor;
using IntegersVisitor = psmrts::algorithms::conversions::IntegersVisitor;
using SizetsVisitor   = psmrts::algorithms::conversions::SizetsVisitor;
using StringsVisitor  = psmrts::algorithms::conversions::StringsVisitor;

using BoolsExtractor    = psmrts::algorithms::conversions::Extractor<psmrts::ProductOption, BoolsVisitor>;
using DoublesExtractor  = psmrts::algorithms::conversions::Extractor<psmrts::ProductOption, DoublesVisitor>;
using IntegersExtractor = psmrts::algorithms::conversions::Extractor<psmrts::ProductOption, IntegersVisitor>;
using SizetsExtractor   = psmrts::algorithms::conversions::Extractor<psmrts::ProductOption, SizetsVisitor>;
using StringsExtractor  = psmrts::algorithms::conversions::Extractor<psmrts::ProductOption, StringsVisitor>;

using BoolsComparator    = psmrts::algorithms::conversions::Comparator<psmrts::ProductOption, BoolsVisitor>;
using DoublesComparator  = psmrts::algorithms::conversions::Comparator<psmrts::ProductOption, DoublesVisitor>;
using IntegersComparator = psmrts::algorithms::conversions::Comparator<psmrts::ProductOption, IntegersVisitor>;
using SizetsComparator   = psmrts::algorithms::conversions::Comparator<psmrts::ProductOption, SizetsVisitor>;
using StringsComparator  = psmrts::algorithms::conversions::Comparator<psmrts::ProductOption, StringsVisitor>;


TEST_CASE("Comparator Default Test", "[comparators][default]") {

  // default comparator constructors
  BoolsComparator    boolsComp;
  DoublesComparator  doublesComp;
  IntegersComparator intsComp;
  SizetsComparator   sizetsComp;
  StringsComparator  stringsComp;
  
  // validate sizes
  CHECK( boolsComp.size()   == 1 );
  CHECK( doublesComp.size() == 1 );
  CHECK( intsComp.size()    == 1 );
  CHECK( sizetsComp.size()  == 1 );
  CHECK( stringsComp.size() == 1 );

  // validate default ProductOption values
  CHECK( boolsComp.default_value()   == false );
  CHECK( psmrts::isnull( doublesComp.default_value() ) );
  CHECK( intsComp.default_value()    == std::numeric_limits<int>::min() );
  CHECK( sizetsComp.default_value()  == std::numeric_limits<size_t>::max() );
  CHECK( stringsComp.default_value() == "" );


  CHECK( boolsComp.compare( doublesComp.container() ) == true );
}

TEST_CASE("Comparator Doubles Test", "[comparators][doubles]") {

  psmrts::ProductOption option_d1( "double", 1.2345678 );
  psmrts::ProductOption option_d2( "double", 3.14159154 );
  psmrts::ProductOption option_string( "double", "527.987" );

  DoublesComparator d1_cmp( option_d1 );
  CHECK( d1_cmp.size()                  == 1 );
  CHECK( d1_cmp.container().to_string() == "1.234567800" );
  CHECK( option_d2.to_string()          == "3.141591540" );

  // why are these failing?
  CHECK( DoublesComparator::compare(option_d1, option_d2) == false );
  CHECK( DoublesComparator::compare(option_d1, option_string) == false );
}
