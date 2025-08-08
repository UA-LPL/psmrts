#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/shapes/ply/PlyShape.hpp>
#include <PsmrtsPLYFormat.hpp>
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
