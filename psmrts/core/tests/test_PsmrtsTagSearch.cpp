#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsTagSearch.hpp>

TEST_CASE("PsmrtsTagSearch Default Values", "[tag][search][default]") {
    // --- orx_kernels.mk ---
    psmrts::PsmrtsTagSearch parser;
    parser.add_search_tag("PATH_VALUES",    ")");
    parser.add_search_tag("PATH_SYMBOLS",   ")");
    parser.add_search_tag("KERNELS_TO_LOAD", ")");
    parser.parse_file( psmrts_core_path("/tests/data/orx_kernels.mk") );

    // size
    CHECK( parser.size() == 3 );

    // contains
    CHECK( parser.contains("PATH_VALUES")    );
    CHECK( parser.contains("PATH_SYMBOLS")   );
    CHECK( parser.contains("KERNELS_TO_LOAD") );
    CHECK_FALSE( parser.contains("BOGUS")    );

    // get_by_start_tag returns correct count
    CHECK( parser.get_by_start_tag("PATH_VALUES").size()    == 1 );
    CHECK( parser.get_by_start_tag("KERNELS_TO_LOAD").size() == 1 );

    // PATH_VALUES raw body contains expected paths
    CHECK( parser.get_value("PATH_VALUES").find("/opt/isis3/data/osirisrex/apophis") != std::string::npos );
    CHECK( parser.get_value("PATH_VALUES").find("/opt/isis3/data/osirisrex")         != std::string::npos );
    CHECK( parser.get_value("PATH_VALUES").find("/opt/isis3/data'")                  != std::string::npos );

    // PATH_SYMBOLS raw body contains expected symbols
    CHECK( parser.get_value("PATH_SYMBOLS").find("data")      != std::string::npos );
    CHECK( parser.get_value("PATH_SYMBOLS").find("osirisrex") != std::string::npos );
    CHECK( parser.get_value("PATH_SYMBOLS").find("apophis")   != std::string::npos );

    // KERNELS_TO_LOAD raw body contains expected kernels
    CHECK( parser.get_value("KERNELS_TO_LOAD").find("naif0012.tls")        != std::string::npos );
    CHECK( parser.get_value("KERNELS_TO_LOAD").find("orx_struct_v04.bsp")  != std::string::npos );
    CHECK( parser.get_value("KERNELS_TO_LOAD").find("de440s.bsp")          == std::string::npos ); // commented out

    // --- IsisPreferences: extract DataDirectory block ---
    psmrts::PsmrtsTagSearch parser2;
    parser2.add_search_tag("DataDirectory", "EndGroup");
    parser2.parse_file( psmrts_core_path("/tests/data/IsisPreferences") );

    CHECK( parser2.size()                        == 1 );
    CHECK( parser2.contains("DataDirectory")     );
    CHECK_FALSE( parser2.contains("UserInterface") ); // not searched for

    // spot-check a few missions are present in the block
    CHECK( parser2.get_value("DataDirectory").find("Apollo15")    != std::string::npos );
    CHECK( parser2.get_value("DataDirectory").find("Lro")         != std::string::npos );
    CHECK( parser2.get_value("DataDirectory").find("Voyager2")    != std::string::npos );
    CHECK( parser2.get_value("DataDirectory").find("Temporary")   != std::string::npos );
    CHECK( parser2.get_value("DataDirectory").find("$ISISDATA")   != std::string::npos );

    // --- Parse individual lines out of the DataDirectory block ---
    psmrts::PsmrtsTagSearch parser3;
    parser3.add_search_tag("Galileo",    "\n");
    parser3.add_search_tag("Lro",        "\n");
    parser3.add_search_tag("Temporary",  "\n");
    parser3.parse_string( parser2.get_value("DataDirectory") );

    CHECK( parser3.size() == 3 );
    CHECK( parser3.get_value("Galileo")   == "= $ISISDATA/galileo"  );
    CHECK( parser3.get_value("Lro")       == "= $ISISDATA/lro"      );
    CHECK( parser3.get_value("Temporary") == "= ."  );
}
