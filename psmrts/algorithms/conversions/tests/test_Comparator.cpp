#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/algorithms/conversions/Comparator.hpp>
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/products/ProductOption.hpp>
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

  CHECK( DoublesComparator::compare(option_d1, option_d2)     == false );
  CHECK( DoublesComparator::compare(option_d1, option_string) == false );

  CHECK( DoublesComparator::compare(option_d1, option_d1)     == true );
  CHECK( DoublesComparator::compare(option_d2, option_d2)     == true );

  psmrts::ProductOption dbl1( "dbl1", { 0.000005, 5.0e-6 } );
  psmrts::ProductOption dbl2( "dbl2", { 5.0e-6 ,  0.000005} );

  CHECK( DoublesComparator::compare( dbl1, dbl2 )  == true );
  CHECK( DoublesComparator::compare( dbl2, dbl1 )  == true );

  CHECK( StringsComparator::compare( dbl1, dbl2 )  == true );
  CHECK( SizetsComparator::compare( dbl1, dbl2 )   == true );
  CHECK( IntegersComparator::compare( dbl1, dbl2 ) == true );
  
  CHECK( DoublesComparator::compare(dbl1, dbl1 )   == true );
  CHECK( DoublesComparator::compare(dbl2, dbl2 )   == true );

  std::vector<bool> matches;
  DoublesComparator dbl_cmp( dbl1 );
  CHECK ( dbl_cmp.compare( dbl2, matches ) == true );
  CHECK( matches.size()     == 2 );
  CHECK( matches[0]         == true );
  CHECK( matches[1]         == true );

  psmrts::ProductOption i_arr( "iarray", { 1, 2, 3, 4, 5 } );
  psmrts::ProductOption st_arr( "starray", std::vector<size_t>( { 1, 2, 3, 4, 5 } ) );
  psmrts::ProductOption s_arr( "sarray", { "1", "2", "3", "4", "5" } );
  CHECK( DoublesComparator::compare( i_arr, s_arr )   == true );
  CHECK( DoublesComparator::compare( i_arr, st_arr )  == true );
  CHECK( DoublesComparator::compare( s_arr, st_arr )  == true );

  CHECK( StringsComparator::compare( s_arr, st_arr )   == true );
  CHECK( IntegersComparator::compare( s_arr, st_arr )  == true );
  CHECK( SizetsComparator::compare( i_arr, s_arr )     == true );
  CHECK( SizetsComparator::compare( s_arr, st_arr )     == true );

  psmrts::ProductOption default_s1( "tracer", "naifdsk" );
  psmrts::ProductOption default_s2( "default", "naifdsk" );
  CHECK( StringsComparator::compare( default_s1, default_s2 )   == true );

  psmrts::ProductOption segment_s1( "dsk_segment_index", { 0 } );
  psmrts::ProductOption segment_s2( "default", "0" );
  CHECK( StringsComparator::compare( segment_s1, segment_s2 )   == true ); 
  
  psmrts::ProductOption bool1( "bool1", false );
  psmrts::ProductOption bool2( "bool2", true );
  CHECK( StringsComparator::compare( bool1, bool2 )   == false ); 
}
