#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsFactory.hpp>


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
