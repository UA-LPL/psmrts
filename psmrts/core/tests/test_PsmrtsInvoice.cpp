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

  psmrts::PsmrtsTranslations trans_t = psmrts::PsmrtsTranslations::create();
  psmrts::PsmrtsInvoice invoice_t( "invoice", trans_t );
  CHECK( invoice_t.size() == 0 );

  psmrts::ProductConfiguration bullet_t("bulletmaker");
  bullet_t.add( psmrts::ProductOption( "shape", "obj" ) );
  bullet_t.add( psmrts::ProductOption( "obj_file", psmrts_shapes_path( "obj/data/bennu_20facets.obj")  ) );
  bullet_t.add( psmrts::ProductOption( "tracer", "bullet" ) );
  bullet_t.add( psmrts::ProductOption( "bullet_compression", true ) );

  CHECK( bullet_t.contains( "shape" ) == true );
  CHECK( bullet_t.contains( "obj_file" ) == true );
  CHECK( bullet_t.contains( "tracer" ) == true );

  CHECK( invoice_t.add_product( bullet_t ) == true );

  psmrts::ProductConfiguration naifdsk_t("naifdskmaker");
  naifdsk_t.add( psmrts::ProductOption( "tracer", "naifdsk" ) );
  naifdsk_t.add( psmrts::ProductOption( "dsk_file", psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds")  ) );

  CHECK( invoice_t.add_product( naifdsk_t ) == true );

  psmrts::ProductConfiguration ellipsoid_t("ellipsoidkmaker");
  // ellipsoid_t.add( psmrts::ProductOption( "tracer", "ellipsoid" ) );
  // ellipsoid_t.add( psmrts::ProductOption( "radii", { 0.283065, 0.271215, 0.249720 } ) );
  ellipsoid_t.add( psmrts::ProductOption( "tracer", "spheroid" ) );
  ellipsoid_t.add( psmrts::ProductOption( "radii", { 0.283065, 0.249720 } ) );  
  ellipsoid_t.add( psmrts::ProductOption( "name", "myspheroid" ) );  
  // ellipsoid_t.add( psmrts::ProductOption( "tracer", "sphere" ) );
  // ellipsoid_t.add( psmrts::ProductOption( "radii", 0.283065 )  ); 

  CHECK( invoice_t.add_product( ellipsoid_t ) == true );

  CHECK( invoice_t.size() == 3);

  psmrts::PsmrtsPriorityTracer priority_t = invoice_t.get_priority_tracer( "priority_tracer1" );
  CHECK( priority_t.size()                      == 3 );
  CHECK( invoice_t.inventory().size()           == 5 );
  CHECK( invoice_t.inventory().tracers().size() == 3 );
  CHECK( invoice_t.inventory().shapes().size()  == 1);

  // CHECK( invoice_t.inventory().shapes().keys()  == std::vector<UIDType>( { 1 } ) );
  // CHECK( invoice_t.inventory().tracers().keys() == std::vector<UIDType>( { 1, 2, 3 } ) );
  CHECK( invoice_t.inventory().prioritytracers().contains( priority_t.uid() ) == true );
  
  CHECK( invoice_t.has_priority_tracer( "priority_tracer1") == true);
  CHECK_NOTHROW( invoice_t.find_priority_tracer( "priority_tracer1") );
  CHECK_NOTHROW( invoice_t.inventory().prioritytracers().find( priority_t.uid() ) );

  psmrts::ProductConfiguration bullet_t2( "bulletmaker2" );
  bullet_t2.add( psmrts::ProductOption( "shape", "obj" ) );
  bullet_t2.add( psmrts::ProductOption( "obj_file", psmrts_shapes_path( "obj/data/bennu_20facets.obj")  ) );
  bullet_t2.add( psmrts::ProductOption( "tracer", "bullet" ) );
  bullet_t2.add( psmrts::ProductOption( "bullet_compression", false ) );

  CHECK( invoice_t.add_product( bullet_t2 ) == true );
  CHECK( invoice_t.size() == 4);

  CHECK( invoice_t.generate_products() == true );
  // CHECK( invoice_t.inventory().shapes().keys()  == std::vector<UIDType>( { 1 } ) );
  // CHECK( invoice_t.inventory().tracers().keys() == std::vector<UIDType>( { 1, 2, 3, 5 } ) );

  psmrts::PsmrtsPriorityTracer priority_t2 = invoice_t.get_priority_tracer( "priority_tracer2" );
  CHECK( priority_t2.isValid() == true );
  CHECK( priority_t2.size()    == 4 );

  CHECK( invoice_t.has_priority_tracer( "priority_tracer2") == true );
  CHECK_NOTHROW( invoice_t.find_priority_tracer( "priority_tracer2") );
  CHECK_NOTHROW( invoice_t.inventory().prioritytracers().find( priority_t2.uid() ) );  
  
}
