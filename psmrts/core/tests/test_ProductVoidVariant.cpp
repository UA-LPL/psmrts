#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/ProductVoidVariant.hpp>
#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/ProductOption.hpp>

TEST_CASE( "ProductVoidVariant Default Test", "[product][void][variant][default]") {
    psmrts::ProductVoidVariant v_var;

    CHECK( v_var.product_specifications().size() == 0 );
    CHECK( v_var.config().size() == 0 );
    CHECK( v_var.type() == "variant" );
    
    psmrts::ProductConfiguration config;

    CHECK( v_var.matches( config ) == false ); // always?

    psmrts::PsmrtsTranslations tln;

    CHECK_THROWS_WITH( v_var.create(config, tln), "PsmrtsVoidVariant is not a valid product!" ); 

    psmrts::ProductConfiguration config2( "void_option", { psmrts::ProductOption("test_key", "test_val") } );

    // This constructor calls create, which always fails
    CHECK_THROWS( psmrts::ProductVoidVariant( config2 ) );
}
