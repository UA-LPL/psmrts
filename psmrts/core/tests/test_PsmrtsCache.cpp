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

    cache.add("test", 1);
    cache.add("test2", 10);
    cache.add("test2", 20);
    CHECK( cache.find("test2") == 20 );
    CHECK( cache.size() == 2 );

    REQUIRE( cache.contains("DNE") == false );
    CHECK_THROWS(cache.find("DNE"));

    psmrts::PsmrtsCache<std::string, int> copy(cache);
    CHECK( copy.size() == cache.size() );
    CHECK( copy.contains( "test" ) == true ); 

    cache.clear();
    CHECK( cache.size() == 0 );

}
