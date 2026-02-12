#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/ProductSpecification.hpp>
#include <psmrts/core/ProductOrder.hpp>
#include <psmrts/core/ProductMaker.hpp>

#include <tuple>

TEST_CASE( "ProductMaker OBJ Shape Test", "[product][maker][shape][obj]" ) {

  psmrts::PsmrtsTranslations trans_t = psmrts::PsmrtsTranslations::create();
  psmrts::ProductConfiguration shape_p("objmaker");
  shape_p.add( psmrts::ProductOption( "shape", "obj" ) );
  shape_p.add( psmrts::ProductOption( "obj_file", psmrts_shapes_path( "obj/data/bennu_20facets.obj") ) );
  CHECK( shape_p.contains( "obj_file" ) == true );

  // CHECK( shape_p.to_json().dump(2) == "" );
  psmrts::ProductMaker<psmrts::PsmrtsShape> maker_s( "obj" );

  CHECK( maker_s.process_config( shape_p, trans_t ) == true );
  CHECK( maker_s.isvalid() == true );
  psmrts::PsmrtsShape shape_m = maker_s.product();
  CHECK( shape_m.isValid() == true );
 //  CHECK( shape_m.config().to_json().dump() == "" );

}


TEST_CASE( "ProductMaker PLY Shape Test", "[product][maker][shape][ply]" ) {

  psmrts::PsmrtsTranslations trans_t = psmrts::PsmrtsTranslations::create();
  psmrts::ProductConfiguration shape_p("plymaker");
  shape_p.add( psmrts::ProductOption( "shape", "ply" ) );
  shape_p.add( psmrts::ProductOption( "ply_file", psmrts_shapes_path( "ply/data/Bennu_Radar.ply" ) ) );
  CHECK( shape_p.contains( "ply_file" ) == true );

  // CHECK( shape_p.to_json().dump(2) == "" );
  psmrts::ProductMaker<psmrts::PsmrtsShape> maker_s( "ply" );

  CHECK( maker_s.process_config( shape_p, trans_t ) == true );
  CHECK( maker_s.isvalid() == true );
  psmrts::PsmrtsShape shape_m = maker_s.product();
  CHECK( shape_m.isValid() == true );
 //  CHECK( shape_m.config().to_json().dump() == "" );

}

TEST_CASE( "ProductMaker Bullet Tracer Test", "[product][maker][tracer][bullet]" ) {

  psmrts::PsmrtsTranslations trans_t = psmrts::PsmrtsTranslations::create();
  psmrts::ProductConfiguration bullet_t("bulletmaker");
  // bullet_t.add( psmrts::ProductOption( "shape", "obj" ) );
  bullet_t.add( psmrts::ProductOption( "obj_file", psmrts_shapes_path( "obj/data/bennu_20facets.obj")  ) );
  bullet_t.add( psmrts::ProductOption( "tracer", "bullet" ) );

  CHECK( bullet_t.contains( "obj_file" ) == true );

  // CHECK( bullet_t.to_json().dump(2) == "" );
  psmrts::ProductMaker<psmrts::PsmrtsTracer> maker_t( "bullet" );

  CHECK( maker_t.process_config( bullet_t, trans_t ) == true );
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
}
#endif

TEST_CASE( "ProductMaker NaifDsk Tracer Test", "[product][maker][tracer][naifdsk]" ) {

  psmrts::PsmrtsTranslations trans_t = psmrts::PsmrtsTranslations::create();
  psmrts::ProductConfiguration naifdsk_t("naifdskmaker");
  naifdsk_t.add( psmrts::ProductOption( "tracer", "naifdsk" ) );
  naifdsk_t.add( psmrts::ProductOption( "dsk_file", psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds")  ) );

  CHECK( naifdsk_t.contains( "dsk_file" ) == true );

  // CHECK( naifdsk_t.to_json().dump(2) == "" );
  psmrts::ProductMaker<psmrts::PsmrtsTracer> maker_t( "naifdsk" );

  CHECK( maker_t.process_config( naifdsk_t, trans_t ) == true );
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

  for ( const auto &[ tracer_t, radii_t, model_t ] : ellipsoid_tests ) {

    SECTION("Ellipsoid Maker Tracer " + tracer_t + " for Model " + model_t ) {
      psmrts::ProductConfiguration ellipsoid_t("ellipsoidmaker");
      ellipsoid_t.add( psmrts::ProductOption( "tracer", tracer_t  ) );
      ellipsoid_t.add( psmrts::ProductOption( "radii", radii_t ) );
      ellipsoid_t.add( psmrts::ProductOption( "name",  model_t )  );

      CHECK( ellipsoid_t.contains( "tracer" ) == true );
      CHECK( ellipsoid_t.contains( "radii" ) == true );
      CHECK( ellipsoid_t.contains( "name" ) == true );

      // CHECK( ellipsoid_t.to_json().dump(2) == "" );
      psmrts::ProductMaker<psmrts::PsmrtsTracer> maker_t( tracer_t );

      CHECK( maker_t.process_config( ellipsoid_t, trans_t ) == true );
      CHECK( maker_t.isvalid() == true );
      psmrts::PsmrtsTracer ellipsoid_m = maker_t.product();
      CHECK( ellipsoid_m.isValid() == true );
      // CHECK( ellipsoid_m.config().to_json().dump() == "" );
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
