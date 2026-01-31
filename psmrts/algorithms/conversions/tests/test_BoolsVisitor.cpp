#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/core/AllOptionConversions.hpp>
#include <psmrts/algorithms/conversions/BoolsVisitor.hpp>

TEST_CASE("BoolsVisitor Conversion Test", "[conversions][option][bools][visitor]"){
    psmrts::ProductOption option_int( "integer", 1 );
    psmrts::OptionBoolsExtractor string_int( option_int );

    CHECK( string_int.name() == "integer" );
    CHECK( string_int.size() == 1 );
    CHECK( string_int.get()  == true );
    CHECK( string_int.get(1) == false );
    CHECK( string_int.visitor_default() == false );
    CHECK( string_int.default_value()   == string_int.visitor_default() );

    psmrts::ProductOption option_string( "string", "true" );
    psmrts::OptionBoolsExtractor string_val( option_string );

    CHECK( string_val.name() == "string" );
    CHECK( string_val.size() == 1 );
    CHECK( string_val.get()  == true );
    CHECK( string_val.get(1) == false );

    CHECK( psmrts::OptionBoolsComparator::compare( option_int, option_string ) );

    psmrts::ProductOption option_sizet_true( "size_t", 1 );
    psmrts::OptionBoolsExtractor bools_st( option_sizet_true );

    CHECK( bools_st.name() == "size_t" );
    CHECK( bools_st.get()  == true );

    psmrts::ProductOption option_sizet_false( "bad_size_t", 0 );
    psmrts::OptionBoolsExtractor bools_st2( option_sizet_false );

    CHECK( bools_st2.name() == "bad_size_t" );
    CHECK( bools_st2.get()  == false );

    CHECK( psmrts::OptionBoolsComparator::compare( option_sizet_true, option_sizet_false ) == false );

    psmrts::ProductOption option_double_true( "double", 3.0 );
    psmrts::OptionBoolsExtractor bools_dbl_true( option_double_true );

    psmrts::ProductOption option_double_false( "double", 0.0 );
    psmrts::OptionBoolsExtractor bools_dbl_false( option_double_false );

    CHECK( bools_dbl_true.size()  == 1 );
    CHECK( bools_dbl_true.get()   == true );

    CHECK( bools_dbl_false.size() == 1 );
    CHECK( bools_dbl_false.get()  == false );

    psmrts::ProductOption option_array( "Double Array", std::vector<double>{0.0, 1.1, 2.2, 3.3, -4.4} );
    psmrts::OptionBoolsExtractor array_dbl( option_array );
    CHECK( array_dbl.name() == "double array" );
    CHECK( array_dbl.size() == 5 );
    CHECK( array_dbl.get()  == false );
    CHECK( array_dbl.get(1) == true  );
    CHECK( array_dbl.get(2) == true  );
    CHECK( array_dbl.get(3) == true  );
    CHECK( array_dbl.get(4) == true  );
    CHECK( array_dbl.get(5) == false );

    psmrts::ProductOption option_array2( "InT ArRay", std::vector<int>{100, 200, 0} );
    psmrts::OptionBoolsExtractor array_ints( option_array2 );
    CHECK( array_ints.name() == "int array" ); 
    CHECK( array_ints.size() == 3 );
    CHECK( array_ints.get()  == true  );
    CHECK( array_ints.get(1) == true  );
    CHECK( array_ints.get(2) == false );
    CHECK( array_ints.get(3) == false );

    psmrts::ProductOption option_array3( "SIZET ArRay", std::vector<size_t>{100, 200, 0} );
    psmrts::OptionBoolsExtractor array_sizet( option_array3 );
    CHECK( array_sizet.name() == "sizet array" ); 
    CHECK( array_sizet.size() == 3 );
    CHECK( array_sizet.get()  == true  );
    CHECK( array_sizet.get(1) == true  );
    CHECK( array_sizet.get(2) == false );
    CHECK( array_sizet.get(3) == false );

    CHECK( psmrts::OptionBoolsComparator::compare( option_array2, option_array3 ) == true );
    CHECK( psmrts::OptionBoolsComparator::compare( option_array, option_array2 )  == false );

    psmrts::ProductOption option_array4( "StRiNG ArRay", std::vector<std::string>{"10", "0.0", "true", "false"} );
    psmrts::OptionBoolsExtractor array_str( option_array4 );
    CHECK( array_str.name() == "string array" ); // Strings are coverted to lowercase
    CHECK( array_str.size() == 4 );
    CHECK( array_str.get()  == false );
    CHECK( array_str.get(1) == false ); 
    CHECK( array_str.get(2) == true  );
    CHECK( array_str.get(3) == false );
    CHECK( array_str.get(4) == false );
}

TEST_CASE( "BoolsVisitor JSON Conversion Test", "[conversions][option][bools][comparator][json]") {
    char text1[] = R"({"key1":"true"})";
    psmrts::ProductOption option_j1( "key1", psmrts::json_utils::parse_json_string( text1 ) );
    psmrts::OptionBoolsExtractor b1( option_j1 );

    CHECK( b1.name() == "key1" );
    CHECK( b1.size() == 1 );
    CHECK( b1.get()  == true );
    CHECK( b1.get(1) == false );
    CHECK( psmrts::OptionBoolsExtractor( option_j1 ).get() == true );

    char text2[] = R"({"key2":"on"})";
    psmrts::ProductOption option_j2( "key2", psmrts::json_utils::parse_json_string( text2 ) );
    psmrts::OptionBoolsExtractor b2( option_j2 );

    CHECK( b2.name() == "key2" );
    CHECK( b2.size() == 1 );
    CHECK( b2.get()  == true ); 
    CHECK( b2.get(1) == false );
    CHECK( psmrts::OptionBoolsExtractor( option_j2 ).get() == true );

    char text3[] = R"({"key3":"off"})";
    psmrts::ProductOption option_j3( "key3", psmrts::json_utils::parse_json_string( text3 ) );
    psmrts::OptionBoolsExtractor b3( option_j3 );

    CHECK( b3.name() == "key3" );
    CHECK( b3.get() == false );

    char text4[] = R"({"array":[1, 1.0, "no", 0]})";
    psmrts::ProductOption option_j4( "array", psmrts::json_utils::parse_json_string( text4 ) );
    psmrts::OptionBoolsExtractor b4( option_j4 );
    CHECK( b4.size() == 4 );
    CHECK( b4.get(0) == true );
    CHECK( b4.get(1) == true );
    CHECK( b4.get(2) == false );
    CHECK( b4.get(3) == false );

    ordered_json text5 = "true";
    psmrts::ProductOption option_j5( "json", text5 );
    psmrts::OptionBoolsExtractor b5( option_j5 );
    CHECK( b5.name() == "json" );
    CHECK( b5.size() == 1 );
    CHECK( b5.get()  == true );
}
