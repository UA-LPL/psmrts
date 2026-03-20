#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/ISISDataDictionary.hpp>
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsTagSearch.hpp>
#include <psmrts/core/tests/TemporaryDirectoryFixture.hpp>

TEST_CASE( "ISISDataDictionary Default Test", "[isis][data][dictionary][default]") {
    psmrts::ISISDataDictionary dict( psmrts_core_path("tests/data/IsisPreferences") );

    // Block count
    CHECK( dict.size() == 8 );

    // has_group
    CHECK( dict.has_group("UserInterface")     );
    CHECK( dict.has_group("DataDirectory")     );
    CHECK( dict.has_group("ErrorFacility")     );
    CHECK( dict.has_group("SessionLog")        );
    CHECK( dict.has_group("CubeCustomization") );
    CHECK( dict.has_group("Performance")       );
    CHECK( dict.has_group("Plugins")           );
    CHECK_FALSE( dict.has_group("ShapeModel")  );   // commented-out in file

    // group() map sizes
    CHECK( dict.group("DataDirectory").size()     == 38 );
    CHECK( dict.group("UserInterface").size()     == 10 );
    CHECK( dict.group("ErrorFacility").size()     == 3  );
    CHECK( dict.group("SessionLog").size()        == 4  );
    CHECK( dict.group("CubeCustomization").size() == 4  );
    CHECK( dict.group("Performance").size()       == 2  );
    CHECK( dict.group("Plugins").size()           == 1  );

    // UserInterface values
    CHECK( dict.value("UserInterface", "ProgressBarPercent").value() == "1"         );
    CHECK( dict.value("UserInterface", "ProgressBar").value()        == "On"        );
    CHECK( dict.value("UserInterface", "GuiHelpBrowser").value()     == "open"      );
    CHECK( dict.value("UserInterface", "GuiFontName").value()        == "helvetica" );
    CHECK( dict.value("UserInterface", "GuiFontSize").value()        == "10"        );
    CHECK( dict.value("UserInterface", "GuiWidth").value()           == "460"       );
    CHECK( dict.value("UserInterface", "GuiHeight").value()          == "600"       );
    CHECK( dict.value("UserInterface", "HistoryRecording").value()   == "On"        );
    CHECK( dict.value("UserInterface", "HistoryLength").value()      == "10"        );
    CHECK( dict.value("UserInterface", "HistoryPath").value()        == "$HOME/.Isis/history");

    // ErrorFacility values
    CHECK( dict.value("ErrorFacility", "FileLine").value()   == "Off"      );
    CHECK( dict.value("ErrorFacility", "Format").value()     == "Standard" );
    CHECK( dict.value("ErrorFacility", "StackTrace").value() == "Off"      );

    // SessionLog values
    CHECK( dict.value("SessionLog", "TerminalOutput").value() == "Off"       );
    CHECK( dict.value("SessionLog", "FileOutput").value()     == "On"        );
    CHECK( dict.value("SessionLog", "FileName").value()       == "print.prt" );
    CHECK( dict.value("SessionLog", "FileAccess").value()     == "Append"    );

    // CubeCustomization values
    CHECK( dict.value("CubeCustomization", "Overwrite").value()   == "Allow"    );
    CHECK( dict.value("CubeCustomization", "Format").value()      == "Attached" );
    CHECK( dict.value("CubeCustomization", "History").value()     == "On"       );
    CHECK( dict.value("CubeCustomization", "MaximumSize").value() == "200"      );

    // Performance values
    CHECK( dict.value("Performance", "CubeWriteThread").value() == "Optimized" );
    CHECK( dict.value("Performance", "GlobalThreads").value()   == "Optimized" );

    // DataDirectory — spot-check several missions
    CHECK( dict.value("DataDirectory", "ISIS3DATA").value()  == "$ISISDATA"                      );
    CHECK( dict.value("DataDirectory", "Apollo15").value()   == "$ISISDATA/apollo15"             );
    CHECK( dict.value("DataDirectory", "Base").value()       == "$ISISDATA/base"                 );
    CHECK( dict.value("DataDirectory", "Lro").value()        == "$ISISDATA/lro"                  );
    CHECK( dict.value("DataDirectory", "Mro").value()        == "$ISISDATA/mro"                  );
    CHECK( dict.value("DataDirectory", "Clipper").value()    == "$ISISDATA/../datalocal/clipper" );
    CHECK( dict.value("DataDirectory", "Temporary").value()  == "."                              );

    // Case-insensitive group and key lookups
    CHECK( dict.value("datadirectory", "lro").value()         == "$ISISDATA/lro" );
    CHECK( dict.value("USERINTERFACE", "PROGRESSBAR").value() == "On"            );

    // value_or — key present returns stored value
    CHECK( dict.value_or("SessionLog",    "FileAccess", "Overwrite") == "Append"  );

    // value_or — key absent returns the default
    CHECK( dict.value_or("UserInterface", "GuiStyle",   "default")   == "default" );

    // Missing group / key returns nullopt
    CHECK_FALSE( dict.value("ShapeModel",    "RayTraceEngine").has_value() );
    CHECK_FALSE( dict.value("UserInterface", "NonExistentKey").has_value() );

    // Unknown group name returns empty map without crashing
    CHECK( dict.group("Bogus").empty() );

    // Multi-line continuation (CSMDirectory) is collapsed into one value
    CHECK( dict.value("Plugins", "CSMDirectory")->find("csmplugins") != std::string::npos );  
    CHECK( dict.value("Plugins", "CSMDirectory")->find("csm3.0.3")   != std::string::npos );

    // operator[] — first block in file order is UserInterface
    CHECK( dict[0].group_name == "UserInterface" );

    // all_blocks() count matches size()
    CHECK( dict.all_blocks().size() == dict.size() );

    // translations() is accessible and translate_path is callable without throwing
    CHECK_NOTHROW( dict.translations().translate_path("$ISISDATA/lro") );
}

