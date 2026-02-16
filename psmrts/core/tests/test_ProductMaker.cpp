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


#include <tuple>

TEST_CASE( "ProductMaker OBJ Shape Test", "[product][maker][shape][obj]" ) {

  psmrts::PsmrtsTranslations trans_t = psmrts::PsmrtsTranslations::create();
  psmrts::ProductProcessing processor_p(trans_t);

  psmrts::ProductConfiguration shape_p("objmaker");
  shape_p.add( psmrts::ProductOption( "shape", "obj" ) );
  shape_p.add( psmrts::ProductOption( "obj_file", psmrts_shapes_path( "obj/data/bennu_20facets.obj") ) );
  CHECK( shape_p.contains( "obj_file" ) == true );

  // CHECK( shape_p.to_json().dump(2) == "" );
  psmrts::ProductOrder order_s = processor_p.process_configuration( shape_p );
  psmrts::ProductMaker<psmrts::PsmrtsShape> maker_s( "obj" );

  CHECK( maker_s.process_config( order_s.config(), trans_t ) == true );
  CHECK( maker_s.isvalid() == true );
  psmrts::PsmrtsShape shape_m = maker_s.product();
  CHECK( shape_m.isValid() == true );
 //  CHECK( shape_m.config().to_json().dump() == "" );

}


TEST_CASE( "ProductMaker PLY Shape Test", "[product][maker][shape][ply]" ) {

  psmrts::PsmrtsTranslations trans_t = psmrts::PsmrtsTranslations::create();
  psmrts::ProductProcessing processor_p(trans_t);

  psmrts::ProductConfiguration shape_p("plymaker");
  shape_p.add( psmrts::ProductOption( "shape", "ply" ) );
  shape_p.add( psmrts::ProductOption( "ply_file", psmrts_shapes_path( "ply/data/Bennu_Radar.ply" ) ) );
  CHECK( shape_p.contains( "ply_file" ) == true );

  // CHECK( shape_p.to_json().dump(2) == "" );
  psmrts::ProductOrder order_s = processor_p.process_configuration( shape_p );
  psmrts::ProductMaker<psmrts::PsmrtsShape> maker_s( "ply" );

  CHECK( maker_s.process_config( order_s.config(), trans_t ) == true );
  CHECK( maker_s.isvalid() == true );
  psmrts::PsmrtsShape shape_m = maker_s.product();
  CHECK( shape_m.isValid() == true );
 //  CHECK( shape_m.config().to_json().dump() == "" );

}

TEST_CASE( "ProductMaker Bullet Tracer Test", "[product][maker][tracer][bullet]" ) {

  psmrts::PsmrtsTranslations trans_t = psmrts::PsmrtsTranslations::create();
  psmrts::ProductProcessing processor_p(trans_t);

  psmrts::ProductConfiguration bullet_t("bulletmaker");
  // bullet_t.add( psmrts::ProductOption( "shape", "obj" ) );
  bullet_t.add( psmrts::ProductOption( "obj_file", psmrts_shapes_path( "obj/data/bennu_20facets.obj")  ) );
  bullet_t.add( psmrts::ProductOption( "tracer", "bullet" ) );

  CHECK( bullet_t.contains( "obj_file" ) == true );

  // CHECK( bullet_t.to_json().dump(2) == "" );
  psmrts::ProductOrder order_t = processor_p.process_configuration( bullet_t );
  CHECK( order_t.error_count() == 0 );
  psmrts::ProductMaker<psmrts::PsmrtsTracer> maker_t( "bullet" );

  CHECK( maker_t.process_config( order_t.config(), trans_t ) == true );
  CHECK( maker_t.isvalid() == true );
  psmrts::PsmrtsTracer bullet_m = maker_t.product();
  CHECK( bullet_m.isValid() == true );
  // CHECK( bullet_m.config().to_json().dump() == "" );
}

