#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/ProductInventory.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>

TEST_CASE( "ProductInventory Default Constructor", "[product][inventory][default]") {
   
    psmrts::ProductInventory<psmrts::PsmrtsProduct::UIDType, psmrts::PsmrtsShape, psmrts::noop_key_id<psmrts::PsmrtsProduct::UIDType>> inv;

    psmrts::PsmrtsShape shape1;
    psmrts::PsmrtsShape shape2;

    auto uid1 = shape1.uid();
    auto uid2 = shape2.uid();

    REQUIRE( uid1 != uid2 );

    inv.add_product(shape1);
    inv.add_product(shape2);

    CHECK( inv.size() == 2);
    CHECK( inv.contains(uid1) == true );
    CHECK( inv.contains(uid2) == true );

    psmrts::PsmrtsShape uid_check = inv.find_by_uid(uid1);
    CHECK( uid_check.uid() == uid1 ); 

    inv.remove(uid1);
    CHECK( inv.contains(uid1) == false );
}
