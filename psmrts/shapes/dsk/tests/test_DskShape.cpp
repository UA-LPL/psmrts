#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/shapes/dsk/DskShape.hpp>
#include <psmrts/shapes/dsk/private/PsmrtsDSKFormat.hpp>
#include <psmrts/core/ProductSpecification.hpp>

TEST_CASE( "DSK SHAPE - Default Test", "[dsk][shape][specification]") {
    psmrts::ProductSpecification spec = psmrts::DskShape::product_specifications();

    CHECK( spec.name()              == "dsk"   );
    CHECK( spec.product()           == "shape" ); 
    CHECK( spec.type()              == "mesh"  );
    CHECK( spec.driver().name()     == "dsk" ); 
    CHECK( spec.size()              == 4 );
    CHECK( spec.parameters().size() == 4 );
    CHECK( spec.required().size()   == 0 );
    CHECK( spec.optional().size()   == 4 );

    CHECK( spec.has_parameter( "obj_mtl_search_path" ) == false );
    CHECK( spec.has_parameter( "dsk_string" )          == true );
    CHECK( spec.has_parameter( "dsk_data_type" )       == true );
    CHECK( spec.has_parameter( "dsk_body_id" )         == true );
    CHECK( spec.has_parameter( "dsk_segment_index" )   == true );

}


TEST_CASE( "PSMRTS Product DSK Specification Test", "[product][type][mesh][dsk]") {
    double tolerance = 1.0e-6;

    std::string dskfile = psmrts_shapes_path( "dsk/data/bennu_20facets.bds" );
    psmrts::DskShape dsk_m( dskfile );
  
    CHECK( dsk_m.name() == "dsk" );
    CHECK( dsk_m.type() == "shape" );
    CHECK( dsk_m.uid()  > psmrts::PsmrtsUID::UID_Reserved );
    
    psmrts::PsmrtsMeshData mesh_d = dsk_m.get_mesh( );
    CHECK( mesh_d.nfacets()        == 36 );
    CHECK( mesh_d.nvectors()       == 20 );
    CHECK( mesh_d.vector_type()    == psmrts::PsmrtsMeshData::PsmrtsDouble );
    CHECK( mesh_d.isVectorDouble() == true );

    CHECK_THAT( mesh_d.mesh_surface_area(),
                Catch::Matchers::WithinAbs( 0.842492, tolerance ) );
    CHECK_THAT( mesh_d.mesh_volume(),
                Catch::Matchers::WithinAbs( 0.063170, tolerance ) );
}
