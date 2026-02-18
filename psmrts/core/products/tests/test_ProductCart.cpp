#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/products/ProductCart.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/core/products/ProductFeature.hpp>

TEST_CASE( "ProductCart Default Test", "[cart][default]") {
    psmrts::ProductCart cart;

    CHECK( cart.isvalid()              == true );
    CHECK( cart.has_valid_content()    == false );
    CHECK( cart.size()                 == 0 );
    CHECK( cart.name()                 == "none" );
    CHECK( cart.product()              == "none" );
    CHECK( cart.residual_size()        == 0 );
    CHECK( cart.configuration().size() == 0 );
    CHECK_NOTHROW( cart.set_configuration() );

    CHECK( cart.configuration().name() == "none" );
    CHECK( cart.configuration().size() == 0 );
    CHECK( cart.options().size()       == 0 );

    psmrts::ProductConfiguration config("test_config", { psmrts::ProductOption("tracer", "test_tracer"),
                                                         psmrts::ProductOption("file", "test_file") } );
                
    
    CHECK_NOTHROW( cart.set_configuration( config ) );
    CHECK( cart.configuration().name() == "test_config" );
    CHECK( cart.configuration().size() == 2 );

    CHECK( cart.options().size() == 2 );

    psmrts::ProductOption opt1("option1", "option_val");
    psmrts::ProductOption opt2("option2", "option_meta_val");

    cart.add_option( opt1 );
    CHECK( cart.options().size() == 3 );

    CHECK_NOTHROW( cart.add_metadata( opt2 ) );

    psmrts::ProductSpecification spec1;
    const char *c1 = "name=double;type=double;status=required";
    psmrts::ProductFeature dbl_feat(psmrts::ProductFeature::from_pvl( c1 ) );
    spec1.add_feature( dbl_feat );

    CHECK( cart.specification().size() == 0 );
    
    cart.set_specification( spec1 );
    CHECK( cart.specification().size() == 1 );

    CHECK( cart.residual().size() == cart.residual_size() );

    psmrts::ProductOption resd1("residual1", "residual_val");
    cart.add_residual( resd1 );
    CHECK( cart.residual_size() == 1 );

    psmrts::ProductConfiguration res_config = cart.residual_config();
    CHECK( res_config.name() == cart.configuration().name() );
    
    ordered_json j_cart = cart.to_json();
    CHECK( cart.to_json().size() == 3 );
}