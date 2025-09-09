#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsFactory.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>

TEST_CASE( "PSMRTS Factory System", "[factory][default]") {

  psmrts::PsmrtsFactory factory;
  CHECK( factory.size()               == 0 );
  CHECK( factory.contains( "psmrts" ) == false );

  auto uid = factory.add_product( psmrts::PsmrtsTracer::sphere( 200.0, "sphere" ) );
  CHECK( factory.size()               == 1 );
  CHECK( factory.contains( "psmrts" ) == true );
  CHECK( factory.find( "psmrts" ).tracers().size() == 1 );
  CHECK( factory.find( "psmrts" ).tracers().contains( uid ) == true );

  factory.liquidate();
  CHECK( factory.size()               == 0 );
  CHECK( factory.contains( "psmrts" ) == false );

}

TEST_CASE( "PSMRTS Factory System Values Test", "[factory][values][tracers]") {

  psmrts::PsmrtsFactory factory;

  std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
  auto uid1 = factory.add_product( psmrts::PsmrtsTracer::bullet( objfile ), "OBJ" );
  CHECK( factory.size() == 1 );
  CHECK( factory.find( "OBJ" ).tracers().size() == 1 );
  CHECK( factory.contains( "psmrts" ) == false );
  CHECK( factory.contains( "obj" ) == true );
  CHECK( factory.contains( "OBJ" ) == true );

  std::string dskfile = psmrts_shapes_path( "dsk/data/bennu_20facets.bds" );
  psmrts::DskShape dsk( dskfile );
  auto uid2 = factory.add_product( psmrts::PsmrtsShape( dsk ), "DSK");
  CHECK( factory.size() == 2 );
  CHECK( factory.contains( "dsk" ) == true );

  psmrts::PsmrtsFactory factory2;
  // Now check/compare factory with factory2 - they should have identical content!
  
  psmrts::PsmrtsTracer sphere = psmrts::PsmrtsTracer::sphere( 200.0, "sphere" );
  psmrts::PsmrtsPriorityTracer p_tracer( sphere );
  auto uid3 = factory2.add_product( p_tracer, "tracearray" );
  CHECK( factory2.size() == 3 ); // 2?
  CHECK( factory2.contains( "tracearray" ) == true );

  psmrts::PsmrtsInventory inventory;
  factory2.add(inventory); //should this function also return associated uid?
  CHECK( factory2.size() == 4 ); // 4?
  CHECK( factory2.contains( "psmrts" ) == true );

  // DON'T FORGET THIS AFTER TESTING PsmrtsFactory
  factory.liquidate();

}
