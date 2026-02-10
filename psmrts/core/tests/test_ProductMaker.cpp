#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/ProductSpecification.hpp>
#include <psmrts/core/ProductOrder.hpp>
#include <psmrts/core/ProductMaker.hpp>

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

