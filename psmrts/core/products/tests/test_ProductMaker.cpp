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


#include <tuple>

TEST_CASE( "ProductMaker OBJ Shape Test", "[product][maker][shape][obj]" ) {

  psmrts::PsmrtsTranslations trans_t = psmrts::PsmrtsTranslations::create();
  psmrts::ProductProcessing processor_p(trans_t);

  psmrts::ProductConfiguration shape_p("objmaker");
  shape_p.add( psmrts::ProductOption( "shape", "obj" ) );
  shape_p.add( psmrts::ProductOption( "obj_file", psmrts_shapes_path( "obj/data/bennu_20facets.obj") ) );
  CHECK( shape_p.contains( "obj_file" ) == true );

  auto order_s = processor_p.process_order( shape_p );
  REQUIRE( order_s != nullptr );
  CHECK( order_s->isvalid()         == true );
  CHECK( order_s->find( "tracer")  == nullptr  );
  CHECK( order_s->find( "shape")   != nullptr  );
  CHECK( order_s->find("shape")->specification().name()   == "obj" );

  psmrts::ProductMaker<psmrts::PsmrtsShape> maker_s( "obj" );
  auto shape_t = maker_s.process_config( order_s->find( "shape")->configuration(), trans_t );

  CHECK( maker_s.product( ) != nullptr );
  CHECK( maker_s.isvalid()  == true );

  auto shape_m = maker_s.product();
  CHECK( shape_m       != nullptr );
  CHECK( shape_t.get() == shape_m.get() );
}


TEST_CASE( "ProductMaker PLY Shape Test", "[product][maker][shape][ply]" ) {

  psmrts::PsmrtsTranslations trans_t = psmrts::PsmrtsTranslations::create();
  psmrts::ProductProcessing processor_p(trans_t);

  psmrts::ProductConfiguration shape_p("plymaker");
  shape_p.add( psmrts::ProductOption( "shape", "ply" ) );
  shape_p.add( psmrts::ProductOption( "ply_file", psmrts_shapes_path( "ply/data/Bennu_Radar.ply" ) ) );
  CHECK( shape_p.contains( "ply_file" ) == true );

  auto order_s = processor_p.process_order( shape_p );
  REQUIRE( order_s != nullptr );
  CHECK( order_s->isvalid()         == true );
  CHECK( order_s->find( "tracer")  == nullptr  );
  CHECK( order_s->find( "shape")   != nullptr  );
  CHECK( order_s->find("shape")->specification().name() == "ply" );

  psmrts::ProductMaker<psmrts::PsmrtsShape> maker_s( "ply" );
  auto shape_t = maker_s.process_config( order_s->find("shape")->configuration(), trans_t );

  CHECK( maker_s.product( ) != nullptr );
  CHECK( maker_s.isvalid()  == true );

  auto shape_m = maker_s.product();
  CHECK( shape_m       != nullptr );
  CHECK( shape_t.get() == shape_m.get() );
}

TEST_CASE( "ProductMaker Bullet Tracer Test", "[product][maker][tracer][bullet]" ) {

  psmrts::PsmrtsTranslations trans_t = psmrts::PsmrtsTranslations::create();
  psmrts::ProductProcessing processor_p(trans_t);

  psmrts::ProductConfiguration bullet_t("bulletmaker");
  bullet_t.add( psmrts::ProductOption( "shape", "obj" ) );
  bullet_t.add( psmrts::ProductOption( "obj_file", psmrts_shapes_path( "obj/data/bennu_20facets.obj")  ) );
  bullet_t.add( psmrts::ProductOption( "tracer", "bullet" ) );

  CHECK( bullet_t.contains( "obj_file" ) == true );

  auto order_s = processor_p.process_order( bullet_t );
  REQUIRE( order_s != nullptr );
  CHECK( order_s->isvalid()         == true );
  REQUIRE( order_s->find( "tracer")  != nullptr  );
  REQUIRE( order_s->find( "shape")   != nullptr  );
  CHECK( order_s->find("tracer")->specification().name() == "bullet" );
  CHECK( order_s->find("shape")->specification().name()  == "obj" );

  psmrts::ProductMaker<psmrts::PsmrtsShape> maker_s( "obj" );
  auto shape_p = maker_s.process_cart( *order_s->find("shape") );
  REQUIRE( shape_p != nullptr );


  psmrts::ProductMaker<psmrts::PsmrtsTracer> maker_t( "bullet" );
  auto tracer_t = maker_t.process_cart( *order_s->find("tracer"), shape_p );
  CHECK( tracer_t != nullptr );
  CHECK( maker_t.product( ) != nullptr );
  CHECK( maker_t.isvalid()  == true );

  auto tracer_m = maker_t.product();
  CHECK( tracer_m       != nullptr );
  CHECK( tracer_m.get() == tracer_t.get() );
}


TEST_CASE( "ProductMaker NaifDsk Tracer Test", "[product][maker][tracer][naifdsk]" ) {

  psmrts::PsmrtsTranslations trans_t = psmrts::PsmrtsTranslations::create();
  psmrts::ProductProcessing processor_p(trans_t);

  psmrts::ProductConfiguration naifdsk_t("naifdskmaker");
  naifdsk_t.add( psmrts::ProductOption( "tracer", "naifdsk" ) );
  naifdsk_t.add( psmrts::ProductOption( "dsk_file", psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds")  ) );

  CHECK( naifdsk_t.contains( "tracer" ) == true );
  CHECK( naifdsk_t.contains( "dsk_file" ) == true );

  auto order_s = processor_p.process_order( naifdsk_t );
  REQUIRE( order_s != nullptr );
  CHECK( order_s->isvalid()         == true );
  CHECK( order_s->find( "tracer")  != nullptr  );
  CHECK( order_s->find( "shape")   == nullptr  );
  CHECK( order_s->find("tracer")->specification().name() == "naifdsk" );

  psmrts::ProductMaker<psmrts::PsmrtsTracer> maker_t( "naifdsk" );
  auto tracer_t = maker_t.process_config( order_s->find("tracer")->configuration(), trans_t );

  CHECK( maker_t.product( ) != nullptr );
  CHECK( maker_t.isvalid()  == true );

  auto tracer_m = maker_t.product();
  CHECK( tracer_m       != nullptr );
  CHECK( tracer_t.get() == tracer_m.get() );
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

      auto order_s = processor_p.process_order( ellipsoid_t );
      REQUIRE( order_s != nullptr );
      CHECK( order_s->isvalid()         == true );
      CHECK( order_s->find( "tracer")  != nullptr  );
      CHECK( order_s->find( "shape")   == nullptr  );
      CHECK( order_s->find("tracer")->specification().name() == "ellipsoid" );

      psmrts::ProductMaker<psmrts::PsmrtsTracer> maker_t( "ellipsoid" );
      auto tracer_t = maker_t.process_config( order_s->find("tracer")->configuration(), trans_t );

      CHECK( maker_t.product( ) != nullptr );
      CHECK( maker_t.isvalid()  == true );

      auto tracer_m = maker_t.product();
      CHECK( tracer_m       != nullptr );
      CHECK( tracer_t.get() == tracer_m.get() );
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
