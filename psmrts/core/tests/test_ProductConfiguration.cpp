#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/core/ProductConfiguration.hpp>


TEST_CASE( "PSMRTS Product Configuration", "[product][configuration][default]") {

  psmrts::ProductConfiguration config;
  CHECK( config.name()          == "none" );
  CHECK( config.size()          == 0 );
  CHECK( config.remove("name")  == false);

  config.add( psmrts::ProductOption( "type", "product") );
  CHECK( config.size()          == 1 );

  config.add( psmrts::ProductOption( "name", "ply") );
  CHECK( config.size()          == 2 );

  CHECK( config.remove("name")  == true );
  CHECK( config.size()          == 1 );


  CHECK( config.remove("type")  == true );
  CHECK( config.size()          == 0 );
}


TEST_CASE( "PSMRTS Product Initializer", "[product][configuration][initializer]") {

  psmrts::ProductConfiguration config( "multi", { psmrts::ProductOption( "tracer", "bullet"), 
                                                  psmrts::ProductOption("obj_file", "l_00050mm_alt_ptm_5595n04217_v020.obj") 
                                                } );
  CHECK( config.name()           == "multi" );
  CHECK( config.size()           == 2 );

  CHECK( config.contains("name") == false);
  CHECK( config.remove("name")   == false );
  CHECK( config.size()           == 2 );

  CHECK( config.contains("tracer") == true );
  CHECK( config.contains("TrAcEr") == true );
  CHECK_NOTHROW( config.find( "Tracer" ) );

  CHECK( config.remove("tracer")  == true );
  CHECK( config.size()            == 1 );
  CHECK_THROWS( config.find( "tracer" ) );

  psmrts::ProductConfiguration config2("single", { psmrts::ProductOption("shape", "ply") });
  CHECK( config.compare( config2 ) == false );

  psmrts::ProductConfiguration comp_check = config.difference( config2 );
  CHECK( comp_check.name() == "" );
  CHECK( comp_check.size() == 1 );

  config2.add( psmrts::ProductOption( "obj_file", "l_00050mm_alt_ptm_5595n04217_v020.obj" ) );
  config2.remove("shape");
  psmrts::ProductConfiguration comp_check2 = config.difference( config2 );
  CHECK( comp_check2.name() == "" );
  CHECK( comp_check2.size() == 1 ); // ?
  CHECK( config.contains( "obj_file" ) == true );
  CHECK( config2.contains( "obj_file" ) == true );

}
