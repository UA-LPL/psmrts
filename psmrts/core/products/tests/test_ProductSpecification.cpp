#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <string>
#include <memory>
#include <iostream>
#include <string>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/products/ProductFeature.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductOrder.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductCart.hpp>
#include <psmrts/core/products/ProductProcessing.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>


TEST_CASE ( "ProductSpecification Constructor / Base Function Test", "[product][specification][constructor][base]") {
    psmrts::ProductSpecification product1;

    CHECK( product1.name() == "none" );
    
    // No constructor to create based on ProductFeature, seemingly must use json
    CHECK_THROWS( psmrts::ProductFeature( product1.to_json() ) );

    ordered_json result;
    result["name"] = "feature";
    psmrts::ProductFeature prodspecs( result );   

    CHECK( prodspecs.size()               == 1      );
    CHECK( prodspecs.contains("Required") == false  );
    CHECK( prodspecs.specs()         == result );

    
    CHECK_NOTHROW( product1.required() );
    CHECK_NOTHROW( product1.optional() );

    CHECK( product1.required().size() == 0 );
    CHECK( product1.optional().size() == 0 );
    const char *c1 = "name=double;type=double;status=required";
    const char *c2 = "name=boolean;type=bool;status=required";
    const char *c3 = "name=integer;type=int;status=required";
    //const char *c4 = "name=double2;type=double;status=optional";
    psmrts::ProductFeature dbl_feat(psmrts::ProductFeature::from_pvl( c1 ) );
    psmrts::ProductFeature bool_feat(psmrts::ProductFeature::from_pvl( c2 ) );
    psmrts::ProductFeature int_feat(psmrts::ProductFeature::from_pvl( c3 ) );
    //psmrts::ProductFeature dbl2_feat(psmrts::ProductFeature::from_pvl( c4 ) );
    CHECK_NOTHROW( product1.add_feature( dbl_feat ) );
    CHECK_NOTHROW( product1.add_feature( bool_feat ) );
    CHECK_NOTHROW( product1.add_feature( int_feat ) );
    //CHECK_NOTHROW( product1.add_feature( dbl2_feat ) );

    psmrts::ProductConfiguration config1;
    config1.add_option(psmrts::ProductOption("double", 3.14));
    config1.add_option(psmrts::ProductOption("boolean", true));
    config1.add_option(psmrts::ProductOption("integer", 42));
    config1.add_option(psmrts::ProductOption("double2", 6.28));

    psmrts::PsmrtsTranslations tls;

}

