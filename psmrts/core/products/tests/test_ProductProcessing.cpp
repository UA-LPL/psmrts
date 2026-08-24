#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsUID.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsFactory.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/products/ProductProcessing.hpp>
#include <psmrts/core/PsmrtsInvoice.hpp>


TEST_CASE( "ProductProcessing Test Search", "[product][processing][shape][search]") {
  using UIDType         = psmrts::PsmrtsProduct::UIDType;

  // Set up translation system
  psmrts::PsmrtsFactory().liquidate();

  psmrts::PsmrtsTranslations trans_t( "ISISTest" );
  trans_t.add_environment( "ISISDATA", psmrts_rootpath() );
  trans_t.add_parameter( "osirisrex", "$ISISDATA/psmrts/shapes" );

  // Set up shapes
  std::string obj_file        = "$osirisrex/obj/data/bennu_20facets.obj";  
  
  psmrts::ProductProcessing processor_t( trans_t );
  std::string obj_file_expanded = processor_t.translate_path( obj_file );
  psmrts::PsmrtsShape obj_s( obj_file_expanded );
  REQUIRE( obj_s.isValid() == true );
  CHECK_NOTHROW( processor_t.cache_shape( obj_s ) );
  psmrts::PsmrtsUID::UIDType uid_s = obj_s.uid();
  REQUIRE( psmrts::PsmrtsUID::is_valid_uid( obj_s.uid() ) == true );

  // CHECK( psmrts::PsmrtsUID::is_valid_uid( uid_s ) == true );
  CHECK( processor_t.shapes().size()              == 1 );
  CHECK( processor_t.shapes().contains( uid_s )   == true );

  psmrts::PsmrtsShape shape_inv = processor_t.shapes().find( uid_s );
  CHECK( shape_inv.uid() == uid_s );

  psmrts::ProductConfiguration config_s( "search_test", { psmrts::ProductOption( "file", obj_file_expanded ) } );
  std::optional<psmrts::PsmrtsShape> shape_opt;

 psmrts::ProductOrder order_t = processor_t.search_shape_inventory( config_s, 
                                                        processor_t.shapes(), 
                                                        shape_opt );

  CHECK( order_t.error_count()          == 0 );
  CHECK( order_t.errors_to_string()     == "" );
  CHECK( order_t.isvalid()              == true );
  CHECK( shape_opt.has_value()          == true );
  CHECK( order_t.cart().get_shape_uid() == uid_s );
  CHECK( order_t.cart().get_shape_uid() == shape_opt.value().uid() );
  CHECK( shape_inv.uid() == shape_opt.value().uid() );

  CHECK( processor_t.shapes().size()                == 1 );
  CHECK( psmrts::PsmrtsFactory().shapes().size()    == 1 );

  psmrts::PsmrtsFactory().remove_shape( uid_s );
  CHECK( processor_t.shapes().size() == 0 );
  psmrts::PsmrtsFactory().liquidate();
}

