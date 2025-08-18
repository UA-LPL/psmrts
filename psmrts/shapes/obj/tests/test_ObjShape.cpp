#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/shapes/obj/ObjShape.hpp>
#include <psmrts/shapes/obj/private/PsmrtsOBJFormat.hpp>
#include <psmrts/core/ProductSpecification.hpp>


TEST_CASE( "OBJ SHAPE - Default Test", "[obj][shape][specification]") {
    psmrts::ProductSpecification spec = psmrts::ObjShape::product_specifications();

    CHECK( spec.name()              == "obj"   );
    CHECK( spec.product()           == "shape" ); 
    CHECK( spec.type()              == "mesh"  );
    CHECK( spec.driver().name()     == "obj" ); 
    CHECK( spec.size()              == 4 );
    CHECK( spec.parameters().size() == 4 );
    CHECK( spec.required().size()   == 1 );
    CHECK( spec.optional().size()   == 3 );
    CHECK( spec.has_parameter( "obj_mtl_search_path" ) == true );
}
