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
  CHECK( uid1 == psmrts::PsmrtsUID::null_uid() );
  CHECK( uid2 == psmrts::PsmrtsUID::null_uid() );
  
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

TEST_CASE( "ProductInventory Case Insensitve Inventory", "[product][inventory][caseinsensitive]") {

  psmrts::CaseSensitivyKeyMap<std::string> nocase = psmrts::create_case_insensitive_inventory<std::string>( "isis" );

  CHECK( nocase.name() == "isis" );
  CHECK( nocase.type() == "inventory" );

  nocase.add("OsirisRex", "$ISISDATA/osirisrex");

  CHECK( nocase.contains( "osirisrex" ) == true );
  CHECK( nocase.contains( "OSIRISREX" ) == true );
  CHECK( nocase.find( "oSiriSreX" )     == "$ISISDATA/osirisrex" );

}

TEST_CASE( "ProductInventory Case Insensitve Inventory", "[product][inventory][casesensitive]") {

  psmrts::CaseSensitivyKeyMap<std::string> case_s = psmrts::create_case_sensitive_inventory<std::string>( "env" );
  CHECK( case_s.name() == "env" );
  CHECK( case_s.type() == "inventory" );

  case_s.add("ISISDATA", "/opt/isis3/data/osirisrex");

  CHECK( case_s.contains( "ISISDATA" ) == true );
  CHECK( case_s.contains( "isisdata" ) == false );

  CHECK( case_s.find( "ISISDATA" )     == "/opt/isis3/data/osirisrex" );
}
