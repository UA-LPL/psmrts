#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductOrder.hpp>
#include <psmrts/core/products/ProductMaker.hpp>
#include <psmrts/core/products/ProductCart.hpp>
#include <psmrts/core/products/ProductProcessing.hpp>
#include <psmrts/core/PsmrtsInvoice.hpp>

#include <tuple>

TEST_CASE( "PsmrtsInvoice Priority Tracer Test 1", "[product][invoice][prioritytracer]" ) {

  using UIDType = psmrts::PsmrtsInventory::UIDType;

  psmrts::PsmrtsFactory().liquidate();
  psmrts::PsmrtsFactory factory;

  CHECK( factory.tracer_count()         == 0 );
  CHECK( factory.shape_count()          == 0 );

  psmrts::SharedTranslations trans_t = psmrts::make_shared_copy( psmrts::PsmrtsTranslations::create() );
  psmrts::PsmrtsInvoice invoice_t( "invoice", trans_t );
  CHECK( invoice_t.size() == 0 );

  psmrts::ProductConfiguration bullet_t("bulletmaker1");
  bullet_t.add( psmrts::ProductOption( "shape", "obj" ) );
  bullet_t.add( psmrts::ProductOption( "obj_file", psmrts_shapes_path( "obj/data/bennu_20facets.obj")  ) );
  bullet_t.add( psmrts::ProductOption( "tracer", "bullet" ) );
  bullet_t.add( psmrts::ProductOption( "bullet_compression", true ) );

  CHECK( bullet_t.contains( "shape" )    == true );
  CHECK( bullet_t.contains( "obj_file" ) == true );
  CHECK( bullet_t.contains( "tracer" )   == true );

  CHECK_NOTHROW( invoice_t.add( bullet_t ) );
  CHECK( invoice_t.size()           == 1 );
  CHECK( invoice_t.tracers().size() == 0 );
  CHECK( factory.tracer_count()     == 0 );
  CHECK( factory.shape_count()      == 0 );
  CHECK( factory.tracer_count()     == invoice_t.inventory().size_tracers() );
  CHECK( factory.shape_count()      == invoice_t.inventory().size_shapes() );

  psmrts::ProductConfiguration naifdsk_t("naifdskmaker0");
  naifdsk_t.add( psmrts::ProductOption( "tracer", "naifdsk" ) );
  naifdsk_t.add( psmrts::ProductOption( "dsk_file", psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds")  ) );

  CHECK_NOTHROW( invoice_t.add( naifdsk_t ) );
  CHECK( invoice_t.size()           == 2 );
  CHECK( invoice_t.tracers().size() == 0 );
  CHECK( factory.tracer_count()     == 0 );
  CHECK( factory.shape_count()      == 0 );
  CHECK( factory.tracer_count()     == invoice_t.inventory().size_tracers() );
  CHECK( factory.shape_count()      == invoice_t.inventory().size_shapes() );

  psmrts::ProductConfiguration ellipsoid_t("ellipsoidmaker0");
  ellipsoid_t.add( psmrts::ProductOption( "tracer", "ellipsoid" ) );
  ellipsoid_t.add( psmrts::ProductOption( "radii", { 0.283065, 0.271215, 0.249720 } ) );

  psmrts::ProductCart cart_e( "ellipsoid");
  psmrts::ProductOrder order_t ( ellipsoid_t, trans_t );
  auto processor = psmrts::ProductProcessing( trans_t );
  auto order     =  processor.process_order( ellipsoid_t );

  auto ellipsoid_tracer = psmrts::EllipsoidTracer( *order->find("tracer") );
  CHECK( ellipsoid_tracer.a() == 0.283065);
  CHECK( ellipsoid_tracer.b() == 0.271215 );
  CHECK( ellipsoid_tracer.c() == 0.249720 );

  psmrts::PsmrtsInventory inventory_t( "ellipsoid", trans_t );
  psmrts::PsmrtsErrors errors;

  auto evalues = psmrts::PsmrtsFactory().process_order( order, inventory_t, errors );
  CHECK( evalues.size()             == 1 );
  CHECK( factory.tracer_count()     == 1 );
  CHECK( factory.shape_count()      == 0 );
  CHECK( factory.tracer_count()     == inventory_t.size_tracers() );
  CHECK( factory.shape_count()      == inventory_t.size_shapes() );  

  CHECK_NOTHROW( invoice_t.add( ellipsoid_t ) );

  CHECK( invoice_t.error_count()      == 0 );
  CHECK( invoice_t.errors_to_string() == "" );

  CHECK( invoice_t.isvalid()        == false );
  CHECK( invoice_t.size()           == 3 );
  CHECK( invoice_t.tracers().size() == 0 );
  CHECK( factory.tracer_count()     == 1 );
  CHECK( factory.shape_count()      == 0 );

  psmrts::PsmrtsPriorityTracer priority_t = invoice_t.make_priority_tracer( "priority_tracer1" );
  CHECK( invoice_t.error_count()       == 0 );
  CHECK( invoice_t.errors_to_string()  == "" );
  CHECK( invoice_t.size()           == 3 );
  CHECK( invoice_t.tracers().size() == 3 );
  CHECK( factory.tracer_count()     == 3 );
  CHECK( factory.shape_count()      == 1 );
  CHECK( invoice_t.inventory().size_tracers() == 3 );
  CHECK( invoice_t.inventory().size_shapes()  == 1);  

  psmrts::ProductConfiguration bullet_t2( "bulletmaker2" );
  bullet_t2.add( psmrts::ProductOption( "shape", "obj" ) );
  bullet_t2.add( psmrts::ProductOption( "obj_file", psmrts_shapes_path( "obj/data/bennu_20facets.obj")  ) );
  bullet_t2.add( psmrts::ProductOption( "tracer", "bullet" ) );
  bullet_t2.add( psmrts::ProductOption( "bullet_compression", false ) );

  CHECK_NOTHROW( invoice_t.add( bullet_t2 ) );
  CHECK( invoice_t.error_count() == 0 );
  CHECK( invoice_t.errors_to_string() == "" );  
  CHECK( invoice_t.size()                       == 4);
  CHECK( invoice_t.tracers().size()             == 3);
  CHECK( invoice_t.isvalid()                    == false );
  CHECK( invoice_t.inventory().tracers()->size() == 3 );
  CHECK( invoice_t.inventory().shapes()->size()  == 1);

  auto orders = invoice_t.orders();
  CHECK( orders.size()              == 4 );
  CHECK( invoice_t.size()           == 4 );
  CHECK( invoice_t.tracers().size() == 3 );

  psmrts::PsmrtsPriorityTracer priority_t2 = invoice_t.make_priority_tracer( "priority_tracer2" );
  CHECK( priority_t2.isValid()                   == true );
  CHECK( priority_t2.size()                      == 4 );
  CHECK( invoice_t.inventory().tracers()->size() == 4 );

  auto tracers = invoice_t.tracers();
  CHECK( tracers.size() == 4 );
  CHECK( psmrts::PsmrtsFactory().tracer_count() == 4 );
  CHECK( tracers[0].get() != nullptr );

  psmrts::ProductConfiguration bullet_t3( "bulletmaker3" );
  bullet_t3.add( psmrts::ProductOption( "shape", "obj" ) );
  bullet_t3.add( psmrts::ProductOption( "obj_file", psmrts_shapes_path( "obj/data/bennu_20facets.obj")  ) );
  bullet_t3.add( psmrts::ProductOption( "tracer", "bullet" ) );

  auto set_b3 = invoice_t.processor().process_order( bullet_t3 );
  REQUIRE( set_b3.get() != nullptr );
  CHECK( set_b3->errors_to_string()           == "" );
  CHECK( set_b3->cart_keys() == std::vector<std::string>( { "shape", "tracer" } ) );
  CHECK( set_b3->size() == 2 );
  auto shape_t = set_b3->find( "shape" );
  REQUIRE( shape_t.get() != nullptr );

  CHECK( shape_t->product() == "shape" );
  CHECK( shape_t->residual_size()  == 0 );

  auto tracer_t = set_b3->find( "tracer" );
  CHECK( set_b3->errors_to_string() == ""  );
  REQUIRE( tracer_t.get()    != nullptr );
  CHECK( tracer_t->product() == "tracer" );
  CHECK( tracer_t->name()    == "bullet" );

 
  CHECK_NOTHROW( invoice_t.add( bullet_t3 ) );
  CHECK( invoice_t.size()                        == 5);
  CHECK( invoice_t.isvalid()                     == false );

  invoice_t.submit_order();
  auto priority_t4 = invoice_t.make_priority_tracer();

  CHECK( invoice_t.size()                        == 5);
  CHECK( invoice_t.inventory().tracers()->size() == 4 );
  CHECK( invoice_t.inventory().shapes()->size()  == 1 );
  CHECK( psmrts::PsmrtsFactory().tracer_count()  == 4 );
  CHECK( psmrts::PsmrtsFactory().shape_count()   == 1 );
  CHECK( priority_t4.size()                      == 4 );

  psmrts::PsmrtsFactory().liquidate();

}
