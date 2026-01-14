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
    CHECK( int_dbl.get() == 42 );
    CHECK( int_dbl.get(1) == std::numeric_limits<int>::min() );

    psmrts::ProductOption option_bool( "bool", true );
    psmrts::OptionIntegersExtractor int_bool( option_bool );

    CHECK( int_bool.name() == "bool" );
    CHECK( int_bool.size() == 1 );
    CHECK( int_bool.get() == 1 );
    CHECK( int_bool.get(1) == std::numeric_limits<int>::min() );
}

TEST_CASE("IntegersVisitor JSON Conversion Test", "[conversions][option][integers][comparator][json]") {
    char text1[] = R"({"key1":360})";
    psmrts::ProductOption option_j1( "key1", psmrts::json_utils::parse_json_string( text1 ) );
    psmrts::OptionIntegersExtractor i1( option_j1 );

    CHECK( i1.name() == "key1" );
    CHECK( i1.size() == 1 );
    CHECK( i1.get() == 360 );
}
