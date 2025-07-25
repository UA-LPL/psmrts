#include <psmrts_catch2_environment.hpp>

#include <DskShape.hpp>
#include <PsmrtsDSKFormat.hpp>
#include <ProductSpecification.hpp>

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