TEST_CASE( "ISISDataDictionary to_string PVL Round-Trip", "[isis][data][dictionary][serialization][pvl]") {
    psmrts::ISISDataDictionary dict( psmrts_core_path("tests/data/IsisPreferences") );

    std::string pvl = dict.to_string();

    // Output is non-empty
    CHECK_FALSE( pvl.empty() );

    // Every group name appears as a Group = Name header
    CHECK( pvl.find("Group = UserInterface")    != std::string::npos );
    CHECK( pvl.find("Group = DataDirectory")    != std::string::npos );
    CHECK( pvl.find("Group = ErrorFacility")    != std::string::npos );
    CHECK( pvl.find("Group = SessionLog")       != std::string::npos );
    CHECK( pvl.find("Group = CubeCustomization")!= std::string::npos );
    CHECK( pvl.find("Group = Performance")      != std::string::npos );
    CHECK( pvl.find("Group = Plugins")          != std::string::npos );

    // Every opened Group has a matching EndGroup
    size_t group_count    = 0;
    size_t endgroup_count = 0;
    size_t pos = 0;
    while ((pos = pvl.find("Group =", pos)) != std::string::npos) { ++group_count;    pos += 7; }
    pos = 0;
    while ((pos = pvl.find("EndGroup",  pos)) != std::string::npos) { ++endgroup_count; pos += 8; }
    CHECK( group_count == endgroup_count );
    CHECK( group_count == dict.size() );

    // Key = Value pairs are present and indented
    CHECK( pvl.find("  ProgressBar = On")        != std::string::npos );
    CHECK( pvl.find("  FileAccess = Append")      != std::string::npos );
    CHECK( pvl.find("  Lro = $ISISDATA/lro")      != std::string::npos );
    CHECK( pvl.find("  CubeWriteThread = Optimized") != std::string::npos );

    // Round-trip: re-parse the serialized string and compare to original
    psmrts::ISISDataDictionary reparsed;
    reparsed.parse_string(pvl);

    CHECK( reparsed.size() == dict.size() );
    CHECK( reparsed.has_group("UserInterface") );
    CHECK( reparsed.has_group("DataDirectory") );

    CHECK( reparsed.value("UserInterface", "ProgressBar").value()   == "On"             );
    CHECK( reparsed.value("UserInterface", "GuiFontName").value()   == "helvetica"      );
    CHECK( reparsed.value("SessionLog",    "FileAccess").value()    == "Append"         );
    CHECK( reparsed.value("DataDirectory", "Lro").value()           == "$ISISDATA/lro"  );
    CHECK( reparsed.value("DataDirectory", "Mro").value()           == "$ISISDATA/mro"  );
    CHECK( reparsed.value("Performance",   "GlobalThreads").value() == "Optimized"      );

    // block_type is preserved through round-trip
    for (const auto& block : reparsed.all_blocks())
        CHECK( block.block_type == "Group" );

    TemporaryDirectoryFixture t_path;
    auto path = t_path.tmppath("output.prefs");
    CHECK_NOTHROW( dict.to_file( path ) );

}

