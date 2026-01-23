#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/core/AllOptionConversions.hpp>
#include <psmrts/algorithms/conversions/IntegersVisitor.hpp>

TEST_CASE("IntegersVisitor Conversion Test", "[conversions][option][integers][visitor]") {

    psmrts::ProductOption option_double( "double", 42.0 );
    psmrts::OptionIntegersExtractor int_dbl( option_double );

    CHECK( int_dbl.name() == "double" );
    CHECK( int_dbl.size() == 1 );
    CHECK( int_dbl.get()  == 42 );
    CHECK( int_dbl.get(1) == std::numeric_limits<int>::min() );

    psmrts::ProductOption option_bool( "bool", true );
    psmrts::OptionIntegersExtractor int_bool( option_bool );

    CHECK( int_bool.name() == "bool" );
    CHECK( int_bool.size() == 1 );
    CHECK( int_bool.get()  == 1 );
    CHECK( int_bool.get(1) == std::numeric_limits<int>::min() );

    psmrts::ProductOption option_sizet( "sizet", static_cast<size_t>( 42 ) );
    psmrts::OptionIntegersExtractor int_sizet( option_sizet );
    CHECK( int_sizet.name() == "sizet" );
    CHECK( int_sizet.size() == 1 );
    CHECK( int_sizet.get()  == 42 );
    CHECK( int_sizet.get(1) == std::numeric_limits<int>::min() );

    psmrts::ProductOption option_str( "string", "42" );
    psmrts::OptionIntegersExtractor int_str( option_str );
    CHECK( int_str.name() == "string" );
    CHECK( int_str.size() == 1 );
    CHECK( int_str.get()  == 42 );
    CHECK( int_str.get(1) == std::numeric_limits<int>::min() );

    CHECK( psmrts::OptionIntegersComparator::compare( option_double, option_bool )  == false );
    CHECK( psmrts::OptionIntegersComparator::compare( option_double, option_sizet ) == true );
    CHECK( psmrts::OptionIntegersComparator::compare( option_sizet, option_str )    == true );

    psmrts::ProductOption option_array( "Double Array", std::vector<double>{0.0, 1.1, 2.2, 3.8, 4.4} );
    psmrts::OptionIntegersExtractor array_dbl( option_array );
    CHECK( array_dbl.name() == "double array" ); // Strings are coverted to lowercase
    CHECK( array_dbl.size() == 5 );
    CHECK( array_dbl.get()  == 0 );
    CHECK( array_dbl.get(1) == 1 );
    CHECK( array_dbl.get(2) == 2 );
    CHECK( array_dbl.get(3) == 3 );
    CHECK( array_dbl.get(4) == 4 );
    CHECK( array_dbl.get(5) == std::numeric_limits<int>::min() );

    psmrts::ProductOption option_array2( "InT ArRay", std::vector<int>{100, 200, 0} );
    psmrts::OptionIntegersExtractor array_ints( option_array2 );
    CHECK( array_ints.name() == "int array" ); // Strings are coverted to lowercase
    CHECK( array_ints.size() == 3 );
    CHECK( array_ints.get()  == 100 );
    CHECK( array_ints.get(1) == 200 );
    CHECK( array_ints.get(2) == 0 );
    CHECK( array_ints.get(3) == std::numeric_limits<int>::min() );

    psmrts::ProductOption option_array3( "SIZET ArRay", std::vector<size_t>{100, 200, 0} );
    psmrts::OptionIntegersExtractor array_sizet( option_array3 );
    CHECK( array_sizet.name() == "sizet array" ); // Strings are coverted to lowercase
    CHECK( array_sizet.size() == 3 );
    CHECK( array_sizet.get()  == 100 );
    CHECK( array_sizet.get(1) == 200 );
    CHECK( array_sizet.get(2) == 0 );
    CHECK( array_sizet.get(3) == std::numeric_limits<int>::min() );

    CHECK( psmrts::OptionIntegersComparator::compare( option_array2, option_array3 ) == true );
    CHECK( psmrts::OptionIntegersComparator::compare( option_array, option_array2 )  == false );

    psmrts::ProductOption option_array4( "StRiNG ArRay", std::vector<std::string>{"10", "4.2", "-42"} );
    psmrts::OptionIntegersExtractor array_str( option_array4 );
    CHECK( array_str.name() == "string array" ); // Strings are coverted to lowercase
    CHECK( array_str.size() == 3 );
    CHECK( array_str.get()  == 10 );
    CHECK( array_str.get(1) == 4 ); 
    CHECK( array_str.get(2) == -42 );
    CHECK( array_str.get(3) == std::numeric_limits<int>::min() );
}

TEST_CASE("IntegersVisitor JSON Conversion Test", "[conversions][option][integers][comparator][json]") {
    char text1[] = R"({"key1":360})";
    psmrts::ProductOption option_j1( "key1", psmrts::json_utils::parse_json_string( text1 ) );
    psmrts::OptionIntegersExtractor i1( option_j1 );

    CHECK( i1.name() == "key1" );
    CHECK( i1.size() == 1 );
    CHECK( i1.get() == 360 );

    char text2[] = R"({"key2":3.14159154})";
    psmrts::ProductOption option_j2("key2", psmrts::json_utils::parse_json_string( text2 ) );
    psmrts::OptionIntegersExtractor i2( option_j2 ); 

    CHECK( i2.name() == "key2" );
    CHECK( i2.size() == 1 );
    CHECK( i2.get()  == 3 );

    char text3[] = R"({"array":[5.0, 4.5, 3.2, 2.0, 1.9]})";
    psmrts::ProductOption option_j3( "array", psmrts::json_utils::parse_json_string( text3 ) );
    psmrts::OptionIntegersExtractor i3( option_j3 );

    CHECK( i3.name() == "array" );
    CHECK( i3.size() == 1 ); // does whole array count as 1?
    CHECK( i3.get(0) == 5 );
    CHECK( i3.get(1) == 4 );
    CHECK( i3.get(2) == 3 );
    CHECK( i3.get(3) == 2 );
    CHECK( i3.get(4) == 1 );

    std::vector<int> partial = psmrts::OptionIntegersExtractor( option_j3 ).get_all( 2, 2 );
    CHECK( partial.size() == 1 ); // Should be 2?
    CHECK( partial[0]     == 2 ); // should be 3?
    // CHECK( partial[1]     == 0 ); 

    char text4[] = R"({"array":[1.0, "2", 3.4, 4, "5", "null"]})";
    psmrts::ProductOption option_j4("array", psmrts::json_utils::parse_json_string( text4 ) );
    psmrts::OptionIntegersExtractor d4( option_j4 );
    CHECK( d4.get()  == 1.0 );
    CHECK( d4.get(1) == 2.0 );
    CHECK( d4.get(2) == 3.0 );
    CHECK( d4.get(3) == 4.0 );
    CHECK( d4.get(4) == 5.0 );
    CHECK( d4.get(5) == std::numeric_limits<int>::min() ); 

    ordered_json text5 = "2";
    psmrts::ProductOption option_j5("array", text5 );
    psmrts::OptionIntegersExtractor d5( option_j5 );
    CHECK( d5.name() == "array" );
    CHECK( d5.size() == 1 );
    CHECK( d5.get()  == 2 );
}