TEST_CASE( "ProductSpecification Configuration Test", "[product][specification][configuration]") {

  auto trans_t = psmrts::make_shared_copy( psmrts::PsmrtsTranslations::create() );
  trans_t->add_parameter("objdir", "obj/data");

  std::string objfile = psmrts_shapes_path( trans_t->translate_path( "$objdir/bennu_20facets.obj" ) );
  psmrts::PsmrtsShape shape_t( objfile );
  psmrts::ProductSpecification specs_t = shape_t.specs();
  psmrts::ProductConfiguration config_t = shape_t.config();

  psmrts::ProductCart cart_t( specs_t, config_t );
  CHECK( config_t.size() == 3 );

  psmrts::ProductProcessing process_t( trans_t );
  psmrts::PsmrtsErrors errors;

  CHECK( process_t.compare_product_config( config_t, cart_t, errors ) == true );
  CHECK( errors.errors_to_string() == "" );
  CHECK( process_t.process_cart( config_t, cart_t ) == true );
  CHECK( cart_t.error_count() == 0 );
  CHECK( cart_t.errors_to_string() == "" );

  auto order_t = process_t.process_order( config_t );
  CHECK( order_t->isvalid()          == true );
  CHECK( order_t->error_count()      == 0 );
  CHECK( order_t->errors_to_string() == "" );
  CHECK( order_t->config().size()    == 3 );
  REQUIRE( order_t->find( "shape") != nullptr );
  CHECK( order_t->find( "shape")->residual().size()  == 0 );

  trans_t->add_environment("PLYDIR", "ply/data");
  std::string plyfile = psmrts_shapes_path( "$PLYDIR/Bennu_Radar.ply" );
  psmrts::PsmrtsShape shape_p( trans_t->translate_path( plyfile ) );
  psmrts::ProductSpecification specs_p  = shape_p.specs();
  psmrts::ProductConfiguration config_p = shape_p.config();
  psmrts::ProductCart cart_p( specs_p, config_p );

  CHECK( config_p.size() == 3 );
  CHECK( process_t.compare_product_config( config_p, cart_p, errors ) == true );
  CHECK( errors.errors_to_string() == "" );
  CHECK( process_t.process_cart( config_p, cart_p ) == true );
  CHECK( cart_p.error_count() == 0 );
  CHECK( cart_p.errors_to_string() == "" );
  auto order_p = process_t.process_order( config_p );
  CHECK( order_p->isvalid()          == true );
  CHECK( order_p->error_count()      == 0 );
  CHECK( order_p->errors_to_string() == "" );
  CHECK( order_p->config().size()    == 3);
  REQUIRE( order_p->find( "shape") != nullptr );
  CHECK( order_p->find("shape")->residual().size()  == 0 );

  std::string dskfile = psmrts_shapes_path( "dsk/data/bennu_20facets.bds" );
  psmrts::PsmrtsShape shape_d(  dskfile );
  psmrts::ProductSpecification specs_d  = shape_d.specs();
  psmrts::ProductConfiguration config_d = shape_d.config();
  psmrts::ProductCart cart_d( specs_d, config_d );
  CHECK( config_d.size() == 2 );

  CHECK( process_t.compare_product_config( config_d, cart_d, errors ) == true );
  CHECK( errors.errors_to_string() == "" );
  CHECK( process_t.process_cart( config_d, cart_d ) == true );
  CHECK( cart_d.size()        == 2 );
  CHECK( cart_d.error_count() == 0 );
  CHECK( cart_d.errors_to_string() == "" );

  auto order_d = process_t.process_order( config_d );
  CHECK( order_d->isvalid()          == true );
  CHECK( order_d->size()             == 1 );
  CHECK( order_d->error_count()      == 0 );
  CHECK( order_d->errors_to_string() == "" );
  CHECK( order_d->config().size()    == 2 );
  CHECK( order_d->find( "shape") != nullptr );
  CHECK( order_d->find("shape")->residual().size()  == 0 );  
}


TEST_CASE( "ProductSpecification DSK Shape Configuration Test", "[product][specification][configuration][dsk]") {

  auto trans_t = psmrts::make_shared_copy( psmrts::PsmrtsTranslations::create() );
  trans_t->add_parameter("objdir", "obj/data");

  psmrts::ProductProcessing process_t( trans_t );
  psmrts::PsmrtsErrors errors;

  std::string dskfile = psmrts_shapes_path( "dsk/data/bennu_20facets.bds" );
  psmrts::PsmrtsShape shape_d(  dskfile );
  psmrts::ProductSpecification specs_d  = shape_d.specs();
  psmrts::ProductConfiguration config_d = shape_d.config();
  psmrts::ProductCart cart_d( specs_d, config_d );
  CHECK( config_d.size() == 2 );

  CHECK( process_t.compare_product_config( config_d, cart_d, errors ) == true );
  CHECK( errors.errors_to_string() == "" );
  psmrts::ProductCart cart_proc( specs_d );
  CHECK( process_t.process_cart( config_d, cart_proc ) == true );
  CHECK( cart_proc.size()          == 2 );
  CHECK( cart_proc.residual_size() == 0 );
  CHECK( cart_proc.error_count()   == 0 );
  CHECK( cart_proc.errors_to_string() == "" );

  auto order_d = process_t.process_order( config_d );
  CHECK( order_d->isvalid()          == true );
  CHECK( order_d->size()             == 1 );
  CHECK( order_d->error_count()      == 0 );
  CHECK( order_d->errors_to_string() == "" );
  CHECK( order_d->config().size()    == 2 );
  REQUIRE( order_d->find( "shape") != nullptr );
  CHECK( order_d->find("shape")->residual().size()  == 0 );  
  REQUIRE( order_d->find( "tracer") == nullptr );
}