TEST_CASE( "ISISDataDictionary to_string_flat", "[isis][data][dictionary][serialization][flat]") {
    psmrts::ISISDataDictionary dict( psmrts_core_path("tests/data/IsisPreferences") );

    std::string flat = dict.to_string_flat();

    // Output is non-empty
    CHECK_FALSE( flat.empty() );

    // Each block appears as a comment header (no Group = / EndGroup wrappers)
    CHECK( flat.find("# Group: UserInterface")     != std::string::npos );
    CHECK( flat.find("# Group: DataDirectory")     != std::string::npos );
    CHECK( flat.find("# Group: ErrorFacility")     != std::string::npos );
    CHECK( flat.find("# Group: SessionLog")        != std::string::npos );
    CHECK( flat.find("# Group: CubeCustomization") != std::string::npos );
    CHECK( flat.find("# Group: Performance")       != std::string::npos );
    CHECK( flat.find("# Group: Plugins")           != std::string::npos );

    // No ISIS PVL wrapper keywords present
    CHECK( flat.find("EndGroup")   == std::string::npos );
    CHECK( flat.find("End_Object") == std::string::npos );

    // Key = Value pairs are present without indentation
    CHECK( flat.find("ProgressBar = On")          != std::string::npos );
    CHECK( flat.find("FileAccess = Append")       != std::string::npos );
    CHECK( flat.find("Lro = $ISISDATA/lro")       != std::string::npos );
    CHECK( flat.find("GlobalThreads = Optimized") != std::string::npos );
    CHECK( flat.find("FileLine = Off")            != std::string::npos );

    // Flat output should NOT have leading spaces before keys (unlike PVL)
    CHECK( flat.find("  ProgressBar") == std::string::npos );
    CHECK( flat.find("  Lro")         == std::string::npos );

    // Round-trip is NOT expected to work (flat format has no Group wrappers),
    // but the content should be parseable as a flat key=value set
    CHECK( flat.find("ISIS3DATA = $ISISDATA")             != std::string::npos );
    CHECK( flat.find("HistoryPath = $HOME/.Isis/history") != std::string::npos );

    // Comment count matches block count
    size_t comment_count = 0;
    size_t pos = 0;
    while ((pos = flat.find("# Group:", pos)) != std::string::npos) { ++comment_count; pos += 8; }
    CHECK( comment_count == dict.size() );
}

