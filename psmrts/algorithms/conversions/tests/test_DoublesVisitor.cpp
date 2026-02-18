#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/core/AllOptionConversions.hpp>

// Extractor tyoes

TEST_CASE("DoublesVisitor Conversion Test", "[conversions][option][doubles][visitor]") {

  psmrts::ProductOption option_int( "integer", 100 );
  psmrts::OptionDoublesExtractor string_int( option_int );

  CHECK( string_int.name() == "integer" );
  CHECK( string_int.size() == 1 );
  CHECK( string_int.get() == 100.0 );
  CHECK( psmrts::isnull( string_int.get(1) ) );

  psmrts::ProductOption option_bool( "bool", true );
  psmrts::OptionDoublesExtractor string_bool( option_bool );
  CHECK( string_bool.get() == 1.0 );

  CHECK( psmrts::OptionDoublesComparator::compare(option_int, option_bool ) == false );
  CHECK( psmrts::OptionBoolsComparator::compare(option_int, option_bool )   == true );
}

TEST_CASE("DoublesVisitor JSON Conversion Test", "[conversions][option][doubles][comparator][json]") {

  char text1[] = R"({"key1":3.14159})";
  psmrts::ProductOption option_j1( "key1", psmrts::json_utils::parse_json_string( text1 ) );
  CHECK( option_j1.to_string() == "3.14159") ;

  psmrts::OptionDoublesExtractor d1( option_j1 );

  CHECK( d1.name() == "key1" );
  CHECK( d1.size() == 1 );
  CHECK( d1.get() == 3.14159 );
  CHECK( psmrts::isnull( d1.get(1) ) );
  CHECK( psmrts::OptionStringsExtractor( option_j1 ).get() == "3.14159" );

  char text2[] = R"({"key2":3.14159154})";
  psmrts::ProductOption option_j2( "key2", psmrts::json_utils::parse_json_string( text2 ) );
  psmrts::OptionDoublesExtractor d2( option_j2 );
  CHECK( d2.get() == 3.14159154 );
  CHECK( psmrts::OptionStringsExtractor( option_j2 ).get() == "3.14159154" );

  CHECK(psmrts::OptionDoublesComparator::compare(option_j1, option_j2 ) == false );
  CHECK(psmrts::OptionDoublesComparator::compare(option_j1, option_j1 ) == true );
  CHECK(psmrts::OptionDoublesComparator::compare(option_j2, option_j2 ) == true );

  char text3[] = R"({"array":[1,2,3,4,5]})";
  psmrts::ProductOption option_j3( "array", psmrts::json_utils::parse_json_string( text3 ) );
  psmrts::OptionDoublesExtractor d3( option_j3 );
  CHECK( d3.size() == 5 );
  CHECK( d3.get()  == 1.0 );
  CHECK( d3.get(1) == 2.0 );
  CHECK( d3.get(2) == 3.0 );
  CHECK( d3.get(3) == 4.0 );
  CHECK( d3.get(4) == 5.0 );
  CHECK( psmrts::isnull( d3.get(6) ) );
  CHECK( psmrts::OptionStringsExtractor( option_j3 ).get() == "1" ); 

  const bool GetAll = true;
  std::vector<double> partial = psmrts::OptionDoublesExtractor( option_j3 ).get_all( 2, 2 );

  char text4[] = R"({"array":[1.0,"2",3,4,"5","null"]})";
  psmrts::ProductOption option_j4( "array", psmrts::json_utils::parse_json_string( text4 ) );
  psmrts::OptionDoublesExtractor d4( option_j4 );
  CHECK( d4.size() == 6 );
  CHECK( d4.get()  == 1.0 );
  CHECK( d4.get(1) == 2.0 );
  CHECK( d4.get(2) == 3.0 );
  CHECK( d4.get(3) == 4.0 );
  CHECK( d4.get(4) == 5.0 );
  CHECK( psmrts::isnull( d4.get(6) ) );
  CHECK( psmrts::OptionStringsExtractor( option_j4 ).get() == "1.0" ); 

  ordered_json text5 = "2.0";
  psmrts::ProductOption option_j5( "array", text5 );
  psmrts::OptionDoublesExtractor d5( option_j5 );
  CHECK( d5.get()  == 2.0 );  
  CHECK( psmrts::OptionStringsExtractor( option_j5 ).get() == "2.0"); 
}

TEST_CASE("DoublesVisitor Oddities Conversion Test", "[conversions][option][doubles][oddities]") {

  auto d = psmrts::ProductOption("d", 1.0);
  auto xtr = psmrts::OptionStringsExtractor(d, "null" );

  std::vector<std::string> data;
  auto v   = xtr.create_visitor( data, d, xtr.traits(), "null" );
  auto cvp = v.parameters();
  CHECK( cvp.index() == 0 );
  CHECK( cvp.count() == 1 );

  CHECK( cvp.done( 1, d.size() )           == true );
  CHECK( cvp.add_valid_value(1, d.size() ) == false );
  CHECK( cvp.dataset_index(1, d.size() )   == 0 );

  CHECK( cvp.traits().digits()     == 9 );
  CHECK( cvp.traits().tolerance()  == 1.0e-9 );
  CHECK( cvp.traits().spaces()  == -1 );

  CHECK( xtr.size() == 1 );
  CHECK( xtr.default_value() == "null" );
  CHECK( xtr.get_all( 1, 2) == std::vector<std::string>( { "null", "null"} ) );
  CHECK( xtr.get(1) == "null" );
  auto e = psmrts::ProductOption("e", xtr.get_all(1, 10) );
  CHECK( e.size() == 10 );
  CHECK( e.to_string(0) == "null" );


}