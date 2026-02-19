#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/products/ProductVoidVariant.hpp>
#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductCart.hpp>

TEST_CASE( "ProductVoidVariant Default Test", "[product][void][variant][default]") {
    psmrts::ProductVoidVariant v_var;

    CHECK( v_var.product_specifications().size() == 0 );
    CHECK( v_var.config().size() == 0 );
    CHECK( v_var.type() == "variant" );
    
    psmrts::ProductConfiguration config;

    CHECK( v_var.matches( config ) == false ); // always?

    psmrts::PsmrtsTranslations tln;
    psmrts::ProductSpecification spec_v = psmrts::ProductVoidVariant::product_specifications();

    CHECK_THROWS_WITH( psmrts::ProductVoidVariant( psmrts::ProductCart( spec_v, config ) ), 
                       "PsmrtsVoidVariant is not a valid product!" ); 

    psmrts::ProductConfiguration config2( "void_option", { psmrts::ProductOption("test_key", "test_val") } );

    // This constructor calls create, which always fails
    CHECK_THROWS( psmrts::ProductVoidVariant( psmrts::ProductCart( spec_v, config2 )) );
}
