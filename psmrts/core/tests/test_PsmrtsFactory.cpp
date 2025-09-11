#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsFactory.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>

TEST_CASE( "PSMRTS Factory System", "[factory][default]") {

  psmrts::PsmrtsFactory factory;
  factory.liquidate();  // Start with empty factory

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
  factory.liquidate();  // Start with empty factory

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

  CHECK( factory.get_inventory_list() == std::vector<std::string>( { "dsk", "obj", "psmrts", "tracearray" } ) );

  // DON'T FORGET THIS AFTER TESTING EVERY PsmrtsFactory!!!
  factory.liquidate();

}

TEST_CASE( "PSMRTS Factory Docs Example", "[factory][docs][example]") {
  // This example is documented in the PsmrtsFactory code 

    // State of an empty factory.
   psmrts::PsmrtsFactory factory1;
  factory1.liquidate();  // Start with empty factory
  
   CHECK( factory1.size()               == 0 );
   CHECK( factory1.contains( "psmrts" ) == false );
   
   // Add a bullet tracer into unque inventory
   std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
   auto uid1 = factory1.add_product( psmrts::PsmrtsTracer::bullet( objfile ), "inv1" );
   CHECK( factory1.size() == 1 );
   CHECK( factory1.find( "inv1" ).tracers().size() == 1 );
   CHECK( factory1.contains( "psmrts" )            == false );
   CHECK( factory1.contains( "inv1" )              == true );
   CHECK( factory1.contains( "INV1" )              == true );
    
   // Instantiate second factory and compare its state to factory1.
   psmrts::PsmrtsFactory factory2;
   CHECK( factory2.size()                          == 1 );
   CHECK( factory2.size()                          == factory1.size() );
   CHECK( factory2.contains( "psmrts" )            == factory1.contains( "psmrts" ) );
   CHECK( factory2.contains( "inv1" )              == factory1.contains( "inv1" ) );
   CHECK( factory2.find( "inv1" ).tracers().size() == factory1.find( "inv1" ).tracers().size() );
   
   // Adding a new shape to a new inventory appears in both factory objects.
   std::string dskfile = psmrts_shapes_path( "dsk/data/bennu_20facets.bds" );
   psmrts::DskShape dsk( dskfile );
   auto uid2 = factory2.add_product( psmrts::PsmrtsShape( dsk ), "inv2");
   CHECK( factory2.size()                           == 2 );
   CHECK( factory2.size()                          == factory1.size() );
   CHECK( factory2.contains( "inv2" )              == true );
   CHECK( factory2.contains( "inv2" )              == factory1.contains( "inv2" ) );
   CHECK( factory2.find( "inv2" ).shapes().size()  == 1 );
   CHECK( factory2.find( "inv2" ).shapes().size()  == factory1.find( "inv2" ).shapes().size());
    
   // PSMRTS inventory cache content check
   CHECK( factory1.get_inventory_list() == std::vector<std::string>( { "inv1", "inv2" } ) );
   CHECK( factory2.get_inventory_list() == factory1.get_inventory_list() );
   
   // Removing a product in one factory affects all factory instances
   CHECK( factory2.find( "inv2" ).shapes().contains( uid2 ) == true );
   CHECK( factory1.find( "inv2" ).shapes().contains( uid2 ) == true );   
   CHECK_NOTHROW( factory2.find( "inv2" ).shapes().remove( uid2 ) );

   CHECK( factory2.find( "inv2" ).shapes().contains( uid2 ) == false );
   CHECK( factory1.find( "inv2" ).shapes().contains( uid2 ) == false );
   CHECK( factory2.find( "inv2" ).shapes().size()           == 0 );
   CHECK( factory2.find( "inv2" ).shapes().size()           == factory1.find( "inv2" ).shapes().size() );
   
   CHECK( factory1.size()   == 2 );
   CHECK( factory2.size()   == 2 );
   factory1.liquidate();
   CHECK( factory1.size()   == 0 );
   CHECK( factory2.size()   == 0 );
}
