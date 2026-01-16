#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/core/AllOptionConversions.hpp>
#include <psmrts/algorithms/conversions/SizetsVisitor.hpp>

/**
  * SizetsVisitor Conversion Test
  * 
  * Tests SizetsVisitor functionality utilizing psmrts::ProductOptions objects.
  */
TEST_CASE("SizetsVisitor Conversion Test", "[conversions][option][sizets][visitor]") {

  // construct ProductOption object with name "double" and a single double entry 
  psmrts::ProductOption option_double( "double", 3.1415926535 );
  
  // construct OptionSizetsExtractor from option_double above
  psmrts::OptionSizetsExtractor sizet_double( option_double );

  // validate sizet_double extractor 
  CHECK( sizet_double.name() == "double" );     // name
  CHECK( sizet_double.size() == 1 );            // size
  CHECK( sizet_double.get() == 3 );             // input double has been converted to a size_t
  CHECK( sizet_double.get(1) ==
         std::numeric_limits<size_t>::max() );  // beyond input index limit generates default

  // construct ProductOption object with name "bool" and a single boolean entry 
  psmrts::ProductOption option_bool( "bool", true );

  // construct OptionSizetsExtractor from sizet_bool above
  psmrts::OptionSizetsExtractor sizet_bool( option_bool );

  // validate sizet_bool extractor 
  CHECK( sizet_bool.name() == "bool" );         // name
  CHECK( sizet_bool.size() == 1 );              // size
  CHECK( sizet_bool.get() == 1 );               // input bool has been converted to a size_t
  CHECK( sizet_bool.get(1) ==
         std::numeric_limits<size_t>::max() );  // beyond input index limit generates default
  
  // ensure option_double and option_bool are equivalent
  CHECK(psmrts::OptionSizetsComparator::compare(option_double, option_bool ) == true );
}

/**
  * SizetsVisitor JSON Conversion Test
  * 
  * Tests SizetsVisitor JSON functionality utilizing psmrts::ProductOptions objects.
  */
TEST_CASE("SizetsVisitor JSON Conversion Test", "[conversions][option][sizets][json]") {

  char text1[] = R"({"key1":3})";
  psmrts::ProductOption option_j1( "key1", psmrts::json_utils::parse_json_string( text1 ) );
  psmrts::OptionSizetsExtractor szt1( option_j1 );
  
  CHECK( szt1.name() == "key1" );
  CHECK( szt1.size() == 1 );
  CHECK( szt1.get() == 3 );
  CHECK( szt1.get(1) == std::numeric_limits<size_t>::max() );
  CHECK( psmrts::OptionStringsExtractor( option_j1 ).get() == std::string( text1 ) );

  char text2[] = R"({"key2":3.14159154})";
  psmrts::ProductOption option_j2( "key2", psmrts::json_utils::parse_json_string( text2 ) );
  psmrts::OptionSizetsExtractor szt2( option_j2 );
  CHECK( szt2.get() == 3 );
  CHECK( szt2.get(1) == std::numeric_limits<size_t>::max() );
  CHECK( psmrts::OptionStringsExtractor( option_j2 ).get() == std::string( text2 ) );

  CHECK(psmrts::OptionSizetsComparator::compare(option_j1, option_j2 ) == true );
  CHECK(psmrts::OptionSizetsComparator::compare(option_j1, option_j1 ) == true );
  CHECK(psmrts::OptionSizetsComparator::compare(option_j2, option_j2 ) == true );

  // validate conversion from a negative double to a size_t
  // should generate the default value of size_t
  // e.g. std::numeric_limits<size_t>::max()
  char text3[] = R"({"key3":-3.14159154})";
  psmrts::ProductOption option_j3( "key3", psmrts::json_utils::parse_json_string( text3 ) );
  psmrts::OptionSizetsExtractor szt3( option_j3 );
  CHECK( psmrts::OptionStringsExtractor( option_j3 ).get() == std::string( text3 ) );
  CHECK( szt3.get()  == std::numeric_limits<size_t>::max() );
  CHECK( szt3.get(1) == std::numeric_limits<size_t>::max() );

  char text4[] = R"({"array":[1,2,3,4,5]})";
  psmrts::ProductOption option_j4( "array", psmrts::json_utils::parse_json_string( text4 ) );
  psmrts::OptionSizetsExtractor szt4( option_j4 );
  CHECK( szt4.get()  == 1.0 );
  CHECK( szt4.get(1) == 2.0 );
  CHECK( szt4.get(2) == 3.0 );
  CHECK( szt4.get(3) == 4.0 );
  CHECK( szt4.get(4) == 5.0 );
  CHECK( szt4.get(6) == std::numeric_limits<size_t>::max() );
  CHECK( psmrts::OptionStringsExtractor( option_j4 ).get() == std::string( text4 ) ); 

  const bool GetAll = true;
  std::vector<size_t> partial = psmrts::OptionSizetsExtractor( option_j4 ).get_all( 2, 2 );

  char text5[] = R"({"array":[1.0,"2",3,4,"5","null"]})";
  psmrts::ProductOption option_j5( "array", psmrts::json_utils::parse_json_string( text5 ) );
  psmrts::OptionSizetsExtractor szt5( option_j5 );
  CHECK( szt5.get()  == 1 );
  CHECK( szt5.get(1) == 2 );
  CHECK( szt5.get(2) == 3 );
  CHECK( szt5.get(3) == 4 );
  CHECK( szt5.get(4) == 5 );
  CHECK( szt5.get(6) == std::numeric_limits<size_t>::max() );
  CHECK( psmrts::OptionStringsExtractor( option_j5 ).get() == std::string( text5 ) ); 

  ordered_json text6 = "2.0";
  psmrts::ProductOption option_j6( "array", psmrts::json_utils::parse_json_string( text6 ) );
  psmrts::OptionSizetsExtractor szt6( option_j6 );
  CHECK( szt6.get() == 2 );  
  CHECK( szt6.get(1) == std::numeric_limits<size_t>::max() );  
}


