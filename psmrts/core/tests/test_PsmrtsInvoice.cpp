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

  psmrts::PsmrtsTranslations trans_t = psmrts::PsmrtsTranslations::create();
  psmrts::PsmrtsInvoice invoice_t( "invoice", trans_t );
  CHECK( invoice_t.size() == 0 );

  psmrts::ProductConfiguration bullet_t("bulletmaker1");
  bullet_t.add( psmrts::ProductOption( "shape", "obj" ) );
  bullet_t.add( psmrts::ProductOption( "obj_file", psmrts_shapes_path( "obj/data/bennu_20facets.obj")  ) );
  bullet_t.add( psmrts::ProductOption( "tracer", "bullet" ) );
  bullet_t.add( psmrts::ProductOption( "bullet_compression", true ) );

  CHECK( bullet_t.contains( "shape" ) == true );
  CHECK( bullet_t.contains( "obj_file" ) == true );
  CHECK( bullet_t.contains( "tracer" ) == true );

  CHECK( invoice_t.create_product( bullet_t ) == true );

  psmrts::ProductConfiguration naifdsk_t("naifdskmaker0");
  naifdsk_t.add( psmrts::ProductOption( "tracer", "naifdsk" ) );
  naifdsk_t.add( psmrts::ProductOption( "dsk_file", psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds")  ) );

  CHECK( invoice_t.create_product( naifdsk_t ) == true );

  psmrts::ProductConfiguration ellipsoid_t("ellipsoidmaker0");
  ellipsoid_t.add( psmrts::ProductOption( "tracer", "ellipsoid" ) );
  ellipsoid_t.add( psmrts::ProductOption( "radii", { 0.283065, 0.271215, 0.249720 } ) );

  CHECK( invoice_t.create_product( ellipsoid_t ) == true );

  CHECK( invoice_t.size() == 3);

  psmrts::PsmrtsPriorityTracer priority_t = invoice_t.make_priority_tracer( "priority_tracer1" );
  CHECK( invoice_t.error_count()                == 0 );
  CHECK( priority_t.size()                      == 3 );
  CHECK( invoice_t.inventory().size()           == 4 );
  CHECK( invoice_t.inventory().tracers().size() == 3 );
  CHECK( invoice_t.inventory().shapes().size()  == 1);

  psmrts::ProductConfiguration bullet_t2( "bulletmaker2" );
  bullet_t2.add( psmrts::ProductOption( "shape", "obj" ) );
  bullet_t2.add( psmrts::ProductOption( "obj_file", psmrts_shapes_path( "obj/data/bennu_20facets.obj")  ) );
  bullet_t2.add( psmrts::ProductOption( "tracer", "bullet" ) );
  bullet_t2.add( psmrts::ProductOption( "bullet_compression", false ) );

  CHECK( invoice_t.create_product( bullet_t2 )  == true );
  CHECK( invoice_t.size()                       == 4);
  CHECK( invoice_t.inventory().tracers().size() == 4 );
  CHECK( invoice_t.inventory().shapes().size()  == 1);


  psmrts::PsmrtsPriorityTracer priority_t2 = invoice_t.make_priority_tracer( "priority_tracer2" );
  CHECK( priority_t2.isValid() == true );
  CHECK( priority_t2.size()    == 4 );

  psmrts::ProductConfiguration bullet_t3( "bulletmaker3" );
  bullet_t3.add( psmrts::ProductOption( "shape", "obj" ) );
  bullet_t3.add( psmrts::ProductOption( "obj_file", psmrts_shapes_path( "obj/data/bennu_20facets.obj")  ) );
  bullet_t3.add( psmrts::ProductOption( "tracer", "bullet" ) );

  psmrts::ProductProcessing::ProductSet set_b3 = invoice_t.processor().process_configuration( bullet_t3 );
  CHECK( set_b3.tracer.size() != 0 );
  CHECK( set_b3.shape.size()  != 0 );

  CHECK( set_b3.tracer.error_count() == 0  );
  CHECK( set_b3.shape.error_count() == 0  );

  CHECK( set_b3.tracer.errors_to_string() == ""  );
  CHECK( set_b3.shape.errors_to_string() == ""  );
 
  CHECK( invoice_t.processor().search_inventory( set_b3, invoice_t.processor().inventory() ) == true );
  CHECK( invoice_t.processor().errors_to_string() == "" );
 
  CHECK( invoice_t.processor().has_valid_tracer( set_b3 ) == true );
  CHECK( invoice_t.processor().has_valid_shape( set_b3 ) == true );

  CHECK( set_b3.has_tracer() == true );
  CHECK( set_b3.has_shape()  == true );

  psmrts::ProductProcessing::ProductSet set_b1 = invoice_t.orders().find("bulletmaker1");
  CHECK( invoice_t.create_product( bullet_t3 )  == true );
  CHECK( invoice_t.size()                       == 5);
  CHECK( invoice_t.inventory().tracers().size() == 4 );
  CHECK( invoice_t.inventory().shapes().size()  == 1);
  
  psmrts::PsmrtsFactory().liquidate();

}
