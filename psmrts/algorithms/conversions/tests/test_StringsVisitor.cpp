#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/core/AllOptionConversions.hpp>


TEST_CASE("StringsVisitor Conversion Test", "[conversions][option][strings][visitor]") {

  psmrts::ProductOption option_int( "integer", 100 );
  psmrts::OptionStringsExtractor string_int( option_int );

  CHECK( string_int.name() == "integer" );
  CHECK( string_int.size() == 1 );
  CHECK( string_int.get()  == "100" );
  CHECK( string_int.get(1) == "" );

  psmrts::ProductOption option_bool( "bool", true );
  psmrts::OptionStringsExtractor string_bool( option_bool );
  CHECK( string_bool.get() == "true" );

  CHECK(psmrts::OptionStringsComparator::compare(option_int, option_bool ) == true );

  psmrts::ProductOption option_dbl("double", 42.123 );
  psmrts::OptionStringsExtractor string_dbl( option_dbl );
  CHECK( string_dbl.name() == "double" );
  CHECK( string_dbl.size() == 1 );
  CHECK( string_dbl.get()  == "42.123000000" );
  CHECK( string_dbl.get(1) == "" );

  psmrts::ProductOption option_sizet( "sizet", static_cast<size_t>( 100 ) );
  psmrts::OptionStringsExtractor string_sizet( option_sizet );
  CHECK( string_sizet.name() == "sizet" );
  CHECK( string_sizet.size() == 1 );
  CHECK( string_sizet.get()  == "100" );
  CHECK( string_sizet.get(1) == "" );

  CHECK( psmrts::OptionStringsComparator::compare( option_int, option_sizet ) == true );

  psmrts::ProductOption option_array( "double ARRAY", std::vector<double>{0.0, 1.1, 2.2, 3.8, 4.4} );
  psmrts::OptionStringsExtractor array_dbl( option_array );
  CHECK( array_dbl.name() == "double array" );
  CHECK( array_dbl.size() == 5 );
  CHECK( array_dbl.get()  == "0.000000000" );
  CHECK( array_dbl.get(1) == "1.100000000" );
  CHECK( array_dbl.get(2) == "2.200000000" );
  CHECK( array_dbl.get(3) == "3.800000000" );
  CHECK( array_dbl.get(4) == "4.400000000" );
  CHECK( array_dbl.get(5) == "" );

  psmrts::ProductOption option_array2( "InT ArRay", std::vector<int>{100, 200, 0} );
  psmrts::OptionStringsExtractor array_ints( option_array2 );
  CHECK( array_ints.name() == "int array" );
  CHECK( array_ints.size() == 3 );
  CHECK( array_ints.get()  == "100" );
  CHECK( array_ints.get(1) == "200" );
  CHECK( array_ints.get(2) == "0" );
  CHECK( array_ints.get(3) == "" );

  psmrts::ProductOption option_array3( "SIZET ArRay", std::vector<size_t>{100, 200, 0} );
  psmrts::OptionStringsExtractor array_sizet( option_array3 );
  CHECK( array_sizet.name() == "sizet array" ); 
  CHECK( array_sizet.size() == 3 );
  CHECK( array_sizet.get()  == "100" );
  CHECK( array_sizet.get(1) == "200" );
  CHECK( array_sizet.get(2) == "0" );
  CHECK( array_sizet.get(3) == "" );

  psmrts::ProductOption option_array4( "StRiNG ArRay", std::vector<std::string>{"one", "4.2", "-42"} );
  psmrts::OptionStringsExtractor array_str( option_array4 );
  CHECK( array_str.name() == "string array" ); 
  CHECK( array_str.size() == 3 );
  CHECK( array_str.get()  == "one" );
  CHECK( array_str.get(1) == "4.2" ); 
  CHECK( array_str.get(2) == "-42" );
  CHECK( array_str.get(3) == "" );

  CHECK( psmrts::OptionStringsComparator::compare( option_array2, option_array3 ) == true );
  CHECK( psmrts::OptionStringsComparator::compare( option_array2, option_array)   == false );
}

TEST_CASE("StringsVisitor JSON Conversion Test", "[conversion][option][strings][comparator][json]") {
  char text1[] = R"({"key1":360})";
  psmrts::ProductOption option_j1( "key1", psmrts::json_utils::parse_json_string( text1 ) );
  psmrts::OptionStringsExtractor s1( option_j1 );
  CHECK( s1.name() == "key1" );
  CHECK( s1.size() == 1 );
  CHECK( s1.get() == "360" ); 

  char text2[] = R"({"key2":3.14159154})";
  psmrts::ProductOption option_j2("key2", psmrts::json_utils::parse_json_string( text2 ) );
  psmrts::OptionStringsExtractor s2( option_j2 ); 
  CHECK( s2.name() == "key2" );
  CHECK( s2.size() == 1 );
  CHECK( s2.get()  == "3.14159154" );

  char text3[] = R"({"array":[5.0, 4.5, 3.2, 2.0, 1.9]})";
  psmrts::ProductOption option_j3( "array", psmrts::json_utils::parse_json_string( text3 ) );
  psmrts::OptionStringsExtractor s3( option_j3 );
  CHECK( s3.name() == "array" );
  CHECK( s3.size() == 5 ); 
  CHECK( s3.get(0) == "5.0" ); 
  CHECK( s3.get(1) == "4.5" ); 
  CHECK( s3.get(2) == "3.2" ); 
  CHECK( s3.get(3) == "2.0" ); 
  CHECK( s3.get(4) == "1.9" ); 

  char text4[] = R"({"array":[1.0, "2", 3.4, 4, "5", "null"]})";
  psmrts::ProductOption option_j4("array", psmrts::json_utils::parse_json_string( text4 ) );
  psmrts::OptionStringsExtractor s4( option_j4 );
  CHECK( s4.get()  == "1.0" ); 
  CHECK( s4.get(1) == "\"2\"" ); 
  CHECK( s4.get(2) == "3.4" );
  CHECK( s4.get(3) == "4" ); 
  CHECK( s4.get(4) == "\"5\"" );
  CHECK( s4.get(5) == "\"null\"" ); 
}

