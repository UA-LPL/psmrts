#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/algorithms/conversions/OptionDoubles.hpp>
#include <psmrts/algorithms/conversions/OptionStrings.hpp>


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

TEST_CASE("OptionDoubles JSON Conversion Test", "[conversions][option][doubles][json]") {
  using OptionDoubles = psmrts::algorithms::conversions::OptionDoubles;
  using OptionStrings = psmrts::algorithms::conversions::OptionStrings;

  char text1[] = R"({"key1":3.14159})";
  psmrts::ProductOption option_j1( "key1", psmrts::json_utils::parse_json_string( text1 ) );
  OptionDoubles d1( option_j1 );

  CHECK( d1.name() == "key1" );
  CHECK( d1.size() == 1 );
  CHECK( d1.get() == 3.14159 );
  CHECK( psmrts::isnull( d1.get(1) ) );
  CHECK( OptionStrings( option_j1 ).get() == std::string( text1 ) );

  char text2[] = R"({"key2":3.14159154})";
  psmrts::ProductOption option_j2( "key2", psmrts::json_utils::parse_json_string( text2 ) );
  psmrts::algorithms::conversions::OptionDoubles d2( option_j2 );
  CHECK( d2.get() == 3.14159154 );
  CHECK( OptionStrings( option_j2 ).get() == std::string( text2 ) );

  CHECK(OptionDoubles::compare(option_j1, option_j2 ) == false );
  CHECK(OptionDoubles::compare(option_j1, option_j1 ) == true );
  CHECK(OptionDoubles::compare(option_j2, option_j2 ) == true );

  char text3[] = R"({"array":[1,2,3,4,5]})";
  psmrts::ProductOption option_j3( "array", psmrts::json_utils::parse_json_string( text3 ) );
  OptionDoubles d3( option_j3 );
  CHECK( d3.get()  == 1.0 );
  CHECK( d3.get(1) == 2.0 );
  CHECK( d3.get(2) == 3.0 );
  CHECK( d3.get(3) == 4.0 );
  CHECK( d3.get(4) == 5.0 );
  CHECK( psmrts::isnull( d3.get(6) ) );
  CHECK( OptionStrings( option_j3 ).get() == std::string( text3 ) ); 

  const bool GetAll = true;
  std::vector<double> partial = OptionDoubles( option_j3 ).get_all( 2, 2 );

  char text4[] = R"({"array":[1.0,"2",3,4,"5","null"]})";
  psmrts::ProductOption option_j4( "array", psmrts::json_utils::parse_json_string( text4 ) );
  psmrts::algorithms::conversions::OptionDoubles d4( option_j4 );
  CHECK( d4.get()  == 1.0 );
  CHECK( d4.get(1) == 2.0 );
  CHECK( d4.get(2) == 3.0 );
  CHECK( d4.get(3) == 4.0 );
  CHECK( d4.get(4) == 5.0 );
  CHECK( psmrts::isnull( d4.get(6) ) );
  CHECK( OptionStrings( option_j4 ).get() == std::string( text4 ) ); 

  ordered_json text5 = "2.0";
  psmrts::ProductOption option_j5( "array", text5 );
  psmrts::algorithms::conversions::OptionDoubles d5( option_j5 );
  CHECK( d5.get()  == 2.0 );  
  CHECK( OptionStrings( option_j5 ).get() == std::string( text5 ) ); 
}

