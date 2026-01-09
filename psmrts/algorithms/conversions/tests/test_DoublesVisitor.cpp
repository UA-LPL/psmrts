#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/algorithms/conversions/AllConversionsVisitors.hpp>

// Extractor tyoes
using DoublesVisitor   = psmrts::algorithms::conversions::DoublesVisitor;
using StringsVisitor   = psmrts::algorithms::conversions::StringsVisitor;

using DoublesExtractor = psmrts::algorithms::conversions::Extractor<psmrts::ProductOption, DoublesVisitor>;
using StringsExtractor = psmrts::algorithms::conversions::Extractor<psmrts::ProductOption, StringsVisitor>;

using StringsComparator = psmrts::algorithms::conversions::Comparator<psmrts::ProductOption, StringsVisitor>;
using DoublesComparator = psmrts::algorithms::conversions::Comparator<psmrts::ProductOption, DoublesVisitor>;

TEST_CASE("DoublesVisitor Conversion Test", "[conversions][option][doubles][visitor]") {

  psmrts::ProductOption option_int( "integer", 100 );
  DoublesExtractor string_int( option_int );

  CHECK( string_int.name() == "integer" );
  CHECK( string_int.size() == 1 );
  CHECK( string_int.get() == 100.0 );
  CHECK( psmrts::isnull( string_int.get(1) ) );

  psmrts::ProductOption option_bool( "bool", true );
  DoublesExtractor string_bool( option_bool );
  CHECK( string_bool.get() == 1.0 );

  CHECK( DoublesComparator::compare(option_int, option_bool ) == true );
}

TEST_CASE("DoublesVisitor JSON Conversion Test", "[conversions][option][doubles][comparator][json]") {

  char text1[] = R"({"key1":3.14159})";
  psmrts::ProductOption option_j1( "key1", psmrts::json_utils::parse_json_string( text1 ) );
  DoublesExtractor d1( option_j1 );

  CHECK( d1.name() == "key1" );
  CHECK( d1.size() == 1 );
  CHECK( d1.get() == 3.14159 );
  CHECK( psmrts::isnull( d1.get(1) ) );
  CHECK( StringsExtractor( option_j1 ).get() == std::string( text1 ) );

  char text2[] = R"({"key2":3.14159154})";
  psmrts::ProductOption option_j2( "key2", psmrts::json_utils::parse_json_string( text2 ) );
  DoublesExtractor d2( option_j2 );
  CHECK( d2.get() == 3.14159154 );
  CHECK( StringsExtractor( option_j2 ).get() == std::string( text2 ) );

  CHECK(DoublesComparator::compare(option_j1, option_j2 ) == true );
  CHECK(DoublesComparator::compare(option_j1, option_j1 ) == true );
  CHECK(DoublesComparator::compare(option_j2, option_j2 ) == true );

  char text3[] = R"({"array":[1,2,3,4,5]})";
  psmrts::ProductOption option_j3( "array", psmrts::json_utils::parse_json_string( text3 ) );
  DoublesExtractor d3( option_j3 );
  CHECK( d3.get()  == 1.0 );
  CHECK( d3.get(1) == 2.0 );
  CHECK( d3.get(2) == 3.0 );
  CHECK( d3.get(3) == 4.0 );
  CHECK( d3.get(4) == 5.0 );
  CHECK( psmrts::isnull( d3.get(6) ) );
  CHECK( StringsExtractor( option_j3 ).get() == std::string( text3 ) ); 

  const bool GetAll = true;
  std::vector<double> partial = DoublesExtractor( option_j3 ).get_all( 2, 2 );

  char text4[] = R"({"array":[1.0,"2",3,4,"5","null"]})";
  psmrts::ProductOption option_j4( "array", psmrts::json_utils::parse_json_string( text4 ) );
  DoublesExtractor d4( option_j4 );
  CHECK( d4.get()  == 1.0 );
  CHECK( d4.get(1) == 2.0 );
  CHECK( d4.get(2) == 3.0 );
  CHECK( d4.get(3) == 4.0 );
  CHECK( d4.get(4) == 5.0 );
  CHECK( psmrts::isnull( d4.get(6) ) );
  CHECK( StringsExtractor( option_j4 ).get() == std::string( text4 ) ); 

  ordered_json text5 = "2.0";
  psmrts::ProductOption option_j5( "array", text5 );
  DoublesExtractor d5( option_j5 );
  CHECK( d5.get()  == 2.0 );  
  CHECK( StringsExtractor( option_j5 ).get() == std::string( text5 ) ); 
}

