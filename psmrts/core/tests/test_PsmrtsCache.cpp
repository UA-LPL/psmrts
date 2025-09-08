#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsCache.hpp>

TEST_CASE( "PSMRTS Cache Default Test", "[cache][default]") {
    psmrts::PsmrtsCache<std::string, int> cache;
    CHECK( cache.size() == 0 );

    cache.add("test", 1);
    CHECK( cache.size() == 1 );
    CHECK( cache.contains( "test" ) == true );
    CHECK( cache.contains( "psmrts" ) == false );
    CHECK( cache.find( "test" ) == 1 );

    cache.remove("test");
    CHECK( cache.size() == 0 );
}
