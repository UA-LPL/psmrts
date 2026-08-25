#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsInventory.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>


TEST_CASE( "PSMRTS Inventory Default", "[product][inventory][default]") {

  psmrts::PsmrtsInventory inventory;

  // Comment out this test as its OS dependent. The sizes of the variants
  // differ per OS, but is useful to track the size as we copy many of them
  // rather than use pointers/virtual classes.
  CHECK( sizeof( psmrts::PsmrtsInventory ) < 100 );

  CHECK( inventory.name()                    == "inventory" );
  CHECK( inventory.shapes()->size()          == 0 );
  CHECK( inventory.shapes()->name()          == "inventory" );

  CHECK( inventory.tracers()->size()         == 0 );
  CHECK( inventory.tracers()->name()         == "inventory" );
}

TEST_CASE( "PSMRTS Inventory Basics", "[product][inventory][basics]") {
  
  psmrts::PsmrtsInventory inventory;
  CHECK( inventory.tracers()->keys().size() == 0 );

  psmrts::PsmrtsInventory::UIDType uid;
  CHECK_NOTHROW( uid = inventory.tracers()->add( psmrts::PsmrtsTracer::sphere( 10.0, "sphere") ) );
  CHECK( inventory.tracers()->size()        == 1 );
  CHECK( inventory.tracers()->keys().size() == 1 );
  CHECK( inventory.tracers()->keys()        == std::vector<psmrts::PsmrtsInventory::UIDType>( { uid } ) );

  psmrts::SharedTracer tracer_t;
  CHECK_NOTHROW( tracer_t = inventory.tracers()->find( uid ) );
  CHECK( tracer_t->uid()   == uid );
  CHECK( tracer_t->name()  == "sphere" );
  CHECK( inventory.tracers()->keys().size() == 1 );

  CHECK_NOTHROW( inventory.tracers()->remove( tracer_t->uid() ) );
  CHECK( inventory.tracers()->size() == 0 );
  CHECK( inventory.tracers()->keys().size() == 0 );

}
