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

    cache.add("test3", 3);
    cache.add("test4", 4);

    std::vector<std::string> keys = cache.keys();
    CHECK( keys.size() == 4 );
    CHECK( keys[0] == "test" );
    CHECK( keys[1] == "test2" );
    CHECK( keys[2] == "test3" );
    CHECK( keys[3] == "test4" );

    std::vector<int> values = cache.values();
    CHECK( values.size() == 4 );
    CHECK( values[0] == 1 );
    CHECK( values[1] == 20 );
    CHECK( values[2] == 3 );
    CHECK( values[3] == 4 ); 

    std::vector<std::pair<std::string, int>> iterated;
    for (auto it = cache.begin(); it != cache.end(); ++it) {
        iterated.emplace_back(it->first, it->second);
    }

#if 0    
    REQUIRE(iterated.size() == 4);
    CHECK(iterated[0] == std::make_pair("test", 1));
    CHECK(iterated[1] == std::make_pair("test2", 20));
    CHECK(iterated[2] == std::make_pair("test3", 3));
    CHECK(iterated[3] == std::make_pair("test4", 4));
#endif
    // Modify values through iterator
    for (auto it = cache.begin(); it != cache.end(); ++it) {
        it->second += 10;
    }

    CHECK(cache.find("test") == 11);
    CHECK(cache.find("test2") == 30);
    CHECK(cache.find("test3") == 13);
    CHECK(cache.find("test4") == 14);


    const auto& const_cache = cache;
    std::vector<std::pair<std::string, int>> const_iterated;
    for (auto it = const_cache.begin(); it != const_cache.end(); ++it) {
        const_iterated.emplace_back(it->first, it->second);
    }

#if 0    

    REQUIRE(const_iterated.size() == 4);
    CHECK(const_iterated[0] == std::make_pair("test", 11));
    CHECK(const_iterated[1] == std::make_pair("test2", 30));
    CHECK(const_iterated[2] == std::make_pair("test3", 13));
    CHECK(const_iterated[3] == std::make_pair("test4", 14));
#endif

    std::vector<std::string> iter_keys;
    std::transform(cache.begin(), cache.end(), std::back_inserter(iter_keys),
        [](const auto& kv) { return kv.first; });

    CHECK(iter_keys == std::vector<std::string>{"test", "test2", "test3", "test4"});
    

    REQUIRE( cache.contains("DNE") == false );
    CHECK_THROWS(cache.find("DNE"));

    psmrts::PsmrtsCache<std::string, int> copy(cache);
    CHECK( copy.size() == cache.size() );
    CHECK( copy.contains( "test" ) == true ); 

    cache.clear();
    CHECK( cache.size() == 0 );
}
