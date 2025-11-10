#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsInventory.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>


TEST_CASE( "PSMRTS Inventory Default", "[product][inventory][default]") {

  psmrts::PsmrtsInventory inventory;

  // Comment out this test as its OS dependent. The sizes of the variants
  // differ per OS, but is useful to track the size as we copy many of them
  // rather than use pointers/virtual classes.
  // CHECK( sizeof( psmrts::PsmrtsInventory ) == 1144 );

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

  CHECK( inventory.parameters().size() == 0 );

  psmrts::PsmrtsInventory inv2;
  psmrts::ProductOption param("param", "example");

  CHECK( inv2.parameters().add("param", param) == true );
  CHECK( inv2.parameters().size() == 1 );
  CHECK( inv2.parameters().contains("param") == true );

  auto param_fetch = inv2.parameters().find("param");
  CHECK( param_fetch.name() == "param" );
  CHECK( param_fetch.type() == psmrts::ProductOption::DataEnums::PsmrtsString );

  CHECK( inventory.merge(inv2) == 1 );
  CHECK( inventory.parameters().size() == 1 );
  CHECK( inventory.parameters().contains("param") == true );
}

/**
TEST_CASE( "PSMRTS Inventory Environment", "[product][inventory][env]") {

  psmrts::PsmrtsInventory::EnvInventory env_t = psmrts::PsmrtsInventory::getenv( "keywords" );
  CHECK( env_t.size( ) > 0 );
  // CHECK( env_t.contains( "PATH" ) == true );
  // CHECK( env_t.cache().values() ==  std::vector<std::string>( { } ));

}
*/

TEST_CASE("PSMRTS Inventory Environment", "[product][inventory][env]") {
  psmrts::PsmrtsInventory inventory;

  // Check environment is initially empty
  CHECK(inventory.env().size() == 0);

  // Load environment variables once
  const auto& env_ref = inventory.load_and_merge_env();
  CHECK(env_ref.size() > 0); 

  // Check that common environment variables exist
  // These checks are conditional because CI/test environments may vary
  if (env_ref.contains("PATH")) {
    CHECK(env_ref.find("PATH").empty() == false);
  }

#ifdef _WIN32
  if (env_ref.contains("USERNAME")) {
    CHECK(env_ref.find("USERNAME").empty() == false);
  }
#else
  if (env_ref.contains("USER")) {
    CHECK(env_ref.find("USER").empty() == false);
  }
#endif

  // Store current size after first load
  const size_t size_after_first_merge = inventory.env().size();

  // Call load_and_merge_env again (should NOT duplicate entries)
  inventory.load_and_merge_env();
  CHECK(inventory.env().size() == size_after_first_merge);

  // Check access to a known key
  for (const auto& key : inventory.env().cache().keys()) {
    CHECK_NOTHROW(inventory.env().find(key));
  }

  // Remove one environment variable
  std::string first_key = inventory.env().cache().keys().front();
  inventory.remove_env(first_key);
  CHECK(inventory.env().contains(first_key) == false);

  // Re-add the removed variable manually
  inventory.env().add(first_key, "manually_added_value");
  CHECK(inventory.env().contains(first_key));
  CHECK(inventory.env().find(first_key) == "manually_added_value");

  // Test static getenv() call independently
  auto static_env = psmrts::PsmrtsInventory::getenv("testenv");
  CHECK(static_env.size() > 0);
  CHECK(static_env.type() == "env");
  CHECK(static_env.name() == "testenv");
}

