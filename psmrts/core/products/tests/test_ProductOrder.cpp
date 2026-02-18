#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/core/products/ProductOrder.hpp>
#include <psmrts/core/products/ProductCart.hpp>

TEST_CASE( "ProductOrder Default Test", "[order][default]") {
    psmrts::ProductOrder po;

    CHECK( po.isvalid()          == false );
    CHECK( po.submitted().name() == "none" );
    CHECK( po.config().name()    == "none" );
    CHECK( po.residual().name()  == "residualoptions" ); 

    psmrts::ProductOption b("bool", true);
    psmrts::ProductOption i("integer", 42);
    psmrts::ProductOption f("float", 3.14); //is default double, named for testing purposes

    po.add_option( b );
    po.add_option( i );
    po.add_option( f );

    CHECK( po.config().contains( "bool" )    == true );
    CHECK( po.config().contains( "integer" ) == true );
    CHECK( po.config().contains( "float" )   == true );
    CHECK( po.config().contains( "double" )  == false );
    CHECK( po.config().options().size()      == 3 );
    
    // Config is no longer empty - so..
    // config != 0 && but specs.size() == 0, no residuals added yet, valid?
    CHECK( po.isvalid() == false );

    psmrts::ProductOption s( "string", "metadata string" );

    CHECK( po.config().metadata().size() == 0 );

    po.add_metadata( s );

    CHECK( po.config().metadata().size() == 1 );
    CHECK( po.config().metadata().contains("string") == true );

    psmrts::ProductOption i2("integer2", 360);

    po.add_residual( i2 );

    CHECK( po.isvalid() == false );
    CHECK( po.residual().size() == 1 );
    CHECK( po.residual().contains( "integer2" ) == true );

    psmrts::ProductConfiguration config( "multi", { psmrts::ProductOption( "bool2", false ),
                                                    psmrts::ProductOption( "extra", 1 ) } );

    //po.set_residual( config );

    //CHECK( po.residual().size() == 2 );
    //CHECK( po.residual().contains( "integer2" ) == false );
}
