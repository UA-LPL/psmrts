#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsUID.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsCache.hpp>
#include <psmrts/core/PsmrtsSharedCache.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/PsmrtsInventory.hpp>
#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/core/products/ProductCart.hpp>
#include <psmrts/core/products/ProductCart.hpp>
#include <psmrts/core/products/ProductOrder.hpp>
#include <psmrts/core/products/ProductProcessing.hpp>
#include <psmrts/core/PsmrtsFactory.hpp>
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

  psmrts::SharedShape obj_s( psmrts::make_shared_copy( psmrts::PsmrtsShape( obj_file_expanded) ) );
  REQUIRE( obj_s.get()    != nullptr );
  REQUIRE( obj_s->isValid() == true );

  psmrts::PsmrtsUID::UIDType uid_s = obj_s->uid();
  REQUIRE( psmrts::PsmrtsUID::is_valid_uid( obj_s->uid() ) == true );
  CHECK( psmrts::PsmrtsUID::is_valid_uid( uid_s ) == true );

  psmrts::PsmrtsInventory inventory_t( "inventory_1", trans_t );
  inventory_t.add( obj_s );


  psmrts::ProductConfiguration config_s( "search_test", { psmrts::ProductOption( "file", obj_file_expanded ) } );
  auto order_t = processor_t.process_order( config_s );
  CHECK( order_t->isvalid() == true );
  CHECK( order_t->size()    == 1 );
  auto cart_s = order_t->find( "shape" );
  REQUIRE( order_t.get() != nullptr );
  
  psmrts::SharedShape shape_opt = processor_t.search_shape_inventory( *cart_s, 
                                                                      *inventory_t.shapes() );

  CHECK( order_t->error_count()       == 0 );
  CHECK( order_t->errors_to_string()  == "" );
  CHECK( order_t->isvalid()           == true );
  CHECK( shape_opt.get()              != nullptr );
  CHECK( inventory_t.size_shapes()    == 1 );

  psmrts::PsmrtsFactory().liquidate();
}

TEST_CASE( "ProductProcessing Configuration", "[product][processing][config]") {

  using UIDType          = psmrts::PsmrtsProduct::UIDType;
  using ProductOrderList = psmrts::PsmrtsInvoice::ProductOrderList;

  psmrts::PsmrtsFactory().liquidate();
  psmrts::PsmrtsFactory factory_t;

  // Set up translation system
  psmrts::PsmrtsTranslations trans_t( "ISISTest" );
  trans_t.add_environment( "ISISDATA", psmrts_rootpath() );
  trans_t.add_parameter( "osirisrex", "$ISISDATA/psmrts/shapes" );

  psmrts::PsmrtsInventory inventory_t( "inventory_t", trans_t );

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
   
  psmrts::ProductProcessing processor_t( trans_t );
  
  ProductOrderList orders_t;
  for ( const psmrts::ProductConfiguration &c : { config_obj, config_dsk_0, config_dsk_1, 
                                                   ellipsoid_0, ellipsoid_1, ellipsoid_2 } ) {
    CHECK( c.isvalid() == true );
    auto set_t = processor_t.process_order( c );
    CHECK( set_t->name()                    == c.name() );
    CHECK( set_t->error_count()      == 0 );
    CHECK( set_t->errors_to_string() == "" );
    CHECK( set_t->isvalid()          == true );
    
    orders_t.push_back( set_t );
  }

  CHECK( orders_t.size() == 6 );

  CHECK( psmrts::PsmrtsFactory().shape_count()  == 0 );
  CHECK( psmrts::PsmrtsFactory().tracer_count() == 0 );

  psmrts::PsmrtsErrors errors;
  auto tracers = factory_t.process_order( orders_t, inventory_t, errors ); 

  for (const auto &order : orders_t ) {

    // CHECK( tracer->config().to_json().dump(-1) == "" );
    auto [ found, tracer_p, shape_p ] = processor_t.search_inventory( *order, 
                                                                      *inventory_t.tracers(),
                                                                      *inventory_t.shapes() );
    CHECK( found == true );
    REQUIRE( tracer_p.get() != nullptr );
    if ( tracer_p->specs().name() == "bullet" ) {
      CHECK( shape_p.get() != nullptr);
    }
    else {
      CHECK( shape_p.get() == nullptr);
    }
  }

  CHECK( factory_t.shape_count()     == 1 );
  CHECK( inventory_t.size_shapes()   == 1 );

  CHECK( factory_t.tracer_count()    == 5 );  
  CHECK( inventory_t.size_tracers()  == 5 );

  psmrts::PsmrtsFactory().liquidate();
}



TEST_CASE( "ProductProcessing Search Comparisons", "[product][processing][search][compare][dsk]") {

  using UIDType          = psmrts::PsmrtsProduct::UIDType;
  using ProductOrderList = psmrts::PsmrtsInvoice::ProductOrderList;

  psmrts::PsmrtsFactory().liquidate();
  psmrts::PsmrtsFactory factory_t;

  // Set up translation system
  psmrts::PsmrtsTranslations trans_t( "ISISTest" );
  trans_t.add_environment( "ISISDATA", psmrts_rootpath() );
  trans_t.add_parameter( "osirisrex", "$ISISDATA/psmrts/shapes" );

  psmrts::PsmrtsInventory inventory_t( "inventory_t", trans_t );

  // Set up shapes
  std::string dsk_file        = "$osirisrex/dsk/data/bennu_20facets.bds";

  psmrts::ProductConfiguration config_dsk_0( "process_config_dsk_0",
                                            { psmrts::ProductOption( "file", dsk_file ) } );
  
  psmrts::ProductConfiguration config_dsk_1( "process_config_dsk_1",
                                             { psmrts::ProductOption( "file", dsk_file ),
                                               psmrts::ProductOption( "tracer", "naifdsk" ) } );

                                                 
  psmrts::ProductProcessing processor_t( trans_t );
  ProductOrderList orders_t;
  for ( const psmrts::ProductConfiguration &c : { config_dsk_0, config_dsk_1 } ) {
    CHECK( c.isvalid() == true );
    auto set_t = processor_t.process_order( c );
    CHECK( set_t->name()             == c.name() );
    CHECK( set_t->error_count()      == 0 );
    CHECK( set_t->errors_to_string() == "" );
    CHECK( set_t->isvalid()          == true );
    
    orders_t.push_back( set_t );
  }

  CHECK( orders_t.size() == 2 );

  psmrts::PsmrtsErrors errors;
  auto tracers = factory_t.process_order( orders_t, inventory_t, errors );   

  CHECK( inventory_t.size_shapes()  == 0 );
  CHECK( inventory_t.size_tracers() == 1 );

  CHECK( factory_t.shape_count()     == 0 );
  CHECK( factory_t.tracer_count()    == 1 );  

  psmrts::PsmrtsFactory().liquidate();
}
