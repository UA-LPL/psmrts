#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/shapes/obj/ObjShape.hpp>
#include <psmrts/shapes/obj/private/PsmrtsOBJFormat.hpp>
#include <psmrts/core/ProductSpecification.hpp>


TEST_CASE( "OBJ SHAPE - Default Test", "[obj][shape][specification]") {
  psmrts::ProductSpecification spec = psmrts::ObjShape::product_specifications();

  CHECK( spec.name()              == "obj"   );
  CHECK( spec.product()           == "shape" );
  CHECK( spec.size()              == 4 );
  CHECK( spec.features().size() == 4 );
  CHECK( spec.required().size()   == 1 );
  CHECK( spec.optional().size()   == 3 );
  CHECK( spec.contains( "obj_mtl_search_path" ) == true );
}

TEST_CASE( "PSMRTS Product OBJ Specification Test", "[product][type][mesh][obj]") {
  double tolerance = 1.0e-6;

  std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
  psmrts::ObjShape obj_m( objfile );
  
  CHECK( obj_m.name() == objfile );
  CHECK( obj_m.type() == "obj" );
  CHECK( psmrts::PsmrtsUID::is_valid_uid( obj_m.uid() ) );
    
  psmrts::PsmrtsMeshData mesh_d = obj_m.get_mesh( );
  CHECK( mesh_d.nfacets()        == 36 );
  CHECK( mesh_d.nvectors()       == 20 );
  CHECK( mesh_d.vector_type()    == psmrts::PsmrtsMeshData::PsmrtsDouble );
  CHECK( mesh_d.isVectorDouble() == true );

  CHECK_THAT( mesh_d.mesh_surface_area(),
              Catch::Matchers::WithinAbs( 0.842492, tolerance ) );
  CHECK_THAT( mesh_d.mesh_volume(),
              Catch::Matchers::WithinAbs( 0.063170, tolerance ) );
}
