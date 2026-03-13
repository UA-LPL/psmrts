#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/ISISDataDictionary.hpp>
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsTagSearch.hpp>

TEST_CASE( "ISISDataDictionary Default Test", "[isis][data][dictionary][default]") {
    psmrts::ISISDataDictionary dict("../psmrts/core/tests/data/IsisPreferences");

    // Block count
    CHECK( dict.size() == 8 );

    // has_group
    CHECK( dict.has_group("UserInterface") );
    CHECK( dict.has_group("DataDirectory") );
    CHECK( dict.has_group("ErrorFacility") );
    CHECK( dict.has_group("SessionLog") );
    CHECK( dict.has_group("CubeCustomization") );
    CHECK( dict.has_group("Performance") );
    CHECK( dict.has_group("Plugins") );
    CHECK_FALSE( dict.has_group("ShapeModel") );   // commented-out in file

    // group() map sizes
    CHECK( dict.group("DataDirectory").size() == 38 );
    CHECK( dict.group("UserInterface").size()  == 10  );
    CHECK( dict.group("ErrorFacility").size()  == 3  );
    CHECK( dict.group("SessionLog").size()     == 4  );
    CHECK( dict.group("CubeCustomization").size() == 4 );
    CHECK( dict.group("Performance").size()    == 2  );
    CHECK( dict.group("Plugins").size()        == 1  );

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
    CHECK( dict.value("DataDirectory", "ISIS3DATA").value()  == "$ISISDATA"                     );
    CHECK( dict.value("DataDirectory", "Apollo15").value()   == "$ISISDATA/apollo15"             );
    CHECK( dict.value("DataDirectory", "Base").value()       == "$ISISDATA/base"                 );
    CHECK( dict.value("DataDirectory", "Lro").value()        == "$ISISDATA/lro"                  );
    CHECK( dict.value("DataDirectory", "Mro").value()        == "$ISISDATA/mro"                  );
    CHECK( dict.value("DataDirectory", "Clipper").value()    == "$ISISDATA/../datalocal/clipper"  );
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
    CHECK( dict.value("Plugins", "CSMDirectory")->find("csmplugins") != std::string::npos );   CHECK( dict.value("Plugins", "CSMDirectory")->find("csm3.0.3")   != std::string::npos );

    // operator[] — first block in file order is UserInterface
    CHECK( dict[0].group_name == "UserInterface" );

    // all_blocks() count matches size()
    CHECK( dict.all_blocks().size() == dict.size() );

    // translations() is accessible and translate_path is callable without throwing
    CHECK_NOTHROW( dict.translations().translate_path("$ISISDATA/lro") );
}

TEST_CASE( "ISISDataDictionary to_string PVL Round-Trip", "[isis][data][dictionary][serialization][pvl]") {
    psmrts::ISISDataDictionary dict("../psmrts/core/tests/data/IsisPreferences");

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

    CHECK( reparsed.value("UserInterface", "ProgressBar").value()   == "On"            );
    CHECK( reparsed.value("UserInterface", "GuiFontName").value()   == "helvetica"      );
    CHECK( reparsed.value("SessionLog",    "FileAccess").value()    == "Append"         );
    CHECK( reparsed.value("DataDirectory", "Lro").value()           == "$ISISDATA/lro"  );
    CHECK( reparsed.value("DataDirectory", "Mro").value()           == "$ISISDATA/mro"  );
    CHECK( reparsed.value("Performance",   "GlobalThreads").value() == "Optimized"      );

    // block_type is preserved through round-trip
    for (const auto& block : reparsed.all_blocks())
        CHECK( block.block_type == "Group" );

    CHECK_NOTHROW( dict.to_file("../psmrts/core/tests/data/output.prefs") );
}

TEST_CASE( "ISISDataDictionary to_string_flat", "[isis][data][dictionary][serialization][flat]") {
    psmrts::ISISDataDictionary dict("../psmrts/core/tests/data/IsisPreferences");

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
    CHECK( flat.find("ProgressBar = On")         != std::string::npos );
    CHECK( flat.find("FileAccess = Append")       != std::string::npos );
    CHECK( flat.find("Lro = $ISISDATA/lro")       != std::string::npos );
    CHECK( flat.find("GlobalThreads = Optimized") != std::string::npos );
    CHECK( flat.find("FileLine = Off")            != std::string::npos );

    // Flat output should NOT have leading spaces before keys (unlike PVL)
    CHECK( flat.find("  ProgressBar") == std::string::npos );
    CHECK( flat.find("  Lro")         == std::string::npos );

    // Round-trip is NOT expected to work (flat format has no Group wrappers),
    // but the content should be parseable as a flat key=value set
    CHECK( flat.find("ISIS3DATA = $ISISDATA")          != std::string::npos );
    CHECK( flat.find("HistoryPath = $HOME/.Isis/history") != std::string::npos );

    // Comment count matches block count
    size_t comment_count = 0;
    size_t pos = 0;
    while ((pos = flat.find("# Group:", pos)) != std::string::npos) { ++comment_count; pos += 8; }
    CHECK( comment_count == dict.size() );
}