TEST_CASE( "ISISDataDictionary Translations Integration", "[isis][translations]" ) {
    // ingest() calls add_parameter() for each key/value found in parsing
    psmrts::ISISDataDictionary dict( psmrts_core_path("tests/data/IsisPreferences") );
 
    // Parsed keys are mirrored into the parameter set
    // Every key that appears in the parsed blocks must be findable as a
    // parameter in the translation table.
    const psmrts::PsmrtsTranslations& tln = dict.translations();
 
    CHECK( tln.parameters().contains( "ISIS3DATA" )       == true );
    CHECK( tln.parameters().contains( "Base" )            == true );
    CHECK( tln.parameters().contains( "Lro")              == true );
    CHECK( tln.parameters().contains( "Mro" )             == true );
    CHECK( tln.parameters().contains( "ProgressBar" )     == true );
    CHECK( tln.parameters().contains( "GuiFontSize" )     == true );
    CHECK( tln.parameters().contains( "TerminalOutput")   == true );
    CHECK( tln.parameters().contains( "FileAccess" )      == true );
    CHECK( tln.parameters().contains( "Overwrite" )       == true );
    CHECK( tln.parameters().contains( "CubeWriteThread" ) == true );
    CHECK( tln.parameters().contains( "GlobalThreads" )   == true );
    CHECK( tln.parameters().contains( "CSMDirectory" )    == true );
 
    // The stored value must match what value() returns from the block.
    CHECK( tln.parameters().find( "ISIS3DATA"     ) == "$ISISDATA"     );
    CHECK( tln.parameters().find( "Lro"           ) == "$ISISDATA/lro" );
    CHECK( tln.parameters().find( "ProgressBar"   ) == "On"            );
    CHECK( tln.parameters().find( "FileAccess"    ) == "Append"        );
    CHECK( tln.parameters().find( "GlobalThreads" ) == "Optimized"     );
 
    // translate_path() strips $, looks up result in parameters()/environment()
    // and substitutes the value in place.
    // $ISISDATA should be found in parameters with value /isis/data, producing
    // /isis/data/lro.
    // (Inject a concrete root so $ISISDATA is resolvable without a real env var.)
    dict.translations().add_parameter( "ISISDATA", "/isis/data" );
 
    std::string lro_raw = dict.value("DataDirectory", "Lro").value();
    CHECK( lro_raw == "$ISISDATA/lro" );
    CHECK( dict.translations().translate_path( lro_raw ) == "/isis/data/lro" );
    CHECK( dict.translations().translate_path(
               dict.value("DataDirectory", "Mro").value() ) == "/isis/data/mro" );
 
    // A literal path (no '$') must pass through unchanged.
    CHECK( dict.translations().translate_path( "/no/variable/here" ) == "/no/variable/here" );
 
    // An empty string must be returned as-is.
    CHECK( dict.translations().translate_path( "" ) == "" );
 
    // Multi-level chained $-variable resolution
    // The file defines ISIS3DATA = $ISISDATA, so translate_path() should
    // chase: $ISIS3DATA -> $ISISDATA -> /isis/data  (two iterations).
    CHECK( dict.translations().translate_path( "$ISIS3DATA" ) == "/isis/data" );
 
    // Three levels: a new alias pointing at ISIS3DATA.
    dict.translations().add_parameter( "MYDATA", "$ISIS3DATA" );
    CHECK( dict.translations().translate_path( "$MYDATA/base" ) == "/isis/data/base" );
 
    // Caller-supplied PsmrtsTranslations is preserved alongside parsed entries
    psmrts::PsmrtsTranslations pre_trans( "pre_seeded" );
    pre_trans.add_parameter( "ISISDATA", "/pre/seeded/isis" );
    pre_trans.add_parameter( "CUSTOM",   "/custom/path"     );

    psmrts::ISISDataDictionary seeded_dict( psmrts_core_path("tests/data/IsisPreferences"), std::move( pre_trans ) );

    // Pre-seeded variables must survive alongside the parsed ones.
    CHECK( seeded_dict.translations().parameters().contains( "ISISDATA" ) );
    CHECK( seeded_dict.translations().parameters().contains( "CUSTOM"   ) );
    CHECK( seeded_dict.translations().parameters().find( "ISISDATA" ) == "/pre/seeded/isis" );
    CHECK( seeded_dict.translations().parameters().find( "CUSTOM"   ) == "/custom/path"     );

    // Parsed variables must also be present.
    CHECK( seeded_dict.translations().parameters().contains( "Lro"         ) );
    CHECK( seeded_dict.translations().parameters().contains( "ProgressBar" ) );

    // Translation with the pre-seeded root must work immediately.
    CHECK( seeded_dict.translations().translate_path(
           seeded_dict.value("DataDirectory", "Lro").value() ) == "/pre/seeded/isis/lro" );


    // Real shell environment loaded via PsmrtsTranslations::create()
    psmrts::ISISDataDictionary env_dict( psmrts_core_path("tests/data/IsisPreferences"), 
                                         psmrts::PsmrtsTranslations::create() );

    const std::string raw = env_dict.value("UserInterface", "HistoryPath").value();
    CHECK( raw == "$HOME/.Isis/history" );

    std::string resolved = env_dict.translations().translate_path( raw );

    #if defined(WIN32) || defined(_MSC_VER) || defined(__CYGWIN__)
        // $HOME is not a standard Windows environment variable — expect no resolution.
        CHECK( resolved == raw );
    #else
        if ( resolved.find('$') == std::string::npos ) {
            // $HOME was set — resolved path must be an absolute POSIX path.
            CHECK( resolved[0] == '/' );
            CHECK( resolved.find("/.Isis/history") != std::string::npos );
        } else {
            // $HOME not set in this environment — input returned unchanged.
            CHECK( resolved == raw );
        }
    #endif
 
    // Post-construction parameter and environment injection via non-const accessor
    // Before injection, $ISISDATA resolves to the value added earlier in this
    // test; remove it first so we can test the unresolved case cleanly.
    dict.translations().remove_parameter( "ISISDATA" );
    CHECK( dict.translations().translate_path( lro_raw ) == "$ISISDATA/lro" );
 
    // Inject a new concrete path and verify resolution updates.
    dict.translations().add_parameter( "ISISDATA", "/injected" );
    CHECK( dict.translations().translate_path( lro_raw ) == "/injected/lro" );
 
    // environment() is a separate container from parameters().
    dict.translations().add_environment( "MY_ENV_VAR", "/env/value" );
    CHECK( dict.translations().environment().contains( "MY_ENV_VAR" ) );
    CHECK( dict.translations().translate_path( "$MY_ENV_VAR/sub" ) == "/env/value/sub" );
 
    // clear() resets both the block list and the translation table
    dict.clear();
 
    CHECK( dict.size() == 0 );
 
    CHECK_FALSE( dict.translations().parameters().contains("Lro")      );
    CHECK_FALSE( dict.translations().parameters().contains("ISISDATA")  );
    CHECK_FALSE( dict.translations().parameters().contains("ISIS3DATA") );
 
    // translate_path must return input unchanged with an empty table.
    CHECK( dict.translations().translate_path("$ISISDATA/base") == "$ISISDATA/base" );
 
    // Parameter lookup and translate_path are case-insensitive
    // Re-parse to restore a populated dictionary.
    dict.parse_file( psmrts_core_path("tests/data/IsisPreferences") );
    dict.translations().add_parameter( "ISISDATA", "/isis/data" );
 
    const psmrts::PsmrtsTranslations& trans2 = dict.translations();
 
    // The key "Lro" was added by ingest(); all case variants must match.
    CHECK( trans2.parameters().contains( "lro" ) );
    CHECK( trans2.parameters().contains( "LRO" ) );
    CHECK( trans2.parameters().contains( "Lro" ) );
 
    CHECK( trans2.parameters().find( "lro" ) == "$ISISDATA/lro" );
    CHECK( trans2.parameters().find( "LRO" ) == "$ISISDATA/lro" );
 
    // translate_path must resolve regardless of the case used in the path string.
    CHECK( trans2.translate_path("$lro")        == "/isis/data/lro" );
    CHECK( trans2.translate_path("$LRO")        == "/isis/data/lro" );
    CHECK( trans2.translate_path("$isisdata/x") == "/isis/data/x"  );
    CHECK( trans2.translate_path("$ISISDATA/x") == "/isis/data/x"  );
}
