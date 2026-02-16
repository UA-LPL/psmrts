#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/ProductSpecification.hpp>
#include <psmrts/core/ProductOrder.hpp>
#include <psmrts/core/ProductMaker.hpp>
#include <psmrts/core/ProductCart.hpp>
#include <psmrts/core/ProductProcessing.hpp>
#include <psmrts/core/PsmrtsInvoice.hpp>

#include <tuple>

TEST_CASE( "PsmrtsInvoice Bullet Tracer Research", "[product][maker][tracer][bullet][research]" ) {

  psmrts::PsmrtsTranslations trans_t = psmrts::PsmrtsTranslations::create();
  psmrts::ProductProcessing process_t( trans_t );
  psmrts::PsmrtsInvoice invoice_t( "invoice", trans_t );
  CHECK( invoice_t.size() == 0 );
#if 0
  psmrts::ProductConfiguration bullet_t("bulletmaker");
  bullet_t.add( psmrts::ProductOption( "shape", "obj" ) );
  bullet_t.add( psmrts::ProductOption( "obj_file", psmrts_shapes_path( "obj/data/bennu_20facets.obj")  ) );
  bullet_t.add( psmrts::ProductOption( "tracer", "bullet" ) );
  bullet_t.add( psmrts::ProductOption( "bullet_compression", true ) );

  CHECK( bullet_t.contains( "shape" ) == true );
  CHECK( bullet_t.contains( "obj_file" ) == true );
  CHECK( bullet_t.contains( "tracer" ) == true );

  psmrts::ProductOrder order_t = invoice_t.process_product( bullet_t );
  CHECK( order_t.isvalid() == true );
  // CHECK( order_t.to_json().dump(2) == "" );

  psmrts::ProductMaker<psmrts::PsmrtsShape> maker_s( "obj" );
  CHECK( maker_s.process_config( bullet_t, trans_t ) == true );
  CHECK( maker_s.isvalid() == true );
  CHECK( maker_s.product().isValid() == true );
  // CHECK( maker_s.to_json().dump() == "" );

  auto shape_specs_v  = psmrts::ProductMaker<psmrts::PsmrtsShape>().get_product_specs();
  auto tracer_specs_v = psmrts::ProductMaker<psmrts::PsmrtsTracer>().get_product_specs();

  for ( const auto &shape_s : shape_specs_v ) {
    psmrts::ProductCart cart_t( shape_s, bullet_t );    
    psmrts::ProductOrder shape_order = process_t.process_cart( cart_t  );
    CHECK( shape_s.name()                == "" );
    CHECK( shape_order.isvalid()         == true );
    CHECK( shape_order.size()            == 2);
    CHECK( shape_order.error_count()     == 0 );
    CHECK( shape_order.has_dependencies() == false );
    CHECK( shape_order.dependency_size() == 0 );
    CHECK( shape_order.has_named_dependency( "shape" ) == false );    
    CHECK( shape_order.config().size()   == bullet_t.size() );
    CHECK( shape_order.residual_size() == 0 );
    // CHECK( shape_order.to_json().dump(2) == "" );
  }

  for ( const auto &tracer_s : tracer_specs_v ) {
    psmrts::ProductCart cart_t( tracer_s, bullet_t );    
    psmrts::ProductOrder tracer_order = process_t.process_cart( cart_t  );
    CHECK( tracer_s.name()                 == "" );
    CHECK( tracer_order.isvalid()          == true );
    CHECK( tracer_order.size()             == 2);
    CHECK( tracer_order.error_count()      == 0 );
    CHECK( tracer_order.has_dependencies() == true );
    CHECK( tracer_order.dependency_size() == 2 );
    CHECK( tracer_order.has_named_dependency( "shape" ) == true );
    CHECK( tracer_order.config().size()    == bullet_t.size() );
    CHECK( tracer_order.residual_size()  == 0 );
    // CHECK( tracer_order.to_json().dump(2)   == "" );
  }

  psmrts::ProductOrder order_c = psmrts::PsmrtsInvoice("bullet", trans_t).process_product( bullet_t );
  CHECK( order_c.isvalid()          == true );
  CHECK( order_c.size()             == 2);
  CHECK( order_c.error_count()      == 0 );
  CHECK( order_c.has_dependencies() == true );
  CHECK( order_c.dependency_size() == 2 );
  CHECK( order_c.has_named_dependency( "shape" ) == true );
  CHECK( order_c.config().size()    == bullet_t.size() );
  CHECK( order_c.residual_size()  == 0 );
  // CHECK( order_c.to_json().dump(2)   == "" );


  psmrts::ProductConfiguration naifdsk_t("naifdskmaker");
  naifdsk_t.add( psmrts::ProductOption( "tracer", "naifdsk" ) );
  naifdsk_t.add( psmrts::ProductOption( "dsk_file", psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds")  ) );
  psmrts::ProductOrder order_d = psmrts::PsmrtsInvoice("tracer", trans_t).process_product( naifdsk_t );
  CHECK( order_d.isvalid()          == true );
  CHECK( order_d.size()             == 2);
  CHECK( order_d.error_count()      == 0 );
  CHECK( order_d.has_dependencies() == true );
  CHECK( order_d.dependency_size() == 1 );
  CHECK( order_d.has_named_dependency( "naifdsk" ) == true );
  CHECK( order_d.config().size()    == naifdsk_t.size() );
  CHECK( order_d.residual_size()  == 0 );
  // CHECK( order_d.to_json().dump(2)   == "" );

  psmrts::ProductConfiguration ellipsoid_t("ellipsoidkmaker");
  // ellipsoid_t.add( psmrts::ProductOption( "tracer", "ellipsoid" ) );
  // ellipsoid_t.add( psmrts::ProductOption( "radii", { 0.283065, 0.271215,
  // 0.249720 } ) );
  ellipsoid_t.add( psmrts::ProductOption( "tracer", "spheroid" ) );
  ellipsoid_t.add( psmrts::ProductOption( "radii", { 0.283065, 0.249720 } ) );  
  ellipsoid_t.add( psmrts::ProductOption( "name", "myspheroid" ) );  
  // ellipsoid_t.add( psmrts::ProductOption( "tracer", "sphere" ) );
  // ellipsoid_t.add( psmrts::ProductOption( "radii", 0.283065 )  );  
  psmrts::ProductOrder order_e = psmrts::PsmrtsInvoice("tracer", trans_t).process_product( ellipsoid_t );
  CHECK( order_e.isvalid()          == true );
  CHECK( order_e.size()             == 1);
  CHECK( order_e.error_count()      == 0 );
  CHECK( order_e.has_dependencies() == true );
  CHECK( order_e.dependency_size() == 1 );
  CHECK( order_e.has_named_dependency( "ellipsoid" ) == true );
  CHECK( order_e.config().size()    == ellipsoid_t.size() );
  CHECK( order_e.residual_size()  == 0 );
  // CHECK( order_e.to_json().dump(2)   == "" );
#endif
}
