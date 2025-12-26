#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/ProductInventory.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>

TEST_CASE( "ProductInventory Default Constructor", "[product][inventory][default]") {
   
  // Creates a product inventory of PsmrtsShapes using the standard Psrmts::UIDType.
  psmrts::ProductInventory<psmrts::PsmrtsProduct::UIDType, psmrts::PsmrtsShape> inv;

  // Create two shapes and show they have undefined ids.
  psmrts::PsmrtsShape shape1;
  psmrts::PsmrtsShape shape2;

  auto uid1 = shape1.uid();
  auto uid2 = shape2.uid();
  CHECK( uid1 == psmrts::PsmrtsUID::null_id() );
  CHECK( uid2 == psmrts::PsmrtsUID::null_id() );
  
  REQUIRE( uid1 == uid2 );

  // Add each to inventory.
  inv.add_product(shape1);
  inv.add_product(shape2);

  // Show both are successfully added and retreivable.
  CHECK( inv.size() == 1);
  CHECK( inv.contains(uid1) == true );
  CHECK( inv.contains(uid2) == true );

  psmrts::PsmrtsShape uid_check = inv.find_by_uid(uid1);
  CHECK( uid_check.uid() == uid1 ); 

  // Verify removeable.
  inv.remove(uid1);
  CHECK( inv.contains(uid1) == false );
}
