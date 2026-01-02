#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include "../PlyShape.hpp"
#include <psmrts/core/ProductSpecification.hpp>

TEST_CASE( "PLY SHAPE - Default Test", "[ply][shape][specification]") {
    psmrts::ProductSpecification spec = psmrts::PlyShape::product_specifications();

    CHECK( spec.name()              == "ply"   );
    CHECK( spec.product()           == "shape" ); 
    CHECK( spec.type()              == "mesh"  );
    CHECK( spec.driver().name()     == "ply" ); 
    CHECK( spec.size()              == 2 );
    CHECK( spec.parameters().size() == 2 );
    CHECK( spec.required().size()   == 0 );
    CHECK( spec.optional().size()   == 2 );
    
    CHECK( spec.has_parameter( "obj_mtl_search_path" ) == false );
    CHECK( spec.has_parameter( "ply_file" )          == true );
    CHECK( spec.has_parameter( "ply_data_type" )       == true );
}

TEST_CASE( "PSMRTS Product PLY Specification Test", "[product][type][mesh][ply]") {
    std::string plyfile = psmrts_shapes_path( "ply/data/Bennu_Radar.ply"  );
    psmrts::PlyShape ply_m( plyfile );
  
    CHECK( ply_m.name() == plyfile );
    CHECK( ply_m.type() == "ply" );
    CHECK( psmrts::PsmrtsUID::is_valid_uid( ply_m.uid() ) );
    
    psmrts::PsmrtsMeshData mesh_d = ply_m.get_mesh( );
    CHECK( mesh_d.nfacets()        == 2692 );
    CHECK( mesh_d.nvectors()       == 1348 );
    CHECK( mesh_d.vector_type()    == psmrts::PsmrtsMeshData::PsmrtsDouble );
    CHECK( mesh_d.isVectorDouble() == true );
}