#if 0
TEST_CASE( "ProductMaker Bullet Tracer Research", "[product][maker][tracer][bullet][research]" ) {

  psmrts::PsmrtsTranslations trans_t = psmrts::PsmrtsTranslations::create();
  psmrts::ProductConfiguration bullet_t("bulletmaker");
  bullet_t.add( psmrts::ProductOption( "shape", "obj" ) );
  bullet_t.add( psmrts::ProductOption( "obj_file", psmrts_shapes_path( "obj/data/bennu_20facets.obj")  ) );
  bullet_t.add( psmrts::ProductOption( "tracer", "bullet" ) );
  bullet_t.add( psmrts::ProductOption( "bullet_compression", true ) );

  CHECK( bullet_t.contains( "shape" ) == true );
  CHECK( bullet_t.contains( "obj_file" ) == true );
  CHECK( bullet_t.contains( "tracer" ) == true );

  psmrts::ProductMaker<psmrts::PsmrtsTracer> maker_t( "bullet" );
  CHECK( maker_t.process_config( bullet_t, trans_t ) == true );
  CHECK( maker_t.isvalid() == true );
  CHECK( maker_t.product().isValid() == true );
  CHECK( maker_t.order().config().to_json().dump(2) == "" );
  CHECK( maker_t.order().residual().to_json().dump(2) == "" );

  psmrts::ProductMaker<psmrts::PsmrtsShape> maker_s( "obj" );
  CHECK( maker_s.process_config( bullet_t, trans_t ) == true );
  CHECK( maker_s.isvalid() == true );
  CHECK( maker_s.product().isValid() == true );
  CHECK( maker_s.order().config().to_json().dump() == "" );
  CHECK( maker_s.order().residual().to_json().dump() == "" );

  auto shape_specs_v  = psmrts::ProductMaker<psmrts::PsmrtsShape>().get_product_specs();
  auto tracer_specs_v = psmrts::ProductMaker<psmrts::PsmrtsTracer>().get_product_specs();

  for ( const auto &shape_s : shape_specs_v ) {
    psmrts::ProductOrder shape_order = shape_s.process_order( bullet_t, trans_t );
    CHECK( shape_s.name()                == "" );
    CHECK( shape_order.isvalid()         == true );
    CHECK( shape_order.size()            == 2);
    CHECK( shape_order.error_count()     == 0 );
    CHECK( shape_order.has_dependencies() == false );
    CHECK( shape_order.dependency_size() == 0 );
    CHECK( shape_order.has_named_dependency( "shape" ) == false );    
    CHECK( shape_order.config().size()   == bullet_t.size() );
    CHECK( shape_order.residual_size() == 0 );
    CHECK( shape_order.to_json().dump(2) == "" );
  }

  for ( const auto &tracer_s : tracer_specs_v ) {
    psmrts::ProductOrder tracer_order = tracer_s.process_order( bullet_t, trans_t );
    CHECK( tracer_s.name()                 == "" );
    CHECK( tracer_order.isvalid()          == true );
    CHECK( tracer_order.size()             == 2);
    CHECK( tracer_order.error_count()      == 0 );
    CHECK( tracer_order.has_dependencies() == true );
    CHECK( tracer_order.dependency_size() == 2 );
    CHECK( tracer_order.has_named_dependency( "shape" ) == true );
    CHECK( tracer_order.config().size()    == bullet_t.size() );
    CHECK( tracer_order.residual_size()  == 0 );
    CHECK( tracer_order.config().to_json().dump(-1)   == "" );
    CHECK( tracer_order.residual().to_json().dump(-1) == "" );
  }

  psmrts::ProductOrder order_c = psmrts::ProductMaker<psmrts::PsmrtsTracer>::process_product( bullet_t, trans_t );
  CHECK( order_c.isvalid()          == true );
  CHECK( order_c.size()             == 2);
  CHECK( order_c.error_count()      == 0 );
  CHECK( order_c.has_dependencies() == true );
  CHECK( order_c.dependency_size() == 2 );
  CHECK( order_c.has_named_dependency( "shape" ) == true );
  CHECK( order_c.config().size()    == bullet_t.size() );
  CHECK( order_c.residual_size()  == 0 );
  CHECK( order_c.config().to_json().dump(-1)   == "" );
  CHECK( order_c.residual().to_json().dump(-1) == "" );


  psmrts::ProductConfiguration naifdsk_t("naifdskmaker");
  naifdsk_t.add( psmrts::ProductOption( "tracer", "naifdsk" ) );
  naifdsk_t.add( psmrts::ProductOption( "dsk_file", psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds")  ) );
  psmrts::ProductOrder order_d = psmrts::ProductMaker<psmrts::PsmrtsTracer>::process_product( naifdsk_t, trans_t );
  CHECK( order_d.isvalid()          == true );
  CHECK( order_d.size()             == 2);
  CHECK( order_d.error_count()      == 0 );
  CHECK( order_d.has_dependencies() == true );
  CHECK( order_d.dependency_size() == 1 );
  CHECK( order_d.has_named_dependency( "naifdsk" ) == true );
  CHECK( order_d.config().size()    == naifdsk_t.size() );
  CHECK( order_d.residual_size()  == 0 );
  CHECK( order_d.config().to_json().dump(-1)   == "" );
  CHECK( order_d.residual().to_json().dump(-1) == "" );

  psmrts::ProductConfiguration ellipsoid_t("ellipsoidkmaker");
  // ellipsoid_t.add( psmrts::ProductOption( "tracer", "ellipsoid" ) );
  // ellipsoid_t.add( psmrts::ProductOption( "radii", { 0.283065, 0.271215,
  // 0.249720 } ) );
  ellipsoid_t.add( psmrts::ProductOption( "tracer", "spheroid" ) );
  ellipsoid_t.add( psmrts::ProductOption( "radii", { 0.283065, 0.249720 } ) );  
  ellipsoid_t.add( psmrts::ProductOption( "name", "myspheroid" ) );  
  // ellipsoid_t.add( psmrts::ProductOption( "tracer", "sphere" ) );
  // ellipsoid_t.add( psmrts::ProductOption( "radii", 0.283065 )  );  
  psmrts::ProductOrder order_e = psmrts::ProductMaker<psmrts::PsmrtsTracer>::process_product( ellipsoid_t, trans_t );
  CHECK( order_e.isvalid()          == true );
  CHECK( order_e.size()             == 2);
  CHECK( order_e.error_count()      == 0 );
  CHECK( order_e.has_dependencies() == true );
  CHECK( order_e.dependency_size() == 1 );
  CHECK( order_e.has_named_dependency( "ellipsoid" ) == true );
  CHECK( order_e.config().size()    == ellipsoid_t.size() );
  CHECK( order_e.residual_size()  == 0 );
  CHECK( order_e.config().to_json().dump(-1)   == "" );
  CHECK( order_e.residual().to_json().dump(-1) == "" );  
}
#endif

TEST_CASE( "ProductMaker NaifDsk Tracer Test", "[product][maker][tracer][naifdsk]" ) {

  psmrts::PsmrtsTranslations trans_t = psmrts::PsmrtsTranslations::create();
  psmrts::ProductProcessing processor_p(trans_t);

  psmrts::ProductConfiguration naifdsk_t("naifdskmaker");
  naifdsk_t.add( psmrts::ProductOption( "tracer", "naifdsk" ) );
  naifdsk_t.add( psmrts::ProductOption( "dsk_file", psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds")  ) );

  CHECK( naifdsk_t.contains( "tracer" ) == true );
  CHECK( naifdsk_t.contains( "dsk_file" ) == true );

  // CHECK( naifdsk_t.to_json().dump(2) == "" );
  psmrts::ProductOrder order_t = processor_p.process_configuration( naifdsk_t );
  psmrts::ProductMaker<psmrts::PsmrtsTracer> maker_t( "naifdsk" );

  // CHECK( order_t.config().to_json().dump(2) == "" );
  CHECK( maker_t.process_config( order_t.config(), trans_t ) == true );
  CHECK( maker_t.isvalid() == true );
  psmrts::PsmrtsTracer naifdsk_m = maker_t.product();
  CHECK( naifdsk_m.isValid() == true );
  // CHECK( naifdsk_m.config().to_json().dump() == "" );
}


TEST_CASE( "ProductMaker Ellipsoid Tracer Test", "[product][maker][tracer][ellipsoid]" ) {

  using RadVec = std::vector<double>;
  auto ellipsoid_tests = {
    // Tuple vars are: ( tracer, radii, name )
    std::make_tuple( std::string("sphere"),    RadVec({ 0.283065 }), std::string("test_sphere") ),
    std::make_tuple( std::string("spheroid"),  RadVec({ 0.283065, 0.249720 }), std::string("test_spheroid") ),
    std::make_tuple( std::string("ellipsoid"), RadVec({ 0.283065, 0.271215, 0.249720 }), std::string("test_ellipsoid") )
  };

  // Setup constant translations for efficieny
  psmrts::PsmrtsTranslations trans_t = psmrts::PsmrtsTranslations::create();
  psmrts::ProductProcessing processor_p(trans_t);

  for ( const auto &[ tracer_t, radii_t, model_t ] : ellipsoid_tests ) {

    SECTION("Ellipsoid Maker Tracer " + tracer_t + " for Model " + model_t ) {
      psmrts::ProductConfiguration ellipsoid_t("ellipsoidmaker");
      ellipsoid_t.add( psmrts::ProductOption( "tracer", tracer_t  ) );
      ellipsoid_t.add( psmrts::ProductOption( "radii", radii_t ) );
      ellipsoid_t.add( psmrts::ProductOption( "name",  model_t )  );

      CHECK( ellipsoid_t.contains( "tracer" ) == true );
      CHECK( ellipsoid_t.contains( "radii" ) == true );
      CHECK( ellipsoid_t.contains( "name" ) == true );

      psmrts::ProductOrder order_t = processor_p.process_configuration( ellipsoid_t );
      CHECK( order_t.error_count() == 0 );
      psmrts::ProductMaker<psmrts::PsmrtsTracer> maker_t( tracer_t );

      CHECK( maker_t.process_config( order_t.config(), trans_t ) == true );
      CHECK( maker_t.isvalid() == true );
      psmrts::PsmrtsTracer ellipsoid_m = maker_t.product();
      CHECK( ellipsoid_m.isValid() == true );
    }
  }

}


TEST_CASE( "ProductMaker Shape Specifications Test", "[product][maker][shape][specs]" ) {
  auto specs_v = psmrts::ProductMaker<psmrts::PsmrtsShape>( "shapes" ).get_product_specs();
  CHECK( specs_v.size() == 4 );
}

TEST_CASE( "ProductMaker Tracer Specifications Test", "[product][maker][tracer][specs]" ) {
  auto specs_v = psmrts::ProductMaker<psmrts::PsmrtsTracer>( "shapes" ).get_product_specs();
  CHECK( specs_v.size() == 3 );
}
