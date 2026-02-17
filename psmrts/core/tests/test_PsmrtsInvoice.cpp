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

TEST_CASE( "PsmrtsInvoice Priority Tracer Test 1", "[product][invoice][prioritytracer]" ) {

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

  psmrts::PsmrtsPriorityTracer priority_t = invoice_t.get_priority_tracer( "priority_tracer1" );
  CHECK( priority_t.size() == 3 );

}
