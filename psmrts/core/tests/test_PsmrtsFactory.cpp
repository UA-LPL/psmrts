#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsFactory.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>

TEST_CASE( "PSMRTS Factory System", "[factory][default]") {

  psmrts::PsmrtsFactory factory;
  factory.liquidate();  // Start with empty factory

  auto uid = factory.add( psmrts::PsmrtsTracer::sphere( 200.0, "sphere" ) );
  CHECK( factory.shape_count()               == 1 );
  CHECK( factory.tracer_count() == 1 );
  CHECK( factory.contains_tracer( uid ) == true );

  factory.liquidate();
  CHECK( factory.shape_count()               == 1 );
  CHECK( factory.tracer_count()               == 1 );
  psmrts::PsmrtsFactory().liquidate();

}

TEST_CASE( "PSMRTS Factory System Values Test", "[factory][values][tracers]") {

  psmrts::PsmrtsFactory factory;
  factory.liquidate();  // liquidate and start with empty psmrts factory

  std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
  auto uid1 = factory.add( psmrts::PsmrtsTracer::bullet( objfile ) );
  CHECK( factory.tracer_count() == 2 );

  std::string dskfile = psmrts_shapes_path( "dsk/data/bennu_20facets.bds" );
  psmrts::DskShape dsk( dskfile );
  auto uid2 = factory.add( psmrts::PsmrtsShape( dsk ) );
  CHECK( factory.shape_count() == 3 );

  psmrts::PsmrtsFactory factory2;
  // Now check/compare factory with factory2 - they should have identical content!
  CHECK( factory.shape_count() == factory2.shape_count() );
  CHECK( factory.tracer_count() == factory2.tracer_count() );

  psmrts::PsmrtsTracer sphere = psmrts::PsmrtsTracer::sphere( 200.0, "sphere" );
  auto uid3 = factory2.add( sphere );
  CHECK( factory2.shape_count() == 4 ); 

  // DON'T FORGET THIS AFTER TESTING EVERY PsmrtsFactory!!!
  factory.liquidate();

}

TEST_CASE( "PSMRTS Factory Docs Example", "[factory][docs][example]") {
  // This example is documented in the PsmrtsFactory code 

    // State of an empty factory.
   psmrts::PsmrtsFactory factory1;
  factory1.liquidate();  // Start with empty factory
  
   // Add a bullet tracer into unque inventory
   std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
   auto uid1 = factory1.add( psmrts::PsmrtsTracer::bullet( objfile ) );
   CHECK( factory1.tracer_count() == 1 );
   CHECK( factory1.shape_count() == 1 );
    
   // Instantiate second factory and compare its state to factory1.
   psmrts::PsmrtsFactory factory2;
   CHECK( factory2.tracer_count()                          == 2 );
   CHECK( factory2.tracer_count()                          == factory1.tracer_count() );
   
   // Adding a new shape to a new inventory appears in both factory objects.
   std::string dskfile = psmrts_shapes_path( "dsk/data/bennu_20facets.bds" );
   psmrts::DskShape dsk( dskfile );
   auto uid2 = factory2.add( psmrts::PsmrtsShape( dsk ) );
   CHECK( factory2.shape_count()  == 3 );
   CHECK( factory2.shape_count()  == factory1.shape_count() );
   CHECK( factory2.shape_count()  == 1 );
    
   // PSMRTS inventory cache content check
   
   // Removing a product in one factory affects all factory instances
   CHECK( factory2.contains_shape( uid2 ) == true );
   CHECK( factory1.contains_shape( uid2 ) == true );   
   CHECK_NOTHROW( factory2.remove_shape( uid2 ) );

   CHECK( factory2.contains_shape( uid2 ) == false );
   CHECK( factory1.contains_shape( uid2 ) == false );
   CHECK( factory2.shape_count()           == 0 );
   CHECK( factory2.shape_count()           == factory1.shape_count() );
   
   CHECK( factory1.shape_count()   == 3 );
   CHECK( factory2.shape_count()   == 3 );
   factory1.liquidate();
   CHECK( factory1.shape_count()   == 0 );
   CHECK( factory2.tracer_count()   == 0 );
  psmrts::PsmrtsFactory().liquidate();

}
