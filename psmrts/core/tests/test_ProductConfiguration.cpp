#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/core/ProductConfiguration.hpp>


TEST_CASE( "PSMRTS Product Configuration", "[product][configuration][default]") {

  // Construct default product configuration.
  psmrts::ProductConfiguration config;
  CHECK( config.name()          == "undefined" );
  CHECK( config.size()          == 0 );
  // CHECK( config.remove("name")  == false);

  // Add a couple of product options.
  config.add( psmrts::ProductOption( "type", "product") );
  CHECK( config.size()          == 1 );

  config.add( psmrts::ProductOption( "name", "ply") );
  CHECK( config.size()          == 2 );

  // Check removal of each one.
  CHECK( config.remove("name")  == true );
  CHECK( config.size()          == 1 );


  CHECK( config.remove("type")  == true );
  CHECK( config.size()          == 0 );
}


TEST_CASE( "PSMRTS Product Initializer", "[product][configuration][initializer]") {

  // Accepts a list of two configuration options and run some tests.
  psmrts::ProductConfiguration config( "multi", { psmrts::ProductOption( "tracer", "bullet"), 
                                                  psmrts::ProductOption("obj_file", "l_00050mm_alt_ptm_5595n04217_v020.obj") 
                                                } );
  CHECK( config.name()           == "multi" );
  CHECK( config.size()           == 2 );

  // Should not see any non-existant products.
  CHECK( config.contains("name") == false);
  CHECK( config.remove("name")   == false );
  CHECK( config.size()           == 2 );

  // Checks mixed case strings.
  CHECK( config.contains("tracer") == true );
  CHECK( config.contains("TrAcEr") == false );
  CHECK_THROWS( config.find( "Tracer" ) );

  // Remove the tracer type and confirm.
  CHECK( config.remove("tracer")  == true );
  CHECK( config.size()            == 1 );
  CHECK_THROWS( config.find( "tracer" ) );

  // Create a single configuration.
  psmrts::ProductConfiguration config2("single", { psmrts::ProductOption("shape", "ply") });
  CHECK( config.compare( config2 ) == false );

  // "single" still remains after common 
  psmrts::ProductConfiguration comp_check = config.difference( config2 );
  CHECK( comp_check.name() == "single" );
  CHECK( comp_check.size() == 1 );

  const bool TwoWay = true;
  psmrts::ProductConfiguration comp_check_twoway = config.difference( config2, TwoWay );
  CHECK( comp_check_twoway.name() == "single" );
  CHECK( comp_check_twoway.size() == 2 );

  
  // Add second option, remove "shape" and check difference.
  config2.add( psmrts::ProductOption( "obj_file", "l_00050mm_alt_ptm_5595n04217_v020.obj" ) );
  config2.remove("single");
  psmrts::ProductConfiguration comp_check2 = config.difference( config2 );
  
  // This results in no options remaining.
  CHECK( comp_check2.name() == "single" );
  CHECK( comp_check2.size() == 0 );
  CHECK( config.contains( "obj_file" ) == true );
  CHECK( config2.contains( "obj_file" ) == true );

}
