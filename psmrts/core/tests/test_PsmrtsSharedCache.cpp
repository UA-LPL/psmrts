#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsSharedCache.hpp>

TEST_CASE( "PSMRTS Shared Cache Default Test", "[cache][default]") {

    /** Generic cache extractor */
    auto cache_extractor = []( auto &keys, auto &values, const auto &cache ) {
      auto cache_iterator = [&] ( const auto &map_c ) -> bool {
        for ( const auto &[ key, value ] : map_c ) {
          keys.push_back( key );
          values.push_back( value );
        }
        return ( true );
      };

      // Run the extraction
      cache.process( cache_iterator );
      return;
    };

    psmrts::PsmrtsSharedCache<std::string, int> cache;
    using SharedInt = psmrts::PsmrtsSharedCache<std::string, int>::SharedType;

    CHECK( cache.size() == 0 );

    cache.add("test", 1);
    CHECK( cache.size() == 1 );
    CHECK( cache.contains( "test" ) == true );
    CHECK( cache.contains( "psmrts" ) == false );
    CHECK( *cache.find( "test" ) == 1 );

    cache.remove("test");
    CHECK( cache.size() == 0 );

    cache.add("test", 1);
    cache.add("test2", 10);
    cache.add("test2", 20);
    CHECK( *cache.find("test2") == 20 );
    CHECK( cache.size() == 2 );

    cache.add("test3", 3);
    cache.add("test4", 4);

    std::vector<std::string> keys = cache.keys();
    CHECK( keys.size() == 4 );
    CHECK( keys[0] == "test" );
    CHECK( keys[1] == "test2" );
    CHECK( keys[2] == "test3" );
    CHECK( keys[3] == "test4" );

    std::vector<SharedInt> values = cache.values();
    CHECK( values.size() == 4 );
    CHECK( *values[0] == 1 );
    CHECK( *values[1] == 20 );
    CHECK( *values[2] == 3 );
    CHECK( *values[3] == 4 ); 

    std::vector<std::string> i_keys;
    std::vector<SharedInt> i_values;
#if 0
    for (auto i = cache.begin(); i != cache.end(); ++i ) {
        i_keys.push_back(i->first);
        i_values.push_back(i->second);
    }
#endif
    cache_extractor(i_keys, i_values, cache );   
    CHECK( i_keys == std::vector<std::string>{"test", "test2", "test3", "test4"} );
    // CHECK( i_values == std::vector<int>{1, 20, 3, 4} );
    
    CHECK( *cache.find("test") == 1);
    CHECK( *cache.find("test2") == 20);
    CHECK( *cache.find("test3") == 3);
    CHECK( *cache.find("test4") == 4);

    const auto& const_cache = cache;
    std::vector<std::string> c_keys;
    std::vector<SharedInt> c_values;
    cache_extractor(c_keys, c_values, const_cache );   
#if 0
    for (auto j = const_cache.begin(); j != const_cache.end(); ++j ) {
        c_keys.push_back(j->first);
        c_values.push_back(j->second);
    }
#endif
    CHECK( c_keys == std::vector<std::string>{"test", "test2", "test3", "test4"} );
    // CHECK( c_values == std::vector<int>{1, 20, 3, 4} );

    std::vector<std::string> iter_keys;
    auto cache_transformer = [&] ( const auto &map_c ) -> bool {
      std::transform(map_c.begin(), map_c.end(), std::back_inserter(iter_keys),
          [](const auto& kv) { return kv.first; });
      return ( true );
    };
    cache.process( cache_transformer );


    CHECK(iter_keys == std::vector<std::string>{"test", "test2", "test3", "test4"});
    

    REQUIRE( cache.contains("DNE") == false );
    CHECK_THROWS(cache.find("DNE"));

    psmrts::PsmrtsSharedCache<std::string, int> copy(cache);
    CHECK( copy.size() == cache.size() );
    CHECK( copy.contains( "test" ) == true ); 

    cache.clear();
    CHECK( cache.size() == 0 );
}