TEST_CASE( "ProductProcessing Configuration", "[product][processing][config]") {

  using UIDType          = psmrts::PsmrtsProduct::UIDType;
  using ProductOrderList = psmrts::PsmrtsInvoice::ProductOrderList;

  psmrts::PsmrtsFactory().liquidate();

  // Set up translation system
  psmrts::PsmrtsTranslations trans_t( "ISISTest" );
  trans_t.add_environment( "ISISDATA", psmrts_rootpath() );
  trans_t.add_parameter( "osirisrex", "$ISISDATA/psmrts/shapes" );

  // Set up shapes
  std::string obj_file        = "$osirisrex/obj/data/bennu_20facets.obj";
  std::string dsk_file        = "$osirisrex/dsk/data/bennu_20facets.bds";
  psmrts::ProductConfiguration config_obj( "process_config_obj",
                                         { psmrts::ProductOption( "file", obj_file ),
                                           psmrts::ProductOption( "tracer", "bullet" ) } );
  psmrts::ProductConfiguration config_dsk_0( "process_config_dsk_0",
                                            { psmrts::ProductOption( "file", dsk_file ) } );
  
  psmrts::ProductConfiguration config_dsk_1( "process_config_dsk_1",
                                             { psmrts::ProductOption( "file", dsk_file ),
                                               psmrts::ProductOption( "tracer", "naifdsk" ) } );

  psmrts::ProductConfiguration ellipsoid_0( "process_config_ellipsoid_0",
                                             { psmrts::ProductOption( "radii", { 1.0, 1.0, 1.0 } ),
                                               psmrts::ProductOption( "tracer", "ellipsoid" ) } );
  psmrts::ProductConfiguration ellipsoid_1( "process_config_ellipsoid_1",
                                             { psmrts::ProductOption( "radii", { 10.0, 20.0  } ),
                                               psmrts::ProductOption( "tracer", "spheroid" ) } );
  psmrts::ProductConfiguration ellipsoid_2( "process_config_ellipsoid_2",
                                             { psmrts::ProductOption( "radii",  200.0 ),
                                               psmrts::ProductOption( "tracer", "sphere" ) } );
   
  psmrts::ProductProcessing processor_t( psmrts::make_shared_copy( trans_t ) );
  
  ProductOrderList orders_t;
  for ( const psmrts::ProductConfiguration &c : { config_obj, config_dsk_0, config_dsk_1, 
                                                   ellipsoid_0, ellipsoid_1, ellipsoid_2 } ) {
    CHECK( c.isvalid() == true );
    auto set_t = processor_t.process_order( c );
    CHECK( set_t->name()                    == c.name() );
    CHECK( set_t->tracer.error_count()      == 0 );
    CHECK( set_t.tracer.errors_to_string() == "" );
    CHECK( set_t.tracer.isvalid()          == true );
    CHECK( set_t.shape.name()              == set_t.shape.specs().name() );
    CHECK( set_t.shape.error_count()       == 0 );
    CHECK( set_t.shape.errors_to_string()  == "" );
    
    orders_t.add( set_t );
  }

  CHECK( orders_t.size()                          == 6 );
  CHECK( psmrts::PsmrtsFactory().shapes().size()  == 0 );
  CHECK( psmrts::PsmrtsFactory().tracers().size() == 0 );

  // Now create tracers
  psmrts::PsmrtsInventory inventory_t( "process_inventory" );

  processor_t.clear_errors();
  for ( ProductSet &set_p : orders_t.data() ) {
    // CHECK( set_p.tracer.to_json().dump(-1) == "" );
    CHECK( processor_t.process_product_set( set_p ) == true );
    CHECK( set_p.has_tracer(  ) == true );
    // CHECK( set_p.tracer_p.value().name() == set_p.tracer.config().name() );
    if ( set_p.tracer.specs().name() == "bullet" ) {
      CHECK( set_p.has_shape(  )  == true );
    }
    else {
      CHECK( set_p.has_shape(  )  == false );
    }
    CHECK( processor_t.error_count()      == 0 );
    CHECK( processor_t.errors_to_string() == "" );
    processor_t.clear_errors();
  }

  CHECK( psmrts::PsmrtsFactory().shapes().size()   == 1 );
  CHECK( processor_t.shapes().size()               == 1 );
  CHECK( psmrts::PsmrtsFactory().tracers().size()  == 5 );  
  CHECK( processor_t.tracers().size()              == 5 );
  psmrts::PsmrtsFactory().liquidate();
}



TEST_CASE( "ProductProcessing Search Comparisons", "[product][processing][search][compare][dsk]") {

  using UIDType          = psmrts::PsmrtsProduct::UIDType;
  using ProductOrderList = psmrts::PsmrtsInvoice::ProductOrderList;

  psmrts::PsmrtsFactory().liquidate();

  // Set up translation system
  psmrts::PsmrtsTranslations trans_t( "ISISTest" );
  trans_t.add_environment( "ISISDATA", psmrts_rootpath() );
  trans_t.add_parameter( "osirisrex", "$ISISDATA/psmrts/shapes" );

  // Set up shapes
  std::string dsk_file        = "$osirisrex/dsk/data/bennu_20facets.bds";

  psmrts::ProductConfiguration config_dsk_0( "process_config_dsk_0",
                                            { psmrts::ProductOption( "file", dsk_file ) } );
  
  psmrts::ProductConfiguration config_dsk_1( "process_config_dsk_1",
                                             { psmrts::ProductOption( "file", dsk_file ),
                                               psmrts::ProductOption( "tracer", "naifdsk" ) } );

                                                 
  psmrts::ProductProcessing processor_t( psmrts::make_shared_copy( trans_t ));
  auto set_0 = processor_t.process_order( config_dsk_0 );
  //-->> THIS NEEDS SOME TESTS!
  // CHECK( tracer_order.residual_size() == 0 );
  // CHECK( shape_order.residual_size()  == 0 );

  psmrts::PsmrtsFactory().liquidate();
}
