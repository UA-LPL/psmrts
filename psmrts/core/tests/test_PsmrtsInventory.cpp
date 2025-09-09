#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsInventory.hpp>


TEST_CASE( "PSMRTS Inventory Default", "[product][inventory][default]") {

  psmrts::PsmrtsInventory inventory;
  CHECK( inventory.product().name() == "product" );
  CHECK( inventory.product().type() == "inventory" );
  CHECK( inventory.product().uid() != psmrts::PsmrtsUID::UID_Reserved );

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
  CHECK( inventory.parameters().name()      == "product" );
  CHECK( inventory.parameters().type()      == "parameters" );

  CHECK( inventory.env().size()             == 0 );
  CHECK( inventory.env().name()             == "product" );
  CHECK( inventory.env().type()             == "env" );

}

TEST_CASE( "PSMRTS Inventory Basics", "[product][inventory][basics]") {

  psmrts::PsmrtsInventory inventory;
  CHECK( inventory.product().name() == "product" );
  CHECK( inventory.product().type() == "inventory" );
  CHECK( inventory.product().uid() != psmrts::PsmrtsUID::UID_Reserved );
  CHECK( inventory.tracers().cache().keys().size() == 0 );

  psmrts::PsmrtsInventory::TracerInventory::UIDType uid;
  CHECK_NOTHROW( uid = inventory.tracers().add_product( psmrts::PsmrtsTracer::sphere( 10.0, "sphere") ) );
  CHECK( inventory.tracers().size()                == 1 );
  CHECK( inventory.tracers().cache().keys().size() == 1 );
  CHECK( inventory.tracers().cache().keys() == std::vector<psmrts::PsmrtsInventory::UIDType>( { uid } ) );

  psmrts::PsmrtsTracer tracer_t;
  CHECK_NOTHROW( tracer_t = inventory.tracers().find_by_uid( uid ) );
  CHECK( tracer_t.product().uid()  == uid );
  CHECK( tracer_t.name()           == "sphere" );
  CHECK( tracer_t.product().name() == "sphere" );
  CHECK( tracer_t.product().type() == "tracer" );
  CHECK( inventory.tracers().cache().keys().size() == 1 );

  CHECK_NOTHROW( inventory.tracers().remove( tracer_t.product().uid() ) );
  CHECK( inventory.tracers().size() == 0 );
  CHECK( inventory.tracers().cache().keys().size() == 0 );

}

TEST_CASE( "PSMRTS Inventory Environment", "[product][inventory][env]") {

  psmrts::PsmrtsInventory::EnvInventory env_t = psmrts::PsmrtsInventory::getenv( "keywords" );
  CHECK( env_t.size( ) > 0 );
  CHECK( env_t.contains( "PATH" ) == true );

}
