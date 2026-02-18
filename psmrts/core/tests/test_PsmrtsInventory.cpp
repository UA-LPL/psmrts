#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsInventory.hpp>
#include <psmrts/core/products/PsmrtsProduct.hpp>


TEST_CASE( "PSMRTS Inventory Default", "[product][inventory][default]") {

  psmrts::PsmrtsInventory inventory;

  // Comment out this test as its OS dependent. The sizes of the variants
  // differ per OS, but is useful to track the size as we copy many of them
  // rather than use pointers/virtual classes.
  // CHECK( sizeof( psmrts::PsmrtsInventory ) == 1144 );

  CHECK( inventory.product().name() == "product" );
  CHECK( inventory.product().type() == "inventory" );
  CHECK( psmrts::PsmrtsUID::is_valid_uid( inventory.product().uid() ) );

  CHECK( inventory.shapes().size()          == 0 );
  CHECK( inventory.shapes().name()          == "product" );
  CHECK( inventory.shapes().type()          == "shapes" );

  CHECK( inventory.tracers().size()         == 0 );
  CHECK( inventory.tracers().name()         == "product" );
  CHECK( inventory.tracers().type()         == "tracers" );

  CHECK( inventory.prioritytracers().size() == 0 );
  CHECK( inventory.prioritytracers().name() == "product" );
  CHECK( inventory.prioritytracers().type() == "prioritytracers" );

  CHECK( inventory.parameters().size()      == 0 );
  CHECK( inventory.parameters().name()      == "parameters" );
  CHECK( inventory.parameters().type()      == "inventory" );

  CHECK( inventory.environment().size()             > 0 );
  CHECK( inventory.environment().name()             == "environment" );
  CHECK( inventory.environment().type()             == "inventory" );

}

TEST_CASE( "PSMRTS Inventory Basics", "[product][inventory][basics]") {

  psmrts::PsmrtsInventory inventory;
  CHECK( inventory.product().name() == "product" );
  CHECK( inventory.product().type() == "inventory" );
  CHECK( psmrts::PsmrtsUID::is_valid_uid( inventory.product().uid() ) );
  CHECK( inventory.tracers().cache().keys().size() == 0 );

  psmrts::PsmrtsInventory::TracerInventory::UIDType uid;
  CHECK_NOTHROW( uid = inventory.tracers().add_product( psmrts::PsmrtsTracer::sphere( 10.0, "sphere") ) );
  CHECK( inventory.tracers().size()                == 1 );
  CHECK( inventory.tracers().cache().keys().size() == 1 );
  CHECK( inventory.tracers().cache().keys() == std::vector<psmrts::PsmrtsInventory::UIDType>( { uid } ) );

  psmrts::PsmrtsTracer tracer_t;
  CHECK_NOTHROW( tracer_t = inventory.tracers().find_by_uid( uid ) );
  CHECK( tracer_t.uid()   == uid );
  CHECK( tracer_t.name()  == "sphere" );
  CHECK( tracer_t.type()  == "sphere" );
  CHECK( inventory.tracers().cache().keys().size() == 1 );

  CHECK_NOTHROW( inventory.tracers().remove( tracer_t.uid() ) );
  CHECK( inventory.tracers().size() == 0 );
  CHECK( inventory.tracers().cache().keys().size() == 0 );

  CHECK( inventory.parameters().size() == 0 );
  CHECK( inventory.environment().size() > 0 );

}
